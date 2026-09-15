$ErrorActionPreference = 'Stop'
$repo = Split-Path $PSScriptRoot -Parent
$nodes = @{}
$stack = [Collections.Generic.List[object]]::new()
foreach ($line in [IO.File]::ReadAllLines((Join-Path $repo 'FormTeaching.dfm'))) {
    if ($line -match '^(\s*)object (\w+): (\w+)') {
        $indent = $Matches[1].Length
        $node = @{ Name=$Matches[2]; Type=$Matches[3]; Indent=$indent; Parent=''; Props=@{} }
        if ($stack.Count) { $node.Parent = $stack[$stack.Count-1].Name }
        $nodes[$node.Name] = $node
        $stack.Add($node)
    } elseif ($line -match '^(\s*)end\s*$') {
        if ($stack.Count -and $Matches[1].Length -eq $stack[$stack.Count-1].Indent) {
            $stack.RemoveAt($stack.Count-1)
        }
    } elseif ($stack.Count -and $line -match '^(\s*)(\w+) = (.*)$') {
        $node = $stack[$stack.Count-1]
        if ($Matches[1].Length -eq $node.Indent+2) { $node.Props[$Matches[2]] = $Matches[3] }
    }
}
function Inside($name) {
    $node=$nodes[$name]; $p=$node.Props; $parent=$nodes[$node.Parent].Props
    # Moving overlays are off-canvas in the designer and alClient at runtime.
    if ($node.Parent -in @('pnlMovingAlarm','pnlMovingAlarm2')) {
        $parent=$nodes[$nodes[$node.Parent].Parent].Props
    }
    if ([int]$p.Left -lt 0 -or [int]$p.Top -lt 0 -or
        [int]$p.Left+[int]$p.Width -gt [int]$parent.Width -or
        [int]$p.Top+[int]$p.Height -gt [int]$parent.Height) { throw "$name outside parent bounds" }
}
$jog=$nodes.pnlJogControl.Props
if ([int]$jog.Height -ne 140) { throw 'Unexpected JOG height' }
foreach ($name in @('AdvSmoothButton_Zup','btnZAxisDown')) {
    if ($nodes[$name].Parent -ne 'pnlManualControl') { throw 'Z buttons must be outside the JOG overlay' }
    Inside $name
    if ([int]$nodes[$name].Props.Top -le [int]$jog.Top+[int]$jog.Height) { throw 'Z button overlaps JOG' }
}
if ([int]$nodes.AdvSmoothButton_Zup.Props.Left+[int]$nodes.AdvSmoothButton_Zup.Props.Width -gt
    [int]$nodes.btnZAxisDown.Props.Left) { throw 'Z buttons overlap' }
if ($nodes.btnStopMoving.Parent -ne 'pnlMovingAlarm' -or
    $nodes.btnStopMoving.Props.OnClick -ne 'stopBtnClick') { throw 'STOP MOVING location/event mismatch' }
foreach ($name in @('btnStopMoving','Label57','Label59')) { Inside $name }
foreach ($node in $nodes.Values) {
    if ($node.Parent -eq 'pnlJogControl' -and $node.Type -eq 'TAdvSmoothButton') { Inside $node.Name }
    if ($node.Parent -eq 'pnlMovingAlarm2' -and $node.Type -match 'Button') { throw 'Duplicate STOP in gripper overlay' }
}
if ($nodes.teachingTimer.Props.OnTimer -ne 'teachingTimerTimer') { throw 'Missing UI status timer' }
$cpp=[IO.File]::ReadAllText((Join-Path $repo 'FormTeaching.cpp'),[Text.Encoding]::GetEncoding(949))
foreach ($name in @('pnlMovingAlarm','pnlMovingAlarm2')) {
    $p=$nodes[$name].Props
    if ($p.Align -ne 'alNone' -or [int]$p.Left+[int]$p.Width -gt 0 -or $p.Visible -ne 'False') {
        throw "$name must not cover controls in the designer"
    }
    if ($cpp -notmatch ($name+'->Align\s*=\s*alClient;')) { throw "$name missing runtime alignment" }
}
Write-Output 'PASS: moving overlays off-canvas at design time, full-panel alignment restored at runtime'
Write-Output 'PASS: one JOG-overlay STOP, JOG buttons fit, Z buttons below/outside overlay, labels fit, timer wired'
