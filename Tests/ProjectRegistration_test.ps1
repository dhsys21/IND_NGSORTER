$ErrorActionPreference = 'Stop'
$root = Split-Path $PSScriptRoot -Parent
[xml]$project = [IO.File]::ReadAllText((Join-Path $root 'NGSORTER.cbproj'))
$ns = New-Object Xml.XmlNamespaceManager($project.NameTable)
$ns.AddNamespace('p', $project.DocumentElement.NamespaceURI)
$compile = @($project.SelectNodes('//p:CppCompile', $ns) | ForEach-Object { $_.GetAttribute('Include') })
$resources = @($project.SelectNodes('//p:FormResources', $ns) | ForEach-Object { $_.GetAttribute('Include') })
$startup = [IO.File]::ReadAllText((Join-Path $root 'NGSORTER.cpp'))
$units = [regex]::Matches($startup, 'USE(FORM|UNIT)\("([^"]+)"')
foreach ($unit in $units) {
    $name = $unit.Groups[2].Value
    if (@($compile | Where-Object { $_ -ieq $name }).Count -ne 1) {
        throw "Project must compile exactly once: $name. Reload the .cbproj in the IDE."
    }
    if (-not (Test-Path -LiteralPath (Join-Path $root $name))) { throw "Source missing: $name" }
    if ($unit.Groups[1].Value -eq 'FORM') {
        $dfm = [IO.Path]::ChangeExtension($name, '.dfm')
        if (@($resources | Where-Object { $_ -ieq $dfm }).Count -ne 1) {
            throw "Form resource registration missing or duplicated: $dfm"
        }
    }
}
$duplicates = $units | ForEach-Object { $_.Groups[2].Value } | Group-Object | Where-Object { $_.Count -gt 1 }
if ($duplicates) { throw 'Duplicate startup unit registration.' }
Write-Output 'PASS: startup units, build sources, and form resources match.'
