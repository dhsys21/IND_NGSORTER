//---------------------------------------------------------------------------

#ifndef FormAlarmH
#define FormAlarmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "AdvSmoothButton.hpp"
#include "AdvSmoothPanel.hpp"
//---------------------------------------------------------------------------
class TAlarmForm : public TForm
{
__published:	// IDE-managed Components
	TAdvSmoothPanel *AdvSmoothPanel1;
	TLabel *errMsg1;
	TLabel *errMsg2;
	TAdvSmoothButton *AdvSmoothButton5;
	TAdvSmoothButton *ignoreBtn;
	void __fastcall ignoreBtnClick(TObject *Sender);
	void __fastcall AdvSmoothButton5Click(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
private:	// User declarations
public:		// User declarations

	void __fastcall ShowError(AnsiString str1, UnicodeString str2);

	__fastcall TAlarmForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TAlarmForm *AlarmForm;
//---------------------------------------------------------------------------
#endif
