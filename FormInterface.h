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
	TLabel *lblTrackOutChannels;
	TEdit *editNgList;
	TButton *btnSourceTrayLoad;
	TButton *btnTargetTrayLoad;
	TTimer *Timer_PLC_Update;
	TTimer *Timer_MES_Update;
	TGroupBox *GroupBox1;
	TButton *btnFmsTest01STrayLoad;
	TButton *btnFmsTest04TTrayLoad;
	TButton *btnFmsTest02ProcessStart;
	TButton *btnFmsTest03ProcessEnd;
	TButton *btnFmsTest05TTrayUnload;
	void __fastcall GroupBox_PLC_PCDblClick(TObject *Sender);
	void __fastcall btnPlcWriteValueClick(TObject *Sender);
	void __fastcall Timer_PLC_UpdateTimer(TObject *Sender);
	void __fastcall btnWriteMesValueClick(TObject *Sender);
	void __fastcall cbMesTagChange(TObject *Sender);
	void __fastcall Timer_MES_UpdateTimer(TObject *Sender);
	void __fastcall ListViewPCTagClick(TObject *Sender);
	void __fastcall btnSourceTrayLoadClick(TObject *Sender);
	void __fastcall btnTargetTrayLoadClick(TObject *Sender);
	// =====================================================================
	// FMS TEST - REMOVE THIS BLOCK AFTER FMS COMMISSIONING
	void __fastcall btnFmsTest01STrayLoadClick(TObject *Sender);
	void __fastcall btnFmsTest02ProcessStartClick(TObject *Sender);
	void __fastcall btnFmsTest03ProcessEndClick(TObject *Sender);
	void __fastcall btnFmsTest04TTrayLoadClick(TObject *Sender);
	void __fastcall btnFmsTest05TTrayUnloadClick(TObject *Sender);
	// END FMS TEST - REMOVE THIS BLOCK AFTER FMS COMMISSIONING
	// =====================================================================
private:	// User declarations
    void __fastcall AddListView(TListView *list, AnsiString address, AnsiString name);
    void __fastcall SetListViewPLC();
	void __fastcall SetListViewPC();

    void __fastcall AddListView(TListView *list, AnsiString tagname);
	void __fastcall AddListView(TListView *list, UnicodeString tagname, UnicodeString datatype, UnicodeString value);
    void __fastcall SetListViewFMSTAG();
    void __fastcall SetListViewPCTAG();
	void __fastcall SetupMesTestControls();
	void __fastcall RefreshMesTagLists();
	void __fastcall RefreshPcTagCombo();
	bool __fastcall IsSameListLayout(TListView *list, TStrings *Rows);
	void __fastcall SyncListViewRows(TListView *list, TStrings *Rows);
	void __fastcall UpdateListViewValues(TListView *list, TStrings *Rows);
	void __fastcall UpdateMesValueEditor();
	void __fastcall WriteMesValue();
	void __fastcall WriteTrackOutCellInformationTest();
	void __fastcall WriteCellTrackOutTest();
	UnicodeString __fastcall FindTrackInCellId(int SourceCellNo);
	bool __fastcall IsTrackOutTestChannel(int Channel);
	UnicodeString __fastcall GetRowPart(const UnicodeString &Row, int Index);
	UnicodeString __fastcall BuildJsonValue(const UnicodeString &DataType, const UnicodeString &Text);
	bool __fastcall CanRunMesTest();

	// =====================================================================
	// FMS TEST - REMOVE THIS BLOCK AFTER FMS COMMISSIONING
	bool __fastcall GetFmsTestBool(const UnicodeString &Tag);
	void __fastcall FlushFmsTest(const AnsiString &Action, bool Value);
	// END FMS TEST - REMOVE THIS BLOCK AFTER FMS COMMISSIONING
	// =====================================================================

    // PLC TEST
    void __fastcall WritePcValue();

	TListItem *tempListItem;
	TComboBox *cbMesBoolValue;
	TLabel *lblFmsDataType;
public:		// User declarations
	__fastcall TInterfaceForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TInterfaceForm *InterfaceForm;
//---------------------------------------------------------------------------
#endif
