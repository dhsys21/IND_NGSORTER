//---------------------------------------------------------------------------

#ifndef FormServoAlarmListH
#define FormServoAlarmListH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
//---------------------------------------------------------------------------
class TServoAlarmListForm : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *GroupBox1;
	TListView *ListView1;
	TGroupBox *GroupBox2;
	TListView *ListView2;
	TGroupBox *GroupBox3;
	TListView *ListView3;
	TGroupBox *GroupBox4;
	TListView *ListView4;
	TMemo *Memo_Ko1;
	TMemo *Memo_En1;
	TMemo *Memo_Hu1;
	TMemo *Memo_Ko2;
	TMemo *Memo_En2;
	TMemo *Memo_Hu2;
	TMemo *Memo_Ko3;
	TMemo *Memo_En3;
	TMemo *Memo_Hu3;
	TMemo *Memo_Ko4;
	TMemo *Memo_En4;
	TMemo *Memo_Hu4;
	void __fastcall FormCreate(TObject *Sender);
private:	// User declarations

	TListView *m_ListView[4];
	TListItem	*ITEM;
    int m_Length[4];

public:		// User declarations
	__fastcall TServoAlarmListForm(TComponent* Owner);

    void __fastcall LanguageChange(int index);
};
//---------------------------------------------------------------------------
extern PACKAGE TServoAlarmListForm *ServoAlarmListForm;
//---------------------------------------------------------------------------
#endif
