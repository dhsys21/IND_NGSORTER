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
	for(int i = 0; i < 2; ++i) comBcr[i] = NULL;
	comSmoke[0] = NULL;
	CreateIoMonitoringPanel();
}
//---------------------------------------------------------------------------
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

	if(robostar->getGripperUpStatus() == false)
	{
		AlarmForm->ShowError(BaseForm->GetLangStr("MSG_DOWN_STATE"), BaseForm->GetLangStr("MSG_CHECK_TRAYOUT"));
        return;
	}

    if(pos == 0){
		NotifyEquipStatus("IDLE");

		if(tray_source.empTray && tray_source.remainCnt == 0) plcOutput.SRC_EMP = 1;
		else plcOutput.SRC_EMP = 0;

		plcOutput.SRC_OUT = 1;
		plcOutput.SRC_WORK = 0;
	}else{
		plcOutput.TARGET_OUT = 1;
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
				if(plcInput.SRC_ARRIVE){
					NotifyTrayInfo(strBcr, true);
				}
				break;
			case 1:
				this->pTrayid_target->Caption = strBcr;
				if(plcInput.TARGET_READY)NotifyTrayInfo(strBcr, false);
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

void __fastcall TMainForm::autoBtnClick(TObject *Sender)
{
	if(m_ServoHome || !robostar->IsSscOpened())
	{
		if(!robostar->IsSscOpened())
			memoRobostarLineAdd("[Servo] SSC is not open. Auto mode continues without servo control.");

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
	else
	{
        autoBtn->Down = false;
		AlarmForm->ShowError(BaseForm->GetLangStr("MSG_AUTO_ALARM1"), BaseForm->GetLangStr("MSG_AUTO_ALARM2"));
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::manualBtnClick(TObject *Sender)
{
	if(autoBtn->Down == true){
		if(robostar->IsSafetyDoorOpen(1) || robostar->IsSafetyDoorOpen(2) || robostar->IsEmergencyStopActive())
		{
			plcOutput.SRC_MANUAL_WORK = plcInput.SRC_READY;

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

				plcOutput.SRC_MANUAL_WORK = plcInput.SRC_READY;

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
	robostar->req_AutoRun();
	if(gripper->seq == 4) gripper->step.step = 0;   //  seqPause
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::trayout_srcBtnClick(TObject *Sender)
{
	if(plcInput.SRC_ARRIVE){
		if(MessageBox(Handle, BaseForm->GetLangStr("MSG_EJECT_SOURCETRAY").c_str(), L"Tray Out", MB_YESNO|MB_ICONQUESTION) == ID_YES){
            plcOutput.SRC_MANUAL_WORK = 0;
			CmdTrayOut(0);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::trayout_targetBtnClick(TObject *Sender)
{
	int reply;
	if(plcInput.TARGET_READY){
		if(MessageBox(Handle, BaseForm->GetLangStr("MSG_EJECT_TARGETTRAY").c_str(), L"Tray Out", MB_YESNO|MB_ICONQUESTION) == ID_YES){
			reply = MessageBox(Handle, BaseForm->GetLangStr("MSG_MES_REQUEST").c_str(), L"MES", MB_YESNOCANCEL|MB_ICONQUESTION);
			if(reply == ID_YES){
				NotifyTransferOut(pTrayid_target->Caption);
				CmdTrayOut(1);
			}else if(reply == ID_NO){
				CmdTrayOut(1);
			}

		}
	}
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

	if(plcInput.SRC_ARRIVE == 0)InitStep(&step[0]);
	if(plcInput.TARGET_READY == 0)InitStep(&step[1]);

    if(!gripper->pauseStatus && !robostar->pauseStatus)
	{
		switch(step[0].step){
			case 0:
				if(plcInput.SRC_ARRIVE){
					NotifyEquipStatus("PROCESS");
					if(chkBypass->Checked == false){
						pTrayid_source->Caption = "";
						pTrayid_source2->Caption = "";
						memoMainLineAdd(BaseForm->GetLangStr("MSG_SOURCETRAY_SCAN"));
						comBcr[0]->GetBarcode();	// 작업1. 선별 바코드 읽고  -> DisplayTrayInfo  	// test
						step[0].step += 1;
					}else{
						memoMainLineAdd(BaseForm->GetLangStr("MSG_BYPASS_TRAYOUT"));
						CmdTrayOut(0);
						step[0].step += 100;
					}
				}
				else{
					if(plcInput.TARGET_READY && pTrayid_target->Caption.IsEmpty()){
						memoMainLineAdd(BaseForm->GetLangStr("MSG_TARGETTRAY_SCAN"));
						comBcr[1]->GetBarcode();                                            // test
					}else{
						memoMainLineAdd(BaseForm->GetLangStr("MSG_SOURCETRAY_WAITING"));
					}
				}
				break;
			case 1:
				if(plcInput.SRC_READY){
					memoMainLineAdd(BaseForm->GetLangStr("MSG_SOURCETRAY_CENTERING_COMPL"));
					NotifyTransferIn(pTrayid_source->Caption);	// 작업3. 센터링을 치면 작업시작 보고를 한다.
					step[0].step += 1;
				}else{
					memoMainLineAdd(BaseForm->GetLangStr("MSG_SOURCETRAY_CENTERING"));
				}
				break;
			case 2:
				if(plcInput.TARGET_READY){
					memoMainLineAdd(BaseForm->GetLangStr("MSG_TARGETTRAY_CENTERING_COMPL"));
					pTrayid_target->Caption = "";       // test
					pTrayid_target2->Caption = "";      // test
					comBcr[1]->GetBarcode();		// 작업4. 센터링이 되어 있으면 바코드를 읽고 -> DisplayTrayInfo 	// test
					step[0].step += 1;
					step[1].step = 1;
				}else{
					memoMainLineAdd("The target tray is missing or not centering.");
					AlarmForm->ShowError("No Target Tray", BaseForm->GetLangStr("MSG_TARGETTRAY_CHECK_CENTERING").c_str());
				}
			default:
				break;
		}


		 switch(step[1].step){
			case 0:
				if(plcInput.TARGET_READY && pwork1->Color == clLime){
					memoMainLineAdd("More target trays arrived.");
					pTrayid_target->Caption = "";
					pTrayid_target2->Caption = "";
					comBcr[1]->GetBarcode();		// 작업4. 센터링이 되어 있으면 바코드를 읽고 -> DisplayTrayInfo     // test
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
	if(chkBypass->Checked)this->InitStep(&step[0]);	
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

	btnIOMonitoring = new TAdvSmoothButton(this);
	btnIOMonitoring->Parent = this;
	btnIOMonitoring->Left = pemergency->Left;
	btnIOMonitoring->Top = 1;
	btnIOMonitoring->Width = pemergency->Width;
	btnIOMonitoring->Height = 24;
	btnIOMonitoring->Caption = "I/O Monitoring";
	btnIOMonitoring->Color = clWhite;
	btnIOMonitoring->BevelColor = clBlack;
	btnIOMonitoring->Appearance->Font->Style = TFontStyles() << fsBold;
	btnIOMonitoring->OnClick = btnIOMonitoringClick;
	btnIOMonitoring->BringToFront();

	grp_io = new TPanel(this);
	grp_io->Parent = this;
	grp_io->Left = 570;
	grp_io->Top = 92;
	grp_io->Width = 600;
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
	inputPanel->Width = 584;
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
	scrInput->Width = 574;
	scrInput->Height = 398;
	scrInput->HorzScrollBar->Visible = false;
	scrInput->Color = clWhite;

	const char *inputNames[48] = {
		"CP01 TRIP", "CP02 TRIP", "CP03 TRIP", "CP04 TRIP", "CP05 TRIP", "CP06 TRIP", "CP07 TRIP", "CP08 TRIP",
		"CP09 TRIP", "CP10 SERVO1 TRIP", "CP11 SERVO2 TRIP", "CP12 SERVO3 TRIP", "CP13 BCR01 TRIP", "CP14 BCR02 TRIP", "MS01 TRIP", "",
		"SERVO01 INPOS", "SERVO01 ALARM", "SERVO01 OK HOME", "SERVO02 INPOS", "SERVO02 ALARM", "SERVO02 OK HOME", "SERVO03 INPOS", "SERVO03 ALARM",
		"SERVO03 OK HOME", "", "", "", "", "", "", "",
		"GRIPPER1 CHUCK", "GRIPPER1 UNCHUCK", "GRIPPER1 CELL DETECT", "GRIPPER1 BUFFER", "EMS NORMAL", "OPBOX RESET SWITCH", "SAFETY DOOR #1 UNLOCKED", "SAFETY DOOR #2 UNLOCKED",
		"SAFETY RESET SW ON", "BY-PASS S/W ON", "BY-PASS S/W OFF", "SAFETY EMG READY", "SAFETY DOOR READY", "", "", ""
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
	outputPanel->Width = 584;
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
	scrOutput->Width = 574;
	scrOutput->Height = 204;
	scrOutput->HorzScrollBar->Visible = false;
	scrOutput->Color = clWhite;
	scrOutput->Tag = 8;

	const char *outputNames[16] = {
		"GRIPPER CHUCK SOL", "GRIPPER UNCHUCK SOL", "SAFETY RESET", "KEYLOCK LEFT",
		"KEYLOCK RIGHT", "OPBOX RESET LAMP", "SAFETY RESET SW LAMP", "OPBOX EMERGENCY LAMP",
		"TOWER LAMP RED", "TOWER LAMP YELLOW", "TOWER LAMP GREEN", "TOWER LAMP BUZZER",
		"BYPASS", "", "", ""
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
		robostar->input.GRIPPER1_UP, robostar->input.GRIPPER1_DOWN, robostar->input.GRIPPER1_CELL_DETECT, robostar->input.GRIPPER1_BUFFER,
		robostar->input.EMS_SWITCH, robostar->input.OPBOX_RESET_SWITCH, robostar->input.SAFETY_DOOR_1, robostar->input.SAFETY_DOOR_2,
		robostar->input.SAFETY_RESET_SW_ON, robostar->input.BYPASS_SW_ON, robostar->input.BYPASS_SW_OFF, robostar->input.SAFETY_EMG_READY,
		robostar->input.SAFETY_DOOR_READY, robostar->input.SAFETY_DOOR_3, robostar->input.X002E, robostar->input.X002F
	};

	bool outputValue[16] = {
		robostar->gripper.GRIPPER1_CHUCK, robostar->gripper.GRIPPER1_UNCHUCK, robostar->gripper.SAFETY_RESET, robostar->gripper.DOOR_LEFT_CLOSE,
		robostar->gripper.DOOR_RIGHT_CLOSE, robostar->gripper.OPBOX_RESET_LAMP, robostar->gripper.SAFETY_RESET_SW_LAMP, robostar->gripper.OPBOX_EMERGENCY_LAMP,
		robostar->gripper.TOWER_LAMP_RED, robostar->gripper.TOWER_LAMP_YELLOW, robostar->gripper.TOWER_LAMP_GREEN, robostar->gripper.TOWER_LAMP_BUZZER,
		robostar->gripper.DOOR_OPEN_SELECT, robostar->gripper.Y003D, robostar->gripper.Y003E, robostar->gripper.Y003F
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
	for(int i=0; i<=3; ++i)GetZoneCount(i);

	sensorColor(pcell1, robostar->input.GRIPPER1_CELL_DETECT);
	sensorColor(pup1, robostar->input.GRIPPER1_UP);
	sensorColor(pdn1, robostar->input.GRIPPER1_DOWN);
	sensorColor(pflow1, robostar->input.GRIPPER1_BUFFER);
	sensorColor(popen1, !robostar->output.GRIPPER1_CHUCK);
	sensorColor(pclose1, robostar->output.GRIPPER1_CHUCK);
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

	int flowValue = robostar->CheckFlow();
	if(flowValue != 0)
	{
		robostar->req_JogMove(-1);
		AlarmForm->ShowError("No." + IntToStr(flowValue) + " " + BaseForm->GetLangStr("MSG_GRIPPER_FLOW"), BaseForm->GetLangStr("MSG_CHECK_RESTART"));
	}

	UpdateIoMonitoringPanel();

	if(robostar->IsSscOpened() && robostar->mr2.system_status == SSC_STS_CODE_RUNNING) popen->Color = clLime;
	else popen->Color = clSilver;

	bool NGflag = false;
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
			if(robostar->mr2.servo[i]) status_on[i]->Color = clLime;
			else status_on[i]->Color = clSilver;

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

			if(status_org[Axis_x]->Color == clLime && status_org[Axis_y]->Color == clLime
				&& status_org[Axis_z]->Color == clLime) {
				m_ServoHome = true;
			}
			else {
				m_ServoHome = false;
			}
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

		teachForm->pup1->Color = pup1->Color;
		teachForm->pup2->Color = pup2->Color;

		teachForm->pdn1->Color = pdn1->Color;
		teachForm->pdn2->Color = pdn2->Color;

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
		teachForm->pOnG1->Color = pOnG1->Color;
		teachForm->pOrgX1->Color = pOrgX1->Color;
		teachForm->pOrgY->Color = pOrgY->Color;
		teachForm->pOrgZ->Color = pOrgY->Color;
		teachForm->pOrgG1->Color = pOrgG1->Color;
		teachForm->pErrorX1->Color = pErrorX1->Color;
		teachForm->pErrorY->Color = pErrorY->Color;
		teachForm->pErrorZ->Color = pErrorZ->Color;
		teachForm->pErrorG1->Color = pErrorG1->Color;
		teachForm->pLspX1->Color = pLspX1->Color;
		teachForm->pLspY->Color = pLspY->Color;
		teachForm->pLspZ->Color = pLspZ->Color;
		teachForm->pLspG1->Color = pLspG1->Color;
		teachForm->pLsnX1->Color = pLsnX1->Color;
		teachForm->pLsnY->Color = pLsnY->Color;
		teachForm->pLsnZ->Color = pLsnZ->Color;
		teachForm->pLsnG1->Color = pLsnG1->Color;

		teachForm->px1->Caption = px1->Caption;
		teachForm->py->Caption = py->Caption;
		teachForm->pz->Caption = pz->Caption;
		teachForm->pspeed->Caption = pspeed->Caption;
	}

	if(ErrorForm_eject->Visible){
		ErrorForm_eject->pcell1->Color = pcell1->Color;
		ErrorForm_eject->pcell2->Color = pcell2->Color;

		ErrorForm_eject->pup1->Color = pup1->Color;
		ErrorForm_eject->pup2->Color = pup2->Color;

		ErrorForm_eject->pdn1->Color = pdn1->Color;
		ErrorForm_eject->pdn2->Color = pdn2->Color;

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

		ErrorForm_insert->pup1->Color = pup1->Color;
		ErrorForm_insert->pup2->Color = pup2->Color;

		ErrorForm_insert->pdn1->Color = pdn1->Color;
		ErrorForm_insert->pdn2->Color = pdn2->Color;

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
	if(CheckBox1->Checked){
		plcInput.SRC_READY = 1;
		plcInput.TARGET_READY = 1;

		plcInput.SRC_ARRIVE = 1;
		psrcArrive->Color = clLime;
		psrcReady->Color = clLime;

		plcInput.TARGET_READY = 1;
		ptargetReady->Color = clLime;

		pTrayid_source2->Caption = "MPA0001";
		pTrayid_target2->Caption = "NG10006";
	}
	//* FOR MES TEST END

	/*
	* 테스트 위해 아래 코드 주석처리
	*/
	if(plcInput.SRC_OUT)
	{
        plcOutput.SRC_EMP = 0;
		plcOutput.SRC_OUT = 0;
	}
	if(plcInput.TARGET_OUT)plcOutput.TARGET_OUT = 0;

	if(plcInput.SRC_READY)
		plcOutput.SRC_WORK = 0;

	if(plcInput.SRC_ARRIVE)psrcArrive->Color = clLime;   		   // test
	else{
		psrcArrive->Color = clSilver;
		pwork1->Color = clSilver;
		pwork2->Color = clSilver;
		psrcReady->Font->Color = clBlack;
	}
	if(plcInput.SRC_READY)  // gsm test 2018 09 14
	{
		psrcReady->Color = clLime;
	}
	else{
		psrcReady->Color = clSilver;
		psrcReady->Font->Color = clBlack;
	}
	if(plcInput.TARGET_READY)ptargetReady->Color = clLime;
	else{
		ptargetReady->Color = clSilver;
		pwork2->Color = clSilver;
	}

	if(plcOutput.SRC_OUT)psrcOut->Color = clLime;
	else	psrcOut->Color = clSilver;

	if(plcOutput.TARGET_OUT)ptargetOut->Color = clLime;
	else	ptargetOut->Color = clSilver;

	if(plcInput.SRC_OUT)psrcOut->Font->Color = clRed;
	else	psrcOut->Font->Color = clBlack;

	if(plcInput.TARGET_OUT)ptargetOut->Font->Color = clRed;
	else	ptargetOut->Font->Color = clBlack;

	if(equipMode == modeAuto)
	{
		plcOutput.AUTO_RUN = 1;
		plcOutput.SRC_MANUAL_WORK = 0;   // for TEST
	}
	else plcOutput.AUTO_RUN = 0;

	WORD nData = 0;

	nData = *((WORD *)&plcOutput + 0);
	plc->WriteWordData(0x4001, 1, IntToHex(nData, 4));

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

            plcOutput.SRC_WORK = 0;

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
    CheckBox1->Visible = !CheckBox1->Visible;
    if(CheckBox1->Visible == false)
        CheckBox1->Checked = false;
}
//---------------------------------------------------------------------------







