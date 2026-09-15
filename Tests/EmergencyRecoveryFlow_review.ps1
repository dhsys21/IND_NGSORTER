$ErrorActionPreference = 'Stop'
$repo = Split-Path $PSScriptRoot -Parent
$enc = [Text.Encoding]::GetEncoding(949)
$main = [IO.File]::ReadAllText((Join-Path $repo 'FormMain.cpp'), [Text.Encoding]::UTF8)
$robot = [IO.File]::ReadAllText((Join-Path $repo 'ModRobostar.cpp'), $enc)
$grip = [IO.File]::ReadAllText((Join-Path $repo 'ModGripper.cpp'), $enc)
$mes = [IO.File]::ReadAllText((Join-Path $repo 'Stage_mes.cpp'), $enc)
function Extract($source, $signature) {
    $m = [regex]::Match($source, '(?ms)^' + [regex]::Escape($signature) + '.*?^\}')
    if (!$m.Success) { throw "Missing function $signature" }
    return $m.Value
}
$bodies = @()
foreach ($name in @('ClearEmergencyRecovery','EmergencyHomeCompleted','ResumeEmergencyCheckpoint')) {
    $ret = if ($name -eq 'ResumeEmergencyCheckpoint') {'bool'} else {'void'}
    $bodies += Extract $grip "$ret Tgripper::$name("
}
foreach ($name in @('InitSequence','req_Pause')) { $bodies += Extract $grip "void __fastcall Tgripper::$name(" }
$bodies += Extract $grip 'bool __fastcall Tgripper::StartPendingCellTrackOutReport('
foreach ($name in @('req_Stop','InitSequence','req_Home','Home','req_Pause')) { $bodies += Extract $robot "void __fastcall Trobostar::$name(" }
$bodies += Extract $robot 'void Trobostar::MotionFault('
$bodies += Extract $robot 'bool Trobostar::StopAxes('
foreach ($name in @('CheckEmergencyRecoveryReady','StartEmergencyRecovery','HandleEmergencyFmsAcknowledgement')) { $bodies += Extract $main "bool TMainForm::$name(" }
$bodies += Extract $main 'void __fastcall TMainForm::stepTimerTimer('
$bodies += Extract $mes 'bool __fastcall TMainForm::ConsumeCellTrackOutMoveRelease('
$insert = Extract $grip 'void __fastcall Tgripper::Inserting('
$steps = [regex]::Match($insert,'(?ms)^\s*case 4:.*?(?=^\s*default:)').Value
if (!$steps) { throw 'Missing report steps' }
$fixture = [IO.File]::ReadAllText((Join-Path $PSScriptRoot 'EmergencyRecoveryFlow_review.cpp.in'))
$fixture = $fixture.Replace('@@PRODUCTION_BODIES@@', ($bodies -join "`r`n")).Replace('@@REPORT_STEPS@@', $steps)
$timer = Extract $robot 'void __fastcall Trobostar::senTimerTimer('
$watchdog = [regex]::Match($timer, '(?ms)^\s*static robotSequence watchedSeq.*?(?=^\s*// While the door)').Value
if (!$watchdog) { throw 'Missing motion watchdog' }
$fixture = $fixture.Replace('@@HOME_WATCHDOG@@', $watchdog)
foreach($fn in @('pause_startBtnClick','playBtnClick')) {
    $body = Extract $main "void __fastcall TMainForm::$fn("
    if($body.IndexOf('HandleEmergencyFmsAcknowledgement()') -lt 0 -or
       $body.IndexOf('HandleEmergencyFmsAcknowledgement()') -ge $body.IndexOf('StartEmergencyRecovery()')) {
        throw 'EMG entry must handle FMS acknowledgement before attempting motion recovery.'
    }
}
$restart = Extract $main 'void __fastcall TMainForm::pause_startBtnClick('
$branch = [regex]::Match($restart, '(?ms)^\tif\(gripper->EmergencyPending\(\).*?^\t\}').Value
if (!$branch) { throw 'Missing EMG restart entry' }
$fixture = $fixture.Replace('@@EMG_START_BRANCH@@', $branch)
$alarm = [IO.File]::ReadAllText((Join-Path $repo 'FormAlarm_fms.cpp'), $enc)
if (!(Extract $alarm 'void __fastcall TAlarmForm_fms::btnRetryClick(').Contains('pause_startBtnClick')) {
    throw 'FMS popup Retry must share the verified Main Restart path.'
}
$out = Join-Path $repo 'Win32/Debug/EmergencyRecoveryReview'
[void][IO.Directory]::CreateDirectory($out)
# Generated fixture only; production CP949 files are never rewritten.
[IO.File]::WriteAllText((Join-Path $out 'review.cpp'), $fixture, $enc)
Copy-Item -LiteralPath (Join-Path $repo 'EmergencyWorkRecovery.h') -Destination $out
Push-Location $out
try {
    & cmd.exe /d /s /c 'set "Path=" && call "C:\Program Files (x86)\Embarcadero\Studio\18.0\bin\rsvars.bat" && bcc32 -tWC -ereview.exe review.cpp && review.exe'
    if ($LASTEXITCODE -ne 0) { throw 'Review harness failed' }
} finally { Pop-Location }
