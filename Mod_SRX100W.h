//---------------------------------------------------------------------------

#ifndef Mod_SRX100WH
#define Mod_SRX100WH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <System.Win.ScktComp.hpp>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TMod_Bcr : public TDataModule
{
__published:	// IDE-managed Components
	TClientSocket *ClientSocketBcr;
	TTimer *Timer1;
	TTimer *Timer_AutoConnect;
	void __fastcall ClientSocketBcrConnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall ClientSocketBcrDisconnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall ClientSocketBcrError(TObject *Sender, TCustomWinSocket *Socket,
          TErrorEvent ErrorEvent, int &ErrorCode);
	void __fastcall Timer_AutoConnectTimer(TObject *Sender);
	void __fastcall ClientSocketBcrRead(TObject *Sender, TCustomWinSocket *Socket);

private:	// User declarations
	bool bClose;
	bool bReading;
	AnsiString rxBuffer;
	void __fastcall ReadTimeoutTimer(TObject *Sender);
	void __fastcall ProcessResult(AnsiString data);
	AnsiString __fastcall ReaderName();

public:		// User declarations
	__fastcall TMod_Bcr(TComponent* Owner);
	void __fastcall Connect();
	void __fastcall Connect(AnsiString IpAddress, int Port);
	void __fastcall Disconnect();
	void __fastcall TriggerOn();
	void __fastcall TriggerOff();
	void __fastcall GetBarcode();
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#endif
