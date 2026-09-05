#include <string>
#include <cstdio>
#include <cassert>
typedef std::string AnsiString;
typedef std::string UnicodeString;
typedef unsigned long DWORD;
DWORD GetTickCount(){return 100;}
AnsiString IntToStr(int n){char s[32];sprintf(s,"%d",n);return s;}
const int modeAuto=1,modeManual=0,fmsAlarmNone=0;
struct Motion {bool pauseStatus;void req_Pause(bool v){pauseStatus=v;}} g,r,*gripper=&g,*robostar=&r;
struct Label{AnsiString Caption;};
struct Alarm{
    Label title,*errMsg1;int shows;bool Visible;
    Alarm():errMsg1(&title),shows(0),Visible(false){}
    void ShowError(AnsiString a,UnicodeString){++shows;Visible=true;title.Caption="S_Maint_"+a;}
    void Hide(){Visible=false;}
} alarm,*AlarmForm=&alarm;
struct Check{bool Checked;}; struct Step{int step;}; struct Timer{bool Enabled;};
struct PLC{
    bool fresh,autoMode,tray,center,targetTray,targetCenter,out,request;
    bool IsPlcStatusFresh(int){return fresh;} bool IsPlcAutoMode(){return autoMode;}
    bool IsSourceTrayIn(){return tray;} bool IsSourceCentering(){return center;}
    bool IsTargetTrayIn(){return targetTray;} bool IsTargetCentering(){return targetCenter;}
    bool IsSourceTrayOutOn(){return out;} bool IsSourceCenteringRequestOn(){return request;}
    void CmdSourceCenteringRequest(bool v){request=v;}
} plc,*PlcBin=&plc;
struct Opc {int requests; void PROCESS_START_REQUEST(){++requests;}} opc,*MesOpc=&opc;
struct Gateway{bool IsGatewayConnected(){return true;}} gateway,*Mod_Fms=&gateway;
struct TMainForm{
    int equipMode,fmsAlarmTransaction,logs,currentProcessStep,opcProcessStartResponseResult;
    bool sourceTrayCycleAdmitted,sourceCenteringCompleted,sourceTrayOutPending;
    bool workStartTrayAlarmActive;int workStartTrayAlarmStep;
    bool opcTrayDisplayed[2],opcTrayLoaded[2],opcProcessStarted,opcProcessStartPending,opcProcessEndPending;
    bool opcProcessStartWaitResponseOff,opcProcessStartResponseOffError;
    DWORD opcProcessStartTick;
    Timer timer,*opcMesTimer;
    Check door,bypass,*chkDoorPlcAuto,*chkBypass;
    Step step[2];
    AnsiString lastProcessWaitStatus[16],lastWorkTraySignalStatus[2],currentProcessDetail;
    TMainForm():equipMode(modeAuto),fmsAlarmTransaction(0),logs(0),sourceTrayCycleAdmitted(false),
        sourceCenteringCompleted(false),sourceTrayOutPending(false),opcProcessStarted(false),opcProcessStartPending(false),
        opcProcessEndPending(false),opcMesTimer(&timer),chkDoorPlcAuto(&door),chkBypass(&bypass){
        door.Checked=bypass.Checked=false;step[0].step=step[1].step=0;
        workStartTrayAlarmActive=false;workStartTrayAlarmStep=6;alarm.shows=0;alarm.Visible=false;
        opcTrayDisplayed[0]=opcTrayDisplayed[1]=opcTrayLoaded[0]=opcTrayLoaded[1]=false;
        g.pauseStatus=r.pauseStatus=false;opc.requests=0;
        plc.fresh=true;plc.autoMode=true;plc.tray=true;plc.center=false;
        plc.targetTray=plc.targetCenter=plc.out=plc.request=false;
    }
    void ReportIdleWaitStatus(){} void UpdateProcessFlowPanel(){}
    void ProcessStepLog(int,AnsiString){++logs;}
    void CompleteProcessStep(int n,AnsiString){currentProcessStep=n;}
    void BeginProcessStep(int n,AnsiString){currentProcessStep=n;}
    void memoMainLineAdd(AnsiString){}
    void PauseFmsAlarm(){g.pauseStatus=r.pauseStatus=true;}
    bool CheckAutomaticFmsMode(AnsiString){return equipMode==modeAuto;}
    void SetProcessOperationStatus(int n,AnsiString,AnsiString,AnsiString,AnsiString){currentProcessStep=n;}
    bool __fastcall IsSourceTrayInSignal() const;
    bool __fastcall IsSourceCenteringSignal() const;
    bool __fastcall IsTargetTrayInSignal() const;
    bool __fastcall IsTargetCenteringSignal() const;
    bool __fastcall CanRequestAutoSourceCentering() const;
    bool __fastcall IsSourceTrayCycleReady() const;
    bool __fastcall UpdateSourceTrayAdmission();
    bool __fastcall CompleteSourceCenteringStep();
    bool __fastcall CheckWorkTraySignals(int,AnsiString&);
    bool __fastcall RetryWorkStartTrayAlarm();
    void __fastcall TryStartOpcProcess();
};
bool __fastcall TMainForm::IsSourceTrayInSignal() const
{
	//* DOOR/PLC AUTO INTERLOCK: actual PLC input only; no simulated Tray In.
	return PlcBin != NULL && PlcBin->IsSourceTrayIn();
}

bool __fastcall TMainForm::IsSourceCenteringSignal() const
{
	//* DOOR/PLC AUTO INTERLOCK: checkbox NEVER forces centering ON.
	return PlcBin != NULL && PlcBin->IsSourceCentering();
}

bool __fastcall TMainForm::IsTargetTrayInSignal() const
{
	return PlcBin != NULL && PlcBin->IsTargetTrayIn();
}

bool __fastcall TMainForm::IsTargetCenteringSignal() const
{
	//* DOOR/PLC AUTO INTERLOCK: actual Target centering remains mandatory.
	return PlcBin != NULL && PlcBin->IsTargetCentering();
}

bool __fastcall TMainForm::CanRequestAutoSourceCentering() const
{
	return equipMode == modeAuto && gripper != NULL && robostar != NULL &&
		!gripper->pauseStatus && !robostar->pauseStatus &&
		fmsAlarmTransaction == fmsAlarmNone && sourceTrayCycleAdmitted &&
		opcTrayDisplayed[0] && opcTrayLoaded[0] && !sourceCenteringCompleted &&
		!sourceTrayOutPending && step[0].step == 2 &&
		PlcBin != NULL && PlcBin->IsPlcStatusFresh(1000) &&
		PlcBin->IsPlcAutoMode() && PlcBin->IsSourceTrayIn() &&
		!PlcBin->IsSourceCentering() && !PlcBin->IsSourceTrayOutOn() &&
		!chkBypass->Checked;
}

bool __fastcall TMainForm::IsSourceTrayCycleReady() const
{
	// STEP 01 admission permits Source information loading BEFORE centering.
	// STEP 03 completion and four live inputs separately gate ProcessStart.
	return sourceTrayCycleAdmitted && PlcBin != NULL &&
		PlcBin->IsPlcStatusFresh(1000) && PlcBin->IsSourceTrayIn() &&
		!sourceTrayOutPending &&
		!PlcBin->IsSourceTrayOutOn() && step[0].step < 100;
}

bool __fastcall TMainForm::UpdateSourceTrayAdmission()
{
	if(PlcBin == NULL || !PlcBin->IsPlcStatusFresh(1000)){
		ReportIdleWaitStatus();
		return false; // Stale cached ON is not an admission or departure signal.
	}
	if(!PlcBin->IsSourceTrayIn()){
		sourceTrayCycleAdmitted = false;
		sourceCenteringCompleted = false;
		return false;
	}
	if(equipMode != modeAuto || gripper == NULL || robostar == NULL ||
		gripper->pauseStatus || robostar->pauseStatus ||
		fmsAlarmTransaction != fmsAlarmNone || sourceTrayOutPending ||
		PlcBin->IsSourceTrayOutOn() || step[0].step >= 100) return false;
	if(sourceTrayCycleAdmitted)
		return IsSourceTrayCycleReady();
	bool plcAuto = PlcBin->IsPlcAutoMode();
	bool test = chkDoorPlcAuto != NULL && chkDoorPlcAuto->Checked;
	// Preserve the existing normal BYPASS flow: do not center a tray to eject it.
	// Even BYPASS cannot dispatch a reverse-loaded tray in PLC MANUAL unless
	// Door/Auto is explicitly selected AND the actual tray is already centered.
	if(plcAuto || (test && PlcBin->IsSourceCentering())){
		sourceTrayCycleAdmitted = true;
		PlcBin->CmdSourceCenteringRequest(false);
		ProcessStepLog(1, "[DOOR/PLC AUTO INTERLOCK] ADMITTED / D10101=" + IntToStr(plcAuto ? 1 : 0) +
			" / D10103=1 / D10104=" + IntToStr(PlcBin->IsSourceCentering() ? 1 : 0) +
			" / Door/Auto=" + IntToStr(test ? 1 : 0) +
			" / STEP 01 admitted; Source TrayLoad precedes STEP 03 centering");
		return true;
	}
	ReportIdleWaitStatus();
	return false;
}

bool __fastcall TMainForm::CompleteSourceCenteringStep()
{
	if(!IsSourceTrayCycleReady() || !opcTrayDisplayed[0] || !opcTrayLoaded[0]) return false;
	bool centered = PlcBin->IsSourceCentering();
	if(sourceCenteringCompleted) return centered;
	bool plcAuto = PlcBin->IsPlcAutoMode();
	bool test = chkDoorPlcAuto != NULL && chkDoorPlcAuto->Checked;
	if(!plcAuto && !test){
		SetProcessOperationStatus(3, "SOURCE CENTERING", "D10101 PLC AUTO", "1 (ON)", "0 (OFF) / D10154=OFF");
		return false;
	}
	if(!centered){
		SetProcessOperationStatus(3, "SOURCE CENTERING", "D10104 Source Centering", "1 (ON)",
			"0 (OFF) / D10101=" + IntToStr(plcAuto ? 1 : 0) +
			" / D10154=" + IntToStr(PlcBin->IsSourceCenteringRequestOn() ? 1 : 0));
		return false;
	}
	sourceCenteringCompleted = true;
	PlcBin->CmdSourceCenteringRequest(false);
	CompleteProcessStep(3, "D10104=1 / D10154=OFF / PLC AUTO no longer gates this tray; real motion interlocks remain active");
	BeginProcessStep(4, "WAIT D10105 Target In + D10106 Target Centering");
	return true;
}

bool __fastcall TMainForm::CheckWorkTraySignals(int stepNo, AnsiString &detail)
{
	// ========================================================================
	//* WORK START TRAY INTERLOCK : STEP 06/07 ONLY, NOT NORMAL STEP 01-05 WAIT.
	// Closing FormAlarm or recovering an input never restarts the equipment.
	// Keep one alarm latched until the operator explicitly presses Restart.
	// ========================================================================
	if(workStartTrayAlarmActive){
		detail = "Work start tray interlock latched / PAUSED / operator Restart required";
		return false;
	}
	bool fresh = PlcBin != NULL && PlcBin->IsPlcStatusFresh(1000);
	bool srcIn = IsSourceTrayInSignal();
	bool srcCenter = IsSourceCenteringSignal();
	bool tgtIn = IsTargetTrayInSignal();
	bool tgtCenter = IsTargetCenteringSignal();
	bool ready = fresh && srcIn && srcCenter && tgtIn && tgtCenter;
	AnsiString missing;
	if(!fresh) missing += "PLC status stale/disconnected; ";
	if(!srcIn) missing += "D10103 Source In; ";
	if(!srcCenter) missing += "D10104 Source Centering; ";
	if(!tgtIn) missing += "D10105 Target In; ";
	if(!tgtCenter) missing += "D10106 Target Centering; ";
	detail = "D10103=" + IntToStr(srcIn ? 1 : 0) + " / D10104=" + IntToStr(srcCenter ? 1 : 0) +
		" / D10105=" + IntToStr(tgtIn ? 1 : 0) + " / D10106=" + IntToStr(tgtCenter ? 1 : 0) +
		" / PLC_FRESH=" + IntToStr(fresh ? 1 : 0) + (ready ? AnsiString(" / PASS") : " / BLOCKED: " + missing);
	// Log each changed signal combination (including WAIT -> PASS) once.
	AnsiString status = "WORK START INTERLOCK / EXPECTED D10103/D10104/D10105/D10106=1, PLC_FRESH=1 / CURRENT " + detail;
	currentProcessStep = stepNo;
	currentProcessDetail = status;
	UpdateProcessFlowPanel();
	int checkIndex = stepNo == 6 ? 0 : 1;
	if(lastWorkTraySignalStatus[checkIndex] != status){
		lastWorkTraySignalStatus[checkIndex] = status;
		ProcessStepLog(stepNo, status);
	}
	if(!ready){
		workStartTrayAlarmActive = true; // Latch before any modeless UI can re-enter.
		workStartTrayAlarmStep = stepNo;
		if(gripper != NULL) gripper->req_Pause(true);
		if(robostar != NULL) robostar->req_Pause(true);
		// A ProcessStart FMS retry must also stop polling/reissuing until Restart.
		if(fmsAlarmTransaction != fmsAlarmNone) PauseFmsAlarm();
		if(AlarmForm != NULL)
			AlarmForm->ShowError("Work start tray interlock",
				UnicodeString("Work cannot start. All four tray signals must be ON.\r\n\r\n") +
				UnicodeString(detail) +
				UnicodeString("\r\n\r\nCorrect the signals, then press Main Restart. Closing this alarm does not resume work."));
	}
	return ready;
}

bool __fastcall TMainForm::RetryWorkStartTrayAlarm()
{
	//* WORK START TRAY INTERLOCK: shared by Main Restart and FMS popup Retry.
	if(!workStartTrayAlarmActive) return true;
	int stepNo = workStartTrayAlarmStep;
	workStartTrayAlarmActive = false;
	lastWorkTraySignalStatus[stepNo == 6 ? 0 : 1] = "";
	// Do not hide a different alarm which might have appeared in the meantime.
	if(AlarmForm != NULL && AlarmForm->errMsg1->Caption == "S_Maint_Work start tray interlock")
		AlarmForm->Hide();
	AnsiString detail;
	return CheckWorkTraySignals(stepNo, detail); // Failure re-latches and stays paused.
}

void __fastcall TMainForm::TryStartOpcProcess()
{
	if(!CheckAutomaticFmsMode("ProcessStart"))
		return;
	//* DOOR/PLC AUTO INTERLOCK: real inputs remain required, PLC AUTO is not rechecked.
	// TRAY LOAD RESTART: an accepted response must not start a new process
	// while either production sequence is paused or an FMS alarm owns recovery.
	if(gripper == NULL || robostar == NULL || gripper->pauseStatus ||
		robostar->pauseStatus || fmsAlarmTransaction != fmsAlarmNone)
		return;
	// A new process must not start while the previous ProcessEnd handshake is active.
	if (opcProcessStarted || opcProcessStartPending || opcProcessEndPending)
		return;
	if (!opcTrayDisplayed[0] || !opcTrayDisplayed[1] ||
		!opcTrayLoaded[0] || !opcTrayLoaded[1])
		return;
	if(!sourceCenteringCompleted) return;
	if (MesOpc == NULL || Mod_Fms == NULL || !Mod_Fms->IsGatewayConnected())
		return;
	// STEP 06: live inputs may have changed during the two FMS handshakes.
	AnsiString traySignalState;
	if(!CheckWorkTraySignals(6, traySignalState) || !IsSourceTrayCycleReady()) return;

	BeginProcessStep(6, "ProcessStart request / wait response");
	MesOpc->PROCESS_START_REQUEST();
	opcProcessStartPending = true;
	opcProcessStartWaitResponseOff = false;
	opcProcessStartResponseOffError = false;
	opcProcessStartResponseResult = 0;
	opcProcessStartTick = GetTickCount();
	opcMesTimer->Enabled = true;
	memoMainLineAdd("[FMS OPC UA] Process start request.");
}

int main(){
    {
        TMainForm f;
        assert(f.UpdateSourceTrayAdmission() && f.IsSourceTrayCycleReady());
        assert(!f.CanRequestAutoSourceCentering() && !f.CompleteSourceCenteringStep());
        f.step[0].step=2;f.opcTrayDisplayed[0]=true;
        assert(!f.CanRequestAutoSourceCentering()); // Response=1 display alone is insufficient.
        f.opcTrayLoaded[0]=true;assert(f.CanRequestAutoSourceCentering());
        plc.autoMode=false;assert(!f.CanRequestAutoSourceCentering());
        plc.center=true;assert(!f.CompleteSourceCenteringStep());
        plc.autoMode=true;assert(f.CompleteSourceCenteringStep() && f.currentProcessStep==4);
        plc.autoMode=false;assert(f.CompleteSourceCenteringStep() && f.IsSourceTrayCycleReady());
        plc.center=false;assert(!f.CompleteSourceCenteringStep() && !f.CanRequestAutoSourceCentering());
        plc.center=true;plc.tray=false;assert(!f.UpdateSourceTrayAdmission() && !f.sourceCenteringCompleted);
    }
    puts("PASS: STEP01 admission -> STEP02 display AND reset -> STEP03 centering; PLC MANUAL only permitted after centering completion");
    for(int bits=0;bits<32;++bits){
        TMainForm f;f.sourceTrayCycleAdmitted=f.sourceCenteringCompleted=true;
        f.opcTrayDisplayed[0]=f.opcTrayDisplayed[1]=f.opcTrayLoaded[0]=f.opcTrayLoaded[1]=true;
        f.door.Checked=true;plc.autoMode=false;
        plc.tray=(bits&1)!=0;plc.center=(bits&2)!=0;
        plc.targetTray=(bits&4)!=0;plc.targetCenter=(bits&8)!=0;plc.fresh=(bits&16)!=0;
        f.TryStartOpcProcess();assert(opc.requests==(bits==31?1:0));
        if(bits!=31){int count=f.logs;f.TryStartOpcProcess();assert(f.logs==count && alarm.shows==1 && g.pauseStatus && r.pauseStatus);}
    }
    puts("PASS: 32 live-input combinations; only 4 ON + fresh sends ProcessStart, even with Door/Auto enabled");
    {
        TMainForm f;AnsiString detail;plc.center=plc.targetTray=plc.targetCenter=true;
        assert(f.CheckWorkTraySignals(6,detail));int count=f.logs;
        assert(f.CheckWorkTraySignals(6,detail) && f.logs==count);
        plc.targetTray=false;assert(!f.CheckWorkTraySignals(6,detail) && f.logs==count+1);
        assert(detail.find("D10105 Target In")!=AnsiString::npos);
        assert(!f.CheckWorkTraySignals(6,detail) && f.logs==count+1);
        alarm.Hide();plc.targetTray=true;
        assert(!f.CheckWorkTraySignals(6,detail) && f.logs==count+1 && alarm.shows==1);
        assert(f.RetryWorkStartTrayAlarm() && !f.workStartTrayAlarmActive && f.logs==count+2);
        plc.targetCenter=false;assert(!f.CheckWorkTraySignals(7,detail));
        assert(!f.RetryWorkStartTrayAlarm() && f.workStartTrayAlarmActive && alarm.Visible);
    }
    puts("PASS: failed check shows FormAlarm and pauses once; Close/input recovery cannot resume; Restart rechecks and re-alarms if invalid");
    {
        TMainForm f;plc.autoMode=false;f.door.Checked=true;
        assert(!f.UpdateSourceTrayAdmission());plc.center=true;assert(f.UpdateSourceTrayAdmission());
        f.opcTrayDisplayed[0]=f.opcTrayLoaded[0]=true;f.step[0].step=2;
        assert(!f.CanRequestAutoSourceCentering() && f.CompleteSourceCenteringStep());
    }
    puts("PASS: Door/Auto uses actual centered tray, never requests centering in PLC MANUAL");
}
