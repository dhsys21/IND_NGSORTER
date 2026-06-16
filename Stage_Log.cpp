#include "FormMain.h"
#include "FormBase.h"

//---------------------------------------------------------------------------
UnicodeString __fastcall TMainForm::GetLangStr(AnsiString key)
{
    // 이미 메모리에 로드된 LangDict에서 값만 찾아서 반환 (매우 빠름)
    UnicodeString value = LangDict->Values[key];
    return value.IsEmpty() ? L"Key Not Found: " + key : value;
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
//	playBtn->Caption = mm->Lines->Strings[2];
//	stopBtn->Caption = mm->Lines->Strings[3];
//	manualBtn->Caption = mm->Lines->Strings[4];
//	pause_stopBtn->Caption = mm->Lines->Strings[5];
//	ppause->Caption = mm->Lines->Strings[5];
//	pause_startBtn->Caption = mm->Lines->Strings[6];
//	buzzerBtn->Caption = mm->Lines->Strings[7];
//	pemergency->Caption = mm->Lines->Strings[8];
//	Panel11->Caption = mm->Lines->Strings[9];
//	Label11->Caption = mm->Lines->Strings[10];
//	trayout_targetBtn->Status->Caption = mm->Lines->Strings[11];
//	trayout_targetBtn->Caption = mm->Lines->Strings[12];
//	trayout_srcBtn->Status->Caption = mm->Lines->Strings[13];
//	trayout_srcBtn->Caption = mm->Lines->Strings[14];
//	Label8->Caption = mm->Lines->Strings[15];
//	Label4->Caption = mm->Lines->Strings[16];
//	Label2->Caption = mm->Lines->Strings[17];
//	Label10->Caption = mm->Lines->Strings[18];
//	Label3->Caption = mm->Lines->Strings[19];
//	Label9->Caption = mm->Lines->Strings[20];
//	Label1->Caption = mm->Lines->Strings[21];
//	psrcArrive->Caption = mm->Lines->Strings[22];
//	psrcReady->Caption = mm->Lines->Strings[23];
//	ptargetReady->Caption = mm->Lines->Strings[23];
//	pwork1->Caption = mm->Lines->Strings[24];
//	pwork2->Caption = mm->Lines->Strings[24];
//	psrcOut->Caption = mm->Lines->Strings[25];
//	ptargetOut->Caption = mm->Lines->Strings[25];
//	Label5->Caption = mm->Lines->Strings[26];
//	Panel13->Caption = mm->Lines->Strings[27];
//	Panel8->Caption = mm->Lines->Strings[28];
//	btnApplyNgLimitCount->Caption = mm->Lines->Strings[29];
//	CLR1->Caption = mm->Lines->Strings[30] + " #1";
//	CLR2->Caption = mm->Lines->Strings[30] + " #2";
//	pup1->Caption = mm->Lines->Strings[31];
//	pup2->Caption = mm->Lines->Strings[31];
//	pflow1->Caption = mm->Lines->Strings[32];
//	pflow2->Caption = mm->Lines->Strings[32];
//	popen1->Caption = mm->Lines->Strings[33];
//	popen2->Caption = mm->Lines->Strings[33];
//	pclose1->Caption = mm->Lines->Strings[34];
//	pclose2->Caption = mm->Lines->Strings[34];
//	pdn1->Caption = mm->Lines->Strings[35];
//	pdn2->Caption = mm->Lines->Strings[35];
//	pcell1->Caption = mm->Lines->Strings[36];
//	pcell2->Caption = mm->Lines->Strings[36];
//	puse1->Caption = mm->Lines->Strings[37];
//	puse2->Caption = mm->Lines->Strings[37];
//	Panel25->Caption = mm->Lines->Strings[38];
//	Panel42->Caption = mm->Lines->Strings[38];
//	Panel29->Caption = mm->Lines->Strings[39];
//	Panel45->Caption = mm->Lines->Strings[39];
//	Panel26->Caption = mm->Lines->Strings[40];
//	Panel43->Caption = mm->Lines->Strings[40];
//	Panel5->Caption = mm->Lines->Strings[41];
//	Panel7->Caption = mm->Lines->Strings[42];
//	Panel17->Caption = mm->Lines->Strings[43];
//	Panel20->Caption = mm->Lines->Strings[44];
//	zone4->Caption = mm->Lines->Strings[45];
//	Panel18->Caption = mm->Lines->Strings[46];
//	Panel10->Caption = mm->Lines->Strings[47];
//	Panel23->Caption = mm->Lines->Strings[48];
//	Panel2->Caption = mm->Lines->Strings[49];
//	Panel70->Caption = mm->Lines->Strings[50];
//	Panel3->Caption = mm->Lines->Strings[51];
////	Panel67->Caption = mm->Lines->Strings[52];
//	AdvSmoothToggleButton_InitWork->Caption = mm->Lines->Strings[53];
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
