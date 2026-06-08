//---------------------------------------------------------------------------

#ifndef FormError_mesH
#define FormError_mesH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "AdvSmoothButton.hpp"
#include "AdvSmoothPanel.hpp"
//---------------------------------------------------------------------------
class TErrorForm_mes : public TForm
{
__published:	// IDE-managed Components
	TAdvSmoothPanel *AdvSmoothPanel1;
	TLabel *errMsg1;
	TAdvSmoothButton *ignoreBtn;
	TAdvSmoothButton *retryBtn;
	TLabel *errMsg2;
	TAdvSmoothButton *AdvSmoothButton5;
	void __fastcall AdvSmoothButton5Click(TObject *Sender);
	void __fastcall ignoreBtnClick(TObject *Sender);
	void __fastcall retryBtnClick(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
private:	// User declarations


public:		// User declarations

	void __fastcall ShowError(AnsiString trayid, AnsiString str1, AnsiString str2);

	__fastcall TErrorForm_mes(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TErrorForm_mes *ErrorForm_mes;
//---------------------------------------------------------------------------
#endif
