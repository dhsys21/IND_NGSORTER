param([ValidateSet('prepare','apply')][string]$Mode)
$ErrorActionPreference = 'Stop'
$root = Split-Path (Split-Path $PSScriptRoot -Parent) -Parent
$cp = [Text.Encoding]::GetEncoding(949,[Text.EncoderFallback]::ExceptionFallback,[Text.DecoderFallback]::ExceptionFallback)
$utf = New-Object Text.UTF8Encoding($false,$true)
foreach($name in @('SmokeDetector_comm.cpp','SmokeDetector_comm.h')) {
    $file = Join-Path $root $name
    $backup = Join-Path $PSScriptRoot ($name+'.original')
    $stage = Join-Path $PSScriptRoot $name
    if($Mode -eq 'prepare') {
        if(Test-Path -LiteralPath $backup) {throw "Existing backup: $backup"}
        Copy-Item -LiteralPath $file -Destination $backup
        [IO.File]::WriteAllText($stage,[IO.File]::ReadAllText($file,$cp),$utf)
    } else {
        if((Get-FileHash -LiteralPath $file).Hash -ne (Get-FileHash -LiteralPath $backup).Hash) {throw "Source changed: $name"}
        $s = [IO.File]::ReadAllText($stage,$utf).Replace("`r`n","`n").Replace("`n","`r`n")
        $bytes = $cp.GetBytes($s)
        if($cp.GetString($bytes) -cne $s) {throw "Encoding loss: $name"}
        [IO.File]::WriteAllBytes($file,$bytes)
    }
}
