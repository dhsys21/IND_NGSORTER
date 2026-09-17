$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$out = Join-Path $root 'Win32/Debug/PeripheralFmsAlarmsTest'
[void][IO.Directory]::CreateDirectory($out)
Push-Location $out
try {
    foreach($test in @('FmsEquipmentErrors_test','PeripheralFmsAlarms_test')) {
        $src = Join-Path $PSScriptRoot ($test+'.cpp')
        & cmd.exe /d /s /c ('set "Path=" && call "C:\Program Files (x86)\Embarcadero\Studio\18.0\bin\rsvars.bat" && bcc32 -tWC -e'+$test+'.exe "'+$src+'" && '+$test+'.exe')
        if($LASTEXITCODE -ne 0) {throw "Failed: $test"}
    }
} finally {Pop-Location}
$cp = [Text.Encoding]::GetEncoding(949)
$smoke = [IO.File]::ReadAllText((Join-Path $root 'SmokeDetector_comm.cpp'),$cp)
$main = [IO.File]::ReadAllText((Join-Path $root 'FormMain.cpp'),[Text.Encoding]::UTF8)
foreach($key in @('SmokeCommunication','SmokeProtocol','SmokeNotRunning','SmokeDetected','SmokeTempWarning','SmokeTempDanger')) {
    if(!$smoke.Contains('MesOpc->SetLocalAlarm(NGSorterErrors::'+$key+',')) {throw "Missing publisher: $key"}
}
if(!$main.Contains('if(plcWasReady) MesOpc->SetLocalAlarm(NGSorterErrors::PowerMeterData,!plcReady);')) {throw 'Missing fresh PLC meter guard'}
if(!$main.Contains('if(comSmoke[0] != NULL) comSmoke[0]->PublishFmsAlarms();')) {throw 'Wrong detector instance'}
$human = [regex]::Match($smoke,'(?ms)^void __fastcall TSmokeDetector::Parse_HumanAuto\(.*?^\}').Value
foreach($key in @('bWaitingResponse = false;','failCount = 0;','m_fmsAlarms.Measurement(false, true, false, false, false);')) {
    if(!$human.Contains($key)) {throw "HumanAutomation recovery missing: $key"}
}
Write-Output 'PASS: production alarm publishers, PLC freshness, real detector instance and human response bookkeeping'
