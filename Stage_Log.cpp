#include "FormMain.h"
#include "FormBase.h"

//---------------------------------------------------------------------------
void __fastcall TMainForm::WriteProgLog(AnsiString msg)
{
	AnsiString str;
	int file_handle;

	str = (AnsiString)PROG_LOG + Now().FormatString("yymmdd") + ".csv";

	if(FileExists(str))
		file_handle = FileOpen(str, fmOpenWrite);
	else {
		file_handle = FileCreate(str);
	}

	FileSeek(file_handle, 0, 2);

	str = Now().FormatString("yyyy-mm-dd hh:nn:ss ") + "," + pTrayid_source2->Caption + "," + pTrayid_target2->Caption + "," +  msg +  "\r\n";

	FileWrite(file_handle, str.c_str(), str.Length());
	FileClose(file_handle);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::WriteErrorLog(AnsiString str1, AnsiString str2)
{
	AnsiString str;
	int file_handle;

	str = (AnsiString)ERROR_LOG + Now().FormatString("yymmdd") + ".csv";

	if(FileExists(str))
		file_handle = FileOpen(str, fmOpenWrite);
	else {
		file_handle = FileCreate(str);
	}

	FileSeek(file_handle, 0, 2);

	str = Now().FormatString("yyyy-mm-dd hh:nn:ss ") + "," + str1 + "," + str2+  "\r\n";

	FileWrite(file_handle, str.c_str(), str.Length());
	FileClose(file_handle);
}
//---------------------------------------------------------------------------
UnicodeString __fastcall TMainForm::GetLangStr(AnsiString key)
{
    // 이미 메모리에 로드된 LangDict에서 값만 찾아서 반환 (매우 빠름)
    UnicodeString value = LangDict->Values[key];
    return value.IsEmpty() ? L" - " + key : value;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::LanguageChange(AnsiString newLang)
{
    CurrentLanguage = newLang;
    LangDict = new TStringList();
    LangDict->Clear();
    std::unique_ptr<TResourceStream> res(new TResourceStream((NativeUInt)HInstance, newLang + L"_DATA", RT_RCDATA));
    LangDict->LoadFromStream(res.get(), TEncoding::UTF8);

    //* Main Form
	lblTitle->Caption = GetLangStr("CAP_TITLE");
	autoBtn->Caption = GetLangStr("CAP_AUTO");
	playBtn->Caption = GetLangStr("CAP_START");
	stopBtn->Caption = GetLangStr("CAP_STOP");
	manualBtn->Caption = GetLangStr("CAP_MANUAL");
	pause_stopBtn->Caption = GetLangStr("CAP_PAUSE");
	ppause->Caption = GetLangStr("CAP_PAUSE");
	pause_startBtn->Caption = GetLangStr("CAP_RESTART");
	buzzerBtn->Caption = GetLangStr("CAP_BUZZER_STOP");
	pemergency->Caption = GetLangStr("CAP_EMG_STOP");
	pnlErrCode->Caption = GetLangStr("CAP_ERR_CODE");
	lblManualControl->Caption = GetLangStr("CAP_MANUAL_TITLE");
	trayout_targetBtn->Status->Caption = GetLangStr("CAP_TARGET_TRAY");
	trayout_targetBtn->Caption = GetLangStr("CAP_TRAY_OUT");
	trayout_srcBtn->Status->Caption = GetLangStr("CAP_SOURCE_TRAY");
	trayout_srcBtn->Caption = GetLangStr("CAP_TRAY_OUT");
	lblXPos->Caption = GetLangStr("CAP_X_POS");
	lblYPos->Caption = GetLangStr("CAP_Y_POS");
	lblZPos->Caption = GetLangStr("CAP_Z_POS");
	lblSpeedXY->Caption = GetLangStr("CAP_SPEED");
	lblSourceTrayInfo->Caption = GetLangStr("CAP_SOURCE_TRAY_INFO");
	lblTargetTrayInfo->Caption = GetLangStr("CAP_TARGET_TRAY_INFO");
	psrcArrive->Caption = GetLangStr("CAP_ARRIVE");
	psrcReady->Caption = GetLangStr("CAP_CENTERING");
	ptargetReady->Caption = GetLangStr("CAP_CENTERING");
	pwork1->Caption = GetLangStr("CAP_READY");
	pwork2->Caption = GetLangStr("CAP_READY");
	psrcOut->Caption = GetLangStr("CAP_OUT");
	ptargetOut->Caption = GetLangStr("CAP_OUT");
	lblNgList->Caption = GetLangStr("CAP_NG_LIST");
	pnlNgQuantity->Caption = GetLangStr("CAP_NG_COUNT");
	pnlNgLimit->Caption = GetLangStr("CAP_NG_LIMIT_COUNT");
	btnApplyNgLimitCount->Caption = GetLangStr("CAP_APPLY");
	CLR1->Caption = GetLangStr("CAP_GRIPPER") + " #1";
	CLR2->Caption = GetLangStr("CAP_GRIPPER") + " #2";
	pup1->Caption = GetLangStr("CAP_UP");
	pflow1->Caption = GetLangStr("CAP_FLOW");
	popen1->Caption = GetLangStr("CAP_OPEN");
	pclose1->Caption = GetLangStr("CAP_CLOSE");
	pdn1->Caption = GetLangStr("CAP_DOWN");
	pcell1->Caption = GetLangStr("CAP_CELL");
	puse1->Caption = GetLangStr("CAP_STOP_USING");
	pnlCode->Caption = GetLangStr("CAP_CODE");
	pnlTarget->Caption = GetLangStr("CAP_TARGET");
	pnlSource->Caption = GetLangStr("CAP_SOURCE");
    lblLogTitle->Caption = GetLangStr("CAP_EQUIPMENT_LOG");

	pnlTargetTrayTitle->Caption = GetLangStr("CAP_TARGET_TRAY");
	pnlTargetRemaining->Caption = GetLangStr("CAP_REMAINING_COUNT");
	pnlSourceTrayTitle->Caption = GetLangStr("CAP_SOURCE_TRAY");
	pnlSourceRemaining->Caption = GetLangStr("CAP_EJECTING_COUNT");
	pnlCh->Caption = GetLangStr("CAP_CHANNEL");
	pnlSource2->Caption = GetLangStr("CAP_SOURCE");
	pnlNgCode->Caption = GetLangStr("CAP_NG_CODE");
	AdvSmoothToggleButton_InitWork->Caption = GetLangStr("CAP_INIT");

    //* Teaching Form
    teachForm->lblMsgRobot->Caption = GetLangStr("MSG_ROBOT_INFO");
    teachForm->lblServoSetting->Caption = GetLangStr("CAP_SERVO_SETTING");
    teachForm->lblServoInfo->Caption = GetLangStr("CAP_SERVO_STATUS");
    teachForm->btnApplyTeaching->Caption = GetLangStr("CAP_APPLY_TEACHING");
    teachForm->pnlSelectGripper->Caption = GetLangStr("CAP_SELECT_GRIPPER");
    teachForm->lblJogControl->Caption = GetLangStr("CAP_JOG_CONTROL");
    teachForm->lblXAxis->Caption = GetLangStr("CAP_X_AXIS");
    teachForm->lblYAxis->Caption = GetLangStr("CAP_Y_AXIS");
    teachForm->lblZAxis->Caption = GetLangStr("CAP_Z_AXIS");
    teachForm->pnlSpeedXY->Caption = GetLangStr("CAP_SPEED_XY");
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
//---------------------------------------------------------------------------
