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

//    ChangeLanguage();
	RadioButton2->Checked = true;
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	MainForm->Close();
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
		robostar->KeyLock(true);
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
		robostar->KeyLock(false);
	}
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::btnBypassOnClick(TObject *Sender)
{
	if(MainForm->equipMode != modeManual)
	{
		ShowMessage(GetLangStr("MSG_MANUAL_CONTROL"));
	}
	else{
		robostar->Bypass(true);
	}
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::btnBypassOffClick(TObject *Sender)
{
	if(MainForm->equipMode != modeManual)
	{
		ShowMessage(GetLangStr("MSG_MANUAL_CONTROL"));
	}
	else{
		robostar->Bypass(false);
	}
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

        ChangeLanguage();
		ServoAlarmListForm->LanguageChange(rbt->Tag);
	}
}
//---------------------------------------------------------------------------
UnicodeString __fastcall TBaseForm::GetLangStr(AnsiString key)
{
    // 이미 메모리에 로드된 LangDict에서 값만 찾아서 반환 (매우 빠름)
    UnicodeString value = LangDict->Values[key];
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
	MainForm->pup1->Caption = GetLangStr("CAP_UP");
	MainForm->pflow1->Caption = GetLangStr("CAP_FLOW");
	MainForm->popen1->Caption = GetLangStr("CAP_OPEN");
	MainForm->pclose1->Caption = GetLangStr("CAP_CLOSE");
	MainForm->pdn1->Caption = GetLangStr("CAP_DOWN");
	MainForm->pcell1->Caption = GetLangStr("CAP_CELL");
	MainForm->puse1->Caption = GetLangStr("CAP_STOP_USING");
	MainForm->pnlCode->Caption = GetLangStr("CAP_CODE");
	MainForm->pnlTarget->Caption = GetLangStr("CAP_TARGET");
	MainForm->pnlSource->Caption = GetLangStr("CAP_SOURCE");
    MainForm->lblLogTitle->Caption = GetLangStr("CAP_EQUIPMENT_LOG");

	MainForm->pnlTargetTrayTitle->Caption = GetLangStr("CAP_TARGET_TRAY");
	MainForm->pnlTargetRemaining->Caption = GetLangStr("CAP_REMAINING_COUNT");
	MainForm->pnlSourceTrayTitle->Caption = GetLangStr("CAP_SOURCE_TRAY");
	MainForm->pnlSourceRemaining->Caption = GetLangStr("CAP_EJECTING_COUNT");
	MainForm->pnlCh->Caption = GetLangStr("CAP_CHANNEL");
	MainForm->pnlSource2->Caption = GetLangStr("CAP_SOURCE");
	MainForm->pnlNgCode->Caption = GetLangStr("CAP_NG_CODE");
	MainForm->AdvSmoothToggleButton_InitWork->Caption = GetLangStr("CAP_INIT");

    //* Teaching Form
    teachForm->lblMsgRobot->Caption = GetLangStr("CAP_ROBOT_MSG");
    teachForm->lblSourceTray->Caption = GetLangStr("CAP_SOURCE_TRAY");
    teachForm->lblTargetTray->Caption = GetLangStr("CAP_TARGET_TRAY");
    teachForm->lblServoSetting->Caption = GetLangStr("CAP_SERVO_SETTING");
    teachForm->lblServoInfo->Caption = GetLangStr("CAP_SERVO_STATUS");
    teachForm->btnApplyTeaching->Caption = GetLangStr("CAP_APPLY_TEACHING");
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
    teachForm->waitBtn->Caption = GetLangStr("CAP_WAIT_POS");
    teachForm->AdvSmoothButton_Zup->Caption = GetLangStr("CAP_Z_UP");
    teachForm->stopBtn->Caption = GetLangStr("CAP_STOP_MOVING");
    teachForm->pnlSpeed->Caption = GetLangStr("CAP_SPEED");
    teachForm->pnlSettingSpeed->Caption = GetLangStr("CAP_SETTING_SPEED");
    teachForm->lblJogControl->Caption = GetLangStr("CAP_JOG_CONTROL");
    teachForm->lblUpDown->Caption = GetLangStr("CAP_UP_DOWN");
    teachForm->lblOpenClose->Caption = GetLangStr("CAP_OPEN_CLOSE");
    teachForm->AdvSmoothButton_LoadFactorInfo->Caption = GetLangStr("CAP_LOAD_FACTOR");
    teachForm->lblLoadFactorTitle->Caption = GetLangStr("CAP_LOAD");
    teachForm->disableChk1->Caption = GetLangStr("CAP_USING");
    teachForm->pup1->Caption = GetLangStr("CAP_UP");
    teachForm->pdn1->Caption = GetLangStr("CAP_DOWN");
    teachForm->pflow1->Caption = GetLangStr("CAP_FLOW");
    teachForm->popen1->Caption = GetLangStr("CAP_OPEN");
    teachForm->pclose1->Caption = GetLangStr("CAP_CLOSE");
    teachForm->pcell1->Caption = GetLangStr("CAP_CELL");

    //* LoadFactor Form
    loadfactorForm->pnlXAxis->Caption = GetLangStr("CAP_X_AXIS");
    loadfactorForm->pnlYAxis->Caption = GetLangStr("CAP_Y_AXIS");
    loadfactorForm->pnlZAxis->Caption = GetLangStr("CAP_Z_AXIS");
    loadfactorForm->pnlLimitValue->Caption = GetLangStr("CAP_LIMIT_VALUE");
    loadfactorForm->AdvSmoothButton_Save->Caption = GetLangStr("CAP_SAVE");
    loadfactorForm->AdvSmoothButton_Cancel->Caption = GetLangStr("CAP_CANCEL");

    //* SERVO Alarm List
    ServoAlarmListForm->gbSystemAlarm->Caption = GetLangStr("CAP_SYSTEM_ALARM");
    ServoAlarmListForm->gbServoAlarm->Caption = GetLangStr("CAP_SERVO_ALARM");
    ServoAlarmListForm->gbOPAlarm->Caption = GetLangStr("CAP_OP_ALARM");
    ServoAlarmListForm->gbSystemError->Caption = GetLangStr("CAP_SYSTEM_ERROR");

    //* FormError_insert
    ErrorForm_insert->lblTitle->Caption = GetLangStr("CAP_ERROR_CHANNEL_INFO");
    ErrorForm_insert->pnlGripperNo->Caption = GetLangStr("CAP_GRIPPER_NO");
    ErrorForm_insert->pnlSourceChannel->Caption = GetLangStr("CAP_SOURCE_CHANNEL");
    ErrorForm_insert->pnlTargetChannel->Caption = GetLangStr("CAP_TARGET_CHANNEL");
    ErrorForm_insert->pnlNgCode->Caption = GetLangStr("CAP_NG_CODE");
    ErrorForm_insert->btnMoveSource->Caption = GetLangStr("CAP_MOVE");
    ErrorForm_insert->btnMoveTarget->Caption = GetLangStr("CAP_MOVE");
    ErrorForm_insert->CLR1->Caption = GetLangStr("CAP_GRIPPER1");
    ErrorForm_insert->lblUpDown->Caption = GetLangStr("CAP_UP_DOWN");
    ErrorForm_insert->lblOpenClose->Caption = GetLangStr("CAP_OPEN_CLOSE");
    ErrorForm_insert->pup1->Caption = GetLangStr("CAP_UP");
    ErrorForm_insert->pdn1->Caption = GetLangStr("CAP_DOWN");
    ErrorForm_insert->pflow1->Caption = GetLangStr("CAP_FLOW");
    ErrorForm_insert->popen1->Caption = GetLangStr("CAP_OPEN");
    ErrorForm_insert->pclose1->Caption = GetLangStr("CAP_CLOSE");
    ErrorForm_insert->pcell1->Caption = GetLangStr("CAP_CELL");

    //* FormError_eject
    ErrorForm_eject->lblTitle->Caption = GetLangStr("CAP_ERROR_CHANNEL_INFO");
    ErrorForm_eject->pnlGripperNo->Caption = GetLangStr("CAP_GRIPPER_NO");
    ErrorForm_eject->pnlSourceChannel->Caption = GetLangStr("CAP_SOURCE_CHANNEL");
    ErrorForm_eject->pnlTargetChannel->Caption = GetLangStr("CAP_TARGET_CHANNEL");
    ErrorForm_eject->pnlNgCode->Caption = GetLangStr("CAP_NG_CODE");
    ErrorForm_eject->btnMoveSource->Caption = GetLangStr("CAP_MOVE");
    ErrorForm_eject->btnMoveTarget->Caption = GetLangStr("CAP_MOVE");
    ErrorForm_eject->CLR1->Caption = GetLangStr("CAP_GRIPPER1");
    ErrorForm_eject->lblUpDown->Caption = GetLangStr("CAP_UP_DOWN");
    ErrorForm_eject->lblOpenClose->Caption = GetLangStr("CAP_OPEN_CLOSE");
    ErrorForm_eject->pup1->Caption = GetLangStr("CAP_UP");
    ErrorForm_eject->pdn1->Caption = GetLangStr("CAP_DOWN");
    ErrorForm_eject->pflow1->Caption = GetLangStr("CAP_FLOW");
    ErrorForm_eject->popen1->Caption = GetLangStr("CAP_OPEN");
    ErrorForm_eject->pclose1->Caption = GetLangStr("CAP_CLOSE");
    ErrorForm_eject->pcell1->Caption = GetLangStr("CAP_CELL");
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
