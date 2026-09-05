$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$enc = [Text.Encoding]::GetEncoding(949)
function Body([string]$file,[string]$signature) {
    $s=[IO.File]::ReadAllText((Join-Path $root $file),$enc)
    $a=$s.IndexOf($signature); if($a -lt 0){throw "Missing $signature"}
    $b=$s.IndexOf('//---------------------------------------------------------------------------',$a)
    $s.Substring($a,$b-$a)
}
$bodies=@()
foreach($name in @('IsSourceTrayInSignal','IsSourceCenteringSignal','IsTargetTrayInSignal','IsTargetCenteringSignal','CanRequestAutoSourceCentering','IsSourceTrayCycleReady','UpdateSourceTrayAdmission')) {
    $bodies+=Body 'FormMain.cpp' ('bool __fastcall TMainForm::'+$name+'(')
}
$main=[IO.File]::ReadAllText((Join-Path $root 'FormMain.cpp'),$enc)
$stage=[IO.File]::ReadAllText((Join-Path $root 'Stage_mes.cpp'),$enc)
$dfm=[IO.File]::ReadAllText((Join-Path $root 'FormMain.dfm'),$enc)
if($main.Contains('cbMES') -or $main.Contains('sourceTraySimulated') -or $dfm.Contains('cbMES')){throw 'Legacy simulation remains'}
if(!$dfm.Contains('object chkDoorPlcAuto: TCheckBox') -or !$dfm.Contains("Caption = 'Door/Auto'")){throw 'Component rename incomplete'}
if($stage.Contains('CmdSourceCenteringRequest(true)')){throw 'FMS re-centering bypass remains'}
$step=Body 'FormMain.cpp' 'void __fastcall TMainForm::stepTimerTimer('
if($step.IndexOf('if(!UpdateSourceTrayAdmission()) return;') -gt $step.IndexOf('ReadSourceTrayBarcode()')){throw 'Barcode precedes admission'}
if($step.IndexOf('CLEAR_TRACK_OUT_CELL_INFORMATION()') -gt $step.IndexOf('ReadSourceTrayBarcode()')){throw 'Payload reset after barcode'}
foreach($name in @('AdvanceOpcTrayLoad','ResumeDeferredTrayLoads','TryStartOpcProcess','NotifyTrayInfo')) {
    if(!(Body 'Stage_mes.cpp' ('void __fastcall TMainForm::'+$name+'(')).Contains('IsSourceTrayCycleReady()')){throw "Missing callback admission gate: $name"}
}
# The motion-stop algorithms must be byte-equivalent to baseline after comments/whitespace.
foreach($file in @('ModRobostar.cpp','FormDryRun.cpp')) {
    $prior = (& git -C $root show ('HEAD:'+$file)) -join "`n"
    $current = [IO.File]::ReadAllText((Join-Path $root $file),$enc)
    # Limit this assertion to the diff: the only added lines must be comments.
    $added = & git -C $root diff --unified=0 -- $file
    foreach($line in $added) {
        if($line.StartsWith('+') -and !$line.StartsWith('+++') -and $line -notmatch '^\+\s*//'){throw "Motion logic changed: $file $line"}
        if($line.StartsWith('-') -and !$line.StartsWith('---')){throw "Motion logic removed: $file"}
    }
}
$template=[IO.File]::ReadAllText((Join-Path $PSScriptRoot 'admission_harness.cpp.in'))
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'admission_harness.cpp'),$template.Replace('/* PRODUCTION_FUNCTIONS */',($bodies -join "`r`n")),$enc)
Push-Location $PSScriptRoot
try {
    & 'C:/Program Files (x86)/Embarcadero/Studio/18.0/bin/bcc32.exe' '-tWC' '-eadmission_test.exe' 'admission_harness.cpp'
    if($LASTEXITCODE -ne 0){throw 'Compile failed'}
    & './admission_test.exe'
    if($LASTEXITCODE -ne 0){throw 'Regression failed'}
} finally {Pop-Location}
