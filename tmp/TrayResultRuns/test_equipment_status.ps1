$ErrorActionPreference='Stop'
$root=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
function Method($file,$signature) {
    $s=[IO.File]::ReadAllText((Join-Path $root $file))
    $start=$s.IndexOf($signature)
    if($start -lt 0){throw "Missing $signature"}
    $end=$s.IndexOf("`n}",$start)
    return $s.Substring($start,$end-$start+2)
}
$methods=(Method 'FormMain.cpp' 'void __fastcall TMainForm::UpdateFmsEquipmentStatus()')+"`n"+
    (Method 'ModMes_OPCUA.cpp' 'void __fastcall TMesOpc::PublishEquipmentStatus(')
$template=[IO.File]::ReadAllText((Join-Path $PSScriptRoot 'equipment_status_harness.cpp.in'))
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'equipment_status_harness.cpp'),$template.Replace('/* METHODS */',$methods))
Push-Location $PSScriptRoot
try {
    & 'C:/Program Files (x86)/Embarcadero/Studio/18.0/bin/bcc32.exe' '-tWC' '-eequipment_status_test.exe' 'equipment_status_harness.cpp'
    if($LASTEXITCODE -ne 0){throw 'Status test compilation failed'}
    & ./equipment_status_test.exe
    if($LASTEXITCODE -ne 0){throw 'Status test failed'}
} finally {Pop-Location}
