//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TtrayinfoForm *trayinfoForm;
//---------------------------------------------------------------------------
__fastcall TtrayinfoForm::TtrayinfoForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TtrayinfoForm::ShowError(AnsiString str1, AnsiString str2, AnsiString trayid, int index)
{
	gripper->req_Pause(true);
	robostar->req_Pause(true);

	if(this->Visible == false){
		MainForm->WriteErrorLog(str1, str2);
		MainForm->BuzzerOn(true);
		MainForm->LampModeChange(LampAlarm);
		errMsg1->Caption = "S_Maint_" + str1;
		errMsg2->Caption = str2;
		FormStyle = fsStayOnTop;
		m_LOT_ID = trayid;
		m_Index = index;
		this->BringToFront();
		this->Show();
	}
}
//---------------------------------------------------------------------------
void __fastcall TtrayinfoForm::AdvSmoothButton5Click(TObject *Sender)
{
    MainForm->BuzzerOn(false);
}
//---------------------------------------------------------------------------
void __fastcall TtrayinfoForm::ignoreBtnClick(TObject *Sender)
{
    this->Close();
}
//---------------------------------------------------------------------------
void __fastcall TtrayinfoForm::FormHide(TObject *Sender)
{
	MainForm->BuzzerOn(false);
	MainForm->LampModeChange(MainForm->beforeLampMode);
}
//---------------------------------------------------------------------------

void __fastcall TtrayinfoForm::AdvSmoothButton1Click(TObject *Sender)
{
	MainForm->setTrayInfo(m_Index);
	MainForm->NotifyTrayInfo(m_LOT_ID, m_Index);	// 트레이 mes 보고
	gripper->req_Pause(false);
	robostar->req_Pause(false);

	this->Close();
}
//---------------------------------------------------------------------------


