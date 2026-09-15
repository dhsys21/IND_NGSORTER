$ErrorActionPreference = 'Stop'
$repo = Split-Path $PSScriptRoot -Parent
$cp = [Text.Encoding]::GetEncoding(949)
$robot = [IO.File]::ReadAllText((Join-Path $repo 'ModRobostar.cpp'), $cp)
$body = [regex]::Match($robot, '(?ms)^int __fastcall Trobostar::UpdateEmergencyLamp\(\).*?^\}').Value
if (!$body) { throw 'Missing lamp updater.' }
$timer = [regex]::Match($robot, '(?ms)^void __fastcall Trobostar::senTimerTimer\(.*?^\}').Value
if ($timer -notmatch '(?s)if\(IsCcLinkReady\(\) && IsEmergencyStopActive\(\)\)\{.*?req_Pause\(true\);.*?UpdateEmergencyLamp\(\);\s*(?:if\(IsManualMotionStopPending\(\)\) ProcessManualMotionStop\(\);\s*)?return;\s*\}\s*UpdateEmergencyLamp\(\);') {
    throw 'Lamp must be updated after emergency stop and before other timer early returns.'
}
$fixture = @'
#include <assert.h>
#include <stdio.h>
const short DevY = 2;
int writes = 0, result = 0;
bool value = false;
short mdDevSet(long path, short station, short device, short address) {
    assert(path == 81 && station == 255 && device == DevY && address == 0x003E);
    ++writes; value = true; return result;
}
short mdDevRst(long path, short station, short device, short address) {
    assert(path == 81 && station == 255 && device == DevY && address == 0x003E);
    ++writes; value = false; return result;
}
class Trobostar {
public:
    struct { bool EMS_SWITCH_01, EMS_SWITCH_02; } input;
    struct { bool EMS_LAMP; } gripper;
    struct { long path; short stno; } config;
    bool ready;
    bool IsCcLinkReady() { return ready; }
    int __fastcall UpdateEmergencyLamp();
};
@@BODY@@
int main() {
    Trobostar r;
    r.config.path = 81; r.config.stno = 255; r.ready = true;
    for (int a = 0; a <= 1; ++a) for (int b = 0; b <= 1; ++b) {
        r.input.EMS_SWITCH_01 = a; r.input.EMS_SWITCH_02 = b;
        assert(r.UpdateEmergencyLamp() == 0);
        assert(r.gripper.EMS_LAMP == (!a || !b));
        assert(value == (!a || !b));
    }
    r.gripper.EMS_LAMP = true; r.ready = false;
    int previous = writes;
    assert(r.UpdateEmergencyLamp() == -1);
    assert(writes == previous && r.gripper.EMS_LAMP);
    r.ready = true; result = 5;
    assert(r.UpdateEmergencyLamp() == 5);
    result = 0;
    assert(r.UpdateEmergencyLamp() == 0 && writes == previous + 2);
    assert(!value && !r.gripper.EMS_LAMP);
    puts("PASS: four input combinations, link-loss hold, reconnect and failed-write retry");
    return 0;
}
'@
$out = Join-Path $repo 'Win32/Debug/EmergencyLampTest'
[void][IO.Directory]::CreateDirectory($out)
[IO.File]::WriteAllText((Join-Path $out 'test.cpp'), $fixture.Replace('@@BODY@@', $body), $cp)
Push-Location $out
try {
    & cmd.exe /d /s /c 'set "Path=" && call "C:\Program Files (x86)\Embarcadero\Studio\18.0\bin\rsvars.bat" && bcc32 -tWC -etest.exe test.cpp && test.exe'
    if ($LASTEXITCODE -ne 0) { throw 'Emergency lamp test failed.' }
} finally { Pop-Location }
Write-Output 'PASS: emergency stop executes before lamp output; Pause/BUFFER early returns covered'
