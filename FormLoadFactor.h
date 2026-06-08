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
	TPanel *Panel35;
	TPanel *px1;
	TEdit *Edit_LimitValue;
	TPanel *Panel39;
	TPanel *Panel31;
	TPanel *py;
	TPanel *Panel33;
	TPanel *pz;
	TAdvSmoothButton *AdvSmoothButton_Save;
	TPanel *Panel44;
	TPanel *px2;
	TAdvSmoothButton *AdvSmoothButton_Cancel;
	TMemo *Memo_Ko;
	TMemo *Memo_En;
	TMemo *Memo_Hu;
	TPanel *Panel1;
	TPanel *Panel2;
	TPanel *pg1;
	TPanel *pg2;
	void __fastcall AdvSmoothButton_SaveClick(TObject *Sender);
	void __fastcall AdvSmoothButton_CancelClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TloadfactorForm(TComponent* Owner);

    void __fastcall LanguageChange(int index);

	void __fastcall SaveInfo();
	void __fastcall LoadInfo();

	TPanel *Panel_Position[7];
    int m_SetLimit, m_Count;
};
//---------------------------------------------------------------------------
extern PACKAGE TloadfactorForm *loadfactorForm;
//---------------------------------------------------------------------------
#endif
