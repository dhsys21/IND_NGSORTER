$ErrorActionPreference='Stop'
$root=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$utf=New-Object Text.UTF8Encoding($false,$true)
$cp=[Text.Encoding]::GetEncoding(949)
function Source($file){$b=[IO.File]::ReadAllBytes((Join-Path $root $file));try{return $utf.GetString($b)}catch{return $cp.GetString($b)}}
function Body($file,$signature){
    $s=Source $file;$start=$s.IndexOf($signature);if($start -lt 0){throw "Missing $signature"}
    $end=$s.IndexOf("`n}",$start)
    return $s.Substring($start,$end-$start+2)
}
$methods=(Body 'FormMain.cpp' 'void __fastcall TMainForm::ResetTrayLoadTransaction(')+"`n"+
    (Body 'FormMain.cpp' 'bool __fastcall TMainForm::CheckTrayLoadPresence(')
$template=[IO.File]::ReadAllText((Join-Path $PSScriptRoot 'presence_harness.cpp.in'))
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'presence_harness.cpp'),$template.Replace('/* METHODS */',$methods))
Push-Location $PSScriptRoot
try{
    & 'C:/Program Files (x86)/Embarcadero/Studio/18.0/bin/bcc32.exe' '-tWC' '-epresence_test.exe' 'presence_harness.cpp'
    if($LASTEXITCODE -ne 0){throw 'Presence test compile failed'}
    & ./presence_test.exe
    if($LASTEXITCODE -ne 0){throw 'Presence test failed'}
}finally{Pop-Location}
foreach($method in @('pause_startBtnClick','ConfirmFmsAlarmRetry','ReissueFmsAlarmRequest',
    'ProcessFmsAlarmRecovery','ResumeAutomaticFmsSequence','opcMesTimerTimer','senTimerTimer')){
    $s=Source 'FormMain.cpp';$start=$s.IndexOf('TMainForm::'+$method+'(');$end=$s.IndexOf("`n}",$start)
    if(!$s.Substring($start,$end-$start).Contains('CheckTrayLoadPresence(')){throw "Missing presence gate: $method"}
}
foreach($method in @('NotifyTrayInfo','AdvanceOpcTrayLoad')){
    if(!(Body 'Stage_mes.cpp' ('void __fastcall TMainForm::'+$method+'(')).Contains('CheckTrayLoadPresence(')){throw "Missing gate: $method"}
}
$reset=Body 'FormMain.cpp' 'void __fastcall TMainForm::AdvSmoothToggleButton_InitWorkClick('
foreach($text in @('ResetTrayLoadTransaction(true)','ResetTrayLoadTransaction(false)',
    'PROCESS_START_CANCEL()','opcFmsSuspendedByManual = false','fmsAlarmTransaction == fmsAlarmCellTrackOut')){
    if(!$reset.Contains($text)){throw "Init Work missing cleanup/protection: $text"}
}
Write-Output 'PASS: Restart, Retry, recovery, auto resume, response polling, manual monitoring, initial dispatch, completion and Init Work wiring'
