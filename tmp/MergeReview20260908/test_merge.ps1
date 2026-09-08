$ErrorActionPreference='Stop'
$taskRoot=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$taskEncoding=[Text.Encoding]::GetEncoding(949)
function ReadSource($file){[IO.File]::ReadAllText((Join-Path $taskRoot $file),$taskEncoding)}
function Body($text,$signature){
 $a=$text.IndexOf($signature);if($a -lt 0){throw "Missing $signature"}
 $b=$text.IndexOf("`n}",$a);if($b -lt 0){throw "No end $signature"}
 $text.Substring($a,$b-$a+2)
}
$main=ReadSource 'FormMain.cpp';$stage=ReadSource 'Stage_mes.cpp'
$manual=Body $stage 'void __fastcall TMainForm::StartManualTrayLoad('
if(!$manual.Contains('IsProductionSequenceBusy()')){throw 'Manual load may overwrite active AUTO transaction'}
$busy=Body $main 'bool __fastcall TMainForm::IsProductionSequenceBusy()'
foreach($flag in @('IsManualTrayLoadBusy()','opcTrayLoadPending[0]','fmsAlarmTransaction')){
 if(!$busy.Contains($flag)){throw "Ownership gate missing: $flag"}
}
foreach($name in @('StartManualTrayLoad','AcceptManualTrayBarcode','RetryManualTrayLoad','PollManualTrayLoad')){
 $fn=Body $stage ('void __fastcall TMainForm::'+$name+'(')
 if($fn -match 'CheckTrayLoadPresence\(|ResetTrayLoadTransaction\(|opcTrayLoadPending\[[^\]]+\]\s*=\s*true|AdvanceOpcTrayLoad\('){throw "Manual ownership crossed AUTO gate: $name"}
}
$restart=Body $main 'void __fastcall TMainForm::pause_startBtnClick('
if($restart.IndexOf('RetryManualTrayLoad()') -gt $restart.IndexOf('CheckTrayLoadPresence(')){throw 'Manual Retry routed through AUTO presence gate'}
$reset=Body $main 'void __fastcall TMainForm::ResetTrayLoadTransaction('
if(!$reset.Contains('!fmsTroubleLatched') -or $reset -match 'fmsTroubleLatched\s*='){throw 'AUTO cleanup hides/clears independent Trouble'}
$init=Body $main 'void __fastcall TMainForm::AdvSmoothToggleButton_InitWorkClick('
if(!$init.Contains('IsManualTrayLoadBusy()') -or !$init.Contains('!fmsTroubleLatched')){throw 'Init Work conflicts with manual service/independent Trouble'}
Write-Output 'PASS: AUTO presence reset, explicit MANUAL ownership and independent FMS Trouble remain separated'
[xml]$project=Get-Content (Join-Path $taskRoot 'NGSORTER.cbproj')
$startup=ReadSource 'NGSORTER.cpp'
foreach($item in $project.Project.ItemGroup.CppCompile){if($item.Form){
 $dfm=[IO.Path]::ChangeExtension([string]$item.Include,'.dfm')
 $first=Get-Content -LiteralPath (Join-Path $taskRoot $dfm) -TotalCount 1
 if($first -notmatch ('^(object|inherited) '+[regex]::Escape([string]$item.Form)+':')){throw "DFM/project mismatch $dfm"}
 $pattern='USEFORM\("'+[regex]::Escape([string]$item.Include)+'",\s*'+[regex]::Escape([string]$item.Form)+'\)'
 if([regex]::Matches($startup,$pattern).Count -ne 1){throw "USEFORM mismatch $dfm"}
}}
Write-Output 'PASS: project/DFM/USEFORM registrations match, including ManualComplete and gripper'
