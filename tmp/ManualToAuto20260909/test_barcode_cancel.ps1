$ErrorActionPreference='Stop'
$root=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$encoding=[Text.Encoding]::GetEncoding(949)
$source=[IO.File]::ReadAllText((Join-Path $root 'Mod_SRX100W.cpp'),$encoding)
$methods=''
foreach($name in @('CancelScan','GetBarcode','ReadTimeoutTimer')){
 $a=$source.IndexOf('void __fastcall TMod_Bcr::'+$name+'(')
 $b=$source.IndexOf("`n}",$a)
 if($a -lt 0 -or $b -lt 0){throw "Missing $name"}
 $methods+=$source.Substring($a,$b-$a+2)+"`r`n"
}
$template=[IO.File]::ReadAllText((Join-Path $PSScriptRoot 'barcode_cancel.cpp.in'))
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'barcode_cancel.cpp'),$template.Replace('/* METHODS */',$methods),$encoding)
Push-Location $PSScriptRoot
try{
 & 'C:/Program Files (x86)/Embarcadero/Studio/18.0/bin/bcc32.exe' '-tWC' '-ebarcode_cancel.exe' 'barcode_cancel.cpp'
 if($LASTEXITCODE -ne 0){throw 'Barcode test compile failed'}
 & './barcode_cancel.exe'
 if($LASTEXITCODE -ne 0){throw 'Barcode cancellation regression failed'}
}finally{Pop-Location}
$a=$source.IndexOf('void __fastcall TMod_Bcr::ClientSocketBcrRead(')
$rx=$source.Substring($a)
if(!$rx.Contains('if(!bReading){ rxBuffer = ""; return; }') -or !$rx.Contains('if(!bReading) return;')){throw 'Cancelled receive may leak into AUTO'}
Write-Output 'PASS: cancelled TCP receive/result guarded before barcode callback'
