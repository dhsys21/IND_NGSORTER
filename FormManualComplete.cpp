#include <vcl.h>
#pragma hdrstop
#include <IniFiles.hpp>
#include "FormBase.h"
#include "FormManualComplete.h"
#pragma package(smart_init)
#pragma resource "*.dfm"
TManualCompleteForm *ManualCompleteForm;
static UnicodeString RecoveryText(const char *key) { return BaseForm->GetLangStr(key); }
__fastcall TManualCompleteForm::TManualCompleteForm(TComponent *Owner) : TForm(Owner)
{
    phase = mcIdle;
    toolNo = sourceNo = targetNo = reservedNo = 0;
    polling = restored = workFlag = false;
    requestClearQueued = false;
    journalOkay = true;
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
            restored = IsBlocking();
        }__finally{delete ini;}
    }catch(Exception &){journalOkay = false;}
}
bool TManualCompleteForm::SaveJournal()
{
    // Physical confirmation is durable BEFORE map changes or the first request.
    // Accepted response is durable BEFORE Request OFF; never resend an accepted cell.
    try{
        UnicodeString temp = JournalPath() + L".pending";
        TMemIniFile *ini = new TMemIniFile(temp);
        try{
            ini->Clear();
            ini->WriteInteger("Recovery", "Phase", phase);
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
    lblCell->Caption = L"Cell ID";
    lblChannel->Caption = RecoveryText("CAP_TARGET_CHANNEL");
    chkInserted->Caption = RecoveryText("CAP_MC_CONFIRM");
    btnReport->Caption = RecoveryText("CAP_MC_REPORT");
    btnRetry->Caption = RecoveryText("CAP_RETRY");
    btnResume->Caption = RecoveryText("CAP_MC_RESUME");
    btnClose->Caption = RecoveryText("CAP_CLOSE");
}
void TManualCompleteForm::RefreshControls()
{
    editSource->Text = UnicodeString(sourceId) + L" / " + IntToStr(sourceNo);
    editTarget->Text = targetId;
    editCell->Text = cellId;
    if(phase != mcIdle) editChannel->Text = IntToStr(targetNo);
    editChannel->Enabled = phase == mcIdle && journalOkay;
    chkInserted->Enabled = phase == mcIdle && journalOkay;
    btnReport->Enabled = phase == mcIdle && journalOkay;
    btnRetry->Enabled = IsBlocking() && !polling &&
        (phase < mcReady || !journalOkay) && !restored;
    btnResume->Enabled = (phase == mcReady || (phase == mcMoving && !polling)) && journalOkay && !restored;
    btnClose->Enabled = !polling;
}
void TManualCompleteForm::Fail(const UnicodeString &message)
{
    if(phase == mcMoving && robostar != NULL) robostar->req_Pause(true);
    polling = false;
    lblStatus->Caption = message;
    MainForm->WriteOpcUaLog("ERROR", "[MANUAL COMPLETE] " + AnsiString(message), true);
    RefreshControls();
}
void TManualCompleteForm::OpenRecovery(int ToolNo)
{
    if(Visible){BringToFront();return;}
    ApplyLanguage();
    if(!IsBlocking()){
        if(ToolNo < 1 || ToolNo > gripCnt || !MainForm->CanStartManualCellCompletion()){
            ShowMessage(RecoveryText("MSG_MC_BUSY"));return;
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
            ShowMessage(RecoveryText("MSG_MC_CONTEXT"));return;
        }
        targetNo = reservedNo;
        editChannel->Text = IntToStr(targetNo);
        chkInserted->Checked = false;
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
    int from = sourceNo-1, to = targetNo-1, reserved = reservedNo-1;
    if(MainForm->tray_target.CELL_EXIST[to] && MainForm->tray_target.SLOT_ID[to] != cellId){
        Fail(RecoveryText("MSG_MC_OCCUPIED"));return false;
    }
    // Idempotent updates: reservations already reduce remainCnt, so recount only
    // after both maps and the replacement reservation are updated.
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
    MesOpc->CELL_TRACK_OUT_CANCEL();
    Mod_Fms->FlushPendingPcTags(false);
    requestClearQueued = true;
}
void TManualCompleteForm::SendRequest()
{
    // This is EXACTLY the production payload builder, with immutable explicit IDs.
    phase = mcWaitResult;
    if(!SaveJournal()) return;
    MesOpc->CELL_TRACK_OUT_REQUEST(sourceNo, targetNo, cellId, sourceId, targetId);
    started = GetTickCount();
    lblStatus->Caption = RecoveryText("MSG_MC_WAIT_RESULT");
}
void __fastcall TManualCompleteForm::btnReportClick(TObject *Sender)
{
    if(IsBlocking() || !chkInserted->Checked) return;
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
    if(restored || phase == mcIdle) return;
    if(phase >= mcReady){
        if(SaveJournal()) RefreshControls();
        return;
    }
    if(!ContextMatches()){Fail(RecoveryText("MSG_MC_CONTEXT"));return;}
    // Retry never replays a physical cell move. After success it only clears Request.
    if(phase == mcWaitResult) phase = mcClearForSend;
    requestClearQueued = false;
    if(!SaveJournal()) return;
    polling = true;
    started = GetTickCount();
    lblStatus->Caption = RecoveryText("MSG_MC_WAIT_RESULT");
    RefreshControls();
}
void __fastcall TManualCompleteForm::pollTimerTimer(TObject *Sender)
{
    if(!polling || !Visible) return;
    if(!ContextMatches()){Fail(RecoveryText("MSG_MC_CONTEXT"));return;}
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
    if(phase == mcPrepared){
        if(!ApplyPhysicalCompletion()) return;
        phase = mcClearForSend;
        if(!SaveJournal()) return;
    }
    if((phase == mcClearForSend || phase == mcAccepted) && !requestClearQueued) CancelRequest();
    int response = MesOpc->CELL_TRACK_OUT_RESPONSE_VALUE();
    TManualReply reply = ManualCellReply(phase, response);
    // The asynchronous sender coalesces pending values. Require OFF to leave the
    // queue before ON, and ON to leave it before accepting the response.
    bool writeComplete = MesOpc->CELL_TRACK_OUT_WRITE_COMPLETE(phase == mcWaitResult);
    if(!writeComplete) reply = mrWait;
    if(reply == mrSuccess){
        phase = mcAccepted;
        if(!SaveJournal()) return;
        MainForm->cellRecoveryReportAccepted = true;
        CancelRequest();
        started = GetTickCount();
        lblStatus->Caption = RecoveryText("MSG_MC_WAIT_RESET");
    }else if(reply == mrFailure || reply == mrInvalid){
        phase = mcClearForSend;
        if(!SaveJournal()) return;
        CancelRequest();
        Fail(RecoveryText("MSG_MC_REJECTED") + L" " + IntToStr(response));
    }else if(reply == mrReset){
        if(phase == mcClearForSend){SendRequest();}
        else{
            MainForm->cellRecoveryReportAccepted = true;
            if(!ApplyPhysicalCompletion()) return;
            phase = mcReady;
            if(!SaveJournal()) return;
            MesOpc->CLEAR_CELL_TRACK_OUT_DATA();
            polling = false;
            lblStatus->Caption = RecoveryText("MSG_MC_READY");
            MainForm->WriteOpcUaLog("EVENT", "[MANUAL COMPLETE] CellTrackOut handshake complete CellId=" + cellId, true);
        }
    }else if(GetTickCount()-started > 30000){
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
    if((phase != mcReady && phase != mcMoving) || restored || !journalOkay) return;
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
    // Closing preserves the journal and the global automatic-start interlock.
}
