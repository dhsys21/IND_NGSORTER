//---------------------------------------------------------------------------

#ifndef FormError_bcrH
#define FormError_bcrH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "AdvSmoothButton.hpp"
#include "AdvSmoothPanel.hpp"
//---------------------------------------------------------------------------
class TErrorForm_bcr : public TForm
{
__published:	// IDE-managed Components
	TAdvSmoothPanel *AdvSmoothPanel1;
	TLabel *errMsg1;
	TAdvSmoothButton *ignoreBtn;
	TAdvSmoothButton *btnScan;
	TPanel *pon;
	TAdvSmoothButton *AdvSmoothButton5;
	void __fastcall ignoreBtnClick(TObject *Sender);
	void __fastcall btnScanClick(TObject *Sender);
	void __fastcall AdvSmoothButton5Click(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
private:	// User declarations

	AnsiString strBcr;

public:		// User declarations

	void __fastcall ShowError(AnsiString str, bool bsuccess);

	__fastcall TErrorForm_bcr(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TErrorForm_bcr *ErrorForm_bcr;
//---------------------------------------------------------------------------
#endif
