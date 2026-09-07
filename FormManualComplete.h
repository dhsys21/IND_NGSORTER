#ifndef FormManualCompleteH
#define FormManualCompleteH
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "ManualCellRecovery.h"
class TManualCompleteForm : public TForm
{
__published:
    TPanel *pnlTitle;
    TLabel *lblSource, *lblTarget, *lblCell, *lblChannel, *lblStatus;
    TLabel *lblFmsState;
    TEdit *editSource, *editTarget, *editCell, *editChannel;
    TCheckBox *chkInserted;
    TButton *btnReport, *btnRetry, *btnResume, *btnClose;
    TTimer *pollTimer;
    void __fastcall btnReportClick(TObject *Sender);
    void __fastcall btnRetryClick(TObject *Sender);
    void __fastcall btnResumeClick(TObject *Sender);
    void __fastcall btnCloseClick(TObject *Sender);
    void __fastcall pollTimerTimer(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
private:
    TManualCellPhase phase;
    int toolNo, sourceNo, targetNo, reservedNo;
    AnsiString sourceId, targetId, cellId, lotId, ngCode, grade;
    bool workFlag, polling, restored, journalOkay, requestClearQueued;
    DWORD started;
    UnicodeString JournalPath() const;
    void LoadJournal();
    bool SaveJournal();
    bool ContextMatches();
    bool ApplyPhysicalCompletion();
    void Fail(const UnicodeString &message);
    void RefreshControls();
    void RefreshFmsWaitLabel();
    void CancelRequest();
    void SendRequest();
public:
    __fastcall TManualCompleteForm(TComponent *Owner);
    bool IsBlocking() const { return phase != mcIdle || !journalOkay; }
    void ApplyLanguage();
    void OpenRecovery(int ToolNo);
};
extern PACKAGE TManualCompleteForm *ManualCompleteForm;
#endif
