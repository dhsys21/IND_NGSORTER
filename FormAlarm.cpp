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

		const int detailWidth = AdvSmoothPanel1->Width - (errMsg2->Left * 2);
		errMsg2->Width = detailWidth;
		RECT textRect = {0, 0, detailWidth, 0};
		HDC dc = GetDC(Handle);
		HFONT oldFont = (HFONT)SelectObject(dc, errMsg2->Font->Handle);
		int detailHeight = DrawTextW(dc, str2.c_str(), str2.Length(), &textRect,
			DT_CALCRECT | DT_WORDBREAK | DT_EDITCONTROL | DT_NOPREFIX);
		SelectObject(dc, oldFont);
		ReleaseDC(Handle, dc);

		if(detailHeight < 24)
			detailHeight = 24;
		errMsg2->Height = detailHeight + 4;

		const int minPanelHeight = 137;
		int panelHeight = errMsg2->Top + errMsg2->Height + 20;
		if(panelHeight < minPanelHeight)
			panelHeight = minPanelHeight;
		AdvSmoothPanel1->Height = panelHeight;

		int buttonTop = AdvSmoothPanel1->Top + panelHeight + 3;
		AdvSmoothButton5->Top = buttonTop;
		ignoreBtn->Top = buttonTop;
		ClientHeight = buttonTop + ignoreBtn->Height + 4;

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

