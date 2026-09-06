$ErrorActionPreference='Stop'
$root=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$cp=[Text.Encoding]::GetEncoding(949)
$utf=New-Object Text.UTF8Encoding($false,$true)
function Read-Source([string]$name) {
    $bytes=[IO.File]::ReadAllBytes((Join-Path $root $name))
    try { return $utf.GetString($bytes) } catch { return $cp.GetString($bytes) }
}
function Body([string]$file,[string]$name) {
    $text=Read-Source $file
    $pattern='(?m)^(?:bool|int|void) __fastcall (?:TMainForm|Tgripper|Trobostar)::'+$name+'\('
    $match=[regex]::Match($text,$pattern)
    if(!$match.Success){throw "Missing $file : $name"}
    $end=$text.IndexOf('//---------------------------------------------------------------------------',$match.Index)
    return $text.Substring($match.Index,$end-$match.Index)
}
$functions=@()
foreach($name in @('IsSortingWorkActive','IsTargetTrayExchangeBoundary')){$functions+=Body 'ModGripper.cpp' $name}
$functions+=Body 'ModRobostar.cpp' 'IsTargetTrayUnloadSafe'
foreach($name in @('IsTargetTrayExchangeActive','GetTargetTrayOccupiedCount','RequestTargetTrayExchange',
 'CheckTargetTrayUnloadAtSourceEnd','CanUnloadTargetTray','StartTargetTrayExchangeUnload',
 'btnTargetTrayExchangeOutClick','IsReplacementTargetLoadAllowed','CompleteTargetTrayExchange',
 'ServiceTargetTrayExchange','UpdateTargetTrayExchangePanel','CmdTrayOut')){$functions+=Body 'FormMain.cpp' $name}
foreach($name in @('NotifyTargetTrayUnload','AdvanceOpcTrayLoad','NotifyTrayInfo','TryStartOpcProcess')){$functions+=Body 'Stage_mes.cpp' $name}
$template=[IO.File]::ReadAllText((Join-Path $PSScriptRoot 'exchange_harness.cpp.in'),$utf)
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'exchange_harness.cpp'),$template.Replace('/* PRODUCTION_FUNCTIONS */',($functions -join "`r`n")),$cp)

# Wiring checks complement executed production bodies. No machine I/O is opened.
$gripper=Read-Source 'ModGripper.cpp'
if(([regex]::Matches($gripper,'CheckTargetTrayUnloadAtSourceEnd\(')).Count -ne 1){throw 'Threshold must have one Source-end caller'}
if($gripper -notmatch 'InitSequence\(seqIdle\);[\s\S]{0,350}CheckTargetTrayUnloadAtSourceEnd\(\)'){throw 'Threshold caller is not Source completion'}
if((Body 'ModGripper.cpp' 'StartNextCycleOrWait') -match 'targetTrayUnloadCount|CheckTargetTrayUnloadAtSourceEnd'){throw 'Threshold interrupts an active Source tray'}
if((Body 'ModGripper.cpp' 'Initialize') -notmatch 'RequestTargetTrayExchange\(false\)'){throw 'Full tray exchange not wired'}
if((Body 'FormMain.cpp' 'stepTimerTimer') -notmatch 'if\(IsTargetTrayExchangeActive\(\)\) return;'){throw 'Missing next Source admission gate'}
if((Body 'FormMain.cpp' 'senTimerTimer') -notmatch 'ServiceTargetTrayExchange\(\)'){throw 'Exchange timer not wired'}
if((Body 'FormMain.cpp' 'ReadSourceTrayBarcode') -notmatch 'if\(IsTargetTrayExchangeActive\(\)\) return;'){throw 'Source rescan can clear exchange reports'}
$config=Read-Source 'FormConfig.cpp'
if($config -notmatch 'ReadInteger\("TARGET_TRAY", "UNLOAD_CELL_COUNT", 0\)' -or
   $config -notmatch 'WriteInteger\("TARGET_TRAY", "UNLOAD_CELL_COUNT", BaseForm->config.targetTrayUnloadCount\)' -or
   $config -notmatch 'unloadCount < 0 \|\| unloadCount > 96'){throw 'Unload-count persistence/validation missing'}
foreach($file in @('Lang_En.ini','Lang_Ko.ini','Lang_Hi.ini')) {
    $lang=Read-Source $file
    foreach($key in @('CAP_TARGET_UNLOAD_SETTING','CAP_TARGET_UNLOAD_COUNT','CAP_TARGET_UNLOAD_DISABLED',
        'MSG_TARGET_UNLOAD_RANGE','CAP_TARGET_EXCHANGE_OUT','CAP_TARGET_EXCHANGE_COUNT',
        'MSG_TARGET_EXCHANGE_SAFE','MSG_TARGET_EXCHANGE_LIMIT','MSG_TARGET_EXCHANGE_FMS',
        'MSG_TARGET_EXCHANGE_OUT','MSG_TARGET_EXCHANGE_IN','MSG_TARGET_EXCHANGE_LOAD','MSG_TARGET_EXCHANGE_RESET')){
        if(([regex]::Matches($lang,"(?m)^$key=")).Count -ne 1){throw "Missing/duplicate translation: $file $key"}
    }
}
foreach($file in @('FormConfig','FormMain')){
    $dfm=Read-Source "$file.dfm"; $header=Read-Source "$file.h"
    foreach($m in [regex]::Matches($dfm,'(?m)^\s*object (\w*(?:TargetUnload|TargetTrayExchange)\w*): (\w+)')){
        if($header -notmatch ('\b'+$m.Groups[2].Value+'\s*\*'+$m.Groups[1].Value+';')){throw "Missing DFM binding: $($m.Value)"}
    }
}
Push-Location $PSScriptRoot
try {
    & 'C:/Program Files (x86)/Embarcadero/Studio/18.0/bin/bcc32.exe' '-tWC' '-eexchange_test.exe' 'exchange_harness.cpp'
    if($LASTEXITCODE -ne 0){throw 'Harness compile failed'}
    & './exchange_test.exe'
    if($LASTEXITCODE -ne 0){throw 'Exchange regression failed'}
    Write-Host 'PASS: Source-end wiring, language keys and DFM bindings'
} finally {Pop-Location}
