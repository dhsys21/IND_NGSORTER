#include "FormMain.h"
#include "FormBase.h"

//---------------------------------------------------------------------------
void __fastcall TMainForm::WriteProgLog(AnsiString msg)
{
	AnsiString str;
	int file_handle;

	str = (AnsiString)PROG_LOG + "STATUS_" + Now().FormatString("yymmdd") + ".log";

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
//* max speed mode - need remove
void __fastcall TMainForm::WriteProgLogBatch(const std::deque<AnsiString> &messages)
{
	if(messages.empty()) return;

	AnsiString fileName = (AnsiString)PROG_LOG + "STATUS_" +
		Now().FormatString("yymmdd") + ".log";
	int fileHandle = FileExists(fileName) ?
		FileOpen(fileName, fmOpenWrite) : FileCreate(fileName);
	if(fileHandle < 0) return;

	FileSeek(fileHandle, 0, 2);
	AnsiString rows;
	for(std::deque<AnsiString>::const_iterator it = messages.begin();
		it != messages.end(); ++it){
		rows += Now().FormatString("yyyy-mm-dd hh:nn:ss ") + "," +
			pTrayid_source2->Caption + "," + pTrayid_target2->Caption + "," +
			*it + "\r\n";
	}
	if(!rows.IsEmpty()) FileWrite(fileHandle, rows.c_str(), rows.Length());
	FileClose(fileHandle);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::WriteOpcUaLog(AnsiString Type, AnsiString Msg, bool bDisplay)
{
	AnsiString str;
	int file_handle;

	str = (AnsiString)SOCK_LOG + "OPCUA_" + Now().FormatString("yymmdd-hh") + ".log";

	if(FileExists(str))
		file_handle = FileOpen(str, fmOpenWrite);
	else
		file_handle = FileCreate(str);

	FileSeek(file_handle, 0, 2);

	str = Now().FormatString("yyyy-mm-dd hh:nn:ss.zzz ") + "[" + Type + "]\t" + Msg + "\r\n";

	FileWrite(file_handle, str.c_str(), str.Length());
	FileClose(file_handle);

	if(bDisplay)
		memoMainLineAdd("[FMS] " + Type + " : " + Msg);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::WriteErrorLog(AnsiString str1, AnsiString str2)
{
	AnsiString str;
	int file_handle;

	str = (AnsiString)ERROR_LOG + "ERROR_" + Now().FormatString("yymmdd") + ".log";

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

