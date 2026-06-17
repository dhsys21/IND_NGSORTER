//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "AdvSmoothButton"
#pragma resource "*.dfm"
TloadfactorForm *loadfactorForm;
//---------------------------------------------------------------------------
__fastcall TloadfactorForm::TloadfactorForm(TComponent* Owner)
	: TForm(Owner)
{
	Panel_Position[1] = px1;
	Panel_Position[2] = py;
	Panel_Position[3] = pz;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void __fastcall TloadfactorForm::SaveInfo()
{
	TIniFile *ini;

	AnsiString file;
	file = (AnsiString)BIN + "LoadFactor.inf";

	ini = new TIniFile(file);

	ini->WriteInteger("INFO", "LimitValue", Edit_LimitValue->Text.ToInt());
	ini->WriteInteger("INFO", "Count", m_Count);

	delete ini;

	m_SetLimit = Edit_LimitValue->Text.ToInt();
}
//---------------------------------------------------------------------------
void __fastcall TloadfactorForm::LoadInfo()
{
	TIniFile *ini;

	AnsiString file;
	file = (AnsiString)BIN + "LoadFactor.inf";

	ini = new TIniFile(file);

	Edit_LimitValue->Text = ini->ReadString("INFO", "LimitValue", "100");
	m_Count = ini->ReadInteger("INFO", "Count", 0);

	delete ini;

	m_SetLimit = Edit_LimitValue->Text.ToInt();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void __fastcall TloadfactorForm::AdvSmoothButton_SaveClick(TObject *Sender)
{
    SaveInfo();
	this->Close();
}
//---------------------------------------------------------------------------
void __fastcall TloadfactorForm::AdvSmoothButton_CancelClick(TObject *Sender)
{
    LoadInfo();
	this->Close();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
void __fastcall TloadfactorForm::LanguageChange(int index)
{
	TMemo *mm;
	if(index == 0) mm = Memo_Ko;
	else if(index == 1) mm = Memo_En;
	else if(index == 2) mm = Memo_Hu;

//	Panel35->Caption = mm->Lines->Strings[0];
//	Panel44->Caption = mm->Lines->Strings[1];
//	Panel31->Caption = mm->Lines->Strings[2];
//	Panel33->Caption = mm->Lines->Strings[3];
//	Panel39->Caption = mm->Lines->Strings[4];
//	AdvSmoothButton_Save->Caption = mm->Lines->Strings[5];
//	AdvSmoothButton_Cancel->Caption = mm->Lines->Strings[6];
}
//---------------------------------------------------------------------------
