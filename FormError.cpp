//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TErrorForm *ErrorForm;
//---------------------------------------------------------------------------
__fastcall TErrorForm::TErrorForm(TComponent* Owner)
	: TForm(Owner)
{
	this->Parent = BaseForm;
}
//---------------------------------------------------------------------------
void __fastcall TErrorForm::ShowError(AnsiString MainStr, AnsiString SubStr, AnsiString SubStr2)
{
	ErrStr = MainStr;
	MainErr->Caption = "S_Maint_" + MainStr;
	pTrayId->Caption = SubStr2;
	SubErr->Caption = SubStr;
	FormStyle = fsStayOnTop;

	if(this->Visible == false){
		this->BringToFront();
		this->Visible = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm::manualBtnClick(TObject *Sender)
{
    this->Visible = false;
}
//---------------------------------------------------------------------------

