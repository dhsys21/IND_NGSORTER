//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CPort"
#pragma resource "*.dfm"
TBarcode *Barcode;
//---------------------------------------------------------------------------
__fastcall TBarcode::TBarcode(TComponent* Owner)
	: TDataModule(Owner)
{
	retryCnt = 3;
	retryInterval = 3000;
}
//---------------------------------------------------------------------------
void __fastcall TBarcode::CommOpen(AnsiString port, int sep)
{
	try{
		if(Comm->Connected){
			Comm->Close();
		}
    	Tag = sep;
		Comm->Port = port;
		Comm->BaudRate = br9600;
		Comm->EventChar = 0x0a;
		Comm->Open();
		Comm->ClearBuffer(true, true);

	}
	catch(...){
		Comm->Close();
		AlarmForm->ShowError("Barcode COM Port", "Can not open " + port + " port.");
	}
}
/**********************************************************/
//														   //
//				바코드 통신    					        //
//														   //
//														   //
//*********************************************************//
//---------------------------------------------------------------------------
void __fastcall TBarcode::GetBarcode()
{
	if(Comm->Connected){
		char stx = 0x02;
		Comm->Write(&stx, 1);
		if(ErrorForm_bcr->Visible){
			chkTimer->Interval = 60000;
			tryCnt = retryCnt;
		}else{
			chkTimer->Interval = retryInterval;
			tryCnt = 0;
		}
		chkTimer->Enabled = true;
	}
	else{
		AlarmForm->ShowError("Barcode COM Port", "Can not open " + Comm->Port + " port.");
	}
}
//---------------------------------------------------------------------------
void __fastcall TBarcode::setBarcodeInfo(int cnt, int interval)
{
	retryCnt = cnt;
	retryInterval = interval;

}
//---------------------------------------------------------------------------
void __fastcall TBarcode::chkTimerTimer(TObject *Sender)
{
	char etx = 0x03;
	Comm->Write(&etx, 1);

	tryCnt += 1;

	if(tryCnt >= retryCnt){
		chkTimer->Enabled = false;
		ErrorForm_bcr->Tag = Tag;
		switch(Tag){
			case 0:
				ErrorForm_bcr->ShowError("Source Tray barcode can not be scanned.", false);
				break;
			case 1:
				ErrorForm_bcr->ShowError("Target Tray barcode can not be scanned.", false);
				break;
		}
	}

}
//---------------------------------------------------------------------------

void __fastcall TBarcode::CommRxFlag(TObject *Sender)
{
	UnicodeString sRxstr;
	sRxstr = "";
	char etx = 0x03;
	char stx = 0x02;
	int cnt = Comm->InputCount();
	Comm->ReadStr(sRxstr, cnt);

	if(sRxstr[1] == 0x02 && sRxstr[sRxstr.Length()] == 0x0a){
		if(sRxstr.Length() > 9){
			chkTimer->Enabled = false;
			sRxstr = sRxstr.SubString(2,7);

			if(ErrorForm_bcr->Visible)ErrorForm_bcr->ShowError(sRxstr, true);
			else MainForm->setBarcode(Tag, sRxstr);

			Comm->Write(&etx, 1);
		}else{
			MainForm->setBarcode(Tag, tryCnt);
			if(tryCnt < retryCnt){
				Comm->Write(&stx, 1);
			}
		}
	}

}
//---------------------------------------------------------------------------
