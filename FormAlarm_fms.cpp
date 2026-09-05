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
	// FMS ALARM PAUSE/CLOSE: independent modeless window; never block MainForm.
	Parent = NULL;
	operatorPaused = false;
	dismissed = false;
	dismissedTick = 0;
}
//---------------------------------------------------------------------------
void __fastcall TAlarmForm_fms::ShowFmsError(const AnsiString &Title,
	const AnsiString &Detail, const AnsiString &RequestName,
	int ResponseValue)
{
	// Keep the popup safe even when a future caller bypasses ShowFmsAlarm().
	if(gripper != NULL) gripper->req_Pause(true);
	if(robostar != NULL) robostar->req_Pause(true);
	operatorPaused = false;
	dismissed = false;

	lblTitle->Caption = Title;
	memoDetail->Lines->Text = Detail;
	lblRequest->Caption = RequestName;
	lblResponse->Caption = IntToStr(ResponseValue);
	lblStatus->Caption = "Auto-paused. Pause: hold alarm / Close: hide only. Correct FMS, then press Retry or Main Restart.";
	btnPause->Enabled = true;

	if(MainForm != NULL){
		MainForm->WriteErrorLog(Title, Detail);
		MainForm->BuzzerOn(true);
		MainForm->LampModeChange(LampAlarm);
	}
	FormStyle = fsNormal;
	Show();
	BringToFront();
}
//---------------------------------------------------------------------------
void __fastcall TAlarmForm_fms::SetRetryWaiting(const AnsiString &Status)
{
	lblStatus->Caption = Status;
}
//---------------------------------------------------------------------------
void __fastcall TAlarmForm_fms::btnPauseClick(TObject *Sender)
{
	if(MainForm != NULL)
		MainForm->PauseFmsAlarm();
}
//---------------------------------------------------------------------------
void __fastcall TAlarmForm_fms::SetOperatorPaused()
{
	operatorPaused = true;
	btnPause->Enabled = false;
	lblStatus->Caption = "PAUSED - Close is available. Correct FMS, then press Retry or Main Restart to retry this step.";
}
//---------------------------------------------------------------------------
void __fastcall TAlarmForm_fms::btnRetryClick(TObject *Sender)
{
	// FMS ALARM RETRY: use the exact Main Restart path, including AUTO checks,
	// handshake recovery and releasing both paused sequences. Do not issue a
	// request directly here: retrying FMS alone would leave motion paused.
	if(MainForm != NULL)
		MainForm->pause_startBtnClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TAlarmForm_fms::btnCloseClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TAlarmForm_fms::FormClose(TObject *Sender, TCloseAction &Action)
{
	// Close never acknowledges/cancels the failed transaction or resumes motion.
	dismissed = true;
	dismissedTick = GetTickCount();
	Action = caHide;
	if(MainForm != NULL)
		MainForm->memoMainLineAdd(operatorPaused ?
			"[FMS ALARM] CLOSED / PAUSED / WAIT Main Restart; transaction retained." :
			"[FMS ALARM] CLOSED without Pause / unresolved alarm will reappear in 10 seconds; transaction retained.");
}
//---------------------------------------------------------------------------
void __fastcall TAlarmForm_fms::RefreshAlarmVisibility()
{
	// Called only while the failed transaction is latched and not retrying.
	// Do not repeatedly log, buzz, or steal focus on every timer tick.
	if(operatorPaused || !dismissed || Visible ||
		(DWORD)(GetTickCount() - dismissedTick) < 10000) return;
	dismissed = false;
	Show();
	BringToFront();
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
		// Hiding an unresolved alarm does not mean the equipment recovered.
	}
}
//---------------------------------------------------------------------------
