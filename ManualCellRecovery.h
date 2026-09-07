#ifndef ManualCellRecoveryH
#define ManualCellRecoveryH
// Manual completion owns one immutable CellTrackOut until response reset.
enum TManualCellPhase { mcIdle, mcPrepared, mcClearForSend, mcWaitResult,
    mcAccepted, mcReady, mcMoving };
enum TManualReply { mrWait, mrSuccess, mrFailure, mrReset, mrInvalid };
inline TManualReply ManualCellReply(TManualCellPhase phase, int response)
{
    if(phase == mcWaitResult){
        if(response == 1) return mrSuccess;
        if(response == 2) return mrFailure;
        return response == 0 ? mrWait : mrInvalid;
    }
    if(phase == mcAccepted || phase == mcClearForSend)
        return response == 0 ? mrReset : mrWait;
    return mrWait;
}
#endif
