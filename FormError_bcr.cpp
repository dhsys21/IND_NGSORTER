//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TErrorForm_bcr *ErrorForm_bcr;
//---------------------------------------------------------------------------
__fastcall TErrorForm_bcr::TErrorForm_bcr(TComponent* Owner)
	: TForm(Owner)
{
	this->Parent = BaseForm;
}
//---------------------------------------------------------------------------
void __fastcall TErrorForm_bcr::ShowError(AnsiString str,  bool bsuccess)
{
	if(bsuccess){
		ignoreBtn->Color = pon->Color;
		errMsg1->Caption = "[ " + str + " ] " + BaseForm->GetLangStr("MSG_COMPLETE_SCAN");
		strBcr = str;
		ignoreBtn->Caption = "Normal progress";
	}
	else{
		MainForm->BuzzerOn(true);
		MainForm->LampModeChange(LampAlarm);
		errMsg1->Caption = str;
		ignoreBtn->Color = clRed;
		ignoreBtn->Caption = "Forced Tray Out";
	}

	MainForm->WriteErrorLog(errMsg1->Caption, "");

	if(this->Visible == false){
		this->BringToFront();
		this->Show();
	}
}
//---------------------------------------------------------------------------
void __fastcall TErrorForm_bcr::ignoreBtnClick(TObject *Sender)
{
	MainForm->memoMainLineAdd("Normal progress");
	if(ignoreBtn->Color == clRed){
		MainForm->CmdTrayOut(this->Tag);
	}else{
        MainForm->setBarcode(Tag, strBcr);
    }
	this->Visible = false;
}
//---------------------------------------------------------------------------


void __fastcall TErrorForm_bcr::btnScanClick(TObject *Sender)
{
	switch(Tag){
		case 0:
			errMsg1->Caption = BaseForm->GetLangStr("MSG_SOURCETRAY_SCAN") + "..";
			break;
		case 1:
			errMsg1->Caption = BaseForm->GetLangStr("MSG_TARGETTRAY_SCAN") + "..";
			break;
	}
	MainForm->comBcr[Tag]->GetBarcode();
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm_bcr::AdvSmoothButton5Click(TObject *Sender)
{
	MainForm->BuzzerOn(false);
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm_bcr::FormHide(TObject *Sender)
{
	MainForm->BuzzerOn(false);
    MainForm->LampModeChange(MainForm->beforeLampMode);
}
//---------------------------------------------------------------------------

