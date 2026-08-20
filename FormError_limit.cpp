//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TErrorForm_limit *ErrorForm_limit;
//---------------------------------------------------------------------------
__fastcall TErrorForm_limit::TErrorForm_limit(TComponent* Owner)
	: TForm(Owner)
{
	this->Parent = BaseForm;
}
//---------------------------------------------------------------------------
void __fastcall TErrorForm_limit::ShowError()
{

	MainForm->NotifyAlarm(true, 24);
	errMsg1->Caption = BaseForm->GetLangStr("MSG_NG_LIMIT_EXCEEDED");
	errMsg2->Caption = BaseForm->GetLangStr("MSG_CONTINUE_SORTING");

	if(this->Visible == false){
		MainForm->WriteErrorLog(errMsg1->Caption, errMsg2->Caption);
		MainForm->BuzzerOn(true);
		MainForm->LampModeChange(LampAlarm);
		this->BringToFront();
		this->Show();
	}
}
//---------------------------------------------------------------------------
void __fastcall TErrorForm_limit::ignoreBtnClick(TObject *Sender)
{
	MainForm->memoMainLineAdd("정상 진행(선별)");
	if(PlcBin != NULL) PlcBin->CmdSourceCenteringRequest(true);
	this->Visible = false;
}
//---------------------------------------------------------------------------


void __fastcall TErrorForm_limit::AdvSmoothButton1Click(TObject *Sender)
{
	MainForm->memoMainLineAdd("트레이 배출");
	MainForm->CmdTrayOut(0);
	this->Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm_limit::AdvSmoothButton5Click(TObject *Sender)
{
	MainForm->BuzzerOn(false);
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm_limit::FormHide(TObject *Sender)
{
	MainForm->NotifyAlarm(false, 24);
	MainForm->BuzzerOn(false);
	MainForm->LampModeChange(MainForm->beforeLampMode);
}
//---------------------------------------------------------------------------

