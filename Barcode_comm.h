//---------------------------------------------------------------------------

#ifndef Barcode_commH
#define Barcode_commH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "CPort.hpp"
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------



class TBarcode : public TDataModule
{
__published:	// IDE-managed Components
	TComPort *Comm;
	TTimer *chkTimer;
	TTimer *Timer1;
	void __fastcall chkTimerTimer(TObject *Sender);
	void __fastcall CommRxFlag(TObject *Sender);

private:	// User declarations

	int retryCnt;
	int tryCnt;
	int retryInterval;
public:		// User declarations

    void __fastcall setBarcodeInfo(int cnt, int interval);
	void __fastcall CommOpen(AnsiString port, int sep);
	void __fastcall GetBarcode();


	__fastcall TBarcode(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TBarcode *Barcode;
//---------------------------------------------------------------------------
#endif
