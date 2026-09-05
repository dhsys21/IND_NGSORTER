#include <string>
#include <cstdio>
#include <cassert>
class Text : public std::string {
public:
    Text() {}
    Text(const char *s):std::string(s){}
    Text(const std::string &s):std::string(s){}
    Text(const wchar_t *s){while(*s)push_back((char)*s++);}
};
typedef Text AnsiString;
typedef Text UnicodeString;
typedef unsigned long DWORD;
AnsiString IntToStr(int n){char s[32];sprintf(s,"%d",n);return s;}
const UnicodeString TAG_SOURCE="Location1",TAG_TARGET="Location2";
int response=1, writes=0; bool request=false;
UnicodeString TrayProcessTag(UnicodeString a,UnicodeString b){return a+"."+b;}
UnicodeString TrayInfoTag(UnicodeString a,UnicodeString b){return a+"."+b;}
UnicodeString CellTrackOutTag(UnicodeString b){return b;}
UnicodeString LocationFor(bool s){return s?TAG_SOURCE:TAG_TARGET;}
int GetFmsInt(UnicodeString){return response;}
void SetPcBool(UnicodeString,bool b){request=b;++writes;}
void SetPcString(UnicodeString,UnicodeString){}
void SetTrayLoadValidationError(bool,AnsiString){}
void LogOpcEvent(AnsiString,bool=false){}
struct Gateway{void FlushPendingPcTags(bool){}} gateway,*Mod_Fms=&gateway;
struct TPanel{AnsiString Caption;} sourceId,targetId;
TPanel *TrayIdPanelFor(bool s){return s?&sourceId:&targetId;}
bool GTargetInfoPromptActive=false;
UnicodeString GTargetInfoResolvedTrayId;
int GTargetInfoResolvedChoice=0;
DWORD GetTickCount(){return 123;}
enum Transaction{fmsAlarmNone,fmsAlarmSourceTrayLoad,fmsAlarmTargetTrayLoad,fmsAlarmProcessStart,
    fmsAlarmCellTrackOut,fmsAlarmProcessEnd,fmsAlarmTrayUnload};
struct CheckBox{bool Checked;} cycle;
struct Alarm{void SetRetryWaiting(AnsiString){} void Hide(){} void RefreshAlarmVisibility(){}} alarm,*AlarmForm_fms=&alarm;
class TMainForm{
public:
    Transaction fmsAlarmTransaction;
    bool fmsAlarmRetryRequested,fmsAlarmAwaitingReset;
    int fmsAlarmAcceptedResult,currentProcessStep,reissues;
    CheckBox *cbCycle;
    bool opcTrayLoadPending[2],opcTrayLoadWaitResponseOff[2],opcTrayLoadRetryRequired[2];
    int opcTrayLoadResponseResult[2]; DWORD opcTrayLoadStartTick[2];
    bool opcProcessStartPending,opcProcessStartWaitResponseOff,opcCellTrackOutPending,opcCellTrackOutWaitResponseOff;
    bool opcProcessEndPending,opcProcessEndWaitResponseOff,opcTargetUnloadPending,opcTargetUnloadWaitResponseOff;
    int opcProcessStartResponseResult,opcCellTrackOutResponseResult,opcProcessEndResponseResult,opcTargetUnloadResponseResult;
    DWORD opcProcessStartTick,opcCellTrackOutStartTick,opcProcessEndTick,opcTargetUnloadTick;
    TMainForm():fmsAlarmTransaction(fmsAlarmTargetTrayLoad),fmsAlarmRetryRequested(true),
        fmsAlarmAwaitingReset(false),fmsAlarmAcceptedResult(1),currentProcessStep(5),reissues(0),cbCycle(&cycle){cycle.Checked=false;}
    bool __fastcall ProcessFmsAlarmRecovery();
    void CheckFmsResetRetryTimeout(int){} // UI retry timeout tested separately.
    int GetFmsAlarmResponse(){return response;}
    void ReissueFmsAlarmRequest(){++reissues;fmsAlarmTransaction=fmsAlarmNone;}
    void ProcessStepLog(int,AnsiString){}
} mainForm,*MainForm=&mainForm;
class TMesOpc{
public:
    void __fastcall TRAY_LOAD_REQUEST(bool);
    int __fastcall TRAY_LOAD_RESPONSE(bool);
    void __fastcall PROCESS_START_REQUEST();
    void __fastcall PROCESS_END_REQUEST();
    void __fastcall TRAY_UNLOAD_REQUEST();
    int __fastcall PROCESS_START_RESPONSE_RESULT();
    int __fastcall PROCESS_END_RESPONSE_RESULT();
    int __fastcall TRAY_UNLOAD_RESPONSE_RESULT();
    int __fastcall CELL_TRACK_OUT_RESPONSE_RESULT();
};
void __fastcall TMesOpc::PROCESS_START_REQUEST()
{
	// FMS CURRENT RESPONSE: no pre-request reset/revision validation.
	const UnicodeString RequestKey = TrayProcessTag(TAG_SOURCE, L"ProcessStart");
	const UnicodeString ResponseKey = TrayProcessTag(TAG_SOURCE, L"ProcessStartResponse");
	int Response = GetFmsInt(ResponseKey);
	SetPcBool(RequestKey, true);
	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
	LogOpcEvent("PROCESS_START_REQUEST / Request=ON / CurrentResponse=" + IntToStr(Response) +
		" / EXPECTED=1 or 2 / pre-request reset check DISABLED");
}

void __fastcall TMesOpc::PROCESS_END_REQUEST()
{
	// FMS CURRENT RESPONSE: no pre-request reset/revision validation.
	const UnicodeString RequestKey = TrayProcessTag(TAG_SOURCE, L"ProcessEnd");
	const UnicodeString ResponseKey = TrayProcessTag(TAG_SOURCE, L"ProcessEndResponse");
	int Response = GetFmsInt(ResponseKey);
	SetPcBool(RequestKey, true);
	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
	LogOpcEvent("PROCESS_END_REQUEST / Request=ON / CurrentResponse=" + IntToStr(Response) +
		" / EXPECTED=1 or 2 / pre-request reset check DISABLED", true);
}

void __fastcall TMesOpc::TRAY_UNLOAD_REQUEST()
{
	// FMS CURRENT RESPONSE: no pre-request reset/revision validation.
	const UnicodeString RequestKey = TrayProcessTag(TAG_TARGET, L"TrayUnloadRequest");
	const UnicodeString ResponseKey = TrayProcessTag(TAG_TARGET, L"TrayUnloadResponse");
	int Response = GetFmsInt(ResponseKey);
	SetPcBool(RequestKey, true);
	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
	LogOpcEvent("TRAY_UNLOAD_REQUEST / Request=ON / CurrentResponse=" + IntToStr(Response) +
		" / EXPECTED=1 or 2 / pre-request reset check DISABLED", false);
}

int __fastcall TMesOpc::PROCESS_START_RESPONSE_RESULT()
{
	const UnicodeString RequestKey = TrayProcessTag(TAG_SOURCE, L"ProcessStart");
	const UnicodeString ResponseKey = TrayProcessTag(TAG_SOURCE, L"ProcessStartResponse");
	int Response = GetFmsInt(ResponseKey);
	// FMS CURRENT RESPONSE: accept the visible value without a saved baseline.
	if(Response == 0)
		return 0;
	// Current 1/2 is the result; FormMain owns the post-result reset wait.

	// Clear the PC request after either ACK result. Completion is handled only
	// after the FMS response also returns to zero.
	SetPcBool(RequestKey, false);
	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
	if(Response == 1){
		LogOpcEvent("PROCESS_START_RESPONSE=1 / Request=OFF / wait Response=0");
		return 1;
	}
	if(Response == 2){
		LogOpcEvent("PROCESS_START_RESPONSE=2 / Request=OFF / wait Response=0", true);
		return 2;
	}

	LogOpcEvent("VALIDATION FAIL ProcessStartResponse=" + IntToStr(Response), true);
	return -1;
}

int __fastcall TMesOpc::PROCESS_END_RESPONSE_RESULT()
{
	const UnicodeString RequestKey = TrayProcessTag(TAG_SOURCE, L"ProcessEnd");
	const UnicodeString ResponseKey = TrayProcessTag(TAG_SOURCE, L"ProcessEndResponse");
	int Response = GetFmsInt(ResponseKey);
	// FMS CURRENT RESPONSE: accept the visible value without a saved baseline.
	if(Response == 0)
		return 0;
	// Current response value only; no prior-value comparison.

	SetPcBool(RequestKey, false);
	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
	if(Response == 1){
		LogOpcEvent("PROCESS_END_RESPONSE=1 / Request=OFF / wait Response=0", true);
		return 1;
	}
	if(Response == 2){
		LogOpcEvent("PROCESS_END_RESPONSE=2 / Request=OFF / wait Response=0", true);
		return 2;
	}
	LogOpcEvent("VALIDATION FAIL ProcessEndResponse=" + IntToStr(Response), true);
	return -1;
}

int __fastcall TMesOpc::TRAY_UNLOAD_RESPONSE_RESULT()
{
	const UnicodeString RequestKey = TrayProcessTag(TAG_TARGET, L"TrayUnloadRequest");
	const UnicodeString ResponseKey = TrayProcessTag(TAG_TARGET, L"TrayUnloadResponse");
	int Response = GetFmsInt(ResponseKey);
	// FMS CURRENT RESPONSE: accept the visible value without a saved baseline.
	if(Response == 0) return 0;
	// Current response value only; no prior-value comparison.

	SetPcBool(RequestKey, false);
	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
	if(Response == 1){
		LogOpcEvent("TRAY_UNLOAD_RESPONSE=1 / Request=OFF / wait Response=0", false);
		return 1;
	}
	if(Response == 2){
		LogOpcEvent("TRAY_UNLOAD_RESPONSE=2 / Request=OFF / wait Response=0", false);
		return 2;
	}
	LogOpcEvent("VALIDATION FAIL TrayUnloadResponse=" + IntToStr(Response), false);
	return -1;
}

int __fastcall TMesOpc::CELL_TRACK_OUT_RESPONSE_RESULT()
{
	const UnicodeString RequestKey = CellTrackOutTag(L"CellUnloadComplete");
	const UnicodeString ResponseKey = CellTrackOutTag(L"CellUnloadCompleteResponse");
	int Response = GetFmsInt(ResponseKey);
	// FMS CURRENT RESPONSE: accept the visible value without a saved baseline.
	if(Response == 0)
		return 0;
	// Current 1/2 is the result; FormMain owns the post-result reset wait.

	SetPcBool(RequestKey, false);
	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
	if(Response == 1){
		LogOpcEvent("CELL_TRACK_OUT RESPONSE=1 / Request=OFF / wait Response=0", true);
		return 1;
	}
	if(Response == 2){
		LogOpcEvent("CELL_TRACK_OUT RESPONSE=2 / Request=OFF / wait Response=0", true);
		return 2;
	}
	LogOpcEvent("VALIDATION FAIL CellUnloadCompleteResponse=" + IntToStr(Response), true);
	return -1;
}

void __fastcall TMesOpc::TRAY_LOAD_REQUEST(bool SourceTray)
{
	if (MainForm == NULL)
		return;
	if (!SourceTray)
	{
		// One FMS/LOCAL decision is valid for one Location2 TrayLoad request.
		GTargetInfoResolvedTrayId = L"";
		GTargetInfoResolvedChoice = 0;
	}

	UnicodeString Location = LocationFor(SourceTray);
	TPanel *TrayIdPanel = TrayIdPanelFor(SourceTray);
	UnicodeString TrayId = L"";
	if (TrayIdPanel != NULL)
		TrayId = TrayIdPanel->Caption;

	// FMS CURRENT RESPONSE: publish payload and Request=ON immediately.
	// InitialResponse is diagnostic only, even if it is already 1 or 2.
	SetTrayLoadValidationError(SourceTray, "");
	UnicodeString RequestKey = TrayProcessTag(Location, L"TrayLoad");
	UnicodeString ResponseKey = TrayProcessTag(Location, L"TrayLoadResponse");
	int InitialResponse = GetFmsInt(ResponseKey);
	SetPcBool(TrayInfoTag(Location, L"TrayExist"), true);
	SetPcString(TrayInfoTag(Location, L"TrayId"), TrayId);
	SetPcBool(RequestKey, true);
	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
	LogOpcEvent("TRAY_LOAD_REQUEST " + AnsiString(Location) + " TrayId=" + AnsiString(TrayId) +
		" / Request=ON / CurrentResponse=" + IntToStr(InitialResponse) +
		" / EXPECTED=1 or 2 / pre-request reset check DISABLED", true);
}

int __fastcall TMesOpc::TRAY_LOAD_RESPONSE(bool SourceTray)
{
	// A modal choice dialog pumps messages. Ignore any nested timer callback
	// until the operator has completed the current FMS/LOCAL decision.
	if (!SourceTray && GTargetInfoPromptActive)
		return 0;

	UnicodeString Location = LocationFor(SourceTray);
	UnicodeString ResponseKey = TrayProcessTag(Location, L"TrayLoadResponse");

	int Response = GetFmsInt(ResponseKey);
	// FMS CURRENT RESPONSE: accept the visible value without a saved baseline.
	if (Response == 0)
		return 0;
	// Current 1/2 is the result; FormMain owns the post-result reset wait.
	if (Response != 1 && Response != 2)
	{
		LogOpcEvent("VALIDATION FAIL TrayLoadResponse=" + IntToStr(Response), true);
		SetPcBool(TrayProcessTag(Location, L"TrayLoad"), false);
		if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
		return -1;
	}
	if (Response == 2)
	{
		LogOpcEvent("TRAY_LOAD_RESPONSE FAIL " + AnsiString(Location), true);
		SetPcBool(TrayProcessTag(Location, L"TrayLoad"), false);
		if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
		return 2;
	}

	return 1;
}

bool __fastcall TMainForm::ProcessFmsAlarmRecovery()
{
	if(fmsAlarmTransaction == fmsAlarmNone)
		return false;
	if(!fmsAlarmRetryRequested){
		if(AlarmForm_fms != NULL) AlarmForm_fms->RefreshAlarmVisibility();
		return true;
	}

	if(!fmsAlarmAwaitingReset){
		// FMS CURRENT RESPONSE: a result timeout retries immediately, even when
		// Response is already 1/2. No old-response reset is required here.
		ReissueFmsAlarmRequest();
		return fmsAlarmTransaction != fmsAlarmNone;
	}
	int response = GetFmsAlarmResponse();
	bool cycleResponseBypass = cbCycle != NULL && cbCycle->Checked;
	if(response != 0 && !cycleResponseBypass){
		if(AlarmForm_fms != NULL)
			AlarmForm_fms->SetRetryWaiting("Result already accepted / Request=OFF / EXPECTED=0 (RESET) / CURRENT=" + IntToStr(response));
		CheckFmsResetRetryTimeout(response);
		return true;
	}
	// Resume the already-accepted transaction at its final phase. The normal
	// polling code performs completion once; no new Request=ON is sent.
	switch(fmsAlarmTransaction){
		case fmsAlarmSourceTrayLoad:
		case fmsAlarmTargetTrayLoad:{
			int i = fmsAlarmTransaction == fmsAlarmSourceTrayLoad ? 0 : 1;
			opcTrayLoadPending[i] = true;
			opcTrayLoadWaitResponseOff[i] = true;
			opcTrayLoadResponseResult[i] = fmsAlarmAcceptedResult;
			opcTrayLoadRetryRequired[i] = false;
			opcTrayLoadStartTick[i] = GetTickCount(); break;
		}
		case fmsAlarmProcessStart:
			opcProcessStartPending = opcProcessStartWaitResponseOff = true;
			opcProcessStartResponseResult = fmsAlarmAcceptedResult;
			opcProcessStartTick = GetTickCount(); break;
		case fmsAlarmCellTrackOut:
			opcCellTrackOutPending = opcCellTrackOutWaitResponseOff = true;
			opcCellTrackOutResponseResult = fmsAlarmAcceptedResult;
			opcCellTrackOutStartTick = GetTickCount(); break;
		case fmsAlarmProcessEnd:
			opcProcessEndPending = opcProcessEndWaitResponseOff = true;
			opcProcessEndResponseResult = fmsAlarmAcceptedResult;
			opcProcessEndTick = GetTickCount(); break;
		case fmsAlarmTrayUnload:
			opcTargetUnloadPending = opcTargetUnloadWaitResponseOff = true;
			opcTargetUnloadResponseResult = fmsAlarmAcceptedResult;
			opcTargetUnloadTick = GetTickCount(); break;
		default: return true;
	}
	ProcessStepLog(currentProcessStep, "FMS Retry / final Response reset accepted / resume completion without resending request");
	fmsAlarmTransaction = fmsAlarmNone;
	fmsAlarmRetryRequested = false;
	fmsAlarmAwaitingReset = false;
	fmsAlarmAcceptedResult = 0;
	if(AlarmForm_fms != NULL) AlarmForm_fms->Hide();
	return false;
}

int main(){
    TMesOpc opc;
    for(int initial=0;initial<=2;++initial){
        response=initial;
        request=false;opc.TRAY_LOAD_REQUEST(true);assert(request);
        request=false;opc.TRAY_LOAD_REQUEST(false);assert(request);
        request=false;opc.PROCESS_START_REQUEST();assert(request);
        request=false;opc.PROCESS_END_REQUEST();assert(request);
        request=false;opc.TRAY_UNLOAD_REQUEST();assert(request);
    }
    puts("PASS: requests start with response already 0/1/2; no pre-request reset gate");
    for(int current=0;current<=3;++current){
        response=current;int expected=current==3?-1:current;
        assert(opc.TRAY_LOAD_RESPONSE(true)==expected);
        assert(opc.TRAY_LOAD_RESPONSE(false)==expected);
        assert(opc.PROCESS_START_RESPONSE_RESULT()==expected);
        assert(opc.PROCESS_END_RESPONSE_RESULT()==expected);
        assert(opc.TRAY_UNLOAD_RESPONSE_RESULT()==expected);
        assert(opc.CELL_TRACK_OUT_RESPONSE_RESULT()==expected);
    }
    puts("PASS: current response classification (including repeated 1/2), six response paths");
    response=1;TMainForm retry;
    assert(!retry.ProcessFmsAlarmRecovery() && retry.reissues==1);
    puts("PASS: result-timeout Retry accepts an already-present response without waiting for zero");
    for(int txn=fmsAlarmSourceTrayLoad;txn<=fmsAlarmTrayUnload;++txn){
        TMainForm reset;reset.fmsAlarmTransaction=(Transaction)txn;reset.fmsAlarmAwaitingReset=true;
        response=1;assert(reset.ProcessFmsAlarmRecovery() && reset.reissues==0);
        response=0;assert(!reset.ProcessFmsAlarmRecovery() && reset.reissues==0);
        assert(reset.fmsAlarmTransaction==fmsAlarmNone);
    }
    puts("PASS: reset-timeout recovery keeps waiting for 0 and never resends accepted transactions");
    TMainForm demo;demo.fmsAlarmAwaitingReset=true;demo.cbCycle->Checked=true;
    response=1;assert(!demo.ProcessFmsAlarmRecovery() && demo.reissues==0);
    puts("PASS: Cycle Test bypasses final reset; normal-mode reset branches and tray validation retained");
    return 0;
}
