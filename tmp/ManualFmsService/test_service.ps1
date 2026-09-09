$ErrorActionPreference='Stop'
$taskRoot=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$taskEncoding=[Text.Encoding]::GetEncoding(949)
$taskSource=[IO.File]::ReadAllText((Join-Path $taskRoot 'Stage_mes.cpp'),$taskEncoding)
$taskA=$taskSource.IndexOf('static const UnicodeString FMS_TROUBLE_STATUS')
$taskB=$taskSource.IndexOf('void __fastcall TMainForm::InitTrayInfo(',$taskA)
if($taskA -lt 0 -or $taskB -lt 0){throw 'Service boundaries missing'}
$taskTemplate=[IO.File]::ReadAllText((Join-Path $PSScriptRoot 'service_harness.cpp.in'))
$taskMain=[IO.File]::ReadAllText((Join-Path $taskRoot 'FormMain.cpp'),$taskEncoding)
$taskResetA=$taskMain.IndexOf('void __fastcall TMainForm::ResetTrayLoadTransaction(')
$taskResetB=$taskMain.IndexOf("`n}",$taskResetA)
if($taskResetA -lt 0 -or $taskResetB -lt 0){throw 'ResetTrayLoadTransaction missing'}
$taskMethods=$taskMain.Substring($taskResetA,$taskResetB-$taskResetA+2)+"`r`n"+$taskSource.Substring($taskA,$taskB-$taskA)
foreach($name in @('autoBtnClick','playBtnClick')){
 $a=$taskMain.IndexOf('void __fastcall TMainForm::'+$name+'(');$b=$taskMain.IndexOf("`n}",$a)
 if($a -lt 0 -or $b -lt 0){throw "Missing $name"}
 $taskMethods+="`r`n"+$taskMain.Substring($a,$b-$a+2)
}
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'service_harness.cpp'),$taskTemplate.Replace('/* PRODUCTION_FUNCTIONS */',$taskMethods),$taskEncoding)
Push-Location $PSScriptRoot
try {
 & 'C:/Program Files (x86)/Embarcadero/Studio/18.0/bin/bcc32.exe' '-tWC' '-eservice_test.exe' 'service_harness.cpp'
 if($LASTEXITCODE -ne 0){throw 'Harness compile failed'}
 & './service_test.exe'
 if($LASTEXITCODE -ne 0){throw 'Service regression failed'}
} finally {Pop-Location}
$taskManual=$taskSource.Substring($taskSource.IndexOf('void __fastcall TMainForm::StartManualTrayLoad'),$taskB-$taskSource.IndexOf('void __fastcall TMainForm::StartManualTrayLoad'))
if($taskManual -match 'CmdTrayOut\(|CmdSourceCenteringRequest\(|TryStartOpcProcess\(|AdvanceOpcTrayLoad\(|req_Pause\(false\)|cbCycle'){throw 'Manual data-only boundary breached'}
Write-Output 'PASS: manual handler contains no AUTO advance, PLC output, Pause release or Cycle Test reset bypass'
$taskMain=[IO.File]::ReadAllText((Join-Path $taskRoot 'FormMain.cpp'),$taskEncoding)
$taskRobot=[IO.File]::ReadAllText((Join-Path $taskRoot 'ModRobostar.cpp'),$taskEncoding)
$taskDry=[IO.File]::ReadAllText((Join-Path $taskRoot 'FormDryRun.cpp'),$taskEncoding)
foreach($taskName in @('opcMesTimerTimer','stepTimerTimer','sourceTrayOutTimerTimer')){
 $taskAt=$taskMain.IndexOf('void __fastcall TMainForm::'+$taskName+'(')
 if($taskAt -lt 0 -or $taskMain.Substring($taskAt,300) -notmatch 'IsFmsTroubleBlocking\(\)'){throw "Missing independent gate: $taskName"}
}
foreach($taskName in @('CanResumeMotion()','InitSequence(robotSequence','setPoint(int')){
 $taskAt=$taskRobot.IndexOf('Trobostar::'+$taskName)
 if($taskAt -lt 0 -or $taskRobot.Substring($taskAt,420) -notmatch 'IsFmsTroubleBlocking\(\)'){throw "Missing motion gate: $taskName"}
}
if($taskDry -notmatch 'stepStartTick \+= \(DWORD\)\(GetTickCount\(\) - fmsPauseTick\)'){throw 'Dry-run timeout not frozen during FMS Pause'}
Write-Output 'PASS: AUTO polling, delayed tray-out, motion resume/new target and dry-run Pause boundaries'
$taskAutoA=$taskMain.IndexOf('void __fastcall TMainForm::autoBtnClick(')
$taskAutoB=$taskMain.IndexOf("`n}",$taskAutoA)
$taskAuto=$taskMain.Substring($taskAutoA,$taskAutoB-$taskAutoA)
if($taskAuto.Contains('IsManualTrayLoadBusy()') -or !$taskAuto.Contains('ResetManualTrayLoadForAuto()')){throw 'AUTO still blocked by manual transaction'}
if($taskAuto.IndexOf('CheckServoAutoReady(true)') -gt $taskAuto.IndexOf('ResetManualTrayLoadForAuto()')){throw 'Manual cleanup precedes AUTO physical interlocks'}
if(!$taskSource.Contains('manualTraySessionUsed = true;') -or !$taskMain.Contains('manualTraySessionUsed = false;')){throw 'Completed manual session does not reset at AUTO entry'}
Write-Output 'PASS: AUTO validates physical interlocks then resets manual session, including already-completed manual data'
if($taskAuto.Contains('IsFmsTroubleBlocking()')){throw 'FMS Trouble still prohibits AUTO selection'}
$a=$taskMain.IndexOf('void __fastcall TMainForm::pause_startBtnClick(');$b=$taskMain.IndexOf("`n}",$a);$restart=$taskMain.Substring($a,$b-$a)
$restart=$restart.Substring($restart.IndexOf('if(equipMode == modeAuto)'))
foreach($guard in @('CanResumeMotion()','RetryPendingTraySaves()','RetryWorkStartTrayAlarm()')){
 if(!$restart.Contains($guard) -or $restart.IndexOf($guard) -gt $restart.IndexOf('robostar->req_Pause(false)')){throw "Physical restart guard missing: $guard"}
}
Write-Output 'PASS: START reuses real physical/recovery checks; no servo, centering, pending-save or tray-input bypass'
