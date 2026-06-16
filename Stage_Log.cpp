#include "FormMain.h"
#include "FormBase.h"

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
//	pup2->Caption = mm->Lines->Strings[31];
	pflow1->Caption = GetLangStr("CAP_FLOW");
//	pflow2->Caption = mm->Lines->Strings[32];
	popen1->Caption = GetLangStr("CAP_OPEN");
//	popen2->Caption = mm->Lines->Strings[33];
	pclose1->Caption = GetLangStr("CAP_CLOSE");
//	pclose2->Caption = mm->Lines->Strings[34];
	pdn1->Caption = GetLangStr("CAP_DOWN");
//	pdn2->Caption = mm->Lines->Strings[35];
	pcell1->Caption = GetLangStr("CAP_CELL");
//	pcell2->Caption = mm->Lines->Strings[36];
	puse1->Caption = GetLangStr("CAP_STOP_USING");
//	puse2->Caption = mm->Lines->Strings[37];
	pnlCode->Caption = GetLangStr("CAP_CODE");
	pnlTarget->Caption = GetLangStr("CAP_TARGET");
	pnlSource->Caption = GetLangStr("CAP_SOURCE");
    lblLogTitle->Caption = GetLangStr("CAP_EQUIPMENT_LOG");

	pnlTargetTrayTitle->Caption = GetLangStr("CAP_TARGET_TRAY");
//	zone4->Caption = mm->Lines->Strings[45];
	pnlTargetRemaining->Caption = GetLangStr("CAP_REMAINING_COUNT");
	pnlSourceTrayTitle->Caption = GetLangStr("CAP_SOURCE_TRAY");
	pnlSourceRemaining->Caption = GetLangStr("CAP_EJECTING_COUNT");
	pnlCh->Caption = GetLangStr("CAP_CHANNEL");
	pnlSource2->Caption = GetLangStr("CAP_SOURCE");
	pnlNgCode->Caption = GetLangStr("CAP_NG_CODE");
	AdvSmoothToggleButton_InitWork->Caption = GetLangStr("CAP_INIT");
}
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
