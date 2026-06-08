//---------------------------------------------------------------------------

#ifndef FormErrorH
#define FormErrorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "AdvSmoothButton.hpp"
//---------------------------------------------------------------------------
class TErrorForm : public TForm
{
__published:	// IDE-managed Components
	TStaticText *MainErr;
	TStaticText *SubErr;
	TStaticText *pTrayId;
	TAdvSmoothButton *manualBtn;
	void __fastcall manualBtnClick(TObject *Sender);
private:	// User declarations

public:		// User declarations
    AnsiString ErrStr;
void __fastcall ShowError(AnsiString MainStr, AnsiString SubStr1="", AnsiString SubStr2 ="");

	__fastcall TErrorForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TErrorForm *ErrorForm;
//---------------------------------------------------------------------------
#endif
