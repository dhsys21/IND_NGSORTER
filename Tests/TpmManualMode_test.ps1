$ErrorActionPreference = 'Stop'
$repo = Split-Path $PSScriptRoot -Parent
$cp949 = [Text.Encoding]::GetEncoding(949)
$utf8 = [Text.UTF8Encoding]::new($false, $true)
$main = [IO.File]::ReadAllText((Join-Path $repo 'FormMain.cpp'), $utf8)
$config = [IO.File]::ReadAllText((Join-Path $repo 'FormConfig.cpp'), $cp949)
$catalog = [IO.File]::ReadAllText((Join-Path $repo 'TpmLoss.h'), $utf8)
$dialog = [IO.File]::ReadAllText((Join-Path $repo 'FormTpmLoss.cpp'), $utf8)
function Require($condition, $message) { if (!$condition) { throw $message } }
$select = [regex]::Match($main, '(?ms)^bool __fastcall TMainForm::SelectTpmManualReason\(.*?^\}').Value
$manual = [regex]::Match($main, '(?ms)^void __fastcall TMainForm::manualBtnClick\(.*?^\}').Value
$enter = [regex]::Match($main, '(?ms)^void __fastcall TMainForm::EnterManualMode\(.*?^\}').Value
Require ($select -and $manual -and $enter) 'Manual TPM functions are missing.'
Require ($manual.Contains('!BaseForm->config.tpmUnused')) 'TPM disable setting is not consulted.'
Require ($manual.IndexOf('SelectTpmManualReason()') -lt $manual.IndexOf('EnterManualMode();')) 'Selection must precede Manual mode.'
Require ($manual.Contains('if(!BaseForm->config.tpmUnused)')) 'Manual re-selection must show the TPM dialog as in IROCV.'
Require ($select.IndexOf('gripper->req_Pause(true)') -lt $select.IndexOf('dialog->SelectReason')) 'Pause must precede modal loop.'
Require ($select.IndexOf('robostar->req_Pause(true)') -lt $select.IndexOf('dialog->SelectReason')) 'Axis stop must precede modal loop.'
Require ($select.Contains('new TTpmLossForm(this)') -and $select.Contains('delete dialog;')) 'Dialog must be created and released on demand.'
Require ($select.IndexOf('pending.robotStep =') -lt $select.IndexOf('req_Pause(true)')) 'Capture interrupted work before Pause changes it.'
Require ($select.IndexOf('reason < 0') -lt $select.IndexOf('lastTpmLoss = pending')) 'Cancel must not overwrite the previous confirmed reason.'
Require ($select -notmatch 'req_Pause\(false\)|SetPcTag|FlushPendingPcTags|Publish') 'TPM must not restart motion or send unapproved FMS tags.'
Require ($enter.Contains('SuspendAutomaticFmsSequence();') -and $enter.Contains('CmdSourceCenteringRequest(false)')) 'Preserve normal Manual interlocks.'
Require ($manual.Contains('MSG_MANUAL_ALARM')) 'Disabled TPM must retain the existing confirmation flow.'
Require ($dialog.Contains('FSelectedReason = -1;') -and $dialog.Contains('ShowModal() == mrOk ? FSelectedReason : -1')) 'Stale modal selection risk.'
Require ($config.Contains('WriteBool("TPM", "UNUSED", BaseForm->config.tpmUnused)')) 'TPM setting is not saved.'
Require ($config.Contains('ReadBool("TPM", "UNUSED", false)')) 'TPM setting is not loaded with enabled default.'
$codes = @('0300','1200','1510','1520','1720','1730','1400','4110','3500','5100')
foreach ($code in $codes) { Require ($catalog.Contains('{"'+$code+'",')) "Missing code $code" }
$dfm = [IO.File]::ReadAllText((Join-Path $repo 'FormTpmLoss.dfm'))
foreach ($code in $codes) {
 Require ($dfm.Contains('object btnSelect'+$code+': TButton')) "Missing designer button $code"
 Require ($dfm.Contains('OnClick = btnSelect'+$code+'Click')) "Missing published handler $code"
}
$display = [regex]::Match($main, '(?ms)^void __fastcall TMainForm::UpdateTpmLossDisplay\(.*?^\}').Value
Require ($display.Contains('tpmReasonActive && equipMode == modeManual && lastTpmLoss.valid')) 'Only an active Manual reason may be displayed.'
Require ($display.Contains('"TPM_DESC_" + lastTpmLoss.code')) 'Reason display must follow the current language.'
$enable = [regex]::Match($main, '(?ms)^void __fastcall TMainForm::EnableButton_auto\(.*?^\}').Value
Require ($enable.Contains('tpmReasonActive = false;') -and $enable.Contains('UpdateTpmLossDisplay();')) 'Mode change must clear the active display.'
Require ($manual.IndexOf('EnterManualMode();') -lt $manual.IndexOf('tpmReasonActive = true;')) 'Reason display must activate after Manual mode is confirmed.'
Require ($select.Contains('pending.unit = "NGSORTER"') -and $select.Contains('pending.description =')) 'Future report snapshot lacks unit/reason.'
$keys = @('CAP_TPM_UNUSED','TPM_DESCRIPTION','TPM_CODE','TPM_CHOICE','TPM_SELECT','TPM_CANCEL','TPM_PAUSED','TPM_BM','TPM_PM','TPM_CM','TPM_MINOR_STOP','TPM_MATERIAL') + @($codes | ForEach-Object { 'TPM_DESC_' + $_ })
foreach ($name in @('Lang_En.ini','Lang_Ko.ini','Lang_Hi.ini')) {
 $language = [IO.File]::ReadAllText((Join-Path $repo $name), $utf8)
 foreach ($key in $keys) { Require ([regex]::Matches($language, '(?m)^'+$key+'=.+$').Count -eq 1) "$name missing/duplicate: $key" }
}
$project = [xml][IO.File]::ReadAllText((Join-Path $repo 'NGSORTER.cbproj'), $utf8)
Require ($project.Project.ItemGroup.CppCompile.Include -contains 'FormTpmLoss.cpp') 'TPM source missing from build.'
Require ($project.Project.ItemGroup.FormResources.Include -contains 'FormTpmLoss.dfm') 'TPM DFM missing from build.'
Write-Output 'PASS: TPM codes, Manual selection/cancel, Pause order, local-only report, settings, languages, project linkage.'
