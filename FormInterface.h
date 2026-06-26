//---------------------------------------------------------------------------

#ifndef FormInterfaceH
#define FormInterfaceH
//---------------------------------------------------------------------------
#include "DEFINE.h"
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TInterfaceForm : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *GroupBox_PLC_PC;
	TListView *ListView_PLC;
	TListView *ListView_PC;
	TGroupBox *GroupBox_FMS;
	TListView *ListView_FMS_TAG;
	TListView *ListView_PC_TAG;
	TPanel *pnlPlcTest;
	TPanel *pnlMesTest;
	TLabel *lblPlcTest;
	TLabel *lblFmsTest;
	TGroupBox *gbPlcTest;
	TLabel *lblPlcAddr;
	TLabel *lblPlcValue;
	TComboBox *cbAddress;
	TEdit *editPcValue;
	TButton *btnPlcWriteValue;
	TGroupBox *gbFmsWriteValues;
	TLabel *lblFmsTag;
	TLabel *lblFmsValue;
	TComboBox *cbMesTag;
	TEdit *editFmsValue;
	TButton *btnWriteMesValue;
	TGroupBox *gbFmsIrOcvValue;
	TLabel *lblIrValue;
	TLabel *lblOcvValue;
	TEdit *editIR;
	TEdit *editOCV;
	TGroupBox *gbIrocvNgValue;
	TEdit *editNgList;
	TTimer *Timer_Update;
	void __fastcall GroupBox_PLC_PCDblClick(TObject *Sender);
	void __fastcall btnPlcWriteValueClick(TObject *Sender);
private:	// User declarations
    void __fastcall AddListView(TListView *list, AnsiString address, AnsiString name);
    void __fastcall SetListViewPLC();
	void __fastcall SetListViewPC();

    void __fastcall AddListView(TListView *list, AnsiString tagname);
    void __fastcall SetListViewFMSTAG();
    void __fastcall SetListViewPCTAG();

    // PLC TEST
    void __fastcall WritePcValue();

    TListItem *tempListItem;
public:		// User declarations
	__fastcall TInterfaceForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TInterfaceForm *InterfaceForm;
//---------------------------------------------------------------------------
#endif
