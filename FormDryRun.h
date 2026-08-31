//---------------------------------------------------------------------------

#ifndef FormDryRunH
#define FormDryRunH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------

// ============================================================================
//* DRY RUN : Inspection-only UI and sequence. No production data is modified.
// ============================================================================
class TDryRunForm : public TForm
{
__published:	// IDE-managed Components
	TLabel *lblTitle;
	TLabel *lblStartChannel;
	TLabel *lblEndChannel;
	TLabel *lblSpeed;
	TEdit *editStartChannel;
	TEdit *editEndChannel;
	TEdit *editSpeed;
	TButton *btnStart;
	TButton *btnStop;
	TButton *btnWaitPosition;
	TButton *btnClose;
	TPanel *pnlRunState;
	TLabel *lblCurrentChannel;
	TLabel *lblCurrentStep;
	TLabel *lblInterlock;
	TMemo *memoDryRun;
	TTimer *dryRunTimer;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall btnStartClick(TObject *Sender);
	void __fastcall btnStopClick(TObject *Sender);
	void __fastcall btnWaitPositionClick(TObject *Sender);
	void __fastcall btnCloseClick(TObject *Sender);
	void __fastcall dryRunTimerTimer(TObject *Sender);

private:	// User declarations
	typedef enum
	{
		drsIdle,
		drsSorting,
		drsEject,
		drsInsert,
		drsReturn,
		drsError
	} TDryRunSequence;

	TDryRunSequence drySequence;
	int dryStep;
	int startChannel;
	int endChannel;
	int currentChannel;
	int completedCycleCount;
	DWORD stepStartTick;
	DWORD stepTimeoutMs;
	bool running;
	bool waitPositionRequested;

	void __fastcall SetDryStep(TDryRunSequence sequence, int step,
		const AnsiString &status, DWORD timeoutMs = 30000);
	void __fastcall WriteDryRunLog(const AnsiString &message);
	void __fastcall UpdateDryRunStatus();
	bool __fastcall ValidateDryRunStart(AnsiString &reason);
	bool __fastcall ValidateWaitPositionStart(AnsiString &reason);
	bool __fastcall ApplyDryRunSpeed(AnsiString &reason);
	bool __fastcall CheckDryRunRuntimeInterlock(AnsiString &reason);
	bool __fastcall CheckStepTimeout();
	void __fastcall AbortDryRun(const AnsiString &reason);
	void __fastcall CompleteDryRun(const AnsiString &message);

	void __fastcall DryRunSorting();
	void __fastcall DryRunEject();
	void __fastcall DryRunInsert();
	void __fastcall DryRunReturn();

public:		// User declarations
	__fastcall TDryRunForm(TComponent* Owner);
	bool __fastcall IsRunning() const;
};
//---------------------------------------------------------------------------
extern PACKAGE TDryRunForm *DryRunForm;
//---------------------------------------------------------------------------
#endif
