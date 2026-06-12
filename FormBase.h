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
#include "ModEcs.h"
#include "FormConfig.h"
#include "Modplc.h"
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
#include "Barcode_comm.h"
#include "FormServoAlarmList.h"
#include "FormLoadFactor.h"
#include "FormAlarm_LoadFactor.h"
#include "FormAlarm_TrayInfo.h"


#include "mmsystem.h"

//---------------------------------------------------------------------------
typedef struct{
	bool file_exists;
	int ims_port;
	AnsiString line;
	AnsiString pc;
	int ocv_retest;
}MAIN_CONFIG;


class TBaseForm : public TForm
{
__published:	// IDE-managed Components
	TTimer *ClockTimer;
	TTimer *FileDeleteTimer;
	TImage *Image12;
	TAdvSmoothPanel *pims;
	TPanel *pon;
	TPanel *poff;
	TAdvSmoothButton *Button1;
	TAdvSmoothPanel *pplc;
	TAdvSmoothPanel *pcclink;
	TAdvSmoothPanel *pbcr1;
	TAdvSmoothPanel *pbcr2;
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
	void __fastcall pplcClick(TObject *Sender);
	void __fastcall btnKeyLockClick(TObject *Sender);
	void __fastcall pecsClick(TObject *Sender);
	void __fastcall btnKeyUnLockClick(TObject *Sender);

private:	// User declarations

	int FormCnt;
	int DeleteDay;
	int DeleteIndex;
	bool __fastcall DeleteLogFile(AnsiString FileName);
	bool __fastcall DeleteLogFolder(AnsiString FolderName);

	unsigned long PID;
    HANDLE hProcess;
public:		// User declarations

    TPanel *pRead[2][16];
	void __fastcall setColor(TAdvSmoothPanel *pnl, bool bon);
	MAIN_CONFIG config;

    int __fastcall StringToInt(UnicodeString str, int def);
    double __fastcall StringToDouble(UnicodeString str, double def);
	__fastcall TBaseForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TBaseForm *BaseForm;
//---------------------------------------------------------------------------
#endif
