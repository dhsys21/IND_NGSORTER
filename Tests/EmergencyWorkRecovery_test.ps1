$ErrorActionPreference = 'Stop'
$repo = Split-Path $PSScriptRoot -Parent
$enc = [Text.Encoding]::GetEncoding(949)
$main = [IO.File]::ReadAllText((Join-Path $repo 'FormMain.cpp'), [Text.Encoding]::UTF8)
$robot = [IO.File]::ReadAllText((Join-Path $repo 'ModRobostar.cpp'), $enc)
$grip = [IO.File]::ReadAllText((Join-Path $repo 'ModGripper.cpp'), $enc)
$config = [IO.File]::ReadAllText((Join-Path $repo 'FormConfig.cpp'), $enc)
$dfm = [IO.File]::ReadAllText((Join-Path $repo 'FormConfig.dfm'))
function Body($text, $signature) {
    $v = [regex]::Match($text, '(?ms)^' + [regex]::Escape($signature) + '.*?^\}').Value
    if (!$v) { throw "Missing function $signature" }
    return $v
}
$ready = Body $main 'bool TMainForm::CheckEmergencyRecoveryReady('
foreach ($guard in @('EmergencyHomeDone','AreAxesStopped','IsSafetyReady','IsKeyLockActive',
    'IsSourceTrayIn','IsTargetTrayIn','IsSourceCentering','IsTargetCentering',
    'IsPlcStatusFresh','opcCellTrackOutPending','opcProcessStarted','ValidateEmergencyRecord')) {
    if (!$ready.Contains($guard)) { throw "Missing EMG gate $guard" }
}
$resume = Body $grip 'bool Tgripper::ResumeEmergencyCheckpoint('
foreach ($forbidden in @('TRAY_LOAD_REQUEST','PROCESS_START_REQUEST','req_Pause(false)', 'sscAutoStart')) {
    if ($resume.Contains($forbidden)) { throw "Recovery replays old work: $forbidden" }
}
if (!$resume.Contains('robostar->req_Stop()') -or !$resume.Contains('step.step = 4')) {
    throw 'Recovery must abandon old motion and use the report-only checkpoint.'
}
foreach ($fn in @('void __fastcall TMainForm::autoBtnClick(',
    'void __fastcall TMainForm::playBtnClick(', 'void __fastcall TMainForm::pause_startBtnClick(')) {
    $body = Body $main $fn
    if (!$body.Contains('emergencyAutoRestart')) { throw "Missing option guard in $fn" }
}
$sensor = Body $robot 'void __fastcall Trobostar::senTimerTimer('
if ($sensor.IndexOf('ObserveEmergency') -gt $sensor.IndexOf('ProcessBufferRecovery')) {
    throw 'EMG must be captured before BUFFER/motion recovery.'
}
if (!$config.Contains('ReadBool("RECOVERY", "EMERGENCY_AUTO_RESTART", false)') -or
    !$config.Contains('WriteBool("RECOVERY", "EMERGENCY_AUTO_RESTART"')) {
    throw 'Recovery setting must persist and default OFF.'
}
if ($dfm -notmatch '(?s)\r?\n  object chkEmergencyAutoRestart: TCheckBox\r?\n    Left = 500\r?\n    Top = 489') {
    throw 'EMG setting must be a form-level checkbox below Target Tray Unload.'
}
Write-Output 'PASS: EMG gates, no old-motion/TrayLoad replay, report checkpoint, settings/UI placement'
