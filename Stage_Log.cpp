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

