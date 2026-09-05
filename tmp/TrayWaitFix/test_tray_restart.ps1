$ErrorActionPreference = 'Stop'
$taskRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$taskEncoding = [Text.Encoding]::GetEncoding(949)
function Get-TaskFunction([string]$file, [string]$signature) {
    $source = [IO.File]::ReadAllText((Join-Path $taskRoot $file), $taskEncoding)
    $start = $source.IndexOf($signature)
    if ($start -lt 0) { throw "Missing function: $signature" }
    $end = $source.IndexOf('//---------------------------------------------------------------------------', $start)
    if ($end -lt 0) { throw "Missing function end: $signature" }
    return $source.Substring($start, $end - $start)
}
$functions = @(
    (Get-TaskFunction 'ModGripper.cpp' 'bool __fastcall Tgripper::IsSortingWorkActive() const'),
    (Get-TaskFunction 'Stage_mes.cpp' 'void __fastcall TMainForm::AdvanceOpcTrayLoad(bool sourceTray)'),
    (Get-TaskFunction 'Stage_mes.cpp' 'void __fastcall TMainForm::ResumeDeferredTrayLoads()'),
    (Get-TaskFunction 'Stage_mes.cpp' 'void __fastcall TMainForm::NotifyTrayInfo(AnsiString strTray, bool bsrc)')
) -join "`r`n"
$template = [IO.File]::ReadAllText((Join-Path $PSScriptRoot 'tray_restart_harness.cpp.in'))
# Compile the actual production function bodies with fake PLC/FMS/UI boundaries.
# No application process, hardware connection, or machine command is involved.
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'tray_restart_harness.cpp'),
    $template.Replace('/* PRODUCTION_FUNCTIONS */', $functions), $taskEncoding)
Push-Location $PSScriptRoot
try {
    & 'C:\Program Files (x86)\Embarcadero\Studio\18.0\bin\bcc32.exe' '-tWC' '-etray_restart_test.exe' 'tray_restart_harness.cpp'
    if ($LASTEXITCODE -ne 0) { throw 'Harness compile failed' }
    & '.\tray_restart_test.exe'
    if ($LASTEXITCODE -ne 0) { throw 'Regression failed' }
} finally { Pop-Location }
