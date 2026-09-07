//---------------------------------------------------------------------------

#ifndef FormError_ejectH
#define FormError_ejectH
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
class TErrorForm_eject : public TForm
{
__published:	// IDE-managed Components
	TAdvSmoothPanel *AdvSmoothPanel1;
	TLabel *errMsg1;
	TLabel *errMsg2;
	TAdvSmoothPanel *AdvSmoothPanel5;
	TAdvSmoothButton *ignoreBtn;
	TAdvSmoothButton *btnManualComplete;
	TAdvSmoothButton *retryBtn;
	TAdvSmoothButton *AdvSmoothButton3;
	TAdvSmoothButton *AdvSmoothButton5;
	TPanel *pnlGripper1;
	TImage *Image1;
	TLabel *CLR1;
	TLabel *lblUpDown;
	TLabel *lblOpenClose;
	TPanel *pdn1;
	TPanel *pup1;
	TPanel *pflow1;
	TPanel *pclose1;
	TPanel *pcell1;
	TPanel *popen1;
	TAdvSmoothButton *AdvSmoothButton12;
	TAdvSmoothButton *AdvSmoothButton11;
	TAdvSmoothButton *btnUp;
	TAdvSmoothButton *btnDown;
	TPanel *Panel40;
	TImage *Image2;
	TLabel *Label14;
	TLabel *Label15;
	TLabel *Label16;
	TPanel *pdn2;
	TPanel *pup2;
	TPanel *pflow2;
	TPanel *pclose2;
	TPanel *pcell2;
	TPanel *popen2;
	TAdvSmoothButton *AdvSmoothButton15;
	TAdvSmoothButton *AdvSmoothButton16;
	TAdvSmoothButton *AdvSmoothButton17;
	TAdvSmoothButton *AdvSmoothButton18;
	TPanel *Panel47;
	TImage *Image3;
	TLabel *Label17;
	TLabel *Label18;
	TLabel *Label19;
	TPanel *pdn3;
	TPanel *pup3;
	TPanel *pflow3;
	TPanel *pclose3;
	TPanel *pcell3;
	TPanel *popen3;
	TAdvSmoothButton *AdvSmoothButton19;
	TAdvSmoothButton *AdvSmoothButton20;
	TAdvSmoothButton *AdvSmoothButton21;
	TAdvSmoothButton *AdvSmoothButton22;
	TPanel *Panel54;
	TImage *Image4;
	TLabel *Label20;
	TLabel *Label21;
	TLabel *Label22;
	TPanel *pdn4;
	TPanel *pup4;
	TPanel *pflow4;
	TPanel *pclose4;
	TPanel *pcell4;
	TPanel *popen4;
	TAdvSmoothButton *AdvSmoothButton23;
	TAdvSmoothButton *AdvSmoothButton24;
	TAdvSmoothButton *AdvSmoothButton25;
	TAdvSmoothButton *AdvSmoothButton26;
	TPanel *Panel61;
	TImage *Image5;
	TLabel *Label23;
	TLabel *Label24;
	TLabel *Label25;
	TPanel *pdn5;
	TPanel *pup5;
	TPanel *pflow5;
	TPanel *pclose5;
	TPanel *pcell5;
	TPanel *popen5;
	TAdvSmoothButton *AdvSmoothButton27;
	TAdvSmoothButton *AdvSmoothButton28;
	TAdvSmoothButton *AdvSmoothButton29;
	TAdvSmoothButton *AdvSmoothButton30;
	TPanel *Panel68;
	TImage *Image6;
	TLabel *Label26;
	TLabel *Label27;
	TLabel *Label28;
	TPanel *pdn6;
	TPanel *pup6;
	TPanel *pflow6;
	TPanel *pclose6;
	TPanel *pcell6;
	TPanel *popen6;
	TAdvSmoothButton *AdvSmoothButton31;
	TAdvSmoothButton *AdvSmoothButton32;
	TAdvSmoothButton *AdvSmoothButton33;
	TAdvSmoothButton *AdvSmoothButton34;
	TPanel *Panel16;
	TLabel *lblTitle;
	TPanel *pcode1;
	TPanel *pnlNgCode;
	TPanel *pnlSourceChannel;
	TPanel *ptarget_ch1;
	TPanel *pnlTargetChannel;
	TPanel *psource_ch1;
	TPanel *ptoolNum;
	TPanel *pnlGripperNo;
	TAdvSmoothButton *btnMoveSource;
	TAdvSmoothButton *btnMoveTarget;
	TMemo *Memo_Ko;
	TMemo *Memo_En;
	TMemo *Memo_Hu;
	void __fastcall retryBtnClick(TObject *Sender);
	void __fastcall ignoreBtnClick(TObject *Sender);
	void __fastcall btnManualCompleteClick(TObject *Sender);
	void __fastcall manualBtnClick(TObject *Sender);
	void __fastcall AdvSmoothButton12Click(TObject *Sender);
	void __fastcall AdvSmoothButton11Click(TObject *Sender);
	void __fastcall btnMoveTargetClick(TObject *Sender);
	void __fastcall btnMoveSourceClick(TObject *Sender);
	void __fastcall AdvSmoothButton5Click(TObject *Sender);
	void __fastcall btnUpClick(TObject *Sender);
	void __fastcall btnDownClick(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
private:	// User declarations
public:		// User declarations

	int toolNum;
	void __fastcall ShowError(AnsiString str1, AnsiString str2, int toolNo, int mesCode);

	__fastcall TErrorForm_eject(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TErrorForm_eject *ErrorForm_eject;
//---------------------------------------------------------------------------
#endif
