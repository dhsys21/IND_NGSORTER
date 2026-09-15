$ErrorActionPreference='Stop'
$repo=Split-Path $PSScriptRoot -Parent
$cp=[Text.Encoding]::GetEncoding(949)
$robot=[IO.File]::ReadAllText((Join-Path $repo 'ModRobostar.cpp'),$cp)
function Extract($source,$signature){
    $m=[regex]::Match($source,'(?ms)^'+[regex]::Escape($signature)+'.*?^\}')
    if(!$m.Success){throw "Missing $signature"};return $m.Value
}
$bodies=@()
foreach($signature in @('bool Trobostar::RequestManualMotionStop(', 'void Trobostar::ProcessManualMotionStop(',
    'bool Trobostar::StopAxes(', 'bool Trobostar::AreAxesStopped(',
    'void __fastcall Trobostar::InitSequence(', 'void __fastcall Trobostar::req_Stop(')){$bodies+=Extract $robot $signature}
$fixture=[IO.File]::ReadAllText((Join-Path $PSScriptRoot 'ManualMotionStop_test.cpp.in'))
$fixture=$fixture.Replace('@@BODIES@@',($bodies -join "`r`n"))
$out=Join-Path $repo 'Win32/Debug/ManualMotionStopTest'
[void][IO.Directory]::CreateDirectory($out)
[IO.File]::WriteAllText((Join-Path $out 'test.cpp'),$fixture,$cp)
Push-Location $out
try{
    & cmd.exe /d /s /c 'set "Path=" && call "C:\Program Files (x86)\Embarcadero\Studio\18.0\bin\rsvars.bat" && bcc32 -tWC -etest.exe test.cpp && test.exe'
    if($LASTEXITCODE -ne 0){throw 'Manual stop harness failed.'}
}finally{Pop-Location}
$timer=Extract $robot 'void __fastcall Trobostar::senTimerTimer('
if($timer.IndexOf('ProcessManualMotionStop();') -gt $timer.IndexOf('ProcessBufferRecovery();')){throw 'Cancellation must prevent automatic buffer retraction.'}
foreach($signature in @('void __fastcall Trobostar::req_Pause(', 'bool Trobostar::CanResumeMotion(',
    'bool __fastcall Trobostar::setPoint(', 'void __fastcall Trobostar::GripperChuck(')){
    if(!(Extract $robot $signature).Contains('IsManualMotionStopPending()')){throw "Missing pending guard: $signature"}
}
$teaching=[IO.File]::ReadAllText((Join-Path $repo 'FormTeaching.cpp'),$cp)
$stop=Extract $teaching 'void __fastcall TteachForm::stopBtnClick('
if(!$stop.Contains('RequestManualMotionStop()') -or $stop.Contains('req_Stop()') -or $stop.Contains('Visible = false')){throw 'Teaching must use confirmed cancellation.'}
$main=[IO.File]::ReadAllText((Join-Path $repo 'FormMain.cpp'),[Text.Encoding]::UTF8)
if(!(Extract $main 'bool __fastcall TMainForm::CheckServoAutoReady(').Contains('IsManualMotionStopPending()')){throw 'AUTO must remain blocked until stop confirmed.'}
Write-Output 'PASS: production entry guards and confirmed-stop wiring'
