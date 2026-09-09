#include <string>
#include <cctype>
#include <cstdio>
#include <cassert>
#include <cstdlib>
#include "../../NGSorterErrors.h"
class Text: public std::string {
public:
 Text(){} Text(const char*s):std::string(s){} Text(const std::string&s):std::string(s){}
 Text(const wchar_t*s){while(*s)push_back((char)*s++);}
 Text Trim()const{size_t a=find_first_not_of(" \r\n\t"),b=find_last_not_of(" \r\n\t");return a==npos?Text():Text(substr(a,b-a+1));}
 Text LowerCase()const{Text t=*this;for(size_t i=0;i<t.size();++i)t[i]=(char)tolower(t[i]);return t;}
 bool IsEmpty()const{return empty();}
 int ToInt()const{return std::atoi(c_str());}
 bool operator==(const wchar_t*s)const{return compare(Text(s))==0;}
 bool operator==(const Text&s)const{return compare(s)==0;}
};
typedef Text AnsiString; typedef Text UnicodeString; typedef unsigned long DWORD; typedef void TObject;
DWORD tick=100; DWORD GetTickCount(){return tick;}
Text IntToStr(int n){char b[32];sprintf(b,"%d",n);return b;}
enum{modeManual,modeAuto,modeAutoStop,seqIdle=0};
enum{LampAuto=1};
struct Button{bool Down;Button():Down(false){}};
struct Edit{Text Text;Edit():Text("1000"){}};
struct Teach{Edit speed,accl,dccl;Edit *speedEdit,*acclSpeedEdit,*dcclSpeedEdit;Teach():speedEdit(&speed),acclSpeedEdit(&accl),dcclSpeedEdit(&dccl){}} teaching,*teachForm=&teaching;
enum TFmsAlarmTransaction{fmsAlarmNone,fmsAlarmSourceTrayLoad,fmsAlarmTargetTrayLoad,fmsAlarmCellTrackOut,fmsAlarmProcessEnd,fmsAlarmTrayUnload};
struct Step{int step;};
struct Motion{bool pauseStatus,work;int seq,seq_save;Motion():pauseStatus(false),work(false),seq(0),seq_save(0){} void req_Pause(bool b){pauseStatus=b;}bool IsSortingWorkActive(){return work;}void req_Speed(int,int,int){}} g,r,*gripper=&g,*robostar=&r;
struct Gateway{bool connected,SnapshotReceived,known;Text raw,code;Gateway():connected(true),SnapshotReceived(true),known(true),raw("false"),code("0"){} bool IsGatewayConnected(){return connected;}bool GetFmsTagJson(Text,Text&v){v=raw;return known;}bool GetFmsTagBool(Text,bool){return known&&(raw==Text("true")||raw==Text("1"));}Text GetFmsTagString(Text,Text){return code;}} gateway,*Mod_Fms=&gateway;
void (*onResponse)()=NULL;
struct Opc{
 int response[2],result[2],requests[2],cancels[2];bool localAlarms[512];
 Opc(){for(int i=0;i<2;i++)response[i]=result[i]=requests[i]=cancels[i]=0;for(int j=0;j<512;j++)localAlarms[j]=false;}
 void SetLocalAlarm(int code,bool on){assert(code>=0&&code<512);localAlarms[code]=on;}
 void PROCESS_START_CANCEL(){}
 void TRAY_LOAD_REQUEST(bool s){requests[s?0:1]++;}void TRAY_LOAD_CANCEL(bool s){cancels[s?0:1]++;}
 int TRAY_LOAD_RESPONSE_VALUE(bool s){return response[s?0:1];}
 int TRAY_LOAD_RESPONSE(bool s){if(onResponse)onResponse();return result[s?0:1];}
 Text TRAY_LOAD_VALIDATION_ERROR(bool){return "test validation details";}
} opc,*MesOpc=&opc;
enum{clSilver=0};
struct Panel{Text Caption;int Color;};
struct Timer{bool Enabled;Timer():Enabled(false){}};
struct Reader{int cancels;Reader():cancels(0){}void CancelScan(){cancels++;}};
struct Alarm{int shows;bool visible;Text status;Alarm():shows(0),visible(false){}void ShowFmsError(Text,Text,Text,int){shows++;visible=true;}void Hide(){visible=false;}void Show(){visible=true;}void BringToFront(){}void SetRetryWaiting(Text s){status=s;}void RefreshAlarmVisibility(){}} alarm,*AlarmForm_fms=&alarm;
struct Dry{bool running;Dry():running(false){}bool IsRunning(){return running;}} dry,*DryRunForm=&dry;
struct Recovery{bool IsBlocking(){return false;}void OpenRecovery(int){}} recovery,*ManualCompleteForm=&recovery;
Alarm bcrAlarm,*ErrorForm_bcr=&bcrAlarm;
struct TMainForm{
 bool fmsTroubleLatched,fmsTroublePresent,fmsTroubleStatusKnown,fmsAlarmRetryRequested,manualFmsPolling;
 bool fmsTroubleRunAcknowledged,physicalReady;int restartCalls,resumeCalls,nowLampMode;Button buttons[4],*autoBtn,*manualBtn,*playBtn,*stopBtn;
 Text fmsTroubleCode,manualTrayWaitLog;int manualTrayIndex,manualTrayPhase,manualTrayRetryPhase,manualTrayResult,equipMode;
 DWORD manualTrayTick;bool opcTrayDisplayed[2],opcTrayLoaded[2],opcTrayAdvanceDeferred[2],opcTrayLoadPending[2];Text opcDeferredTrayId[2];DWORD opcTrayLoadStartTick[2];
 bool opcProcessStartPending,opcSortingStartPending,opcProcessEndPending,opcCellTrackOutPending,opcTargetUnloadPending;
 bool manualTraySessionUsed,sourceTrayOutPending,opcProcessStartWaitResponseOff,opcProcessStartResponseOffError;
 bool opcProcessStarted,opcSortingStartWaitError,opcFmsSuspendedByManual,sourceTrayCycleAdmitted,sourceCenteringCompleted;
 bool sourceTrayResultActive,sourceTrayInTimeSet,sourceSortStartTimeSet,sourceSortEndTimeSet,sourceTrayOutTimeSet;
 int opcProcessStartResponseResult,flowResets;Panel work[2],*pwork1,*pwork2;Timer timer,*opcMesTimer;Reader reader[2],*comBcr[2];
 bool opcTrayLoadWaitResponseOff[2],opcTrayLoadResponseOffError[2],opcTrayLoadRetryRequired[2];
 int opcTrayLoadResponseResult[2];Step step[2];
 TFmsAlarmTransaction fmsAlarmTransaction;bool fmsAlarmAwaitingReset;int fmsAlarmAcceptedResult;DWORD fmsAlarmRetryStartTick;
 DWORD opcProcessStartTick,opcSortingStartTick,opcProcessEndTick,opcCellTrackOutStartTick,opcTargetUnloadTick;
 Panel source,target,*pTrayid_source,*pTrayid_target;int logs,commonErrors,displayCount[2],bypass[2],reads[2];bool productionBusy;
 TMainForm():fmsTroubleLatched(false),fmsTroublePresent(false),fmsTroubleStatusKnown(false),fmsAlarmRetryRequested(false),manualFmsPolling(false),manualTrayIndex(-1),manualTrayPhase(0),manualTrayRetryPhase(0),manualTrayResult(0),equipMode(modeManual),manualTrayTick(0),opcProcessStartPending(false),opcSortingStartPending(false),opcProcessEndPending(false),opcCellTrackOutPending(false),opcTargetUnloadPending(false),fmsAlarmTransaction(fmsAlarmNone),fmsAlarmAwaitingReset(false),fmsAlarmAcceptedResult(0),fmsAlarmRetryStartTick(0),pTrayid_source(&source),pTrayid_target(&target),logs(0),commonErrors(0),productionBusy(false){for(int i=0;i<2;i++){opcTrayDisplayed[i]=opcTrayLoaded[i]=opcTrayAdvanceDeferred[i]=opcTrayLoadPending[i]=false;opcTrayLoadWaitResponseOff[i]=opcTrayLoadResponseOffError[i]=opcTrayLoadRetryRequired[i]=false;opcTrayLoadResponseResult[i]=step[i].step=0;displayCount[i]=bypass[i]=reads[i]=0;}manualTraySessionUsed=sourceTrayOutPending=opcProcessStartWaitResponseOff=opcProcessStartResponseOffError=false;opcProcessStarted=opcSortingStartWaitError=opcFmsSuspendedByManual=sourceTrayCycleAdmitted=sourceCenteringCompleted=false;sourceTrayResultActive=sourceTrayInTimeSet=sourceSortStartTimeSet=sourceSortEndTimeSet=sourceTrayOutTimeSet=false;opcProcessStartResponseResult=flowResets=0;pwork1=&work[0];pwork2=&work[1];opcMesTimer=&timer;comBcr[0]=&reader[0];comBcr[1]=&reader[1];fmsTroubleRunAcknowledged=false;physicalReady=true;restartCalls=resumeCalls=nowLampMode=0;autoBtn=&buttons[0];manualBtn=&buttons[1];playBtn=&buttons[2];stopBtn=&buttons[3];manualBtn->Down=true;}
 void __fastcall ResetTrayLoadTransaction(bool);void InitStep(Step*s){s->step=0;}
 bool __fastcall ResetManualTrayLoadForAuto();bool IsTargetTrayExchangeActive(){return false;}
 void ResetProcessFlow(){flowResets++;}
 bool __fastcall IsFmsTroubleBlocking()const;bool __fastcall IsManualTrayLoadBusy()const;
 void __fastcall PollFmsTrouble();bool __fastcall AcknowledgeFmsTrouble();void __fastcall fmsServiceTimerTimer(TObject*);
 void __fastcall StartManualTrayLoad(bool,const Text & = "");void __fastcall AcceptManualTrayBarcode(int,const Text&);
 void __fastcall FailManualTrayLoad(const Text&);void __fastcall RetryManualTrayLoad();void __fastcall PollManualTrayLoad();
 bool IsProductionSequenceBusy(){return productionBusy||IsManualTrayLoadBusy();}
 void memoMainLineAdd(Text){logs++;}void ShowCommonError(Text,Text){commonErrors++;}
 void PrepareActiveTrayInfoFile(bool,Text){}int RestoreTargetTrayInfo(Text,bool){return 0;}
 void SetTrayLoadBypassDisplay(bool s,int n){bypass[s?0:1]=n==2;}
 void DisplayOpcTrayLoad(bool s){int i=s?0:1;displayCount[i]++;opcTrayDisplayed[i]=true;}
 void ReadSourceTrayBarcode(){reads[0]++;}void ReadTargetTrayBarcode(){reads[1]++;}
 void __fastcall autoBtnClick(TObject*);void __fastcall playBtnClick(TObject*);
 bool CheckServoAutoReady(bool){return physicalReady;}void EnableButton_auto(bool){}
 void ResumeAutomaticFmsSequence(){resumeCalls++;}
 // Physical Restart is a dependency here; the runner checks its real interlock wiring.
 void pause_startBtnClick(TObject*){restartCalls++;if(physicalReady&&!IsFmsTroubleBlocking()){r.req_Pause(false);g.req_Pause(false);}}
};
void __fastcall TMainForm::ResetTrayLoadTransaction(bool sourceTray)
{
	int i = sourceTray ? 0 : 1;
	if(MesOpc != NULL){
		MesOpc->SetLocalAlarm(sourceTray ? NGSorterErrors::FmsSourceLoad : NGSorterErrors::FmsTargetLoad,false);
		MesOpc->SetLocalAlarm(sourceTray ? NGSorterErrors::SourceBarcode : NGSorterErrors::TargetBarcode,false);
	}
	if(MesOpc != NULL) MesOpc->TRAY_LOAD_CANCEL(sourceTray);
	opcTrayLoadPending[i] = false;
	opcTrayLoadWaitResponseOff[i] = false;
	opcTrayLoadResponseOffError[i] = false;
	opcTrayLoadRetryRequired[i] = false;
	opcTrayLoadResponseResult[i] = 0;
	opcTrayLoadStartTick[i] = 0;
	opcTrayDisplayed[i] = false;
	opcTrayLoaded[i] = false;
	opcTrayAdvanceDeferred[i] = false;
	opcDeferredTrayId[i] = "";
	// A discarded load must never be revived by popup Retry or Main Restart.
	TFmsAlarmTransaction loadAlarm = sourceTray ? fmsAlarmSourceTrayLoad : fmsAlarmTargetTrayLoad;
	if(fmsAlarmTransaction == loadAlarm){
		fmsAlarmTransaction = fmsAlarmNone;
		fmsAlarmRetryRequested = false;
		fmsAlarmAwaitingReset = false;
		fmsAlarmAcceptedResult = 0;
		fmsAlarmRetryStartTick = 0;
		// MERGE 2026-09-08: the shared popup may now display independent FMS Trouble.
		// Discarding an AUTO load must not dismiss that latched equipment-wide alarm.
		if(AlarmForm_fms != NULL && !fmsTroubleLatched) AlarmForm_fms->Hide();
	}
}
static const UnicodeString FMS_TROUBLE_STATUS = L"NGS.F1NGS01.FmsStatus.Trouble.Status";
static const UnicodeString FMS_TROUBLE_CODE = L"NGS.F1NGS01.FmsStatus.Trouble.ErrorNo";

bool __fastcall TMainForm::IsFmsTroubleBlocking() const
{
	// FMS START: one operator acknowledgement permits the SAME incident to stay
	// ON without stopping again. A new code is blocked even before the next poll.
	bool active = Mod_Fms != NULL && Mod_Fms->GetFmsTagBool(FMS_TROUBLE_STATUS, false);
	if(fmsTroubleRunAcknowledged){
		if(!active || AnsiString(Mod_Fms->GetFmsTagString(FMS_TROUBLE_CODE, L"UNKNOWN")) == fmsTroubleCode)
			return false;
	}
	return fmsTroubleLatched || active;
}

bool __fastcall TMainForm::IsManualTrayLoadBusy() const
{
	return manualTrayPhase != 0;
}

bool __fastcall TMainForm::ResetManualTrayLoadForAuto()
{
	// ========================================================================
	//* MANUAL -> AUTO 2026-09-09: manual response/reset waits never block AUTO.
	// A completed manual display is NOT a completed automatic TrayLoad either.
	// Cancel requests and restart admission from STEP 01, without moving axes,
	// deleting saved cells, clearing safety latches or dropping completed reports.
	// ========================================================================
	if(!manualTraySessionUsed && !IsManualTrayLoadBusy()) return true;
	// Mode selection may discard manual requests even during FMS Trouble.
	// Keep the independent alarm/latch; START, not AUTO selection, acknowledges it.
	if(manualFmsPolling){
		ShowCommonError("AUTO initialization waiting", "Close the manual tray data selection dialog, then select AUTO again.");
		return false;
	}
	if(opcCellTrackOutPending || opcProcessEndPending || opcTargetUnloadPending || sourceTrayOutPending ||
		fmsAlarmTransaction == fmsAlarmCellTrackOut || fmsAlarmTransaction == fmsAlarmProcessEnd ||
		fmsAlarmTransaction == fmsAlarmTrayUnload || IsTargetTrayExchangeActive() ||
		(gripper != NULL && gripper->IsSortingWorkActive()) ||
		(ManualCompleteForm != NULL && ManualCompleteForm->IsBlocking())){
		ShowCommonError("AUTO initialization blocked", "An actual automatic cell operation/report is unfinished. Recover it before starting a new cycle; manual TrayLoad response is not the blocker.");
		return false;
	}
	int oldPhase = manualTrayPhase;
	// Retire the owner first, so a late barcode/timer cannot recreate the request.
	manualTrayPhase = manualTrayRetryPhase = manualTrayResult = 0;
	manualTrayIndex = -1;
	manualTrayTick = 0;
	manualTrayWaitLog = "";
	for(int i = 0; i < 2; ++i){
		if(comBcr[i] != NULL) comBcr[i]->CancelScan();
		ResetTrayLoadTransaction(i == 0);
		InitStep(&step[i]);
		SetTrayLoadBypassDisplay(i == 0, 0);
	}
	if(opcMesTimer != NULL) opcMesTimer->Enabled = false;
	if(MesOpc != NULL){
		MesOpc->PROCESS_START_CANCEL();
		MesOpc->SetLocalAlarm(NGSorterErrors::ManualSourceLoad, false);
		MesOpc->SetLocalAlarm(NGSorterErrors::ManualTargetLoad, false);
		MesOpc->SetLocalAlarm(NGSorterErrors::FmsProcessStart, false);
	}
	opcProcessStartPending = opcProcessStartWaitResponseOff = opcProcessStartResponseOffError = false;
	opcProcessStartResponseResult = 0;
	opcProcessStartTick = 0;
	opcProcessStarted = opcSortingStartPending = opcSortingStartWaitError = false;
	opcSortingStartTick = 0;
	opcFmsSuspendedByManual = false;
	fmsAlarmTransaction = fmsAlarmNone;
	fmsAlarmRetryRequested = fmsAlarmAwaitingReset = false;
	fmsAlarmAcceptedResult = 0;
	fmsAlarmRetryStartTick = 0;
	if(AlarmForm_fms != NULL && !fmsTroubleLatched) AlarmForm_fms->Hide();
	if(ErrorForm_bcr != NULL) ErrorForm_bcr->Hide();
	sourceTrayCycleAdmitted = sourceCenteringCompleted = false;
	pwork1->Color = clSilver;
	pwork2->Color = clSilver;
	// Manual display timing must not become the next AUTO tray's timing.
	sourceTrayResultActive = false;
	sourceTrayInTimeSet = sourceSortStartTimeSet = sourceSortEndTimeSet = sourceTrayOutTimeSet = false;
	ResetProcessFlow();
	manualTraySessionUsed = false;
	memoMainLineAdd("[MANUAL -> AUTO] Manual TrayLoad cancelled / phase=" + IntToStr(oldPhase) +
		" / both requests OFF / manual waits, retry and load approval reset / START begins STEP 01 with live PLC inputs and a NEW barcode. Safety Pause retained; use Restart if paused.");
	return true;
}

void __fastcall TMainForm::PollFmsTrouble()
{
	UnicodeString raw;
	bool parsed = Mod_Fms != NULL && Mod_Fms->GetFmsTagJson(FMS_TROUBLE_STATUS, raw);
	if(parsed){
		raw = raw.Trim().LowerCase();
		parsed = raw == L"true" || raw == L"false" || raw == L"1" || raw == L"0";
	}
	fmsTroubleStatusKnown = parsed && Mod_Fms->IsGatewayConnected() && Mod_Fms->SnapshotReceived;
	// An observed ON is always enough to stop, even if the connection just fell.
	// An OFF may clear the input only from a connected, valid snapshot.
	if(fmsTroubleStatusKnown || (parsed && (raw == L"true" || raw == L"1"))){
		bool active = raw == L"true" || raw == L"1";
		AnsiString code = Mod_Fms->GetFmsTagString(FMS_TROUBLE_CODE, L"UNKNOWN");
		bool changed = active && (!fmsTroublePresent || !fmsTroubleLatched || code != fmsTroubleCode);
		bool cleared = !active && fmsTroublePresent;
		fmsTroublePresent = active;
		if(MesOpc != NULL) MesOpc->SetLocalAlarm(NGSorterErrors::FmsTrouble,active);
		if(active){
			// Latch BEFORE UI callbacks: closing a popup or clearing Status does not resume.
			fmsTroubleLatched = true;
			fmsTroubleCode = code;
		}
		if(changed){
			fmsTroubleRunAcknowledged = false;
			fmsAlarmRetryRequested = false;
			if(manualTrayPhase > 0 && manualTrayPhase != 4)
				FailManualTrayLoad("New FMS Trouble interrupted manual TrayLoad. START/Retry/Restart can acknowledge this alarm and continue.");
			if(gripper != NULL) gripper->req_Pause(true);
			if(robostar != NULL) robostar->req_Pause(true);
			memoMainLineAdd("[FMS TROUBLE] Status=ON / ErrorNo=" + code + " / PAUSE (all modes)");
			if(AlarmForm_fms != NULL)
				AlarmForm_fms->ShowFmsError("FMS Trouble", "FmsStatus.Trouble.Status=true / ErrorNo=" + code +
					"\r\nPaused for a NEW FMS alarm. AUTO selection is allowed. START / Retry / Main Restart may continue while this alarm remains ON; physical interlocks and response validation still apply. Close does not resume.",
					AnsiString(FMS_TROUBLE_STATUS), 1);
			if(AlarmForm_fms != NULL)
				AlarmForm_fms->SetRetryWaiting("FMS Trouble ON: START/Retry/Restart accepts this incident; safety interlocks remain active.");
		}
		if(cleared){
			if(fmsTroubleRunAcknowledged){
				fmsTroubleLatched = false;
				fmsTroubleRunAcknowledged = false;
				memoMainLineAdd("[FMS TROUBLE] Acknowledged alarm is now OFF / equipment alarm code cleared / no motion change");
			}else{
				memoMainLineAdd("[FMS TROUBLE] Status=OFF / pause retained / WAIT operator START, Retry or Restart");
				if(AlarmForm_fms != NULL)
					AlarmForm_fms->SetRetryWaiting("FMS Trouble cleared. PAUSED until operator START / Retry / Main Restart.");
			}
		}
	}
	// Only an unacknowledged incident holds Pause. Never clear a reported alarm
	// merely because its tag disappeared or the operator elected to continue.
	if(IsFmsTroubleBlocking()){
		if(gripper != NULL) gripper->req_Pause(true);
		if(robostar != NULL) robostar->req_Pause(true);
		if(AlarmForm_fms != NULL) AlarmForm_fms->RefreshAlarmVisibility();
	}
}

bool __fastcall TMainForm::AcknowledgeFmsTrouble()
{
	PollFmsTrouble();
	if(!IsFmsTroubleBlocking()) return true;
	// FMS START 2026-09-09: this is an explicit operator continuation, not a fake
	// FMS reset. Keep the alarm code published until FMS actually sends Status=OFF.
	fmsTroubleRunAcknowledged = true;
	if(fmsTroubleStatusKnown && !fmsTroublePresent){
		fmsTroubleLatched = false;
		fmsTroubleRunAcknowledged = false;
	}
	// Restart wait budgets without changing the accepted phase/result or sending
	// new requests. Time spent in an independent Trouble must not cause an instant
	// production response timeout on the first resumed polling tick.
	DWORD nowTick = GetTickCount();
	for(int i = 0; i < 2; ++i)
		if(opcTrayLoadPending[i]) opcTrayLoadStartTick[i] = nowTick;
	if(opcProcessStartPending) opcProcessStartTick = nowTick;
	if(opcSortingStartPending) opcSortingStartTick = nowTick;
	if(opcProcessEndPending) opcProcessEndTick = nowTick;
	if(opcCellTrackOutPending) opcCellTrackOutStartTick = nowTick;
	if(opcTargetUnloadPending) opcTargetUnloadTick = nowTick;
	if(AlarmForm_fms != NULL) AlarmForm_fms->Hide();
	memoMainLineAdd("[FMS TROUBLE] Operator START/Restart accepted current alarm / Status=" +
		AnsiString(!fmsTroubleStatusKnown ? "UNKNOWN" : (fmsTroublePresent ? "ON" : "OFF")) +
		" / ErrorNo=" + fmsTroubleCode + " / same alarm will not re-pause; physical interlocks and FMS response checks retained.");
	return true;
}

void __fastcall TMainForm::fmsServiceTimerTimer(TObject *Sender)
{
	// Always monitor Trouble, including while a data-validation dialog pumps messages.
	PollFmsTrouble();
	if(IsFmsTroubleBlocking() || manualFmsPolling) return;
	manualFmsPolling = true;
	try { PollManualTrayLoad(); }
	__finally { manualFmsPolling = false; }
}

void __fastcall TMainForm::StartManualTrayLoad(bool sourceTray, const AnsiString &enteredId)
{
	// ========================================================================
	//* MANUAL SCAN 2026-09-09: explicit data request, NOT production admission.
	// Do not require PLC AUTO/In/Centering, servo state, or an idle AUTO step.
	// No motion/centering/tray-out is commanded and no physical interlock changes.
	// FMS Trouble and one-request ownership still apply to this data service.
	// ========================================================================
	if(equipMode != modeManual){
		ShowCommonError("Manual TrayLoad blocked", "Select MANUAL mode before Scan.");
		return;
	}
	if(IsFmsTroubleBlocking()){
		ShowCommonError("Manual TrayLoad blocked", "A new FMS Trouble is unacknowledged. Press Retry/Restart to accept this alarm; FMS Status may remain ON.");
		return;
	}
	if(MesOpc == NULL || Mod_Fms == NULL || !Mod_Fms->IsGatewayConnected()){
		ShowCommonError("Manual TrayLoad blocked", "FMS Gateway is not connected. No request was sent.");
		return;
	}
	if(IsManualTrayLoadBusy()){
		if(manualTrayIndex == (sourceTray ? 0 : 1)){
			if(manualTrayPhase == 4) RetryManualTrayLoad();
			else memoMainLineAdd("[MANUAL FMS] Scan ignored: existing request retained / Location" +
				IntToStr(manualTrayIndex + 1) + " / WAIT " + AnsiString(manualTrayPhase == 1 ?
				"barcode" : (manualTrayPhase == 3 ? "Response=0 (RESET)" : "Response=1 or 2 / valid tray data")));
		}else ShowCommonError("Manual TrayLoad busy", "Complete or retry Location" +
			IntToStr(manualTrayIndex + 1) + " manual TrayLoad before scanning the other tray.");
		return;
	}
	// Retire this Location's old AUTO load owner BEFORE arming manual Scan.
	// Otherwise senTimer's AUTO presence check can turn the manual request OFF.
	// Other completed-work reports (CellTrackOut/ProcessEnd/Unload) are preserved.
	ResetTrayLoadTransaction(sourceTray);
	int index = sourceTray ? 0 : 1;
	if(step[index].step == 1) InitStep(&step[index]); // AUTO must perform its own new load.
	manualTraySessionUsed = true;
	manualTrayIndex = sourceTray ? 0 : 1;
	manualTrayPhase = 1;
	manualTrayRetryPhase = manualTrayResult = 0;
	manualTrayTick = GetTickCount();
	manualTrayWaitLog = "";
	opcTrayDisplayed[manualTrayIndex] = opcTrayLoaded[manualTrayIndex] = false;
	opcTrayAdvanceDeferred[manualTrayIndex] = false;
	opcDeferredTrayId[manualTrayIndex] = "";
	memoMainLineAdd(AnsiString("[MANUAL FMS] ") + (sourceTray ? "Location1" : "Location2") +
		" / WAIT barcode (reader or configured FAT ID) / data-only; PLC and motion admission not required");
	if(!enteredId.Trim().IsEmpty()) AcceptManualTrayBarcode(manualTrayIndex, enteredId);
	else if(sourceTray) ReadSourceTrayBarcode();
	else ReadTargetTrayBarcode();
}

void __fastcall TMainForm::AcceptManualTrayBarcode(int index, const AnsiString &trayId)
{
	if(manualTrayPhase != 1 || index != manualTrayIndex) return;
	if(equipMode != modeManual || IsFmsTroubleBlocking()){
		FailManualTrayLoad("Mode changed or FMS Trouble is active. Barcode was not reported.");
		return;
	}
	AnsiString id = trayId.Trim();
	if(id.IsEmpty()) { FailManualTrayLoad("Barcode is empty. Check the reader/configured ID."); return; }
	if(MesOpc == NULL || Mod_Fms == NULL || !Mod_Fms->IsGatewayConnected()){
		FailManualTrayLoad("Gateway disconnected before TrayLoad request."); return;
	}
	bool sourceTray = index == 0;
	PrepareActiveTrayInfoFile(sourceTray, id);
	(sourceTray ? pTrayid_source : pTrayid_target)->Caption = id;
	if(!sourceTray && RestoreTargetTrayInfo(id, false) < 0){
		FailManualTrayLoad("Target local information could not be prepared."); return;
	}
	manualTrayPhase = 2; // Set ownership before sending/any callbacks.
	manualTrayTick = GetTickCount();
	manualTrayWaitLog = "";
	SetTrayLoadBypassDisplay(sourceTray, 0);
	MesOpc->TRAY_LOAD_REQUEST(sourceTray); // TrayId + TrayExist=true + TrayLoad=true.
	memoMainLineAdd("[MANUAL FMS] Location" + IntToStr(index + 1) + " TrayId=" + id +
		" / TrayExist=ON / TrayLoad=ON / WAIT TrayLoadResponse=1 or 2 and valid data");
}

void __fastcall TMainForm::FailManualTrayLoad(const AnsiString &detail)
{
	if(manualTrayPhase == 0 || manualTrayPhase == 4) return;
	manualTrayRetryPhase = manualTrayPhase;
	manualTrayPhase = 4; // Latch failure before showing modeless UI.
	if(MesOpc != NULL) MesOpc->SetLocalAlarm(manualTrayIndex==0 ?
		NGSorterErrors::ManualSourceLoad : NGSorterErrors::ManualTargetLoad,true);
	if(MesOpc != NULL && manualTrayRetryPhase != 1)
		MesOpc->TRAY_LOAD_CANCEL(manualTrayIndex == 0);
	if(gripper != NULL) gripper->req_Pause(true);
	if(robostar != NULL) robostar->req_Pause(true);
	memoMainLineAdd("[MANUAL FMS] ERROR Location" + IntToStr(manualTrayIndex + 1) + " / " + detail);
	if(!IsFmsTroubleBlocking() && AlarmForm_fms != NULL)
		AlarmForm_fms->ShowFmsError("Manual TrayLoad error", detail +
			"\r\nCorrect the cause, then press Retry or Main Restart. Motion remains paused.",
			"Location" + IntToStr(manualTrayIndex + 1) + ".TrayLoad", MesOpc != NULL ?
			MesOpc->TRAY_LOAD_RESPONSE_VALUE(manualTrayIndex == 0) : -1);
}

void __fastcall TMainForm::RetryManualTrayLoad()
{
	if(manualTrayPhase != 4 || equipMode != modeManual || IsFmsTroubleBlocking()) return;
	if(MesOpc == NULL || Mod_Fms == NULL || !Mod_Fms->IsGatewayConnected()){
		if(AlarmForm_fms != NULL) AlarmForm_fms->SetRetryWaiting("WAIT connected FMS Gateway before manual Retry.");
		return;
	}
	if(AlarmForm_fms != NULL) AlarmForm_fms->Hide();
	manualTrayPhase = manualTrayRetryPhase;
	manualTrayTick = GetTickCount();
	manualTrayWaitLog = "";
	if(manualTrayPhase == 1){
		if(manualTrayIndex == 0) ReadSourceTrayBarcode(); else ReadTargetTrayBarcode();
	}else if(manualTrayPhase == 2){
		MesOpc->TRAY_LOAD_REQUEST(manualTrayIndex == 0);
	}else MesOpc->TRAY_LOAD_CANCEL(manualTrayIndex == 0);
	memoMainLineAdd("[MANUAL FMS] Operator Retry / WAIT " + AnsiString(manualTrayPhase == 3 ?
		"TrayLoadResponse=0 (RESET)" : "barcode/TrayLoadResponse=1 or 2 (RESULT)"));
}

void __fastcall TMainForm::PollManualTrayLoad()
{
	if(manualTrayPhase == 0) return;
	if(manualTrayPhase == 4){
		if(AlarmForm_fms != NULL) AlarmForm_fms->RefreshAlarmVisibility();
		return;
	}
	if(equipMode != modeManual){ FailManualTrayLoad("MANUAL mode was lost."); return; }
	if(MesOpc == NULL || Mod_Fms == NULL || !Mod_Fms->IsGatewayConnected()){
		FailManualTrayLoad("FMS Gateway disconnected."); return;
	}
	if(manualTrayPhase == 1){
		if((DWORD)(GetTickCount() - manualTrayTick) >= 10000)
			FailManualTrayLoad("Barcode timeout: no reader/configured ID within 10 seconds.");
		return;
	}
	bool sourceTray = manualTrayIndex == 0;
	int response = MesOpc->TRAY_LOAD_RESPONSE_VALUE(sourceTray);
	AnsiString wait = "[MANUAL FMS] Location" + IntToStr(manualTrayIndex + 1) +
		(manualTrayPhase == 3 ? " Request=OFF / WAIT Response=0 (RESET)" :
		" Request=ON / WAIT Response=1 or 2 (RESULT) and validated data") + " / CURRENT=" + IntToStr(response);
	if(wait != manualTrayWaitLog){ manualTrayWaitLog = wait; memoMainLineAdd(wait); }
	if(manualTrayPhase == 2){
		int result = MesOpc->TRAY_LOAD_RESPONSE(sourceTray);
		// A modal FMS/LOCAL choice can pump the independent Trouble monitor.
		if(manualTrayPhase != 2 || IsFmsTroubleBlocking()) return;
		if(result == 1 || result == 2){
			if(result == 1){
				DisplayOpcTrayLoad(sourceTray);
				if(!opcTrayDisplayed[manualTrayIndex]){
					FailManualTrayLoad("Tray data display did not complete."); return;
				}
			}
			manualTrayResult = result;
			SetTrayLoadBypassDisplay(sourceTray, result);
			MesOpc->TRAY_LOAD_CANCEL(sourceTray);
			manualTrayPhase = 3;
			manualTrayTick = GetTickCount();
			manualTrayWaitLog = "";
			memoMainLineAdd("[MANUAL FMS] Response=" + IntToStr(result) +
				" accepted / Request=OFF / WAIT Response=0 (RESET); no AUTO advance or tray discharge");
			return;
		}
		if(result < 0){ FailManualTrayLoad("Invalid TrayLoadResponse: " + IntToStr(response)); return; }
	}else if(response == 0){
		// Even Cycle Test must complete the explicitly requested manual reset handshake.
		opcTrayLoaded[manualTrayIndex] = false; // Never authorize production from manual data.
		opcTrayAdvanceDeferred[manualTrayIndex] = false;
		memoMainLineAdd("[MANUAL FMS] Location" + IntToStr(manualTrayIndex + 1) +
			" COMPLETE / Response=0 / data retained / no ProcessStart, centering, tray-out or motion");
		manualTrayPhase = 0;
		if(MesOpc != NULL) MesOpc->SetLocalAlarm(manualTrayIndex==0 ?
			NGSorterErrors::ManualSourceLoad : NGSorterErrors::ManualTargetLoad,false);
		manualTrayIndex = -1;
		return;
	}
	if((DWORD)(GetTickCount() - manualTrayTick) >= 10000)
		FailManualTrayLoad(wait + " / 10-second timeout / " + MesOpc->TRAY_LOAD_VALIDATION_ERROR(sourceTray));
}

// 전지 정보 표시

void __fastcall TMainForm::autoBtnClick(TObject *Sender)
{
	//* FMS START 2026-09-09: FMS Trouble does not prohibit selecting AUTO.
	// START/Restart acknowledges the current incident separately; never fake its OFF tag.
	// AUTO entry always validates the real servo/CC-Link/gripper interlocks.
	// cbCycle bypasses FMS response freshness/reset handshakes for unattended FAT
	// demonstration. Motion/barcode options remain independent, and no setting
	// bypasses the physical AUTO readiness interlocks.
	if(!CheckServoAutoReady(true))
	{
		autoBtn->Down = false;
		return;
	}
	//* MANUAL -> AUTO: discard only manual data-service state, then start at STEP 01.
	// Selecting AUTO is not START/Restart; never release a physical safety Pause here.
	if(equipMode == modeManual && !ResetManualTrayLoadForAuto()){
		autoBtn->Down = false;
		return;
	}

	int servo_speed = teachForm->speedEdit->Text.ToInt();
	int servo_accl_speed = teachForm->acclSpeedEdit->Text.ToInt();
	int servo_dccl_speed = teachForm->dcclSpeedEdit->Text.ToInt();
	robostar->req_Speed(servo_speed, servo_accl_speed, servo_dccl_speed);
	if(manualBtn->Down == true){
		equipMode = modeAutoStop;
		autoBtn->Down = true;
		manualBtn->Down = false;
		playBtn->Down = false;
		stopBtn->Down = true;
		EnableButton_auto(true);
	}else{
		autoBtn->Down = true;
	}
}
void __fastcall TMainForm::playBtnClick(TObject *Sender)
{
	if(IsManualTrayLoadBusy()){
		ShowCommonError("START blocked", "Select AUTO to initialize the manual TrayLoad session first.");
		return;
	}
	if(ManualCompleteForm != NULL && ManualCompleteForm->IsBlocking()){
		ManualCompleteForm->OpenRecovery(0);
		return;
	}
	// Servo OPEN/ON/HOME is checked only when entering AUTO mode.
	// START resumes the paused sequence without re-running the AUTO interlock.
	//* FMS START: explicit operator command may continue while the same FMS alarm is ON.
	// Remember its Pause before acknowledgement, then use the normal physical Restart path.
	PollFmsTrouble();
	bool resumeFmsPause = fmsTroubleLatched || IsFmsTroubleBlocking() || fmsAlarmTransaction != fmsAlarmNone;
	if(!AcknowledgeFmsTrouble()) return;
	equipMode = modeAuto;
	if(resumeFmsPause){
		pause_startBtnClick(Sender);
		if(gripper->pauseStatus || robostar->pauseStatus){
			equipMode = modeAutoStop;
			playBtn->Down = false;
			stopBtn->Down = true;
			memoMainLineAdd("[FMS START] Current FMS alarm accepted, but physical/recovery interlock retained Pause. Correct it and press Restart.");
			return;
		}
	}
	nowLampMode = LampAuto;
	ResumeAutomaticFmsSequence();
	playBtn->Down = true;
	stopBtn->Down = false;
   
}
TMainForm *interruptMain=NULL;
void troubleDuringResponse(){gateway.raw="true";gateway.code="77";interruptMain->PollFmsTrouble();}
void reset(){gateway=Gateway();opc=Opc();g=Motion();r=Motion();alarm=Alarm();dry=Dry();tick=100;onResponse=NULL;}
int main(){
 for(int mode=0;mode<7;mode++){
  reset();TMainForm m;m.equipMode=mode;m.PollFmsTrouble();assert(!m.IsFmsTroubleBlocking());
  gateway.raw="true";gateway.code="123";m.PollFmsTrouble();assert(g.pauseStatus&&r.pauseStatus&&m.fmsTroubleLatched&&alarm.shows==1);
  int logs=m.logs;for(int j=0;j<10;j++)m.PollFmsTrouble();assert(alarm.shows==1&&m.logs==logs);
  m.opcProcessStartPending=true;tick=90123;assert(m.AcknowledgeFmsTrouble());assert(!m.IsFmsTroubleBlocking()&&m.fmsTroubleLatched&&m.fmsTroubleRunAcknowledged&&g.pauseStatus&&r.pauseStatus&&m.opcProcessStartTick==tick);
  assert(opc.localAlarms[NGSorterErrors::FmsTrouble]);g.pauseStatus=r.pauseStatus=false;
  tick+=50000;m.fmsAlarmRetryRequested=true;for(int j=0;j<10;j++)m.PollFmsTrouble();assert(!g.pauseStatus&&!r.pauseStatus&&alarm.shows==1&&m.opcProcessStartTick==90123&&m.fmsAlarmRetryRequested);
  gateway.code="124";assert(m.IsFmsTroubleBlocking());m.PollFmsTrouble();assert(g.pauseStatus&&r.pauseStatus&&!m.fmsTroubleRunAcknowledged&&alarm.shows==2);
  assert(m.AcknowledgeFmsTrouble());gateway.raw="false";m.PollFmsTrouble();assert(!m.IsFmsTroubleBlocking()&&!m.fmsTroubleLatched&&!opc.localAlarms[NGSorterErrors::FmsTrouble]);
  gateway.raw="true";m.PollFmsTrouble();assert(m.IsFmsTroubleBlocking()&&alarm.shows==3);
 }
 puts("PASS: new Trouble pauses all modes; operator accepts ON without faking OFF; repeated ON never re-pauses; new code/ON re-arms");
 reset();{TMainForm m;gateway.raw="true";gateway.connected=false;m.PollFmsTrouble();assert(m.fmsTroubleLatched&&g.pauseStatus&&r.pauseStatus);gateway.raw="false";assert(m.AcknowledgeFmsTrouble()&&m.fmsTroubleLatched&&!m.IsFmsTroubleBlocking()&&opc.localAlarms[NGSorterErrors::FmsTrouble]);}
 for(int i=0;i<2;i++){
  reset();TMainForm m;m.StartManualTrayLoad(i==0);assert(m.manualTrayPhase==1&&m.reads[i]==1);
  m.AcceptManualTrayBarcode(i,"TRAY-ID");assert(m.manualTrayPhase==2&&opc.requests[i]==1);
  m.PollManualTrayLoad();int logs=m.logs;m.PollManualTrayLoad();assert(m.logs==logs);
  opc.response[i]=1;opc.result[i]=0;m.PollManualTrayLoad();assert(m.manualTrayPhase==2&&m.displayCount[i]==0);
  opc.result[i]=1;m.PollManualTrayLoad();assert(m.manualTrayPhase==3&&m.displayCount[i]==1&&opc.cancels[i]==2);
  m.PollManualTrayLoad();assert(m.manualTrayPhase==3);opc.response[i]=0;m.PollManualTrayLoad();assert(!m.IsManualTrayLoadBusy()&&!m.opcTrayLoaded[i]&&m.opcTrayDisplayed[i]);
  assert(opc.requests[1-i]==0&&m.reads[1-i]==0);
 }
 puts("PASS: both locations scan/request/data/display/OFF/reset independently, unchanged waits logged once, no AUTO authorization");
 reset();{TMainForm m;m.StartManualTrayLoad(true,"BYPASS");opc.response[0]=opc.result[0]=2;m.PollManualTrayLoad();assert(m.manualTrayPhase==3&&m.bypass[0]==1&&m.displayCount[0]==0);opc.response[0]=0;m.PollManualTrayLoad();assert(m.manualTrayPhase==0);}
 reset();{TMainForm m;m.StartManualTrayLoad(true,"TIMEOUT");tick+=10000;m.PollManualTrayLoad();assert(m.manualTrayPhase==4&&m.manualTrayRetryPhase==2&&g.pauseStatus&&r.pauseStatus);m.RetryManualTrayLoad();assert(m.manualTrayPhase==2&&opc.requests[0]==2&&g.pauseStatus);opc.response[0]=opc.result[0]=1;m.PollManualTrayLoad();tick+=10000;m.PollManualTrayLoad();assert(m.manualTrayPhase==4&&m.manualTrayRetryPhase==3);m.RetryManualTrayLoad();assert(m.manualTrayPhase==3&&opc.requests[0]==2);opc.response[0]=0;m.PollManualTrayLoad();assert(m.manualTrayPhase==0);}
 puts("PASS: response=2 display-only, ON timeout retry, reset timeout retry without resending accepted request");
 reset();{TMainForm m;m.StartManualTrayLoad(false,"TARGET");interruptMain=&m;onResponse=troubleDuringResponse;opc.response[1]=opc.result[1]=1;m.PollManualTrayLoad();assert(m.manualTrayPhase==4&&m.fmsTroubleLatched&&m.displayCount[1]==0&&opc.cancels[1]>0);onResponse=NULL;gateway.raw="false";assert(m.AcknowledgeFmsTrouble());m.RetryManualTrayLoad();assert(m.manualTrayPhase==2&&r.pauseStatus);}
 reset();{TMainForm m;m.equipMode=modeAuto;m.StartManualTrayLoad(true);assert(m.manualTrayPhase==0);m.equipMode=modeManual;m.StartManualTrayLoad(true);tick+=10000;m.PollManualTrayLoad();assert(m.manualTrayPhase==4&&m.manualTrayRetryPhase==1);m.StartManualTrayLoad(true);assert(m.manualTrayPhase==1&&m.reads[0]==2);}
 puts("PASS: Trouble during data callback halts manual work; nonmanual rejected; Scan retries barcode failure");
 for(int i=0;i<2;i++){
  reset();TMainForm m;m.productionBusy=true;g.work=true;r.seq=9;r.seq_save=9;r.pauseStatus=true;dry.running=true;
  m.opcTrayLoadPending[i]=m.opcTrayLoadRetryRequired[i]=m.opcTrayAdvanceDeferred[i]=true;
  m.opcDeferredTrayId[i]="OLD";m.step[i].step=1;m.fmsAlarmTransaction=i==0?fmsAlarmSourceTrayLoad:fmsAlarmTargetTrayLoad;
  m.StartManualTrayLoad(i==0,"MANUAL");assert(m.manualTrayPhase==2&&m.commonErrors==0&&opc.requests[i]==1);
  assert(!m.opcTrayLoadPending[i]&&!m.opcTrayLoadRetryRequired[i]&&!m.opcTrayAdvanceDeferred[i]&&m.opcDeferredTrayId[i].IsEmpty());
  assert(m.fmsAlarmTransaction==fmsAlarmNone&&m.step[i].step==0&&r.pauseStatus);
  m.StartManualTrayLoad(i==0);assert(opc.requests[i]==1);m.StartManualTrayLoad(i!=0);assert(m.commonErrors==1&&opc.requests[1-i]==0);
  tick+=10000;m.PollManualTrayLoad();assert(m.manualTrayPhase==4);m.StartManualTrayLoad(i==0);assert(m.manualTrayPhase==2&&opc.requests[i]==2);
  opc.response[i]=opc.result[i]=1;m.PollManualTrayLoad();m.StartManualTrayLoad(i==0);assert(m.manualTrayPhase==3&&opc.requests[i]==2);
 }
 puts("PASS: manual Scan independent of production/motion state, retires only selected AUTO load; duplicate ownership and Scan retry");
 reset();{TMainForm m;m.fmsAlarmTransaction=fmsAlarmCellTrackOut;m.opcCellTrackOutPending=m.opcProcessEndPending=m.opcTargetUnloadPending=true;m.opcTrayLoadPending[1]=true;
  m.StartManualTrayLoad(true,"DATA");assert(m.manualTrayPhase==2&&m.fmsAlarmTransaction==fmsAlarmCellTrackOut&&m.opcCellTrackOutPending&&m.opcProcessEndPending&&m.opcTargetUnloadPending&&m.opcTrayLoadPending[1]);}
 reset();{TMainForm m;gateway.connected=false;m.StartManualTrayLoad(true,"OFFLINE");assert(m.manualTrayPhase==0&&opc.requests[0]==0);gateway.connected=true;gateway.raw="true";m.StartManualTrayLoad(true,"TROUBLE");assert(m.manualTrayPhase==0&&opc.requests[0]==0);}
 puts("PASS: completed-work reports preserved; disconnected Gateway and independent Trouble still blocked");
 for(int i=0;i<2;i++){
  reset();TMainForm m;int code=i==0?NGSorterErrors::ManualSourceLoad:NGSorterErrors::ManualTargetLoad;
  m.StartManualTrayLoad(i==0,"ALARM");tick+=10000;m.PollManualTrayLoad();assert(opc.localAlarms[code]);
  m.StartManualTrayLoad(i==0);assert(opc.localAlarms[code]);opc.response[i]=opc.result[i]=1;m.PollManualTrayLoad();assert(opc.localAlarms[code]);
  opc.response[i]=0;m.PollManualTrayLoad();assert(!opc.localAlarms[code]);
 }
 puts("PASS: latest equipment alarm reporting retained until full manual handshake/reset completes");
 for(int phase=0;phase<=4;phase++){
  reset();TMainForm m;m.manualTraySessionUsed=true;m.manualTrayPhase=phase;m.manualTrayIndex=1;
  m.manualTrayRetryPhase=2;m.manualTrayResult=1;m.manualTrayTick=777;m.manualTrayWaitLog="OLD";
  m.opcTrayDisplayed[0]=m.opcTrayLoaded[0]=m.opcTrayLoadPending[1]=true;m.step[0].step=3;m.step[1].step=1;
  m.opcProcessStartPending=m.opcProcessStarted=m.opcSortingStartPending=true;m.opcFmsSuspendedByManual=true;
  m.sourceTrayCycleAdmitted=m.sourceCenteringCompleted=m.sourceTrayInTimeSet=true;
  opc.localAlarms[NGSorterErrors::ManualTargetLoad]=true;g.pauseStatus=r.pauseStatus=true;
  assert(m.ResetManualTrayLoadForAuto());assert(!m.manualTraySessionUsed&&!m.IsManualTrayLoadBusy()&&m.manualTrayIndex==-1);
  assert(m.manualTrayRetryPhase==0&&m.manualTrayTick==0&&m.manualTrayWaitLog.IsEmpty());
  assert(!m.opcTrayDisplayed[0]&&!m.opcTrayLoaded[0]&&!m.opcTrayLoadPending[1]&&m.step[0].step==0&&m.step[1].step==0);
  assert(!m.opcProcessStartPending&&!m.opcProcessStarted&&!m.opcSortingStartPending&&!m.opcFmsSuspendedByManual);
  assert(!m.sourceTrayCycleAdmitted&&!m.sourceCenteringCompleted&&!m.sourceTrayInTimeSet&&m.flowResets==1);
  assert(m.reader[0].cancels==1&&m.reader[1].cancels==1&&opc.cancels[0]==1&&opc.cancels[1]==1);
  assert(!opc.localAlarms[NGSorterErrors::ManualTargetLoad]&&g.pauseStatus&&r.pauseStatus);
  m.RetryManualTrayLoad();m.AcceptManualTrayBarcode(1,"LATE");m.PollManualTrayLoad();assert(opc.requests[0]==0&&opc.requests[1]==0);
 }
 puts("PASS: completed, barcode/result/reset waits and failure all reset for AUTO; old callbacks cannot revive manual requests; safety Pause retained");
 reset();{TMainForm m;m.opcProcessStarted=true;assert(m.ResetManualTrayLoadForAuto());assert(m.opcProcessStarted&&m.flowResets==0);}
 reset();{TMainForm m;m.StartManualTrayLoad(true,"TEST");gateway.raw="true";m.PollFmsTrouble();assert(m.ResetManualTrayLoadForAuto()&&m.manualTrayPhase==0&&m.fmsTroubleLatched&&alarm.visible);}
 reset();{TMainForm m;m.StartManualTrayLoad(true,"TEST");m.manualFmsPolling=true;assert(!m.ResetManualTrayLoadForAuto());m.manualFmsPolling=false;m.opcCellTrackOutPending=true;assert(!m.ResetManualTrayLoadForAuto()&&m.opcCellTrackOutPending);}
 puts("PASS: manual cleanup during Trouble keeps remote alarm; active data dialog and actual unreported cell not discarded");
 reset();{TMainForm m;m.StartManualTrayLoad(true,"BEFORE ALARM");gateway.raw="true";gateway.code="9001";m.PollFmsTrouble();m.autoBtnClick(NULL);
  assert(m.equipMode==modeAutoStop&&m.autoBtn->Down&&!m.manualBtn->Down&&m.manualTrayPhase==0&&m.fmsTroubleLatched&&g.pauseStatus);
  m.playBtnClick(NULL);assert(m.equipMode==modeAuto&&m.playBtn->Down&&!g.pauseStatus&&!r.pauseStatus&&m.restartCalls==1&&m.resumeCalls==1);
  m.fmsServiceTimerTimer(NULL);assert(!g.pauseStatus&&!r.pauseStatus&&opc.localAlarms[NGSorterErrors::FmsTrouble]&&gateway.raw==Text("true"));}
 reset();{TMainForm m;gateway.raw="true";m.PollFmsTrouble();m.autoBtnClick(NULL);m.physicalReady=false;m.playBtnClick(NULL);
  assert(m.equipMode==modeAutoStop&&!m.playBtn->Down&&g.pauseStatus&&r.pauseStatus&&m.resumeCalls==0);m.physicalReady=true;m.playBtnClick(NULL);assert(m.equipMode==modeAuto&&m.resumeCalls==1&&!g.pauseStatus);}
 reset();{TMainForm m;m.physicalReady=false;gateway.raw="true";m.PollFmsTrouble();m.autoBtnClick(NULL);assert(m.equipMode==modeManual&&!m.autoBtn->Down);}
 puts("PASS: actual AUTO/START handlers enter AUTO RUN with Trouble ON and invoke physical Restart; physical failure still prevents RUN");
 return 0;
}
