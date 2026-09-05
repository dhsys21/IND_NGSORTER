#include <string>
#include <cstdio>
#include <cstdlib>
#include <cassert>
class AnsiString : public std::string {
public:
    AnsiString() {}
    AnsiString(const char *s) : std::string(s) {}
    AnsiString(const std::string &s) : std::string(s) {}
    bool IsEmpty() const { return empty(); }
    int ToIntDef(int fallback) const { return empty() ? fallback : std::atoi(c_str()); }
};
const int modeAuto=1, modeManual=0, fmsAlarmNone=0, clSilver=0;
unsigned long GetTickCount() { return 100; }
struct Panel { AnsiString Caption; int Color; };
struct Timer { bool Enabled; };
struct Tray {};
struct Step { int step; };
struct Stage { int limitCnt; };
class Tgripper {
public:
    enum gripperSequence { seqIdle, seqInit, seqSorting, seqInserting, seqPause };
    bool pauseStatus;
    gripperSequence seq, seq_save;
    Tgripper() : pauseStatus(false), seq(seqIdle), seq_save(seqIdle) {}
    bool __fastcall IsSortingWorkActive() const;
};
struct Robot { bool pauseStatus; };
struct PLC { int requests; void CmdSourceCenteringRequest(bool) { ++requests; } };
struct Opc { int loads; void TRAY_LOAD_REQUEST(bool) { ++loads; } void PROCESS_START_CANCEL() {} };
struct Gateway { bool IsGatewayConnected() { return true; } };
struct ErrorWindow { void ShowError() { assert(false); } };
Tgripper grip, *gripper=&grip;
Robot robot, *robostar=&robot;
PLC plc, *PlcBin=&plc;
Opc opc, *MesOpc=&opc;
Gateway gateway, *Mod_Fms=&gateway;
ErrorWindow errorWindow, *ErrorForm_limit=&errorWindow;
class TMainForm {
public:
    int equipMode, fmsAlarmTransaction;
    bool opcTrayAdvanceDeferred[2], opcTrayDisplayed[2], opcTrayLoaded[2];
    bool opcTrayLoadPending[2], opcTrayLoadRetryRequired[2], opcTrayLoadWaitResponseOff[2], opcTrayLoadResponseOffError[2];
    unsigned long opcTrayLoadStartTick[2], opcProcessStartTick;
    bool opcProcessStarted, opcSortingStartPending, opcSortingStartWaitError;
    bool opcProcessStartPending, opcProcessStartWaitResponseOff, opcProcessStartResponseOffError;
    int opcProcessStartResponseResult;
    bool targetTrayInfoPromptActive, centered;
    AnsiString opcDeferredTrayId[2], targetTrayInfoActiveId;
    Panel panel[5], *pTrayid_source, *pTrayid_target, *pTrayid_target2, *pbad_sum, *pwork1, *pwork2;
    Tray tray_source, tray_target, *tray;
    Step step[2]; Stage stage; Timer timer, *opcMesTimer;
    int scans, restores, starts, errors;
    TMainForm() {
        equipMode=modeAuto; fmsAlarmTransaction=fmsAlarmNone;
        for(int i=0;i<2;++i) {
            opcTrayAdvanceDeferred[i]=opcTrayDisplayed[i]=opcTrayLoaded[i]=false;
            opcTrayLoadPending[i]=opcTrayLoadRetryRequired[i]=false;
            opcTrayLoadWaitResponseOff[i]=opcTrayLoadResponseOffError[i]=false;
            step[i].step=0;
        }
        opcProcessStarted=opcSortingStartPending=opcSortingStartWaitError=false;
        opcProcessStartPending=opcProcessStartWaitResponseOff=opcProcessStartResponseOffError=false;
        targetTrayInfoPromptActive=false; centered=true;
        targetTrayInfoActiveId="TR-20260818-trg";
        pTrayid_source=&panel[0]; pTrayid_target=&panel[1]; pTrayid_target2=&panel[2];
        pbad_sum=&panel[3]; pwork1=&panel[3]; pwork2=&panel[4];
        pbad_sum->Caption="1"; stage.limitCnt=96; opcMesTimer=&timer;
        scans=restores=starts=errors=0;
        grip=Tgripper(); robot.pauseStatus=false; plc.requests=0; opc.loads=0;
    }
    void __fastcall AdvanceOpcTrayLoad(bool);
    void __fastcall ResumeDeferredTrayLoads();
    void __fastcall NotifyTrayInfo(AnsiString,bool);
    void ReportIdleWaitStatus() {}
    bool CheckAutomaticFmsMode(const AnsiString&) { return equipMode==modeAuto; }
    void ProcessStepLog(int,const AnsiString&) {}
    void CompleteProcessStep(int,const AnsiString&) {}
    void BeginProcessStep(int,const AnsiString&) {}
    void SetProcessWaitStatus(int,const AnsiString&,const AnsiString&,int) {}
    void memoMainLineAdd(const AnsiString&) {}
    void ShowCommonError(const AnsiString&,const AnsiString&) { ++errors; }
    bool IsTargetCenteringSignal() { return centered; }
    bool IsSourceCenteringSignal() { return true; }
    void TryStartOpcProcess() { ++starts; }
    int RestoreTargetTrayInfo(const AnsiString&,bool) { ++restores; return 0; }
    void ReadTargetTrayBarcode() { ++scans; NotifyTrayInfo("TR-20260818-trg",false); }
};
bool __fastcall Tgripper::IsSortingWorkActive() const
{
	// PAUSE alone is not evidence of an active cell operation. Preserve actual
	// initialization/pickup/insert reservations by inspecting the saved sequence.
	gripperSequence active = pauseStatus ? seq_save : seq;
	return active == seqInit || active == seqSorting || active == seqInserting;
}

void __fastcall TMainForm::AdvanceOpcTrayLoad(bool sourceTray)
{
	int index = sourceTray ? 0 : 1;
	int stepNo = sourceTray ? 2 : 5;
	AnsiString locationName = sourceTray ? "Location1" : "Location2";
	AnsiString trayId = sourceTray ? pTrayid_source->Caption : pTrayid_target->Caption;
	// TRAY LOAD RESTART: response handling may finish while an alarm has the
	// equipment paused. Remember completion, but issue no NEXT-stage requests
	// (centering, Target barcode or ProcessStart) until operator Restart.
	if(equipMode != modeAuto || gripper == NULL || robostar == NULL ||
		gripper->pauseStatus || robostar->pauseStatus){
		if(!opcTrayAdvanceDeferred[index])
			ProcessStepLog(stepNo, "HANDSHAKE COMPLETE / next stage deferred / WAIT operator Restart / "
				"Request=OFF / response accepted / no next-stage request sent");
		opcTrayAdvanceDeferred[index] = true;
		ReportIdleWaitStatus();
		return;
	}
	opcTrayAdvanceDeferred[index] = false;

	if(!opcTrayDisplayed[index])
	{
		opcTrayLoaded[index] = false;
		ProcessStepLog(stepNo, "ERROR - Target advance blocked: tray was not displayed");
		ShowCommonError(locationName + " TrayLoad sequence error",
			"TrayLoadResponse=1 display completion is required before Response=0.");
		return;
	}
	opcTrayLoaded[index] = true;
	CompleteProcessStep(stepNo, locationName +
		".TrayLoadResponse returned to 0 / Tray ID=" + trayId);
	memoMainLineAdd("[FMS OPC UA] " + locationName +
		" TrayLoad handshake complete; advancing to next process.");

	if(sourceTray)
	{
		if(pbad_sum->Caption.ToIntDef(0) <= stage.limitCnt)
		{
			// Source TrayLoad is complete. Request Source centering, but continue
			// directly to Target Tray loading; ProcessStart checks D10104 again.
			BeginProcessStep(3, "D10154 Centering Request=ON / wait D10104");
			SetProcessWaitStatus(3, "D10154 Source Centering Request=ON",
				"D10104 Source Centering", IsSourceCenteringSignal() ? 1 : 0);
			memoMainLineAdd("[FMS OPC UA] Source tray displayed; moving to Target tray process.");
			if(PlcBin != NULL) PlcBin->CmdSourceCenteringRequest(true);

			// Skip the old serial wait at step[0]=1. Target Tray load can run while
			// Source centering is completing.
			step[0].step = 2;
			if(opcTrayLoadPending[1] || opcTrayLoaded[1])
			{
				// Preserve an already active Target transaction; never scan twice.
				step[0].step = 3;
			}
			else if(IsTargetCenteringSignal())
			{
				BeginProcessStep(4, "D10106 Target Centering=ON / waiting barcode");
				pTrayid_target->Caption = "";
				pTrayid_target2->Caption = "";
				ReadTargetTrayBarcode();
				step[0].step = 3;
				step[1].step = 1;
			}
			else
			{
				SetProcessWaitStatus(4, "Source TrayLoad complete",
					"D10106 Target Centering", 0);
			}
		}
		else
		{
			opcTrayLoaded[0] = false;
			memoMainLineAdd("[FMS OPC UA] NG count exceeds the configured limit.");
			ErrorForm_limit->ShowError();
		}
	}

	TryStartOpcProcess();
	tray = &tray_target;
}

void __fastcall TMainForm::ResumeDeferredTrayLoads()
{
	// Called by the AUTO timer, never by an FMS polling callback while paused.
	if(equipMode != modeAuto || gripper == NULL || robostar == NULL ||
		gripper->pauseStatus || robostar->pauseStatus ||
		fmsAlarmTransaction != fmsAlarmNone)
		return;
	for(int i = 0; i < 2; ++i){
		if(gripper->pauseStatus || robostar->pauseStatus) return;
		if(opcTrayAdvanceDeferred[i])
			AdvanceOpcTrayLoad(i == 0);
		if(gripper->pauseStatus || robostar->pauseStatus) return;
		if(!opcDeferredTrayId[i].IsEmpty()){
			AnsiString trayId = opcDeferredTrayId[i];
			opcDeferredTrayId[i] = ""; // Clear before callbacks/modal dialogs can re-enter.
			NotifyTrayInfo(trayId, i == 0);
		}
	}
}

void __fastcall TMainForm::NotifyTrayInfo(AnsiString strTray, bool bsrc)
{
	if(!CheckAutomaticFmsMode(bsrc ? "Source TrayLoad" : "Target TrayLoad"))
		return;
	int index = bsrc ? 0 : 1;
	tray = bsrc ? &tray_source : &tray_target;

	//* 불량트레이 관리
	// Ignore actual duplicate requests and protect active cell reservations.
	// A paused IDLE sequence is NOT active sorting; the previous check treated
	// seqPause as work and discarded the next Source cycle's Location2 request.
	if(!bsrc && IsTargetCenteringSignal() &&
		targetTrayInfoActiveId == strTray &&
		(opcTrayLoadPending[1] || opcTrayLoaded[1] || opcTrayAdvanceDeferred[1] ||
		 (gripper != NULL && gripper->IsSortingWorkActive()))){
		memoMainLineAdd("[LOCAL TARGET] Duplicate Location2 load ignored; target map/reservation preserved. TrayId=" + strTray);
		tray = &tray_target;
		return;
	}
	// A barcode callback can also arrive during Pause. Retain the ID instead
	// of losing the request or replacing tray/reservation data behind the alarm.
	if(gripper == NULL || robostar == NULL || gripper->pauseStatus || robostar->pauseStatus){
		if(opcDeferredTrayId[index] != strTray)
			ProcessStepLog(bsrc ? 2 : 5, "TrayLoad NOT SENT / barcode retained / WAIT operator Restart / TrayId=" + strTray);
		opcDeferredTrayId[index] = strTray;
		ReportIdleWaitStatus();
		return;
	}
	if(!bsrc){
		ProcessStepLog(5, "PREPARE - load LOCAL Target tray information for FMS comparison / TrayId=" + strTray);
		//* 불량트레이 관리
		// 모달 확인창이 열린 동안 타이머가 다시 스캔을 호출해도 중첩 진입하지 않는다.
		if(targetTrayInfoPromptActive){
			tray = &tray_target;
			return;
		}
		targetTrayInfoPromptActive = true;
		int prepareResult = 0;
		try{
			// 바코드를 읽은 직후 기존 정보를 확인하고, 취소 시 FMS 요청도 보내지 않는다.
			// Use the dedicated FMS/LOCAL comparison dialog after TrackIn validation.
			// Do not show the legacy Yes/No confirmation before the FMS request.
			prepareResult = RestoreTargetTrayInfo(strTray, false);
		}
		catch(...){
			targetTrayInfoPromptActive = false;
			throw;
		}
		targetTrayInfoPromptActive = false;
		if(prepareResult < 0){
			opcTrayLoaded[1] = false;
			pwork2->Color = clSilver;
			memoMainLineAdd("[LOCAL TARGET] Operator cancelled before target tray load request.");
			tray = &tray_target;
			return;
		}
		ProcessStepLog(5, "READY - LOCAL Target tray information loaded / next=Location2 TrayLoad Request and FMS comparison");
	}
	opcTrayLoadRetryRequired[index] = false;
	opcTrayAdvanceDeferred[index] = false;
	opcDeferredTrayId[index] = "";
	opcTrayDisplayed[index] = false;
	opcTrayLoaded[index] = false;
	if (bsrc)
	{
		opcProcessStarted = false;
		opcSortingStartPending = false;
		opcSortingStartWaitError = false;
		if (opcProcessStartPending && MesOpc != NULL)
			MesOpc->PROCESS_START_CANCEL();
		opcProcessStartPending = false;
		opcProcessStartWaitResponseOff = false;
		opcProcessStartResponseOffError = false;
		opcProcessStartResponseResult = 0;
	}
	if (bsrc) pwork1->Color = clSilver;
	else pwork2->Color = clSilver;

	if (MesOpc == NULL || Mod_Fms == NULL || !Mod_Fms->IsGatewayConnected())
	{
		ProcessStepLog(bsrc ? 2 : 5, "ERROR - FMS Gateway disconnected / TrayLoad Request not sent");
		ShowCommonError("FMS Gateway is not connected",
			"Tray load request was not sent. Check the gateway connection.");
		return;
	}

	BeginProcessStep(bsrc ? 2 : 5, (bsrc ? AnsiString("Location1.Source") : AnsiString("Location2.Target")) +
		" TrayLoad Request=ON / Tray ID=" + strTray + " / WAIT TrayLoadResponse=1");
	ProcessStepLog(bsrc ? 2 : 5,
		(bsrc ? AnsiString("Location1") : AnsiString("Location2")) +
		".TrayLoad Request issued / WAIT " +
		(bsrc ? AnsiString("Location1") : AnsiString("Location2")) +
		".TrayLoadResponse=1");
	MesOpc->TRAY_LOAD_REQUEST(bsrc);
	opcTrayLoadPending[index] = true;
	opcTrayLoadWaitResponseOff[index] = false;
	opcTrayLoadResponseOffError[index] = false;
	opcTrayLoadStartTick[index] = GetTickCount();
	opcMesTimer->Enabled = true;
}

int main() {
    {
        TMainForm f; f.opcTrayDisplayed[0]=true;
        grip.pauseStatus=true; grip.seq=Tgripper::seqPause;
        robot.pauseStatus=true;
        f.AdvanceOpcTrayLoad(true);
        assert(f.opcTrayAdvanceDeferred[0] && !f.opcTrayLoaded[0]);
        assert(f.scans==0 && plc.requests==0 && opc.loads==0 && f.starts==0);
        f.ResumeDeferredTrayLoads();
        assert(f.scans==0);
        grip.pauseStatus=false; grip.seq=grip.seq_save; robot.pauseStatus=false;
        f.ResumeDeferredTrayLoads();
        assert(f.opcTrayLoaded[0] && !f.opcTrayAdvanceDeferred[0]);
        assert(f.scans==1 && plc.requests==1 && opc.loads==1 && f.opcTrayLoadPending[1]);
        f.ResumeDeferredTrayLoads();
        assert(f.scans==1 && opc.loads==1);
        puts("PASS: paused Source completion sends nothing; Restart sends Target load exactly once");
    }
    {
        TMainForm f; grip.pauseStatus=true; grip.seq=Tgripper::seqPause;
        f.NotifyTrayInfo("TR-20260818-trg",false);
        assert(!f.opcDeferredTrayId[1].IsEmpty() && opc.loads==0 && f.restores==0);
        grip.pauseStatus=false; grip.seq=grip.seq_save;
        f.ResumeDeferredTrayLoads();
        assert(f.opcDeferredTrayId[1].IsEmpty() && opc.loads==1 && f.restores==1);
        f.NotifyTrayInfo("TR-20260818-trg",false);
        assert(opc.loads==1 && f.restores==1);
        puts("PASS: paused IDLE barcode retained; duplicate pending load does not replace map");
    }
    {
        TMainForm f; grip.pauseStatus=true; grip.seq=Tgripper::seqPause;
        grip.seq_save=Tgripper::seqInserting;
        assert(grip.IsSortingWorkActive());
        f.NotifyTrayInfo("TR-20260818-trg",false);
        assert(opc.loads==0 && f.restores==0 && f.opcDeferredTrayId[1].IsEmpty());
        puts("PASS: paused actual INSERT retains reservation protection");
    }
    {
        TMainForm f; f.opcTrayDisplayed[1]=true;
        robot.pauseStatus=true;
        f.AdvanceOpcTrayLoad(false);
        assert(f.opcTrayAdvanceDeferred[1] && f.starts==0);
        robot.pauseStatus=false; f.equipMode=modeManual;
        f.ResumeDeferredTrayLoads(); assert(f.starts==0);
        f.equipMode=modeAuto; f.fmsAlarmTransaction=1;
        f.ResumeDeferredTrayLoads(); assert(f.starts==0);
        f.fmsAlarmTransaction=fmsAlarmNone;
        f.ResumeDeferredTrayLoads(); assert(f.opcTrayLoaded[1] && f.starts==1);
        puts("PASS: robot-only Pause, MANUAL and active FMS alarm prevent next-stage dispatch");
    }
    return 0;
}
