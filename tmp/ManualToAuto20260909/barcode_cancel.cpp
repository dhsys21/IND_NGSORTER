#include <string>
#include <cassert>
#include <cstdio>
typedef void TObject;
typedef std::string AnsiString;
struct Timer{bool Enabled;int Interval;Timer():Enabled(false),Interval(0){}};
struct Client{bool Active;int closes;Client():Active(true),closes(0){}void Close(){Active=false;closes++;}};
struct Popup{bool Visible;int Tag,shows;Popup():Visible(false),Tag(0),shows(0){}void ShowError(std::string,bool){shows++;}} popup,*ErrorForm_bcr=&popup;
struct Main{int logs;Main():logs(0){}void memoMainLineAdd(std::string){logs++;}} mainform,*MainForm=&mainform;
struct TMod_Bcr{
 bool bReading;int Tag,on,off;AnsiString rxBuffer;Timer timer,reconnect,*Timer1,*Timer_AutoConnect;Client client,*ClientSocketBcr;
 TMod_Bcr():bReading(false),Tag(0),on(0),off(0),Timer1(&timer),Timer_AutoConnect(&reconnect),ClientSocketBcr(&client){}
 AnsiString ReaderName(){return "TEST";}void TriggerOn(){on++;}void TriggerOff(){if(client.Active)off++;}
 void __fastcall GetBarcode();void __fastcall CancelScan();void __fastcall ReadTimeoutTimer(TObject*);
};
void __fastcall TMod_Bcr::CancelScan()
{
	//* MANUAL -> AUTO: no old timeout/result may create an AUTO load request.
	bool wasReading = bReading;
	bReading = false;
	Timer1->Enabled = false;
	rxBuffer = "";
	if(!wasReading) return;
	TriggerOff();
	// This protocol has no request ID: isolate late packets using a new session,
	// just as ReadTimeoutTimer does. The normal reconnect timer stays responsible.
	ClientSocketBcr->Close();
	Timer_AutoConnect->Enabled = true;
	if(MainForm != NULL)
		MainForm->memoMainLineAdd(ReaderName() + " manual scan cancelled / old TCP session discarded / reconnect pending.");
}
void __fastcall TMod_Bcr::GetBarcode()
{
	if(bReading) return; // One outstanding trigger per TCP session.
	if(!ClientSocketBcr->Active) {
		ErrorForm_bcr->Tag = Tag;
		ErrorForm_bcr->ShowError(ReaderName() + " is disconnected.", false);
		return;
	}

	rxBuffer = "";
	bReading = true;
	Timer1->Enabled = false;
	Timer1->Interval = ErrorForm_bcr->Visible ? 50000 : 3000;
	Timer1->Enabled = true;
	TriggerOn();
}
void __fastcall TMod_Bcr::ReadTimeoutTimer(TObject *Sender)
{
	Timer1->Enabled = false;
	if(!bReading) return;

	bReading = false;
	TriggerOff();
	rxBuffer = "";
	// The protocol has no request ID. A new TCP session isolates late results.
	ClientSocketBcr->Close();
	ErrorForm_bcr->Tag = Tag;
	if(Tag == 0)
		ErrorForm_bcr->ShowError("Source Tray barcode can not be scanned.", false);
	else
		ErrorForm_bcr->ShowError("Target Tray barcode can not be scanned.", false);
}

int main(){
 TMod_Bcr b;b.GetBarcode();assert(b.timer.Enabled&&b.bReading&&b.on==1);
 b.rxBuffer="OLD";b.CancelScan();assert(!b.bReading&&!b.timer.Enabled&&b.off==1&&b.rxBuffer.empty());
 assert(!b.client.Active&&b.client.closes==1&&b.reconnect.Enabled);
 b.ReadTimeoutTimer(NULL);assert(popup.shows==0&&b.client.closes==1);
 b.CancelScan();assert(b.client.closes==1);
 b.client.Active=true;b.GetBarcode();assert(b.bReading&&b.timer.Enabled&&b.on==2);
 b.client.Active=false;b.CancelScan();assert(!b.bReading&&!b.timer.Enabled&&b.reconnect.Enabled);
 puts("PASS: pending TCP scan cancelled, timeout inert, old session isolated, idle cancel harmless, new scan rearms");
}
