#include <string>
#include <cassert>
#include <cstdio>
struct TPanel { std::string Caption; };
struct TMainForm {
 TPanel source,target; TPanel *pSourceTrayBypass,*pTargetTrayBypass;
 TMainForm():pSourceTrayBypass(&source),pTargetTrayBypass(&target){}
 void __fastcall SetTrayLoadBypassDisplay(bool,int);
};
void __fastcall TMainForm::SetTrayLoadBypassDisplay(bool sourceTray, int response)
{
	// TRAY BYPASS DISPLAY: independent Source/Target result; no PLC/FMS writes.
	// Keep this separate from old pBYPASS, which now displays Target Route ID.
	TPanel *value = sourceTray ? pSourceTrayBypass : pTargetTrayBypass;
	if(value != NULL) value->Caption = response == 2 ? "Y" : "N";
}

int main(){
 TMainForm f;
 f.SetTrayLoadBypassDisplay(true,0);f.SetTrayLoadBypassDisplay(false,0);
 f.SetTrayLoadBypassDisplay(true,2);assert(f.source.Caption=="Y" && f.target.Caption=="N");
 f.SetTrayLoadBypassDisplay(false,2);assert(f.target.Caption=="Y");
 f.SetTrayLoadBypassDisplay(true,1);assert(f.source.Caption=="N" && f.target.Caption=="Y");
 f.SetTrayLoadBypassDisplay(false,-1);assert(f.target.Caption=="N");
 puts("PASS: independent Source/Target BYPASS 2=Y, otherwise N");
}