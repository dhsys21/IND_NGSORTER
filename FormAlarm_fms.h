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
	TButton *btnRetry;
	void __fastcall btnBuzzerStopClick(TObject *Sender);
	void __fastcall btnRetryClick(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
private:
public:
	__fastcall TAlarmForm_fms(TComponent* Owner);
	void __fastcall ShowFmsError(const AnsiString &Title,
		const AnsiString &Detail, const AnsiString &RequestName,
		int ResponseValue);
	void __fastcall SetRetryWaiting(const AnsiString &Status);
};
//---------------------------------------------------------------------------
extern PACKAGE TAlarmForm_fms *AlarmForm_fms;
//---------------------------------------------------------------------------
#endif
