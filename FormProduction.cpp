#include <vcl.h>
#pragma hdrstop
#include "FormProduction.h"
#include "ProductionHistory.h"
#include "AccessControl.h"
#include <System.DateUtils.hpp>
#pragma package(smart_init)
#pragma resource "*.dfm"
TProductionForm *ProductionForm;

__fastcall TProductionForm::TProductionForm(TComponent *Owner) : TForm(Owner), FUpdating(true)
{
    dateProduction->Date = Date();
    FUpdating = false;
}

void TProductionForm::Prepare()
{
    FUpdating = true;
    try{
        Caption = AccessText("PRODUCTION_TITLE", "Production");
        lblTitle->Caption = Caption;
        tabsPeriod->Tabs->Strings[0] = AccessText("PRODUCTION_DAILY", "Daily");
        tabsPeriod->Tabs->Strings[1] = AccessText("PRODUCTION_WEEKLY", "Weekly (Mon - Sun)");
        tabsPeriod->Tabs->Strings[2] = AccessText("PRODUCTION_MONTHLY", "Monthly");
        btnToday->Caption = AccessText("PRODUCTION_TODAY", "Today");
        btnRefresh->Caption = AccessText("PRODUCTION_REFRESH", "Refresh");
        btnClose->Caption = AccessText("ACCESS_GUIDE_CLOSE", "Close");
        btnPrevious->Hint = AccessText("PRODUCTION_PREVIOUS", "Previous period");
        btnNext->Hint = AccessText("PRODUCTION_NEXT", "Next period");
        lblBasis->Caption = AccessText("PRODUCTION_BASIS",
            "Source tray completion (FMS ProcessEnd OK); incoming occupied cells, OK and NG.\r\n"
            "One count per source tray cycle. Bypass / CYCLE / FAT tests excluded.");
    }__finally{ FUpdating = false; }
    RefreshData();
}

void TProductionForm::RefreshData()
{
    if(FUpdating) return;
    FUpdating = true;
    try{
        gridProduction->Cells[0][0] = AccessText("PRODUCTION_BUCKET", "Time / Date");
        gridProduction->Cells[1][0] = AccessText("PRODUCTION_TRAYS", "Trays");
        gridProduction->Cells[2][0] = AccessText("PRODUCTION_CELLS", "Cells");
        gridProduction->Cells[3][0] = AccessText("PRODUCTION_GOOD", "OK");
        gridProduction->Cells[4][0] = AccessText("PRODUCTION_BAD", "NG");
        if(!ProductionHistory().FlushPending()) throw Exception(ProductionHistory().Error());
        std::vector<TProductionBucket> buckets = ProductionHistory().Query(
            static_cast<TProductionPeriod>(tabsPeriod->TabIndex), dateProduction->Date);
        int topRow = gridProduction->TopRow;
        gridProduction->RowCount = buckets.size() + 1;
        __int64 trays = 0, cells = 0, good = 0, bad = 0;
        for(unsigned int i = 0; i < buckets.size(); ++i){
            const TProductionBucket &bucket = buckets[i];
            UnicodeString label;
            if(bucket.Hour >= 0)
                label = FormatFloat("00",bucket.Hour) + " ~ " + FormatFloat("00",bucket.Hour+1);
            else label = bucket.Date.FormatString("yyyy-mm-dd (ddd)");
            gridProduction->Cells[0][i+1] = label;
            gridProduction->Cells[1][i+1] = IntToStr(bucket.Trays);
            gridProduction->Cells[2][i+1] = IntToStr(bucket.Cells);
            gridProduction->Cells[3][i+1] = IntToStr(bucket.Good);
            gridProduction->Cells[4][i+1] = IntToStr(bucket.Bad);
            trays += bucket.Trays; cells += bucket.Cells;
            good += bucket.Good; bad += bucket.Bad;
        }
        if(topRow > 0 && topRow < gridProduction->RowCount) gridProduction->TopRow = topRow;
        lblRange->Caption = buckets.front().Date.FormatString("yyyy-mm-dd");
        if(tabsPeriod->TabIndex != 0)
            lblRange->Caption = lblRange->Caption + " ~ " + buckets.back().Date.FormatString("yyyy-mm-dd");
        lblTotal->Caption = AccessText("PRODUCTION_TOTAL", "Total") + "   " +
            AccessText("PRODUCTION_TRAYS", "Trays") + ": " + IntToStr(trays) + "    " +
            AccessText("PRODUCTION_CELLS", "Cells") + ": " + IntToStr(cells) + "\r\n" +
            AccessText("PRODUCTION_GOOD", "OK") + ": " + IntToStr(good) + "    " +
            AccessText("PRODUCTION_BAD", "NG") + ": " + IntToStr(bad);
        lblStatus->Font->Color = clGrayText;
        lblStatus->Caption = AccessText("PRODUCTION_UPDATED", "Updated") + " " + Now().FormatString("hh:nn:ss");
    }catch(Exception &e){
        // Corrupt/unreadable history is an error, never a misleading zero-production result.
        gridProduction->RowCount = 2;
        for(int col = 0; col < gridProduction->ColCount; ++col) gridProduction->Cells[col][1] = "-";
        lblTotal->Caption = "-";
        lblRange->Caption = "";
        lblStatus->Font->Color = clRed;
        lblStatus->Caption = AccessText("PRODUCTION_READ_ERROR", "Production history error:") + " " + e.Message;
    }
    FUpdating = false;
}

void TProductionForm::MovePeriod(int Direction)
{
    FUpdating = true;
    if(tabsPeriod->TabIndex == 2) dateProduction->Date = IncMonth(dateProduction->Date, Direction);
    else dateProduction->Date = IncDay(dateProduction->Date, Direction * (tabsPeriod->TabIndex == 1 ? 7 : 1));
    FUpdating = false;
    // TDateTimePicker programmatic assignments do not reliably emit OnChange.
    PeriodChange(NULL);
}
void __fastcall TProductionForm::btnCloseClick(TObject *Sender) { Close(); }
void __fastcall TProductionForm::PeriodChange(TObject *Sender)
{
    if(FUpdating) return;
    gridProduction->TopRow = 1;
    RefreshData();
}
void __fastcall TProductionForm::btnPreviousClick(TObject *Sender) { MovePeriod(-1); }
void __fastcall TProductionForm::btnNextClick(TObject *Sender) { MovePeriod(1); }
void __fastcall TProductionForm::btnTodayClick(TObject *Sender) { dateProduction->Date = Date(); RefreshData(); }
void __fastcall TProductionForm::btnRefreshClick(TObject *Sender) { RefreshData(); }
void __fastcall TProductionForm::FormShow(TObject *Sender) { timerRefresh->Enabled = true; }
void __fastcall TProductionForm::FormHide(TObject *Sender) { timerRefresh->Enabled = false; }
void __fastcall TProductionForm::timerRefreshTimer(TObject *Sender) { if(Visible) RefreshData(); }

void __fastcall TProductionForm::gridProductionDrawCell(TObject *Sender, int ACol,
    int ARow, TRect &Rect, TGridDrawState State)
{
    TCanvas *canvas = gridProduction->Canvas;
    canvas->Font->Assign(gridProduction->Font);
    if(ARow > 0 && ACol == 3) canvas->Font->Color = static_cast<TColor>(RGB(0,110,65));
    else if(ARow > 0 && ACol == 4) canvas->Font->Color = static_cast<TColor>(RGB(180,35,35));
    canvas->Brush->Color = ARow == 0 ? static_cast<TColor>(RGB(225,237,241)) :
        (ARow % 2 ? clWhite : static_cast<TColor>(RGB(246,248,249)));
    if(ARow == 0) canvas->Font->Style = TFontStyles() << fsBold;
    else if(State.Contains(gdSelected)){
        canvas->Brush->Color = clHighlight;
        canvas->Font->Color = clHighlightText;
    }
    canvas->FillRect(Rect);
    TRect textRect = Rect;
    textRect.Left += 8; textRect.Right -= 8;
    UnicodeString value = gridProduction->Cells[ACol][ARow];
    // Draw both Latin and Korean text inside each cell; right-align quantities.
    int saved = SaveDC(canvas->Handle);
    SetTextAlign(canvas->Handle, TA_LEFT | TA_TOP | TA_NOUPDATECP);
    SetBkMode(canvas->Handle, TRANSPARENT);
    DrawTextW(canvas->Handle, value.c_str(), value.Length(), &textRect,
        DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | (ACol == 0 ? DT_LEFT : DT_RIGHT));
    RestoreDC(canvas->Handle, saved);
}
