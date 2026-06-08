//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ModEcs.h"
#include "FormConfig.h"
#include "FormBase.h"
#include "define.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEcsMod *EcsMod;
//---------------------------------------------------------------------------
__fastcall TEcsMod::TEcsMod(TComponent* Owner)
	: TDataModule(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TEcsMod::ReContactTimerTimer(TObject *Sender)
{
    ReContactTimer->Enabled = false;
	if(ecsSock->Active == false)ecsSock->Active = true;
}
//---------------------------------------------------------------------------
void __fastcall TEcsMod::SendMsg(AnsiString stage, AnsiString status, AnsiString in, AnsiString out)
{
	AnsiString msg;
	if(ecsSock->Active == true){
//		stage = IntToHex(stage.ToInt(), 3);
		WriteEcsLog(ETC, "["+stage+"]");
		msg = "CONVC1063000000000" + status;
//		       CONVC106200000000090
		WriteEcsLog(ETC, msg);
		ecs_sock->SendText(msg);
	}
}
//---------------------------------------------------------------------------
void __fastcall TEcsMod::WriteEcsLog(int type, AnsiString msg)
{
	AnsiString str;
	int file_handle;

	str = (AnsiString)SOCK_LOG + "EcsMsg-" + Now().FormatString("yymmdd") + ".log";

	if(FileExists(str))
		file_handle = FileOpen(str, fmOpenWrite);
	else {
		file_handle = FileCreate(str);
	}

	FileSeek(file_handle, 0, 2);

	AnsiString flow;
	switch(type){
		case SEND:
			flow = "¡ÞPC ¡æ ECS";
			break;
		case RECEIVE:
			flow = "¡ßECS ¡æ PC";
			break;
		default:
			flow = "STAGE";
			break;
	}
	str = Now().FormatString("yyyy-mm-dd hh:nn:ss ") + flow + "\t" + msg +  "\r\n";

	FileWrite(file_handle, str.c_str(), str.Length());
	FileClose(file_handle);

}
//---------------------------------------------------------------------------

void __fastcall TEcsMod::ecsSockConnect(TObject *Sender, TCustomWinSocket *Socket)

{
    ecs_sock = Socket;
//	if(BaseForm->RadioButton_en->Checked)
//		ConfigForm->pecs->Caption = "ECS is connected";
//	else
		ConfigForm->pecs->Caption = "ECS ¿¬°áµÊ.";
}
//---------------------------------------------------------------------------

void __fastcall TEcsMod::ecsSockDisconnect(TObject *Sender, TCustomWinSocket *Socket)

{
    ecs_sock = NULL;
//	if(BaseForm->RadioButton_en->Checked)
//		ConfigForm->pecs->Caption = "ECS is disconnected.";
//	else
		ConfigForm->pecs->Caption = "ECS ¿¬°á ²÷±è.";
	if(auto_contact){
		ReContactTimer->Enabled = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TEcsMod::ecsSockError(TObject *Sender, TCustomWinSocket *Socket, TErrorEvent ErrorEvent,
          int &ErrorCode)
{
    ErrorCode = 0;
	Socket->Close();
}
//---------------------------------------------------------------------------

void __fastcall TEcsMod::ecsSockRead(TObject *Sender, TCustomWinSocket *Socket)
{
    AnsiString msg;

	msg = Socket->ReceiveText();
	WriteEcsLog(RECEIVE, msg);
}
//---------------------------------------------------------------------------

