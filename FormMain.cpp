//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TMainForm *MainForm;

//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
	: TForm(Owner)
{
	this->Parent = BaseForm;
	this->Visible = true;
	this->Left = 15;
	this->Top = 90;

	this->Width = 1900;
	this->Height = 1000;

	MakePanel();
	MakePanel_TargetTray();
	pnlProcessStep[0] = pnlProcessStep01;
	pnlProcessStep[1] = pnlProcessStep02;
	pnlProcessStep[2] = pnlProcessStep03;
	pnlProcessStep[3] = pnlProcessStep04;
	pnlProcessStep[4] = pnlProcessStep05;
	pnlProcessStep[5] = pnlProcessStep06;
	pnlProcessStep[6] = pnlProcessStep07;
	pnlProcessStep[7] = pnlProcessStep08;
	pnlProcessStep[8] = pnlProcessStep09;
	pnlProcessStep[9] = pnlProcessStep10;
	pnlProcessStep[10] = pnlProcessStep11;
	pnlProcessStep[11] = pnlProcessStep12;
	pnlProcessStep[12] = pnlProcessStep13;
	pnlProcessStep[13] = pnlProcessStep14;
	pnlProcessStep[14] = pnlProcessStep15;
	pnlProcessStep[15] = pnlProcessStep16;
	ResetProcessFlow();
	setMapping();
	tray = &tray_target;
	equipMode = modeManual;
    nowLampMode = LampManual;
	InitStep(&step[0]);
	InitStep(&step[1]);

	status_on[1] = pOnX1;
	status_on[2] = pOnY;
	status_on[3] = pOnZ;

	status_org[1] = pOrgX1;
	status_org[2] = pOrgY;
	status_org[3] = pOrgZ;

	status_error[1] = pErrorX1;
	status_error[2] = pErrorY;
	status_error[3] = pErrorZ;

	status_lsp[1] = pLspX1;
	status_lsp[2] = pLspY;
	status_lsp[3] = pLspZ;

	status_lsn[1] = pLsnX1;
	status_lsn[2] = pLsnY;
	status_lsn[3] = pLsnZ;

	status_pos[1] = px1;
	status_pos[2] = py;
	status_pos[3] = pz;


	m_ServoOpen = false;
	m_ServoON = false;
	m_ServoHome = false;
	m_ServoHomeEmg = false;
	// CellExist is the single physical-cell criterion used by Source/Target displays.
	for(int i = 0; i < 96; ++i){
		tray_source.CELL_EXIST[i] = false;
		tray_target.CELL_EXIST[i] = false;
		m_saveTrayInfo[0].CELL_EXIST[i] = false;
		m_saveTrayInfo[1].CELL_EXIST[i] = false;
	}
	for(int i = 0; i < 2; ++i) {
		comBcr[i] = NULL;
		opcTrayLoadPending[i] = false;
		opcTrayLoadWaitResponseOff[i] = false;
		opcTrayLoadResponseOffError[i] = false;
		opcTrayLoadRetryRequired[i] = false;
		opcTrayLoadStartTick[i] = 0;
		opcTrayDisplayed[i] = false;
		opcTrayLoaded[i] = false;
	}
	opcProcessStartPending = false;
	opcProcessStartWaitResponseOff = false;
	opcProcessStartResponseOffError = false;
	opcProcessStartResponseResult = 0;
	opcProcessStarted = false;
	opcProcessStartTick = 0;
	opcProcessEndPending = false;
	opcProcessEndWaitResponseOff = false;
	opcProcessEndResponseOffError = false;
	opcProcessEndResponseResult = 0;
	opcProcessEndTick = 0;
	opcCellTrackOutPending = false;
	opcCellTrackOutWaitResponseOff = false;
	opcCellTrackOutResponseOffError = false;
	opcCellTrackOutResponseResult = 0;
	opcCellTrackOutStartTick = 0;
	opcTargetUnloadPending = false;
	opcTargetUnloadWaitResponseOff = false;
	opcTargetUnloadResponseOffError = false;
	opcTargetUnloadResponseResult = 0;
	opcTargetUnloadTick = 0;
	//* 불량트레이 관리
	targetTrayInfoDeletePending = false;
	targetTrayInfoWasCentered = false;
	targetTrayInfoPromptActive = false;
	targetTrayInfoActiveId = "";
	opcMesTimer = new TTimer(this);
	opcMesTimer->Enabled = false;
	opcMesTimer->Interval = 200;
	opcMesTimer->OnTimer = opcMesTimerTimer;
	comSmoke[0] = NULL;
	CreateIoMonitoringPanel();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::EndThread()
{
	// Stop all sequence/response timers before closing communication objects.
	if(opcMesTimer != NULL) opcMesTimer->Enabled = false;
	if(senTimer != NULL) senTimer->Enabled = false;
	if(stepTimer != NULL) stepTimer->Enabled = false;
	if(mesTimer != NULL) mesTimer->Enabled = false;
	if(gripper != NULL){
		gripper->stepTimer->Enabled = false;
		gripper->waitTimer->Enabled = false;
	}
	if(robostar != NULL){
		robostar->senTimer->Enabled = false;
		robostar->Timer_zUpTest->Enabled = false;
	}

	for(int i = 0; i < 2; ++i){
		opcTrayLoadPending[i] = false;
		opcTrayLoadWaitResponseOff[i] = false;
		opcTrayLoadResponseOffError[i] = false;
		opcTrayLoadRetryRequired[i] = false;
		opcTrayDisplayed[i] = false;
		opcTrayLoaded[i] = false;
		if(comBcr[i] != NULL)
			comBcr[i]->Disconnect();
	}
	opcProcessStartPending = false;
	opcProcessStartWaitResponseOff = false;
	opcProcessStartResponseOffError = false;
	opcProcessStartResponseResult = 0;
	opcProcessEndPending = false;
	opcProcessEndWaitResponseOff = false;
	opcProcessEndResponseOffError = false;
	opcProcessEndResponseResult = 0;
	opcCellTrackOutPending = false;
	opcCellTrackOutWaitResponseOff = false;
	opcCellTrackOutResponseOffError = false;
	opcCellTrackOutResponseResult = 0;
	opcTargetUnloadPending = false;
	opcTargetUnloadWaitResponseOff = false;
	opcTargetUnloadResponseOffError = false;
	opcTargetUnloadResponseResult = 0;

	if(PlcBin != NULL)
		PlcBin->DisConnect();
	if(comSmoke[0] != NULL)
		comSmoke[0]->CommClose();
	if(mes != NULL)
		mes->Stop();
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMainForm::GetProcessStepName(int stepNo) const
{
	static const char *names[16] = {
		"SOURCE TRAY IN", "SOURCE TRAY LOAD", "SOURCE CENTERING", "TARGET TRAY READY",
		"TARGET TRAY LOAD", "PROCESS START", "NG CHANNEL SELECT", "MOVE EJECT CHANNEL",
		"CELL EJECT", "MOVE INSERT CHANNEL", "CELL INSERT", "CELL TRACK OUT",
		"WAIT / NEXT CHECK", "PROCESS END", "SOURCE TRAY OUT", "TARGET TRAY UNLOAD"
	};
	if(stepNo < 1 || stepNo > 16) return "UNKNOWN";
	return names[stepNo - 1];
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ResetProcessFlow()
{
	for(int i = 0; i < 16; ++i){
		processStepComplete[i] = false;
		lastProcessWaitStatus[i] = "";
	}
	currentProcessStep = 0;
	currentProcessDetail = "WAITING FOR SOURCE TRAY";
	UpdateProcessFlowPanel();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::UpdateProcessFlowPanel()
{
	for(int i = 0; i < 16; ++i){
		if(pnlProcessStep[i] != NULL)
			pnlProcessStep[i]->Color = (processStepComplete[i] || currentProcessStep == i + 1) ? clLime : clSilver;
	}
	if(lblCurrentProcess != NULL){
		if(currentProcessStep > 0)
			lblCurrentProcess->Caption = "CURRENT PROCESS : " +
				Format("[STEP %2.2d %s] ", ARRAYOFCONST((currentProcessStep, GetProcessStepName(currentProcessStep)))) +
				currentProcessDetail;
		else
			lblCurrentProcess->Caption = "CURRENT PROCESS : " + currentProcessDetail;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ProcessStepLog(int stepNo, AnsiString msg)
{
	if(stepNo < 1 || stepNo > 16) return;
	AddStatusLog("PROCESS", Format("[STEP %2.2d %s] ",
		ARRAYOFCONST((stepNo, GetProcessStepName(stepNo)))) + msg);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::BeginProcessStep(int stepNo, AnsiString detail)
{
	if(stepNo < 1 || stepNo > 16) return;
	if(stepNo == 1 && currentProcessStep != 1){
		for(int i = 0; i < 16; ++i) processStepComplete[i] = false;
	}
	// Keep every preceding process panel lime once the sequence advances.
	// Repeated sorting (STEP 13 -> STEP 07) does not clear prior completions.
	for(int i = 0; i < stepNo - 1; ++i)
		processStepComplete[i] = true;
	bool changed = currentProcessStep != stepNo;
	currentProcessStep = stepNo;
	if(detail.IsEmpty()) currentProcessDetail = "RUNNING";
	else currentProcessDetail = detail;
	if(changed)
		lastProcessWaitStatus[stepNo - 1] = "";
	UpdateProcessFlowPanel();
	if(changed){
		AnsiString logText = "START";
		if(!detail.IsEmpty()) logText += " - " + detail;
		ProcessStepLog(stepNo, logText);
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CompleteProcessStep(int stepNo, AnsiString detail)
{
	if(stepNo < 1 || stepNo > 16) return;
	processStepComplete[stepNo - 1] = true;
	currentProcessStep = stepNo;
	if(detail.IsEmpty()) currentProcessDetail = "COMPLETE";
	else currentProcessDetail = detail;
	lastProcessWaitStatus[stepNo - 1] = "";
	UpdateProcessFlowPanel();
	AnsiString logText = "COMPLETE";
	if(!detail.IsEmpty()) logText += " - " + detail;
	ProcessStepLog(stepNo, logText);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SetProcessWaitStatus(int stepNo, AnsiString requestName,
	AnsiString responseName, int responseValue)
{
	if(stepNo < 1 || stepNo > 16) return;
	for(int i = 0; i < stepNo - 1; ++i)
		processStepComplete[i] = true;
	AnsiString upperResponseName = responseName.UpperCase();
	AnsiString expected;
	if(upperResponseName.Pos("OFF") > 0)
		expected = "EXPECTED=0 (RESET)";
	else if(upperResponseName.Pos("RESPONSE") > 0)
		expected = "EXPECTED=1 or 2 (RESULT)";
	else
		expected = "EXPECTED=1 (ON)";
	AnsiString status = requestName + " / WAIT " + responseName +
		" / " + expected + " / CURRENT=" + IntToStr(responseValue);
	currentProcessStep = stepNo;
	currentProcessDetail = status;
	UpdateProcessFlowPanel();
	// A global cache causes concurrent Source/Target waits to alternate and log
	// continuously. Cache each process step independently and log each state once.
	if(lastProcessWaitStatus[stepNo - 1] != status){
		lastProcessWaitStatus[stepNo - 1] = status;
		ProcessStepLog(stepNo, status);
	}
}//---------------------------------------------------------------------------
void __fastcall TMainForm::FormShow(TObject *Sender)
{
	ReadSystemInfo();
	if(PlcBin != NULL && !BaseForm->config.plcIp.IsEmpty() &&
		BaseForm->config.plcPortPlc > 0 && BaseForm->config.plcPortPc > 0)
		PlcBin->Connect(BaseForm->config.plcIp,
			BaseForm->config.plcPortPlc, BaseForm->config.plcPortPc);
	InitBarcodeAndSmoke();

	tx = new TX_DATA;
	mes->savePath = (AnsiString)SOCK_LOG;

	stage.init = true;
	stage.limitCnt = 10;
	mdOpen(81,-1,&path);	/*	open 1st CC-Link board		*/
	robostar->config.path = path;
	robostar->io_Init();
	// Recover the real controller state after a program restart without rebooting it.
	robostar->RestoreServoState();

	for(int i=0; i<4; ++i){
		color_target[0][i] = clWhite;
		color_target[1][i] = clWhite;
		color_target[2][i] = clWhite;
		color_target[3][i] = clWhite;
	}

	for(int i = 0; i < 24; i++)
		color_target2[i] = clWhite;

	zoneBtn[0] = zone1;
	zoneBtn[1] = zone2;
	zoneBtn[2] = zone3;
	zoneBtn[3] = zone4;
	ReadZoneList();

	senTimer->Enabled = true;
	stepTimer->Enabled = true;
    LampCount = 0;
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::ReadSystemInfo()
{
	AnsiString file = (AnsiString)BIN + "MainSystemInfo.inf";
	bool exists = FileExists(file);

	if(!exists)
		return false;

	TIniFile *ini = new TIniFile(file);

	BaseForm->config.plcIp = ini->ReadString("COMMUNICATION", "PLC_IP",
		ini->ReadString("PLC", "IPADDRESS", BaseForm->config.plcIp));
	BaseForm->config.plcPortPlc = ini->ReadInteger("COMMUNICATION", "PLC_PORT_PLC",
		ini->ReadInteger("PLC", "PORT1", BaseForm->config.plcPortPlc));
	BaseForm->config.plcPortPc = ini->ReadInteger("COMMUNICATION", "PLC_PORT_PC",
		ini->ReadInteger("PLC", "PORT2", BaseForm->config.plcPortPc));

	BaseForm->config.bcrIp[0] = ini->ReadString("COMMUNICATION", "BCR_SOURCE_IP",
		ini->ReadString("COMMUNICATION", "BCR_IP", BaseForm->config.bcrIp[0]));
	BaseForm->config.bcrPort[0] = ini->ReadInteger("COMMUNICATION", "BCR_SOURCE_PORT",
		ini->ReadInteger("COMMUNICATION", "BCR_PORT", BaseForm->config.bcrPort[0]));
	BaseForm->config.bcrIp[1] = ini->ReadString("COMMUNICATION", "BCR_TARGET_IP", BaseForm->config.bcrIp[1]);
	BaseForm->config.bcrPort[1] = ini->ReadInteger("COMMUNICATION", "BCR_TARGET_PORT", BaseForm->config.bcrPort[1]);
	BaseForm->config.smokePort = ini->ReadString("COMMUNICATION", "SMOKE_PORT", BaseForm->config.smokePort);
	BaseForm->config.smokeId = ini->ReadInteger("COMMUNICATION", "SMOKE_ID", BaseForm->config.smokeId);
	BaseForm->config.smokeMode = ini->ReadInteger("COMMUNICATION", "SMOKE_MODE", BaseForm->config.smokeMode);
	BaseForm->config.smokeBaudRate = ini->ReadInteger("COMMUNICATION", "SMOKE_BAUDRATE", BaseForm->config.smokeBaudRate);

	delete ini;
	return true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::InitBarcodeAndSmoke()
{
	for(int i = 0; i < 2; ++i) {
		if(comBcr[i] == NULL) {
			comBcr[i] = new TMod_Bcr(this);
			comBcr[i]->Tag = i;
		}

		AnsiString ip = BaseForm->config.bcrIp[i];
		int port = BaseForm->config.bcrPort[i];
		if(!ip.IsEmpty() && port > 0)
			comBcr[i]->Connect(ip, port);
	}

	if(comSmoke[0] == NULL)
		comSmoke[0] = new TSmokeDetector(this);

	if(!BaseForm->config.smokePort.IsEmpty())
		comSmoke[0]->CommOpen(BaseForm->config.smokePort, 0,
			BaseForm->config.smokeId,
			BaseForm->config.smokeMode,
			BaseForm->config.smokeBaudRate);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CmdTrayOut(int pos)
{
	Sleep(3000);

    if(pos == 0){
		NotifyEquipStatus("IDLE");

		// Legacy SRC_EMP omitted: no corresponding ModPLC_BIN word in D10150-D10158.

		if(PlcBin != NULL) PlcBin->CmdSourceTrayOut(true);
		if(PlcBin != NULL) PlcBin->CmdSourceCenteringRequest(false);
	}else{
		//* 불량트레이 관리
		// 배출 명령만으로 파일을 지우지 않고 D10106 OFF(실물 배출)를 확인한 뒤 삭제한다.
		targetTrayInfoDeletePending = true;
		targetTrayInfoWasCentered = IsTargetCenteringSignal();
		if(targetTrayInfoActiveId.IsEmpty())
			targetTrayInfoActiveId = pTrayid_target->Caption.Trim();
		if(PlcBin != NULL) PlcBin->CmdTargetTrayOut(true);
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::targetGridDrawCell(TObject *Sender, int ACol,
	  int ARow, TRect &Rect, TGridDrawState State)
{

	targetGrid->Canvas->Font = targetGrid->Font; // 지정하지 않으면 System 폰트가 됨
	if( State.Contains(gdSelected)){
		targetGrid->Canvas->Brush->Color = clHighlight;
		targetGrid->Canvas->Font->Color = clHighlightText;
	}else{
		targetGrid->Canvas->Brush->Color = color_target[ACol][ARow];
	}
	targetGrid->Canvas->FillRect(Rect);

	DrawText(targetGrid->Canvas->Handle, targetGrid->Cells[ACol][ARow].c_str(), -1, &Rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::pause_startBtnClick(TObject *Sender)
{
	if(equipMode == modeAuto)
	{
		if(gripper->pauseStatus && robostar->pauseStatus)
		{
			// TrayLoad ON-timeout cancels the old request and stops its timer.
			// Restart restores that exact Source/Target transaction.
			for(int i = 0; i < 2; ++i)
			{
				if(!opcTrayLoadRetryRequired[i])
					continue;

				bool sourceTray = (i == 0);
				int stepNo = sourceTray ? 2 : 5;
				AnsiString locationName = sourceTray ? "Location1" : "Location2";
				int currentResponse = MesOpc != NULL ?
					MesOpc->TRAY_LOAD_RESPONSE_VALUE(sourceTray) : -1;

				opcTrayLoadPending[i] = true;
				opcTrayLoadWaitResponseOff[i] = false;
				opcTrayLoadResponseOffError[i] = false;
				opcTrayLoadRetryRequired[i] = false;
				opcTrayDisplayed[i] = false;
				opcTrayLoaded[i] = false;
				opcTrayLoadStartTick[i] = GetTickCount();
				lastProcessWaitStatus[stepNo - 1] = "";

				if(currentResponse == 1 || currentResponse == 2)
				{
					// The operator/FMS corrected Response while the timer was stopped.
					// Keep the original response revision baseline and consume it now.
					ProcessStepLog(stepNo, "RESTART RESUME / " + locationName +
						".TrayLoadResponse CURRENT=" + IntToStr(currentResponse) +
						" / EXPECTED=1 or 2 (RESULT)");
				}
				else
				{
					// No result is available yet: issue a fresh request and wait again.
					if(MesOpc != NULL)
						MesOpc->TRAY_LOAD_REQUEST(sourceTray);
					ProcessStepLog(stepNo, "RESTART RETRY / " + locationName +
						".TrayLoad Request=ON / CURRENT Response=" +
						IntToStr(currentResponse) + " / EXPECTED=1 or 2 (RESULT)");
				}
			}

			opcMesTimer->Enabled = true;
			if(ErrorForm != NULL)
				ErrorForm->Visible = false;
			gripper->req_Pause(false);
			robostar->req_Pause(false);
		}
	}
	else ShowMessage(BaseForm->GetLangStr("MSG_START_ALARM"));
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::pause_stopBtnClick(TObject *Sender)
{
	gripper->req_Pause(true);
	robostar->req_Pause(true);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::teachingBtnClick(TObject *Sender)
{
	teachForm->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::btnScanTargetTrayClick(TObject *Sender)
{
	pTrayid_source->Caption = BaseForm->GetLangStr("MSG_SCANNING");
	comBcr[0]->GetBarcode();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::btnScanSourceTrayClick(TObject *Sender)
{
	pTrayid_target->Caption = BaseForm->GetLangStr("MSG_SCANNING");
	ReadTargetTrayBarcode();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ReadTargetTrayBarcode()
{
	// Cycle test bypasses the unconfigured Target/NG tray barcode reader.
	if(cbCycle != NULL && cbCycle->Checked){
		memoMainLineAdd("[CYCLE TEST] Target tray barcode reader bypass: NG00001");
		setBarcode(1, "NG00001");
		return;
	}

	if(comBcr[1] != NULL)
		comBcr[1]->GetBarcode();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::setBarcode(int pos, AnsiString strBcr)
{
	memoMainLineAdd(BaseForm->GetLangStr("MSG_COMPLETE_SCAN") + " : " + strBcr);
	if(strBcr.Length() == 7)
	{
		switch(pos){
			case 0:
				this->pTrayid_source->Caption = strBcr;
				ProcessStepLog(1, "Tray Exist=ON / Tray ID=" + strBcr);
				CompleteProcessStep(1, "Tray ID=" + strBcr);
				if(IsSourceTrayInSignal()){
					NotifyTrayInfo(strBcr, true);
				}
				break;
			case 1:
				this->pTrayid_target->Caption = strBcr;
				ProcessStepLog(4, "Tray Exist=ON / Tray ID=" + strBcr);
				CompleteProcessStep(4, "Tray ID=" + strBcr);
				if(IsTargetCenteringSignal())NotifyTrayInfo(strBcr, false);
				break;
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::EnableButton_auto(bool benable)
{
	// 자동상태  true인 것들
	playBtn->Enabled = benable;
	stopBtn->Enabled = benable;

	// 자동상태 false인 것들
	openBtn->Enabled = !benable;
	trayout_srcBtn->Enabled = !benable;
	trayout_targetBtn->Enabled = !benable;
	teachingBtn->Enabled = !benable;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::mesTimerTimer(TObject *Sender)
{
	ErrorForm_mes->ShowError(tx->LOT_ID, "MES No response", tx->errMsg);
	mesTimer->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::opcMesTimerTimer(TObject *Sender)
{
	const DWORD RESPONSE_TIMEOUT_MS = 10000;
	DWORD nowTick = GetTickCount();

	for (int i = 0; i < 2; ++i)
	{
		if (!opcTrayLoadPending[i])
			continue;

		bool sourceTray = (i == 0);
		AnsiString trayName = sourceTray ? "Source" : "Target";
		AnsiString locationName = sourceTray ? "Location1" : "Location2";
		int stepNo = sourceTray ? 2 : 5;

		// Four-phase TrayLoad handshake:
		// Request ON -> Response ON -> Request OFF -> Response OFF -> complete.
		if (opcTrayLoadWaitResponseOff[i])
		{
			int response = MesOpc != NULL ?
				MesOpc->TRAY_LOAD_RESPONSE_VALUE(sourceTray) : -1;
			SetProcessWaitStatus(stepNo, locationName + ".TrayLoad Request=OFF",
				locationName + ".TrayLoadResponse OFF", response);

			if (response == 0)
			{
				// A Response OFF is valid only after Response ON data was displayed.
				if(!opcTrayDisplayed[i])
				{
					opcTrayLoadPending[i] = false;
					opcTrayLoadWaitResponseOff[i] = false;
					ProcessStepLog(stepNo, "ERROR - Response=0 without tray display completion");
					ShowCommonError(trayName + " tray load sequence error",
						"TrayLoadResponse=1 and tray display were not completed.");
					continue;
				}

				// After an OFF-timeout, do not advance merely because FMS was
				// corrected. The operator confirms recovery with FormMain Restart.
				if (opcTrayLoadResponseOffError[i] &&
					gripper != NULL && robostar != NULL &&
					(gripper->pauseStatus || robostar->pauseStatus))
					continue;

				opcTrayLoadPending[i] = false;
				opcTrayLoadWaitResponseOff[i] = false;
				opcTrayLoadResponseOffError[i] = false;
				opcTrayLoadRetryRequired[i] = false;
				ProcessStepLog(stepNo, locationName +
					".TrayLoadResponse=0 received / advance to next process");
				AdvanceOpcTrayLoad(sourceTray);
				continue;
			}

			if ((DWORD)(GetTickCount() - opcTrayLoadStartTick[i]) >= RESPONSE_TIMEOUT_MS)
			{
				if (!opcTrayLoadResponseOffError[i])
				{
					opcTrayLoadResponseOffError[i] = true;
					if (MesOpc != NULL)
						MesOpc->LogTrayLoadResponseOffTimeout(sourceTray);
					ProcessStepLog(stepNo,
						"ERROR - TrayLoadResponse reset timeout / EXPECTED=0 (RESET) / CURRENT=" +
						IntToStr(response));
					ShowCommonError(trayName +
						" tray TrayLoadResponse OFF timeout",
						"EXPECTED=0 (RESET), but CURRENT=" + IntToStr(response) +
						" remained for 10 seconds.");
				}
				else if (gripper != NULL && robostar != NULL &&
					!gripper->pauseStatus && !robostar->pauseStatus)
				{
					// Restart was pressed before FMS cleared the response.
					ShowCommonError(trayName +
						" tray TrayLoadResponse is still ON",
						"Clear TrayLoadResponse, then press Restart again.");
				}
			}
			continue;
		}

		int rawResponse = MesOpc != NULL ? MesOpc->TRAY_LOAD_RESPONSE_VALUE(sourceTray) : -1;
		int response = MesOpc != NULL ? MesOpc->TRAY_LOAD_RESPONSE(sourceTray) : -1;
		SetProcessWaitStatus(stepNo, locationName + ".TrayLoad Request=ON",
			locationName + ".TrayLoadResponse ON", rawResponse);
		if(response == 3)
		{
			opcTrayLoadStartTick[i] = nowTick;
			ProcessStepLog(stepNo, "Stale TrayLoadResponse cleared / actual Request=ON");
		}
		else if (response == 1)
		{
			// Display the validated tray immediately when Response turns ON.
			// The next process is still blocked until the same Response returns OFF.
			DisplayOpcTrayLoad(sourceTray);
			if(!opcTrayDisplayed[i])
			{
				opcTrayLoadPending[i] = false;
				ProcessStepLog(stepNo, "ERROR - TrayLoadResponse=1 but tray display failed");
				ShowCommonError(trayName + " tray display failed",
					"Tray data was not displayed. Target process is blocked.");
				continue;
			}
			opcTrayLoadWaitResponseOff[i] = true;
			opcTrayLoadResponseOffError[i] = false;
			opcTrayLoadStartTick[i] = nowTick;
			ProcessStepLog(stepNo, locationName +
				".TrayLoadResponse=1 received / tray displayed / " +
				locationName + ".TrayLoad=OFF / WAIT TrayLoadResponse=0");
		}
		else if (response == 2 || response < 0)
		{
			opcTrayLoadPending[i] = false;
			opcTrayLoadWaitResponseOff[i] = false;
			opcTrayLoadResponseOffError[i] = false;
			ProcessStepLog(stepNo, "ERROR - TrayLoadResponse=" + IntToStr(response));
			ShowCommonError(trayName + " tray load failed",
				"Check FMS TrayLoadResponse and tray information.");
		}
		else if ((DWORD)(GetTickCount() - opcTrayLoadStartTick[i]) >= RESPONSE_TIMEOUT_MS)
		{
			opcTrayLoadPending[i] = false;
			opcTrayLoadWaitResponseOff[i] = false;
			opcTrayLoadResponseOffError[i] = false;
			opcTrayLoadRetryRequired[i] = true;
			if (MesOpc != NULL){
				MesOpc->LogTrayLoadTimeout(sourceTray);
				MesOpc->TRAY_LOAD_CANCEL(sourceTray);
			}
			ProcessStepLog(stepNo, "ERROR - TrayLoadResponse result timeout / " +
				AnsiString("EXPECTED=1 or 2 (RESULT) / CURRENT=") + IntToStr(rawResponse));
			ShowCommonError(trayName + " tray response ON timeout",
				"EXPECTED=1 or 2 (RESULT), but CURRENT=" + IntToStr(rawResponse) +
				" or tray data was not completed within 10 seconds. Correct it, then press Restart.");
		}
	}
	if (opcProcessStartPending)
	{
		if(opcProcessStartWaitResponseOff)
		{
			int response = MesOpc != NULL ? MesOpc->PROCESS_START_RESPONSE_VALUE() : -1;
			SetProcessWaitStatus(6, "ProcessStart Request=OFF", "ProcessStartResponse OFF", response);
			if(response == 0)
			{
				bool paused = gripper != NULL && robostar != NULL &&
					(gripper->pauseStatus || robostar->pauseStatus);
				if(!(opcProcessStartResponseOffError && paused))
				{
					int result = opcProcessStartResponseResult;
					opcProcessStartPending = false;
					opcProcessStartWaitResponseOff = false;
					opcProcessStartResponseOffError = false;
					opcProcessStartResponseResult = 0;
					if(result == 1){
						opcProcessStarted = true;
						pwork1->Color = clLime;
						pwork2->Color = clLime;
						CompleteProcessStep(6, "ProcessStartResponse returned OFF");
						memoMainLineAdd("[FMS OPC UA] ProcessStart four-phase handshake complete.");
						if(gripper->seq == seqIdle && robostar->seq == seqIdle)
							gripper->req_Init();
					}else{
						ProcessStepLog(6, "ERROR - ProcessStartResponse=2 / clear complete");
						ShowCommonError("Process start failed", "FMS returned ProcessStartResponse=2.");
					}
				}
			}
			else if((DWORD)(GetTickCount() - opcProcessStartTick) >= RESPONSE_TIMEOUT_MS)
			{
				if(!opcProcessStartResponseOffError){
					opcProcessStartResponseOffError = true;
					if(MesOpc != NULL) MesOpc->LogProcessStartResponseOffTimeout();
					ProcessStepLog(6, "ERROR - ProcessStartResponse OFF timeout");
					ShowCommonError("ProcessStartResponse OFF timeout",
						"ProcessStart is OFF, but ProcessStartResponse stayed ON for 10 seconds.");
				}else if(gripper != NULL && robostar != NULL &&
					!gripper->pauseStatus && !robostar->pauseStatus){
					ShowCommonError("ProcessStartResponse is still ON",
						"Clear ProcessStartResponse, then press Restart again.");
				}
			}
		}
		else
		{
			int response = MesOpc != NULL ? MesOpc->PROCESS_START_RESPONSE_RESULT() : -1;
			SetProcessWaitStatus(6, "ProcessStart Request=ON", "ProcessStartResponse ON", response);
			if(response == 3){
				opcProcessStartTick = nowTick;
				ProcessStepLog(6, "Stale ProcessStartResponse cleared / actual Request=ON");
			}else if(response == 1 || response == 2){
				opcProcessStartWaitResponseOff = true;
				opcProcessStartResponseOffError = false;
				opcProcessStartResponseResult = response;
				opcProcessStartTick = nowTick;
				ProcessStepLog(6, "ProcessStartResponse=" + IntToStr(response) +
					" / Request=OFF / wait Response=0");
			}else if(response < 0){
				opcProcessStartPending = false;
				ProcessStepLog(6, "ERROR - invalid ProcessStartResponse=" + IntToStr(response));
				ShowCommonError("Process start failed", "Invalid or missing ProcessStartResponse.");
			}else if((DWORD)(GetTickCount() - opcProcessStartTick) >= RESPONSE_TIMEOUT_MS){
				opcProcessStartPending = false;
				ProcessStepLog(6, "ERROR - ProcessStartResponse ON timeout");
				if(MesOpc != NULL) MesOpc->PROCESS_START_CANCEL();
				ShowCommonError("Process start response ON timeout",
					"No ProcessStartResponse from FMS Gateway within 10 seconds.");
			}
		}
	}

	if(opcProcessEndPending)
	{
		if(opcProcessEndWaitResponseOff)
		{
			int response = MesOpc != NULL ? MesOpc->PROCESS_END_RESPONSE_VALUE() : -1;
			SetProcessWaitStatus(14, "ProcessEnd Request=OFF", "ProcessEndResponse OFF", response);
			if(response == 0)
			{
				bool paused = gripper != NULL && robostar != NULL &&
					(gripper->pauseStatus || robostar->pauseStatus);
				if(!(opcProcessEndResponseOffError && paused))
				{
					int result = opcProcessEndResponseResult;
					opcProcessEndPending = false;
					opcProcessEndWaitResponseOff = false;
					opcProcessEndResponseOffError = false;
					opcProcessEndResponseResult = 0;
					opcProcessStarted = false;
					if(result == 1){
						CompleteProcessStep(14, "ProcessEndResponse returned OFF");
						BeginProcessStep(15, "D10155 Source Tray Out request");
						CmdTrayOut(0);
						CompleteProcessStep(15, "D10155=ON");
						memoMainLineAdd("[FMS OPC UA] ProcessEnd four-phase handshake complete.");
					}else{
						ProcessStepLog(14, "ERROR - ProcessEndResponse=2 / clear complete");
						ShowCommonError("Source ProcessEnd failed", "FMS returned ProcessEndResponse=2.");
					}
				}
			}
			else if((DWORD)(GetTickCount() - opcProcessEndTick) >= RESPONSE_TIMEOUT_MS)
			{
				if(!opcProcessEndResponseOffError){
					opcProcessEndResponseOffError = true;
					if(MesOpc != NULL) MesOpc->LogProcessEndResponseOffTimeout();
					ProcessStepLog(14, "ERROR - ProcessEndResponse OFF timeout");
					ShowCommonError("ProcessEndResponse OFF timeout",
						"ProcessEnd is OFF, but ProcessEndResponse stayed ON for 10 seconds.");
				}else if(gripper != NULL && robostar != NULL &&
					!gripper->pauseStatus && !robostar->pauseStatus){
					ShowCommonError("ProcessEndResponse is still ON",
						"Clear ProcessEndResponse, then press Restart again.");
				}
			}
		}
		else
		{
			int response = MesOpc != NULL ? MesOpc->PROCESS_END_RESPONSE_RESULT() : -1;
			SetProcessWaitStatus(14, "ProcessEnd Request=ON", "ProcessEndResponse ON", response);
			if(response == 3){
				opcProcessEndTick = nowTick;
				ProcessStepLog(14, "Stale ProcessEndResponse cleared / actual Request=ON");
			}else if(response == 1 || response == 2){
				opcProcessEndWaitResponseOff = true;
				opcProcessEndResponseOffError = false;
				opcProcessEndResponseResult = response;
				opcProcessEndTick = nowTick;
				ProcessStepLog(14, "ProcessEndResponse=" + IntToStr(response) +
					" / Request=OFF / wait Response=0");
			}else if(response < 0){
				opcProcessEndPending = false;
				opcProcessStarted = false;
				ProcessStepLog(14, "ERROR - invalid ProcessEndResponse=" + IntToStr(response));
				ShowCommonError("Source ProcessEnd failed", "Invalid or missing ProcessEndResponse.");
			}else if((DWORD)(GetTickCount() - opcProcessEndTick) >= RESPONSE_TIMEOUT_MS){
				opcProcessEndPending = false;
				opcProcessStarted = false;
				ProcessStepLog(14, "ERROR - ProcessEndResponse ON timeout");
				if(MesOpc != NULL){
					MesOpc->LogProcessEndTimeout();
					MesOpc->PROCESS_END_CANCEL();
				}
				ShowCommonError("Source ProcessEnd response ON timeout",
					"No ProcessEndResponse from FMS Gateway within 10 seconds.");
			}
		}
	}

	if(opcCellTrackOutPending)
	{
		if(opcCellTrackOutWaitResponseOff)
		{
			int response = MesOpc != NULL ? MesOpc->CELL_TRACK_OUT_RESPONSE_VALUE() : -1;
			SetProcessWaitStatus(12, "CellUnloadComplete=OFF", "CellUnloadCompleteResponse OFF", response);
			if(response == 0)
			{
				bool paused = gripper != NULL && robostar != NULL &&
					(gripper->pauseStatus || robostar->pauseStatus);
				if(!(opcCellTrackOutResponseOffError && paused))
				{
					int result = opcCellTrackOutResponseResult;
					opcCellTrackOutPending = false;
					opcCellTrackOutWaitResponseOff = false;
					opcCellTrackOutResponseOffError = false;
					opcCellTrackOutResponseResult = 0;
					if(result == 1){
						CompleteProcessStep(12, "CellUnloadCompleteResponse returned OFF");
						memoMainLineAdd("[FMS OPC UA] CellTrackOut four-phase handshake complete.");
					}else{
						ProcessStepLog(12, "ERROR - CellUnloadCompleteResponse=2 / clear complete");
						ShowCommonError("CellTrackOut failed", "FMS returned CellUnloadCompleteResponse=2.");
					}
				}
			}
			else if((DWORD)(GetTickCount() - opcCellTrackOutStartTick) >= RESPONSE_TIMEOUT_MS)
			{
				if(!opcCellTrackOutResponseOffError){
					opcCellTrackOutResponseOffError = true;
					if(MesOpc != NULL) MesOpc->LogCellTrackOutResponseOffTimeout();
					ProcessStepLog(12, "ERROR - CellUnloadCompleteResponse OFF timeout");
					ShowCommonError("CellUnloadCompleteResponse OFF timeout",
						"CellUnloadComplete is OFF, but its response stayed ON for 10 seconds.");
				}else if(gripper != NULL && robostar != NULL &&
					!gripper->pauseStatus && !robostar->pauseStatus){
					ShowCommonError("CellUnloadCompleteResponse is still ON",
						"Clear the response, then press Restart again.");
				}
			}
		}
		else
		{
			int response = MesOpc != NULL ? MesOpc->CELL_TRACK_OUT_RESPONSE_RESULT() : -1;
			SetProcessWaitStatus(12, "CellUnloadComplete=ON", "CellUnloadCompleteResponse ON", response);
			if(response == 3){
				opcCellTrackOutStartTick = nowTick;
				ProcessStepLog(12, "Stale CellUnloadCompleteResponse cleared / actual Request=ON");
			}else if(response == 1 || response == 2){
				opcCellTrackOutWaitResponseOff = true;
				opcCellTrackOutResponseOffError = false;
				opcCellTrackOutResponseResult = response;
				opcCellTrackOutStartTick = nowTick;
				ProcessStepLog(12, "CellUnloadCompleteResponse=" + IntToStr(response) +
					" / Request=OFF / wait Response=0");
			}else if(response < 0){
				opcCellTrackOutPending = false;
				ProcessStepLog(12, "ERROR - invalid CellUnloadCompleteResponse=" + IntToStr(response));
				ShowCommonError("CellTrackOut failed", "Invalid or missing CellUnloadCompleteResponse.");
			}else if((DWORD)(GetTickCount() - opcCellTrackOutStartTick) >= RESPONSE_TIMEOUT_MS){
				opcCellTrackOutPending = false;
				ProcessStepLog(12, "ERROR - CellUnloadCompleteResponse ON timeout");
				if(MesOpc != NULL){
					MesOpc->LogCellTrackOutTimeout();
					MesOpc->CELL_TRACK_OUT_CANCEL();
				}
				ShowCommonError("CellTrackOut response ON timeout",
					"No CellUnloadCompleteResponse from FMS Gateway within 10 seconds.");
			}
		}
	}

	if(opcTargetUnloadPending)
	{
		if(opcTargetUnloadWaitResponseOff)
		{
			int response = MesOpc != NULL ? MesOpc->TRAY_UNLOAD_RESPONSE_VALUE() : -1;
			SetProcessWaitStatus(16, "TrayUnloadRequest=OFF", "TrayUnloadResponse OFF", response);
			if(response == 0)
			{
				bool paused = gripper != NULL && robostar != NULL &&
					(gripper->pauseStatus || robostar->pauseStatus);
				if(!(opcTargetUnloadResponseOffError && paused))
				{
					int result = opcTargetUnloadResponseResult;
					opcTargetUnloadPending = false;
					opcTargetUnloadWaitResponseOff = false;
					opcTargetUnloadResponseOffError = false;
					opcTargetUnloadResponseResult = 0;
					if(result == 1){
						CompleteProcessStep(16, "TrayUnloadResponse returned OFF");
						CmdTrayOut(1);
						memoMainLineAdd("[FMS OPC UA] TrayUnload four-phase handshake complete.");
					}else{
						ProcessStepLog(16, "ERROR - TrayUnloadResponse=2 / clear complete");
						ShowCommonError("Target tray unload failed", "FMS returned TrayUnloadResponse=2.");
					}
				}
			}
			else if((DWORD)(GetTickCount() - opcTargetUnloadTick) >= RESPONSE_TIMEOUT_MS)
			{
				if(!opcTargetUnloadResponseOffError){
					opcTargetUnloadResponseOffError = true;
					if(MesOpc != NULL) MesOpc->LogTrayUnloadResponseOffTimeout();
					ProcessStepLog(16, "ERROR - TrayUnloadResponse OFF timeout");
					ShowCommonError("TrayUnloadResponse OFF timeout",
						"TrayUnloadRequest is OFF, but TrayUnloadResponse stayed ON for 10 seconds.");
				}else if(gripper != NULL && robostar != NULL &&
					!gripper->pauseStatus && !robostar->pauseStatus){
					ShowCommonError("TrayUnloadResponse is still ON",
						"Clear TrayUnloadResponse, then press Restart again.");
				}
			}
		}
		else
		{
			int response = MesOpc != NULL ? MesOpc->TRAY_UNLOAD_RESPONSE_RESULT() : -1;
			SetProcessWaitStatus(16, "TrayUnloadRequest=ON", "TrayUnloadResponse ON", response);
			if(response == 3){
				opcTargetUnloadTick = nowTick;
				ProcessStepLog(16, "Stale TrayUnloadResponse cleared / actual Request=ON");
			}else if(response == 1 || response == 2){
				opcTargetUnloadWaitResponseOff = true;
				opcTargetUnloadResponseOffError = false;
				opcTargetUnloadResponseResult = response;
				opcTargetUnloadTick = nowTick;
				ProcessStepLog(16, "TrayUnloadResponse=" + IntToStr(response) +
					" / Request=OFF / wait Response=0");
			}else if(response < 0){
				opcTargetUnloadPending = false;
				ProcessStepLog(16, "ERROR - invalid TrayUnloadResponse=" + IntToStr(response));
				ShowCommonError("Target tray unload failed", "Invalid or missing TrayUnloadResponse.");
			}else if((DWORD)(GetTickCount() - opcTargetUnloadTick) >= RESPONSE_TIMEOUT_MS){
				opcTargetUnloadPending = false;
				ProcessStepLog(16, "ERROR - TrayUnloadResponse ON timeout");
				if(MesOpc != NULL){
					MesOpc->LogTrayUnloadTimeout();
					MesOpc->TRAY_UNLOAD_CANCEL();
				}
				ShowCommonError("Target tray unload response ON timeout",
					"No TrayUnloadResponse from FMS Gateway within 10 seconds.");
			}
		}
	}

	bool pending = opcProcessStartPending || opcProcessEndPending ||
		opcCellTrackOutPending || opcTargetUnloadPending;
	for (int i = 0; i < 2; ++i)
		pending = pending || opcTrayLoadPending[i];
	opcMesTimer->Enabled = pending;
}
//---------------------------------------------------------------------------

bool __fastcall TMainForm::CheckServoAutoReady(bool showError)
{
	// Read controller status at the AUTO transition; UI colors are display only.
	// m_ServoHome includes origin completion and the physical XYZ=0 wait position.
	robostar->RestoreServoState();
	bool servoOpenReady = m_ServoOpen;
	bool servoOnReady = m_ServoON;
	bool servoHomeReady = m_ServoHome;
	bool gripperOpenReady = robostar->getGripperOpenStatus();
	// X0022 is active-low: AUTO is allowed only when no cell remains in the gripper.
	bool gripperCellClear = !robostar->getCellDetectStatus();

	if(servoOpenReady && servoOnReady && servoHomeReady && gripperOpenReady && gripperCellClear)
		return true;

	UnicodeString detail = L"Servo OPEN  : " + UnicodeString(servoOpenReady ? L"OK" : L"NOT COMPLETE") + L"\r\n";
	detail += L"Servo ON      : " + UnicodeString(servoOnReady ? L"OK" : L"NOT COMPLETE") + L"\r\n";
	detail += L"Servo HOME    : " + UnicodeString(servoHomeReady ? L"OK" : L"NOT COMPLETE") + L"\r\n";
	detail += L"Gripper OPEN  : " + UnicodeString(gripperOpenReady ? L"OK" : L"NOT COMPLETE") + L"\r\n";
	detail += L"Gripper CELL  : " + UnicodeString(gripperCellClear ? L"CLEAR" : L"DETECTED - REMOVE CELL") + L"\r\n";
	detail += L"Confirm X0021 OPEN=ON, X0020 CHUCK=OFF, and X0022 CELL DETECT=ON before AUTO.";

	memoRobostarLineAdd("[AUTO INTERLOCK] OPEN=" + IntToStr(servoOpenReady ? 1 : 0) +
		", ON=" + IntToStr(servoOnReady ? 1 : 0) +
		", HOME=" + IntToStr(servoHomeReady ? 1 : 0) +
		", GRIPPER_OPEN=" + IntToStr(gripperOpenReady ? 1 : 0) +
		", CELL_CLEAR=" + IntToStr(gripperCellClear ? 1 : 0) +
		" (X0021=" + IntToStr(robostar->input.GRIPPER1_UNCHUCK ? 1 : 0) +
		", X0020=" + IntToStr(robostar->input.GRIPPER1_CHUCK ? 1 : 0) +
		", X0022=" + IntToStr(robostar->input.GRIPPER1_CELL_DETECT ? 1 : 0) + ")");
	if(showError)
		AlarmForm->ShowError(BaseForm->GetLangStr("MSG_AUTO_ALARM1"), detail);
	return false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::autoBtnClick(TObject *Sender)
{
	if(!CheckServoAutoReady(true)){
		autoBtn->Down = false;
		return;
	}

	int servo_speed = teachForm->speedEdit->Text.ToInt();
	int servo_accl_speed = teachForm->acclSpeedEdit->Text.ToInt();
	int servo_dccl_speed = teachForm->dcclSpeedEdit->Text.ToInt();
	robostar->req_Speed(servo_speed, servo_accl_speed, servo_dccl_speed);
	if(manualBtn->Down == true){
		equipMode = modeAutoStop;
		autoBtn->Down = true;
		manualBtn->Down = false;
		playBtn->Down = false;
		stopBtn->Down = true;
		EnableButton_auto(true);
	}else{
		autoBtn->Down = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::manualBtnClick(TObject *Sender)
{
	if(autoBtn->Down == true){
		if(robostar->IsSafetyDoorOpen(1) || robostar->IsSafetyDoorOpen(2) || robostar->IsEmergencyStopActive())
		{
			if(PlcBin != NULL) PlcBin->CmdSourceCenteringRequest(IsSourceCenteringSignal());

			equipMode = modeManual;
			nowLampMode = LampManual;
			autoBtn->Down = false;
			manualBtn->Down = true;
			EnableButton_auto(false);
		}
		else
		{
			if(MessageBox(Handle, BaseForm->GetLangStr("MSG_MANUAL_ALARM").c_str(), L"Manual", MB_YESNO|MB_ICONQUESTION) == ID_YES){
				gripper->req_Pause(true);
				robostar->req_Pause(true);

				if(PlcBin != NULL) PlcBin->CmdSourceCenteringRequest(IsSourceCenteringSignal());

				equipMode = modeManual;
                nowLampMode = LampManual;
				autoBtn->Down = false;
				manualBtn->Down = true;
				EnableButton_auto(false);

			}else{
				autoBtn->Down = true;
				manualBtn->Down = false;
			}
		}
	}else{
		manualBtn->Down = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::playBtnClick(TObject *Sender)
{
	// Servo OPEN/ON/HOME is checked only when entering AUTO mode.
	// START resumes the paused sequence without re-running the AUTO interlock.
	equipMode = modeAuto;
	nowLampMode = LampAuto;
	playBtn->Down = true;
	stopBtn->Down = false;
   
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::stopBtnClick(TObject *Sender)
{
	equipMode = modeAutoStop;
	nowLampMode = LampManual;
	playBtn->Down = false;
	stopBtn->Down = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::target_idEditKeyDown(TObject *Sender, WORD &Key,
	  TShiftState Shift)
{
	if(Key == VK_RETURN){
		pTrayid_target->Caption = target_idEdit->Text;
		NotifyTrayInfo(pTrayid_target->Caption, false);	// 대상 트레이 mes 보고
		target_idEdit->Visible = false;
	}
	else if(Key == VK_ESCAPE){
		target_idEdit->Visible = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::pTrayid_sourceDblClick(TObject *Sender)
{
	src_idEdit->Text = pTrayid_source->Caption;
	src_idEdit->Visible = true;
	src_idEdit->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::src_idEditKeyDown(TObject *Sender, WORD &Key,
	  TShiftState Shift)
{
	if(Key == VK_RETURN){
		pTrayid_source->Caption = src_idEdit->Text;
		NotifyTrayInfo(pTrayid_source->Caption, true);	// UF 트레이 mes 보고
		src_idEdit->Visible = false;
	}
	else if(Key == VK_ESCAPE){
    	src_idEdit->Visible = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::pTrayid_targetDblClick(TObject *Sender)
{
	target_idEdit->Text = pTrayid_target->Caption;
	target_idEdit->Visible = true;
	target_idEdit->SetFocus();
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::buzzerBtnClick(TObject *Sender)
{
	BuzzerOn(false);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::openBtnClick(TObject *Sender)
{
	robostar->req_Init();
	if(gripper->seq == 4) gripper->step.step = 0;   //  seqPause
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::trayout_srcBtnClick(TObject *Sender)
{
	if(IsSourceTrayInSignal()){
		if(MessageBox(Handle, BaseForm->GetLangStr("MSG_EJECT_SOURCETRAY").c_str(), L"Tray Out", MB_YESNO|MB_ICONQUESTION) == ID_YES){
            if(PlcBin != NULL) PlcBin->CmdSourceCenteringRequest(false);
			CmdTrayOut(0);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::trayout_targetBtnClick(TObject *Sender)
{
	int reply;
	if(IsTargetCenteringSignal()){
		if(MessageBox(Handle, BaseForm->GetLangStr("MSG_EJECT_TARGETTRAY").c_str(), L"Tray Out", MB_YESNO|MB_ICONQUESTION) == ID_YES){
			reply = MessageBox(Handle, BaseForm->GetLangStr("MSG_MES_REQUEST").c_str(), L"MES", MB_YESNOCANCEL|MB_ICONQUESTION);
			if(reply == ID_YES){
				NotifyTransferOut(pTrayid_target->Caption);
			}else if(reply == ID_NO){
				CmdTrayOut(1);
			}

		}
	}
}
//---------------------------------------------------------------------------

bool __fastcall TMainForm::IsSourceTrayInSignal() const
{
	// MES TEST simulates D10103 even when the PLC socket is disconnected.
	return (cbMES != NULL && cbMES->Checked)
		|| (PlcBin != NULL && PlcBin->IsSourceTrayIn());
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::IsSourceCenteringSignal() const
{
	// MES TEST simulates D10104.
	return (cbMES != NULL && cbMES->Checked)
		|| (PlcBin != NULL && PlcBin->IsSourceCentering());
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::IsTargetTrayInSignal() const
{
	return PlcBin != NULL && PlcBin->IsTargetTrayIn();
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::IsTargetCenteringSignal() const
{
	// MES TEST simulates D10106 (bad/target tray centering).
	return (cbMES != NULL && cbMES->Checked)
		|| (PlcBin != NULL && PlcBin->IsTargetCentering());
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::InitStep(STEP *data)
{
	data->step = 0;
	data->cnt = 0;
	data->timeout = 0;
}
//---------------------------------------------------------------------------
// 원본
void __fastcall TMainForm::stepTimerTimer(TObject *Sender)
{
	if(equipMode != modeAuto){
		memoMainLineAdd(BaseForm->GetLangStr("MSG_AUTOMODE_WARNING"));
		return;
	}

	if(IsSourceTrayInSignal() == 0){
		opcTrayDisplayed[0] = false;
		opcTrayLoaded[0] = false;
		InitStep(&step[0]);
		if(currentProcessStep != 0) ResetProcessFlow();
	}
	if(IsTargetCenteringSignal() == 0)InitStep(&step[1]);

    if(!gripper->pauseStatus && !robostar->pauseStatus)
	{
		switch(step[0].step){
			case 0:
				if(IsSourceTrayInSignal()){
					BeginProcessStep(1, "Tray Exist=ON / waiting barcode");
					NotifyEquipStatus("PROCESS");
					if(chkBypass->Checked == false){
						pTrayid_source->Caption = "";
						pTrayid_source2->Caption = "";
						memoMainLineAdd(BaseForm->GetLangStr("MSG_SOURCETRAY_SCAN"));
						comBcr[0]->GetBarcode();	// 작업1. 선별 바코드 읽고  -> DisplayTrayInfo  	// test
						step[0].step += 1;
					}else{
						// AUTO + D10103 Tray In + BYPASS: cancel centering and eject.
						if(PlcBin != NULL)
							PlcBin->CmdSourceCenteringRequest(false); // D10154 OFF
						memoMainLineAdd(BaseForm->GetLangStr("MSG_BYPASS_TRAYOUT"));
						CmdTrayOut(0);                            // D10155 ON
						step[0].step += 100;
					}
				}
				else{
					// Tray loading is sequential: do not scan/load Target before the
					// Source TrayLoad handshake has returned Response to zero.
					// The process panel already shows WAITING FOR SOURCE TRAY.
				}
				break;
			case 1:
				// Barcode completion alone must never advance the PLC sequence.
				// Displayed requires Response=1; Loaded additionally requires Response=0.
				if(!opcTrayDisplayed[0] || !opcTrayLoaded[0])
					break;
				step[0].step = 2;
				break;
			case 2:
				if(IsTargetCenteringSignal()){
					BeginProcessStep(4, "D10106 Target Centering=ON / waiting barcode");
					memoMainLineAdd(BaseForm->GetLangStr("MSG_TARGETTRAY_CENTERING_COMPL"));
					pTrayid_target->Caption = "";       // test
					pTrayid_target2->Caption = "";      // test
					ReadTargetTrayBarcode();		// 작업4. 센터링이 되어 있으면 바코드를 읽고 -> DisplayTrayInfo 	// test
					step[0].step += 1;
					step[1].step = 1;
				}else{
					SetProcessWaitStatus(4, "Source TrayLoad complete",
						"D10106 Target Centering", 0);
				}
				break;
			default:
				break;
		}


		// ProcessStart is allowed only after both TrayLoad handshakes and Source
		// Centering are complete. Polling here has no log side effects.
		TryStartOpcProcess();

		 switch(step[1].step){
			case 0:
				if(IsTargetCenteringSignal() && pwork1->Color == clLime){
					memoMainLineAdd("More target trays arrived.");
					pTrayid_target->Caption = "";
					pTrayid_target2->Caption = "";
					ReadTargetTrayBarcode();		// 작업4. 센터링이 되어 있으면 바코드를 읽고 -> DisplayTrayInfo     // test
					step[1].step += 1;
				}
			default:
				break;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::chkBypassClick(TObject *Sender)
{
	// State only. The automatic sequence evaluates BYPASS after D10103 Tray In.
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::resetBtnClick(TObject *Sender)
{
	BuzzerOn(false);
	MainForm->LampModeChange(MainForm->beforeLampMode);
	robostar->req_Reset();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::CreateIoRow(TScrollBox *parent, TPanel **statePanel, int index, AnsiString address, AnsiString name)
{
	const TColor ioNoBackColor = (TColor)15269887;
	const TColor ioNoFontColor = (TColor)13996080;
	int rowLimit = (parent->Tag > 0) ? parent->Tag : 24;
	int col = index / rowLimit;
	int row = index % rowLimit;
	int left = 8 + (col * 278);
	int top = 8 + (row * 24);

	TPanel *addressPanel = new TPanel(parent);
	addressPanel->Parent = parent;
	addressPanel->Left = left;
	addressPanel->Top = top;
	addressPanel->Width = 54;
	addressPanel->Height = 22;
	addressPanel->BevelOuter = bvNone;
	addressPanel->Alignment = taCenter;
	addressPanel->Caption = address;
	addressPanel->Color = ioNoBackColor;
	addressPanel->ParentBackground = false;
	addressPanel->BevelKind = bkFlat;
	addressPanel->Font->Color = ioNoFontColor;
	addressPanel->Font->Style = TFontStyles() << fsBold;

	TPanel *statusPanel = new TPanel(parent);
	statusPanel->Parent = parent;
	statusPanel->Left = left + 58;
	statusPanel->Top = top;
	statusPanel->Width = 210;
	statusPanel->Height = 22;
	statusPanel->BevelKind = bkFlat;
	statusPanel->BevelOuter = bvNone;
	statusPanel->ParentBackground = false;
	statusPanel->Alignment = taLeftJustify;
	statusPanel->Caption = name;
	statusPanel->Color = clWhite;
	statusPanel->Font->Color = clBlack;
	statusPanel->Font->Style = TFontStyles() << fsBold;
	statePanel[index] = statusPanel;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CreateIoMonitoringPanel()
{
	for(int i = 0; i < 64; ++i) ioInputState[i] = NULL;
	for(int i = 0; i < 32; ++i) ioOutputState[i] = NULL;
	ioInputCount = 0;
	ioOutputCount = 0;


	grp_io = new TPanel(this);
	grp_io->Parent = this;
	grp_io->Left = 570;
	grp_io->Top = 92;
	grp_io->Width = 870;
	grp_io->Height = 720;
	grp_io->BevelKind = bkFlat;
	grp_io->Color = clWhite;
	grp_io->Visible = false;
	grp_io->ParentBackground = false;

	TPanel *title = new TPanel(grp_io);
	title->Parent = grp_io;
	title->Align = alTop;
	title->Height = 32;
	title->Caption = "I/O Monitoring";
	title->Color = clWhite;
	title->Font->Color = (TColor)13204573;
	title->Font->Style = TFontStyles() << fsBold;
	title->Font->Height = -17;

	TAdvSmoothButton *closeButton = new TAdvSmoothButton(grp_io);
	closeButton->Parent = grp_io;
	closeButton->Left = grp_io->Width - 82;
	closeButton->Top = 4;
	closeButton->Width = 72;
	closeButton->Height = 24;
	closeButton->Caption = "Close";
	closeButton->Color = clWhite;
	closeButton->Appearance->Font->Style = TFontStyles() << fsBold;
	closeButton->OnClick = btnCloseIoPanelClick;
	closeButton->BringToFront();

	TPanel *inputPanel = new TPanel(grp_io);
	inputPanel->Parent = grp_io;
	inputPanel->Left = 8;
	inputPanel->Top = 40;
	inputPanel->Width = 854;
	inputPanel->Height = 432;
	inputPanel->BevelKind = bkFlat;
	inputPanel->Color = clWhite;

	TPanel *inputTitle = new TPanel(inputPanel);
	inputTitle->Parent = inputPanel;
	inputTitle->Align = alTop;
	inputTitle->Height = 22;
	inputTitle->Caption = "INPUT";
	inputTitle->Color = clGray;
	inputTitle->ParentBackground = false;
	inputTitle->Font->Color = clWhite;
	inputTitle->Font->Style = TFontStyles() << fsBold;

	TScrollBox *scrInput = new TScrollBox(inputPanel);
	scrInput->Parent = inputPanel;
	scrInput->Left = 4;
	scrInput->Top = 26;
	scrInput->Width = 844;
	scrInput->Height = 398;
	scrInput->HorzScrollBar->Visible = false;
	scrInput->Color = clWhite;
	scrInput->Tag = 16;

	const char *inputNames[48] = {
		"CP01 TRIP", "CP02 TRIP", "CP03 TRIP", "CP04 TRIP", "CP05 TRIP", "CP06 TRIP", "CP07 TRIP", "CP08 TRIP",
		"CP09 TRIP", "CP10 SERVO1 TRIP", "CP11 SERVO2 TRIP", "CP12 SERVO3 TRIP", "CP13 BCR01 TRIP", "CP14 BCR02 TRIP", "MS01 TRIP", "",
		"SERVO01 INPOS", "SERVO01 ALARM", "SERVO01 OK HOME", "SERVO02 INPOS", "SERVO02 ALARM", "SERVO02 OK HOME", "SERVO03 INPOS", "SERVO03 ALARM",
		"SERVO03 OK HOME", "", "", "", "", "", "", "",
		"GRIPPER1 CHUCK", "GRIPPER1 UNCHUCK", "GRIPPER1 CELL DETECT", "X0023 UNUSED", "EMS NORMAL", "OPBOX RESET SWITCH", "SAFETY DOOR #1 UNLOCKED", "SAFETY DOOR #2 UNLOCKED",
		"SAFETY RESET SW ON", "BY-PASS S/W OFF", "BY-PASS S/W ON", "SAFETY EMG READY", "SAFETY DOOR READY", "", "", ""
	};
	for(int i = 0; i < 48; ++i){
		AnsiString address = "X" + IntToHex(i, 4);
		CreateIoRow(scrInput, ioInputState, ioInputCount, address, inputNames[i]);
		ioInputCount++;
	}

	TPanel *outputPanel = new TPanel(grp_io);
	outputPanel->Parent = grp_io;
	outputPanel->Left = 8;
	outputPanel->Top = 480;
	outputPanel->Width = 854;
	outputPanel->Height = 232;
	outputPanel->BevelKind = bkFlat;
	outputPanel->Color = clWhite;

	TPanel *outputTitle = new TPanel(outputPanel);
	outputTitle->Parent = outputPanel;
	outputTitle->Align = alTop;
	outputTitle->Height = 22;
	outputTitle->Caption = "OUTPUT";
	outputTitle->Color = clGray;
	outputTitle->ParentBackground = false;
	outputTitle->Font->Color = clWhite;
	outputTitle->Font->Style = TFontStyles() << fsBold;

	TScrollBox *scrOutput = new TScrollBox(outputPanel);
	scrOutput->Parent = outputPanel;
	scrOutput->Left = 4;
	scrOutput->Top = 26;
	scrOutput->Width = 844;
	scrOutput->Height = 204;
	scrOutput->HorzScrollBar->Visible = false;
	scrOutput->Color = clWhite;
	scrOutput->Tag = 6;

	const char *outputNames[16] = {
		"GRIPPER CHUCK SOL", "GRIPPER UNCHUCK SOL", "SAFETY RESET", "KEYLOCK LEFT",
		"KEYLOCK RIGHT", "OPBOX RESET LAMP", "SAFETY RESET SW LAMP", "OPBOX EMERGENCY LAMP",
		"TOWER LAMP RED", "TOWER LAMP YELLOW", "TOWER LAMP GREEN", "TOWER LAMP BUZZER",
		"BYPASS", "SAFETY BYPASS ON", "", ""
	};
	for(int i = 0; i < 16; ++i){
		AnsiString address = "Y" + IntToHex(0x0030 + i, 4);
		CreateIoRow(scrOutput, ioOutputState, ioOutputCount, address, outputNames[i]);
		ioOutputCount++;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::UpdateIoMonitoringPanel()
{
	if(grp_io == NULL) return;

	bool inputValue[48] = {
		robostar->input.CP01_TRIP, robostar->input.CP02_TRIP, robostar->input.CP03_TRIP, robostar->input.CP04_TRIP,
		robostar->input.CP05_TRIP, robostar->input.CP06_TRIP, robostar->input.CP07_TRIP, robostar->input.CP08_TRIP,
		robostar->input.CP09_TRIP, robostar->input.CP10_TRIP, robostar->input.CP11_TRIP, robostar->input.CP12_TRIP,
		robostar->input.CP13_TRIP, robostar->input.CP14_TRIP, robostar->input.MS01_TRIP, robostar->input.X000F,
		robostar->input.SERVO01_INPOS, robostar->input.SERVO01_ALARM, robostar->input.SERVO01_OK_HOME, robostar->input.SERVO02_INPOS,
		robostar->input.SERVO02_ALARM, robostar->input.SERVO02_OK_HOME, robostar->input.SERVO03_INPOS, robostar->input.SERVO03_ALARM,
		robostar->input.SERVO03_OK_HOME, robostar->input.X0019, robostar->input.X001A, robostar->input.X001B,
		robostar->input.X001C, robostar->input.X001D, robostar->input.X001E, robostar->input.X001F,
		robostar->input.GRIPPER1_CHUCK, robostar->input.GRIPPER1_UNCHUCK, robostar->input.GRIPPER1_CELL_DETECT, robostar->input.X0023_UNUSED,
		robostar->input.EMS_SWITCH, robostar->input.OPBOX_RESET_SWITCH, robostar->input.SAFETY_DOOR_1, robostar->input.SAFETY_DOOR_2,
		robostar->input.SAFETY_RESET_SW_ON, robostar->input.BYPASS_SW_OFF, robostar->input.BYPASS_SW_ON, robostar->input.SAFETY_EMG_READY,
		robostar->input.SAFETY_DOOR_READY, robostar->input.SAFETY_DOOR_3, robostar->input.X002E, robostar->input.X002F
	};

	bool outputValue[16] = {
		robostar->gripper.GRIPPER1_CHUCK, robostar->gripper.GRIPPER1_UNCHUCK, robostar->gripper.SAFETY_RESET, robostar->gripper.DOOR_LEFT_CLOSE,
		robostar->gripper.DOOR_RIGHT_CLOSE, robostar->gripper.OPBOX_RESET_LAMP, robostar->gripper.SAFETY_RESET_SW_LAMP, robostar->gripper.OPBOX_EMERGENCY_LAMP,
		robostar->gripper.TOWER_LAMP_RED, robostar->gripper.TOWER_LAMP_YELLOW, robostar->gripper.TOWER_LAMP_GREEN, robostar->gripper.TOWER_LAMP_BUZZER,
		robostar->gripper.DOOR_OPEN_SELECT, robostar->gripper.SAFETY_BYPASS_ON, robostar->gripper.Y003E, robostar->gripper.Y003F
	};

	for(int i = 0; i < ioInputCount; ++i){
		if(ioInputState[i] == NULL) continue;
		ioInputState[i]->Color = inputValue[i] ? clLime : clWhite;
	}
	for(int i = 0; i < ioOutputCount; ++i){
		if(ioOutputState[i] == NULL) continue;
		ioOutputState[i]->Color = outputValue[i] ? clLime : clWhite;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::btnIOMonitoringClick(TObject *Sender)
{
	UpdateIoMonitoringPanel();
	grp_io->BringToFront();
	grp_io->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::btnCloseIoPanelClick(TObject *Sender)
{
	grp_io->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::senTimerTimer(TObject *Sender)
{
	//* 불량트레이 관리
	// 배출 도중 D10106이 한 번이라도 ON이었고 OFF로 바뀌면 실물 배출 완료로 판단한다.
	bool targetCenteringNow = IsTargetCenteringSignal();
	if(targetTrayInfoDeletePending){
		if(targetCenteringNow)
			targetTrayInfoWasCentered = true;
		else if(targetTrayInfoWasCentered){
			ClearTargetTrayInfo();
			targetTrayInfoDeletePending = false;
			targetTrayInfoWasCentered = false;
			opcTrayLoaded[1] = false;
		}
	}

	for(int i=0; i<=3; ++i)GetZoneCount(i);

	// X0022 is active-low. This status panel shows logical cell presence,
	// while the I/O monitoring panel continues to show the physical X0022 state.
	sensorColor(pcell1, robostar->getCellDetectStatus());
	// X0023 vertical-cylinder/buffer input is not used on this machine.
	sensorColor(pflow1, false);
	sensorColor(popen1, robostar->input.GRIPPER1_UNCHUCK);
	// CLOSE follows the actual X0020 input and remains visible while Y0030 is commanded.
	sensorColor(pclose1, robostar->getGripperChuckStatus());
    //* 2026 07 24 MR-MC axis operation status
	bool servoRunning = false;
	if(robostar->IsSscOpened()){
		for(int i = 1; i <= servoCnt; ++i){
			if(robostar->mr2.running[i]){
				servoRunning = true;
				break;
			}
		}
	}
	if(servoRunning)
		pRun->Color = clLime;
	else
		pRun->Color = clGray;

	setLamp();

	UpdateIoMonitoringPanel();
	// Y003D follows a valid pair of BYPASS contacts; do not write CC-Link repeatedly.
	bool safetyBypassOn = robostar->IsBypassActive();
	if(robostar->gripper.SAFETY_BYPASS_ON != safetyBypassOn)
		robostar->Y003D(safetyBypassOn);

	//* 2026 08 07 천안 불량선별기와 동일하게 OPEN은 SSC 시스템 RUNNING 상태로 표시
	if(robostar->IsSscOpened() && robostar->mr2.system_status == SSC_STS_CODE_RUNNING)
		popen->Color = clLime;
	else popen->Color = clSilver;

	bool NGflag = false;
	//* 2026 08 07 천안 불량선별기와 동일하게 각 축의 실제 AX_RDY로 Servo ON 표시
	for(int i = 1; i <= servoCnt; ++i){
		if(robostar->IsSscOpened() && robostar->mr2.servo[i] == SSC_BIT_ON)
			status_on[i]->Color = clLime;
		else status_on[i]->Color = clSilver;
	}

	if(robostar->IsSscOpened() && robostar->mr2.system_detail == 0)
	{
		if(robostar->mr2.speed[Axis_x] < 0) robostar->mr2.speed[Axis_x] *= -1;
		if(robostar->mr2.speed[Axis_y] < 0) robostar->mr2.speed[Axis_y] *= -1;
		pspeed->Caption = FloatToStr(robostar->mr2.speed[Axis_x]) + " / " + FloatToStr(robostar->mr2.speed[Axis_y]);
        //* servoCnt = 3, X, Y, Z 총3개
		for(int i = 1; i <= servoCnt; ++i){
			loadfactorForm->Panel_Position[i]->Caption = FormatFloat("0 %", robostar->mr2.mondata[i][0]);
			teachForm->lblLoadFactor[i]->Caption = FormatFloat("0 %", robostar->mr2.mondata[i][0]);
			status_pos[i]->Caption = FloatToStr(robostar->mr2.pos[i]);
			if(!robostar->mr2.zero[i] && popen->Color == clLime) status_org[i]->Color = clLime;
			else status_org[i]->Color = clSilver;

			if(robostar->mr2.limit[i] & SSC_BIT_LSP) status_lsp[i]->Color = clSilver;
			else status_lsp[i]->Color = clRed;

			if(robostar->mr2.limit[i] & SSC_BIT_LSN) status_lsn[i]->Color = clSilver;
			else status_lsn[i]->Color = clRed;
		}

		for(int i = 1; i <= servoCnt; ++i){
			if(robostar->mr2.servo_alarm[i] > 0){
				perr->Color = clRed;
				perr->Caption = IntToHex(robostar->mr2.servo_alarm[i], 2);
				status_error[i]->Color = clRed;
				NGflag = true;
				break;
			}
			else status_error[i]->Color = clSilver;
			if(robostar->mr2.oper_alarm[i] > 0){
				perr->Color = clRed;
				perr->Caption = IntToHex(robostar->mr2.oper_alarm[i], 2);
				status_error[i]->Color = clRed;
				NGflag = true;
				break;
			}
			else status_error[i]->Color = clSilver;
		}
	}
	else if(robostar->IsSscOpened())
	{
		NGflag = true;
		perr->Color = clRed;
		popen->Color = clRed;
		perr->Caption = IntToHex(robostar->mr2.system_detail, 8);
	}

	if(robostar->IsSscOpened()){
		for(int i = 1; i <= servoCnt; ++i){
			if(robostar->mr2.mondata[i][0] > loadfactorForm->m_SetLimit)
			{
				loadfactorForm->m_Count++;
				if(loadfactorForm->m_Count > 10)
				{
					loadfactor_AlarmForm->ShowError(BaseForm->GetLangStr("MSG_LOADFACTOR_LIMIT"), BaseForm->GetLangStr("MSG_CHECK_SERVOSTATUS"));
					break;
				}
			}
		}
	}

	if(popen->Color == clLime)
	{
		m_ServoOpen = true;

		if(status_on[Axis_x]->Color == clLime && status_on[Axis_y]->Color == clLime
			&& status_on[Axis_z]->Color == clLime)
		{
			m_ServoON = true;

			//* 2026 08 07 천안 불량선별기와 동일하게 원점 완료와 실제 대기 위치(0)를 모두 확인
			bool allServoOrigin = true;
			bool allServoAtWaitPosition = true;
			for(int i = 1; i <= servoCnt; ++i){
				if(status_org[i]->Color != clLime)
					allServoOrigin = false;
				if(robostar->mr2.pos[i] != 0)
					allServoAtWaitPosition = false;
			}
			m_ServoHome = allServoOrigin && allServoAtWaitPosition;
            if(status_org[Axis_x]->Color == clLime && status_org[Axis_y]->Color == clLime
				&& status_org[Axis_z]->Color == clLime) {
				m_ServoHomeEmg = true;
			}
			else {
				m_ServoHomeEmg = false;
			}
		}
		else
		{
			m_ServoON = false;
			m_ServoHome = false;
            m_ServoHomeEmg = false;
		}
	}
	else
	{
		m_ServoOpen = false;
		m_ServoON = false;
		m_ServoHome = false;
        m_ServoHomeEmg = false;
	}

	if(teachForm->Visible){
		teachForm->pcell1->Color = pcell1->Color;
		teachForm->pcell2->Color = pcell2->Color;

		teachForm->pflow1->Color = pflow1->Color;
		teachForm->pflow2->Color = pflow2->Color;

		teachForm->popen1->Color = popen1->Color;
		teachForm->popen2->Color = popen2->Color;

		teachForm->pclose1->Color = pclose1->Color;
		teachForm->pclose2->Color = pclose2->Color;

		teachForm->popen->Color = popen->Color;
		teachForm->pOnX1->Color = pOnX1->Color;
		teachForm->pOnY->Color = pOnY->Color;
		teachForm->pOnZ->Color = pOnZ->Color;
		teachForm->pOrgX1->Color = pOrgX1->Color;
		teachForm->pOrgY->Color = pOrgY->Color;
		teachForm->pOrgZ->Color = pOrgY->Color;
		teachForm->pErrorX1->Color = pErrorX1->Color;
		teachForm->pErrorY->Color = pErrorY->Color;
		teachForm->pErrorZ->Color = pErrorZ->Color;
		teachForm->pLspX1->Color = pLspX1->Color;
		teachForm->pLspY->Color = pLspY->Color;
		teachForm->pLspZ->Color = pLspZ->Color;
		teachForm->pLsnX1->Color = pLsnX1->Color;
		teachForm->pLsnY->Color = pLsnY->Color;
		teachForm->pLsnZ->Color = pLsnZ->Color;

		teachForm->px1->Caption = px1->Caption;
		teachForm->py->Caption = py->Caption;
		teachForm->pz->Caption = pz->Caption;
		teachForm->pspeed->Caption = pspeed->Caption;
	}

	if(ErrorForm_eject->Visible){
		ErrorForm_eject->pcell1->Color = pcell1->Color;
		ErrorForm_eject->pcell2->Color = pcell2->Color;

		ErrorForm_eject->pflow1->Color = pflow1->Color;
		ErrorForm_eject->pflow2->Color = pflow2->Color;

		ErrorForm_eject->popen1->Color = popen1->Color;
		ErrorForm_eject->popen2->Color = popen2->Color;

		ErrorForm_eject->pclose1->Color = pclose1->Color;
		ErrorForm_eject->pclose2->Color = pclose2->Color;
	}

	if(ErrorForm_insert->Visible){
		ErrorForm_insert->pcell1->Color = pcell1->Color;
		ErrorForm_insert->pcell2->Color = pcell2->Color;

		ErrorForm_insert->pflow1->Color = pflow1->Color;
		ErrorForm_insert->pflow2->Color = pflow2->Color;

		ErrorForm_insert->popen1->Color = popen1->Color;
		ErrorForm_insert->popen2->Color = popen2->Color;

		ErrorForm_insert->pclose1->Color = pclose1->Color;
		ErrorForm_insert->pclose2->Color = pclose2->Color;
	}

    for(int i = 0; i < gripCnt; i++){
        pcode1->Caption = gripper->tool[i].code;
        psource_ch1->Caption = gripper->tool[i].source_ch;
        ptarget_ch1->Caption = gripper->tool[i].target_ch;
        puse1->Visible = gripper->tool[i].disable;
    }

	if(robostar->IsSafetyDoorOpen(1))pdoor_left->Color = clRed;
	else pdoor_left->Color = clSilver;

	if(robostar->IsSafetyDoorOpen(2))pdoor_right->Color = clRed;
	else pdoor_right->Color = clSilver;

	if(robostar->IsEmergencyStopActive())pemergency->Color = clRed;
	else pemergency->Color = clSilver;

	if(gripper->pauseStatus)ppause->Color = clRed;
	else ppause->Color = clSilver;

	if(NGflag) AlarmForm->ShowError(BaseForm->GetLangStr("MSG_ROBOT_ALARM") + " (Error Code : " +  perr->Caption + ")", "Please RESET.");
	else
	{
        perr->Caption = "";
		perr->Color = clWhite;
	}

	//* 테스트 위해 주석처리 2019 05 15
	if(robostar->IsSafetyDoorOpen(1))
		doorForm->ShowError("DOOR #1 Open", BaseForm->GetLangStr("MSG_CLOSE_DOOR"), 0);
	if(robostar->IsSafetyDoorOpen(2))
		doorForm->ShowError("DOOR #2 Open", BaseForm->GetLangStr("MSG_CLOSE_DOOR"), 1);
	if(robostar->IsEmergencyStopActive())
		doorForm->ShowError("Emergency Stop", BaseForm->GetLangStr("MSG_CHECK_EMGSWITCH"), 2);
	if(!robostar->IsKeyLockActive())
	{
		if(equipMode == modeAuto)
			doorForm->ShowError("KEYLOCK Unlock", BaseForm->GetLangStr("MSG_CHECK_EMGSWITCH"), 4);
		else
            robostar->req_Speed(200, 3000, 3000);
	}

	if(robostar->IsSscOpened() && popen->Color != clLime)
		doorForm->ShowError("RESET", BaseForm->GetLangStr("MSG_SERVO_OPEN"), 5);

	pejectremainCnt->Caption = tray_source.remainCnt;
	pinsertremainCnt->Caption = tray_target.remainCnt;

	//* for mes test START
	if(cbMES->Checked){
		psrcArrive->Color = clLime;
		psrcReady->Color = clLime;
		ptargetReady->Color = clLime;

		pTrayid_source2->Caption = pTrayid_source->Caption;
		pTrayid_target2->Caption = pTrayid_target->Caption;
	}
	//* FOR MES TEST END

	/*
	* 테스트 위해 아래 코드 주석처리
	*/

	// D10104 Source Centering complete resets D10154 through ModPLC_BIN.
	if(IsSourceCenteringSignal() && PlcBin != NULL)
		PlcBin->CmdSourceCenteringRequest(false);

	if(IsSourceTrayInSignal())psrcArrive->Color = clLime;   		   // test
	else{
		psrcArrive->Color = clSilver;
		pwork1->Color = clSilver;
		pwork2->Color = clSilver;
		psrcReady->Font->Color = clBlack;
	}
	if(IsSourceCenteringSignal())  // gsm test 2018 09 14
	{
		psrcReady->Color = clLime;
	}
	else{
		psrcReady->Color = clSilver;
		psrcReady->Font->Color = clBlack;
	}
	if(IsTargetCenteringSignal())ptargetReady->Color = clLime;
	else{
		if(!opcTrayLoadPending[1]){
			opcTrayDisplayed[1] = false;
			opcTrayLoaded[1] = false;
		}
		ptargetReady->Color = clSilver;
		pwork2->Color = clSilver;
	}

	if(PlcBin != NULL && PlcBin->IsSourceTrayOutOn())psrcOut->Color = clLime;
	else	psrcOut->Color = clSilver;

	if(PlcBin != NULL && PlcBin->IsTargetTrayOutOn())ptargetOut->Color = clLime;
	else	ptargetOut->Color = clSilver;
	psrcOut->Font->Color = clBlack;
	ptargetOut->Font->Color = clBlack;

	//* 2026 08 07 PC D10150~D10158 Word 신호 전송

	if(PlcBin != NULL){
		bool doorOpen = robostar->IsSafetyDoorOpen(1) || robostar->IsSafetyDoorOpen(2);
		bool pcError = NGflag || (AlarmForm != NULL && AlarmForm->Visible) ||
			(doorForm != NULL && doorForm->Visible);
		bool pcAutoMode = (equipMode == modeAuto);
		bool sourceTrayIn = IsSourceTrayInSignal();
		bool sourceCentering = IsSourceCenteringSignal();
		bool targetTrayIn = IsTargetTrayInSignal();

		// D10151 follows PC mode: AUTO=1, MANUAL/other=0.
		PlcBin->CmdPcAutoMode(pcAutoMode);
		PlcBin->CmdPcError(pcError);

		// D10153 follows only the physical servo-home/XYZ=0 condition.
		PlcBin->CmdTrayInReady(m_ServoHome);

		if(pcAutoMode){
			if(sourceTrayIn && chkBypass->Checked){
				// Tray Out itself is issued by the automatic sequence.
				PlcBin->CmdSourceCenteringRequest(false);
			}
			else if(sourceTrayIn && !sourceCentering){
				PlcBin->CmdSourceCenteringRequest(true);
			}
			else if(sourceCentering){
				PlcBin->CmdSourceCenteringRequest(false);
			}
		}

		// D10103 OFF completes D10155.
		if(!sourceTrayIn)
			PlcBin->CmdSourceTrayOut(false);
		if(!targetTrayIn)
			PlcBin->CmdTargetTrayOut(false);

		PlcBin->CmdPcEmergency(robostar->IsEmergencyStopActive());
		PlcBin->CmdPcDoorOpen(doorOpen);
	}
}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void __fastcall TMainForm::BuzzerOn(bool on)
{
	robostar->gripper.TOWER_LAMP_BUZZER = on;
	if(on && (nowLampMode != LampEmergency) && (nowLampMode != LampAlarm))
		beforeLampMode = nowLampMode;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::LampModeChange(LampMode mode)
{
	nowLampMode = mode;
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::btnApplyNgLimitCountClick(TObject *Sender)
{
	try{
		stage.limitCnt = limitEdit->Text.ToInt();
		MessageBox(Handle, BaseForm->GetLangStr("MSG_NGLIMIT_SET").c_str(), L"NG limt", MB_OK|MB_ICONINFORMATION);
	}
	catch(...){
		MessageBox(Handle, BaseForm->GetLangStr("MSG_CHECK_VALUE").c_str(), L"WARNING", MB_OK|MB_ICONWARNING);
	}

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::zone1Click(TObject *Sender)
{
	TAdvSmoothButton *btn;
	btn = (TAdvSmoothButton*)Sender;
	TGridRect sRec;
	sRec = targetGrid->Selection;


	int nch = 0;
	if(MessageBox(Handle, L"Do you want to set it to the area you selected?", L"Area select", MB_YESNO|MB_ICONQUESTION) == ID_YES){

		for(int nrow = sRec.Left; nrow<=sRec.Right; ++nrow){
			for(int ncol = sRec.Top; ncol<=sRec.Bottom; ++ncol){
				nch = ( (3 - nrow) *6) + (ncol%6);
				pt_ch[nch]->Fill->Color = btn->Color;
				pt_ch[nch]->Fill->ColorMirror = btn->Color;
				pt_ch[nch]->Fill->ColorMirrorTo = btn->Color;
				pt_ch[nch]->Fill->ColorTo = btn->Color;
			}
		}
		WriteZoneList();
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::WriteZoneList()
{
	AnsiString file;
	file = (AnsiString)BIN + "zone.ini";

	ini = new TIniFile(file);
		for(int i=0; i<12; ++i){
			ini->WriteInteger("COLOR", i, pt_ch[i]->Fill->Color);

		}
	delete ini;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ReadZoneList()
{
	AnsiString file;
	file = (AnsiString)BIN + "zone.ini";
	TColor clr;
	ini = new TIniFile(file);
		for(int i=0; i<12; ++i){
			clr = (TColor)ini->ReadInteger("COLOR", i, clWhite);
			pt_ch[i]->Fill->Color = clr;
			pt_ch[i]->Fill->ColorMirror = clr;
			pt_ch[i]->Fill->ColorMirrorTo = clr;
			pt_ch[i]->Fill->ColorTo = clr;
		}
	delete ini;
}
//---------------------------------------------------------------------------

int __fastcall TMainForm::GetZoneCount(int zone)
{
	int ncnt = 0;
	int ntotal = 0;

	for(int i=0; i<12; ++i){
		if(pt_ch[i]->Fill->Color == zoneBtn[zone]->Color){
			ntotal += 1;	// zone 총 수량
			if(tray_target.SLOT_ID[i].IsEmpty() && tray_target.PICK[i] == "N")
				ncnt += 1;	// empty 수량
		}
	}

	if(ntotal == 0)ncnt = -1;
	return ncnt;
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::GetZoneChannel(int zone, int ch)
{
	if(tray_target.PICK[ch] == "N" && pt_ch[ch]->Fill->Color == zoneBtn[zone]->Color)return true;
	else return false;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMainForm::GetAlarmMsg(int code)
{
	switch(code){
		case 1: return "MES로부터 선별 TRAY정보응답 ERROR"; break;
		case 2: return "MES로부터 선별 TRAY_REPLY_S 응답시간 초과"; break;
		case 3: return "MES로부터 대상 TRAY정보응답 ERROR"; break;
		case 4: return "MES로부터 대상 TRAY_REPLY_T 응답시간 초과"; break;
		case 5: return "MES로부터 대상 ID_MATCHING 응답 Error"; break;
		case 6: return "MES로부터 대상 ID_MATCHING 응답시간 초과"; break;
		case 7: return "MES로부터 선별 TRAY TRANSFER_OUT 응답 Error"; break;
		case 8: return "MES로부터 선별 TRAY TRANSFER_OUT 응답시간 초과"; break;
		case 9: return "MES로부터 대상 TRAY TRANSFER_OUT 응답 Error"; break;
		case 10: return "MES로부터 대상 TRAY TRANSFER_OUT 응답시간 초과"; break;
		case 11: return "MES로부터 SEND_EVENT ERROR"; break;
		case 12: return "MES로부터 SEND_EVENT 응답시간 초과"; break;
		case 13: return "MES로부터 선별 ID_MATCHING 응답 Error"; break;
		case 14: return "MES로부터 선별 ID_MATCHING 응답시간 초과"; break;
		case 15: return "[선별TRAY] GRIP DOWN센서 감지 시간초과"; break;
		case 16: return "[선별TRAY] GRIP UP센서 감지 시간초과"; break;
		case 17: return "[선별TRAY] GRIP DOWN시 충돌되었습니다"; break;
		case 18: return "[대상TRAY] GRIP UNCHUCK 센서 감지시간 초과"; break;
		case 19: return "[대상TRAY] GRIP UP 센서 감지시간 초과"; break;
		case 20: return "[선별TRAY] Cell No, Grip No에 셀이 없음"; break;
		case 21: return "[대상TRAY] GRIP DOWN시 충돌되었습니다"; break;
		case 22: return "[대상TRAY] GRIP DOWN시간 초과"; break;
		case 23: return "[대상TRAY] Cell No, Grip No 에 셀이 없음"; break;
		case 24: return "불량수가 설정치 보다 많습니다"; break;
		case 25: return "DOOR #1 Open"; break;
		case 26: return "DOOR #2 Open"; break;
		case 27: return "Emergency stop"; break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::AddStatusLog(AnsiString source, AnsiString msg)
{
	AnsiString logMsg = "[" + source + "] " + msg;

	WriteProgLog(logMsg);

	if(memoLog == NULL)
		return;

	const int maxLogLines = 1000;
	AnsiString displayMsg = Now().FormatString("yyyy-mm-dd hh:nn:ss.zzz ") + logMsg;

	memoLog->Lines->BeginUpdate();
	try{
		memoLog->Lines->Insert(0, displayMsg);
		while(memoLog->Lines->Count > maxLogLines)
			memoLog->Lines->Delete(memoLog->Lines->Count - 1);
	}
	__finally{
		memoLog->Lines->EndUpdate();
	}

	memoLog->SelStart = 0;
	memoLog->SelLength = 0;
	memoLog->Perform(EM_SCROLLCARET, 0, 0);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::memoMainLineAdd(AnsiString msg)
{
	if(pmainMsg->Hint != msg){
		pmainMsg->Caption = msg;
		pmainMsg->Hint = msg;
		AddStatusLog("MAIN", msg);
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::memoGripperLineAdd(AnsiString msg)
{
	if(pgripperMsg->Hint != msg){
		pgripperMsg->Caption = msg;
		pgripperMsg->Hint = msg;
		AddStatusLog("GRIPPER", msg);
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::memoRobostarLineAdd(AnsiString msg)
{
	if(probostarMsg->Hint != msg){
		probostarMsg->Caption = msg;
		probostarMsg->Hint = msg;
		AddStatusLog("ROBOT", msg);
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::AdvSmoothToggleButton_InitWorkClick(TObject *Sender)
{
	if(gripper->pauseStatus && robostar->pauseStatus)
	{
		if(MessageBox(Handle, BaseForm->GetLangStr("MSG_INIT_WORK").c_str(),
			L"Initialize", MB_YESNO|MB_ICONQUESTION) == ID_YES)
		{
            gripper->seq_save = seqIdle;
			robostar->seq_save = seqIdle;

            if(PlcBin != NULL) PlcBin->CmdSourceCenteringRequest(false);

			InitStep(&step[0]);
			InitStep(&step[1]);

			pwork1->Color = clSilver;
			pwork2->Color = clSilver;
		}
	}else ShowMessage(BaseForm->GetLangStr("MSG_INIT_WORK_ALARM"));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TMainForm::pnlSource2Click(TObject *Sender)
{
    badCode->Visible = !badCode->Visible;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::lblTitleClick(TObject *Sender)
{
	bool showTestOptions = !cbMES->Visible;
	cbMES->Visible = showTestOptions;
	cbCycle->Visible = showTestOptions;

	if(!showTestOptions){
		cbMES->Checked = false;
		cbCycle->Checked = false;
	}
}
//---------------------------------------------------------------------------

