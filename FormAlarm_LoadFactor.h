//---------------------------------------------------------------------------

#ifndef FormAlarm_LoadFactorH
#define FormAlarm_LoadFactorH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "AdvSmoothButton.hpp"
#include "AdvSmoothPanel.hpp"
//---------------------------------------------------------------------------
class Tloadfactor_AlarmForm : public TForm
{
__published:	// IDE-managed Components
	TAdvSmoothPanel *AdvSmoothPanel1;
	TLabel *errMsg1;
	TLabel *errMsg2;
	TAdvSmoothButton *AdvSmoothButton5;
	TAdvSmoothButton *ignoreBtn;
	void __fastcall FormHide(TObject *Sender);
	void __fastcall AdvSmoothButton5Click(TObject *Sender);
	void __fastcall ignoreBtnClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall Tloadfactor_AlarmForm(TComponent* Owner);

	void __fastcall ShowError(AnsiString str1, AnsiString str2);
};
//---------------------------------------------------------------------------
extern PACKAGE Tloadfactor_AlarmForm *loadfactor_AlarmForm;
//---------------------------------------------------------------------------
#endif
