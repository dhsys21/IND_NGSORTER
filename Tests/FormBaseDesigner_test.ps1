$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
$encoding = [Text.Encoding]::GetEncoding(949)
$header = $encoding.GetString([IO.File]::ReadAllBytes((Join-Path $root 'FormBase.h')))
$source = $encoding.GetString([IO.File]::ReadAllBytes((Join-Path $root 'FormBase.cpp')))
$dfm = [IO.File]::ReadAllText((Join-Path $root 'FormBase.dfm'))
$section = [regex]::Match($header, '(?s)__published:(.*?)(?:private|public|protected):').Groups[1].Value
if (!$section) { throw 'Missing IDE-managed section' }
$methods = [regex]::Matches($section, 'void\s+__fastcall\s+(\w+)\([^;]+;')
$fields = [regex]::Matches($section, '\b(T\w+)\s*\*\s*(\w+)\s*;')
if (!$methods.Count -or !$fields.Count) { throw 'Missing fields/events' }
foreach ($field in $fields) {
    if ($field.Index -gt $methods[0].Index) { throw "Component follows event declaration: $($field.Value)" }
    $pattern = '(?m)^\s*object ' + $field.Groups[2].Value + ': ' + $field.Groups[1].Value + '\s*$'
    if ($dfm -notmatch $pattern) { throw "Component missing/mismatched in DFM: $($field.Value)" }
}
foreach ($event in [regex]::Matches($dfm, '(?m)^\s*On\w+ = (\w+)')) {
    $name = $event.Groups[1].Value
    if (@($methods | Where-Object { $_.Groups[1].Value -eq $name }).Count -ne 1) {
        throw "DFM event declaration missing/duplicated: $name"
    }
    $pattern = 'void\s+__fastcall\s+TBaseForm::' + $name + '\s*\('
    if ([regex]::Matches($source, $pattern).Count -ne 1) { throw "Event implementation missing/duplicated in FormBase.cpp: $name" }
}
Write-Output 'PASS: FormBase fields precede events; DFM fields/events match; handlers are in FormBase.cpp.'
