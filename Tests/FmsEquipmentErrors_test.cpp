#include "../NGSorterErrors.h"
#include <cassert>
#include <iostream>
using namespace NGSorterErrors;
int main() {
    assert(Encode(10,0x35)==10000053UL);
    assert(Encode(20,0x1A)==20000026UL);
    assert(Encode(30,0x1A)==30000026UL);
    assert(SystemErrorCode(0xE001)==40000001UL);
    assert(SystemErrorCode(0xE1AB)==40000427UL);
    assert(SystemErrorCode(0x000A)==0);
    assert(Encode(50,Door1)==50000121UL);
    assert(Encode(10,0)==0);
    assert(Encode(99,1)==0);
    assert(Encode(50,4294967295UL)==0);
    ActiveAlarms alarms;
    assert(alarms.Json()=="[]" && alarms.First()==0 && alarms.Status(8)==8);
    alarms.Set("servo1",20000026UL);
    alarms.Set("servo2",20000026UL);
    alarms.Set("door1",50000121UL);
    assert(alarms.Count()==2 && alarms.First()==20000026UL && alarms.Status(2)==4);
    assert(!alarms.Set("servo1",20000026UL));
    alarms.Set("servo1",0);
    assert(alarms.First()==20000026UL); // Other axis still owns this error.
    alarms.Set("servo2",0);
    assert(alarms.First()==50000121UL);
    alarms.Set("door1",0);
    assert(alarms.Json()=="[]" && alarms.First()==0 && alarms.Status(1)==1);
    alarms.Set("fms",50000304UL);
    // Closing UI, Retry and failed reads do not change the registry.
    assert(alarms.Status(8)==4 && alarms.Json()=="[50000304]");
    alarms.Set("fms",0); // Only the completed/abandoned transaction clears it.
    alarms.Set("axis1",20000022UL);
    alarms.Set("axis1",20000026UL);
    assert(alarms.Json()=="[20000026]");
    alarms.Set("axis1",0);
    for(int i=1;i<=25;++i){std::ostringstream name;name<<i;alarms.Set(name.str(),50001000UL+i);}
    assert(alarms.Count()==25 && alarms.Json().find("50001025")!=std::string::npos);
    assert(TransactionStep(1)==2 && TransactionStep(2)==5);
    assert(TransactionStep(3)==6 && TransactionStep(4)==12);
    assert(TransactionStep(5)==14 && TransactionStep(6)==16 && TransactionStep(0)==0);
    std::cout << "PASS: encoding, UInt32 bounds, duplicate owners, recovery, order, array and handshake mapping\n";
    return 0;
}
