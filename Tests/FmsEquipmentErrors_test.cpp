#include "../NGSorterErrors.h"
#include <cassert>
#include <iostream>
#include <set>
using namespace NGSorterErrors;
int main() {
    assert(Encode(10,0x35)==10053UL);
    assert(Encode(20,0x1A)==20026UL);
    assert(Encode(30,0x1A)==30026UL);
    assert(SystemErrorCode(0xE001)==40001UL);
    assert(SystemErrorCode(0xE1AB)==40427UL);
    assert(SystemErrorCode(0x000A)==0);
    assert(Encode(50,Door1)==50121UL);
    assert(Encode(10,0)==0);
    assert(Encode(99,1)==0);
    assert(Encode(50,4294967295UL)==0);
    assert(Encode(50,FmsTrouble)==50307UL);
    assert(Encode(50,ManualSourceLoad)==50308UL);
    assert(Encode(10,1)==10001UL);
    assert(Encode(20,999)==20999UL);
    assert(Encode(20,1000)==201000UL);
    assert(Encode(20,0x888)==202184UL);
    assert(SystemErrorCode(0xE40B)==401035UL);
    // No truncation, cross-category collisions or loss of the two-digit prefix.
    std::set<unsigned long> encoded;
    for(unsigned int category=10;category<=50;category+=10)
        for(unsigned long raw=1;raw<=65535UL;++raw) {
            unsigned long value=Encode(category,raw);
            assert(value!=0 && encoded.insert(value).second);
            std::ostringstream wire; wire << value;
            std::ostringstream prefix; prefix << category;
            assert(wire.str().substr(0,2)==prefix.str());
        }
    ActiveAlarms alarms;
    assert(alarms.Json()=="[]" && alarms.First()==0 && alarms.Status(8)==8);
    alarms.Set("servo1",20026UL);
    alarms.Set("servo2",20026UL);
    alarms.Set("door1",50121UL);
    assert(alarms.Count()==2 && alarms.First()==20026UL && alarms.Status(2)==4);
    assert(!alarms.Set("servo1",20026UL));
    alarms.Set("servo1",0);
    assert(alarms.First()==20026UL); // Other axis still owns this error.
    alarms.Set("servo2",0);
    assert(alarms.First()==50121UL);
    alarms.Set("door1",0);
    assert(alarms.Json()=="[]" && alarms.First()==0 && alarms.Status(1)==1);
    alarms.Set("fms",50304UL);
    // Closing UI, Retry and failed reads do not change the registry.
    assert(alarms.Status(8)==4 && alarms.Json()=="[50304]");
    alarms.Set("fms",0); // Only the completed/abandoned transaction clears it.
    alarms.Set("axis1",20022UL);
    alarms.Set("axis1",20026UL);
    assert(alarms.Json()=="[20026]");
    alarms.Set("axis1",0);
    for(int i=1;i<=25;++i){std::ostringstream name;name<<i;alarms.Set(name.str(),Encode(50,1000+i));}
    assert(alarms.Count()==25 && alarms.Json().find("501025")!=std::string::npos);
    assert(TransactionStep(1)==2 && TransactionStep(2)==5);
    assert(TransactionStep(3)==6 && TransactionStep(4)==12);
    assert(TransactionStep(5)==14 && TransactionStep(6)==16 && TransactionStep(0)==0);
    std::cout << "PASS: encoding, UInt32 bounds, duplicate owners, recovery, order, array and handshake mapping\n";
    return 0;
}
