//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormDryRun.h"
#include "FormBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TDryRunForm *DryRunForm;
//---------------------------------------------------------------------------

// ============================================================================
//* DRY RUN : The dedicated timer owns Sorting/Eject/Insert for empty-cell test.
//* DRY RUN : Cell detect, tray data, PLC commands and FMS reports are not used.
// ============================================================================
__fastcall TDryRunForm::TDryRunForm(TComponent* Owner)
	: TForm(Owner)
{
	drySequence = drsIdle;
	dryStep = 0;
	startChannel = 1;
	endChannel = 96;
	currentChannel = 0;
	completedCycleCount = 0;
	stepStartTick = 0;
	stepTimeoutMs = 0;
	running = false;
	waitPositionRequested = false;
	//* DRY RUN : Explicit event binding also works when the form was added to an
	//* already-open IDE project and its Designer cache has not refreshed yet.
	if(btnWaitPosition != NULL)
		btnWaitPosition->OnClick = btnWaitPositionClick;
}
//---------------------------------------------------------------------------
bool __fastcall TDryRunForm::IsRunning() const
{
	return running;
}
//---------------------------------------------------------------------------
void __fastcall TDryRunForm::FormShow(TObject *Sender)
{
	//* DRY RUN : Never restore an active test after reopening the form.
	if(!running){
		drySequence = drsIdle;
		dryStep = 0;
		currentChannel = 0;
		completedCycleCount = 0;
		waitPositionRequested = false;
		pnlRunState->Caption = "IDLE";
		pnlRunState->Color = clSilver;
		lblCurrentStep->Caption = "Current step : Waiting for START";
		btnStart->Enabled = true;
		btnStop->Enabled = false;
		btnWaitPosition->Enabled = true;
		//* DRY RUN : Start with the commissioned speed; the operator may override
		//* it in this form without changing the saved teaching value.
		if(teachForm != NULL)
			editSpeed->Text = teachForm->speedEdit->Text;
	}
	dryRunTimer->Enabled = true;
	UpdateDryRunStatus();
}
//---------------------------------------------------------------------------
void __fastcall TDryRunForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	//* DRY RUN : The modal form cannot close while inspection motion is active.
	if(running){
		CanClose = false;
		ShowMessage(L"Dry Run is active. Press STOP and wait for HOME completion.");
		return;
	}
	CanClose = true;
	dryRunTimer->Enabled = false;
	if(MainForm != NULL && MainForm->btnDryRun != NULL)
		MainForm->btnDryRun->Color = clWhite;
}
//---------------------------------------------------------------------------
void __fastcall TDryRunForm::btnCloseClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TDryRunForm::SetDryStep(TDryRunSequence sequence, int step,
	const AnsiString &status, DWORD timeoutMs)
{
	drySequence = sequence;
	dryStep = step;
	stepStartTick = GetTickCount();
	stepTimeoutMs = timeoutMs;
	lblCurrentStep->Caption = "Current step : " + status;
}
//---------------------------------------------------------------------------
void __fastcall TDryRunForm::WriteDryRunLog(const AnsiString &message)
{
	AnsiString line = FormatDateTime("hh:nn:ss.zzz", Now()) + " [DRY RUN] " + message;
	//* DRY RUN : Newest inspection event is always visible at the top.
	memoDryRun->Lines->Insert(0, line);
	if(memoDryRun->Lines->Count > 300)
		memoDryRun->Lines->Delete(memoDryRun->Lines->Count - 1);
	memoDryRun->SelStart = 0;
	memoDryRun->SelLength = 0;
	memoDryRun->Perform(EM_SCROLLCARET, 0, 0);
	if(MainForm != NULL)
		MainForm->AddStatusLog("DRY RUN", message);
}
//---------------------------------------------------------------------------
void __fastcall TDryRunForm::UpdateDryRunStatus()
{
	if(robostar == NULL || MainForm == NULL){
		lblInterlock->Caption = "Module status : NOT READY";
		return;
	}

	bool sourceCentering = PlcBin != NULL && PlcBin->ClientSocket_PLC != NULL &&
		PlcBin->ClientSocket_PLC->Active && PlcBin->IsSourceCentering();
	AnsiString channelText = "-";
	if(currentChannel > 0) channelText = IntToStr(currentChannel);
	lblCurrentChannel->Caption = "Cycle=" + IntToStr(completedCycleCount + 1) +
		" / Current channel : " +
		channelText +
		" / X=" + IntToStr((__int64)robostar->mr2.pos[Axis_x]) +
		" Y=" + IntToStr((__int64)robostar->mr2.pos[Axis_y]) +
		" Z=" + IntToStr((__int64)robostar->mr2.pos[Axis_z]);
	lblInterlock->Caption = "D10104 Source Centering=" +
		AnsiString(sourceCentering ? "ON" : "OFF") +
		" / Mode=" + AnsiString(MainForm->equipMode == modeManual ? "MANUAL" : "NOT MANUAL") +
		" / Production=" + AnsiString(MainForm->IsProductionSequenceBusy() ? "BUSY" : "IDLE") +
		" / Safety=" + AnsiString(robostar->IsSafetyReady() ? "READY" : "NOT READY") +
		" / Servo=" + AnsiString(MainForm->m_ServoON ? "ON" : "OFF") +
		" / RobotSeq=" + IntToStr((int)robostar->seq);
	lblInterlock->Font->Color = sourceCentering && robostar->IsSafetyReady() &&
		MainForm->equipMode == modeManual && !MainForm->IsProductionSequenceBusy() ? clGreen : clRed;
}
//---------------------------------------------------------------------------
bool __fastcall TDryRunForm::ValidateDryRunStart(AnsiString &reason)
{
	reason = "";
	if(MainForm == NULL || robostar == NULL || gripper == NULL){
		reason = "Required motion module is not initialized.";
		return false;
	}
	//* DRY RUN : AUTO STOP is not a dry-run mode. Only explicit MANUAL prevents
	//* the production sequence from starting again while Tray In remains ON.
	if(MainForm->equipMode != modeManual){
		reason = "Equipment mode must be MANUAL. AUTO and AUTO STOP are not allowed.";
		return false;
	}
	if(MainForm->IsProductionSequenceBusy()){
		reason = "A production PLC/FMS/local sequence started before MANUAL mode is still active. Complete/cancel it or use Initialize Work first.";
		return false;
	}
	if((int)gripper->seq != 0 || gripper->pauseStatus){
		reason = "Gripper sequence is not IDLE.";
		return false;
	}
	if(robostar->seq != seqIdle || robostar->pauseStatus){
		reason = "Robot sequence is not IDLE.";
		return false;
	}
	if(!robostar->RestoreServoState() || !MainForm->m_ServoOpen ||
		!MainForm->m_ServoON || !MainForm->m_ServoHome){
		reason = "Servo OPEN/ON/HOME or actual X/Y/Z=0 is not complete.";
		return false;
	}
	if(!robostar->IsSafetyReady() || robostar->IsEmergencyStopActive() ||
		!robostar->IsKeyLockActive()){
		reason = "Safety Reset, Emergency Stop, Door or Keylock condition is not ready.";
		return false;
	}
	if(!robostar->IsCcLinkReady()){
		reason = "CC-Link is not ready for gripper OPEN/CLOSE confirmation.";
		return false;
	}
	if(!robostar->getGripperOpenStatus()){
		reason = "Gripper OPEN confirmation is not ON.";
		return false;
	}
	if(PlcBin == NULL || PlcBin->ClientSocket_PLC == NULL ||
		!PlcBin->ClientSocket_PLC->Active){
		reason = "PLC is not connected. D10104 cannot be verified.";
		return false;
	}
	if(!PlcBin->IsSourceCentering()){
		reason = "D10104 Source Centering is OFF.";
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
bool __fastcall TDryRunForm::ValidateWaitPositionStart(AnsiString &reason)
{
	reason = "";
	if(MainForm == NULL || robostar == NULL || gripper == NULL){
		reason = "Required motion module is not initialized.";
		return false;
	}
	if(MainForm->equipMode != modeManual){
		reason = "Equipment mode must be MANUAL.";
		return false;
	}
	if(MainForm->IsProductionSequenceBusy()){
		reason = "A production PLC/FMS/local sequence is still active.";
		return false;
	}
	if((int)gripper->seq != 0 || gripper->pauseStatus){
		reason = "Gripper sequence is not IDLE.";
		return false;
	}
	if(robostar->seq != seqIdle || robostar->pauseStatus){
		reason = "Robot sequence is not IDLE.";
		return false;
	}
	if(!robostar->RestoreServoState() || !MainForm->m_ServoOpen ||
		!MainForm->m_ServoON || !MainForm->m_ServoHomeEmg){
		reason = "Servo OPEN/ON or origin return is not complete.";
		return false;
	}
	if(!robostar->IsSafetyReady() || robostar->IsEmergencyStopActive() ||
		!robostar->IsKeyLockActive()){
		reason = "Safety Reset, Emergency Stop, Door or Keylock condition is not ready.";
		return false;
	}
	if(!robostar->IsCcLinkReady()){
		reason = "CC-Link is not ready.";
		return false;
	}
	if(PlcBin == NULL || PlcBin->ClientSocket_PLC == NULL ||
		!PlcBin->ClientSocket_PLC->Active || !PlcBin->IsSourceCentering()){
		reason = "PLC connection and D10104 Source Centering=ON are required.";
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
bool __fastcall TDryRunForm::ApplyDryRunSpeed(AnsiString &reason)
{
	int speed = editSpeed->Text.ToIntDef(0);
	if(speed < 200 || speed > 2000){
		reason = "Dry Run speed must be between 200 and 2000.";
		return false;
	}
	//* DRY RUN : Keep acceleration/deceleration at the commissioned test value.
	//* Increasing these time constants together with speed made short channel
	//* moves look slow because the axis could not reach its command speed.
	const int DryRunAccelerationMs = 300;
	const int DryRunDecelerationMs = 300;
	robostar->req_Speed(speed, DryRunAccelerationMs, DryRunDecelerationMs);
	WriteDryRunLog("SPEED/APPLIED speed/acc/dec=" + IntToStr(speed) + "/" +
		IntToStr(DryRunAccelerationMs) + "/" + IntToStr(DryRunDecelerationMs));
	return true;
}
//---------------------------------------------------------------------------
bool __fastcall TDryRunForm::CheckDryRunRuntimeInterlock(AnsiString &reason)
{
	reason = "";
	if(robostar == NULL || MainForm == NULL){
		reason = "Motion module is unavailable.";
		return false;
	}
	//* DRY RUN : A mode change during inspection must never hand motion back to
	//* the automatic production sequence.
	if(MainForm->equipMode != modeManual){
		reason = "Equipment mode changed from MANUAL.";
		return false;
	}
	if(PlcBin == NULL || PlcBin->ClientSocket_PLC == NULL ||
		!PlcBin->ClientSocket_PLC->Active){
		reason = "PLC disconnected while monitoring D10104.";
		return false;
	}
	if(!PlcBin->IsSourceCentering()){
		reason = "D10104 Source Centering changed to OFF.";
		return false;
	}
	if(!robostar->IsSafetyReady() || robostar->IsEmergencyStopActive() ||
		!robostar->IsKeyLockActive()){
		reason = "Safety, Emergency Stop, Door or Keylock changed to NOT READY.";
		return false;
	}
	if(!MainForm->m_ServoOpen || !MainForm->m_ServoON){
		reason = "Servo OPEN/ON status was lost.";
		return false;
	}
	if(!robostar->IsCcLinkReady()){
		reason = "CC-Link readiness was lost.";
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
bool __fastcall TDryRunForm::CheckStepTimeout()
{
	if(stepTimeoutMs == 0) return false;
	if((DWORD)(GetTickCount() - stepStartTick) <= stepTimeoutMs) return false;
	AbortDryRun("Step timeout: " + lblCurrentStep->Caption);
	return true;
}
//---------------------------------------------------------------------------
void __fastcall TDryRunForm::AbortDryRun(const AnsiString &reason)
{
	//* DRY RUN : Fail closed. Do not issue another motion after an interlock loss.
	if(robostar != NULL)
		robostar->req_Stop();
	running = false;
	waitPositionRequested = false;
	drySequence = drsError;
	dryStep = 0;
	pnlRunState->Caption = "ERROR";
	pnlRunState->Color = clRed;
	btnStart->Enabled = true;
	btnStop->Enabled = false;
	btnWaitPosition->Enabled = true;
	if(MainForm != NULL && MainForm->btnDryRun != NULL)
		MainForm->btnDryRun->Color = clRed;
	WriteDryRunLog("ERROR - " + reason);
	ShowMessage(UnicodeString("Dry Run stopped.\r\n") + UnicodeString(reason));
}
//---------------------------------------------------------------------------
void __fastcall TDryRunForm::CompleteDryRun(const AnsiString &message)
{
	running = false;
	waitPositionRequested = false;
	drySequence = drsIdle;
	dryStep = 0;
	pnlRunState->Caption = "COMPLETE";
	pnlRunState->Color = clLime;
	btnStart->Enabled = true;
	btnStop->Enabled = false;
	btnWaitPosition->Enabled = true;
	if(MainForm != NULL && MainForm->btnDryRun != NULL)
		MainForm->btnDryRun->Color = clWhite;
	WriteDryRunLog(message);
}
//---------------------------------------------------------------------------
void __fastcall TDryRunForm::btnStartClick(TObject *Sender)
{
	//* DRY RUN : Start is accepted only from an idle robot at physical HOME.
	if(running) return;

	startChannel = editStartChannel->Text.ToIntDef(0);
	endChannel = editEndChannel->Text.ToIntDef(0);
	if(startChannel < 1 || startChannel > 96 || endChannel < startChannel || endChannel > 96){
		ShowMessage(L"Dry Run channel range must be 1 to 96 and Start <= End.");
		return;
	}

	AnsiString reason;
	if(!ValidateDryRunStart(reason)){
		WriteDryRunLog("START BLOCKED - " + reason);
		ShowMessage(UnicodeString("Dry Run cannot start.\r\n") + UnicodeString(reason));
		return;
	}

	if(!ApplyDryRunSpeed(reason)){
		WriteDryRunLog("START BLOCKED - " + reason);
		ShowMessage(UnicodeString(reason));
		return;
	}

	currentChannel = startChannel;
	completedCycleCount = 0;
	running = true;
	waitPositionRequested = false;
	btnStart->Enabled = false;
	btnStop->Enabled = true;
	btnWaitPosition->Enabled = false;
	pnlRunState->Caption = "RUNNING";
	pnlRunState->Color = clLime;
	if(MainForm->btnDryRun != NULL)
		MainForm->btnDryRun->Color = clLime;
	SetDryStep(drsSorting, 0, "SORTING / select Source channel", 5000);
	WriteDryRunLog("START Channel=" + IntToStr(startChannel) + ".." + IntToStr(endChannel) +
		" / D10104=ON / cell detect ignored / no fixed dwell");
}
//---------------------------------------------------------------------------
void __fastcall TDryRunForm::btnStopClick(TObject *Sender)
{
	//* DRY RUN : STOP means immediate axis stop at the current position. It does
	//* not raise Z or return HOME; WAIT POSITION is a separate operator command.
	if(!running) return;
	if(robostar != NULL)
		robostar->req_Stop();
	running = false;
	waitPositionRequested = false;
	drySequence = drsIdle;
	dryStep = 0;
	currentChannel = 0;
	pnlRunState->Caption = "STOPPED";
	pnlRunState->Color = clYellow;
	lblCurrentStep->Caption = "Current step : STOPPED AT CURRENT POSITION";
	btnStart->Enabled = true;
	btnStop->Enabled = false;
	btnWaitPosition->Enabled = true;
	if(MainForm != NULL && MainForm->btnDryRun != NULL)
		MainForm->btnDryRun->Color = clYellow;
	WriteDryRunLog("STOP COMPLETE / axes stopped at current position / next START begins at Start Channel");
}
//---------------------------------------------------------------------------
void __fastcall TDryRunForm::btnWaitPositionClick(TObject *Sender)
{
	//* DRY RUN : Manual recovery command after an immediate STOP. It raises Z,
	//* opens the empty gripper, then moves X/Y to the configured wait position.
	if(running) return;

	AnsiString reason;
	if(!ValidateWaitPositionStart(reason)){
		WriteDryRunLog("WAIT POSITION BLOCKED - " + reason);
		ShowMessage(UnicodeString("Wait Position cannot start.\r\n") + UnicodeString(reason));
		return;
	}
	if(!ApplyDryRunSpeed(reason)){
		WriteDryRunLog("WAIT POSITION BLOCKED - " + reason);
		ShowMessage(UnicodeString(reason));
		return;
	}

	currentChannel = 0;
	running = true;
	waitPositionRequested = true;
	btnStart->Enabled = false;
	btnStop->Enabled = true;
	btnWaitPosition->Enabled = false;
	pnlRunState->Caption = "WAIT POSITION";
	pnlRunState->Color = clLime;
	SetDryStep(drsReturn, 0, "WAIT POSITION / Z UP", 30000);
	WriteDryRunLog("WAIT POSITION START / Z UP -> GRIPPER OPEN -> X/Y=0");
}
//---------------------------------------------------------------------------
void __fastcall TDryRunForm::DryRunSorting()
{
	//* DRY RUN : Select channels by number only; NG/tray information is never read.
	switch(dryStep){
		case 0:
		{
			int sourceMap = MainForm->mapSort[0][currentChannel - 1];
			WriteDryRunLog("SORTING SourceCh=" + IntToStr(currentChannel) +
				" Map=" + IntToStr(sourceMap));
			robostar->req_AutoMove(1, 1, sourceMap, 962);
			if(robostar->seq != seqAutoMove){
				AbortDryRun("Source channel move request was rejected.");
				return;
			}
			SetDryStep(drsSorting, 1, "MOVE SOURCE CHANNEL " + IntToStr(currentChannel), 30000);
			break;
		}
		case 1:
			if(robostar->seq == seqIdle){
				WriteDryRunLog("MOVE SOURCE COMPLETE Channel=" + IntToStr(currentChannel));
				SetDryStep(drsEject, 0, "EJECT / Z DOWN", 5000);
			}
			break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDryRunForm::DryRunEject()
{
	//* DRY RUN : Eject ignores X0022 cell detection and has no stabilization dwell.
	switch(dryStep){
		case 0:
			if(!robostar->req_zDown()){
				AbortDryRun("Source Z DOWN request was rejected.");
				return;
			}
			SetDryStep(drsEject, 1, "EJECT / SOURCE Z DOWN", 30000);
			break;
		case 1:
			if(robostar->seq == seqIdle){
				robostar->GripperChuck(1, false, true);
				WriteDryRunLog("EJECT GRIPPER CLOSE command / X0022 ignored");
				SetDryStep(drsEject, 2, "EJECT / wait X0020 CHUCK", 5000);
			}
			break;
		case 2:
			if(robostar->input.GRIPPER1_CHUCK){
				robostar->req_zUp();
				SetDryStep(drsEject, 3, "EJECT / Z UP", 30000);
			}
			break;
		case 3:
			if(robostar->seq == seqIdle){
				WriteDryRunLog("EJECT COMPLETE Channel=" + IntToStr(currentChannel));
				SetDryStep(drsInsert, 0, "INSERT / move Target channel", 5000);
			}
			break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDryRunForm::DryRunInsert()
{
	//* DRY RUN : Insert ignores X0022 and does not save/report any cell information.
	switch(dryStep){
		case 0:
			robostar->req_AutoMove(2, 1, currentChannel, 96);
			if(robostar->seq != seqAutoMove){
				AbortDryRun("Target channel move request was rejected.");
				return;
			}
			WriteDryRunLog("MOVE TARGET Channel=" + IntToStr(currentChannel));
			SetDryStep(drsInsert, 1, "MOVE TARGET CHANNEL " + IntToStr(currentChannel), 30000);
			break;
		case 1:
			if(robostar->seq == seqIdle){
				if(!robostar->req_zDown()){
					AbortDryRun("Target Z DOWN request was rejected.");
					return;
				}
				SetDryStep(drsInsert, 2, "INSERT / TARGET Z DOWN", 30000);
			}
			break;
		case 2:
			if(robostar->seq == seqIdle){
				robostar->GripperChuck(1, true, false);
				WriteDryRunLog("INSERT GRIPPER OPEN command / X0022 ignored");
				SetDryStep(drsInsert, 3, "INSERT / wait X0021 UNCHUCK", 5000);
			}
			break;
		case 3:
			if(robostar->input.GRIPPER1_UNCHUCK && !robostar->input.GRIPPER1_CHUCK){
				robostar->req_zUp();
				SetDryStep(drsInsert, 4, "INSERT / Z UP", 30000);
			}
			break;
		case 4:
			if(robostar->seq == seqIdle){
				WriteDryRunLog("INSERT COMPLETE Channel=" + IntToStr(currentChannel));
				currentChannel++;
				if(currentChannel <= endChannel){
					SetDryStep(drsSorting, 0, "NEXT CHANNEL / direct Source move", 5000);
				}else{
					//* DRY RUN : Continuous exhibition loop. Keep Z at zero and
					//* restart from Start Channel until the operator presses STOP.
					completedCycleCount++;
					WriteDryRunLog("CYCLE COMPLETE Count=" + IntToStr(completedCycleCount) +
						" / restart Channel=" + IntToStr(startChannel));
					currentChannel = startChannel;
					SetDryStep(drsSorting, 0, "REPEAT / SOURCE CHANNEL " +
						IntToStr(startChannel), 5000);
				}
			}
			break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDryRunForm::DryRunReturn()
{
	//* DRY RUN : Dedicated return has no production STEP panel or cell condition.
	switch(dryStep){
		case 0:
			if(robostar->seq != seqIdle) return;
			if(robostar->mr2.pos[Axis_z] != 0){
				robostar->req_zUp();
				SetDryStep(drsReturn, 1, "RETURN / Z UP", 30000);
			}else{
				SetDryStep(drsReturn, 2, "RETURN / OPEN GRIPPER", 5000);
			}
			break;
		case 1:
			if(robostar->seq == seqIdle)
				SetDryStep(drsReturn, 2, "RETURN / OPEN GRIPPER", 5000);
			break;
		case 2:
			robostar->GripperChuck(1, true, false);
			SetDryStep(drsReturn, 3, "RETURN / wait X0021 UNCHUCK", 5000);
			break;
		case 3:
			if(robostar->input.GRIPPER1_UNCHUCK && !robostar->input.GRIPPER1_CHUCK){
				if(!robostar->req_DryRunWaitPosition()){
					AbortDryRun("Dry Run HOME return request was rejected.");
					return;
				}
				SetDryStep(drsReturn, 4, "RETURN / X/Y HOME", 30000);
			}
			break;
		case 4:
			if(robostar->seq == seqIdle){
				CompleteDryRun(waitPositionRequested ?
					"WAIT POSITION COMPLETE / Z UP / GRIPPER OPEN / X/Y=0" :
					"DRY RUN COMPLETE / Z UP and HOME");
			}
			break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TDryRunForm::dryRunTimerTimer(TObject *Sender)
{
	UpdateDryRunStatus();
	if(!running) return;

	//* DRY RUN : D10104 and safety are monitored continuously, not only at START.
	AnsiString reason;
	if(!CheckDryRunRuntimeInterlock(reason)){
		AbortDryRun(reason);
		return;
	}
	if(CheckStepTimeout()) return;

	switch(drySequence){
		case drsSorting: DryRunSorting(); break;
		case drsEject:   DryRunEject(); break;
		case drsInsert:  DryRunInsert(); break;
		case drsReturn:  DryRunReturn(); break;
		default: break;
	}
}
//---------------------------------------------------------------------------
