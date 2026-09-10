$ErrorActionPreference = 'Stop'
$repo = Split-Path $PSScriptRoot -Parent
$enc = [Text.Encoding]::GetEncoding(949)
$source = [IO.File]::ReadAllText((Join-Path $repo 'ModRobostar.cpp'), $enc)
$match = [regex]::Match($source,
    '(?ms)^bool __fastcall Trobostar::ContinueZDownProfile\(\).*?^\}')
if (!$match.Success) { throw 'ContinueZDownProfile function missing.' }
$body = $match.Value
$stage = $body.IndexOf('zDownProfileStage == 1 && currentZ == zDownApproachPosition')
$opRead = $body.IndexOf('SSC_STSBIT_AX_OP', $stage)
$speedRead = $body.IndexOf('sscGetCmdSpeedFast', $stage)
$stopGuard = $body.IndexOf('moving != SSC_BIT_OFF || speed != 0', $stage)
$finalStart = $body.IndexOf('setZPoint(zDownFinalPosition, zSpeed20)', $stage)
if ($stage -lt 0 -or $opRead -lt 0 -or $speedRead -lt 0 -or
    $stopGuard -lt 0 -or $finalStart -lt 0) {
    throw 'Z final descent guard is incomplete.'
}
if (!($opRead -lt $stopGuard -and $speedRead -lt $stopGuard -and
    $stopGuard -lt $finalStart)) {
    throw 'Z final descent is started before stopped-state validation.'
}
if ($body -notmatch 'if\(moving != SSC_BIT_OFF \|\| speed != 0\)\s*return false;') {
    throw 'Moving Z must wait without issuing the final start.'
}
Write-Output 'PASS: Z final 20% starts only after AX_OP=OFF and speed=0.'
