#ifndef DetectorFmsAlarmStateH
#define DetectorFmsAlarmStateH

// Reporting only: no motion commands or safety interlock changes.
// TransportFailure sets Communication; Measurement clears it after valid data.
class DetectorFmsAlarmState {
    bool Enabled;
public:
    bool Communication, ReadException, WriteException;
    bool NotRunning, Smoke, TempWarning, TempDanger;
    DetectorFmsAlarmState(): Enabled(false), Communication(false),
        ReadException(false), WriteException(false), NotRunning(false),
        Smoke(false), TempWarning(false), TempDanger(false) {}

    void SetEnabled(bool enabled) {
        Enabled=enabled;
        if(!enabled) Communication=false;
        // Disabling a port must not acknowledge a last confirmed device alarm.
    }
    void TransportFailure() { if(Enabled) Communication=true; }
    void ExceptionResponse(bool write) {
        if(!Enabled) return;
        if(write) WriteException=true; else ReadException=true;
    }
    void WriteSucceeded() { WriteException=false; }
    bool ProtocolError() const { return ReadException || WriteException; }
    void Measurement(bool hasAlarmBits, bool running, bool smoke,
        bool warning, bool danger) {
        if(!Enabled) return;
        Communication=false;
        ReadException=false;
        NotRunning=!running;
        // Modbus bits alone set/clear physical alarms. HumanAutomation has no
        // such bits, so a mode change cannot falsely clear a recorded alarm.
        if(hasAlarmBits) {
            Smoke=smoke;
            TempWarning=warning;
            TempDanger=danger;
        }
    }
};
#endif
