#include "../EmergencyWorkRecovery.h"
#include <assert.h>
#include <stdio.h>

int main()
{
    // Empty before pickup, held after CHUCK, and released after OPEN acknowledgement.
    assert(SelectEmergencyCheckpoint(true,false,false,false,false,false,false,true,false)==erSource);
    assert(SelectEmergencyCheckpoint(false,true,false,true,false,true,true,false,false)==erTarget);
    assert(SelectEmergencyCheckpoint(false,true,true,true,false,true,false,true,true)==erReport);
    assert(SelectEmergencyCheckpoint(false,true,false,true,true,false,false,true,true)==erReport);
    // Lost cell is never another pickup; an interrupted output switch is ambiguous.
    assert(SelectEmergencyCheckpoint(true,true,false,true,false,false,false,true,false)==erBlocked);
    assert(SelectEmergencyCheckpoint(false,true,false,true,false,true,true,false,true)==erBlocked);
    assert(SelectEmergencyCheckpoint(false,false,false,false,false,true,false,true,true)==erBlocked);
    // Without placement acknowledgement, an empty gripper never proves insertion.
    assert(SelectEmergencyCheckpoint(false,true,false,true,false,false,false,true,true)==erBlocked);
    for(int mask=0; mask<512; ++mask){
        bool v[9];
        for(int i=0;i<9;++i) v[i]=(mask & (1<<i))!=0;
        EmergencyCheckpoint p=SelectEmergencyCheckpoint(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8]);
        if(v[6] && v[7]) assert(p==erBlocked);
        if(p==erTarget) assert(v[1] && v[5] && v[6] && !v[7] && !v[8]);
        if(p==erReport) assert((v[2] || v[4]) && v[7] && !v[6]);
        if(p==erSource) assert(v[0] && !v[3] && !v[5] && v[7] && !v[6]);
    }
    puts("PASS: EMG checkpoint selection, held-cell loss, ambiguous release, sensor conflicts");
    return 0;
}
