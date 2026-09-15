$ErrorActionPreference='Stop'
$repo=Split-Path $PSScriptRoot -Parent
$cp=[Text.Encoding]::GetEncoding(949)
$source=[IO.File]::ReadAllText((Join-Path $repo 'FormTeaching.cpp'),$cp)
$bodies=@()
foreach($name in @('stopBtnClick','teachingTimerTimer')){
    $m=[regex]::Match($source,'(?ms)^void __fastcall TteachForm::'+$name+'\(.*?^\}')
    if(!$m.Success){throw "Missing $name"};$bodies+=$m.Value
}
$fixture=@'
#include <assert.h>
#include <stdio.h>
#include <string>
typedef std::string UnicodeString;
struct TObject {};
struct Control {bool Visible,Enabled;UnicodeString Caption;void BringToFront(){}};
struct Robot {
 int state,requests;
 Robot():state(0),requests(0){}
 bool IsManualMotionStopPending(){return state==1||state==2;}
 int ManualMotionStopState(){return state;}
 bool RequestManualMotionStop(){++requests;state=1;return true;}
} robot,*robostar=&robot;
struct Base {UnicodeString GetLangStr(const char*s){return s;}} base,*BaseForm=&base;
void ShowMessage(UnicodeString){}
struct TteachForm {
 bool Visible,manualStopOverlayHidden;
 Control controls[7];
 Control *AdvSmoothButton_Zup,*btnZAxisDown,*lblManualMotionStatus,*Label57,*Label59,*pnlMovingAlarm,*pnlMovingAlarm2;
 TteachForm():Visible(true),manualStopOverlayHidden(false),AdvSmoothButton_Zup(&controls[0]),
 btnZAxisDown(&controls[1]),lblManualMotionStatus(&controls[2]),Label57(&controls[3]),
 Label59(&controls[4]),pnlMovingAlarm(&controls[5]),pnlMovingAlarm2(&controls[6]){}
 void __fastcall stopBtnClick(TObject*);
 void __fastcall teachingTimerTimer(TObject*);
};
@@BODIES@@
int main(){
 TteachForm f;
 f.stopBtnClick(0);assert(f.pnlMovingAlarm->Visible&&robot.requests==1);
 f.stopBtnClick(0);assert(!f.pnlMovingAlarm->Visible&&!f.pnlMovingAlarm2->Visible);
 for(int i=0;i<20;++i)f.teachingTimerTimer(0);
 assert(!f.pnlMovingAlarm->Visible&&robot.state==1&&robot.requests==1);
 assert(!f.AdvSmoothButton_Zup->Enabled&&!f.btnZAxisDown->Enabled&&f.lblManualMotionStatus->Visible);
 robot.state=2;f.teachingTimerTimer(0);assert(!f.pnlMovingAlarm->Visible);
 f.stopBtnClick(0);assert(f.pnlMovingAlarm->Visible&&f.pnlMovingAlarm2->Visible&&robot.requests==1);
 f.stopBtnClick(0);robot.state=3;f.teachingTimerTimer(0);
 assert(!f.pnlMovingAlarm->Visible&&!f.manualStopOverlayHidden&&f.AdvSmoothButton_Zup->Enabled);
 f.stopBtnClick(0);assert(f.pnlMovingAlarm->Visible&&robot.requests==2);
 puts("PASS: show/hide/show, hidden across timer/timeout, no extra stop request or interlock reset, new request shows panel");
}
'@
$out=Join-Path $repo 'Win32/Debug/TeachingStopToggleTest'
[void][IO.Directory]::CreateDirectory($out)
[IO.File]::WriteAllText((Join-Path $out 'test.cpp'),$fixture.Replace('@@BODIES@@',($bodies -join "`r`n")),$cp)
Push-Location $out
try {
 & cmd.exe /d /s /c 'set "Path=" && call "C:\Program Files (x86)\Embarcadero\Studio\18.0\bin\rsvars.bat" && bcc32 -tWC -etest.exe test.cpp && test.exe'
 if($LASTEXITCODE -ne 0){throw 'Teaching toggle test failed'}
} finally {Pop-Location}
