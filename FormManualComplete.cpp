#include <vcl.h>
#pragma hdrstop
#include <IniFiles.hpp>
#include "FormBase.h"
#include "FormManualComplete.h"
#pragma package(smart_init)
#pragma resource "*.dfm"
TManualCompleteForm *ManualCompleteForm;
static UnicodeString RecoveryText(const char *key) { return BaseForm->GetLangStr(key); }
// [수동삽입 완료] 두 가지 사용 방식
// 1) 자동작업 정보 있음: 삽입 확인 -> 트레이 기록 저장 -> CellTrackOut 보고/초기화
//    -> 재개 버튼 -> 대기위치 이동 확인 -> 다음 셀 작업 준비/재개.
// 2) 자동작업 정보 없음(manualEntry): 채널 직접 입력 -> 수신 원본의 셀 정보 조회
//    -> CellTrackOut 보고/초기화 -> 닫기. 로컬 트레이 기록/자동 시퀀스는 변경하지 않는다.
// phase는 처리 단계, polling은 그 단계를 타이머가 진행 중인지 나타낸다.
// 오류 시 polling만 끄고 phase/복구파일을 남겨 이미 성공한 보고를 중복 전송하지 않는다.
__fastcall TManualCompleteForm::TManualCompleteForm(TComponent *Owner) : TForm(Owner)
{
    phase = mcIdle;
    toolNo = sourceNo = targetNo = reservedNo = 0;
    polling = restored = workFlag = false;
    requestClearQueued = false;
    journalOkay = true;
    contextReady = false;
    manualEntry = false;
    started = 0;
    LoadJournal();
}
UnicodeString TManualCompleteForm::JournalPath() const
{
    return ExtractFilePath(Application->ExeName) + L"ManualCellCompletion.ini";
}
void TManualCompleteForm::LoadJournal()
{
    if(!FileExists(JournalPath())) return;
    try{
        TMemIniFile *ini = new TMemIniFile(JournalPath());
        try{
            int value = ini->ReadInteger("Recovery", "Phase", -1);
            if(value < mcIdle || value > mcMoving) throw Exception("Invalid manual recovery journal");
            phase = (TManualCellPhase)value;
            manualEntry = ini->ReadBool("Recovery", "ManualEntry", false);
            toolNo = ini->ReadInteger("Recovery", "Tool", 0);
            sourceNo = ini->ReadInteger("Recovery", "SourceNo", 0);
            targetNo = ini->ReadInteger("Recovery", "TargetNo", 0);
            reservedNo = ini->ReadInteger("Recovery", "ReservedNo", 0);
            sourceId = ini->ReadString("Recovery", "SourceId", "");
            targetId = ini->ReadString("Recovery", "TargetId", "");
            cellId = ini->ReadString("Recovery", "CellId", "");
            lotId = ini->ReadString("Recovery", "LotId", "");
            ngCode = ini->ReadString("Recovery", "NGCode", "");
            grade = ini->ReadString("Recovery", "Grade", "");
            workFlag = ini->ReadBool("Recovery", "WorkFlag", false);
            // 이전 실행의 미완료 건은 표시만 복원한다. 자동 시퀀스까지 복원된 것은 아니다.
            // restored가 true이면 Retry/Resume으로 임의 재개하지 못하도록 막는다.
            restored = IsBlocking();
        }__finally{delete ini;}
    }catch(Exception &){journalOkay = false;}
}
bool TManualCompleteForm::SaveJournal()
{
    // Journal = 재실행 시 중복 보고를 막기 위한 복구 기록(일반 운전 로그와 별개).
    // 삽입 확인은 트레이 기록 변경/최초 보고보다 먼저, 성공 응답은 Request OFF보다
    // 먼저 저장한다. 도중에 종료되어도 어느 셀을 어디까지 처리했는지 남기기 위함이다.
    try{
        UnicodeString temp = JournalPath() + L".pending";
        TMemIniFile *ini = new TMemIniFile(temp);
        try{
            ini->Clear();
            ini->WriteInteger("Recovery", "Phase", phase);
            ini->WriteBool("Recovery", "ManualEntry", manualEntry);
            ini->WriteInteger("Recovery", "Tool", toolNo);
            ini->WriteInteger("Recovery", "SourceNo", sourceNo);
            ini->WriteInteger("Recovery", "TargetNo", targetNo);
            ini->WriteInteger("Recovery", "ReservedNo", reservedNo);
            ini->WriteString("Recovery", "SourceId", sourceId);
            ini->WriteString("Recovery", "TargetId", targetId);
            ini->WriteString("Recovery", "CellId", cellId);
            ini->WriteString("Recovery", "LotId", lotId);
            ini->WriteString("Recovery", "NGCode", ngCode);
            ini->WriteString("Recovery", "Grade", grade);
            ini->WriteBool("Recovery", "WorkFlag", workFlag);
            ini->UpdateFile();
        }__finally{delete ini;}
        // 임시파일을 디스크에 기록한 후 본 파일로 교체하여 저장 중 기존 기록 손상을 줄인다.
        HANDLE h = CreateFileW(temp.c_str(), GENERIC_WRITE, FILE_SHARE_READ,
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if(h == INVALID_HANDLE_VALUE) RaiseLastOSError();
        bool flushed = FlushFileBuffers(h) != 0;
        CloseHandle(h);
        if(!flushed || !MoveFileExW(temp.c_str(), JournalPath().c_str(),
            MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) RaiseLastOSError();
        journalOkay = true;
        return true;
    }catch(Exception &e){
        journalOkay = false;
        Fail(RecoveryText("MSG_MC_SAVE_FAILED") + L"\r\n" + e.Message);
        return false;
    }
}
void TManualCompleteForm::ApplyLanguage()
{
    Caption = RecoveryText("CAP_MANUAL_COMPLETE");
    pnlTitle->Caption = Caption;
    lblSource->Caption = RecoveryText("CAP_SOURCE_TRAY");
    lblTarget->Caption = RecoveryText("CAP_TARGET_TRAY");
    lblSourceChannel->Caption = RecoveryText("CAP_SOURCE_CHANNEL");
    lblChannel->Caption = RecoveryText("CAP_TARGET_CHANNEL");
    chkInserted->Caption = RecoveryText("CAP_MC_CONFIRM");
    btnReport->Caption = RecoveryText("CAP_MC_REPORT");
    btnRetry->Caption = RecoveryText("CAP_RETRY");
    btnResume->Caption = RecoveryText("CAP_MC_RESUME");
    btnClose->Caption = RecoveryText("CAP_CLOSE");
    RefreshFmsWaitLabel();
}
void TManualCompleteForm::RefreshFmsWaitLabel()
{
    // 오류 설명과 FMS 대기 단계를 분리 표시. 성공 응답만으로 완료가 아니며,
    // Request OFF 전송 및 Response=0 초기화까지 끝나야 보고 완료로 표시한다.
    lblFmsState->Visible = IsBlocking();
    if(!lblFmsState->Visible) return;
    const char *key = "MSG_MC_FMS_PREPARING";
    lblFmsState->Font->Color = (TColor)0x00006090;
    if(phase >= mcReady){
        key = "MSG_MC_FMS_COMPLETE";
        lblFmsState->Font->Color = journalOkay ? clGreen : clMaroon;
    }else if(!polling || restored || !journalOkay){
        key = "MSG_MC_FMS_PENDING";
        lblFmsState->Font->Color = clMaroon;
    }else if(phase == mcClearForSend){
        key = "MSG_MC_FMS_CLEAR";
    }else if(phase == mcWaitResult){
        key = "MSG_MC_FMS_RESPONSE";
    }else if(phase == mcAccepted){
        key = "MSG_MC_FMS_RESET";
    }
    lblFmsState->Caption = RecoveryText(key);
}
void TManualCompleteForm::RefreshControls()
{
    // 채널 직접 입력은 보고 시작 전만 허용. 시작 후에는 보고 대상이 바뀌지 않도록 잠근다.
    bool editable = manualEntry && phase == mcIdle && journalOkay;
    editSource->Text = sourceId;
    editTarget->Text = targetId;
    if(!editable){
        editSourceChannel->Text = sourceNo > 0 ? IntToStr(sourceNo) : UnicodeString(L"");
    }
    editSource->ReadOnly = true;
    editSourceChannel->ReadOnly = !editable;
    editTarget->ReadOnly = true;
    if(phase != mcIdle) editChannel->Text = IntToStr(targetNo);
    editChannel->Enabled = phase == mcIdle && journalOkay && contextReady;
    chkInserted->Enabled = phase == mcIdle && journalOkay && contextReady;
    btnReport->Enabled = phase == mcIdle && journalOkay && contextReady;
    btnRetry->Enabled = IsBlocking() && !polling &&
        (phase < mcReady || !journalOkay) && !restored;
    btnResume->Enabled = !manualEntry && (phase == mcReady || (phase == mcMoving && !polling)) && journalOkay && !restored;
    btnSourceReturn->Visible = !manualEntry;
    btnClose->Enabled = !polling;
    RefreshFmsWaitLabel();
}
void TManualCompleteForm::Fail(const UnicodeString &message)
{
	if(MesOpc != NULL) MesOpc->SetLocalAlarm(NGSorterErrors::ManualRecovery,true);
    if(phase == mcMoving && robostar != NULL) robostar->req_Pause(true);
    polling = false;
    lblStatus->Caption = message;
    MainForm->WriteOpcUaLog("ERROR", "[MANUAL COMPLETE] " + AnsiString(message), true);
    RefreshControls();
}
void TManualCompleteForm::OpenManualEntry()
{
    // 자동작업의 셀을 특정하지 못한 경우에만 사용한다. 현재 트레이 ID는 표시만 하고
    // 채널만 입력받는다. contextReady=true여도 실제 보고 전에 ReadManualEntry로 재검증.
    // 기존 미완료 보고가 있으면 이 함수로 들어오지 않아 기존 복구 기록을 보존한다.
    manualEntry = true;
    contextReady = true;
    toolNo = sourceNo = targetNo = reservedNo = 0;
    sourceId = MainForm->pTrayid_source->Caption.Trim();
    targetId = MainForm->pTrayid_target->Caption.Trim();
    cellId = lotId = ngCode = grade = "";
    workFlag = false;
    editSourceChannel->Text = "";
    editChannel->Text = "";
    chkInserted->Checked = false;
    lblStatus->Caption = RecoveryText("MSG_MC_MANUAL_ENTRY");
    RefreshControls();
    ShowModal();
}

void TManualCompleteForm::OpenRecovery(int ToolNo)
{
    if(Visible){BringToFront();return;}
    ApplyLanguage();
    // 미완료 건이 없을 때만 새 정보를 읽는다. 창을 다시 열었다고 기존 보고를 초기화하지 않는다.
    if(!IsBlocking()){
        contextReady = false;
        manualEntry = false;
        if(ToolNo < 1 || ToolNo > gripCnt || !MainForm->CanStartManualCellCompletion()){
            OpenManualEntry();return;
        }
        toolNo = ToolNo;
        sourceNo = gripper->tool[toolNo-1].source_ch.ToIntDef(0);
        reservedNo = gripper->tool[toolNo-1].target_ch.ToIntDef(0);
        sourceId = MainForm->pTrayid_source->Caption.Trim();
        targetId = MainForm->pTrayid_target->Caption.Trim();
        if(sourceNo < 1 || sourceNo > MainForm->tray_source.SLOT_COUNT ||
            reservedNo < 1 || reservedNo > MainForm->tray_target.SLOT_COUNT ||
            sourceId.IsEmpty() || targetId.IsEmpty() || MesOpc == NULL ||
            !MesOpc->ReadApprovedSource(sourceNo, cellId, lotId, ngCode, grade, workFlag)){
            OpenManualEntry();return;
        }
        targetNo = reservedNo;
        editChannel->Text = IntToStr(targetNo);
        chkInserted->Checked = false;
        contextReady = true;
        lblStatus->Caption = RecoveryText("MSG_MC_CONFIRM");
        gripper->req_Pause(true);
        robostar->req_Pause(true);
        MainForm->stopBtnClick(NULL);
    }else if(restored || !journalOkay){
        lblStatus->Caption = RecoveryText("MSG_MC_RESTORED");
    }
    RefreshControls();
    ShowModal();
}
bool TManualCompleteForm::ContextMatches()
{
    if(manualEntry){
        return MainForm->CanStartManualCellReport() &&
            MainForm->pTrayid_source->Caption.Trim() == sourceId &&
            MainForm->pTrayid_target->Caption.Trim() == targetId &&
            sourceNo >= 1 && sourceNo <= 96 && targetNo >= 1 && targetNo <= 96 &&
            !sourceId.IsEmpty() && !targetId.IsEmpty() && sourceId != targetId && !cellId.IsEmpty();
    }
    return MainForm->pTrayid_source->Caption.Trim() == sourceId &&
        MainForm->pTrayid_target->Caption.Trim() == targetId &&
        sourceNo >= 1 && sourceNo <= MainForm->tray_source.SLOT_COUNT &&
        targetNo >= 1 && targetNo <= MainForm->tray_target.SLOT_COUNT &&
        reservedNo >= 1 && reservedNo <= MainForm->tray_target.SLOT_COUNT &&
        toolNo >= 1 && toolNo <= gripCnt &&
        MainForm->tray_source.SLOT_ID[sourceNo-1] == cellId &&
        gripper->tool[toolNo-1].source_ch.ToIntDef(0) == sourceNo;
}
bool TManualCompleteForm::ApplyPhysicalCompletion()
{
    if(!ContextMatches()) { Fail(RecoveryText("MSG_MC_CONTEXT"));return false; }
    // 이 함수는 물리 삽입 명령이 아니라 이미 완료한 삽입 결과를 기록하는 함수이다.
    // 직접 보고 방식에서는 연결된 자동작업을 모르므로 로컬 트레이 정보를 변경하지 않는다.
    if(manualEntry) return true;
    int from = sourceNo-1, to = targetNo-1, reserved = reservedNo-1;
    if(MainForm->tray_target.CELL_EXIST[to] && MainForm->tray_target.SLOT_ID[to] != cellId){
        Fail(RecoveryText("MSG_MC_OCCUPIED"));return false;
    }
    // 원래 예약 채널과 실제 삽입 채널이 다르면 비어 있는 기존 예약만 해제한다.
    // 재시도 때 중복 차감하지 않도록, 양쪽 기록을 반영한 뒤 빈 슬롯 수를 다시 센다.
    if(reserved != to && MainForm->tray_target.PICK[reserved] == "R" &&
        !MainForm->tray_target.CELL_EXIST[reserved]){
        MainForm->tray_target.PICK[reserved] = "N";
        MainForm->tray_target.SLOT_ID[reserved] = "";
        MainForm->tray_target.CELL_LOT_ID[reserved] = "";
        MainForm->tray_target.LOSS_CD[reserved] = "";
        MainForm->tray_target.RANK[reserved] = "";
        MainForm->tray_target.WORK_FLAG[reserved] = false;
        MainForm->DisplayTargetCell(-1, reserved);
        MainForm->DisplayTargetCellInfo(-1, reserved);
    }
    MainForm->tray_source.CELL_EXIST[from] = false;
    MainForm->tray_source.PICK[from] = "N";
    MainForm->tray_target.CELL_EXIST[to] = true;
    MainForm->tray_target.PICK[to] = "Y";
    MainForm->tray_target.SLOT_ID[to] = cellId;
    MainForm->tray_target.CELL_LOT_ID[to] = lotId;
    MainForm->tray_target.LOSS_CD[to] = ngCode;
    MainForm->tray_target.RANK[to] = grade;
    MainForm->tray_target.WORK_FLAG[to] = workFlag;
    gripper->tool[toolNo-1].target_ch = IntToStr(targetNo);
    gripper->tool[toolNo-1].eject_end = gripper->tool[toolNo-1].insert_end = true;
    MainForm->tray_target.remainCnt = 0;
    for(int i=0;i<MainForm->tray_target.SLOT_COUNT;++i)
        if(!MainForm->tray_target.CELL_EXIST[i] && MainForm->tray_target.PICK[i] != "R")
            ++MainForm->tray_target.remainCnt;
    MainForm->DisplaySourceCell(-1, from);
    MainForm->DisplayTargetCell(-1, to);
    MainForm->DisplayTargetCellInfo(-1, to);
    if(!MainForm->setTrayInfo(0) || !MainForm->setTrayInfo(1)){
        Fail(RecoveryText("MSG_MC_SAVE_FAILED"));return false;
    }
    return true;
}
void TManualCompleteForm::CancelRequest()
{
    // '취소'는 FMS 요청 비트 OFF를 뜻한다. 성공한 보고/작업자 삽입을 되돌리지 않는다.
    MesOpc->CELL_TRACK_OUT_CANCEL();
    Mod_Fms->FlushPendingPcTags(false);
    requestClearQueued = true;
}
void TManualCompleteForm::SendRequest()
{
    // 수동 전용 태그가 아니라 기존 자동작업의 CellTrackOut 함수를 그대로 사용한다.
    // mcWaitResult를 먼저 저장한 뒤 요청하므로, 중단되어도 보고 진행 사실이 남는다.
    phase = mcWaitResult;
    if(!SaveJournal()) return;
    MesOpc->CELL_TRACK_OUT_REQUEST(sourceNo, targetNo, cellId, sourceId, targetId);
    started = GetTickCount();
    lblStatus->Caption = RecoveryText("MSG_MC_WAIT_RESULT");
}
bool TManualCompleteForm::ReadManualEntry()
{
    sourceNo = editSourceChannel->Text.ToIntDef(0);
    targetNo = editChannel->Text.ToIntDef(0);
    if(sourceId.IsEmpty() || targetId.IsEmpty() ||
        sourceId == targetId || sourceNo < 1 || sourceNo > 96 || targetNo < 1 || targetNo > 96){
        lblStatus->Caption = RecoveryText("MSG_MC_MANUAL_FIELDS");
        return false;
    }
    // 창을 연 뒤 트레이가 바뀌었다면 이전 채널/셀을 새 트레이에 보고하지 못하게 한다.
    if(MainForm->pTrayid_source->Caption.Trim() != sourceId ||
        MainForm->pTrayid_target->Caption.Trim() != targetId){
        lblStatus->Caption = RecoveryText("MSG_MC_CONTEXT");
        return false;
    }
    if(!MainForm->CanStartManualCellReport()){
        lblStatus->Caption = RecoveryText("MSG_MC_MANUAL_BUSY");
        return false;
    }
    if(MesOpc == NULL || Mod_Fms == NULL || !Mod_Fms->IsGatewayConnected()){
        lblStatus->Caption = RecoveryText("MSG_MC_CONNECTION");
        return false;
    }
    cellId = lotId = ngCode = grade = "";
    workFlag = false;
    // Cell ID는 직접 입력하지 않는다. Tray Load 때 보관한 해당 Source 채널 원본을 사용한다.
    if(!MesOpc->ReadApprovedSource(sourceNo, cellId, lotId, ngCode, grade, workFlag)){
        lblStatus->Caption = RecoveryText("MSG_MC_CELL_LOOKUP");
        return false;
    }
    UnicodeString detail = L"Source: " + UnicodeString(sourceId) + L" / " + IntToStr(sourceNo) +
        L"\r\nTarget: " + UnicodeString(targetId) + L" / " + IntToStr(targetNo) +
        L"\r\n\r\n" + RecoveryText("MSG_MC_MANUAL_CONFIRM");
    return MessageBox(Handle, detail.c_str(), Caption.c_str(),
        MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES;
}

void __fastcall TManualCompleteForm::btnReportClick(TObject *Sender)
{
    // [보고 버튼] 권한/삽입 확인 후 보고 내용을 확정하고 저장한다. 여기서 셀을 움직이지 않는다.
    // 직접 보고와 자동작업 복구는 준비 검사가 다르지만, 이후 FMS 응답 처리는 같은 타이머 사용.
    if(!AccessControl().Require(alEngineer, "TManualCompleteForm.btnReportClick")) return;
    if(IsBlocking() || !contextReady || !chkInserted->Checked) return;
    if(manualEntry){
        if(!ReadManualEntry()) return;
        phase = mcPrepared;
        if(!SaveJournal()) return;
        MainForm->WriteOpcUaLog("EVENT", "[MANUAL ENTRY] CONFIRMED CellId=" + cellId +
            " From=" + sourceId + "/" + IntToStr(sourceNo) + " To=" + targetId + "/" +
            IntToStr(targetNo) + " / report only; no tray-map update or motion", true);
        btnRetryClick(Sender);
        return;
    }
    targetNo = editChannel->Text.ToIntDef(0);
    if(!ContextMatches() || !MainForm->CanStartManualCellCompletion()){
        Fail(RecoveryText("MSG_MC_CONTEXT"));return;
    }
    int to = targetNo-1;
    bool alreadyInserted = targetNo == reservedNo && gripper->tool[toolNo-1].insert_end &&
        MainForm->tray_target.SLOT_ID[to] == cellId;
    if((MainForm->tray_target.CELL_EXIST[to] && !alreadyInserted) ||
        (targetNo != reservedNo && MainForm->tray_target.CELL_EXIST[reservedNo-1]) ||
        (targetNo != reservedNo && MainForm->tray_target.PICK[to] == "R")){
        Fail(RecoveryText("MSG_MC_OCCUPIED"));return;
    }
    if(!robostar->PrepareCellRecovery(false)){
        Fail(RecoveryText("MSG_RECOVERY_INTERLOCK"));return;
    }
    robostar->req_Pause(true);
    phase = mcPrepared;
    if(!SaveJournal()) return;
    MainForm->WriteOpcUaLog("EVENT", "[MANUAL COMPLETE] PHYSICAL CONFIRMED CellId=" + cellId +
        " From=" + sourceId + "/" + IntToStr(sourceNo) + " To=" + targetId + "/" + IntToStr(targetNo), true);
    btnRetryClick(Sender);
}
void __fastcall TManualCompleteForm::btnRetryClick(TObject *Sender)
{
    if(!AccessControl().Require(alEngineer, "TManualCompleteForm.btnRetryClick")) return;
    if(restored || phase == mcIdle) return;
    if(phase >= mcReady){
        if(SaveJournal()) RefreshControls();
        return;
    }
    if(!ContextMatches()){Fail(RecoveryText("MSG_MC_CONTEXT"));return;}
    // [재시도 버튼] 셀 취출/삽입을 다시 하지 않는다. 응답 대기 실패이면 먼저 OFF/0 초기화.
    // mcAccepted(성공 기록 있음)이면 다시 ON 보고하지 않고 OFF/0 초기화만 이어서 한다.
    if(phase == mcWaitResult) phase = mcClearForSend;
    requestClearQueued = false;
    if(!SaveJournal()) return;
    polling = true;
    started = GetTickCount();
    lblStatus->Caption = RecoveryText(phase == mcAccepted ?
        "MSG_MC_WAIT_RESET" : "MSG_MC_HANDSHAKE_WAIT");
    RefreshControls();
}
void __fastcall TManualCompleteForm::pollTimerTimer(TObject *Sender)
{
    // [진행 타이머] 버튼에서 polling=true로 켠 경우에만 단계 진행.
    // Fail()은 현재 단계를 보존하고 polling=false로 하므로 오류 상태에서 다음 단계로 가지 않는다.
    if(!polling || !Visible) return;
    if(!ContextMatches()){Fail(RecoveryText("MSG_MC_CONTEXT"));return;}
    // FMS 보고 완료 후 재개 버튼이 요청한 대기위치 이동을 확인하는 별도 경로(120초 제한).
    if(phase == mcMoving){
        if(robostar->pauseStatus || robostar->seq == seqPause){Fail(RecoveryText("MSG_RECOVERY_INTERLOCK"));return;}
        if(robostar->seq == seqIdle){
            if(!robostar->AreAxesStopped() || !robostar->IsRecoveryStandby()){
                Fail(RecoveryText("MSG_RECOVERY_INTERLOCK"));return;
            }
            if(!gripper->PrepareNextAfterManualCompletion()){
                Fail(RecoveryText("MSG_MC_SAVE_FAILED"));return;
            }
            robostar->req_Pause(true);
            phase = mcIdle;
            if(!SaveJournal()){phase = mcReady;RefreshControls();return;}
            polling = false;
            ErrorForm_eject->Hide();
            ErrorForm_insert->Hide();
            ModalResult = mrOk;
            MainForm->ResumeAfterManualCellCompletion();
			if(MesOpc != NULL) MesOpc->SetLocalAlarm(NGSorterErrors::ManualRecovery,false);
            return;
        }
        if(GetTickCount()-started > 120000){
            robostar->req_Pause(true);
            Fail(RecoveryText("MSG_RECOVERY_INTERLOCK"));
        }
        return;
    }
    if(MesOpc == NULL || Mod_Fms == NULL || !Mod_Fms->IsGatewayConnected()){
        Fail(RecoveryText("MSG_MC_CONNECTION"));return;
    }
    // 삽입 확인 저장 -> 로컬 기록 반영 -> 새 요청 전 초기화 순서. 직접 보고는 기록 반영 생략.
    if(phase == mcPrepared){
        if(!ApplyPhysicalCompletion()) return;
        phase = mcClearForSend;
        if(!SaveJournal()) return;
    }
    if((phase == mcClearForSend || phase == mcAccepted) && !requestClearQueued) CancelRequest();
    int response = MesOpc->CELL_TRACK_OUT_RESPONSE_VALUE();
    TManualReply reply = ManualCellReply(phase, response);
    // 전송 큐에 넣었다고 전송 완료가 아니다. OFF 전송이 끝나야 ON으로 바꾸고,
    // ON 전송이 끝나야 응답을 판정한다(큐에서 OFF/ON이 하나로 합쳐지는 것을 방지).
    bool writeComplete = MesOpc->CELL_TRACK_OUT_WRITE_COMPLETE(phase == mcWaitResult);
    if(!writeComplete) reply = mrWait;
    if(reply == mrSuccess){
        // Response=1: 성공 사실을 먼저 저장하고 Request OFF. 아직 보고 처리 전체 완료는 아니다.
        phase = mcAccepted;
        if(!SaveJournal()) return;
        if(!manualEntry) MainForm->cellRecoveryReportAccepted = true;
        CancelRequest();
        started = GetTickCount();
        lblStatus->Caption = RecoveryText("MSG_MC_WAIT_RESET");
    }else if(reply == mrFailure || reply == mrInvalid){
        // Response=2 또는 잘못된 값: OFF 요청 후 오류 표시. 작업자가 Retry해야 다시 진행한다.
        phase = mcClearForSend;
        if(!SaveJournal()) return;
        CancelRequest();
        Fail(RecoveryText("MSG_MC_REJECTED") + L" " + IntToStr(response));
    }else if(reply == mrReset){
        // Response=0: 보고 전 초기화이면 새 요청, 성공 후 초기화이면 전체 보고 완료 처리.
        if(phase == mcClearForSend){SendRequest();}
        else{
            if(!manualEntry) MainForm->cellRecoveryReportAccepted = true;
            if(!ApplyPhysicalCompletion()) return;
            phase = mcReady;
            if(!SaveJournal()) return;
            MesOpc->CLEAR_CELL_TRACK_OUT_DATA();
            MesOpc->SetLocalAlarm(NGSorterErrors::ManualRecovery,false);
            if(!manualEntry){
                MesOpc->SetLocalAlarm(NGSorterErrors::Eject,false);
                MesOpc->SetLocalAlarm(NGSorterErrors::Insert,false);
            }
			MesOpc->SetLocalAlarm(NGSorterErrors::FmsCellTrackOut,false);
            polling = false;
            lblStatus->Caption = RecoveryText(manualEntry ? "MSG_MC_MANUAL_DONE" : "MSG_MC_READY");
            MainForm->WriteOpcUaLog("EVENT", "[MANUAL COMPLETE] CellTrackOut handshake complete CellId=" + cellId, true);
        }
    }else if(GetTickCount()-started > 30000){
        // FMS 대기 30초 초과. 성공 전에는 재요청 준비, 성공 후에는 초기화 대기 단계를 유지.
        if(phase == mcWaitResult){
            phase = mcClearForSend;
            if(!SaveJournal()) return;
            CancelRequest();
        }
        Fail(RecoveryText("MSG_MC_TIMEOUT"));
    }
    RefreshControls();
}
void __fastcall TManualCompleteForm::btnResumeClick(TObject *Sender)
{
    // [재개 버튼] 자동작업 정보가 있는 복구만 허용. 즉시 다음 취출을 하지 않고,
    // 대기위치 이동 -> 타이머의 정지/위치 확인 -> PrepareNextAfterManualCompletion 순서로 진행.
    if(!AccessControl().Require(alEngineer, "TManualCompleteForm.btnResumeClick")) return;
    if(manualEntry || (phase != mcReady && phase != mcMoving) || restored || !journalOkay) return;
    if(!ContextMatches() || !robostar->PrepareCellRecovery(false)){
        Fail(RecoveryText("MSG_RECOVERY_INTERLOCK"));return;
    }
    phase = mcMoving;
    if(!SaveJournal()) return;
    robostar->req_WaitPosition();
    polling = true;
    started = GetTickCount();
    lblStatus->Caption = RecoveryText("MSG_MC_MOVING");
    RefreshControls();
}
void __fastcall TManualCompleteForm::btnCloseClick(TObject *Sender)
{
    if(polling) return;
    Close();
}
void __fastcall TManualCompleteForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    CanClose = !polling;
    if(CanClose && manualEntry && phase == mcReady && !restored){
        // 직접 보고는 '닫기'로 완료 기록을 mcIdle로 바꾼다. AUTO 전환/서보 이동은 하지 않는다.
        phase = mcIdle;
        if(!SaveJournal()){
            phase = mcReady;
            CanClose = false;
            RefreshControls();
        }else contextReady = false;
    }
    // 그 외 닫기는 미완료 단계/복구파일을 보존한다. 창을 닫아도 IsBlocking 차단은 유지된다.
}
