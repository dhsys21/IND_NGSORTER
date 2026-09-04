//---------------------------------------------------------------------------

#ifndef FormBaseH
#define FormBaseH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <jpeg.hpp>
#include <Graphics.hpp>
#include "AdvSmoothButton.hpp"
#include "AdvSmoothPanel.hpp"

#include "FormMain.h"
#include "Modmes.h"
#include "ModMes_Gateway.h"
#include "ModMes_OPCUA.h"
#include "ModEcs.h"
#include "FormConfig.h"
#include "ModPLC_Bin.h"
#include "DEFINE.h"
#include "ModRobostar.h"
#include "Mdfunc.h"
#include "ModGripper.h"
#include "FormTeaching.h"
#include "FormError.h"
#include "FormError_eject.h"
#include "FormError_insert.h"
#include "FormError_bcr.h"
#include "FormError_mes.h"
#include "FormError_limit.h"
#include "FormDoor.h"
#include "FormAlarm.h"
#include "FormAlarm_fms.h"
#include "Barcode_comm.h"
#include "Mod_SRX100W.h"
#include "SmokeDetector_comm.h"
#include "FormServoAlarmList.h"
#include "FormLoadFactor.h"
#include "FormAlarm_LoadFactor.h"
#include "FormAlarm_TrayInfo.h"
#include "FormInterface.h"

#include "mmsystem.h"

//---------------------------------------------------------------------------
typedef struct{
	bool file_exists;
	int ims_port;
	AnsiString fmsIp;
	int gatewayPort;
	// FAT maximum-speed mode changes only the timing of non-critical work:
	// start the next motion first, then write logs/results and report CellTrackOut.
	bool maximumSpeedMode;
	// These two switches remain independent so each cycle-time reduction can be
	// measured separately from the maximum-speed I/O/report reordering.
	bool optimizeSequenceDelay;
	bool skipGripStabilization;
	// Bench trays currently have no labels. Keep barcode simulation separate
	// from cbCycle so field operation can use real readers with either handshake mode.
	bool useFatTestBarcodes;
	AnsiString fatTestSourceBarcode;
	AnsiString fatTestTargetBarcode;
	AnsiString line;
	AnsiString pc;
	int ocv_retest;
	AnsiString bcrIp[2];
	int bcrPort[2];
	AnsiString smokePort;
	int smokeId;
	int smokeMode;
	int smokeBaudRate;
	AnsiString plcIp;
	int plcPortPlc;
	int plcPortPc;
}MAIN_CONFIG;


class TBaseForm : public TForm
{
__published:	// IDE-managed Components
	TTimer *ClockTimer;
	TTimer *FileDeleteTimer;
	TImage *Image12;
	TAdvSmoothPanel *pims;
	TAdvSmoothPanel *psmokedetector;
	TPanel *pon;
	TPanel *poff;
	TAdvSmoothButton *Button1;
	TAdvSmoothPanel *pplc;
	TAdvSmoothPanel *pcclink;
	TAdvSmoothPanel *pbcr1;
	TAdvSmoothPanel *pbcr2;
	TAdvSmoothPanel *grp_tmperature;
	TLabel *lblManualOperation;
	TMemo *Memo1;
	TGroupBox *GroupBox2;
	TAdvSmoothPanel *pnlTempPV;
	TAdvSmoothPanel *pnlTempOffset;
	TAdvSmoothPanel *pnlTempWarning;
	TAdvSmoothPanel *pnlTempDanger;
	TPanel *PanelSmokeSet;
	TGroupBox *GroupBox1;
	TRadioButton *rbSetTemperatureOffset;
	TRadioButton *rbSetTemperatureWarningSV;
	TRadioButton *rbSetTemperatureDangerSV;
	TEdit *editTempValue;
	TAdvSmoothButton *btnSetValue;
	TGroupBox *GroupBox4;
	TAdvSmoothPanel *pnlAlarmReserve;
	TAdvSmoothPanel *pnlAlarmSmoke;
	TAdvSmoothPanel *pnlAlarmDanger;
	TAdvSmoothPanel *pnlAlarmWarning;
	TGroupBox *GroupBox3;
	TAdvSmoothPanel *pnlStatusSmoke;
	TAdvSmoothPanel *pnlStatusDanger;
	TAdvSmoothPanel *pnlStatusWarning;
	TAdvSmoothPanel *pnlStatusRun;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *Label8;
	TRadioButton *rbAlarmClear;
	TAdvSmoothPanel *grp_bcr;
	TLabel *Label9;
	TMemo *memoBcr;
	TPanel *Panel2;
	TAdvSmoothButton *btnTriggerOn;
	TAdvSmoothButton *btnTriggerOff;
	TPanel *Panel1;
	TAdvSmoothButton *AdvSmoothButton2;
	TAdvSmoothButton *AdvSmoothButton1;
	TLabel *Label1;
	TPanel *Panel5;
	TPanel *pstepInfo1;
	TRadioButton *RadioButton1;
	TRadioButton *RadioButton2;
	TRadioButton *RadioButton3;
	TAdvSmoothButton *AdvSmoothButton3;
	TLabel *Label2;
	TAdvSmoothButton *btnKeyLock;
	TAdvSmoothButton *btnKeyUnLock;
	TAdvSmoothButton *btnBypassOn;
	TAdvSmoothButton *btnSafetyReset;
	TAdvSmoothButton *AdvSmoothButton4;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall ClockTimerTimer(TObject *Sender);
	void __fastcall FileDeleteTimerTimer(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall AdvSmoothButton2Click(TObject *Sender);
	void __fastcall AdvSmoothButton1Click(TObject *Sender);
	void __fastcall RadioButton1Click(TObject *Sender);
	void __fastcall AdvSmoothButton3Click(TObject *Sender);
	void __fastcall btnKeyLockClick(TObject *Sender);
	void __fastcall btnKeyUnLockClick(TObject *Sender);
	void __fastcall btnBypassOnClick(TObject *Sender);
	void __fastcall btnSafetyResetClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall AdvSmoothButton4Click(TObject *Sender);
	void __fastcall pbcrClick(TObject *Sender);
	void __fastcall psmokedetectorClick(TObject *Sender);
	void __fastcall btnSetValueClick(TObject *Sender);
	void __fastcall rbAlarmClearClick(TObject *Sender);
	void __fastcall pnlTempPVClick(TObject *Sender);
	void __fastcall pnlTempOffsetClick(TObject *Sender);
	void __fastcall btnTriggerOnClick(TObject *Sender);
	void __fastcall btnTriggerOffClick(TObject *Sender);

private:	// User declarations

	int FormCnt;
	int DeleteDay;
	int DeleteIndex;
	int SelectedBcrIndex;
	bool __fastcall DeleteLogFile(AnsiString FileName);
	bool __fastcall DeleteLogFolder(AnsiString FolderName);

	unsigned long PID;
    HANDLE hProcess;
public:		// User declarations

	void __fastcall setColor(TAdvSmoothPanel *pnl, bool bon);
	MAIN_CONFIG config;

    AnsiString CurrentLanguage;
    TStringList *LangDict;
    UnicodeString __fastcall GetLangStr(AnsiString key);
	void __fastcall ReadLanguage(AnsiString newLang);
    void __fastcall ChangeLanguage();

    int __fastcall StringToInt(UnicodeString str, int def);
    double __fastcall StringToDouble(UnicodeString str, double def);
	__fastcall TBaseForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TBaseForm *BaseForm;
//---------------------------------------------------------------------------
#endif
