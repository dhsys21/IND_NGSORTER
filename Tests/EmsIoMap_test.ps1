$ErrorActionPreference = 'Stop'
$repo = Split-Path $PSScriptRoot -Parent
$cp = [Text.Encoding]::GetEncoding(949)
$header = [IO.File]::ReadAllText((Join-Path $repo 'ModRobostar.h'), $cp)
$main = [IO.File]::ReadAllText((Join-Path $repo 'FormMain.cpp'), [Text.Encoding]::UTF8)
function GetStruct($name) {
    $m = [regex]::Match($header, '(?s)typedef struct\{[^{}]*\}' + $name + ';')
    if (!$m.Success) { throw "Missing I/O struct: $name" }
    return $m.Value
}
function GetArray($name) {
    $m = [regex]::Match($main, '(?s)\b' + $name + '\[\d+\]\s*=\s*\{(.*?)\};')
    if (!$m.Success) { throw "Missing monitor array: $name" }
    return $m.Groups[1].Value
}
$inputNames = @([regex]::Matches((GetArray 'inputNames'), '"([^"]*)"') | ForEach-Object { $_.Groups[1].Value })
$outputNames = @([regex]::Matches((GetArray 'outputNames'), '"([^"]*)"') | ForEach-Object { $_.Groups[1].Value })
$inputValues = @([regex]::Matches((GetArray 'inputValue'), 'robostar->input\.(\w+)') | ForEach-Object { $_.Groups[1].Value })
$outputValues = @([regex]::Matches((GetArray 'outputValue'), 'robostar->gripper\.(\w+)') | ForEach-Object { $_.Groups[1].Value })
if ($inputNames.Count -ne 48 -or $inputValues.Count -ne 48 -or
    $outputNames.Count -ne 16 -or $outputValues.Count -ne 16) { throw 'Monitor array size changed.' }
if ($inputNames[0x24] -cne 'EMS Switch 01' -or $inputValues[0x24] -cne 'EMS_SWITCH_01' -or
    $inputNames[0x2D] -cne 'EMS Switch 02' -or $inputValues[0x2D] -cne 'EMS_SWITCH_02' -or
    $outputNames[0xE] -cne 'EMS LAMP' -or $outputValues[0xE] -cne 'EMS_LAMP') {
    throw 'EMS monitor address/name/value mapping mismatch.'
}
$fixture = @'
#include <stdio.h>
#include <string.h>
typedef unsigned char uint8_t;
@@INPUT@@
@@OUTPUT@@
bool onlyBit(const void *data, unsigned size, unsigned bit) {
    const unsigned char *bytes = (const unsigned char *)data;
    for (unsigned i = 0; i < size; ++i)
        if (bytes[i] != (i == bit / 8 ? (1u << (bit % 8)) : 0)) return false;
    return true;
}
int main() {
    INPUT_ROBOT input;
    OUTPUT_IO output;
    if (sizeof(input) != 8 || sizeof(output) != 4) return 1;
    memset(&input, 0, sizeof(input)); input.EMS_SWITCH_01 = 1;
    if (!onlyBit(&input, sizeof(input), 0x24)) return 2;
    memset(&input, 0, sizeof(input)); input.EMS_SWITCH_02 = 1;
    if (!onlyBit(&input, sizeof(input), 0x2D)) return 3;
    memset(&output, 0, sizeof(output)); output.EMS_LAMP = 1;
    if (!onlyBit(&output, sizeof(output), 0x3E - 0x20)) return 4;
    puts("PASS: bcc32 I/O sizes and X0024/X002D/Y003E bit offsets unchanged");
    return 0;
}
'@
$fixture = $fixture.Replace('@@INPUT@@', (GetStruct 'INPUT_ROBOT')).Replace('@@OUTPUT@@', (GetStruct 'OUTPUT_IO'))
$out = Join-Path $repo 'Win32/Debug/EmsIoMapTest'
[void][IO.Directory]::CreateDirectory($out)
[IO.File]::WriteAllText((Join-Path $out 'test.cpp'), $fixture, $cp)
Push-Location $out
try {
    & cmd.exe /d /s /c 'set "Path=" && call "C:\Program Files (x86)\Embarcadero\Studio\18.0\bin\rsvars.bat" && bcc32 -tWC -etest.exe test.cpp && test.exe'
    if ($LASTEXITCODE -ne 0) { throw 'EMS I/O layout test failed.' }
} finally { Pop-Location }
Write-Output 'PASS: monitor EMS labels and raw signal mapping'
