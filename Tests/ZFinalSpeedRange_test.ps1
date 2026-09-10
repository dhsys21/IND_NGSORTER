$ErrorActionPreference = 'Stop'
$repo = Split-Path $PSScriptRoot -Parent
$enc = [Text.Encoding]::GetEncoding(949)
$robot = [IO.File]::ReadAllText((Join-Path $repo 'ModRobostar.cpp'), $enc)
$teaching = [IO.File]::ReadAllText((Join-Path $repo 'FormTeaching.cpp'), $enc)

$setRange = [regex]::Match($robot,
    '(?ms)^bool __fastcall Trobostar::SetZSpeeds\(.*?^\}').Value
if (!$setRange) { throw 'SetZSpeeds function missing.' }
if ($setRange -notmatch 'speed20 < 100 \|\| speed20 > 500') {
    throw 'Runtime Z final 20% range is not 100-500.'
}
if ($setRange -notmatch 'speed20 > speed80') {
    throw 'Z final speed must remain no faster than the first 80% speed.'
}
if ($teaching -notmatch 'Z_FINAL_SPEED_MIN = 100;' -or
    $teaching -notmatch 'Z_FINAL_SPEED_MAX = 500;' -or
    $teaching -notmatch 'z20 < Z_FINAL_SPEED_MIN \|\| z20 > Z_FINAL_SPEED_MAX') {
    throw 'Teaching dialog Z final speed validation is incomplete.'
}
Write-Output 'PASS: Z final 20% speed range is 100-500 in UI and runtime validation.'
