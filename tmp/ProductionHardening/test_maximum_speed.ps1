$ErrorActionPreference='Stop'
$root=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$cp=[Text.Encoding]::GetEncoding(949)
$utf=New-Object Text.UTF8Encoding($false,$true)
function Source($file){
    $b=[IO.File]::ReadAllBytes((Join-Path $root $file))
    try{return $utf.GetString($b)}catch{return $cp.GetString($b)}
}
function Body($file,$signature){
    $s=Source $file; $start=$s.IndexOf($signature)
    if($start -lt 0){throw "Missing $signature"}
    $open=$s.IndexOf('{',$start); $depth=0
    for($i=$open;$i -lt $s.Length;$i++){
        if($s[$i] -eq '{'){$depth++}
        if($s[$i] -eq '}'){$depth--;if($depth -eq 0){return $s.Substring($start,$i-$start+1)}}
    }
    throw "Unclosed $signature"
}
$body=Body 'ModGripper.cpp' 'bool __fastcall Tgripper::CompleteFatPostMove('
$mode=Body 'ModGripper.cpp' 'static bool UseFatMaximumSpeedMode('
$template=[IO.File]::ReadAllText((Join-Path $PSScriptRoot 'maximum_speed_harness.cpp.in'))
$code=$template.Replace('/* POST MOVE */',$body).Replace('/* OPTION */',$mode)
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'maximum_speed_harness.cpp'),$code,$cp)
Push-Location $PSScriptRoot
try{
    & 'C:/Program Files (x86)/Embarcadero/Studio/18.0/bin/bcc32.exe' '-tWC' '-emaximum_speed_test.exe' 'maximum_speed_harness.cpp'
    if($LASTEXITCODE -ne 0){throw 'Compile failed'}
    & ./maximum_speed_test.exe
    if($LASTEXITCODE -ne 0){throw 'Regression failed'}
}finally{Pop-Location}
$insert=Body 'ModGripper.cpp' 'void __fastcall Tgripper::Inserting('
# Use the retained assignment as the unambiguous start of the FAT-only section.
$fast=$insert.Substring($insert.IndexOf('// Retain the old assignment'))
if($fast.IndexOf('pendingCellTrackOutCellId = reportCellId[0]') -gt $fast.IndexOf('StartNextCycleOrWait();')){throw 'Old assignment not retained before move'}
if($fast.IndexOf('StartNextCycleOrWait();') -gt $fast.IndexOf('CompleteFatPostMove();')){throw 'Save/report precedes FAT move'}
if($insert.Contains('MainForm->ReportCellTrackOut(reportSourceChannel')){throw 'Unchecked report remains'}
$normal=$insert.Substring($insert.IndexOf('case 4:'),$insert.IndexOf('default:', $insert.IndexOf('case 4:'))-$insert.IndexOf('case 4:'))
if($normal.IndexOf('ConsumeCellTrackOutMoveRelease()') -gt $normal.IndexOf('StartNextCycleOrWait()') -or
   $normal.IndexOf('SaveTransferResult(true)') -gt $normal.IndexOf('StartNextCycleOrWait()')){throw 'Normal order changed'}
$timer=Body 'ModGripper.cpp' 'void __fastcall Tgripper::stepTimerTimer('
if($timer.IndexOf('CompleteFatPostMove()') -gt $timer.IndexOf('Inserting();')){throw 'FAT drain does not gate next insert'}
if(!(Body 'ModGripper.cpp' 'bool __fastcall Tgripper::IsTargetTrayExchangeBoundary(').Contains('!fastPostMovePending')){throw 'Tray exchange can race FAT report'}
$init=Body 'ModGripper.cpp' 'void __fastcall Tgripper::Initialize('
if(([regex]::Matches($init,'UseFatOptimizeSequenceDelay\(\).*deferTargetReservationSave')).Count -lt 3){throw 'FAT same-scan path requires optimize-delay option'}
$request=Body 'ModRobostar.cpp' 'void __fastcall Trobostar::req_AutoEject('
if(!$request.Contains('UseFatMaximumSpeedMode()') -or !$request.Contains('CheckTrayCenteringMotionInterlock()')){throw 'Immediate request missing option/centering guard'}
$move=Body 'ModRobostar.cpp' 'void __fastcall Trobostar::AutoMove('
if(!$move.Contains('if(immediateMove && freshZ &&')){throw 'Immediate XY can use stale Z'}
Write-Output 'PASS: FAT snapshot -> next move -> save/report; normal handshake/save -> next move; next insert and tray exchange gated; fresh-Z and centering retained'
