#ifndef FormProductionH
#define FormProductionH
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.Forms.hpp>

class TProductionForm : public TForm
{
__published:
    TPanel *pnlHeader;
    TLabel *lblTitle;
    TButton *btnClose;
    TTabControl *tabsPeriod;
    TDateTimePicker *dateProduction;
    TButton *btnPrevious;
    TButton *btnNext;
    TButton *btnToday;
    TButton *btnRefresh;
    TLabel *lblRange;
    TLabel *lblTotal;
    TStringGrid *gridProduction;
    TLabel *lblStatus;
    TLabel *lblBasis;
    TTimer *timerRefresh;
    void __fastcall btnCloseClick(TObject *Sender);
    void __fastcall PeriodChange(TObject *Sender);
    void __fastcall btnPreviousClick(TObject *Sender);
    void __fastcall btnNextClick(TObject *Sender);
    void __fastcall btnTodayClick(TObject *Sender);
    void __fastcall btnRefreshClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormHide(TObject *Sender);
    void __fastcall timerRefreshTimer(TObject *Sender);
    void __fastcall gridProductionDrawCell(TObject *Sender, int ACol, int ARow,
        TRect &Rect, TGridDrawState State);
private:
    bool FUpdating;
    void MovePeriod(int Direction);
public:
    __fastcall TProductionForm(TComponent *Owner);
    void Prepare();
    void RefreshData();
};
extern PACKAGE TProductionForm *ProductionForm;
#endif
