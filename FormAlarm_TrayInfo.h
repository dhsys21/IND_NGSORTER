//---------------------------------------------------------------------------

#ifndef FormAlarm_TrayInfoH
#define FormAlarm_TrayInfoH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "AdvSmoothButton.hpp"
#include "AdvSmoothPanel.hpp"
//---------------------------------------------------------------------------
class TtrayinfoForm : public TForm
{
__published:	// IDE-managed Components
	TAdvSmoothPanel *AdvSmoothPanel1;
	TLabel *errMsg1;
	TLabel *errMsg2;
	TAdvSmoothButton *AdvSmoothButton5;
	TAdvSmoothButton *ignoreBtn;
	TAdvSmoothButton *AdvSmoothButton1;
	void __fastcall AdvSmoothButton5Click(TObject *Sender);
	void __fastcall ignoreBtnClick(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
	void __fastcall AdvSmoothButton1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TtrayinfoForm(TComponent* Owner);

	void __fastcall ShowError(AnsiString str1, AnsiString str2, AnsiString trayid, int index);

	AnsiString m_LOT_ID;
	int m_Index;
};
//---------------------------------------------------------------------------
extern PACKAGE TtrayinfoForm *trayinfoForm;
//---------------------------------------------------------------------------
#endif
