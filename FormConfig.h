//---------------------------------------------------------------------------

#ifndef FormConfigH
#define FormConfigH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <jpeg.hpp>
#include <Buttons.hpp>
#include <Menus.hpp>
#include "AdvSmoothButton.hpp"
#include "AdvSmoothLabel.hpp"
//---------------------------------------------------------------------------
class TConfigForm : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *GroupBox1;
	TGroupBox *GroupBox3;
	TEdit *pcEdit;
	TEdit *PortEdit;
	TAdvSmoothButton *btnConMes;
	TAdvSmoothButton *btnDisconMes;
	TImage *Image12;
	TAdvSmoothLabel *AdvSmoothLabel1;
	TAdvSmoothButton *AdvSmoothButton2;
	TAdvSmoothButton *AdvSmoothButton3;
	TPanel *Panel3;
	TPanel *pnlFmsIp;
	TEdit *editFmsIp;
	TPanel *Panel5;
	TGroupBox *GroupBox5;
	TEdit *editRecipe;
	TPanel *Panel6;
	TGroupBox *GroupBox2;
	TCheckBox *chkZAxisUp;
	//* max speed mode - need remove
	TCheckBox *chkMaximumSpeedMode;
	TCheckBox *chkOptimizeSequenceDelay;
	TCheckBox *chkSkipGripStabilization;
	TCheckBox *chkUseFatTestBarcodes;
	TGroupBox *GroupBoxFatTest;
	TGroupBox *GroupBoxTargetUnload;
	TLabel *lblTargetUnloadCount;
	TLabel *lblTargetUnloadDisabled;
	TEdit *editTargetUnloadCount;
	TEdit *editFatSourceBcr;
	TEdit *editFatTargetBcr;
	TPanel *pnlFatSourceBcr;
	TPanel *pnlFatTargetBcr;
	TGroupBox *GroupBox4;
	TEdit *editPlcPort1;
	TAdvSmoothButton *btnConPLC;
	TAdvSmoothButton *btnDisconPLC;
	TPanel *Panel1;
	TPanel *Panel2;
	TEdit *editPLCIpaddress;
	TPanel *Panel4;
	TEdit *editPlcPort2;
	TGroupBox *GroupBoxBcr;
	TPanel *pnlBcrSourceIp;
	TPanel *pnlBcrSourcePort;
	TPanel *pnlBcrTargetIp;
	TPanel *pnlBcrTargetPort;
	TEdit *editBcrSourceIp;
	TEdit *editBcrSourcePort;
	TEdit *editBcrTargetIp;
	TEdit *editBcrTargetPort;
	TButton *btnBcrSourceConn;
	TButton *btnBcrSourceDisconn;
	TButton *btnBcrTargetConn;
	TButton *btnBcrTargetDisconn;
	TGroupBox *GroupBoxSmoke;
	TPanel *pnlSmokePort;
	TPanel *pnlSmokeId;
	TPanel *pnlSmokeMode;
	TPanel *pnlSmokeBaud;
	TEdit *editSmokePort;
	TEdit *editSmokeId;
	TEdit *editSmokeMode;
	TEdit *editSmokeBaud;
	TButton *btnSmokeConn;
	TButton *btnSmokeDisconn;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall btnConMesClick(TObject *Sender);
	void __fastcall btnDisconMesClick(TObject *Sender);
	void __fastcall AdvSmoothButton2Click(TObject *Sender);
	void __fastcall AdvSmoothButton3Click(TObject *Sender);
private:	// User declarations
	void __fastcall CreateCommunicationControls();
	TPanel* __fastcall AddFieldLabel(TWinControl *Parent, int Left, int Top, int Width, AnsiString Caption);
	TEdit* __fastcall AddFieldEdit(TWinControl *Parent, int Left, int Top, int Width, AnsiString Text);
	TButton* __fastcall AddActionButton(TWinControl *Parent, int Left, int Top, int Width, AnsiString Caption, TNotifyEvent OnClick);
	void __fastcall LoadCommunicationEdits();
	void __fastcall UpdateCommunicationConfigFromEdits();
	void __fastcall btnPlcConnClick(TObject *Sender);
	void __fastcall btnPlcDisconnClick(TObject *Sender);
	void __fastcall btnBcrSourceConnClick(TObject *Sender);
	void __fastcall btnBcrSourceDisconnClick(TObject *Sender);
	void __fastcall btnBcrTargetConnClick(TObject *Sender);
	void __fastcall btnBcrTargetDisconnClick(TObject *Sender);
	void __fastcall btnSmokeConnClick(TObject *Sender);
	void __fastcall btnSmokeDisconnClick(TObject *Sender);
	bool __fastcall ReadSystemInfo();
	void __fastcall ApplyConfig();
public:		// User declarations
    void __fastcall WriteSystemInfo(AnsiString type="");
	__fastcall TConfigForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TConfigForm *ConfigForm;
//---------------------------------------------------------------------------
#endif
