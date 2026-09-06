//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma classgroup "Vcl.Controls.TControl"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TMod_Bcr::TMod_Bcr(TComponent* Owner)
	: TDataModule(Owner)
{
	bClose = false;
	bReading = false;
	rxBuffer = "";
	Timer1->Enabled = false;
	Timer1->Interval = 3000;
	Timer1->OnTimer = ReadTimeoutTimer;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMod_Bcr::ReaderName()
{
	if(Tag == 0) return "Source Tray BCR";
	if(Tag == 1) return "Target Tray BCR";
	return "BCR";
}
//---------------------------------------------------------------------------
void __fastcall TMod_Bcr::Connect()
{
	Connect("192.168.100.238", 9004);
}
//---------------------------------------------------------------------------
void __fastcall TMod_Bcr::Connect(AnsiString IpAddress, int Port)
{
	bClose = false;
	if(ClientSocketBcr->Active) return;

	try {
		ClientSocketBcr->Address = IpAddress;
		ClientSocketBcr->Host = IpAddress;
		ClientSocketBcr->Port = Port;
		ClientSocketBcr->Open();
		if(MainForm != NULL)
			MainForm->memoMainLineAdd(ReaderName() + " connect request: " + IpAddress + ":" + IntToStr(Port));
	}
	catch(const Exception &e) {
		if(MainForm != NULL)
			MainForm->memoMainLineAdd(ReaderName() + " connect fail: " + e.Message);
	}
}
//---------------------------------------------------------------------------
void __fastcall TMod_Bcr::Disconnect()
{
	bClose = true;
	Timer1->Enabled = false;
	bReading = false;

	if(ClientSocketBcr->Active) {
		try {
			unsigned char cmd[5] = {0x4C, 0x4F, 0x46, 0x46, 0x0D};
			ClientSocketBcr->Socket->SendBuf(cmd, 5);
			ClientSocketBcr->Close();
		}
		catch(...) {
			ClientSocketBcr->Close();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMod_Bcr::TriggerOn()
{
	if(!ClientSocketBcr->Active) {
		if(MainForm != NULL)
			MainForm->memoMainLineAdd(ReaderName() + " is disconnected.");
		return;
	}

	try {
		unsigned char cmd[4] = {0x4C, 0x4F, 0x4E, 0x0D};
		ClientSocketBcr->Socket->SendBuf(cmd, 4);
	}
	catch(const Exception &e) {
		if(MainForm != NULL)
			MainForm->memoMainLineAdd(ReaderName() + " trigger on error: " + e.Message);
		Disconnect();
	}
}
//---------------------------------------------------------------------------
void __fastcall TMod_Bcr::TriggerOff()
{
	if(!ClientSocketBcr->Active) return;

	try {
		unsigned char cmd[5] = {0x4C, 0x4F, 0x46, 0x46, 0x0D};
		ClientSocketBcr->Socket->SendBuf(cmd, 5);
	}
	catch(const Exception &e) {
		if(MainForm != NULL)
			MainForm->memoMainLineAdd(ReaderName() + " trigger off error: " + e.Message);
		Disconnect();
	}
}
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
void __fastcall TMod_Bcr::ClientSocketBcrConnect(TObject *Sender, TCustomWinSocket *Socket)
{
	Timer_AutoConnect->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TMod_Bcr::ClientSocketBcrDisconnect(TObject *Sender, TCustomWinSocket *Socket)
{
	bReading = false;
	rxBuffer = "";
	Timer1->Enabled = false;
	if(bClose) bClose = false;
	else Timer_AutoConnect->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TMod_Bcr::ClientSocketBcrError(TObject *Sender, TCustomWinSocket *Socket,
		  TErrorEvent ErrorEvent, int &ErrorCode)
{
	ErrorCode = 0;
	Socket->Close();
	if(MainForm != NULL)
		MainForm->memoMainLineAdd(ReaderName() + " communication error.");
}
//---------------------------------------------------------------------------
void __fastcall TMod_Bcr::Timer_AutoConnectTimer(TObject *Sender)
{
	ClientSocketBcr->Active = true;
	Timer_AutoConnect->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TMod_Bcr::ClientSocketBcrRead(TObject *Sender, TCustomWinSocket *Socket)
{
	char buf[1024] = {0};
	int len = ClientSocketBcr->Socket->ReceiveBuf(buf, sizeof(buf) - 1);

	if(len <= 0) return;
	if(!bReading){ rxBuffer = ""; return; }
	buf[len] = 0;
	rxBuffer += AnsiString(buf);
	if(rxBuffer.Length() > 4096){ ReadTimeoutTimer(Sender); return; }

	int pos = rxBuffer.Pos("\r");
	int lf = rxBuffer.Pos("\n");
	if(pos <= 0 || (lf > 0 && lf < pos)) pos = lf;

	while(pos > 0) {
		AnsiString line = rxBuffer.SubString(1, pos - 1);
		rxBuffer.Delete(1, pos);
		rxBuffer = rxBuffer.TrimLeft();
		ProcessResult(line);

		pos = rxBuffer.Pos("\r");
		lf = rxBuffer.Pos("\n");
		if(pos <= 0 || (lf > 0 && lf < pos)) pos = lf;
	}

	// Keep the unterminated tail for the next TCP packet.
}
//---------------------------------------------------------------------------
void __fastcall TMod_Bcr::ProcessResult(AnsiString data)
{
	if(!bReading) return;
	AnsiString result = data.Trim();
	if(result.IsEmpty()) return;

	AnsiString upper = result.UpperCase();
	if(upper == "OK" || upper == "LON" || upper == "LOFF") return;

	Timer1->Enabled = false;
	bReading = false;
	TriggerOff();

	if(upper == "ERROR" || upper == "NG" || upper.Pos("ERROR") > 0) {
		ErrorForm_bcr->Tag = Tag;
		if(Tag == 0)
			ErrorForm_bcr->ShowError("Source Tray barcode can not be scanned.", false);
		else
			ErrorForm_bcr->ShowError("Target Tray barcode can not be scanned.", false);
		return;
	}

	if(MainForm != NULL)
		MainForm->memoMainLineAdd(ReaderName() + " scan complete: " + result);
	if(BaseForm != NULL && BaseForm->memoBcr != NULL)
		BaseForm->memoBcr->Lines->Add(ReaderName() + " scan complete: " + result);
	if(ErrorForm_bcr->Visible) ErrorForm_bcr->ShowError(result, true);
	else MainForm->setBarcode(Tag, result);
}
//---------------------------------------------------------------------------
