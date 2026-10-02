#include <vcl.h>
#pragma hdrstop
#include "../ProductionHistory.h"
#include "../FormProduction.h"
#include "../AccessControl.h"
#include <System.DateUtils.hpp>
#include <Vcl.Imaging.pngimage.hpp>
#include <memory>
#include <stdio.h>

static FILE *Result;
static void Check(bool Ok, const char *Message)
{
    if(!Ok) throw Exception(Message);
    fprintf(Result, "PASS: %s\n", Message);
}
static void Add(TProductionHistory &Store, TDateTime Time, int Cells, int Bad = 0)
{
    Store.BeginCycle();
    Check(Store.CompleteCycle("TRAY,\"001\"", Cells, false, Time, Cells - Bad, Bad), "Save automatic completion");
}
static void Capture(TForm *Form, const UnicodeString &File)
{
    Form->Show(); Form->Repaint(); Application->ProcessMessages(); Form->Update();
    std::auto_ptr<Graphics::TBitmap> bitmap(new Graphics::TBitmap());
    bitmap->SetSize(Form->ClientWidth,Form->ClientHeight);
    HDC dc = GetDC(Form->Handle);
    BitBlt(bitmap->Canvas->Handle,0,0,Form->ClientWidth,Form->ClientHeight,dc,0,0,SRCCOPY);
    ReleaseDC(Form->Handle,dc);
    std::auto_ptr<TPngImage> png(new TPngImage());
    png->Assign(bitmap.get()); png->SaveToFile(File); Form->Hide();
}
int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
    SetCurrentDir(ExpandFileName(ExtractFilePath(ParamStr(0)) + "..\\.."));
    Result = fopen("tmp/Access20261001/production_result.txt", "w");
    try{
        Application->Initialize();
        UnicodeString folder = ExpandFileName("tmp/Access20261001/production_" +
            IntToStr((int)GetCurrentProcessId())) + "\\";
        TProductionHistory store;
        store.Initialize(folder);
        TDateTime day = EncodeDate(2026,10,1);
        Check(store.Query(ppDaily,day).size() == 24, "Missing day has 24 zero buckets");
        Add(store, day, 96, 2);
        Check(store.CompleteCycle("TRAY,\"001\"", 96, false, day), "Duplicate callback succeeds without duplicate count");
        Add(store, day + EncodeTime(0,59,59,0), 95, 1);
        Add(store, day + EncodeTime(1,0,0,0), 80);
        Add(store, day + EncodeTime(23,59,59,0), 1, 1);
        std::vector<TProductionBucket> daily = store.Query(ppDaily,day);
        Check(daily[0].Trays == 2 && daily[0].Cells == 191 && daily[1].Cells == 80 && daily[23].Cells == 1,
            "Hourly boundaries and actual occupied cell counts");
        Check(daily[0].Good == 188 && daily[0].Bad == 3 &&
            daily[1].Good == 80 && daily[1].Bad == 0 && daily[23].Good == 0 && daily[23].Bad == 1,
            "Final OK/NG counts include all-OK and all-NG batches without duplicate callbacks");
        store.BeginCycle();
        Check(store.CompleteCycle("TEST",96,true,day), "CYCLE tests excluded");
        store.BeginCycle();
        Check(store.CompleteCycle("EMPTY",0,false,day), "Empty trays excluded");
        Check(!store.CompleteCycle("INVALID",97,false,day), "Invalid cell count rejected");
        Check(!store.CompleteCycle("INVALID",96,false,day,96,1) &&
            !store.CompleteCycle("INVALID",96,false,day,-2,0), "Invalid quality totals rejected");
        Add(store, IncDay(day,1), 96);
        Add(store, EncodeDate(2026,9,30), 90);
        std::vector<TProductionBucket> week = store.Query(ppWeekly,day);
        Check(week.size() == 7 && week[0].Date == EncodeDate(2026,9,28) && week[2].Cells == 90 && week[3].Cells == 272,
            "Monday-Sunday week crosses month boundary correctly");
        Check(store.Query(ppMonthly,day).size() == 31 && store.Query(ppMonthly,EncodeDate(2026,4,1)).size() == 30 &&
            store.Query(ppMonthly,EncodeDate(2024,2,1)).size() == 29 && store.Query(ppMonthly,EncodeDate(2026,2,1)).size() == 28,
            "Actual month length and leap year");
        TProductionHistory reloaded;
        reloaded.Initialize(folder);
        Check(reloaded.Query(ppDaily,day)[0].Cells == 191, "History persists across restart");
        Check(reloaded.Query(ppDaily,day)[0].Good == 188 && reloaded.Query(ppDaily,day)[0].Bad == 3,
            "Quality counts persist across restart");
        Check(store.Query(ppWeekly,day)[3].Bad == 4 && store.Query(ppMonthly,day)[0].Good == 268,
            "Daily quality counts roll up into weekly and monthly views");
        Add(store, EncodeDate(2026,12,31), 7);
        Add(store, EncodeDate(2027,1,1), 8);
        week = store.Query(ppWeekly,EncodeDate(2027,1,1));
        Check(week[3].Cells == 7 && week[4].Cells == 8, "Week across year boundary");

        // A malformed file must remain untouched; failed saves are retried without duplicates.
        UnicodeString broken = folder + "20261003.json";
        std::auto_ptr<TStringList> text(new TStringList());
        text->Text = "not JSON"; text->SaveToFile(broken,TEncoding::UTF8);
        bool rejected = false;
        try{ store.Query(ppDaily,EncodeDate(2026,10,3)); }catch(Exception &){ rejected = true; }
        Check(rejected, "Corrupt history reported, not treated as zero");
        store.BeginCycle();
        Check(!store.CompleteCycle("PENDING",88,false,EncodeDate(2026,10,3),85,3), "Failed save kept pending");
        text->LoadFromFile(broken,TEncoding::UTF8);
        Check(text->Text.Trim() == "not JSON", "Failed save preserves original file");
        DeleteFile(broken); // Only the deliberately corrupted isolated test fixture.
        Check(store.FlushPending() && store.FlushPending() && store.Query(ppDaily,EncodeDate(2026,10,3))[0].Trays == 1,
            "Retry saves exactly one pending event");
        Check(store.Query(ppDaily,EncodeDate(2026,10,3))[0].Bad == 3, "Retry preserves final quality snapshot");

        UnicodeString legacy = folder + "20261004.json";
        text->Text = "{\"schema\":1,\"events\":[{\"id\":\"legacy\",\"stamp\":\"20261004000000\",\"trayId\":\"OLD\",\"cells\":10}]}";
        text->SaveToFile(legacy,TEncoding::UTF8);
        TProductionBucket old = store.Query(ppDaily,EncodeDate(2026,10,4))[0];
        Check(old.Cells == 10 && old.Good == 10 && old.Bad == 0,
            "Old totals without quality information count as OK");
        Add(store,EncodeDate(2026,10,4),12,1);
        old = store.Query(ppDaily,EncodeDate(2026,10,4))[0];
        Check(old.Cells == 22 && old.Good == 21 && old.Bad == 1,
            "Schema migration preserves legacy totals alongside new quality counts");
        text->LoadFromFile(legacy,TEncoding::UTF8);
        Check(text->Text.Pos("\"schema\":3") > 0 && text->Text.Pos("\"unknown\"") == 0,
            "New history saves OK/NG only without unknown field");
        UnicodeString validHistory = text->Text;
        text->Text = StringReplace(validHistory,"\"good\":11","\"good\":12",TReplaceFlags() << rfReplaceAll);
        Check(text->Text != validHistory, "Prepare inconsistent quality fixture");
        text->SaveToFile(legacy,TEncoding::UTF8);
        rejected = false;
        try{ store.Query(ppDaily,EncodeDate(2026,10,4)); }catch(Exception &){ rejected = true; }
        Check(rejected,"Stored OK + NG must equal occupied cell count");
        text->Text = validHistory; text->SaveToFile(legacy,TEncoding::UTF8);

        UnicodeString legacyQuality = folder + "20261005.json";
        text->Text = "{\"schema\":2,\"events\":[{\"id\":\"legacy-quality\",\"stamp\":\"20261005000000\",\"trayId\":\"OLD\",\"cells\":10,\"good\":4,\"bad\":2,\"unknown\":4}]}";
        text->SaveToFile(legacyQuality,TEncoding::UTF8);
        old = store.Query(ppDaily,EncodeDate(2026,10,5))[0];
        Check(old.Good == 8 && old.Bad == 2, "Legacy unknown counts become OK while NG is preserved");
        Add(store,EncodeDate(2026,10,5),5,1);
        old = store.Query(ppDaily,EncodeDate(2026,10,5))[0];
        Check(old.Cells == 15 && old.Good == 12 && old.Bad == 3,
            "Schema 2 migration preserves NG and converts unknown to OK");
        store.BeginCycle();
        Check(store.CompleteCycle("NO-VERDICT",10,false,EncodeDate(2026,11,1)), "Missing verdict is accepted as OK");
        store.BeginCycle();
        Check(store.CompleteCycle("PARTIAL-VERDICT",10,false,EncodeDate(2026,11,1),4,2), "Incomplete verdict counts remaining cells as OK");
        old = store.Query(ppDaily,EncodeDate(2026,11,1))[0];
        Check(old.Cells == 20 && old.Good == 18 && old.Bad == 2, "Missing and partial verdicts persist with OK + NG equal to total");

        ProductionHistory().Initialize(folder);
        std::auto_ptr<TProductionForm> form(new TProductionForm(NULL));
        form->dateProduction->Date = day;
        form->Prepare();
        Check(form->gridProduction->RowCount == 25 && form->gridProduction->Cells[0][24] == "23 ~ 24",
            "Daily grid has all 24 time slots");
        Check(form->gridProduction->ColCount == 5 && form->gridProduction->Cells[3][1] == "188" &&
            form->gridProduction->Cells[4][1] == "3", "Daily grid displays OK and NG columns");
        Capture(form.get(),"tmp/Access20261001/production_daily_en.png");
        form->tabsPeriod->TabIndex = 1; form->PeriodChange(NULL);
        Check(form->gridProduction->RowCount == 8, "Weekly grid has 7 days");
        Capture(form.get(),"tmp/Access20261001/production_weekly_en.png");
        form->tabsPeriod->TabIndex = 2; form->PeriodChange(NULL);
        Check(form->gridProduction->RowCount == 32, "Monthly grid has 31 days");
        form->btnPreviousClick(NULL);
        Check(MonthOf(form->dateProduction->Date) == 9 && form->gridProduction->RowCount == 31, "Previous month navigation refreshes grid");
        form->btnNextClick(NULL);
        Check(MonthOf(form->dateProduction->Date) == 10, "Next month navigation");
        text->LoadFromFile("Lang_Ko.ini",TEncoding::UTF8);
        SetAccessLanguage(text.get());
        form->Prepare();
        fprintf(Result,"Range bounds: %d,%d %dx%d; text=%s\n",form->lblRange->Left,form->lblRange->Top,
            form->lblRange->Width,form->lblRange->Height,AnsiString(form->lblRange->Caption).c_str());
        Capture(form.get(),"tmp/Access20261001/production_monthly_ko.png");
        form->gridProduction->TopRow = 8;
        Capture(form.get(),"tmp/Access20261001/production_monthly_bottom_ko.png");
        Check(!form->timerRefresh->Enabled, "Refresh timer disabled when hidden");
        text->Text = "bad JSON"; text->SaveToFile(broken,TEncoding::UTF8);
        form->Prepare();
        Check(form->lblStatus->Font->Color == clRed && form->lblTotal->Caption == "-",
            "UI does not show false totals when history is corrupt");
        SetAccessLanguage(NULL);
        fprintf(Result,"ALL TESTS PASSED\n"); fclose(Result); return 0;
    }catch(Exception &e){
        fprintf(Result,"FAIL: %s\n",AnsiString(e.Message).c_str()); fclose(Result); return 1;
    }
}
