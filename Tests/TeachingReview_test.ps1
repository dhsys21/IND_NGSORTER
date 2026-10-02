$ErrorActionPreference='Stop'
Set-Location (Split-Path $PSScriptRoot -Parent)
$root=$PWD.Path
$dir=Join-Path $root 'tmp\TeachingReview20261002'
[IO.Directory]::CreateDirectory($dir) | Out-Null
$encoding=[Text.Encoding]::GetEncoding(949)
$source=$encoding.GetString([IO.File]::ReadAllBytes((Join-Path $root 'FormTeaching.cpp')))
$start=$source.IndexOf('UnicodeString __fastcall TteachForm::GetTeachingXYDeviations()')
$end=$source.IndexOf('void __fastcall TteachForm::ApplyTeaching()', $start)
$methods=$source.Substring($start,$end-$start)
if($methods -match 'edit_SZ|edit_TZ'){throw 'Z must not be part of XY deviation review'}
$fixture=[IO.File]::ReadAllText((Join-Path $PSScriptRoot 'TeachingReview_test.cpp.in'))
[IO.File]::WriteAllText((Join-Path $dir 'test.cpp'),$fixture.Replace('// REVIEW_METHODS',$methods),[Text.UTF8Encoding]::new($false))
$dfm=[IO.File]::ReadAllText((Join-Path $root 'FormConfig.dfm'))
$dfm=$dfm.Replace('object ConfigForm: TConfigForm','object Preview: TForm')
$dfm=[regex]::Replace($dfm,'(?m)^\s+On\w+ = \w+\r?\n','')
[IO.File]::WriteAllText((Join-Path $dir 'config.dfm'),$dfm,[Text.UTF8Encoding]::new($false))
& cmd /c Tests\TeachingReview_build.cmd
if($LASTEXITCODE -ne 0){throw 'Teaching review test compile failed'}
$p=Start-Process -FilePath (Join-Path $dir 'test.exe') -WorkingDirectory $root -WindowStyle Hidden -Wait -PassThru
Get-Content (Join-Path $dir 'test-result.txt')
if($p.ExitCode -ne 0){throw 'Teaching review tests failed'}
