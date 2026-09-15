#ifndef EmergencyWorkRecoveryH
#define EmergencyWorkRecoveryH

//* 비상정지후 취출/삽입 계속작업.
// Capture once at EMG; never infer a completed insert from an empty sensor alone.
enum EmergencyCheckpoint { erBlocked, erSource, erTarget, erReport };
inline EmergencyCheckpoint SelectEmergencyCheckpoint(bool sourceMove,
    bool pickupConfirmed, bool releaseConfirmed, bool sourceCommitted,
    bool targetCommitted, bool cell, bool chuck, bool open, bool commandOpen)
{
    if(chuck && open) return erBlocked;
    if(targetCommitted || releaseConfirmed)
        return open && !chuck ? erReport : erBlocked;
    if(pickupConfirmed && cell && chuck && !open && !commandOpen)
        return erTarget;
    if(sourceMove && !sourceCommitted && !cell && open && !chuck)
        return erSource;
    return erBlocked;
}
#endif
