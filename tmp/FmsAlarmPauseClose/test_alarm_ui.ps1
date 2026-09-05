$ErrorActionPreference = 'Stop'
$taskRoot = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$enc = [Text.Encoding]::GetEncoding(949)
function Get-Body([string]$file, [string]$signature) {
    $s = [IO.File]::ReadAllText((Join-Path $taskRoot $file), $enc)
    $a = $s.IndexOf($signature)
    if ($a -lt 0) { throw "Missing $signature" }
    $b = $s.IndexOf('//---------------------------------------------------------------------------', $a)
    $s.Substring($a, $b-$a)
}
$bodies = @()
foreach ($name in @('ShowFmsError','SetRetryWaiting','btnPauseClick','btnRetryClick','SetOperatorPaused','btnCloseClick','FormClose','RefreshAlarmVisibility','FormHide')) {
    $bodies += Get-Body 'FormAlarm_fms.cpp' ('void __fastcall TAlarmForm_fms::'+$name+'(')
}
foreach ($name in @('PauseFmsAlarm','CheckFmsResetRetryTimeout','ConfirmFmsAlarmRetry')) {
    $bodies += Get-Body 'FormMain.cpp' ('void __fastcall TMainForm::'+$name+'(')
}
$dfm = [IO.File]::ReadAllText((Join-Path $taskRoot 'FormAlarm_fms.dfm'))
$header = [IO.File]::ReadAllText((Join-Path $taskRoot 'FormAlarm_fms.h'))
foreach ($eventName in @('btnPauseClick','btnRetryClick','btnCloseClick','FormClose')) {
    if (!$dfm.Contains('= '+$eventName) -or !$header.Contains($eventName+'(')) { throw "DFM event missing: $eventName" }
}
if ($dfm -match 'btnCancel|ModalResult') { throw 'Unexpected dialog action' }
$template = [IO.File]::ReadAllText((Join-Path $PSScriptRoot 'alarm_ui_harness.cpp.in'))
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'alarm_ui_harness.cpp'), $template.Replace('/* PRODUCTION_FUNCTIONS */', ($bodies -join "`r`n")), $enc)
Push-Location $PSScriptRoot
try {
    & 'C:/Program Files (x86)/Embarcadero/Studio/18.0/bin/bcc32.exe' '-tWC' '-ealarm_ui_test.exe' 'alarm_ui_harness.cpp'
    if ($LASTEXITCODE -ne 0) { throw 'Compile failed' }
    & './alarm_ui_test.exe'
    if ($LASTEXITCODE -ne 0) { throw 'Regression failed' }
} finally { Pop-Location }
