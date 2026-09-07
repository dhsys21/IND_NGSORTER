$ErrorActionPreference='Stop'
$root=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$cp=[Text.Encoding]::GetEncoding(949)
$utf=New-Object Text.UTF8Encoding($false,$true)
function Source($file){
    $b=[IO.File]::ReadAllBytes((Join-Path $root $file))
    try{return $utf.GetString($b)}catch{return $cp.GetString($b)}
}
function Body($file,$signature){
    $s=Source $file;$start=$s.IndexOf($signature)
    if($start -lt 0){throw "Missing $signature"}
    $open=$s.IndexOf('{',$start);$depth=0
    for($i=$open;$i -lt $s.Length;$i++){
        if($s[$i] -eq '{'){$depth++};if($s[$i] -eq '}'){$depth--;if($depth -eq 0){return $s.Substring($start,$i-$start+1)}}
    }
    throw "Unclosed $signature"
}
$methods=@()
$methods+=Body 'FormManualComplete.cpp' 'void TManualCompleteForm::RefreshFmsWaitLabel('
foreach($sig in @('bool TManualCompleteForm::ContextMatches(', 'bool TManualCompleteForm::ApplyPhysicalCompletion(',
 'void TManualCompleteForm::CancelRequest(', 'void TManualCompleteForm::SendRequest(',
 'void __fastcall TManualCompleteForm::btnReportClick(', 'void __fastcall TManualCompleteForm::btnRetryClick(',
 'void __fastcall TManualCompleteForm::pollTimerTimer(', 'void __fastcall TManualCompleteForm::btnResumeClick(')){
    $methods+=Body 'FormManualComplete.cpp' $sig
}
foreach($sig in @('bool __fastcall Trobostar::PrepareCellRecovery(', 'bool __fastcall Trobostar::req_EjectComplete(', 'bool __fastcall Trobostar::req_InsertComplete(')){
    $methods+=Body 'ModRobostar.cpp' $sig
}
foreach($sig in @('void __fastcall Tgripper::ResumeCompletedCell(', 'bool __fastcall Tgripper::PrepareNextAfterManualCompletion(')){
    $methods+=Body 'ModGripper.cpp' $sig
}
$template=[IO.File]::ReadAllText((Join-Path $PSScriptRoot 'manual_completion_harness.cpp.in'))
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'manual_completion_harness.cpp'),$template.Replace('/* PRODUCTION METHODS */',($methods -join "`r`n")),$cp)
Push-Location $PSScriptRoot
try{
    & 'C:/Program Files (x86)/Embarcadero/Studio/18.0/bin/bcc32.exe' '-tWC' '-emanual_completion_test.exe' 'manual_completion_harness.cpp'
    if($LASTEXITCODE -ne 0){throw 'Compile failed'}
    & ./manual_completion_test.exe
    if($LASTEXITCODE -ne 0){throw 'Regression failed'}
}finally{Pop-Location}
foreach($f in @('eject','insert')){
    $handler=Body "FormError_$f.cpp" "void __fastcall TErrorForm_${f}::ignoreBtnClick("
    if($handler.Contains('req_Pause(false)')){throw 'Complete replays saved motion'}
    if(!(Source "FormError_$f.dfm").Contains('OnClick = btnManualCompleteClick')){throw 'Manual button not wired'}
}
foreach($sig in @('void __fastcall TMainForm::playBtnClick(', 'void __fastcall TMainForm::pause_startBtnClick(', 'bool __fastcall TMainForm::CheckServoAutoReady(', 'void __fastcall TMainForm::AdvSmoothToggleButton_InitWorkClick(')){
    if(!(Body 'FormMain.cpp' $sig).Contains('ManualCompleteForm->IsBlocking()')){throw "No recovery interlock: $sig"}
}
$form=Source 'FormManualComplete.cpp'
if(!(Body 'FormManualComplete.cpp' 'void TManualCompleteForm::Fail(').Contains('robostar->req_Pause(true)')){throw 'Recovery failure does not stop standby motion'}
$resume=Body 'Stage_mes.cpp' 'void TMainForm::ResumeAfterManualCellCompletion('
if($resume.Contains('CheckServoAutoReady(') -or !$resume.Contains('IsRecoveryStandby()') -or !$resume.Contains('!m_ServoHomeEmg')){throw 'Recovery standby/origin interlock is wrong'}
if($form.Contains('MANUAL_CELL_OUT') -or $form.Contains('ManualCellOut.')){throw 'Wrong FMS protocol'}
if(!(Body 'FormManualComplete.cpp' 'bool TManualCompleteForm::SaveJournal(').Contains('MOVEFILE_WRITE_THROUGH')){throw 'Journal is not durable'}
foreach($lang in @('En','Ko','Hi')){
    $s=Source "Lang_$lang.ini"
    foreach($match in [regex]::Matches($form,'RecoveryText\("([A-Z_]+)"\)')){
        if(!$s.Contains($match.Groups[1].Value+'=')){throw "Missing $lang translation: $($match.Groups[1].Value)"}
    }
    foreach($match in [regex]::Matches($form,'"(MSG_MC_FMS_[A-Z_]+|MSG_MC_HANDSHAKE_WAIT)"')){
        if(!$s.Contains($match.Groups[1].Value+'=')){throw "Missing $lang wait-state translation: $($match.Groups[1].Value)"}
    }
}
if(!(Body 'FormManualComplete.cpp' 'void TManualCompleteForm::RefreshControls(').Contains('RefreshFmsWaitLabel()')){throw 'Wait label is not updated on state changes'}
$dfm=Source 'FormManualComplete.dfm'
function Rect($name){
    $m=[regex]::Match($dfm,"(?s)object ${name}: \w+\s+Left = (\d+)\s+Top = (\d+)\s+Width = (\d+)\s+Height = (\d+)")
    if(!$m.Success){throw "Missing bounds: $name"}
    return @{x=[int]$m.Groups[1].Value;y=[int]$m.Groups[2].Value;w=[int]$m.Groups[3].Value;h=[int]$m.Groups[4].Value}
}
$state=Rect 'lblFmsState';$detail=Rect 'lblStatus';$report=Rect 'btnReport';$resume=Rect 'btnResume'
$clientHeight=[int][regex]::Match($dfm,'ClientHeight = (\d+)').Groups[1].Value
if($state.y+$state.h -gt $detail.y -or $detail.y+$detail.h -gt $report.y -or $report.y+$report.h -gt $resume.y -or $resume.y+$resume.h -gt $clientHeight){throw 'FMS status label overlaps details or buttons'}
Write-Output 'PASS: wait/reset/error/complete label transitions, reset retry text, three languages and DFM layout bounds'
Write-Output 'PASS: UI wiring, automatic-start/reset guards, translations, durable journal and CellTrackOut-only protocol'
