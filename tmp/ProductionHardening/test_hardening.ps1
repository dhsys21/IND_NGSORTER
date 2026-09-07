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
    # All extracted functions have balanced braces, including comments/strings.
    for($i=$open;$i -lt $s.Length;$i++){
        if($s[$i] -eq '{'){$depth++};if($s[$i] -eq '}'){$depth--;if($depth -eq 0){return $s.Substring($start,$i-$start+1)}}
    }
    throw "Unclosed $signature"
}
$motion=@()
foreach($sig in @('bool __fastcall Trobostar::setPoint(', 'void __fastcall Trobostar::req_Pause(',
 'bool Trobostar::StopAxes(', 'bool Trobostar::AreAxesStopped(', 'bool Trobostar::CanResumeMotion(',
 'void Trobostar::MotionFault(')){$motion+=Body 'ModRobostar.cpp' $sig}
$template=[IO.File]::ReadAllText((Join-Path $PSScriptRoot 'hardening_harness.cpp.in'))
$gateway=Body 'ModMes_Gateway.cpp' 'void TMod_Fms::AcknowledgePcTags('
$gateway+="`r`n"+(Body 'ModMes_Gateway.cpp' 'bool TMod_Fms::IsPcTagWriteComplete(')
$save=Body 'ModGripper.cpp' 'bool __fastcall Tgripper::SavePendingTransferResult('
$source=$template.Replace('/* MOTION */',($motion -join "`r`n")).Replace('/* ACK */',$gateway).Replace('/* SAVE */',$save)
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'hardening_harness.cpp'),$source,$cp)
Push-Location $PSScriptRoot
try{
    & 'C:/Program Files (x86)/Embarcadero/Studio/18.0/bin/bcc32.exe' '-tWC' '-ehardening_test.exe' 'hardening_harness.cpp'
    if($LASTEXITCODE -ne 0){throw 'Compile failed'}
    & ./hardening_test.exe
    if($LASTEXITCODE -ne 0){throw 'Regression failed'}
}finally{Pop-Location}
$g=Source 'ModMes_Gateway.cpp';$m=Source 'ModGripper.cpp';$b=Source 'Mod_SRX100W.cpp'
if((Body 'ModMes_Gateway.cpp' 'void __fastcall TMod_Fms::FlushPendingPcTags(').Contains('IOHandler')){throw 'UI thread socket write'}
if((Body 'ModMes_Gateway.cpp' 'UnicodeString __fastcall TMod_Fms::BuildSuccessResponse(').Contains('FPendingPcTags.clear()')){throw 'Premature pending clear'}
if(!$g.Contains('Id_SO_SNDTIMEO, 2000')){throw 'Unbounded socket send'}
if(!(Body 'ModGripper.cpp' 'static bool UseFatMaximumSpeedMode(').Contains('BaseForm->config.maximumSpeedMode')){throw 'FAT option is not connected'}
if(!(Body 'Stage_mes.cpp' 'bool __fastcall TMainForm::ReportCellTrackOut(').Contains('if(opcCellTrackOutPending) return false;')){throw 'Single report overwrite guard missing'}
if($b.Contains('ProcessResult(rxBuffer)') -or !(Body 'Mod_SRX100W.cpp' 'void __fastcall TMod_Bcr::ProcessResult(').Contains('if(!bReading) return;')){throw 'Partial/late barcode accepted'}
$resume=Body 'FormMain.cpp' 'void __fastcall TMainForm::ResumeAutomaticFmsSequence('
foreach($x in @('opcTrayLoadWaitResponseOff[i]','opcProcessStartWaitResponseOff','opcCellTrackOutWaitResponseOff','opcProcessEndWaitResponseOff','opcTargetUnloadWaitResponseOff')){
    if(!$resume.Contains('if(!'+$x+')') -and !$resume.Contains('if('+$x+') continue')){throw "Accepted phase discarded: $x"}
}
if(!(Body 'Stage_mes.cpp' 'bool __fastcall TMainForm::ReportCellTrackOut(').Contains('ReadApprovedSource(')){throw 'Live TrackIn reread'}
$opc=Source 'ModMes_OPCUA.cpp'
if(!$opc.Contains('Duplicate Source CellNo=') -or !$opc.Contains('Duplicate Target CellNo=')){throw 'Duplicate CellNo validation missing'}
Write-Output 'PASS: async-only sender, bounded send, immutable source, duplicate CellNo, accepted handshake phase, barcode and maximum-speed gates'
