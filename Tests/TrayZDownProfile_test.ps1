$ErrorActionPreference = 'Stop'
$repo = Split-Path $PSScriptRoot -Parent
$enc = [Text.Encoding]::GetEncoding(949)
$robot = [IO.File]::ReadAllText((Join-Path $repo 'ModRobostar.cpp'), $enc)
$teaching = [IO.File]::ReadAllText((Join-Path $repo 'FormTeaching.cpp'), $enc)
$config = [IO.File]::ReadAllText((Join-Path $repo 'FormConfig.cpp'), $enc)

$start = [regex]::Match($robot,
    '(?ms)^bool __fastcall Trobostar::StartZDownProfile\(.*?^\}').Value
if (!$start) { throw 'StartZDownProfile missing.' }
if ($start -notmatch '(?s)activeMove\.pallet == 1.*?setZPoint\(zDownFinalPosition, zSpeed80\)') {
    throw 'Source tray must use one full descent at the fast Z speed.'
}
if ($start -notmatch 'activeMove\.pallet != 2' -or
    $start -notmatch 'zDownApproachPosition = targetZSlowStartPosition' -or
    $start -match 'travel \* 80') {
    throw 'Target tray must use the registered absolute slow-start Z position.'
}
foreach($key in @('TARGET_Z_SLOW_START_POS','SetTargetZSlowStartPosition')) {
    if (!$config.Contains($key)) { throw "Config persistence missing: $key" }
}
if (!$teaching.Contains('editTargetSlowZ') -or
    !$teaching.Contains('MSG_TARGET_Z_SLOW_POS_RANGE')) {
    throw 'Teaching dialog absolute Target Z input/validation missing.'
}
Write-Output 'PASS: Source uses one-stage descent; Target uses a persisted absolute slow-start Z.'
