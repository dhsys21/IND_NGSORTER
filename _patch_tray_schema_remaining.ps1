$ErrorActionPreference = 'Stop'
$enc = [System.Text.Encoding]::GetEncoding(949)

function Replace-Regex([string]$path, [string]$pattern, [string]$replacement) {
    $full = Join-Path $PSScriptRoot $path
    $text = [System.IO.File]::ReadAllText($full, $enc)
    $matches = [System.Text.RegularExpressions.Regex]::Matches($text, $pattern)
    if ($matches.Count -ne 1) { throw "Expected one match in $path, found $($matches.Count)" }
    $text = [System.Text.RegularExpressions.Regex]::Replace($text, $pattern, $replacement)
    [System.IO.File]::WriteAllText($full, $text, $enc)
}

Replace-Regex 'ModGripper.cpp' '(?m)^(\tMainForm->tray_target\.CELL_EXIST\[targetIndex\] = true;\r?\n)' ('$1' + "`tMainForm->tray_target.WORK_FLAG[targetIndex] = MainForm->tray_source.WORK_FLAG[sourceIndex];`r`n")
Replace-Regex 'Stage_mes.cpp' '(?m)^(\ttray_target\.CELL_EXIST\[targetIndex\] = true;\r?\n)' ('$1' + "`ttray_target.WORK_FLAG[targetIndex] = tray_source.WORK_FLAG[sourceChannel - 1];`r`n")

$logReplacement = @'
		" NGCode=" + TrackInNGCode +
		" WorkFlag=" + IntToStr(tray_target.WORK_FLAG[targetIndex] ? 1 : 0), false);
'@
Replace-Regex 'Stage_mes.cpp' '(?m)^\t\t" NGCode=" \+ TrackInNGCode, false\);$' $logReplacement
Replace-Regex 'FormBase.dfm' "Caption = 'Ver\. 2026-08-19 / SN: 20260819-027'" "Caption = 'Ver. 2026-08-19 / SN: 20260819-028'"
