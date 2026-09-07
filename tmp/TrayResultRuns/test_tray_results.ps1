$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$cp = [Text.Encoding]::GetEncoding(949, [Text.EncoderFallback]::ExceptionFallback, [Text.DecoderFallback]::ExceptionFallback)
$utf = New-Object Text.UTF8Encoding($false, $true)
$source = [IO.File]::ReadAllText((Join-Path $root 'Stage_Form.cpp'), $cp)
$start = $source.IndexOf('static AnsiString MakeSafeTrayFileId(')
$end = $source.IndexOf('AnsiString __fastcall TMainForm::GetSourceTrayInfoFile(', $start)
if ($start -lt 0 -or $end -lt 0) { throw 'Result method boundaries not found' }
$methods = $source.Substring($start, $end - $start)
$template = [IO.File]::ReadAllText((Join-Path $PSScriptRoot 'tray_result_harness.cpp.in'), $utf)
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'tray_result_harness.cpp'), $template.Replace('/* PRODUCTION METHODS */', $methods), $cp)
Push-Location $PSScriptRoot
try {
    & 'C:/Program Files (x86)/Embarcadero/Studio/18.0/bin/bcc32.exe' '-tCVM' '-etray_result_test.exe' 'tray_result_harness.cpp' 'rtl.lib' 'vcl.lib'
    if ($LASTEXITCODE -ne 0) { throw 'Result test build failed' }
    & ./tray_result_test.exe
    if ($LASTEXITCODE -ne 0) { throw 'Result regression failed' }
} finally { Pop-Location }
foreach ($file in @('FormMain.cpp', 'Stage_mes.cpp')) {
    $text = [IO.File]::ReadAllText((Join-Path $root $file))
    if ($text.Contains('MarkSourceSortStart();')) { throw 'Sort start still recorded before NG selection' }
}
$gripper = [IO.File]::ReadAllText((Join-Path $root 'ModGripper.cpp'), $cp)
if ($gripper.IndexOf('MainForm->MarkSourceSortStart();', $gripper.IndexOf('Tgripper::BeginTransferResult(')) -lt 0) {
    throw 'Selected-cell transfer does not record sorting start'
}
Write-Output 'PASS: sorting timestamps begin with a selected-cell transfer, not ProcessStart initialization'
