//---------------------------------------------------------------------------

#ifndef FormDoorH
#define FormDoorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <jpeg.hpp>
#include "AdvSmoothButton.hpp"
#include "AdvSmoothPanel.hpp"
//---------------------------------------------------------------------------
class TdoorForm : public TForm
{
__published:	// IDE-managed Components
	TAdvSmoothPanel *AdvSmoothPanel1;
	TAdvSmoothPanel *AdvSmoothPanel2;
	TLabel *Label3;
	TLabel *Label1;
	TStaticText *MainErr1;
	TAdvSmoothButton *okBtn;
	TAdvSmoothButton *AdvSmoothButton4;
	TStaticText *MainErr2;
	TStaticText *MainErr3;
	TPanel *perr2;
	TPanel *perr3;
	TTimer *errTimer;
	TPanel *perr1;
	TPanel *perr4;
	TAdvSmoothPanel *AdvSmoothPanel3;
	TLabel *Label2;
	TStaticText *MainErr4;
	TAdvSmoothPanel *AdvSmoothPanel4;
	TLabel *Label4;
	TStaticText *StaticText1;
	TStaticText *StaticText3;
	TStaticText *StaticText2;
	TImage *Image1;
	TAdvSmoothPanel *AdvSmoothPanel5;
	TLabel *Label5;
	TPanel *perr5;
	TAdvSmoothButton *btnSetKEYLOCK;
	TImage *Image2;
	TPanel *Panel2;
	TStaticText *MainErr5;
	TAdvSmoothButton *btnServoOpen;
	TStaticText *StaticText4;
	TAdvSmoothPanel *pnlOpenGripper;
	TLabel *Label6;
	TAdvSmoothButton *AdvSmoothButton12;
	TAdvSmoothButton *AdvSmoothButton1;
	TAdvSmoothButton *stopBtn;
	TAdvSmoothButton *btnKeyUnlock;
	TPanel *pPassword;
	TPanel *Panel61;
	TEdit *PassEdit;
	TAdvSmoothButton *cancelBtn2;
	TAdvSmoothButton *PasswordBtn;
	void __fastcall AdvSmoothButton4Click(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
	void __fastcall errTimerTimer(TObject *Sender);
	void __fastcall okBtnClick(TObject *Sender);
	void __fastcall btnSetKEYLOCKClick(TObject *Sender);
	void __fastcall btnServoOpenClick(TObject *Sender);
	void __fastcall stopBtnClick(TObject *Sender);
	void __fastcall AdvSmoothButton12MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall AdvSmoothButton12MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall AdvSmoothButton1MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall btnKeyUnlockClick(TObject *Sender);
	void __fastcall Label3DblClick(TObject *Sender);
	void __fastcall cancelBtn2Click(TObject *Sender);
	void __fastcall PasswordBtnClick(TObject *Sender);
private:	// User declarations
	TStaticText *text[5];
	TPanel *perr[5];


	int m_errCode;
public:		// User declarations
	void __fastcall ShowError(AnsiString MainStr, AnsiString SubStr, int errCode);
	int nCode;
	bool isDoorOpen;
	bool isGripperOpen1;
    bool isGripperOpen2;

    bool flag;
	__fastcall TdoorForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TdoorForm *doorForm;
//---------------------------------------------------------------------------
#endif
