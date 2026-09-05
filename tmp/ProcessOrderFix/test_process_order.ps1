$ErrorActionPreference='Stop'
$root=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$enc=[Text.Encoding]::GetEncoding(949)
function Body([string]$file,[string]$name) {
    $s=[IO.File]::ReadAllText((Join-Path $root $file),$enc)
    $a=$s.IndexOf($name); if($a -lt 0){throw "Missing $name"}
    $b=$s.IndexOf('//---------------------------------------------------------------------------',$a)
    $s.Substring($a,$b-$a)
}
$bodies=@()
foreach($name in @('IsSourceTrayInSignal','IsSourceCenteringSignal','IsTargetTrayInSignal','IsTargetCenteringSignal','CanRequestAutoSourceCentering','IsSourceTrayCycleReady','UpdateSourceTrayAdmission','CompleteSourceCenteringStep','CheckWorkTraySignals','RetryWorkStartTrayAlarm')) {
    $bodies+=Body 'FormMain.cpp' ('bool __fastcall TMainForm::'+$name+'(')
}
$bodies+=Body 'Stage_mes.cpp' 'void __fastcall TMainForm::TryStartOpcProcess()'
$restart=Body 'FormMain.cpp' 'void __fastcall TMainForm::pause_startBtnClick('
if($restart.IndexOf('RetryWorkStartTrayAlarm()') -gt $restart.IndexOf('gripper->req_Pause(false)')){throw 'Restart releases motion before interlock retry'}
$advance=Body 'Stage_mes.cpp' 'void __fastcall TMainForm::AdvanceOpcTrayLoad('
if($advance.Contains('ReadTargetTrayBarcode()') -or $advance.Contains('CmdSourceCenteringRequest(true)')) {throw 'Out-of-order STEP02 action'}
$step=Body 'FormMain.cpp' 'void __fastcall TMainForm::stepTimerTimer('
if(!$step.Contains('if(!CompleteSourceCenteringStep()) break;') -or !$step.Contains('if(IsTargetTrayInSignal() && IsTargetCenteringSignal())')){throw 'STEP03/04 gate missing'}
$poll=Body 'FormMain.cpp' 'void __fastcall TMainForm::opcMesTimerTimer('
if(!$poll.Contains('CheckWorkTraySignals(7, traySignalState)')){throw 'Post-FMS local-start check missing'}
foreach($name in @('ReissueFmsAlarmRequest','ResumeAutomaticFmsSequence')){
    if(!(Body 'FormMain.cpp' ('void __fastcall TMainForm::'+$name+'(')).Contains('CheckWorkTraySignals(6, traySignalState)')) {throw 'Retry bypass'}
}
$template=[IO.File]::ReadAllText((Join-Path $PSScriptRoot 'process_order_harness.cpp.in'))
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'process_order_harness.cpp'),$template.Replace('/* PRODUCTION_FUNCTIONS */',($bodies -join "`r`n")),$enc)
Push-Location $PSScriptRoot
try {
    & 'C:/Program Files (x86)/Embarcadero/Studio/18.0/bin/bcc32.exe' '-tWC' '-eprocess_order_test.exe' 'process_order_harness.cpp'
    if($LASTEXITCODE -ne 0){throw 'Compile failed'}
    & './process_order_test.exe'
    if($LASTEXITCODE -ne 0){throw 'Regression failed'}
} finally {Pop-Location}
