#ifndef ProductionHistoryH
#define ProductionHistoryH
#include <System.SysUtils.hpp>
#include <vector>

struct TProductionEvent {
    UnicodeString Id, Stamp, TrayId;
    int Cells, Good, Bad;
};
struct TProductionBucket {
    TDateTime Date;
    int Hour;
    __int64 Trays, Cells, Good, Bad;
    TProductionBucket() : Date(0.0), Hour(-1), Trays(0), Cells(0), Good(0), Bad(0) {}
};
enum TProductionPeriod { ppDaily, ppWeekly, ppMonthly };

// UI-thread only. Separate from legacy dayReport counters and remeasurement CSVs.
class TProductionHistory {
private:
    UnicodeString FFolder, FCurrentId, FError;
    bool FQueued;
    std::vector<TProductionEvent> FPending;
    std::vector<TProductionEvent> ReadDay(TDateTime Day) const;
    void SaveEvent(const TProductionEvent &Event);
public:
    TProductionHistory() : FQueued(false) {}
    void Initialize(const UnicodeString &Folder);
    void BeginCycle();
    bool CompleteCycle(const UnicodeString &TrayId, int Cells, bool TestCycle,
        TDateTime CompletedAt, int Good = -1, int Bad = -1);
    bool FlushPending();
    std::vector<TProductionBucket> Query(TProductionPeriod Period, TDateTime Selected) const;
    UnicodeString Error() const { return FError; }
};
TProductionHistory &ProductionHistory();
#endif
