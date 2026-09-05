#include <string>
#include <cstdio>
#include <cassert>
typedef std::string AnsiString;
typedef unsigned long DWORD;
typedef void TObject;
enum TCloseAction { caNone, caHide };
enum { fsNormal, LampAlarm, fmsAlarmNone, fmsAlarmTargetTrayLoad };
DWORD ticks=0;
DWORD GetTickCount(){return ticks;}
AnsiString IntToStr(int n){char s[32];sprintf(s,"%d",n);return s;}
struct Axis { bool paused; Axis():paused(false){} void req_Pause(bool v){paused=v;} } g,r,*gripper=&g,*robostar=&r;
struct Widget { AnsiString Caption,Text; bool Enabled; Widget *Lines; Widget():Enabled(true),Lines(this){} };
class TAlarmForm_fms {
public:
    bool operatorPaused,dismissed,Visible;
    DWORD dismissedTick;
    int shows,FormStyle;
    Widget title,detail,request,response,status,pause;
    Widget *lblTitle,*memoDetail,*lblRequest,*lblResponse,*lblStatus,*btnPause;
    TAlarmForm_fms():operatorPaused(false),dismissed(false),Visible(false),dismissedTick(0),shows(0),
        lblTitle(&title),memoDetail(&detail),lblRequest(&request),lblResponse(&response),lblStatus(&status),btnPause(&pause){}
    void Show(){Visible=true;++shows;}
    void Hide(){Visible=false;FormHide(NULL);}
    void Close(){TCloseAction a=caNone;FormClose(NULL,a);assert(a==caHide);Hide();}
    void BringToFront(){}
    void __fastcall ShowFmsError(const AnsiString&,const AnsiString&,const AnsiString&,int);
    void __fastcall SetRetryWaiting(const AnsiString&);
    void __fastcall btnPauseClick(TObject*);
    void __fastcall btnRetryClick(TObject*);
    void __fastcall SetOperatorPaused();
    void __fastcall btnCloseClick(TObject*);
    void __fastcall FormClose(TObject*,TCloseAction&);
    void __fastcall RefreshAlarmVisibility();
    void __fastcall FormHide(TObject*);
} alarm,*AlarmForm_fms=&alarm;
class TMainForm {
public:
    int fmsAlarmTransaction,fmsAlarmAcceptedResult,logs,errors,cancels;
    bool fmsAlarmRetryRequested,fmsAlarmAwaitingReset,automatic;
    DWORD fmsAlarmRetryStartTick;
    Widget timer,*opcMesTimer;
    TMainForm():fmsAlarmTransaction(fmsAlarmTargetTrayLoad),fmsAlarmAcceptedResult(1),logs(0),errors(0),cancels(0),
        fmsAlarmRetryRequested(false),fmsAlarmAwaitingReset(true),automatic(true),fmsAlarmRetryStartTick(0),opcMesTimer(&timer){}
    void memoMainLineAdd(AnsiString){++logs;}
    void WriteErrorLog(AnsiString,AnsiString){++errors;}
    void WriteOpcUaLog(AnsiString,AnsiString,bool){}
    void BuzzerOn(bool){}
    void LampModeChange(int){}
    bool CheckAutomaticFmsMode(AnsiString){return automatic;}
    void CancelFmsAlarmRequest(){++cancels;}
    void __fastcall PauseFmsAlarm();
    void __fastcall CheckFmsResetRetryTimeout(int);
    void __fastcall ConfirmFmsAlarmRetry();
    // Spy: popup must delegate to Main Restart, not directly to FMS recovery.
    int restartCalls;
    void pause_startBtnClick(TObject*){++restartCalls;}
} mainForm,*MainForm=&mainForm;
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

void __fastcall TAlarmForm_fms::SetRetryWaiting(const AnsiString &Status)
{
	lblStatus->Caption = Status;
}

void __fastcall TAlarmForm_fms::btnPauseClick(TObject *Sender)
{
	if(MainForm != NULL)
		MainForm->PauseFmsAlarm();
}

void __fastcall TAlarmForm_fms::btnRetryClick(TObject *Sender)
{
	// FMS ALARM RETRY: use the exact Main Restart path, including AUTO checks,
	// handshake recovery and releasing both paused sequences. Do not issue a
	// request directly here: retrying FMS alone would leave motion paused.
	if(MainForm != NULL)
		MainForm->pause_startBtnClick(Sender);
}

void __fastcall TAlarmForm_fms::SetOperatorPaused()
{
	operatorPaused = true;
	btnPause->Enabled = false;
	lblStatus->Caption = "PAUSED - Close is available. Correct FMS, then press Retry or Main Restart to retry this step.";
}

void __fastcall TAlarmForm_fms::btnCloseClick(TObject *Sender)
{
	Close();
}

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

void __fastcall TAlarmForm_fms::FormHide(TObject *Sender)
{
	if(MainForm != NULL){
		MainForm->BuzzerOn(false);
		// Hiding an unresolved alarm does not mean the equipment recovered.
	}
}

void __fastcall TMainForm::PauseFmsAlarm()
{
	// FMS ALARM PAUSE/CLOSE: no InitWork, no data clearing and no new request.
	// Also stop an in-progress reset retry; only Main Restart may re-arm it.
	if(gripper != NULL) gripper->req_Pause(true);
	if(robostar != NULL) robostar->req_Pause(true);
	if(fmsAlarmTransaction == fmsAlarmNone) return;
	fmsAlarmRetryRequested = false;
	if(AlarmForm_fms != NULL) AlarmForm_fms->SetOperatorPaused();
	memoMainLineAdd("[FMS ALARM] PAUSED / transaction and accepted result retained / WAIT Main Restart.");
}

void __fastcall TMainForm::CheckFmsResetRetryTimeout(int ResponseValue)
{
	// FMS ALARM PAUSE/CLOSE: a hidden reset retry must not wait indefinitely.
	if((DWORD)(GetTickCount() - fmsAlarmRetryStartTick) < 10000) return;
	fmsAlarmRetryRequested = false;
	if(gripper != NULL) gripper->req_Pause(true);
	if(robostar != NULL) robostar->req_Pause(true);
	AnsiString detail = "Final response reset retry timed out after 10 seconds / Request=OFF / EXPECTED=0 (RESET) / CURRENT=" + IntToStr(ResponseValue);
	WriteOpcUaLog("ERROR", detail, true);
	if(AlarmForm_fms != NULL)
		AlarmForm_fms->ShowFmsError("FMS response reset retry timeout", detail,
			AlarmForm_fms->lblRequest->Caption, ResponseValue);
}

void __fastcall TMainForm::ConfirmFmsAlarmRetry()
{
	if(!CheckAutomaticFmsMode("FMS alarm retry"))
		return;
	if(fmsAlarmTransaction == fmsAlarmNone)
		return;

	fmsAlarmRetryRequested = true;
	fmsAlarmRetryStartTick = GetTickCount();
	CancelFmsAlarmRequest();
	if(AlarmForm_fms != NULL)
		AlarmForm_fms->SetRetryWaiting(
			fmsAlarmAwaitingReset ?
			"Result already accepted. Request is OFF; waiting for final Response=0." :
			"Retrying Request=ON; current Response=1/2 is accepted without a pre-request reset.");
	// Keep MainForm accessible while retrying; any new failure shows the popup.
	if(AlarmForm_fms != NULL) AlarmForm_fms->Hide();
	opcMesTimer->Enabled = true;
}

int main(){
    alarm.ShowFmsError("Timeout","detail","Location2.TrayLoad",1);
    assert(g.paused && r.paused && alarm.Visible && alarm.FormStyle==fsNormal);
    alarm.btnCloseClick(NULL);
    assert(!alarm.Visible && mainForm.fmsAlarmTransaction==fmsAlarmTargetTrayLoad && !mainForm.fmsAlarmRetryRequested);
    ticks=9999;alarm.RefreshAlarmVisibility();assert(!alarm.Visible);
    ticks=10000;alarm.RefreshAlarmVisibility();assert(alarm.Visible && alarm.shows==2 && mainForm.errors==1);
    alarm.RefreshAlarmVisibility();assert(alarm.shows==2);
    puts("PASS: Close retains transaction; unresolved alarm reappears once after 10s, without duplicate error logs");
    alarm.btnPauseClick(NULL);alarm.btnCloseClick(NULL);
    ticks=90000;alarm.RefreshAlarmVisibility();assert(!alarm.Visible && alarm.operatorPaused);
    assert(mainForm.fmsAlarmAwaitingReset && mainForm.fmsAlarmAcceptedResult==1);
    mainForm.ConfirmFmsAlarmRetry();assert(mainForm.fmsAlarmRetryRequested && !alarm.Visible && mainForm.cancels==1);
    alarm.SetRetryWaiting("waiting");assert(!alarm.Visible);
    mainForm.PauseFmsAlarm();assert(!mainForm.fmsAlarmRetryRequested);
    puts("PASS: Pause suppresses popup; Restart arms same phase; Pause during reset retry stops retry without losing result");
    mainForm.ConfirmFmsAlarmRetry();
    ticks=99999;mainForm.CheckFmsResetRetryTimeout(1);assert(mainForm.fmsAlarmRetryRequested);
    ticks=100000;mainForm.CheckFmsResetRetryTimeout(1);
    assert(!mainForm.fmsAlarmRetryRequested && alarm.Visible && !alarm.operatorPaused && g.paused && r.paused);
    assert(mainForm.fmsAlarmAcceptedResult==1 && mainForm.fmsAlarmAwaitingReset);
    assert(alarm.lblStatus->Caption.find("Main Restart")!=AnsiString::npos);
    puts("PASS: failed final-reset retry pauses and shows a fresh alarm after 10s; accepted result retained");
    mainForm.automatic=false;mainForm.ConfirmFmsAlarmRetry();assert(!mainForm.fmsAlarmRetryRequested);
    puts("PASS: MANUAL cannot retry; design-time Pause/Retry/Close event bindings verified by runner");
    mainForm.restartCalls=0;
    int oldCancels=mainForm.cancels;
    alarm.btnRetryClick(NULL);
    assert(mainForm.restartCalls==1 && mainForm.cancels==oldCancels);
    MainForm=NULL;alarm.btnRetryClick(NULL);MainForm=&mainForm;
    puts("PASS: popup Retry delegates exactly once to Main Restart; no direct FMS request or bypass");
    return 0;
}
