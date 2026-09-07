$ErrorActionPreference='Stop'
$root=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$cp=[Text.Encoding]::GetEncoding(949)
function Method($file,$signature){
    $s=[IO.File]::ReadAllText((Join-Path $root $file),$cp)
    $start=$s.IndexOf($signature);if($start -lt 0){throw "Missing $signature"}
    $end=$s.IndexOf("`n}",$start)
    return $s.Substring($start,$end-$start+2)
}
$header=[IO.File]::ReadAllText((Join-Path $root 'ModPLC_Bin.h'),$cp)
$constants=([regex]::Matches($header,'(?m)^const int PLC_D_(?:INTERFACE_LEN|METER_\w+)[^;]*;')|ForEach-Object {$_.Value}) -join "`n"
$methods=(Method 'ModPLC_Bin.cpp' 'static double DecodePlcMeterValue(')+"`n"+
    (Method 'ModPLC_Bin.cpp' 'bool __fastcall TPlcBin::GetPowerMeterInfo(')+"`n"+
    (Method 'ModMes_OPCUA.cpp' 'void __fastcall TMesOpc::PublishPowerMeter(')
$template=[IO.File]::ReadAllText((Join-Path $PSScriptRoot 'power_meter_harness.cpp.in'))
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'power_meter_harness.cpp'),$template.Replace('/* CONSTANTS */',$constants).Replace('/* METHODS */',$methods))
Push-Location $PSScriptRoot
try{
    & 'C:/Program Files (x86)/Embarcadero/Studio/18.0/bin/bcc32.exe' '-tWC' '-epower_meter_test.exe' 'power_meter_harness.cpp'
    if($LASTEXITCODE -ne 0){throw 'Power meter test build failed'}
    & ./power_meter_test.exe
    if($LASTEXITCODE -ne 0){throw 'Power meter regression failed'}
}finally{Pop-Location}
