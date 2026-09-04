//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "AdvSmoothButton"
#pragma link "AdvSmoothPanel"
#pragma resource "*.dfm"
Tloadfactor_AlarmForm *loadfactor_AlarmForm;
//---------------------------------------------------------------------------
__fastcall Tloadfactor_AlarmForm::Tloadfactor_AlarmForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall Tloadfactor_AlarmForm::ShowError(AnsiString str1, AnsiString str2)
{
	// Every alarm popup freezes both automatic state machines. Closing the form
	// does not release Pause; the operator resumes after correcting the cause.
	MainForm->pause_stopBtnClick(this);

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
void __fastcall Tloadfactor_AlarmForm::AdvSmoothButton5Click(TObject *Sender)
{
	MainForm->BuzzerOn(false);
}
//---------------------------------------------------------------------------
void __fastcall Tloadfactor_AlarmForm::ignoreBtnClick(TObject *Sender)
{
	this->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall Tloadfactor_AlarmForm::FormHide(TObject *Sender)
{
	MainForm->BuzzerOn(false);
	MainForm->LampModeChange(MainForm->beforeLampMode);
	loadfactorForm->m_Count = 0;
}
//---------------------------------------------------------------------------

