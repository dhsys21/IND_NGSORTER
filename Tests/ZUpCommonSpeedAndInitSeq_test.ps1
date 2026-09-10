$ErrorActionPreference = 'Stop'
$repo = Split-Path $PSScriptRoot -Parent
$enc = [Text.Encoding]::GetEncoding(949)
$robot = [IO.File]::ReadAllText((Join-Path $repo 'ModRobostar.cpp'), $enc)
$main = [IO.File]::ReadAllText((Join-Path $repo 'FormMain.cpp'), $enc)

$speed = [regex]::Match($robot,
    '(?ms)^void __fastcall Trobostar::req_Speed\(.*?^\}').Value
if (!$speed -or $speed -match 'point\[0\]\.speed\s*=\s*zSpeed80') {
    throw 'Z UP still overrides the common X/Y speed.'
}
if ($speed -notmatch '(?s)for\(int i=0; i<=servoCnt; \+\+i\).*?point\[i\]\.speed = speed') {
    throw 'Common speed is not applied to the Z UP point.'
}
$zSpeeds = [regex]::Match($robot,
    '(?ms)^bool __fastcall Trobostar::SetZSpeeds\(.*?^\}').Value
if ($zSpeeds -match 'point\[0\]\.speed') {
    throw 'Changing Z DOWN speed must not change Z UP speed.'
}
$buffer = [regex]::Match($robot,
    '(?ms)^bool Trobostar::StartBufferRecoveryZUp\(\).*?^\}').Value
if ($buffer -match 'recoveryPoint\.speed\s*=\s*zSpeed80') {
    throw 'BUFFER recovery Z UP still uses the dedicated down speed.'
}

$handler = [regex]::Match($main,
    '(?ms)^void __fastcall TMainForm::btnTrayStepInitClick\(.*?^\}').Value
if (!$handler -or $handler -notmatch 'IsAtWaitPosition\(\)') {
    throw 'INIT SEQ wait-position check is missing.'
}
$beforeReset = $handler.Substring(0, $handler.IndexOf('int sourceStep'))
foreach($oldGuard in @('equipMode != modeManual','IsCcLinkReady','getCellDetectStatus',
    'IsSortingWorkActive','IsTargetTrayExchangeActive','opcCellTrackOutPending',
    'ManualCompleteForm','DryRunForm')) {
    if ($beforeReset.Contains($oldGuard)) { throw "Unexpected INIT SEQ guard: $oldGuard" }
}
foreach($cancel in @('CELL_TRACK_OUT_CANCEL','PROCESS_END_CANCEL','TRAY_UNLOAD_CANCEL')) {
    if (!$handler.Contains($cancel)) { throw "INIT SEQ request cancellation missing: $cancel" }
}
Write-Output 'PASS: Z UP uses common speed; INIT SEQ is guarded only by stopped WAIT position.'
