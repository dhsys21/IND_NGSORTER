#include "FormBase.h"

// 전지 정보 표시
void __fastcall TMainForm::InitTrayInfo(int pos)
{
	if(pos == 0){
		pPROCESS->Caption = "";
		pOPER->Caption = "";
		pDATE->Caption = "";
		pSLOT_COUNT->Caption = "";
		pKIND->Caption = "";
		pBYPASS->Caption = "";
	}else{
		pPROCESS_target->Caption = "";
		pDATE_target->Caption = "";
		pSLOT_COUNT_target->Caption = "";
		pKIND_target->Caption = "";
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::DisplayTrayInfo()
{
	// 선별 트레이 정보
	if(tray == &tray_source){
		memoMainLineAdd("[C_Maint] [MES] 선별 트레이 정보 응답 완료.");
		pTrayid_source2->Caption = mes->mes_rx.LOT_ID;
		pPROCESS->Caption = mes->mes_rx.PROCESS;
		pOPER->Caption = mes->mes_rx.OPER;
		pDATE->Caption = mes->mes_rx.DATE;
		pSLOT_COUNT->Caption = tray->SLOT_COUNT;
		pKIND->Caption = tray->KIND + " " + tray->STOPPERTYPE;
		pBYPASS->Caption = tray->PASS;
		tray->startTime = Now();

		tray->remainCnt = 0;	// 대상 트레이 취출가능 수량 확인
		tray->empTray = true;

		pbad_sum->Caption = "0";
		badList->Clear();
		for(int i=0; i<tray->SLOT_COUNT && i<96; ++i){
			bool CellExist = tray->CELL_EXIST[i];
			psort_bad[i]->Color = clWhite;
			psort_ing[i]->Color = clWhite;
			psort_bad[i]->Caption = (CellExist && tray->PICK[i] == "Y") ? tray->LOSS_CD[i] : AnsiString("");
			if(CellExist && tray->PICK[i] == "Y"){
                tray->remainCnt += 1;
				psort_ing[i]->Caption = "NG";
				AddList(tray->LOSS_CD[i]);
			}else{
				psort_ing[i]->Caption = "**";

				if(tray->empTray && !tray->SLOT_ID[i].IsEmpty())
					tray->empTray = false;
			}
		}

		if(tray->PASS == "Y"){
			memoMainLineAdd("[MES] BYPASS");
			this->CmdTrayOut(0);
		}
		else{
            loadTrayInfo(0);
            if(checkTrayInfo(0))
            {
                if(pbad_sum->Caption.ToInt() <= stage.limitCnt){
                    memoMainLineAdd("[C_Maint] [PLC] 선별 트레이 센터링 요청");
                    if(PlcBin != NULL) PlcBin->CmdSourceCenteringRequest(true);	// 작업2.선별을 해야 할 경우 센터링을 친다. -> stepTimer
                }else{
                    memoMainLineAdd("[C_Maint] NG 수가 설정값을 초과 했습니다.");
                    ErrorForm_limit->ShowError();
                }
            }
            else trayinfoForm->ShowError("[C_Maint] 선별 트레이 정보가 다릅니다.", "선별 트레이 정보를 확인해 주세요.", pTrayid_source->Caption, 0);
		}
		tray = &tray_target;
	}
	else if(tray == &tray_target){
		memoMainLineAdd("[C_Maint] [MES] 대상 트레이 정보 요청 완료.");
		pTrayid_target2->Caption = mes->mes_rx.LOT_ID;
		pPROCESS_target->Caption = mes->mes_rx.PROCESS;
		pDATE_target->Caption = mes->mes_rx.DATE;
		pSLOT_COUNT_target->Caption = tray->SLOT_COUNT;
		pKIND_target->Caption = tray->KIND;

		AnsiString str;
		tray->remainCnt = 0;	// 대상 트레이 투입가능 수량 확인

		for(int i = 0; i < tray->TRAY_GUBUN && i < 96; ++i)
		{
			if(tray->PICK[i] == "Y"){
				color_target[i/24][23 - (i%24)] = clSilver;
				str = tray->LOSS_CD[i]; // Target tray displays NGCode only.
				targetGrid->Cells[i/24][23 - (i%24)] = str;
				pTarget_bad[i]->Caption = str;
				pTarget_bad[i]->Color = clSilver;
			}else{
				tray->remainCnt += 1;
				color_target[i/24][23 - (i%24)] = clWhite;
				targetGrid->Cells[i/24][23 - (i%24)] = "";
				pTarget_bad[i]->Caption = "";
				pTarget_bad[i]->Color = clWhite;
			}
		}
		tray->startTime = Now();

        if(gripper->getReadyStatus())
		{
			loadTrayInfo(1);
			if(checkTrayInfo(1))
			{
				if(MainForm->IsSourceCenteringSignal()){
					memoMainLineAdd("[MES] Work start request.");
					NotifyTransferIn(pTrayid_target2->Caption);		// 작업5. 선별 트레이가 센터링을 치고 있으면 작업 시작 보고를 한다.
				}
			}
			else trayinfoForm->ShowError("[C_Maint] 대상 트레이 정보가 다릅니다.", "대상 트레이 정보를 확인해 주세요.", pTrayid_target->Caption, 1);
        }
		else Memo1->Lines->Add("gripper->getReadyStatus(ntarget, false)");
    }
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMainForm::getCodeName(AnsiString code)
{
	AnsiString str1, str2;
	int pos1;
	for(int i=0; i<badCode->Lines->Count; ++i){
		str1 = badCode->Lines->Strings[i];
		if(str1.Pos(code) > 0){
        	return str1.SubString(6, str1.Length()-5);
		}
	}
    return "";
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::DisplayTranserIn(AnsiString trayid)
{
	if(trayid == pTrayid_source->Caption){
		pwork1->Color = clLime;

		if(pwork2->Color == clLime)
		{
			if(gripper->seq == seqIdle && robostar->seq == seqIdle)
				gripper->req_Init();
		}
	}
	else if(trayid == pTrayid_target->Caption){
		pwork2->Color = clLime;		// 작업6. 대상 트레이 작업 시작 보고가 들어오면 선별 작업을 시작한다.

		if(pwork1->Color == clLime)
		{
			if(gripper->seq == seqIdle && robostar->seq == seqIdle)
				gripper->req_Init();
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::DisplayOpcTrayLoad(bool sourceTray)
{
	int displayIndex = sourceTray ? 0 : 1;
	opcTrayDisplayed[displayIndex] = false;
	TRAY_INFO *loadedTray = sourceTray ? &tray_source : &tray_target;
	tray = loadedTray;
	loadedTray->startTime = Now();
	loadedTray->remainCnt = 0;

	if (sourceTray)
	{
		ProcessStepLog(2, "Location1.TrayLoadResponse=1 / Source tray data displayed");
		memoMainLineAdd("[FMS OPC UA] Source TrayLoadResponse=1; tray data displayed. Waiting Response=0.");
		pBYPASS->Caption = loadedTray->PASS;
		pbad_sum->Caption = "0";
		badList->Clear();
		for (int i = 0; i < loadedTray->SLOT_COUNT && i < 96; ++i)
		{
			bool CellExist = loadedTray->CELL_EXIST[i];
			psort_bad[i]->Color = clWhite;
			psort_ing[i]->Color = clWhite;
			psort_bad[i]->Caption =
				(CellExist && loadedTray->PICK[i] == "Y") ? loadedTray->LOSS_CD[i] : AnsiString("");
			if (CellExist && loadedTray->PICK[i] == "Y")
			{
				++loadedTray->remainCnt;
				psort_ing[i]->Caption = "NG";
				AddList(loadedTray->LOSS_CD[i]);
			}
			else
				psort_ing[i]->Caption = CellExist ? "**" : "";
		}

		setTrayInfo(0);
	}
	else
	{
		ProcessStepLog(5, "Location2.TrayLoadResponse=1 / Target tray data displayed");
		memoMainLineAdd("[FMS OPC UA] Target TrayLoadResponse=1; tray data displayed. Waiting Response=0.");
		//* 불량트레이 관리
		// Location2 셀 정보가 제공되기 전까지 바코드별 로컬 파일을 사용한다.
		int restoreResult = RestoreTargetTrayInfo(pTrayid_target->Caption, false);
		if(restoreResult < 0){
			if(MesOpc != NULL) MesOpc->TRAY_LOAD_CANCEL(false);
			opcTrayLoaded[1] = false;
			pwork2->Color = clSilver;
			memoMainLineAdd("[LOCAL TARGET] Operator cancelled target tray information load.");
			tray = &tray_target;
			return;
		}
		bool restoredLocalTarget = (restoreResult == 1);
		if(restoredLocalTarget)
			memoMainLineAdd("[LOCAL TARGET] Restored: " + pTrayid_target->Caption);
		AnsiString cellText;
		for (int i = 0; i < loadedTray->SLOT_COUNT && i < 96; ++i)
		{
			if (loadedTray->PICK[i] == "R")
			{
				DisplayTargetCell(-1, i);
				DisplayTargetCellInfo(-1, i);
				//* 불량트레이 관리
				memoGripperLineAdd("[TARGET CELL] DISPLAY RESERVATION RESTORED TargetCh=" +
					IntToStr(i + 1) + " PICK=R");
			}
			else if (loadedTray->PICK[i] == "Y")
			{
				cellText = loadedTray->LOSS_CD[i]; // Target tray displays NGCode only.
				color_target[i / 24][23 - (i % 24)] = clSilver;
				targetGrid->Cells[i / 24][23 - (i % 24)] = cellText;
				pTarget_bad[i]->Caption = cellText;
				pTarget_bad[i]->Color = clSilver;
				//* 불량트레이 관리
				memoGripperLineAdd("[TARGET CELL] DISPLAY INSERTED NG RESTORED TargetCh=" +
					IntToStr(i + 1) + " PICK=Y LossCode=" + loadedTray->LOSS_CD[i] +
					" Rank=" + loadedTray->RANK[i]);
			}
			else
			{
				++loadedTray->remainCnt;
				color_target[i / 24][23 - (i % 24)] = clWhite;
				targetGrid->Cells[i / 24][23 - (i % 24)] = "";
				pTarget_bad[i]->Caption = "";
				pTarget_bad[i]->Color = clWhite;
			}
		}
		//* 불량트레이 관리
		// 새 바코드 또는 초기화 선택 시 빈 상태의 바코드별 파일을 즉시 생성한다.
		if(!restoredLocalTarget)
			setTrayInfo(1);
	}

	opcTrayDisplayed[displayIndex] = true;
	ProcessStepLog(sourceTray ? 2 : 5,
		"Tray display complete / Response=1 confirmed");
	tray = &tray_target;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::AdvanceOpcTrayLoad(bool sourceTray)
{
	int index = sourceTray ? 0 : 1;
	int stepNo = sourceTray ? 2 : 5;
	AnsiString locationName = sourceTray ? "Location1" : "Location2";
	AnsiString trayId = sourceTray ? pTrayid_source->Caption : pTrayid_target->Caption;

	if(!opcTrayDisplayed[index])
	{
		opcTrayLoaded[index] = false;
		ProcessStepLog(stepNo, "ERROR - Target advance blocked: tray was not displayed");
		ShowCommonError(locationName + " TrayLoad sequence error",
			"TrayLoadResponse=1 display completion is required before Response=0.");
		return;
	}
	opcTrayLoaded[index] = true;
	CompleteProcessStep(stepNo, locationName +
		".TrayLoadResponse returned to 0 / Tray ID=" + trayId);
	memoMainLineAdd("[FMS OPC UA] " + locationName +
		" TrayLoad handshake complete; advancing to next process.");

	if(sourceTray)
	{
		if(pbad_sum->Caption.ToIntDef(0) <= stage.limitCnt)
		{
			// Source TrayLoad is complete. Request Source centering, but continue
			// directly to Target Tray loading; ProcessStart checks D10104 again.
			BeginProcessStep(3, "D10154 Centering Request=ON / wait D10104");
			SetProcessWaitStatus(3, "D10154 Source Centering Request=ON",
				"D10104 Source Centering", IsSourceCenteringSignal() ? 1 : 0);
			memoMainLineAdd("[FMS OPC UA] Source tray displayed; moving to Target tray process.");
			if(PlcBin != NULL) PlcBin->CmdSourceCenteringRequest(true);

			// Skip the old serial wait at step[0]=1. Target Tray load can run while
			// Source centering is completing.
			step[0].step = 2;
			if(opcTrayLoadPending[1] || opcTrayLoaded[1])
			{
				// Preserve an already active Target transaction; never scan twice.
				step[0].step = 3;
			}
			else if(IsTargetCenteringSignal())
			{
				BeginProcessStep(4, "D10106 Target Centering=ON / waiting barcode");
				pTrayid_target->Caption = "";
				pTrayid_target2->Caption = "";
				ReadTargetTrayBarcode();
				step[0].step = 3;
				step[1].step = 1;
			}
			else
			{
				SetProcessWaitStatus(4, "Source TrayLoad complete",
					"D10106 Target Centering", 0);
			}
		}
		else
		{
			opcTrayLoaded[0] = false;
			memoMainLineAdd("[FMS OPC UA] NG count exceeds the configured limit.");
			ErrorForm_limit->ShowError();
		}
	}

	TryStartOpcProcess();
	tray = &tray_target;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::TryStartOpcProcess()
{
	if(!CheckAutomaticFmsMode("ProcessStart"))
		return;
	// A new process must not start while the previous ProcessEnd handshake is active.
	if (opcProcessStarted || opcProcessStartPending || opcProcessEndPending)
		return;
	if (!opcTrayDisplayed[0] || !opcTrayDisplayed[1] ||
		!opcTrayLoaded[0] || !opcTrayLoaded[1])
		return;
	if (!IsSourceCenteringSignal())
		return;
	if (MesOpc == NULL || Mod_Fms == NULL || !Mod_Fms->IsGatewayConnected())
		return;

	BeginProcessStep(6, "ProcessStart request / wait response");
	MesOpc->PROCESS_START_REQUEST();
	opcProcessStartPending = true;
	opcProcessStartWaitResponseOff = false;
	opcProcessStartResponseOffError = false;
	opcProcessStartResponseResult = 0;
	opcProcessStartTick = GetTickCount();
	opcMesTimer->Enabled = true;
	memoMainLineAdd("[FMS OPC UA] Process start request.");
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::NotifyTrayInfo(AnsiString strTray, bool bsrc)
{
	if(!CheckAutomaticFmsMode(bsrc ? "Source TrayLoad" : "Target TrayLoad"))
		return;
	int index = bsrc ? 0 : 1;
	tray = bsrc ? &tray_source : &tray_target;

	//* 불량트레이 관리
	// Ignore duplicate Location2 loads while the same centered tray is pending/loaded.
	// They otherwise reset PICK=R reservations because Location2 has no cell payload.
	if(!bsrc && IsTargetCenteringSignal() &&
		targetTrayInfoActiveId == strTray &&
		(opcTrayLoadPending[1] || opcTrayLoaded[1] || (gripper != NULL && gripper->seq != seqIdle))){
		memoMainLineAdd("[LOCAL TARGET] Duplicate Location2 load ignored; target map/reservation preserved. TrayId=" + strTray);
		tray = &tray_target;
		return;
	}
	if(!bsrc){
		ProcessStepLog(5, "PREPARE - load LOCAL Target tray information for FMS comparison / TrayId=" + strTray);
		//* 불량트레이 관리
		// 모달 확인창이 열린 동안 타이머가 다시 스캔을 호출해도 중첩 진입하지 않는다.
		if(targetTrayInfoPromptActive){
			tray = &tray_target;
			return;
		}
		targetTrayInfoPromptActive = true;
		int prepareResult = 0;
		try{
			// 바코드를 읽은 직후 기존 정보를 확인하고, 취소 시 FMS 요청도 보내지 않는다.
			// Use the dedicated FMS/LOCAL comparison dialog after TrackIn validation.
			// Do not show the legacy Yes/No confirmation before the FMS request.
			prepareResult = RestoreTargetTrayInfo(strTray, false);
		}
		catch(...){
			targetTrayInfoPromptActive = false;
			throw;
		}
		targetTrayInfoPromptActive = false;
		if(prepareResult < 0){
			opcTrayLoaded[1] = false;
			pwork2->Color = clSilver;
			memoMainLineAdd("[LOCAL TARGET] Operator cancelled before target tray load request.");
			tray = &tray_target;
			return;
		}
		ProcessStepLog(5, "READY - LOCAL Target tray information loaded / next=Location2 TrayLoad Request and FMS comparison");
	}
	opcTrayLoadRetryRequired[index] = false;
	opcTrayDisplayed[index] = false;
	opcTrayLoaded[index] = false;
	if (bsrc)
	{
		opcProcessStarted = false;
		opcSortingStartPending = false;
		opcSortingStartWaitError = false;
		if (opcProcessStartPending && MesOpc != NULL)
			MesOpc->PROCESS_START_CANCEL();
		opcProcessStartPending = false;
		opcProcessStartWaitResponseOff = false;
		opcProcessStartResponseOffError = false;
		opcProcessStartResponseResult = 0;
	}
	if (bsrc) pwork1->Color = clSilver;
	else pwork2->Color = clSilver;

	if (MesOpc == NULL || Mod_Fms == NULL || !Mod_Fms->IsGatewayConnected())
	{
		ProcessStepLog(bsrc ? 2 : 5, "ERROR - FMS Gateway disconnected / TrayLoad Request not sent");
		ShowCommonError("FMS Gateway is not connected",
			"Tray load request was not sent. Check the gateway connection.");
		return;
	}

	BeginProcessStep(bsrc ? 2 : 5, (bsrc ? AnsiString("Location1.Source") : AnsiString("Location2.Target")) +
		" TrayLoad Request=ON / Tray ID=" + strTray + " / WAIT TrayLoadResponse=1");
	ProcessStepLog(bsrc ? 2 : 5,
		(bsrc ? AnsiString("Location1") : AnsiString("Location2")) +
		".TrayLoad Request issued / WAIT " +
		(bsrc ? AnsiString("Location1") : AnsiString("Location2")) +
		".TrayLoadResponse=1");
	MesOpc->TRAY_LOAD_REQUEST(bsrc);
	opcTrayLoadPending[index] = true;
	opcTrayLoadWaitResponseOff[index] = false;
	opcTrayLoadResponseOffError[index] = false;
	opcTrayLoadStartTick[index] = GetTickCount();
	opcMesTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::NotifyIdMatching_source()
{
	// 구형 ASCII ID_MATCHING_EVENT는 사용하지 않는다.
	mesTimer->Enabled = false;
	memoMainLineAdd("[FMS OPC UA] Source cell information is managed by TrackIn/TrackOut.");
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::NotifyIdMatching_target(AnsiString matchingStep)
{
	// 구형 ASCII ID_MATCHING_EVENT 대신 현재 불량트레이 전체 TrackOut 정보를 갱신한다.
	mesTimer->Enabled = false;
	// TrackOutCellInformation is written once at Source completion or before Target unload.
	WriteOpcUaLog("DETAIL", "Target working map saved; final TrackOut deferred. Step=" + matchingStep, false);
	setTrayInfo(1);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::NotifyTransferIn(AnsiString strTray)
{
	// OPC UA ProcessStart is issued after both trays are loaded and source centering is complete.
	TryStartOpcProcess();
}//---------------------------------------------------------------------------

void __fastcall TMainForm::ReportCellTrackOut(int sourceChannel, int targetChannel, AnsiString cellId)
{
	if(!CheckAutomaticFmsMode("CellTrackOut"))
		return;
	// 셀 삽입 완료 직후 전체 TrackOut 맵과 단일 CellTrackOut 이벤트를 함께 보고한다.
	mesTimer->Enabled = false;
	if(MesOpc == NULL || Mod_Fms == NULL || !Mod_Fms->IsGatewayConnected()){
		WriteOpcUaLog("ERROR", "CellTrackOut write skipped: Gateway disconnected", true);
		return;
	}
	if(sourceChannel < 1 || sourceChannel > tray_source.SLOT_COUNT ||
		targetChannel < 1 || targetChannel > tray_target.SLOT_COUNT){
		WriteOpcUaLog("ERROR", "CellTrackOut write skipped: invalid source/target channel", true);
		return;
	}

	AnsiString TrackInCellId;
	AnsiString TrackInLotId;
	AnsiString TrackInNGCode;
	AnsiString TrackInGrade;
	if(!MesOpc->READ_TRACK_IN_CELL(sourceChannel, TrackInCellId,
		TrackInLotId, TrackInNGCode, TrackInGrade)){
		WriteOpcUaLog("ERROR", "CellTrackOut write skipped: Location1 TrackIn cell not found" +
			AnsiString(" CellNoFrom=") + IntToStr(sourceChannel), true);
		return;
	}

	if(!cellId.IsEmpty() && cellId != TrackInCellId){
		WriteOpcUaLog("WARN", "CellTrackOut CellId corrected from target memory=" + cellId +
			" to TrackIn=" + TrackInCellId + " CellNoFrom=" + IntToStr(sourceChannel), true);
	}

	// Merge the just-inserted cell from Location1 TrackInCellInformation into
	// the locally managed target tray before writing the cumulative TrackOut array.
	int targetIndex = targetChannel - 1;
	tray_target.SLOT_ID[targetIndex] = TrackInCellId;
	tray_target.CELL_LOT_ID[targetIndex] = TrackInLotId;
	tray_target.LOSS_CD[targetIndex] = TrackInNGCode;
	tray_target.RANK[targetIndex] = TrackInGrade;
	tray_target.CELL_EXIST[targetIndex] = true;
	tray_target.WORK_FLAG[targetIndex] = tray_source.WORK_FLAG[sourceChannel - 1];
	tray_target.PICK[targetIndex] = "Y";

	BeginProcessStep(12, "CellTrackOut request / wait CellUnloadCompleteResponse");
	WriteOpcUaLog("DETAIL", "CellTrackOut payload SourceCh=" + IntToStr(sourceChannel) +
		" TargetCh=" + IntToStr(targetChannel) + " CellId=" + TrackInCellId +
		" LotId=" + TrackInLotId +
		" Grade=" + TrackInGrade +
		" NGCode=" + TrackInNGCode +
		" WorkFlag=" + IntToStr(tray_target.WORK_FLAG[targetIndex] ? 1 : 0), false);
	MesOpc->CELL_TRACK_OUT_REQUEST(sourceChannel, targetChannel, TrackInCellId);
	opcCellTrackOutPending = true;
	opcCellTrackOutWaitResponseOff = false;
	opcCellTrackOutResponseOffError = false;
	opcCellTrackOutResponseResult = 0;
	opcCellTrackOutStartTick = GetTickCount();
	opcMesTimer->Enabled = true;
	SetProcessWaitStatus(12, "CellUnloadComplete=ON", "CellUnloadCompleteResponse", 0);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::NotifyTransferOut(AnsiString strTray)
{
	if(!CheckAutomaticFmsMode("ProcessEnd/TrayUnload"))
		return;
	// 구형 ASCII TRANSFER_OUT_EVENT는 사용하지 않는다.
	mesTimer->Enabled = false;
	if(MesOpc == NULL || Mod_Fms == NULL || !Mod_Fms->IsGatewayConnected()){
		WriteOpcUaLog("ERROR", "OPC transfer-out report skipped: Gateway disconnected", true);
		return;
	}
	if(opcCellTrackOutPending){
		opcFinalTrackOutTrayId = strTray;
		ProcessStepLog(12, "WAIT - final TrackOutCellInformation deferred until CellTrackOut response completes");
		memoMainLineAdd("[FMS OPC UA] Waiting for the last CellTrackOut response before final TrackOut report.");
		return;
	}
	opcFinalTrackOutTrayId = "";

	// Final cumulative report: Location2 TrackIn snapshot plus every completed insert.
	MesOpc->PROCESS_DATA_WRITE();
	WriteOpcUaLog("DETAIL", "Final TrackOutCellInformation written before process/tray completion", false);
	if(strTray == pTrayid_source->Caption || strTray == pTrayid_source2->Caption){
		if(!opcProcessEndPending){
			BeginProcessStep(14, "ProcessEnd request / wait response");
			MesOpc->PROCESS_END_REQUEST();
			opcProcessEndPending = true;
			opcProcessEndWaitResponseOff = false;
			opcProcessEndResponseOffError = false;
			opcProcessEndResponseResult = 0;
			opcProcessEndTick = GetTickCount();
			opcMesTimer->Enabled = true;
			memoMainLineAdd("[FMS OPC UA] Source ProcessEnd requested; waiting ProcessEndResponse.");
		}else{
			memoMainLineAdd("[FMS OPC UA] Duplicate Source ProcessEnd request ignored; response is pending.");
		}
	}else{
		if(!opcTargetUnloadPending){
			BeginProcessStep(16, "TrayUnload request / wait response");
			MesOpc->TRAY_UNLOAD_REQUEST();
			opcTargetUnloadPending = true;
			opcTargetUnloadWaitResponseOff = false;
			opcTargetUnloadResponseOffError = false;
			opcTargetUnloadResponseResult = 0;
			opcTargetUnloadTick = GetTickCount();
			opcMesTimer->Enabled = true;
			SetProcessWaitStatus(16, "TrayUnloadRequest=ON", "TrayUnloadResponse", 0);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::NotifyEquipStatus(AnsiString process)
{
	/*
		<READY></READY>	                         //READY
		<STATE></STATE>	                         //설비 상태: AUTO / MANUAL
		<PROCESS_STATE></PROCESS_STATE>          //설비 상태: IDLE / PROCESS /  PAUSE / DOWN / MAINT
		<MAINT_STATE></MAINT_STATE>              //MAINT( MAINT 발생시 ) ; SET(보고), CLEAR(해제)
		<MAINT_CODE> <MAINT_CODE>                //MAINT 발생 코드  ( 보고, 해제 모두 처리 )
	 */
	// 설비 상태 보고

	AnsiString state;
	tx->MSG_ID = "EQ_STATE_EVENT";
	tx->LOT_ID = pTrayid_source2->Caption;
	if(stage.arl == nAuto)state = "AUTO";
	else state = "MANUAL";

	if(process == "DOWN")
		tx->DATA = "<DATA><READY>READY</READY><STATE>MANUAL</STATE><PROCESS_STATE>DOWN</PROCESS_STATE><MAINT_STATE>SET</MAINT_STATE><MAINT_CODE>DOWN</MAINT_CODE></DATA>";
	else if(process == "CLEAR")
		tx->DATA = "<DATA><READY>READY</READY><STATE>AUTO</STATE><PROCESS_STATE>IDLE</PROCESS_STATE><MAINT_STATE>CLEAR</MAINT_STATE><MAINT_CODE>DOWN</MAINT_CODE></DATA>";
	else
		tx->DATA = "<DATA><READY>READY</READY><STATE>" + state + "</STATE><PROCESS_STATE>" + process + "</PROCESS_STATE><MAINT_STATE></MAINT_STATE><MAINT_CODE></MAINT_CODE></DATA>";

	mes->SendMsg(tx);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::NotifyAlarm(bool alarm, AnsiString code,  bool warning)
{
	/*
	<DATA>
		 <ALM_STATE></ALM_STATE>	                         //Alarm ; ALARM_SET, ALARM_CLEAR ( 경알람 설비모드, 설비상태 관계없이 발생 )
		 <ALM_CODE></ALM_CODE>	                         //Alarm Code ; 100, 200, 300, 400 …
		 <ALM_TYPE></ALM_TYPE>	                         //Alarm Type ; WARNING(경알람), ABORT(중알람, 설비모드 DOWN 발생)
		 <ALM_TEXT></ALM_TEXT>	                         //Alarm Text ; DOOR OPEN …, READER ERROR …
		 <PORT></PORT>                                                  //알람발생시 C/V PORT 번호 ( C/V 물류만 포함 )

	</DATA>
	*/
	AnsiString state, strWarn;
	tx->MSG_ID = "ALARM_EVENT";
	tx->LOT_ID = pTrayid_source->Caption;

	if(alarm)state = "ALARM_SET";
	else	state = "ALARM_CLEAR";

	if(warning)strWarn = "WARNING";
	else strWarn = "ABORT";

	if(code > 0){
		tx->DATA = "<DATA><ALM_STATE>" + state + "</ALM_STATE><ALM_CODE>" + code + "</ALM_CODE><ALM_TYPE>" + strWarn + "</ALM_TYPE><ALM_TEXT>" + GetAlarmMsg(code.ToInt()) + "</ALM_TEXT><PORT></PORT></DATA>";
		mes->SendMsg(tx);
	}


}
//---------------------------------------------------------------------------




