$ErrorActionPreference = 'Stop'
Set-Location (Split-Path $PSScriptRoot -Parent)
$utf = [Text.UTF8Encoding]::new($false, $true)
$source = [IO.File]::ReadAllText((Join-Path (Get-Location) 'Stage_production.cpp'), $utf)
$methods = $source.Substring($source.IndexOf('void __fastcall TMainForm::BeginProductionSourceCycle'))
$fixture = [IO.File]::ReadAllText((Join-Path $PSScriptRoot 'SourceProduction_test.cpp.in'), $utf)
[IO.File]::WriteAllText((Join-Path (Get-Location) 'tmp/Access20261001/source_test.cpp'), $fixture.Replace('// SOURCE_METHODS', $methods), $utf)
& cmd /c tmp\Access20261001\build_source_test.cmd
if($LASTEXITCODE -ne 0){throw 'Source production test build failed'}
$p = Start-Process -FilePath (Join-Path (Get-Location) 'tmp/Access20261001/source_test.exe') -WorkingDirectory (Get-Location) -WindowStyle Hidden -Wait -PassThru
Get-Content tmp/Access20261001/source-result.txt
if($p.ExitCode -ne 0){throw 'Source production test failed'}
