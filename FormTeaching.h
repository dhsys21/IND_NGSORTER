//---------------------------------------------------------------------------

#ifndef FormTeachingH
#define FormTeachingH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <jpeg.hpp>
#include <Dialogs.hpp>
#include "AdvSmoothButton.hpp"
#include "AdvSmoothPanel.hpp"
#include "AdvSmoothToggleButton.hpp"
#include <Vcl.ComCtrls.hpp>
#include <DateUtils.hpp>
#include <IniFiles.hpp>

//---------------------------------------------------------------------------
typedef struct{
	AnsiString str_x;
	AnsiString str_y;
	AnsiString str_z;
	AnsiString point;
	int index;
	int total;
	int tag;
}TEACHING;


typedef struct{
	int Top;
	int Bottom;
	int Left;
	int Right;
}TRAY_POSITION;




class TteachForm : public TForm
{
__published:	// IDE-managed Components
	TImage *Image12;
	TAdvSmoothPanel *pnlSourceBase;
	TAdvSmoothPanel *pnlTargetBase;
	TAdvSmoothPanel *t1;
	TEdit *editCh13_TX;
	TEdit *editCh13_TY;
	TPanel *Panel2;
	TPanel *Panel1;
	TPanel *Panel3;
	TEdit *editCh85_SX;
	TEdit *editCh85_SY;
	TPanel *Panel24;
	TPanel *Panel25;
	TPanel *Panel26;
	TPanel *pnlTargetZAxis;
	TEdit *edit_TZ;
	TEdit *edit_SZ;
	TPanel *pnlSourceZAxis;
	TLabel *lblTargetTray;
	TLabel *lblSourceTray;
	TPanel *pselect;
	TAdvSmoothPanel *pnlManualControl;
	TAdvSmoothPanel *AdvSmoothPanel3;
	TAdvSmoothPanel *AdvSmoothPanel2;
	TPanel *pnlXAxis;
	TPanel *px1;
	TEdit *speedEdit;
	TPanel *pnlSpeed;
	TPanel *pnlYAxis;
	TPanel *py;
	TPanel *pnlZAxis;
	TPanel *pz;
	TPanel *pnlSpeedXY;
	TPanel *pnlErrCode;
	TPanel *perr;
	TPanel *pspeed;
	TLabel *lblServoInfo;
	TLabel *lblServoSetting;
	TAdvSmoothPanel *pnlJogControl;
	TLabel *lblJogControl;
	TAdvSmoothPanel *pnlGripperControl;
	TPanel *pnlGripper;
	TImage *Image1;
	TLabel *CLR1;
	TPanel *pdn1;
	TPanel *pup1;
	TPanel *pflow1;
	TPanel *pclose1;
	TPanel *pcell1;
	TPanel *popen1;
	TAdvSmoothButton *btnOpenGripper;
	TAdvSmoothButton *btnCloseGripper;
	TAdvSmoothButton *btnUpGripper;
	TAdvSmoothButton *btnDownGripper;
	TLabel *lblUpDown;
	TLabel *lblOpenClose;
	TCheckBox *disableChk1;
	TPanel *pnlGripper2;
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
	TCheckBox *disableChk2;
	TPanel *pnlGripper3;
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
	TCheckBox *disableChk3;
	TPanel *pnlGripper4;
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
	TCheckBox *disableChk4;
	TPanel *pnlGripper5;
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
	TCheckBox *disableChk5;
	TPanel *pnlGripper6;
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
	TCheckBox *disableChk6;
	TAdvSmoothButton *btnDownAllGripper;
	TAdvSmoothButton *btnUpAllGripper;
	TLabel *lblUpAllGripper;
	TLabel *lblDownAllGripper;
	TAdvSmoothButton *btnOpenAllGripper;
	TAdvSmoothButton *btnCloseAllGripper;
	TLabel *lblOpenAllGripper;
	TLabel *lblCloseAllGripper;
	TTimer *teachingTimer;
	TOpenDialog *OpenDialog;
	TSaveDialog *SaveDialog;
	TAdvSmoothButton *AdvSmoothButton_ServoOff;
	TAdvSmoothButton *waitBtn;
	TPanel *Panel9;
	TPanel *Panel8;
	TPanel *Panel7;
	TEdit *editCh61_TY;
	TEdit *editCh61_TX;
	TEdit *editCh37_TX;
	TEdit *editCh37_TY;
	TPanel *Panel4;
	TPanel *Panel5;
	TPanel *Panel6;
	TPanel *Panel11;
	TPanel *Panel12;
	TPanel *Panel13;
	TEdit *editCh85_TY;
	TEdit *editCh85_TX;
	TEdit *editCh13_SX;
	TEdit *editCh13_SY;
	TPanel *Panel14;
	TPanel *Panel15;
	TPanel *Panel16;
	TEdit *editCh37_SX;
	TEdit *editCh37_SY;
	TPanel *Panel17;
	TPanel *Panel18;
	TPanel *Panel19;
	TEdit *editCh61_SX;
	TEdit *editCh61_SY;
	TPanel *Panel21;
	TPanel *Panel22;
	TPanel *Panel23;
	TAdvSmoothPanel *s1;
	TAdvSmoothButton *openBtn;
	TLabel *lblXAxis;
	TLabel *lblYAxis;
	TLabel *lblZAxis;
	TButton *Button1;
	TButton *Button2;
	TButton *Button3;
	TButton *Button4;
	TButton *Button5;
	TButton *Button6;
	TAdvSmoothButton *AdvSmoothButton_ServoOn;
	TAdvSmoothButton *AdvSmoothButton_Zup;
	TAdvSmoothButton *homeBtn;
	TAdvSmoothButton *stopBtn;
	TAdvSmoothButton *AdvSmoothButton_Reset;
	TPanel *popen;
	TPanel *pnlServo;
	TPanel *pnlOn;
	TPanel *pOnX1;
	TPanel *pOnY;
	TPanel *pOnZ;
	TPanel *pnlHome;
	TPanel *pOrgX1;
	TPanel *pOrgY;
	TPanel *pOrgZ;
	TPanel *pnlError;
	TPanel *pErrorX1;
	TPanel *pErrorY;
	TPanel *pErrorZ;
	TPanel *pnlLimitPlus;
	TPanel *pLspX1;
	TPanel *pLspY;
	TPanel *pLspZ;
	TPanel *pnlLimitMinus;
	TPanel *pLsnX1;
	TPanel *pLsnY;
	TPanel *pLsnZ;
	TLabel *Label33;
	TPanel *pnlSeperator1;
	TMemo *Memo_Ko;
	TMemo *Memo_En;
	TMemo *Memo_Hu;
	TPanel *pnlSettingSpeed;
	TPanel *Panel_speedEdit;
	TAdvSmoothButton *AdvSmoothButton_LoadFactorInfo;
	TPanel *pOnG1;
	TPanel *pErrorG1;
	TPanel *pLsnG1;
	TPanel *pOrgG1;
	TPanel *pLspG1;
	TPanel *pnlGripper1;
	TPanel *pg1;
	TAdvSmoothPanel *AdvSmoothPanel8;
	TLabel *Label50;
	TButton *Button7;
	TButton *Button8;
	TLabel *Label51;
	TPanel *pnlDcc;
	TPanel *pnlAcc;
	TEdit *acclSpeedEdit;
	TEdit *dcclSpeedEdit;
	TLabel *lblLoadFactor4;
	TLabel *lblLoadFactorTitle;
	TLabel *lblLoadFactor1;
	TLabel *Label60;
	TLabel *lblLoadFactor3;
	TLabel *lblLoadFactor2;
	TAdvSmoothButton *btnKeyLock;
	TTimer *unchuckTimer;
	TTimer *unchuckAllTimer;
	TAdvSmoothPanel *pnlMovingAlarm;
	TLabel *Label57;
	TLabel *Label69;
	TAdvSmoothPanel *pnlMovingAlarm2;
	TLabel *Label59;
	TLabel *Label61;
	TAdvSmoothButton *btnClose;
	TPanel *Panel20;
	TPanel *Panel30;
	TPanel *Panel44;
	TEdit *editCh01_SX;
	TEdit *editCh01_SY;
	TPanel *Panel48;
	TPanel *Panel49;
	TPanel *Panel50;
	TEdit *editCh25_SX;
	TEdit *editCh25_SY;
	TPanel *Panel51;
	TPanel *Panel52;
	TPanel *Panel53;
	TEdit *editCh49_SX;
	TEdit *editCh49_SY;
	TPanel *Panel55;
	TPanel *Panel56;
	TPanel *Panel57;
	TEdit *editCh73_SX;
	TEdit *editCh73_SY;
	TPanel *Panel58;
	TPanel *Panel59;
	TPanel *Panel60;
	TEdit *editCh01_TX;
	TEdit *editCh01_TY;
	TPanel *Panel62;
	TPanel *Panel63;
	TPanel *Panel64;
	TEdit *editCh25_TX;
	TEdit *editCh25_TY;
	TPanel *Panel65;
	TPanel *Panel66;
	TPanel *Panel67;
	TEdit *editCh49_TX;
	TEdit *editCh49_TY;
	TPanel *Panel70;
	TPanel *Panel71;
	TPanel *Panel72;
	TEdit *editCh73_TX;
	TEdit *editCh73_TY;
	TPanel *pnlSeperator2;
	TComboBox *sCombo;
	TPanel *pnlSelectGripper;
	TLabel *lblMsgRobot;
	TAdvSmoothButton *btnApplyTeaching;
	TAdvSmoothPanel *pnlBackground;
	TAdvSmoothButton *AdvSmoothButton1;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall sClick(TObject *Sender);
	void __fastcall tClick(TObject *Sender);
	void __fastcall AdvSmoothButton_ResetClick(TObject *Sender);
	void __fastcall AdvSmoothButton_ServoOffClick(TObject *Sender);
	void __fastcall btnUpGripperClick(TObject *Sender);
	void __fastcall btnDownGripperClick(TObject *Sender);
	void __fastcall btnOpenGripperClick(TObject *Sender);
	void __fastcall btnCloseGripperClick(TObject *Sender);
	void __fastcall btnUpAllGripperClick(TObject *Sender);
	void __fastcall btnDownAllGripperClick(TObject *Sender);
	void __fastcall btnOpenAllGripperClick(TObject *Sender);
	void __fastcall btnCloseAllGripperClick(TObject *Sender);
	void __fastcall speedEditKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall disableChk1Click(TObject *Sender);
	void __fastcall waitBtnClick(TObject *Sender);
	void __fastcall openBtnClick(TObject *Sender);
	void __fastcall Button1MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
		  int X, int Y);
	void __fastcall Button2MouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall Button1MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
		  int X, int Y);
	void __fastcall AdvSmoothButton_ServoOnClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall AdvSmoothButton_ZupClick(TObject *Sender);
	void __fastcall homeBtnClick(TObject *Sender);
	void __fastcall stopBtnClick(TObject *Sender);
	void __fastcall AdvSmoothButton_LoadFactorInfoClick(TObject *Sender);
	void __fastcall btnApplyTeachingClick(TObject *Sender);
	void __fastcall btnZAxisDownMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall btnZAxisDownMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall btnZAxisUpMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall btnZAxisUpMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall btnKeyLockClick(TObject *Sender);
	void __fastcall btnOpenGripperMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall btnOpenGripperMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall unchuckTimerTimer(TObject *Sender);
	void __fastcall btnOpenAllGripperMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall btnOpenAllGripperMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
          int X, int Y);
	void __fastcall unchuckAllTimerTimer(TObject *Sender);
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall AdvSmoothButton1Click(TObject *Sender);
private:	// User declarations
    TListItem	*ITEM;
	TAdvSmoothPanel *sTray[96];
	TAdvSmoothPanel *tTray[96];

	TEACHING teach;
    AnsiString teachingFilePath;
    void __fastcall MakePanel();
	void __fastcall ApplyTeaching();
    void __fastcall SaveTeaching(AnsiString filePath);
	void __fastcall LoadTeaching(AnsiString filePath);
    void __fastcall SetTrayMaxPosition();

	bool __fastcall CheckPositionDown(int gripperIndex);
	bool __fastcall CheckUnchuckPosition(int gripperIndex);
    bool __fastcall CheckChuckPosition(int gripperIndex);
    bool __fastcall CheckMoveTargetChannel(int channel);
    bool __fastcall CheckMoveSourceChannel();
    //* 마우스 버튼 3초이상 클릭시 UnCheck
    TDateTime downTime;
	bool isButtonPressed;
    int nCurrentTag;
public:		// User declarations
    TLabel *lblLoadFactor[7];
    void __fastcall zdown();
    void __fastcall zup();

	TRAY_POSITION sTray_Position, tTray_Position;
    TEdit* __fastcall GetTrayEdit(int channel, TrayAxisEdit editType);
    int __fastcall GetTrayPosValue(int channel, TrayAxisEdit editType);
	bool isGripperOpen1;
    bool isGripperOpen2;

	__fastcall TteachForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TteachForm *teachForm;
//---------------------------------------------------------------------------
#endif
