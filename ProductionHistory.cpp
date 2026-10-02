#include <vcl.h>
#pragma hdrstop
#include "ProductionHistory.h"
#include <System.DateUtils.hpp>
#include <System.JSON.hpp>
#include <memory>
#include <set>
#pragma package(smart_init)

static TProductionHistory ProductionInstance;
TProductionHistory &ProductionHistory() { return ProductionInstance; }

static TDateTime EventTime(const UnicodeString &Stamp)
{
    if(Stamp.Length() != 14) throw Exception("Invalid production timestamp.");
    for(int i = 1; i <= 14; ++i)
        if(Stamp[i] < '0' || Stamp[i] > '9') throw Exception("Invalid production timestamp.");
    return EncodeDateTime(StrToInt(Stamp.SubString(1,4)), StrToInt(Stamp.SubString(5,2)),
        StrToInt(Stamp.SubString(7,2)), StrToInt(Stamp.SubString(9,2)),
        StrToInt(Stamp.SubString(11,2)), StrToInt(Stamp.SubString(13,2)), 0);
}

void TProductionHistory::Initialize(const UnicodeString &Folder)
{
    FFolder = IncludeTrailingPathDelimiter(Folder);
    FCurrentId = "";
    FQueued = false;
    FError = "";
    FPending.clear();
}

void TProductionHistory::BeginCycle()
{
    // Step_Ready calls this once for a newly accepted physical tray. Retests keep the ID.
    FCurrentId = "";
    FQueued = false;
    TGUID guid;
    if(CreateGUID(guid) == 0) FCurrentId = GUIDToString(guid);
    else FError = "Cannot create production cycle ID.";
}

std::vector<TProductionEvent> TProductionHistory::ReadDay(TDateTime Day) const
{
    if(FFolder.IsEmpty()) throw Exception("Production history is not initialized.");
    UnicodeString day = Day.FormatString("yyyymmdd");
    UnicodeString file = FFolder + day + ".json";
    std::vector<TProductionEvent> events;
    DWORD attributes = GetFileAttributesW(file.c_str());
    if(attributes == INVALID_FILE_ATTRIBUTES){
        DWORD error = GetLastError();
        if(error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) return events;
        throw Exception("Cannot read production history: " + file);
    }
    std::auto_ptr<TStringList> text(new TStringList());
    text->LoadFromFile(file, TEncoding::UTF8);
    std::auto_ptr<TJSONValue> parsed(TJSONObject::ParseJSONValue(text->Text));
    TJSONObject *root = dynamic_cast<TJSONObject*>(parsed.get());
    TJSONArray *items = root ? dynamic_cast<TJSONArray*>(root->GetValue("events")) : NULL;
    TJSONNumber *schema = root ? dynamic_cast<TJSONNumber*>(root->GetValue("schema")) : NULL;
    if(!items || !schema || (schema->Value() != "1" && schema->Value() != "2" && schema->Value() != "3"))
        throw Exception("Invalid production history: " + file);
    std::set<UnicodeString> ids;
    for(int i = 0; i < items->Count; ++i){
        TJSONObject *item = dynamic_cast<TJSONObject*>(items->Items[i]);
        TJSONString *id = item ? dynamic_cast<TJSONString*>(item->GetValue("id")) : NULL;
        TJSONString *stamp = item ? dynamic_cast<TJSONString*>(item->GetValue("stamp")) : NULL;
        TJSONString *tray = item ? dynamic_cast<TJSONString*>(item->GetValue("trayId")) : NULL;
        TJSONNumber *cells = item ? dynamic_cast<TJSONNumber*>(item->GetValue("cells")) : NULL;
        if(!id || !stamp || !tray || !cells) throw Exception("Incomplete production entry: " + file);
        TProductionEvent event;
        event.Id = id->Value(); event.Stamp = stamp->Value(); event.TrayId = tray->Value();
        event.Cells = StrToIntDef(cells->Value(), -1);
        // Per production reporting policy, cells without a recorded verdict count as OK.
        // Schema 1 has totals only; schema 2 also has an obsolete unknown count.
        event.Good = event.Cells; event.Bad = 0;
        int legacyUnknown = 0;
        if(schema->Value() != "1"){
            TJSONNumber *good = dynamic_cast<TJSONNumber*>(item->GetValue("good"));
            TJSONNumber *bad = dynamic_cast<TJSONNumber*>(item->GetValue("bad"));
            if(!good || !bad) throw Exception("Missing production quality counts: " + file);
            event.Good = StrToIntDef(good->Value(), -1);
            event.Bad = StrToIntDef(bad->Value(), -1);
            if(schema->Value() == "2"){
                TJSONNumber *unknown = dynamic_cast<TJSONNumber*>(item->GetValue("unknown"));
                if(!unknown) throw Exception("Missing legacy production quality count: " + file);
                legacyUnknown = StrToIntDef(unknown->Value(), -1);
            }
        }
        if(event.Id.IsEmpty() || !ids.insert(event.Id).second || event.Cells < 1 || event.Cells > 96 ||
            event.Good < 0 || event.Good > 96 || event.Bad < 0 || event.Bad > 96 ||
            legacyUnknown < 0 || legacyUnknown > 96 || event.Good + event.Bad + legacyUnknown != event.Cells ||
            EventTime(event.Stamp).FormatString("yyyymmdd") != day)
            throw Exception("Invalid production entry: " + file);
        event.Good += legacyUnknown;
        events.push_back(event);
    }
    return events;
}

void TProductionHistory::SaveEvent(const TProductionEvent &Event)
{
    TDateTime day = DateOf(EventTime(Event.Stamp));
    std::vector<TProductionEvent> events = ReadDay(day);
    for(unsigned int i = 0; i < events.size(); ++i){
        if(events[i].Id == Event.Id){
            if(events[i].Cells != Event.Cells || events[i].Stamp != Event.Stamp || events[i].TrayId != Event.TrayId ||
                events[i].Good != Event.Good || events[i].Bad != Event.Bad)
                throw Exception("Conflicting production cycle ID.");
            return; // A retry after a successful write never counts the same cycle twice.
        }
    }
    events.push_back(Event);
    std::auto_ptr<TJSONObject> root(new TJSONObject());
    root->AddPair("schema", new TJSONNumber(3));
    TJSONArray *items = new TJSONArray();
    root->AddPair("events", items);
    for(unsigned int i = 0; i < events.size(); ++i){
        TJSONObject *item = new TJSONObject();
        items->AddElement(item);
        item->AddPair("id", events[i].Id);
        item->AddPair("stamp", events[i].Stamp);
        item->AddPair("trayId", events[i].TrayId);
        item->AddPair("cells", new TJSONNumber(events[i].Cells));
        item->AddPair("good", new TJSONNumber(events[i].Good));
        item->AddPair("bad", new TJSONNumber(events[i].Bad));
    }
    if(!ForceDirectories(FFolder)) throw Exception("Cannot create production history folder.");
    UnicodeString file = FFolder + day.FormatString("yyyymmdd") + ".json";
    UnicodeString temporary = file + ".tmp";
    std::auto_ptr<TStringList> text(new TStringList());
    text->Text = root->ToJSON();
    text->SaveToFile(temporary, TEncoding::UTF8);
    // Replace only a completely written document. Leave the previous valid day file on failure.
    if(!MoveFileExW(temporary.c_str(), file.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
        throw Exception("Cannot save production history: " + file);
}

bool TProductionHistory::CompleteCycle(const UnicodeString &TrayId, int Cells, bool TestCycle,
    TDateTime CompletedAt, int Good, int Bad)
{
    // Called only after FMS ProcessEnd success, never from ProcessIR/OCV or WriteResultFile.
    // Count occupied cells (including final NG), not 96 slots unconditionally.
    if(TestCycle || Cells == 0) return FlushPending();
    if(!FQueued){
        if(Good == -1 && Bad == -1){ Good = 0; Bad = 0; }
        if(FCurrentId.IsEmpty() || Cells < 1 || Cells > 96){
            FError = "Missing production cycle or invalid cell count.";
            return false;
        }
        if(Good < 0 || Bad < 0 || Good > Cells || Bad > Cells || Good + Bad > Cells){
            FError = "Invalid production quality counts.";
            return false;
        }
        TProductionEvent event;
        event.Id = FCurrentId; event.Stamp = CompletedAt.FormatString("yyyymmddhhnnss");
        event.TrayId = TrayId; event.Cells = Cells;
        // Keep explicit NG only; any remaining unclassified occupied cells are OK.
        event.Good = Cells - Bad; event.Bad = Bad;
        FPending.push_back(event);
        FQueued = true;
    }
    return FlushPending();
}

bool TProductionHistory::FlushPending()
{
    try{
        // Retry failed writes on the next completion or production-panel refresh.
        // Keep the original completion time and cycle ID; never silently drop a failed entry.
        while(!FPending.empty()){
            SaveEvent(FPending.front());
            FPending.erase(FPending.begin());
        }
        FError = "";
        return true;
    }catch(Exception &e){
        FError = e.Message;
        return false;
    }
}

std::vector<TProductionBucket> TProductionHistory::Query(TProductionPeriod Period, TDateTime Selected) const
{
    TDateTime first = DateOf(Selected);
    int count = 24;
    if(Period == ppWeekly){ first = IncDay(first, 1 - DayOfTheWeek(first)); count = 7; }
    else if(Period == ppMonthly){ first = StartOfTheMonth(first); count = DaysInAMonth(YearOf(first), MonthOf(first)); }
    std::vector<TProductionBucket> result(count);
    for(int i = 0; i < count; ++i){
        result[i].Date = Period == ppDaily ? first : IncDay(first, i);
        result[i].Hour = Period == ppDaily ? i : -1;
    }
    int days = Period == ppDaily ? 1 : count;
    for(int d = 0; d < days; ++d){
        std::vector<TProductionEvent> events = ReadDay(IncDay(first,d));
        for(unsigned int e = 0; e < events.size(); ++e){
            int index = Period == ppDaily ? HourOf(EventTime(events[e].Stamp)) : d;
            ++result[index].Trays;
            result[index].Cells += events[e].Cells;
            result[index].Good += events[e].Good;
            result[index].Bad += events[e].Bad;
        }
    }
    return result;
}
