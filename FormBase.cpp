//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
#pragma link "wininet.lib"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TBaseForm *BaseForm;
//---------------------------------------------------------------------------
__fastcall TBaseForm::TBaseForm(TComponent* Owner)
	: TForm(Owner)
{
	DeleteDay = 180;
	DeleteIndex = 0;
	//* max speed mode - need remove
	// FAT options default OFF. A missing or old INI therefore starts with
	// the normal production sequence and the physical barcode readers.
	config.maximumSpeedMode = false;
	config.targetTrayUnloadCount = 0;
	config.optimizeSequenceDelay = false;
	config.skipGripStabilization = false;
	config.useFatTestBarcodes = false;
	config.fatTestSourceBarcode = "TR-20260818-src";
	config.fatTestTargetBarcode = "TR-20260818-trg";
	config.bcrIp[0] = "192.168.100.238";
	config.bcrIp[1] = "192.168.100.239";
	config.bcrPort[0] = 9004;
	config.bcrPort[1] = 9004;
	config.smokePort = "COM3";
	config.smokeId = 1;
	config.smokeMode = 0;
	config.smokeBaudRate = 115200;
	config.plcIp = "192.168.0.1";
	config.plcPortPlc = 6002;
	config.plcPortPc = 6003;
	SelectedBcrIndex = 0;

    GetWindowThreadProcessId(this->Handle, &PID);
	hProcess = OpenProcess(PROCESS_SET_QUOTA, FALSE, PID);
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::FormCreate(TObject *Sender)
{
    ReadLanguage("EN");
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::FormShow(TObject *Sender)
{
 	if(config.file_exists == false){		// test
		ConfigForm->Visible = true;
	}

	loadfactorForm->LoadInfo();

	this->Left = 0;//Screen->Width - this->Width;
	this->Top = 0;

	RadioButton2->Checked = true;
	ChangeLanguage();
	ServoAlarmListForm->LanguageChange(1);
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	// Stop the OPC UA/Indy worker threads while MainForm is still valid.
	// This prevents late socket callbacks and shortens application shutdown.
	ClockTimer->Enabled = false;
	FileDeleteTimer->Enabled = false;
	if (MesOpc != NULL)
		MesOpc->Shutdown();
	if (Mod_Fms != NULL)
		Mod_Fms->Stop();
	if (MainForm != NULL)
	{
		MainForm->EndThread();
		MainForm->Close();
	}
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	if(MessageBox(Handle, GetLangStr("MSG_PROGRAM_EXIT").c_str(), L"EXIT PROGRAM", MB_YESNO|MB_ICONQUESTION) == ID_YES){
		CanClose = true;
	}else{
		CanClose = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::setColor(TAdvSmoothPanel *pnl, bool bon)
{
	if(bon){
		pnl->Fill->Color = pon->Color;
		pnl->Fill->ColorMirror = pon->Color;
		pnl->Fill->ColorMirrorTo = pon->Color;
		pnl->Fill->ColorTo = pon->Color;
	}else{
		pnl->Fill->Color = poff->Color;
		pnl->Fill->ColorMirror = poff->Color;
		pnl->Fill->ColorMirrorTo = poff->Color;
		pnl->Fill->ColorTo = poff->Color;
	}
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::ClockTimerTimer(TObject *Sender)
{
	TDateTime dt;
	dt = Now();

	setColor(pims, Mod_Fms != NULL && Mod_Fms->IsGatewayConnected());
	setColor(pplc, PlcBin != NULL &&
		PlcBin->ClientSocket_PLC->Active && PlcBin->ClientSocket_PC->Active);
	if(MainForm->path  == 81)setColor(pcclink, true);
	else setColor(pcclink, false);

	setColor(pbcr1, MainForm->comBcr[0] != NULL && MainForm->comBcr[0]->ClientSocketBcr->Active);
	setColor(pbcr2, MainForm->comBcr[1] != NULL && MainForm->comBcr[1]->ClientSocketBcr->Active);
	setColor(psmokedetector, MainForm->comSmoke[0] != NULL && MainForm->comSmoke[0]->Comm->Connected);

	// KEYLOCK button colors follow the commanded Y0033/Y0034 output state.
	// If the two outputs disagree, neither state is shown as complete.
	bool keyLockSetOutput = robostar->gripper.DOOR_LEFT_CLOSE
		&& robostar->gripper.DOOR_RIGHT_CLOSE;
	bool keyLockReleaseOutput = !robostar->gripper.DOOR_LEFT_CLOSE
		&& !robostar->gripper.DOOR_RIGHT_CLOSE;
	btnKeyLock->Color = keyLockSetOutput ? clLime : clWhite;
	btnKeyUnLock->Color = keyLockReleaseOutput ? clLime : clWhite;
	// The single BY-PASS button displays the actual Y003C output state.
	btnBypassOn->Color = robostar->gripper.DOOR_OPEN_SELECT ? clLime : clWhite;
	btnSafetyReset->Enabled = MainForm->path == 81
		&& !robostar->IsSoftwareSafetyResetActive();
	btnSafetyReset->Color = robostar->gripper.SAFETY_RESET ? clLime : clWhite;

	if(dt.FormatString("hhnn") == "0700") {
		DeleteDay = 90;	// 12시가 되면 DeleteDay를 90으로 초기화
	}
}
//---------------------------------------------------------------------------
bool __fastcall TBaseForm::DeleteLogFile(AnsiString FileName)
{
	if(FileExists(FileName))
	{
		DeleteFile(FileName);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool __fastcall TBaseForm::DeleteLogFolder(AnsiString FolderName)
{
	if(DirectoryExists(FolderName)) {
		SHFILEOPSTRUCTA FileOp;

		FileOp.hwnd = this->WindowHandle;
		FileOp.wFunc = FO_DELETE;
		FileOp.pFrom = FolderName.c_str();
		FileOp.pTo = NULL;
		FileOp.fFlags = FOF_NOCONFIRMATION |FOF_NOERRORUI;
		FileOp.fAnyOperationsAborted = FALSE;
		FileOp.lpszProgressTitle = "Delete Logging";
		SHFileOperationA(&FileOp);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------

void __fastcall TBaseForm::FileDeleteTimerTimer(TObject *Sender)
{
	SetProcessWorkingSetSize( hProcess, -1, -1 );

	TDateTime  dt;

	// 1시간에 파일 하나씩 삭제

	dt = Now() - DeleteDay;
	switch(DeleteIndex){
	case 0:   // IMS 로그
		DeleteLogFolder((AnsiString)SOCK_LOG + dt.FormatString("yymmdd"));
		DeleteIndex +=3;
		break;
	case 1:   // COMM 로그
//		DeleteLogFolder((AnsiString)COMM_LOG + dt.FormatString("yymmdd"));
		DeleteIndex +=1;
		break;
	case 2:
//		DeleteLogFolder((AnsiString)REM_PATH + dt.FormatString("yyyy_mm_dd"));
		DeleteIndex += 1;
		break;
	case 3:
		DeleteLogFile((AnsiString)ERROR_LOG + "ERROR_" + dt.FormatString("yymmdd") + ".log");
		DeleteIndex += 1;
		break;
	case 4:    // 검사결과 파일
		DeleteLogFile((AnsiString)PROG_LOG + "STATUS_" + dt.FormatString("yymmdd") + ".log");
		DeleteIndex = 0;
		DeleteDay += 1;
		break;
	}
	if(DeleteDay > 300)DeleteDay = 90;
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::Button1Click(TObject *Sender)
{
	if(ConfigForm->Visible == true)ConfigForm->Visible = false;
	ConfigForm->FormStyle = fsStayOnTop;
	ConfigForm->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::AdvSmoothButton2Click(TObject *Sender)
{
	Application->Minimize();
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::AdvSmoothButton1Click(TObject *Sender)
{
    this->Close();
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::AdvSmoothButton3Click(TObject *Sender)
{
	ServoAlarmListForm->Left = AdvSmoothButton3->Left;
	ServoAlarmListForm->Top = AdvSmoothButton3->Top + 80;
	ServoAlarmListForm->BringToFront();
	ServoAlarmListForm->Show();
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::btnKeyLockClick(TObject *Sender)
{
	if(MainForm->equipMode != modeManual)
	{
        ShowMessage(GetLangStr("MSG_UNLOCK_KEY"));
	}
	else{
		// X0026/X0027 ON means Door #1/#2 is open. Do not start KEYLOCK setting.
		if(!robostar->CanSetKeyLock()){
			ShowMessage(GetLangStr("MSG_CLOSE_DOORS_FOR_KEYLOCK"));
			return;
		}
		if(!robostar->KeyLock(true))
			ShowMessage(GetLangStr("MSG_KEYLOCK_REJECTED"));
	}
}
//---------------------------------------------------------------------------

int __fastcall TBaseForm::StringToInt(UnicodeString str, int def)
{
	int iVal;
	if(TryStrToInt(str, iVal) == true) return iVal;
	else return def;
}
//---------------------------------------------------------------------------
double __fastcall TBaseForm::StringToDouble(UnicodeString str, double def)
{
	double dVal;
	if(TryStrToFloat(str, dVal) == true) return dVal;
	else return def;
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::btnKeyUnLockClick(TObject *Sender)
{
	if(MainForm->equipMode != modeManual)
	{
        ShowMessage(GetLangStr("MSG_UNLOCK_KEY"));
	}
	else{
		if(!robostar->KeyLock(false))
			ShowMessage(GetLangStr("MSG_KEYLOCK_RELEASE_REQUIREMENTS"));
	}
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::btnBypassOnClick(TObject *Sender)
{
	// Y003C ON is permitted only after KEYLOCK set is fully confirmed.
	if(!robostar->CanEnableBypassSol())
	{
		bool keyLockSet = robostar->gripper.DOOR_LEFT_CLOSE
			&& robostar->gripper.DOOR_RIGHT_CLOSE && robostar->IsKeyLockActive();
		if(!keyLockSet || robostar->IsBypassActive())
			ShowMessage(GetLangStr("MSG_BYPASS_SET_FIRST"));
		else
			ShowMessage(GetLangStr("MSG_BYPASS_KEY_ON"));
		return;
	}
	if(!robostar->Bypass(true))
		ShowMessage(GetLangStr("MSG_BYPASS_REJECTED"));
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::btnSafetyResetClick(TObject *Sender)
{
	if(!robostar->RequestSafetyResetPulse())
		ShowMessage(GetLangStr("MSG_CCLINK_NOT_CONNECTED"));
}
//---------------------------------------------------------------------------
//--------------------     언어 변경          -------------------------------
//---------------------------------------------------------------------------
void __fastcall TBaseForm::RadioButton1Click(TObject *Sender)
{
   	TRadioButton *rbt = (TRadioButton *)Sender;
	if(rbt->Checked)
	{
        if(rbt->Tag == 0) ReadLanguage("KO");
        else if(rbt->Tag == 1) ReadLanguage("EN");
        else if(rbt->Tag == 2) ReadLanguage("HI");

        ChangeLanguage();
		ServoAlarmListForm->LanguageChange(rbt->Tag);
	}
}
//---------------------------------------------------------------------------
UnicodeString __fastcall TBaseForm::GetLangStr(AnsiString key)
{
    // 이미 메모리에 로드된 LangDict에서 값만 찾아서 반환 (매우 빠름)
    UnicodeString value = LangDict->Values[key];
	value = StringReplace(value, L"\\r\\n", L"\r\n", TReplaceFlags() << rfReplaceAll);
    return value.IsEmpty() ? L" - " + key : value;
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::ReadLanguage(AnsiString newLang)
{
    CurrentLanguage = newLang;
    LangDict = new TStringList();
    LangDict->Clear();
    std::unique_ptr<TResourceStream> res(new TResourceStream((NativeUInt)HInstance, newLang + L"_DATA", RT_RCDATA));
    LangDict->LoadFromStream(res.get(), TEncoding::UTF8);
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::ChangeLanguage()
{
    //* Main Form
	MainForm->lblTitle->Caption = GetLangStr("CAP_TITLE");
	MainForm->autoBtn->Caption = GetLangStr("CAP_AUTO");
	MainForm->playBtn->Caption = GetLangStr("CAP_START");
	MainForm->stopBtn->Caption = GetLangStr("CAP_STOP");
	MainForm->manualBtn->Caption = GetLangStr("CAP_MANUAL");
	MainForm->pause_stopBtn->Caption = GetLangStr("CAP_PAUSE");
	MainForm->ppause->Caption = GetLangStr("CAP_PAUSE");
	MainForm->pause_startBtn->Caption = GetLangStr("CAP_RESTART");
	MainForm->buzzerBtn->Caption = GetLangStr("CAP_BUZZER_STOP");
	MainForm->pemergency->Caption = GetLangStr("CAP_EMG_STOP");
	MainForm->pnlErrCode->Caption = GetLangStr("CAP_ERR_CODE");
	MainForm->lblManualControl->Caption = GetLangStr("CAP_MANUAL_TITLE");
	MainForm->trayout_targetBtn->Status->Caption = GetLangStr("CAP_TARGET_TRAY");
	MainForm->trayout_targetBtn->Caption = GetLangStr("CAP_TRAY_OUT");
	MainForm->trayout_srcBtn->Status->Caption = GetLangStr("CAP_SOURCE_TRAY");
	MainForm->trayout_srcBtn->Caption = GetLangStr("CAP_TRAY_OUT");
	MainForm->lblXPos->Caption = GetLangStr("CAP_X_POS");
	MainForm->lblYPos->Caption = GetLangStr("CAP_Y_POS");
	MainForm->lblZPos->Caption = GetLangStr("CAP_Z_POS");
	MainForm->lblSpeedXY->Caption = GetLangStr("CAP_SPEED");
	MainForm->lblSourceTrayInfo->Caption = GetLangStr("CAP_SOURCE_TRAY_INFO");
	MainForm->lblTargetTrayInfo->Caption = GetLangStr("CAP_TARGET_TRAY_INFO");
	MainForm->psrcArrive->Caption = GetLangStr("CAP_ARRIVE");
	MainForm->psrcReady->Caption = GetLangStr("CAP_CENTERING");
	MainForm->ptargetReady->Caption = GetLangStr("CAP_CENTERING");
	MainForm->pwork1->Caption = GetLangStr("CAP_READY");
	MainForm->pwork2->Caption = GetLangStr("CAP_READY");
	MainForm->psrcOut->Caption = GetLangStr("CAP_OUT");
	MainForm->ptargetOut->Caption = GetLangStr("CAP_OUT");
	MainForm->lblNgList->Caption = GetLangStr("CAP_NG_LIST");
	MainForm->pnlNgQuantity->Caption = GetLangStr("CAP_NG_COUNT");
	MainForm->pnlNgLimit->Caption = GetLangStr("CAP_NG_LIMIT_COUNT");
	MainForm->btnApplyNgLimitCount->Caption = GetLangStr("CAP_APPLY");
	MainForm->CLR1->Caption = GetLangStr("CAP_GRIPPER") + " #1";
	MainForm->CLR2->Caption = GetLangStr("CAP_GRIPPER") + " #2";
	MainForm->pflow1->Caption = GetLangStr("CAP_FLOW");
	MainForm->popen1->Caption = GetLangStr("CAP_OPEN");
	MainForm->pclose1->Caption = GetLangStr("CAP_CLOSE");
	MainForm->pcell1->Caption = GetLangStr("CAP_CELL");
	MainForm->puse1->Caption = GetLangStr("CAP_STOP_USING");
	MainForm->puse2->Caption = GetLangStr("CAP_STOP_USING");
	MainForm->pdn1->Caption = GetLangStr("CAP_DOWN");
	MainForm->pup1->Caption = GetLangStr("CAP_UP");
	MainForm->pdn2->Caption = GetLangStr("CAP_DOWN");
	MainForm->pup2->Caption = GetLangStr("CAP_UP");
	MainForm->pflow2->Caption = GetLangStr("CAP_FLOW");
	MainForm->popen2->Caption = GetLangStr("CAP_OPEN");
	MainForm->pclose2->Caption = GetLangStr("CAP_CLOSE");
	MainForm->pcell2->Caption = GetLangStr("CAP_CELL");
	MainForm->Panel42->Caption = GetLangStr("CAP_CODE");
	MainForm->Panel43->Caption = GetLangStr("CAP_SOURCE");
	MainForm->Panel45->Caption = GetLangStr("CAP_TARGET");
	MainForm->pflow4->Caption = GetLangStr("CAP_FLOW");
	MainForm->pnlCode->Caption = GetLangStr("CAP_CODE");
	MainForm->pnlTarget->Caption = GetLangStr("CAP_TARGET");
	MainForm->pnlSource->Caption = GetLangStr("CAP_SOURCE");
    MainForm->lblLogTitle->Caption = GetLangStr("CAP_EQUIPMENT_LOG");
	MainForm->Panel5->Caption = GetLangStr("CAP_PROGRESS_INFO");
	MainForm->Panel7->Caption = GetLangStr("CAP_INOUT_INFO");
	MainForm->Panel17->Caption = GetLangStr("CAP_ROBOT_INFO");

	MainForm->pnlTargetTrayTitle->Caption = GetLangStr("CAP_TARGET_TRAY");
	MainForm->pnlTargetRemaining->Caption = GetLangStr("CAP_REMAINING_COUNT");
	MainForm->pnlSourceTrayTitle->Caption = GetLangStr("CAP_SOURCE_TRAY");
	MainForm->pnlSourceRemaining->Caption = GetLangStr("CAP_EJECTING_COUNT");
	MainForm->pnlCh->Caption = GetLangStr("CAP_CHANNEL");
	MainForm->pnlSource2->Caption = GetLangStr("CAP_SOURCE");
	MainForm->pnlNgCode->Caption = GetLangStr("CAP_NG_CODE");
	MainForm->zone4->Caption = GetLangStr("CAP_ETC");
	MainForm->AdvSmoothToggleButton_InitWork->Caption = GetLangStr("CAP_INIT");

    //* Teaching Form
    teachForm->lblMsgRobot->Caption = GetLangStr("CAP_ROBOT_MSG");
    teachForm->lblSourceTray->Caption = GetLangStr("CAP_SOURCE_TRAY");
    teachForm->lblTargetTray->Caption = GetLangStr("CAP_TARGET_TRAY");
    teachForm->lblServoSetting->Caption = GetLangStr("CAP_SERVO_SETTING");
    teachForm->lblServoInfo->Caption = GetLangStr("CAP_SERVO_STATUS");
    teachForm->btnApplyTeaching->Caption = GetLangStr("CAP_APPLY_TEACHING_LINE1")
		+ "\r\n" + GetLangStr("CAP_APPLY_TEACHING_LINE2");
    teachForm->pnlSelectGripper->Caption = GetLangStr("CAP_SELECT_GRIPPER");
    teachForm->lblJogControl->Caption = GetLangStr("CAP_JOG_CONTROL");
    teachForm->lblXAxis->Caption = GetLangStr("CAP_X_AXIS");
    teachForm->lblYAxis->Caption = GetLangStr("CAP_Y_AXIS");
    teachForm->lblZAxis->Caption = GetLangStr("CAP_Z_AXIS");
    teachForm->pnlSourceZAxis->Caption = GetLangStr("CAP_Z_AXIS");
    teachForm->pnlTargetZAxis->Caption = GetLangStr("CAP_Z_AXIS");
    teachForm->pnlXAxis->Caption = GetLangStr("CAP_X_AXIS");
    teachForm->pnlYAxis->Caption = GetLangStr("CAP_Y_AXIS");
    teachForm->pnlZAxis->Caption = GetLangStr("CAP_Z_AXIS");
    teachForm->pnlSpeedXY->Caption = GetLangStr("CAP_SPEED_XY");
    teachForm->pnlAcc->Caption = GetLangStr("CAP_ACC");
    teachForm->pnlDcc->Caption = GetLangStr("CAP_DCC");
    teachForm->pnlErrCode->Caption = GetLangStr("CAP_ERR_CODE");
    teachForm->waitBtn->Caption = GetLangStr("CAP_WAIT_POS_LINE1")
		+ "\r\n" + GetLangStr("CAP_WAIT_POS_LINE2");
    teachForm->AdvSmoothButton_Zup->Caption = GetLangStr("CAP_Z_UP");
    teachForm->stopBtn->Caption = GetLangStr("CAP_STOP_MOVING");
    teachForm->pnlSpeed->Caption = GetLangStr("CAP_SPEED");
    teachForm->pnlSettingSpeed->Caption = GetLangStr("CAP_SETTING_SPEED");
    teachForm->lblJogControl->Caption = GetLangStr("CAP_JOG_CONTROL");
    teachForm->lblOpenClose->Caption = GetLangStr("CAP_OPEN_CLOSE");
    teachForm->AdvSmoothButton_LoadFactorInfo->Caption = GetLangStr("CAP_LOAD_FACTOR");
    teachForm->lblLoadFactorTitle->Caption = GetLangStr("CAP_LOAD");
	teachForm->btnJogSpeed->Caption = GetLangStr("CAP_JOG_SPEED");
	teachForm->btnZAxisDown->Caption = GetLangStr("CAP_Z_DOWN");
	teachForm->Label57->Caption = GetLangStr("CAP_SERVO_MOVING");
	teachForm->Label59->Caption = GetLangStr("CAP_SERVO_MOVING");
	teachForm->Label50->Caption = GetLangStr("CAP_MODEL_TEACHING");
	teachForm->pselect->Caption = GetLangStr("CAP_SELECTED_COLOR");
	teachForm->lblUpAllGripper->Caption = GetLangStr("CAP_ALL_UP");
	teachForm->lblDownAllGripper->Caption = GetLangStr("CAP_ALL_DOWN");
	teachForm->lblOpenAllGripper->Caption = GetLangStr("CAP_ALL_OPEN");
	teachForm->lblCloseAllGripper->Caption = GetLangStr("CAP_ALL_CLOSE");
	teachForm->CLR1->Caption = GetLangStr("CAP_GRIPPER") + " #1";
	teachForm->lblUpDown->Caption = GetLangStr("CAP_UP_DOWN");
	teachForm->disableChk1->Caption = GetLangStr("CAP_UNSING");
    teachForm->pflow1->Caption = GetLangStr("CAP_FLOW");
    teachForm->popen1->Caption = GetLangStr("CAP_OPEN");
    teachForm->pclose1->Caption = GetLangStr("CAP_CLOSE");
    teachForm->pcell1->Caption = GetLangStr("CAP_CELL");
	teachForm->pdn1->Caption = GetLangStr("CAP_DOWN");
	teachForm->pup1->Caption = GetLangStr("CAP_UP");
	TLabel *gripperLabels[5] = { teachForm->Label14, teachForm->Label17,
		teachForm->Label20, teachForm->Label23, teachForm->Label26 };
	TLabel *upDownLabels[5] = { teachForm->Label15, teachForm->Label18,
		teachForm->Label21, teachForm->Label24, teachForm->Label27 };
	TLabel *openCloseLabels[5] = { teachForm->Label16, teachForm->Label19,
		teachForm->Label22, teachForm->Label25, teachForm->Label28 };
	TPanel *downPanels[5] = { teachForm->pdn2, teachForm->pdn3, teachForm->pdn4,
		teachForm->pdn5, teachForm->pdn6 };
	TPanel *upPanels[5] = { teachForm->pup2, teachForm->pup3, teachForm->pup4,
		teachForm->pup5, teachForm->pup6 };
	TPanel *flowPanels[5] = { teachForm->pflow2, teachForm->pflow3, teachForm->pflow4,
		teachForm->pflow5, teachForm->pflow6 };
	TPanel *openPanels[5] = { teachForm->popen2, teachForm->popen3, teachForm->popen4,
		teachForm->popen5, teachForm->popen6 };
	TPanel *closePanels[5] = { teachForm->pclose2, teachForm->pclose3, teachForm->pclose4,
		teachForm->pclose5, teachForm->pclose6 };
	TPanel *cellPanels[5] = { teachForm->pcell2, teachForm->pcell3, teachForm->pcell4,
		teachForm->pcell5, teachForm->pcell6 };
	TCheckBox *unusedChecks[5] = { teachForm->disableChk2, teachForm->disableChk3,
		teachForm->disableChk4, teachForm->disableChk5, teachForm->disableChk6 };
	for(int i = 0; i < 5; i++) {
		gripperLabels[i]->Caption = GetLangStr("CAP_GRIPPER") + " #" + IntToStr(i + 2);
		upDownLabels[i]->Caption = GetLangStr("CAP_UP_DOWN");
		openCloseLabels[i]->Caption = GetLangStr("CAP_OPEN_CLOSE");
		downPanels[i]->Caption = GetLangStr("CAP_DOWN");
		upPanels[i]->Caption = GetLangStr("CAP_UP");
		flowPanels[i]->Caption = GetLangStr("CAP_FLOW");
		openPanels[i]->Caption = GetLangStr("CAP_OPEN");
		closePanels[i]->Caption = GetLangStr("CAP_CLOSE");
		cellPanels[i]->Caption = GetLangStr("CAP_CELL");
		unusedChecks[i]->Caption = GetLangStr("CAP_UNSING");
	}

	//* Base / Door / Config Forms
	Button1->Caption = GetLangStr("CAP_CONFIGURATION");
	AdvSmoothButton3->Caption = GetLangStr("CAP_SERVO_ALARM_LIST");
	btnSafetyReset->Caption = GetLangStr("CAP_SAFETY_RESET");
	lblManualOperation->Caption = GetLangStr("CAP_SMOKE_DETECTOR");
	Label3->Caption = GetLangStr("CAP_CURRENT_VALUES");
	rbAlarmClear->Caption = GetLangStr("CAP_ALARM_CLEAR");
	btnSetValue->Caption = GetLangStr("CAP_SET_VALUE");
	Label8->Caption = GetLangStr("CAP_ALARM_FLAG");
	Label6->Caption = GetLangStr("CAP_OUTPUT_STATE");
	btnKeyLock->Caption = GetLangStr("CAP_KEYLOCK_SET");
	btnKeyUnLock->Caption = GetLangStr("CAP_KEYLOCK_RELEASE");
	doorForm->Caption = GetLangStr("CAP_ALARM_OCCURRED");
	doorForm->Label1->Caption = GetLangStr("CAP_ALARM_INFO");
	doorForm->Label4->Caption = GetLangStr("CAP_ALARM_INFO");
	doorForm->MainErr1->Caption = GetLangStr("CAP_DOOR") + " #1 " + GetLangStr("CAP_OPEN");
	doorForm->MainErr2->Caption = GetLangStr("CAP_DOOR") + " #2 " + GetLangStr("CAP_OPEN");
	doorForm->MainErr3->Caption = GetLangStr("CAP_EMG_STOP");
	doorForm->MainErr5->Caption = GetLangStr("CAP_KEYLOCK_RELEASE");
	doorForm->okBtn->Caption = GetLangStr("CAP_OK");
	doorForm->AdvSmoothButton4->Caption = GetLangStr("CAP_BUZZER_STOP");
	doorForm->lblSafetyResetGuide->Caption = GetLangStr("CAP_PRESS_RESET_BUTTON");
	doorForm->StaticText1->Caption = GetLangStr("CAP_CLOSE_DOOR_GUIDE");
	doorForm->StaticText2->Caption = GetLangStr("CAP_RELEASE_EMERGENCY_SWITCH");
	doorForm->lblRecoverySequence->Caption = GetLangStr("CAP_RECOVERY_SEQUENCE");
	doorForm->btnSafetyResetDoor->Caption = GetLangStr("CAP_SAFETY_RESET");
	doorForm->Label3->Caption = GetLangStr("CAP_ALARM_POSITION");
	doorForm->perr1->Caption = GetLangStr("CAP_DOOR") + " #1";
	doorForm->perr2->Caption = GetLangStr("CAP_DOOR") + " #2";
	doorForm->perr3->Caption = GetLangStr("CAP_EMG_STOP");
	doorForm->Panel61->Caption = GetLangStr("CAP_PASSWORD");
	doorForm->cancelBtn2->Caption = GetLangStr("CAP_CANCEL");
	doorForm->PasswordBtn->Caption = GetLangStr("CAP_OK");
	doorForm->btnSetKEYLOCK->Caption = GetLangStr("CAP_KEYLOCK_SET");
	doorForm->btnKeyUnlock->Caption = GetLangStr("CAP_KEYLOCK_RELEASE");
	doorForm->btnSetBypass->Caption = GetLangStr("CAP_BYPASS_SET");
	doorForm->btnServoOpen->Caption = GetLangStr("CAP_SERVO_OPEN_CMD");
	doorForm->Label6->Caption = GetLangStr("CAP_GRIPPER_OPEN");
	doorForm->btnGripper1Open->Caption = GetLangStr("CAP_GRIPPER") + " #1 " + GetLangStr("CAP_OPEN");
	doorForm->btnGripper2Open->Caption = GetLangStr("CAP_GRIPPER") + " #2 " + GetLangStr("CAP_OPEN");
	doorForm->stopBtn->Caption = GetLangStr("CAP_STOP_MOVING");
	ConfigForm->Caption = GetLangStr("CAP_CONFIGURATION");
	ConfigForm->AdvSmoothLabel1->Caption->Text = GetLangStr("CAP_CONFIGURATION");
	ConfigForm->btnConMes->Caption = GetLangStr("CAP_CONNECT");
	ConfigForm->btnDisconMes->Caption = GetLangStr("CAP_DISCONNECT");
	ConfigForm->GroupBox3->Caption = GetLangStr("CAP_STAGE_INFO");
	ConfigForm->AdvSmoothButton2->Caption = GetLangStr("CAP_SAVE");
	ConfigForm->AdvSmoothButton3->Caption = GetLangStr("CAP_CANCEL");
	ConfigForm->btnConPLC->Caption = GetLangStr("CAP_CONNECT");
	ConfigForm->btnDisconPLC->Caption = GetLangStr("CAP_DISCONNECT");
	ConfigForm->GroupBoxBcr->Caption = GetLangStr("CAP_BARCODE_READER") + " (SRX100W)";
	ConfigForm->btnBcrSourceConn->Caption = GetLangStr("CAP_SOURCE") + " " + GetLangStr("CAP_CONNECT");
	ConfigForm->btnBcrSourceDisconn->Caption = GetLangStr("CAP_SOURCE") + " " + GetLangStr("CAP_DISCONNECT");
	ConfigForm->btnBcrTargetConn->Caption = GetLangStr("CAP_TARGET") + " " + GetLangStr("CAP_CONNECT");
	ConfigForm->btnBcrTargetDisconn->Caption = GetLangStr("CAP_TARGET") + " " + GetLangStr("CAP_DISCONNECT");
	ConfigForm->GroupBoxSmoke->Caption = GetLangStr("CAP_SMOKE_DETECTOR");
	ConfigForm->btnSmokeConn->Caption = GetLangStr("CAP_CONNECT");
	ConfigForm->btnSmokeDisconn->Caption = GetLangStr("CAP_DISCONNECT");
	ConfigForm->GroupBox5->Caption = GetLangStr("CAP_MACHINE_SELECT");
	ConfigForm->Panel6->Caption = GetLangStr("CAP_MODEL_NO");
	ConfigForm->GroupBox2->Caption = GetLangStr("CAP_Z_UP_ON_MOVE");
	ConfigForm->chkZAxisUp->Caption = GetLangStr("CAP_Z_UP");
	ConfigForm->GroupBoxTargetUnload->Caption = GetLangStr("CAP_TARGET_UNLOAD_SETTING");
	ConfigForm->lblTargetUnloadCount->Caption = GetLangStr("CAP_TARGET_UNLOAD_COUNT");
	ConfigForm->lblTargetUnloadDisabled->Caption = GetLangStr("CAP_TARGET_UNLOAD_DISABLED");
	MainForm->UpdateTargetTrayExchangePanel();

    //* LoadFactor Form
	loadfactorForm->Caption = GetLangStr("CAP_LOAD_FACTOR_INFO");
    loadfactorForm->pnlXAxis->Caption = GetLangStr("CAP_X_AXIS");
    loadfactorForm->pnlYAxis->Caption = GetLangStr("CAP_Y_AXIS");
    loadfactorForm->pnlZAxis->Caption = GetLangStr("CAP_Z_AXIS");
    loadfactorForm->pnlLimitValue->Caption = GetLangStr("CAP_LIMIT_VALUE");
    loadfactorForm->AdvSmoothButton_Save->Caption = GetLangStr("CAP_SAVE");
    loadfactorForm->AdvSmoothButton_Cancel->Caption = GetLangStr("CAP_CANCEL");

    //* SERVO Alarm List
	ServoAlarmListForm->Caption = GetLangStr("CAP_SERVO_ALARM_LIST");
    ServoAlarmListForm->gbSystemAlarm->Caption = GetLangStr("CAP_SYSTEM_ALARM");
    ServoAlarmListForm->gbServoAlarm->Caption = GetLangStr("CAP_SERVO_ALARM");
    ServoAlarmListForm->gbOPAlarm->Caption = GetLangStr("CAP_OP_ALARM");
    ServoAlarmListForm->gbSystemError->Caption = GetLangStr("CAP_SYSTEM_ERROR");

	//* Common Error Forms
	ErrorForm_bcr->Caption = GetLangStr("CAP_BARCODE_ERROR");
	ErrorForm_bcr->ignoreBtn->Caption = GetLangStr("CAP_NORMAL_PROGRESS");
	ErrorForm_bcr->btnScan->Caption = GetLangStr("CAP_SCAN_BARCODE_60S");
	ErrorForm_bcr->AdvSmoothButton5->Caption = GetLangStr("CAP_BUZZER_STOP");
	ErrorForm_limit->Caption = GetLangStr("CAP_NG_LIMITS");
	ErrorForm_limit->ignoreBtn->Caption = GetLangStr("CAP_NORMAL_PROGRESS");
	ErrorForm_limit->AdvSmoothButton1->Caption = GetLangStr("CAP_TRAY_OUT");
	ErrorForm_limit->AdvSmoothButton5->Caption = GetLangStr("CAP_BUZZER_STOP");
	ErrorForm_mes->Caption = GetLangStr("CAP_MES_ERROR");
	ErrorForm_mes->ignoreBtn->Caption = GetLangStr("CAP_FORCED_TRAY_OUT");
	ErrorForm_mes->retryBtn->Caption = GetLangStr("CAP_RETRY");
	ErrorForm_mes->AdvSmoothButton5->Caption = GetLangStr("CAP_BUZZER_STOP");

    //* FormError_insert
	ErrorForm_insert->Caption = GetLangStr("CAP_INSERT_ERROR");
	ErrorForm_insert->ignoreBtn->Caption = GetLangStr("CAP_INSERT_COMPLETE");
	ErrorForm_insert->retryBtn->Caption = GetLangStr("CAP_RETRY");
	ErrorForm_insert->AdvSmoothButton1->Caption = GetLangStr("CAP_MANUAL_SWITCHING");
	ErrorForm_insert->AdvSmoothButton5->Caption = GetLangStr("CAP_BUZZER_STOP");
    ErrorForm_insert->lblTitle->Caption = GetLangStr("CAP_ERROR_CHANNEL_INFO");
    ErrorForm_insert->pnlGripperNo->Caption = GetLangStr("CAP_GRIPPER_NO");
    ErrorForm_insert->pnlSourceChannel->Caption = GetLangStr("CAP_SOURCE_CHANNEL");
    ErrorForm_insert->pnlTargetChannel->Caption = GetLangStr("CAP_TARGET_CHANNEL");
    ErrorForm_insert->pnlNgCode->Caption = GetLangStr("CAP_NG_CODE");
    ErrorForm_insert->btnMoveSource->Caption = GetLangStr("CAP_MOVE");
    ErrorForm_insert->btnMoveTarget->Caption = GetLangStr("CAP_MOVE");
    ErrorForm_insert->CLR1->Caption = GetLangStr("CAP_GRIPPER1");
    ErrorForm_insert->lblOpenClose->Caption = GetLangStr("CAP_OPEN_CLOSE");
    ErrorForm_insert->pdn1->Caption = GetLangStr("CAP_DOWN");
    ErrorForm_insert->pup1->Caption = GetLangStr("CAP_UP");
    ErrorForm_insert->pflow1->Caption = GetLangStr("CAP_FLOW");
    ErrorForm_insert->popen1->Caption = GetLangStr("CAP_OPEN");
    ErrorForm_insert->pclose1->Caption = GetLangStr("CAP_CLOSE");
    ErrorForm_insert->pcell1->Caption = GetLangStr("CAP_CELL");

    //* FormError_eject
	ErrorForm_eject->Caption = GetLangStr("CAP_EJECT_ERROR");
	ErrorForm_eject->ignoreBtn->Caption = GetLangStr("CAP_EJECT_COMPLETE");
	ErrorForm_eject->retryBtn->Caption = GetLangStr("CAP_RETRY");
	ErrorForm_eject->AdvSmoothButton3->Caption = GetLangStr("CAP_MANUAL_SWITCHING");
	ErrorForm_eject->AdvSmoothButton5->Caption = GetLangStr("CAP_BUZZER_STOP");
    ErrorForm_eject->lblTitle->Caption = GetLangStr("CAP_ERROR_CHANNEL_INFO");
    ErrorForm_eject->pnlGripperNo->Caption = GetLangStr("CAP_GRIPPER_NO");
    ErrorForm_eject->pnlSourceChannel->Caption = GetLangStr("CAP_SOURCE_CHANNEL");
    ErrorForm_eject->pnlTargetChannel->Caption = GetLangStr("CAP_TARGET_CHANNEL");
    ErrorForm_eject->pnlNgCode->Caption = GetLangStr("CAP_NG_CODE");
    ErrorForm_eject->btnMoveSource->Caption = GetLangStr("CAP_MOVE");
    ErrorForm_eject->btnMoveTarget->Caption = GetLangStr("CAP_MOVE");
    ErrorForm_eject->CLR1->Caption = GetLangStr("CAP_GRIPPER1");
    ErrorForm_eject->lblOpenClose->Caption = GetLangStr("CAP_OPEN_CLOSE");
    ErrorForm_eject->pdn1->Caption = GetLangStr("CAP_DOWN");
    ErrorForm_eject->pup1->Caption = GetLangStr("CAP_UP");
    ErrorForm_eject->pflow1->Caption = GetLangStr("CAP_FLOW");
    ErrorForm_eject->popen1->Caption = GetLangStr("CAP_OPEN");
    ErrorForm_eject->pclose1->Caption = GetLangStr("CAP_CLOSE");
    ErrorForm_eject->pcell1->Caption = GetLangStr("CAP_CELL");

	TLabel *insertGrip[5] = { ErrorForm_insert->Label14, ErrorForm_insert->Label17,
		ErrorForm_insert->Label20, ErrorForm_insert->Label23, ErrorForm_insert->Label26 };
	TLabel *insertUpDown[5] = { ErrorForm_insert->Label15, ErrorForm_insert->Label18,
		ErrorForm_insert->Label21, ErrorForm_insert->Label24, ErrorForm_insert->Label27 };
	TLabel *insertOpenClose[5] = { ErrorForm_insert->Label16, ErrorForm_insert->Label19,
		ErrorForm_insert->Label22, ErrorForm_insert->Label25, ErrorForm_insert->Label28 };
	TPanel *insertDown[5] = { ErrorForm_insert->pdn2, ErrorForm_insert->pdn3, ErrorForm_insert->pdn4, ErrorForm_insert->pdn5, ErrorForm_insert->pdn6 };
	TPanel *insertUp[5] = { ErrorForm_insert->pup2, ErrorForm_insert->pup3, ErrorForm_insert->pup4, ErrorForm_insert->pup5, ErrorForm_insert->pup6 };
	TPanel *insertFlow[5] = { ErrorForm_insert->pflow2, ErrorForm_insert->pflow3, ErrorForm_insert->pflow4, ErrorForm_insert->pflow5, ErrorForm_insert->pflow6 };
	TPanel *insertOpen[5] = { ErrorForm_insert->popen2, ErrorForm_insert->popen3, ErrorForm_insert->popen4, ErrorForm_insert->popen5, ErrorForm_insert->popen6 };
	TPanel *insertClose[5] = { ErrorForm_insert->pclose2, ErrorForm_insert->pclose3, ErrorForm_insert->pclose4, ErrorForm_insert->pclose5, ErrorForm_insert->pclose6 };
	TPanel *insertCell[5] = { ErrorForm_insert->pcell2, ErrorForm_insert->pcell3, ErrorForm_insert->pcell4, ErrorForm_insert->pcell5, ErrorForm_insert->pcell6 };
	TLabel *ejectGrip[5] = { ErrorForm_eject->Label14, ErrorForm_eject->Label17,
		ErrorForm_eject->Label20, ErrorForm_eject->Label23, ErrorForm_eject->Label26 };
	TLabel *ejectUpDown[5] = { ErrorForm_eject->Label15, ErrorForm_eject->Label18,
		ErrorForm_eject->Label21, ErrorForm_eject->Label24, ErrorForm_eject->Label27 };
	TLabel *ejectOpenClose[5] = { ErrorForm_eject->Label16, ErrorForm_eject->Label19,
		ErrorForm_eject->Label22, ErrorForm_eject->Label25, ErrorForm_eject->Label28 };
	TPanel *ejectDown[5] = { ErrorForm_eject->pdn2, ErrorForm_eject->pdn3, ErrorForm_eject->pdn4, ErrorForm_eject->pdn5, ErrorForm_eject->pdn6 };
	TPanel *ejectUp[5] = { ErrorForm_eject->pup2, ErrorForm_eject->pup3, ErrorForm_eject->pup4, ErrorForm_eject->pup5, ErrorForm_eject->pup6 };
	TPanel *ejectFlow[5] = { ErrorForm_eject->pflow2, ErrorForm_eject->pflow3, ErrorForm_eject->pflow4, ErrorForm_eject->pflow5, ErrorForm_eject->pflow6 };
	TPanel *ejectOpen[5] = { ErrorForm_eject->popen2, ErrorForm_eject->popen3, ErrorForm_eject->popen4, ErrorForm_eject->popen5, ErrorForm_eject->popen6 };
	TPanel *ejectClose[5] = { ErrorForm_eject->pclose2, ErrorForm_eject->pclose3, ErrorForm_eject->pclose4, ErrorForm_eject->pclose5, ErrorForm_eject->pclose6 };
	TPanel *ejectCell[5] = { ErrorForm_eject->pcell2, ErrorForm_eject->pcell3, ErrorForm_eject->pcell4, ErrorForm_eject->pcell5, ErrorForm_eject->pcell6 };
	for(int i = 0; i < 5; i++) {
		insertGrip[i]->Caption = GetLangStr("CAP_GRIPPER") + " #" + IntToStr(i + 2);
		insertUpDown[i]->Caption = GetLangStr("CAP_UP_DOWN");
		insertOpenClose[i]->Caption = GetLangStr("CAP_OPEN_CLOSE");
		insertDown[i]->Caption = GetLangStr("CAP_DOWN");
		insertUp[i]->Caption = GetLangStr("CAP_UP");
		insertFlow[i]->Caption = GetLangStr("CAP_FLOW");
		insertOpen[i]->Caption = GetLangStr("CAP_OPEN");
		insertClose[i]->Caption = GetLangStr("CAP_CLOSE");
		insertCell[i]->Caption = GetLangStr("CAP_CELL");
		ejectGrip[i]->Caption = GetLangStr("CAP_GRIPPER") + " #" + IntToStr(i + 2);
		ejectUpDown[i]->Caption = GetLangStr("CAP_UP_DOWN");
		ejectOpenClose[i]->Caption = GetLangStr("CAP_OPEN_CLOSE");
		ejectDown[i]->Caption = GetLangStr("CAP_DOWN");
		ejectUp[i]->Caption = GetLangStr("CAP_UP");
		ejectFlow[i]->Caption = GetLangStr("CAP_FLOW");
		ejectOpen[i]->Caption = GetLangStr("CAP_OPEN");
		ejectClose[i]->Caption = GetLangStr("CAP_CLOSE");
		ejectCell[i]->Caption = GetLangStr("CAP_CELL");
	}
}
void __fastcall TBaseForm::AdvSmoothButton4Click(TObject *Sender)
{
    InterfaceForm->Position = poDesigned;
	InterfaceForm->Left = 300;
	InterfaceForm->Top = 85;
	InterfaceForm->Visible = true;
    InterfaceForm->WindowState = wsNormal;
    InterfaceForm->BringToFront();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void __fastcall TBaseForm::pbcrClick(TObject *Sender)
{
	TComponent *comp = dynamic_cast<TComponent *>(Sender);
	if(comp != NULL && comp->Tag >= 0 && comp->Tag < 2)
		SelectedBcrIndex = comp->Tag;

	AnsiString readerName = (SelectedBcrIndex == 0) ? "Source Tray BCR" : "Target Tray BCR";
	Label9->Caption = readerName + " - Keyence SR-X100W";
	if(memoBcr != NULL) {
		memoBcr->Lines->Clear();
		memoBcr->Lines->Add(readerName);
		memoBcr->Lines->Add("IP : " + BaseForm->config.bcrIp[SelectedBcrIndex] + ":" + IntToStr(BaseForm->config.bcrPort[SelectedBcrIndex]));
	}

	grp_bcr->Visible = !grp_bcr->Visible;
	grp_bcr->BringToFront();
	grp_bcr->Left = 1498;
	grp_bcr->Top = 71;

	if(grp_tmperature != NULL)
		grp_tmperature->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::psmokedetectorClick(TObject *Sender)
{
	grp_tmperature->Visible = !grp_tmperature->Visible;
	grp_tmperature->BringToFront();
	grp_tmperature->Left = 1498;
	grp_tmperature->Top = 71;

	if(grp_bcr != NULL)
		grp_bcr->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::btnSetValueClick(TObject *Sender)
{
	if(MainForm->comSmoke[0] == NULL)
		return;

	UnicodeString text = "0x" + editTempValue->Text.Trim();
	short tempvalue = 0;
	if(!text.IsEmpty())
		tempvalue = (short)wcstoul(text.w_str(), NULL, 0);

	short addr = 0x1002;
	if(rbSetTemperatureOffset->Checked) addr = 0x1002;
	else if(rbSetTemperatureWarningSV->Checked) addr = 0x1003;
	else if(rbSetTemperatureDangerSV->Checked) addr = 0x1004;
	else if(rbAlarmClear->Checked) addr = 0x1005;

	if(addr == 0x1005)
		MainForm->comSmoke[0]->ClearAlarm();
	else
		MainForm->comSmoke[0]->setTsdData(addr, tempvalue);
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::rbAlarmClearClick(TObject *Sender)
{
	if(MainForm->comSmoke[0] != NULL)
		MainForm->comSmoke[0]->ClearAlarm();
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::pnlTempPVClick(TObject *Sender)
{
	if(MainForm->comSmoke[0] != NULL)
		MainForm->comSmoke[0]->GetTsdData_Modbus();
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::pnlTempOffsetClick(TObject *Sender)
{
	if(MainForm->comSmoke[0] != NULL)
		MainForm->comSmoke[0]->chkTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::btnTriggerOnClick(TObject *Sender)
{
	AnsiString readerName = (SelectedBcrIndex == 0) ? "Source Tray BCR" : "Target Tray BCR";
	if(memoBcr != NULL) memoBcr->Lines->Add(readerName + " Trigger ON");
	if(MainForm->comBcr[SelectedBcrIndex] != NULL)
		MainForm->comBcr[SelectedBcrIndex]->TriggerOn();
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::btnTriggerOffClick(TObject *Sender)
{
	AnsiString readerName = (SelectedBcrIndex == 0) ? "Source Tray BCR" : "Target Tray BCR";
	if(memoBcr != NULL) memoBcr->Lines->Add(readerName + " Trigger OFF");
	if(MainForm->comBcr[SelectedBcrIndex] != NULL)
		MainForm->comBcr[SelectedBcrIndex]->TriggerOff();
}
//---------------------------------------------------------------------------


