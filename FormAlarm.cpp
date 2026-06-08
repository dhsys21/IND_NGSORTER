//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TAlarmForm *AlarmForm;
//---------------------------------------------------------------------------
__fastcall TAlarmForm::TAlarmForm(TComponent* Owner)
	: TForm(Owner)
{
	this->Parent = BaseForm;
}
//---------------------------------------------------------------------------
void __fastcall TAlarmForm::ShowError(AnsiString str1, UnicodeString str2)
{
	robostar->req_Pause(true);
	gripper->req_Pause(true);
	if(this->Visible == false){
		MainForm->WriteErrorLog(str1, str2);
		MainForm->BuzzerOn(true);
		MainForm->LampModeChange(LampAlarm);
		errMsg1->Caption = "S_Maint_" + str1;
		errMsg2->Caption = str2;
		FormStyle = fsStayOnTop;
		this->BringToFront();
		this->Show();
	}
}
//---------------------------------------------------------------------------
void __fastcall TAlarmForm::ignoreBtnClick(TObject *Sender)
{
	this->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TAlarmForm::AdvSmoothButton5Click(TObject *Sender)
{
	MainForm->BuzzerOn(false);
}
//---------------------------------------------------------------------------

void __fastcall TAlarmForm::FormHide(TObject *Sender)
{
	MainForm->BuzzerOn(false);
	MainForm->LampModeChange(MainForm->beforeLampMode);
}
//---------------------------------------------------------------------------

