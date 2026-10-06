$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot -Parent
$header=[IO.File]::ReadAllText((Join-Path $root 'FormManualComplete.h'))
$dfm=[IO.File]::ReadAllText((Join-Path $root 'FormManualComplete.dfm'))
$source=[Text.Encoding]::GetEncoding(949).GetString([IO.File]::ReadAllBytes((Join-Path $root 'FormManualComplete.cpp')))
$section=[regex]::Match($header,'(?s)__published:(.*?)private:').Groups[1].Value
if(!$section){throw 'IDE-managed section missing'}
if($section -match '(?m)^\s*T\w+\s+\*[^;]*,'){throw 'Declare each published component on its own line'}
$fields=[regex]::Matches($section,'(?m)^\s*(T\w+)\s+\*(\w+);')
$methods=[regex]::Matches($section,'void\s+__fastcall\s+(\w+)\([^;]+;')
foreach($field in $fields){
    if($field.Index -gt $methods[0].Index){throw 'Field declaration after event method'}
    if($dfm -notmatch ('(?m)^\s*object '+$field.Groups[2].Value+': '+$field.Groups[1].Value+'\s*$')){
        throw "Missing DFM component: $($field.Value)"
    }
}
foreach($control in [regex]::Matches($dfm,'(?m)^\s*object (\w+): (\w+)')){
    if($control.Groups[2].Value -eq 'TManualCompleteForm'){continue}
    if($section -notmatch ('\b'+$control.Groups[2].Value+'\s+\*'+$control.Groups[1].Value+';')){
        throw "Missing field: $($control.Value)"
    }
}
foreach($event in [regex]::Matches($dfm,'(?m)^\s*On\w+ = (\w+)')){
    $name=$event.Groups[1].Value
    if(@($methods | Where-Object {$_.Groups[1].Value -eq $name}).Count -ne 1){throw "Invalid event declaration $name"}
    if($source -notmatch ('void\s+__fastcall\s+TManualCompleteForm::'+$name+'\s*\(')){throw "Event body missing $name"}
}
if(($header+$dfm+$source) -match '\b(?:editCell|lblCell)\b'){throw 'Cell ID UI remains'}
if($dfm -notmatch 'object ManualCompleteForm: TManualCompleteForm'){throw 'Form class mismatch'}
Write-Output 'PASS: Individual published fields, DFM component/event matching, no Cell ID UI.'
