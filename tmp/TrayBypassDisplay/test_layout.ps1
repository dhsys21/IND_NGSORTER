$ErrorActionPreference='Stop'
$root=(Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
$enc=[Text.Encoding]::GetEncoding(949)
$dfm=[IO.File]::ReadAllText((Join-Path $root 'FormMain.dfm'),$enc)
$header=[IO.File]::ReadAllText((Join-Path $root 'FormMain.h'),$enc)
function Block([string]$name){
    $m=[regex]::Match($dfm,'(?ms)^    object '+$name+':.*?^    end')
    if(!$m.Success){throw "Missing design component $name"};$m.Value
}
function Prop([string]$block,[string]$key){
    $m=[regex]::Match($block,'(?m)^      '+[regex]::Escape($key)+' = (.+)$')
    if(!$m.Success){throw "Missing property $key"};$m.Groups[1].Value.Trim()
}
Add-Type -AssemblyName System.Drawing
Add-Type -AssemblyName System.Windows.Forms
$rightEdge=0
foreach($name in @('chkDoorPlcAuto','cbCycle','btnDryRun')){
    $b=Block $name
    $left=[int](Prop $b 'Left');$top=[int](Prop $b 'Top');$width=[int](Prop $b 'Width');$height=[int](Prop $b 'Height')
    $fontKey=if($name -eq 'btnDryRun'){'Appearance.Font.Height'}else{'Font.Height'}
    $fontSize=[Math]::Abs([int](Prop $b $fontKey));$caption=(Prop $b 'Caption').Trim("'")
    $font=New-Object Drawing.Font('Tahoma',$fontSize,[Drawing.FontStyle]::Bold,[Drawing.GraphicsUnit]::Pixel)
    try{
        $size=[Windows.Forms.TextRenderer]::MeasureText($caption,$font,[Drawing.Size]::Empty,([Windows.Forms.TextFormatFlags]::NoPadding -bor [Windows.Forms.TextFormatFlags]::SingleLine))
        $margin=if($name -eq 'btnDryRun'){16}else{24}
        if($size.Width+$margin -gt $width -or $size.Height -gt $height){throw "Clipped text: $name $size"}
        if($left -lt $rightEdge -or $top+$height -gt 27){throw "Overlapping controls: $name"}
        $rightEdge=$left+$width
        Write-Output "PASS: $name text $($size.Width)x$($size.Height), control ${width}x${height}, no overlap"
    }finally{$font.Dispose()}
}
foreach($name in @('pnlSourceTrayBypass','pSourceTrayBypass','pnlTargetTrayBypass','pTargetTrayBypass')){
    $b=Block $name
    if(!$header.Contains('*'+$name+';')){throw "Missing field $name"}
    if([int](Prop $b 'Top') -le 241 -or [int](Prop $b 'Top')+[int](Prop $b 'Height') -gt 288){throw 'Row outside panel'}
    if([int](Prop $b 'Left')+[int](Prop $b 'Width') -gt 600){throw 'Row outside panel width'}
}
if((Prop (Block 'pnlSourceBypass') 'Caption') -ne "'ROUTE ID'"){throw 'Existing Route ID overwritten'}
$main=[IO.File]::ReadAllText((Join-Path $root 'FormMain.cpp'),$enc)
$a=$main.IndexOf('void __fastcall TMainForm::SetTrayLoadBypassDisplay(')
$b=$main.IndexOf('//---------------------------------------------------------------------------',$a)
$body=$main.Substring($a,$b-$a)
$template=@'
#include <string>
#include <cassert>
#include <cstdio>
struct TPanel { std::string Caption; };
struct TMainForm {
 TPanel source,target; TPanel *pSourceTrayBypass,*pTargetTrayBypass;
 TMainForm():pSourceTrayBypass(&source),pTargetTrayBypass(&target){}
 void __fastcall SetTrayLoadBypassDisplay(bool,int);
};
/* BODY */
int main(){
 TMainForm f;
 f.SetTrayLoadBypassDisplay(true,0);f.SetTrayLoadBypassDisplay(false,0);
 f.SetTrayLoadBypassDisplay(true,2);assert(f.source.Caption=="Y" && f.target.Caption=="N");
 f.SetTrayLoadBypassDisplay(false,2);assert(f.target.Caption=="Y");
 f.SetTrayLoadBypassDisplay(true,1);assert(f.source.Caption=="N" && f.target.Caption=="Y");
 f.SetTrayLoadBypassDisplay(false,-1);assert(f.target.Caption=="N");
 puts("PASS: independent Source/Target BYPASS 2=Y, otherwise N");
}
'@
[IO.File]::WriteAllText((Join-Path $PSScriptRoot 'bypass_display_test.cpp'),$template.Replace('/* BODY */',$body),$enc)
# No response reset branch may rewrite the displayed ON-phase result.
$phase4=$main.Substring($main.IndexOf('// PHASE 4: Request is already OFF.'),$main.IndexOf('// PHASE 1-2: Request is ON. Wait for the FMS result.')-$main.IndexOf('// PHASE 4: Request is already OFF.'))
if($phase4.Contains('SetTrayLoadBypassDisplay')){throw 'Reset erases BYPASS result'}
Push-Location $PSScriptRoot
try{
 & 'C:/Program Files (x86)/Embarcadero/Studio/18.0/bin/bcc32.exe' '-tWC' '-ebypass_display_test.exe' 'bypass_display_test.cpp'
 if($LASTEXITCODE -ne 0){throw 'Compile failed'}
 & './bypass_display_test.exe'
 if($LASTEXITCODE -ne 0){throw 'Regression failed'}
}finally{Pop-Location}
