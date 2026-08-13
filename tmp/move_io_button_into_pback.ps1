$encoding = [System.Text.Encoding]::GetEncoding(949)
$path = Join-Path $PSScriptRoot '..\FormMain.dfm'
$text = [System.IO.File]::ReadAllText($path, $encoding)

$buttonStart = $text.IndexOf('  object btnIOMonitoring: TAdvSmoothButton', [System.StringComparison]::Ordinal)
if ($buttonStart -lt 0) { throw 'Root btnIOMonitoring not found.' }
$buttonEnd = $text.IndexOf('  end', $buttonStart, [System.StringComparison]::Ordinal)
if ($buttonEnd -lt 0) { throw 'btnIOMonitoring end not found.' }
$buttonEnd += '  end'.Length
$buttonBlock = $text.Substring($buttonStart, $buttonEnd - $buttonStart)
$text = $text.Remove($buttonStart, $buttonEnd - $buttonStart)

# Convert the root component indentation to a child of pback.
$lines = $buttonBlock -split "`r?`n"
for ($i = 0; $i -lt $lines.Count; ++$i) {
    if ($lines[$i].Length -gt 0) { $lines[$i] = '  ' + $lines[$i] }
}
$childBlock = ($lines -join "`r`n") + "`r`n"

$pbackStart = $text.IndexOf('  object pback: TAdvSmoothPanel', [System.StringComparison]::Ordinal)
if ($pbackStart -lt 0) { throw 'pback not found.' }
$insertAnchor = $text.IndexOf('    object lblTitle: TLabel', $pbackStart, [System.StringComparison]::Ordinal)
if ($insertAnchor -lt 0) { throw 'pback child anchor not found.' }
$text = $text.Insert($insertAnchor, $childBlock)

[System.IO.File]::WriteAllText($path, $text, $encoding)
