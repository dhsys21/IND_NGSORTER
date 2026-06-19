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
	TPanel *Panel5;
	TGroupBox *GroupBox5;
	TEdit *editRecipe;
	TPanel *Panel6;
	TGroupBox *GroupBox2;
	TCheckBox *chkZAxisUp;
	TGroupBox *GroupBox4;
	TEdit *editPlcPort1;
	TAdvSmoothButton *btnConPLC;
	TAdvSmoothButton *btnDisconPLC;
	TPanel *Panel1;
	TPanel *Panel2;
	TEdit *editPLCIpaddress;
	TPanel *Panel4;
	TEdit *editPlcPort2;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall btnConMesClick(TObject *Sender);
	void __fastcall btnDisconMesClick(TObject *Sender);
	void __fastcall AdvSmoothButton2Click(TObject *Sender);
	void __fastcall AdvSmoothButton3Click(TObject *Sender);
	void __fastcall AdvSmoothButton6Click(TObject *Sender);
	void __fastcall AdvSmoothButton7Click(TObject *Sender);
	void __fastcall AdvSmoothButton9Click(TObject *Sender);
	void __fastcall AdvSmoothButton8Click(TObject *Sender);
	void __fastcall AdvSmoothButton11Click(TObject *Sender);
	void __fastcall AdvSmoothButton10Click(TObject *Sender);
	void __fastcall AdvSmoothButton14Click(TObject *Sender);
	void __fastcall AdvSmoothButton15Click(TObject *Sender);
	void __fastcall AdvSmoothButton16Click(TObject *Sender);
	void __fastcall AdvSmoothButton17Click(TObject *Sender);
	void __fastcall AdvSmoothButton12Click(TObject *Sender);
	void __fastcall AdvSmoothButton13Click(TObject *Sender);
private:	// User declarations
	bool __fastcall ReadSystemInfo();
	void __fastcall ApplyConfig();
    void __fastcall WritePlcData();
public:		// User declarations
    void __fastcall WriteSystemInfo(AnsiString type="");
	__fastcall TConfigForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TConfigForm *ConfigForm;
//---------------------------------------------------------------------------
#endif
