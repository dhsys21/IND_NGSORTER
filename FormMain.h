//---------------------------------------------------------------------------

#ifndef FormMainH
#define FormMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <IniFiles.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <jpeg.hpp>
#include <Menus.hpp>
#include <Graphics.hpp>
#include <Dialogs.hpp>
#include <Grids.hpp>
#include <System.Win.ScktComp.hpp>
#include <System.Classes.hpp>
#include "AdvSmoothButton.hpp"
#include "AdvSmoothPanel.hpp"
#include "AdvSmoothToggleButton.hpp"

//---------------------------------------------------------------------------
#include "DEFINE.h"
#include "Barcode_comm.h"
#include "Mod_SRX100W.h"
#include "SmokeDetector_comm.h"


typedef enum Mode
{
	modeInit,
	modeAuto,
	modeAutoStop,
	modeManual,
	modeEmergency,
	modeReset,
    modAlarm
}SorterMode;

typedef enum
{
	LampAuto,
	LampManual,
	LampEmergency,
	LampAlarm,
    LampStop
}LampMode;


typedef struct
{
	AnsiString LOT_ID;
	int SLOT_COUNT;
	AnsiString SLOT_POSITION[96];
	AnsiString SLOT_ID[96];
	AnsiString PICK[96];
}SAVE_TRAY_INFO;



class TMainForm : public TForm
{
__published:	// IDE-managed Components
	TClientSocket *Client;
	TLabel *Label6;
	TAdvSmoothPanel *pback;
	TLabel *lblTitle;
	TAdvSmoothPanel *AdvSmoothPanel2;
	TLabel *lblSourceTrayInfo;
	TPanel *Panel59;
	TPanel *pTrayid_source2;
	TPanel *pPROCESS;
	TPanel *pKIND;
	TPanel *pDATE;
	TPanel *pnlSourceTrayId;
	TPanel *pnlSouceOper;
	TPanel *pnlSourceProcess;
	TPanel *pSLOT_COUNT;
	TPanel *pnlSourceKind;
	TPanel *pnlSourceSlotCount;
	TAdvSmoothPanel *AdvSmoothPanel3;
	TPanel *popen;
	TAdvSmoothPanel *pnlLog;
	TAdvSmoothPanel *pnlTargetTray;
	TAdvSmoothPanel *pnlSourceTray;
	TPanel *pBase;
	TAdvSmoothPanel *pnlGripper;
	TPanel *Panel27;
	TSaveDialog *SaveDialog;
	TPanel *pOPER;
	TPanel *pnlSourceDate;
	TOpenDialog *OpenDialog;
	TAdvSmoothToggleButton *manualBtn;
	TAdvSmoothToggleButton *autoBtn;
	TAdvSmoothToggleButton *stopBtn;
	TAdvSmoothToggleButton *playBtn;
	TAdvSmoothToggleButton *pause_stopBtn;
	TAdvSmoothToggleButton *buzzerBtn;
	TCheckBox *chkBypass;
	TLabel *Label7;
	TPanel *pnlErrCode;
	TPanel *perr;
	TAdvSmoothPanel *AdvSmoothPanel1;
	TLabel *lblNgList;
	TPanel *Panel6;
	TPanel *pbad_sum;
	TPanel *pnlNgQuantity;
	TAdvSmoothButton *trayout_targetBtn;
	TAdvSmoothButton *teachingBtn;
	TLabel *lblManualControl;
	TAdvSmoothPanel *AdvSmoothPanel5;
	TPanel *Panel14;
	TPanel *Panel16;
	TImage *Image1;
	TPanel *pdn1;
	TPanel *pup1;
	TPanel *pflow1;
	TPanel *pclose1;
	TPanel *pcell1;
	TPanel *popen1;
	TLabel *CLR1;
	TPanel *pcode1;
	TPanel *pnlCode;
	TPanel *pnlSource;
	TPanel *ptarget_ch1;
	TPanel *pnlTarget;
	TPanel *psource_ch1;
	TPanel *Panel32;
	TImage *Image4;
	TLabel *CLR2;
	TPanel *pdn2;
	TPanel *pup2;
	TPanel *pflow2;
	TPanel *pclose2;
	TPanel *pcell2;
	TPanel *popen2;
	TPanel *pcode2;
	TPanel *Panel42;
	TPanel *Panel43;
	TPanel *ptarget_ch2;
	TPanel *Panel45;
	TPanel *psource_ch2;
	TStringGrid *targetGrid;
	TPanel *pnlCh;
	TPanel *pnlNgCode;
	TPanel *pnlSource2;
	TPanel *pTrayid_target;
	TPanel *pnlTargetTrayTitle;
	TPanel *pTrayid_source;
	TPanel *pnlSourceTrayTitle;
	TLabel *lblTargetTrayInfo;
	TPanel *pTrayid_target2;
	TPanel *pPROCESS_target;
	TPanel *pKIND_target;
	TPanel *pDATE_target;
	TPanel *pnlTargetTrayId;
	TPanel *pnlTargetProcess;
	TPanel *pSLOT_COUNT_target;
	TPanel *pnlTargetKind;
	TPanel *pnlTargetSlotCount;
	TPanel *pnlTargetDate;
	TPanel *pBYPASS;
	TPanel *pnlSourceBypass;
	TAdvSmoothPanel *pt1;
	TAdvSmoothPanel *pt2;
	TAdvSmoothPanel *pt3;
	TAdvSmoothPanel *pt20;
	TAdvSmoothPanel *pt17;
	TAdvSmoothPanel *pt18;
	TAdvSmoothPanel *pt19;
	TAdvSmoothPanel *pt16;
	TAdvSmoothPanel *pt13;
	TAdvSmoothPanel *pt14;
	TAdvSmoothPanel *pt15;
	TAdvSmoothPanel *pt12;
	TAdvSmoothPanel *pt9;
	TAdvSmoothPanel *pt10;
	TAdvSmoothPanel *pt11;
	TAdvSmoothPanel *pt8;
	TAdvSmoothPanel *pt5;
	TAdvSmoothPanel *pt6;
	TAdvSmoothPanel *pt7;
	TAdvSmoothPanel *pt4;
	TAdvSmoothPanel *pt21;
	TAdvSmoothPanel *pt22;
	TAdvSmoothPanel *pt23;
	TAdvSmoothPanel *pt24;
	TListView *badList;
	TLabel *puse1;
	TLabel *puse2;
	TPanel *pinsertremainCnt;
	TPanel *pnlTargetRemaining;
	TPanel *px1;
	TPanel *py;
	TPanel *pz;
	TPanel *pspeed;
	TLabel *lblZPos;
	TLabel *lblYPos;
	TLabel *lblXPos;
	TLabel *lblSpeedXY;
	TTimer *stepTimer;
	TAdvSmoothToggleButton *btnScanTargetTray;
	TAdvSmoothToggleButton *btnScanSourceTray;
	TTimer *senTimer;
	TTimer *mesTimer;
	TEdit *target_idEdit;
	TEdit *src_idEdit;
	TPanel *psrcReady;
	TPanel *psrcArrive;
	TPanel *ptargetReady;
	TPanel *pdoor_left;
	TPanel *pdoor_right;
	TPanel *pemergency;
	TPanel *psrcOut;
	TPanel *ptargetOut;
	TPanel *ppause;
	TAdvSmoothToggleButton *resetBtn;
	TPanel *pwork1;
	TPanel *pwork2;
	TMemo *badCode;
	TEdit *limitEdit;
	TPanel *pnlNgLimit;
	TAdvSmoothToggleButton *btnApplyNgLimitCount;
	TMemo *zoneCode;
	TAdvSmoothButton *zone1;
	TAdvSmoothButton *zone2;
	TAdvSmoothButton *zone3;
	TAdvSmoothButton *zone4;
	TPanel *pmainMsg;
	TPanel *Panel5;
	TPanel *Panel7;
	TPanel *pgripperMsg;
	TPanel *probostarMsg;
	TPanel *Panel17;
	TPanel *Panel1;
	TPanel *Panel9;
	TPanel *Panel12;
	TPanel *Panel22;
	TAdvSmoothButton *trayout_srcBtn;
	TPanel *pnlSourceRemaining;
	TPanel *pejectremainCnt;
	TPanel *Panel24;
	TPanel *pLspX1;
	TPanel *pLspY;
	TPanel *pLspZ;
	TPanel *Panel34;
	TPanel *pLsnX1;
	TPanel *pLsnY;
	TPanel *pLsnZ;
	TPanel *pOnX1;
	TPanel *pOnY;
	TPanel *pOnZ;
	TPanel *pOrgX1;
	TPanel *pOrgY;
	TPanel *pOrgZ;
	TPanel *pErrorX1;
	TPanel *pErrorY;
	TPanel *pErrorZ;
	TAdvSmoothButton *openBtn;
	TMemo *Memo_Ko;
	TMemo *Memo_En;
	TMemo *Memo_Hu;
	TAdvSmoothToggleButton *AdvSmoothToggleButton_InitWork;
	TAdvSmoothToggleButton *pause_startBtn;
	TAdvSmoothPanel *pTargetBase;
	TPanel *Panel31;
	TPanel *pflow3;
	TPanel *pflow4;
	TCheckBox *CheckBox1;
	TPanel *pRun;
	TAdvSmoothPanel *pnlSourceTrayHeader;
	TAdvSmoothPanel *pnlTargetTrayHeader;
	TMemo *Memo1;
	TMemo *memoLog;
	TAdvSmoothPanel *pnlLogTitle;
	TLabel *lblLogTitle;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall targetGridDrawCell(TObject *Sender, int ACol, int ARow,
		  TRect &Rect, TGridDrawState State);
	void __fastcall pause_startBtnClick(TObject *Sender);
	void __fastcall pause_stopBtnClick(TObject *Sender);
	void __fastcall teachingBtnClick(TObject *Sender);
	void __fastcall btnScanSourceTrayClick(TObject *Sender);
	void __fastcall btnScanTargetTrayClick(TObject *Sender);
	void __fastcall senTimerTimer(TObject *Sender);
	void __fastcall mesTimerTimer(TObject *Sender);
	void __fastcall autoBtnClick(TObject *Sender);
	void __fastcall manualBtnClick(TObject *Sender);
	void __fastcall playBtnClick(TObject *Sender);
	void __fastcall stopBtnClick(TObject *Sender);
	void __fastcall target_idEditKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall src_idEditKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall pTrayid_targetDblClick(TObject *Sender);
	void __fastcall buzzerBtnClick(TObject *Sender);
	void __fastcall pTrayid_sourceDblClick(TObject *Sender);
	void __fastcall trayout_srcBtnClick(TObject *Sender);
	void __fastcall trayout_targetBtnClick(TObject *Sender);
	void __fastcall stepTimerTimer(TObject *Sender);
	void __fastcall chkBypassClick(TObject *Sender);
	void __fastcall resetBtnClick(TObject *Sender);
	void __fastcall btnApplyNgLimitCountClick(TObject *Sender);
	void __fastcall zone1Click(TObject *Sender);
	void __fastcall openBtnClick(TObject *Sender);
	void __fastcall AdvSmoothToggleButton_InitWorkClick(TObject *Sender);
	void __fastcall pnlSource2Click(TObject *Sender);
	void __fastcall lblTitleClick(TObject *Sender);
private:	// User declarations
//------------ 폼 관련 -------------------//

	void __fastcall InitMode();
	void __fastcall AutoMode();
	void __fastcall ManualMode();
	void __fastcall EmergencyMode();
	void __fastcall ResetMode();

	TListItem	*ITEM;
	TIniFile *ini;

	typedef struct{
		int step;
		int cnt;
		int timeout;
	}STEP;
	STEP step[2];
	void __fastcall InitStep(STEP *data);

	void __fastcall MakePanel();
	void __fastcall MakePanel_TargetTray();
	void __fastcall SetOption(TPanel *pnl, int nx, int ny, int nw, int nh, TColor clr, int tagValue);
    void __fastcall SetOption_TargetTray(TPanel *pnl, int nx, int ny, int nw, int nh, TColor clr, int tagValue);
	void __fastcall ChangeTrayMap(int channel);
	void __fastcall ChangeTrayMap_TargetTray(int channel);

	void __fastcall setMapping();

	void __fastcall DisplayStatus(int status);
	void __fastcall DisplaySensorInfo();
	void __fastcall sensorColor(TPanel *pnl, bool bon);
	void __fastcall CreateIoMonitoringPanel();
	void __fastcall CreateIoRow(TScrollBox *parent, TPanel **statePanel, int index, AnsiString address, AnsiString name);
	void __fastcall UpdateIoMonitoringPanel();
	void __fastcall btnIOMonitoringClick(TObject *Sender);
	void __fastcall btnCloseIoPanelClick(TObject *Sender);

	int __fastcall FindList(AnsiString strType);
	void __fastcall AddList(AnsiString strType);

	void __fastcall EnableButton_auto(bool benable);
	void __fastcall setLamp();

	AnsiString __fastcall getCodeName(AnsiString code);

	TAdvSmoothButton *zoneBtn[4];
	void __fastcall WriteZoneList();
	void __fastcall ReadZoneList();

	TPanel *status_on[AxisCnt], *status_org[AxisCnt], *status_error[AxisCnt], *status_lsp[AxisCnt], *status_lsn[AxisCnt], *status_pos[AxisCnt];
	TAdvSmoothButton *btnIOMonitoring;
	TPanel *grp_io;
	TPanel *ioInputState[64];
	TPanel *ioOutputState[32];
	int ioInputCount;
	int ioOutputCount;

public:		// User declarations

	TMod_Bcr *comBcr[2];
	TSmokeDetector *comSmoke[1];
	void __fastcall setBarcode(int pos, AnsiString strBcr);
	bool __fastcall ReadSystemInfo();
	void __fastcall InitBarcodeAndSmoke();
	SorterMode equipMode;
	LampMode nowLampMode, beforeLampMode;

	long	path;				/*	CCLINK variable to save path		*/
	TX_DATA *tx;
	TPanel *psort_ch[96];
	TPanel *psort_ing[96];
	TPanel *psort_bad[96];
	TPanel *psort_rank[96];

	TPanel *pTarget_ch[96];
	TPanel *pTarget_bad[96];

	TColor color_target[4][24];
    TColor color_target2[96];
	int mapSort[2][96];		// 96채널 48채널 맵핑
	TAdvSmoothPanel *pt_ch[96];

	STAGE_INFO stage;
	TRAY_INFO tray_source;
	TRAY_INFO tray_target;
	TRAY_INFO *tray;

	int __fastcall GetZoneCount(int zone);
	bool __fastcall GetZoneChannel(int zone, int ch);
	bool __fastcall IsSourceTrayInSignal() const;
	bool __fastcall IsSourceCenteringSignal() const;
	bool __fastcall IsTargetTrayInSignal() const;
	bool __fastcall IsTargetCenteringSignal() const;

	void __fastcall InitTrayInfo(int pos);
	void __fastcall DisplayTrayInfo();
	void __fastcall DisplayTranserIn(AnsiString trayid);
	void __fastcall DisplayRecipeInfo();

	void __fastcall DisplaySourceCell(int toolNum, int ch);
	void __fastcall DisplayTargetCell(int toolNum, int ch);
	void __fastcall DisplayTargetCellInfo(int toolNum, int ch);

	void __fastcall NotifyTrayInfo(AnsiString strTray, bool bsrc);
	void __fastcall NotifyTransferIn(AnsiString strTray);
	void __fastcall NotifyTransferOut(AnsiString strTray);
	void __fastcall NotifyIdMatching_source();
	void __fastcall NotifyIdMatching_target(AnsiString matchingStep);
	void __fastcall NotifyEquipStatus(AnsiString process);
	// PLC_INPUT plcInput; // Legacy ASCII interface disabled; use ModPLC_BIN status APIs.
	// PLC_OUTPUT plcOutput; // Legacy ASCII interface disabled; use ModPLC_BIN commands.
	void __fastcall BuzzerOn(bool on);
	void __fastcall LampModeChange(LampMode mode);

	void __fastcall CmdTrayOut(int pos);
	void __fastcall WriteProgLog(AnsiString msg);
	void __fastcall WriteOpcUaLog(AnsiString Type, AnsiString Msg, bool bDisplay = true);
	void __fastcall WriteErrorLog(AnsiString str1, AnsiString str2);
	AnsiString __fastcall GetAlarmMsg(int code);
	void __fastcall NotifyAlarm(bool alarm, AnsiString code = -1,  bool warning = true);

	void __fastcall AddStatusLog(AnsiString source, AnsiString msg);
	void __fastcall memoMainLineAdd(AnsiString msg);
	void __fastcall memoGripperLineAdd(AnsiString msg);
	void __fastcall memoRobostarLineAdd(AnsiString msg);

	void __fastcall setTrayInfo(int index);
	void __fastcall saveTrayInfo(int index);
	void __fastcall loadTrayInfo(int index);
	bool __fastcall checkTrayInfo(int index);
	SAVE_TRAY_INFO m_saveTrayInfo[2];

	bool m_ServoOpen, m_ServoON, m_ServoHome, m_ServoHomeEmg;//* 에러 났을 때 x,y,z축이 원점일 때 gripper 조그버튼 동작가능
    int LampCount;

	__fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
