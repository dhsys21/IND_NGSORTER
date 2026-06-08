//---------------------------------------------------------------------------

#ifndef FormError_limitH
#define FormError_limitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "AdvSmoothButton.hpp"
#include "AdvSmoothPanel.hpp"
//---------------------------------------------------------------------------
class TErrorForm_limit : public TForm
{
__published:	// IDE-managed Components
	TAdvSmoothPanel *AdvSmoothPanel1;
	TLabel *errMsg1;
	TAdvSmoothButton *ignoreBtn;
	TAdvSmoothButton *AdvSmoothButton1;
	TPanel *pon;
	TLabel *errMsg2;
	TAdvSmoothButton *AdvSmoothButton5;
	void __fastcall ignoreBtnClick(TObject *Sender);
	void __fastcall AdvSmoothButton1Click(TObject *Sender);
	void __fastcall AdvSmoothButton5Click(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
private:	// User declarations

	AnsiString strBcr;

public:		// User declarations

	void __fastcall ShowError();

	__fastcall TErrorForm_limit(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TErrorForm_limit *ErrorForm_limit;
//---------------------------------------------------------------------------
#endif
