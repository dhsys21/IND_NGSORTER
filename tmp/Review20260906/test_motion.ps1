$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$encoding = [Text.Encoding]::GetEncoding(949)
$source = [IO.File]::ReadAllText((Join-Path $root 'ModRobostar.cpp'), $encoding)
$bodies = @()
foreach ($signature in @('bool __fastcall Trobostar::setPoint(', 'void __fastcall Trobostar::req_Pause(')) {
    $start = $source.IndexOf($signature)
    if ($start -lt 0) { throw "Missing function: $signature" }
    $end = $source.IndexOf('//---------------------------------------------------------------------------', $start)
    if ($end -lt 0) { throw "Missing function delimiter: $signature" }
    $bodies += $source.Substring($start, $end - $start)
}
$template = [IO.File]::ReadAllText((Join-Path $PSScriptRoot 'motion_harness.cpp.in'))
# Generated harness only; the production CP949 source is never written.
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'motion_harness.cpp'),
    $template.Replace('/* PRODUCTION_FUNCTIONS */', ($bodies -join "`r`n")), $encoding)
Push-Location $PSScriptRoot
try {
    & 'C:/Program Files (x86)/Embarcadero/Studio/18.0/bin/bcc32.exe' '-tWC' '-emotion_review_test.exe' 'motion_harness.cpp'
    if ($LASTEXITCODE -ne 0) { throw 'Harness compile failed' }
    & './motion_review_test.exe'
    if ($LASTEXITCODE -ne 0) { throw 'Expected current-code behavior was not reproduced' }
} finally { Pop-Location }
