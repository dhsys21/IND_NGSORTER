//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TAlarmForm_fms *AlarmForm_fms;
//---------------------------------------------------------------------------
__fastcall TAlarmForm_fms::TAlarmForm_fms(TComponent* Owner)
	: TForm(Owner)
{
	Parent = BaseForm;
}
//---------------------------------------------------------------------------
void __fastcall TAlarmForm_fms::ShowFmsError(const AnsiString &Title,
	const AnsiString &Detail, const AnsiString &RequestName,
	int ResponseValue)
{
	// Keep the popup safe even when a future caller bypasses ShowFmsAlarm().
	if(MainForm != NULL)
		MainForm->pause_stopBtnClick(this);

	lblTitle->Caption = Title;
	memoDetail->Lines->Text = Detail;
	lblRequest->Caption = RequestName;
	lblResponse->Caption = IntToStr(ResponseValue);
	lblStatus->Caption = "Correct the FMS condition, then press Retry.";
	btnRetry->Enabled = true;

	if(MainForm != NULL){
		MainForm->WriteErrorLog(Title, Detail);
		MainForm->BuzzerOn(true);
		MainForm->LampModeChange(LampAlarm);
	}
	FormStyle = fsStayOnTop;
	BringToFront();
	Show();
}
//---------------------------------------------------------------------------
void __fastcall TAlarmForm_fms::SetRetryWaiting(const AnsiString &Status)
{
	lblStatus->Caption = Status;
	btnRetry->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TAlarmForm_fms::btnRetryClick(TObject *Sender)
{
	if(MainForm != NULL)
		MainForm->ConfirmFmsAlarmRetry();
}
//---------------------------------------------------------------------------
void __fastcall TAlarmForm_fms::btnBuzzerStopClick(TObject *Sender)
{
	if(MainForm != NULL)
		MainForm->BuzzerOn(false);
}
//---------------------------------------------------------------------------
void __fastcall TAlarmForm_fms::FormHide(TObject *Sender)
{
	if(MainForm != NULL){
		MainForm->BuzzerOn(false);
		MainForm->LampModeChange(MainForm->beforeLampMode);
	}
}
//---------------------------------------------------------------------------
