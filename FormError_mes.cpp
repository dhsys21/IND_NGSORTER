//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TErrorForm_mes *ErrorForm_mes;
//---------------------------------------------------------------------------
__fastcall TErrorForm_mes::TErrorForm_mes(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TErrorForm_mes::ShowError(AnsiString trayid, AnsiString str1, AnsiString str2)
{

	MainForm->BuzzerOn(true);
	MainForm->LampModeChange(LampAlarm);
	errMsg1->Caption = "[ " + trayid + " ] : " + str1;
	errMsg2->Caption = "S_Maint_" + str2;

	if(trayid == MainForm->pTrayid_target->Caption)ignoreBtn->Tag = 1;
	else ignoreBtn->Tag = 0;

	gripper->req_Pause(true);
	robostar->req_Pause(true);

	MainForm->WriteErrorLog(errMsg1->Caption, errMsg2->Caption);

	if(this->Visible == false){
		this->BringToFront();
		this->Show();
	}
	
	this->BringToFront();
	this->Show();
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm_mes::AdvSmoothButton5Click(TObject *Sender)
{
	MainForm->BuzzerOn(false);
}
//---------------------------------------------------------------------------
void __fastcall TErrorForm_mes::ignoreBtnClick(TObject *Sender)
{
	MainForm->memoMainLineAdd("Forced Tray Out");
	MainForm->CmdTrayOut(ignoreBtn->Tag);
	this->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TErrorForm_mes::retryBtnClick(TObject *Sender)
{
	MainForm->memoMainLineAdd("Retry");
	mes->SendMsg(MainForm->tx);
	this->Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm_mes::FormHide(TObject *Sender)
{
    MainForm->BuzzerOn(false);
	MainForm->LampModeChange(MainForm->beforeLampMode);
}
//---------------------------------------------------------------------------

