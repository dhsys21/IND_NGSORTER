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
	void __fastcall ShowError(AnsiString MainStr, AnsiString SubStr1="", AnsiString SubStr2 ="");

	__fastcall TErrorForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TErrorForm *ErrorForm;

// Safe common entry point: creates ErrorForm if a startup/call-order path has not.
void __fastcall ShowCommonError(AnsiString MainStr, AnsiString SubStr1="", AnsiString SubStr2="");
//---------------------------------------------------------------------------
#endif
