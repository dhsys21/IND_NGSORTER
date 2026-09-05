$ErrorActionPreference = 'Stop'
$taskRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$taskEncoding = [Text.Encoding]::GetEncoding(949)
function Get-TaskFunction([string]$file,[string]$signature) {
    $s=[IO.File]::ReadAllText((Join-Path $taskRoot $file),$taskEncoding)
    $a=$s.IndexOf($signature); if($a -lt 0){throw "Missing $signature"}
    $b=$s.IndexOf('//---------------------------------------------------------------------------',$a)
    return $s.Substring($a,$b-$a)
}
$bodies=@()
foreach($name in @('PROCESS_START','PROCESS_END','TRAY_UNLOAD')) {
    $bodies+=Get-TaskFunction 'ModMes_OPCUA.cpp' ('void __fastcall TMesOpc::'+$name+'_REQUEST()')
}
foreach($name in @('PROCESS_START','PROCESS_END','TRAY_UNLOAD','CELL_TRACK_OUT')) {
    $bodies+=Get-TaskFunction 'ModMes_OPCUA.cpp' ('int __fastcall TMesOpc::'+$name+'_RESPONSE_RESULT()')
}
$bodies+=Get-TaskFunction 'ModMes_OPCUA.cpp' 'void __fastcall TMesOpc::TRAY_LOAD_REQUEST(bool SourceTray)'
$trayResponse=Get-TaskFunction 'ModMes_OPCUA.cpp' 'int __fastcall TMesOpc::TRAY_LOAD_RESPONSE(bool SourceTray)'
# Exercise the actual response classification before the separate tray-map validation.
$boundary=$trayResponse.IndexOf('TRAY_INFO *Tray = TrayFor(SourceTray);')
if($boundary -lt 0){throw 'Tray validation boundary missing'}
$bodies+=$trayResponse.Substring(0,$boundary)+"return 1;`r`n}`r`n"
$bodies+=Get-TaskFunction 'FormMain.cpp' 'bool __fastcall TMainForm::ProcessFmsAlarmRecovery()'
$s=[IO.File]::ReadAllText((Join-Path $taskRoot 'ModMes_OPCUA.cpp'),$taskEncoding)
if($s -match 'WaitResponseIdle|ResponseRevision|FMS_POLL_REQUEST_STARTED|GetFmsTagRevision'){throw 'Old response gate remains'}
if($s -notmatch 'ValidateTargetTrackInCells\(false\)'){throw 'Target validation removed'}
$main=[IO.File]::ReadAllText((Join-Path $taskRoot 'FormMain.cpp'),$taskEncoding)
if(([regex]::Matches($main,'if\s*\(response == 0 \|\| cycleResponseBypass\)')).Count -ne 5){throw 'Post-result reset branches changed'}
$template=[IO.File]::ReadAllText((Join-Path $PSScriptRoot 'current_response_harness.cpp.in'))
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'current_response_harness.cpp'),
    $template.Replace('/* PRODUCTION_FUNCTIONS */',($bodies -join "`r`n")),$taskEncoding)
Push-Location $PSScriptRoot
try {
    & 'C:\Program Files (x86)\Embarcadero\Studio\18.0\bin\bcc32.exe' '-tWC' '-ecurrent_response_test.exe' 'current_response_harness.cpp'
    if($LASTEXITCODE -ne 0){throw 'Compile failed'}
    & '.\current_response_test.exe'
    if($LASTEXITCODE -ne 0){throw 'Regression failed'}
} finally {Pop-Location}
