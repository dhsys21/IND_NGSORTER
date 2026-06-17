//---------------------------------------------------------------------------

#ifndef FormLoadFactorH
#define FormLoadFactorH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "AdvSmoothButton.hpp"
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TloadfactorForm : public TForm
{
__published:	// IDE-managed Components
	TPanel *pnlXAxis;
	TPanel *px1;
	TEdit *Edit_LimitValue;
	TPanel *pnlLimitValue;
	TPanel *pnlYAxis;
	TPanel *py;
	TPanel *pnlZAxis;
	TPanel *pz;
	TAdvSmoothButton *AdvSmoothButton_Save;
	TAdvSmoothButton *AdvSmoothButton_Cancel;
	TMemo *Memo_Ko;
	TMemo *Memo_En;
	TMemo *Memo_Hu;
	void __fastcall AdvSmoothButton_SaveClick(TObject *Sender);
	void __fastcall AdvSmoothButton_CancelClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TloadfactorForm(TComponent* Owner);

    void __fastcall LanguageChange(int index);

	void __fastcall SaveInfo();
	void __fastcall LoadInfo();

	TPanel *Panel_Position[4];
    int m_SetLimit, m_Count;
};
//---------------------------------------------------------------------------
extern PACKAGE TloadfactorForm *loadfactorForm;
//---------------------------------------------------------------------------
#endif
