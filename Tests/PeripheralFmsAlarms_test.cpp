#include "../DetectorFmsAlarmState.h"
#include "../NGSorterErrors.h"
#include <cassert>
#include <iostream>

int main() {
    DetectorFmsAlarmState s;
    s.TransportFailure();
    assert(!s.Communication && !s.ProtocolError() && !s.NotRunning);
    s.SetEnabled(true);
    s.TransportFailure();
    assert(s.Communication && !s.NotRunning); // no device status before first read
    s.Measurement(true, true, false, false, false);
    assert(!s.Communication && !s.NotRunning && !s.Smoke);
    s.Measurement(true, false, true, true, true);
    assert(s.NotRunning && s.Smoke && s.TempWarning && s.TempDanger);
    s.TransportFailure();
    assert(s.Communication && s.Smoke && s.TempWarning && s.TempDanger);
    s.SetEnabled(false);
    assert(!s.Communication && s.NotRunning && s.Smoke && s.TempDanger);
    s.SetEnabled(true);
    s.Measurement(false, true, false, false, false);
    assert(!s.Communication && !s.NotRunning && s.Smoke && s.TempDanger);
    s.Measurement(true, true, false, false, false);
    assert(!s.Smoke && !s.TempWarning && !s.TempDanger);

    s.ExceptionResponse(true);
    s.ExceptionResponse(false);
    assert(s.ProtocolError());
    s.Measurement(true, true, false, false, false);
    assert(!s.ReadException && s.WriteException && s.ProtocolError());
    s.ExceptionResponse(false);
    s.WriteSucceeded();
    assert(s.ReadException && !s.WriteException && s.ProtocolError());
    s.Measurement(false, true, false, false, false);
    assert(!s.ProtocolError());

    NGSorterErrors::ActiveAlarms alarms;
    alarms.Set("Local:414",NGSorterErrors::Encode(50,NGSorterErrors::SmokeDetected));
    alarms.Set("Local:411",NGSorterErrors::Encode(50,NGSorterErrors::SmokeCommunication));
    assert(alarms.Json()=="[50414,50411]" && alarms.First()==50414UL && alarms.Status(8)==4);
    alarms.Set("Local:411",0);
    assert(alarms.Json()=="[50414]" && alarms.Status(2)==4);
    alarms.Set("Local:414",0);
    assert(alarms.Json()=="[]" && alarms.First()==0 && alarms.Status(8)==8);
    std::cout << "PASS: detector communication, retained alarms, independent read/write recovery, human mode and FMS status\n";
}
