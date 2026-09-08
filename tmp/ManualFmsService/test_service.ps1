$ErrorActionPreference='Stop'
$taskRoot=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$taskEncoding=[Text.Encoding]::GetEncoding(949)
$taskSource=[IO.File]::ReadAllText((Join-Path $taskRoot 'Stage_mes.cpp'),$taskEncoding)
$taskA=$taskSource.IndexOf('static const UnicodeString FMS_TROUBLE_STATUS')
$taskB=$taskSource.IndexOf('void __fastcall TMainForm::InitTrayInfo(',$taskA)
if($taskA -lt 0 -or $taskB -lt 0){throw 'Service boundaries missing'}
$taskTemplate=[IO.File]::ReadAllText((Join-Path $PSScriptRoot 'service_harness.cpp.in'))
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'service_harness.cpp'),$taskTemplate.Replace('/* PRODUCTION_FUNCTIONS */',$taskSource.Substring($taskA,$taskB-$taskA)),$taskEncoding)
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
