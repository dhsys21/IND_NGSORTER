#ifndef NGSorterErrorsH
#define NGSorterErrorsH
#include <string>
#include <vector>
#include <sstream>

namespace NGSorterErrors {
// FMS ERROR CODE 2026-09-09: category + at least three decimal payload digits.
// Remove the old three padding zeros: 50 + 307 = 50307 (previously 50000307).
// Payloads >= 1000 keep every digit (E40Bh -> 40 + 1035 = 401035).
// Both ErrorNo and ErrorLevel use this value; incoming FMS ErrorNo is unchanged.
inline unsigned long Encode(unsigned int category, unsigned long raw) {
    if(raw == 0) return 0;
    if(category!=10 && category!=20 && category!=30 && category!=40 && category!=50) return 0;
    unsigned long scale=1000UL;
    while(raw >= scale) {
        if(scale > 100000000UL) return 0;
        scale *= 10UL;
    }
    if(category > (4294967295UL-raw)/scale) return 0;
    return category*scale+raw;
}
inline unsigned long SystemErrorCode(unsigned short raw) {
    return (raw & 0xF000)==0xE000 ? Encode(40,raw & 0x0FFF) : 0;
}
enum Local {
    Door1=121, Door2=122, Emergency=123, KeyLock=124, ServoNotRunning=125,
    Buffer=126, CcLink=151, PlcCommunication=152, PlcError=153,
    SourceBarcode=181, TargetBarcode=182,
    Eject=201, Insert=202, Motion=203, Sequence=204, RobotInterlock=205,
    NgLimit=206, LoadFactor=207, ManualRecovery=209,
    FmsSourceLoad=301, FmsTargetLoad=302, FmsProcessStart=303,
    FmsCellTrackOut=304, FmsProcessEnd=305, FmsTrayUnload=306,
    FmsTrouble=307, ManualSourceLoad=308, ManualTargetLoad=309
};
inline int TransactionStep(int transaction) {
    const int steps[]={0,2,5,6,12,14,16};
    return transaction>=1 && transaction<=6 ? steps[transaction] : 0;
}
// Each axis/type or API operation clears only its own alarm. Arrival order is
// stable and duplicate codes remain active until their last owner recovers.
class ActiveAlarms {
    struct Owner { std::string Name; unsigned long Code;
        Owner(const std::string &n,unsigned long c):Name(n),Code(c){} };
    std::vector<Owner> Owners;
    std::vector<unsigned long> Codes;
    bool Referenced(unsigned long code) const {
        for(unsigned int i=0;i<Owners.size();++i) if(Owners[i].Code==code) return true;
        return false;
    }
public:
    bool Set(const std::string &owner,unsigned long code) {
        unsigned long old=0;
        for(unsigned int i=0;i<Owners.size();++i) if(Owners[i].Name==owner){
            old=Owners[i].Code;
            if(old==code) return false;
            Owners.erase(Owners.begin()+i); break;
        }
        bool already=Referenced(code);
        if(code) Owners.push_back(Owner(owner,code));
        if(old && !Referenced(old))
            for(unsigned int i=0;i<Codes.size();++i) if(Codes[i]==old){Codes.erase(Codes.begin()+i);break;}
        if(code && !already) Codes.push_back(code);
        return old!=code;
    }
    unsigned long First() const { return Codes.empty()?0:Codes[0]; }
    unsigned int Count() const { return Codes.size(); }
    int Status(int requested) const { return Codes.empty()?requested:4; }
    std::string Json() const {
        std::ostringstream out; out << '[';
        for(unsigned int i=0;i<Codes.size();++i){if(i) out << ',';out << Codes[i];}
        out << ']';return out.str();
    }
};
}
#endif
