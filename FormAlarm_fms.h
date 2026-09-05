//---------------------------------------------------------------------------

#ifndef FormAlarm_fmsH
#define FormAlarm_fmsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TAlarmForm_fms : public TForm
{
__published:
	TPanel *pnlTitle;
	TLabel *lblTitle;
	TMemo *memoDetail;
	TLabel *lblRequestTitle;
	TLabel *lblRequest;
	TLabel *lblResponseTitle;
	TLabel *lblResponse;
	TLabel *lblStatus;
	TButton *btnBuzzerStop;
	TButton *btnPause;
	TButton *btnRetry;
	TButton *btnClose;
	void __fastcall btnBuzzerStopClick(TObject *Sender);
	void __fastcall btnPauseClick(TObject *Sender);
	void __fastcall btnRetryClick(TObject *Sender);
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormHide(TObject *Sender);
private:
	// FMS ALARM PAUSE/CLOSE: acknowledgement affects presentation only.
	bool operatorPaused;
	bool dismissed;
	DWORD dismissedTick;
public:
	__fastcall TAlarmForm_fms(TComponent* Owner);
	void __fastcall ShowFmsError(const AnsiString &Title,
		const AnsiString &Detail, const AnsiString &RequestName,
		int ResponseValue);
	void __fastcall SetRetryWaiting(const AnsiString &Status);
	void __fastcall SetOperatorPaused();
	void __fastcall RefreshAlarmVisibility();
};
//---------------------------------------------------------------------------
extern PACKAGE TAlarmForm_fms *AlarmForm_fms;
//---------------------------------------------------------------------------
#endif
