$ErrorActionPreference='Stop'
$root=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$e=[Text.Encoding]::GetEncoding(949)
$main=[IO.File]::ReadAllText((Join-Path $root 'FormMain.cpp'),$e)
$dfm=[IO.File]::ReadAllText((Join-Path $root 'FormMain.dfm'),$e)
$header=[IO.File]::ReadAllText((Join-Path $root 'FormMain.h'),$e)
$fn=[regex]::Match($main,'(?s)void __fastcall TMainForm::btnTrayStepInitClick\(TObject \*Sender\)\s*\{.*?\r?\n\}').Value
if(!$fn){throw 'Missing handler'}
$mutation=$fn.IndexOf('sourceTrayOutTimer->Enabled = false')
foreach($guard in @('equipMode != modeManual','AreAxesStopped()','getCellDetectStatus()',
 'IsSortingWorkActive()','robostar->seq_save != seqIdle','DryRunForm->IsRunning()',
 'ManualCompleteForm->IsBlocking()','manualFmsPolling','IsTargetTrayExchangeActive()',
 'opcCellTrackOutPending','opcProcessEndPending','opcTargetUnloadPending','traySavePending[0]')){
 if($fn.IndexOf($guard) -lt 0 -or $fn.IndexOf($guard) -gt $mutation){throw "Missing pre-mutation guard: $guard"}
}
foreach($token in @('sourceTrayOutPending = false','manualTrayIndex = -1',
 'ResetTrayLoadTransaction(i == 0)','InitStep(&step[i])','PROCESS_START_CANCEL()',
 'sourceTrayCycleAdmitted = sourceCenteringCompleted = false','ResetProcessFlow()')){
 if(!$fn.Contains($token)){throw "Reset missing: $token"}
}
$off=$fn.IndexOf('CmdSourceCenteringRequest(false)')
$out=$fn.IndexOf('CmdSourceTrayOut(false)')
if($off -lt $mutation -or $out -lt $off -or !$fn.Contains('CmdTargetTrayOut(false)')){throw 'PLC cancellation order incorrect'}
if($fn -match 'req_Pause\(false\)|req_Home\(|CmdSourceCenteringRequest\(true\)|Cmd.*TrayOut\(true\)|DeleteFile\(|ClearTargetTrayInfo\('){throw 'INIT moves hardware or erases cells'}
if(!$fn.Contains('!fmsTroubleLatched')){throw 'Remote alarm visibility guard lost'}
$button=[regex]::Match($dfm,'(?ms)^    object btnTrayStepInit: TAdvSmoothButton\r?\n.*?^    end').Value
foreach($p in @("Caption = 'INIT SEQ'",'Visible = False','OnClick = btnTrayStepInitClick','Left = 294','Width = 54')){
 if(!$button.Contains($p)){throw "DFM missing $p"}
}
if(!$header.Contains('void __fastcall btnTrayStepInitClick(TObject *Sender);') -or
 !$main.Contains('btnTrayStepInit->Visible = showTestOptions;') -or
 !$main.Contains('btnTrayStepInit->OnClick = btnTrayStepInitClick;')){throw 'Designer/runtime event or visibility missing'}
$plc=[IO.File]::ReadAllText((Join-Path $root 'ModPLC_Bin.cpp'),$e)
$cancel=[regex]::Match($plc,'(?s)void __fastcall TPlcBin::CmdSourceTrayOut\(bool bOn\).*?\r?\n\}').Value
if(!$cancel.Contains('sourceTrayOutInterlockActive = false;')){throw 'Tray Out latch release missing'}
'PASS: hidden designer/runtime event; guarded dual-step reset; delayed output cancellation; centering OFF before Tray Out latch release; cell records and safety Pause preserved.'
