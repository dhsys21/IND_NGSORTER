$ErrorActionPreference = 'Stop'
Set-Location (Split-Path $PSScriptRoot -Parent)
[IO.Directory]::CreateDirectory((Join-Path $PWD 'tmp\manual-complete-open')) | Out-Null
$source=[Text.Encoding]::GetEncoding(949).GetString([IO.File]::ReadAllBytes((Join-Path $PWD 'FormManualComplete.cpp')))
$controls=$source.Substring($source.IndexOf('void TManualCompleteForm::RefreshControls()'))
$controls=$controls.Substring(0,$controls.IndexOf('void TManualCompleteForm::Fail('))
$open=$source.Substring($source.IndexOf('void TManualCompleteForm::OpenManualEntry('))
$open=$open.Substring(0,$open.IndexOf('bool TManualCompleteForm::ContextMatches()'))
$manual=$source.Substring($source.IndexOf('bool TManualCompleteForm::ReadManualEntry()'))
$manual=$manual.Substring(0,$manual.IndexOf('void __fastcall TManualCompleteForm::btnReportClick('))
if($source -notmatch 'if\(IsBlocking\(\) \|\| !contextReady \|\| !chkInserted->Checked\) return;') {throw 'Report context guard missing'}
$fixture=[IO.File]::ReadAllText((Join-Path $PSScriptRoot 'ManualCompleteOpen_test.cpp.in'))
[IO.File]::WriteAllText((Join-Path $PWD 'tmp\manual-complete-open\test.cpp'),$fixture.Replace('// EXTRACTED_METHODS',$controls+$open+$manual),[Text.UTF8Encoding]::new($false))
$p=New-Object System.Diagnostics.ProcessStartInfo
$p.FileName=$env:ComSpec
$p.Arguments='/c Tests\ManualCompleteOpen_build.cmd'
$p.UseShellExecute=$false
[void]$p.EnvironmentVariables.Remove('PATH')
[void]$p.EnvironmentVariables.Remove('Path')
$p.EnvironmentVariables['Path']=[Environment]::GetEnvironmentVariable('Path')
$c=[Diagnostics.Process]::Start($p)
$c.WaitForExit()
if($c.ExitCode -ne 0){throw 'Test compile failed'}
$t=Start-Process -FilePath (Join-Path $PWD 'tmp\manual-complete-open\test.exe') -WorkingDirectory $PWD -WindowStyle Hidden -Wait -PassThru
Get-Content tmp\manual-complete-open\result.txt
if($t.ExitCode -ne 0){throw 'Test failed'}
