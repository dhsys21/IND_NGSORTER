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
	MainErr->Caption = MainStr;
	pTrayId->Caption = SubStr2;
	SubErr->Caption = SubStr;
	FormStyle = fsStayOnTop;

	if(this->Visible == false){
		this->BringToFront();
		this->Visible = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall ShowCommonError(AnsiString MainStr, AnsiString SubStr1, AnsiString SubStr2)
{
	// Most calls come from UI timers. Keep a defensive lazy-create path so a
	// startup ordering change cannot turn ErrorForm->ShowError() into a NULL call.
	if(ErrorForm == NULL && Application != NULL && BaseForm != NULL)
		Application->CreateForm(__classid(TErrorForm), &ErrorForm);

	if(ErrorForm != NULL){
		ErrorForm->ShowError(MainStr, SubStr1, SubStr2);
		return;
	}

	// Last-resort message when form creation is unavailable.
	ShowMessage(MainStr + "\n" + SubStr1 + "\n" + SubStr2);
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm::manualBtnClick(TObject *Sender)
{
    this->Visible = false;
}
//---------------------------------------------------------------------------

