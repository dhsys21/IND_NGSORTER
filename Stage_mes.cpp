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
		for(int i=0; i<tray->SLOT_COUNT; ++i){
			psort_bad[i]->Color = clWhite;
			psort_rank[i]->Color = clWhite;
			psort_ing[i]->Color = clWhite;
			psort_bad[i]->Caption = tray->LOSS_CD[i] + "[" + tray->PICK[i] + "]";
			psort_rank[i]->Caption = tray->RANK[i];
			if(tray->PICK[i] == "Y"){
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

		for(int i = 0; i < tray->TRAY_GUBUN; ++i)
		{
			if(tray->PICK[i] == "Y"){
				color_target[i/24][23 - (i%24)] = clSilver;
				str = tray->LOSS_CD[i] + "-" + getCodeName(tray->LOSS_CD[i].Trim());
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

void __fastcall TMainForm::CompleteOpcTrayLoad(bool sourceTray)
{
	TRAY_INFO *loadedTray = sourceTray ? &tray_source : &tray_target;
	tray = loadedTray;
	loadedTray->startTime = Now();
	loadedTray->remainCnt = 0;

	if (sourceTray)
	{
		memoMainLineAdd("[FMS OPC UA] Source tray load response complete.");
		pBYPASS->Caption = loadedTray->PASS;
		pbad_sum->Caption = "0";
		badList->Clear();
		loadedTray->empTray = true;

		for (int i = 0; i < loadedTray->SLOT_COUNT && i < 96; ++i)
		{
			psort_bad[i]->Color = clWhite;
			psort_rank[i]->Color = clWhite;
			psort_ing[i]->Color = clWhite;
			psort_bad[i]->Caption = loadedTray->LOSS_CD[i] + "[" + loadedTray->PICK[i] + "]";
			psort_rank[i]->Caption = loadedTray->RANK[i];
			if (loadedTray->PICK[i] == "Y")
			{
				++loadedTray->remainCnt;
				psort_ing[i]->Caption = "NG";
				AddList(loadedTray->LOSS_CD[i]);
			}
			else
			{
				psort_ing[i]->Caption = "**";
				if (loadedTray->empTray && !loadedTray->SLOT_ID[i].IsEmpty())
					loadedTray->empTray = false;
			}
		}

		setTrayInfo(0);
		if (pbad_sum->Caption.ToIntDef(0) <= stage.limitCnt)
		{
			memoMainLineAdd("[FMS OPC UA] Source tray centering request.");
			if (PlcBin != NULL) PlcBin->CmdSourceCenteringRequest(true);
		}
		else
		{
			memoMainLineAdd("[FMS OPC UA] NG count exceeds the configured limit.");
			ErrorForm_limit->ShowError();
		}
	}
	else
	{
		memoMainLineAdd("[FMS OPC UA] Target tray load response complete.");
		AnsiString cellText;
		for (int i = 0; i < loadedTray->SLOT_COUNT && i < 96; ++i)
		{
			if (loadedTray->PICK[i] == "Y")
			{
				cellText = loadedTray->LOSS_CD[i] + "-" + getCodeName(loadedTray->LOSS_CD[i].Trim());
				color_target[i / 24][23 - (i % 24)] = clSilver;
				targetGrid->Cells[i / 24][23 - (i % 24)] = cellText;
				pTarget_bad[i]->Caption = cellText;
				pTarget_bad[i]->Color = clSilver;
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
		setTrayInfo(1);
	}

	opcTrayLoaded[sourceTray ? 0 : 1] = true;
	TryStartOpcProcess();
	tray = &tray_target;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::TryStartOpcProcess()
{
	if (opcProcessStarted || opcProcessStartPending)
		return;
	if (!opcTrayLoaded[0] || !opcTrayLoaded[1])
		return;
	if (!IsSourceCenteringSignal())
		return;
	if (MesOpc == NULL || Mod_Fms == NULL || !Mod_Fms->IsGatewayConnected())
		return;

	MesOpc->PROCESS_START_REQUEST();
	opcProcessStartPending = true;
	opcProcessStartTick = GetTickCount();
	opcMesTimer->Enabled = true;
	memoMainLineAdd("[FMS OPC UA] Process start request.");
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::NotifyTrayInfo(AnsiString strTray, bool bsrc)
{
	int index = bsrc ? 0 : 1;
	tray = bsrc ? &tray_source : &tray_target;
	opcTrayLoaded[index] = false;
	if (bsrc)
	{
		opcProcessStarted = false;
		if (opcProcessStartPending && MesOpc != NULL)
			MesOpc->PROCESS_START_CANCEL();
		opcProcessStartPending = false;
	}
	if (bsrc) pwork1->Color = clSilver;
	else pwork2->Color = clSilver;

	if (MesOpc == NULL || Mod_Fms == NULL || !Mod_Fms->IsGatewayConnected())
	{
		ShowCommonError("FMS Gateway is not connected",
			"Tray load request was not sent. Check the gateway connection.");
		return;
	}

	MesOpc->TRAY_LOAD_REQUEST(bsrc);
	opcTrayLoadPending[index] = true;
	opcTrayLoadStartTick[index] = GetTickCount();
	opcMesTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::NotifyIdMatching_source()
{
	// 트레이 정보
	AnsiString strData;
	tx->MSG_ID = "ID_MATCHING_EVENT";
	tx->LOT_ID = pTrayid_source->Caption;
	strData = "<DATA><MATCHING_LOCAL>" + pTrayid_source2->Caption + "</MATCHING_LOCAL>";
	strData = strData + "<MATCHING_TARGET></MATCHING_TARGET>";
	strData = strData + "<ID_MATCHING>Y</ID_MATCHING><MATCHING_STEP>1</MATCHING_STEP>";   	//
	strData = strData + "<SLOT_COUNT>" + IntToStr(tray_source.SLOT_COUNT) + "</SLOT_COUNT>";

	for(int i=0; i< tray_source.SLOT_COUNT; ++i){
		strData = strData + "<SLOT_DATA><SLOT_POSITION>" + tray_source.SLOT_POSITION[i] + "</SLOT_POSITION><SLOT_ID>" + tray_source.SLOT_ID[i] + "</SLOT_ID>";
		strData = strData  + "<RESULT>" + tray_source.PICK[i] + "</RESULT><UDF></UDF></SLOT_DATA>";
	}
	tx->DATA = strData + "</DATA>";
	mes->SendMsg(tx);

//	setTrayInfo(0);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::NotifyIdMatching_target(AnsiString matchingStep)
{
	// 트레이 정보
	AnsiString strData;
	tx->MSG_ID = "ID_MATCHING_EVENT";
	tx->LOT_ID = pTrayid_target->Caption;
	strData = "<DATA><MATCHING_LOCAL></MATCHING_LOCAL>";
	strData = strData + "<MATCHING_TARGET>" + pTrayid_target2->Caption +"</MATCHING_TARGET>";
	strData = strData + "<ID_MATCHING>Y</ID_MATCHING><MATCHING_STEP>" + matchingStep +"</MATCHING_STEP>";
	strData = strData + "<SLOT_COUNT>" + IntToStr(tray_target.SLOT_COUNT) + "</SLOT_COUNT>";

	for(int i=0; i< tray_target.SLOT_COUNT ; ++i){	// 대상 트레이 셀 수량은 SLOT_COUNT개로 수정
		strData = strData + "<SLOT_DATA><SLOT_POSITION>" + tray_target.SLOT_POSITION[i] + "</SLOT_POSITION><SLOT_ID>" + tray_target.SLOT_ID[i] + "</SLOT_ID>";
		strData = strData + "<RESULT>" + tray_target.PICK[i] + "</RESULT><UDF></UDF></SLOT_DATA>";
	}
	tx->DATA = strData + "</DATA>";
	mes->SendMsg(tx);
	tx->errMsg = "[" + tx->LOT_ID + "] Target tray ID_MATCHING_EVENT response time out from MES";
	mesTimer->Enabled = true;

	setTrayInfo(1);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::NotifyTransferIn(AnsiString strTray)
{
	// OPC UA ProcessStart is issued after both trays are loaded and source centering is complete.
	TryStartOpcProcess();
}//---------------------------------------------------------------------------

void __fastcall TMainForm::NotifyTransferOut(AnsiString strTray)
{
	// 작업시작 보고
	tx->MSG_ID = "TRANSFER_OUT_EVENT";
	tx->LOT_ID = strTray;
	tx->DATA = "<DATA><WORK_NO></WORK_NO></DATA>";
	tx->errMsg = "[" + tx->LOT_ID + "] Source tray TRANSFER_OUT_EVENT response timeout from MES";
	mes->SendMsg(tx);
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




