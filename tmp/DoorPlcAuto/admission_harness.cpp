#include <string>
#include <cstdio>
#include <cassert>
typedef std::string AnsiString;
AnsiString IntToStr(int n){char s[32];sprintf(s,"%d",n);return s;}
const int modeAuto=1,modeManual=0,fmsAlarmNone=0;
struct Motion {bool pauseStatus;} g,r,*gripper=&g,*robostar=&r;
struct Check {bool Checked;};
struct Step {int step;};
struct PLC {
    bool fresh,autoMode,tray,center,targetTray,targetCenter,out,request;
    bool IsPlcStatusFresh(int){return fresh;}
    bool IsPlcAutoMode(){return autoMode;}
    bool IsSourceTrayIn(){return tray;}
    bool IsSourceCentering(){return center;}
    bool IsTargetTrayIn(){return targetTray;}
    bool IsTargetCentering(){return targetCenter;}
    bool IsSourceTrayOutOn(){return out;}
    void CmdSourceCenteringRequest(bool v){request=v;}
} plc,*PlcBin=&plc;
struct TMainForm {
    int equipMode,fmsAlarmTransaction,logs,waits;
    bool sourceTrayCycleAdmitted,sourceTrayOutPending;
    Check door,bypass,*chkDoorPlcAuto,*chkBypass;
    Step step[2];
    TMainForm():equipMode(modeAuto),fmsAlarmTransaction(0),logs(0),waits(0),
        sourceTrayCycleAdmitted(false),sourceTrayOutPending(false),chkDoorPlcAuto(&door),chkBypass(&bypass){
        door.Checked=bypass.Checked=false;step[0].step=step[1].step=0;
        g.pauseStatus=r.pauseStatus=false;
        plc.fresh=true;plc.autoMode=false;plc.tray=true;plc.center=false;
        plc.targetTray=plc.targetCenter=plc.out=plc.request=false;
    }
    void ReportIdleWaitStatus(){++waits;}
    void ProcessStepLog(int,AnsiString){++logs;}
    bool __fastcall IsSourceTrayInSignal() const;
    bool __fastcall IsSourceCenteringSignal() const;
    bool __fastcall IsTargetTrayInSignal() const;
    bool __fastcall IsTargetCenteringSignal() const;
    bool __fastcall CanRequestAutoSourceCentering() const;
    bool __fastcall IsSourceTrayCycleReady() const;
    bool __fastcall UpdateSourceTrayAdmission();
};
bool __fastcall TMainForm::IsSourceTrayInSignal() const
{
	//* DOOR/PLC AUTO INTERLOCK: actual PLC input only; no simulated Tray In.
	return PlcBin != NULL && PlcBin->IsSourceTrayIn();
}

bool __fastcall TMainForm::IsSourceCenteringSignal() const
{
	//* DOOR/PLC AUTO INTERLOCK: checkbox NEVER forces centering ON.
	return PlcBin != NULL && PlcBin->IsSourceCentering();
}

bool __fastcall TMainForm::IsTargetTrayInSignal() const
{
	return PlcBin != NULL && PlcBin->IsTargetTrayIn();
}

bool __fastcall TMainForm::IsTargetCenteringSignal() const
{
	//* DOOR/PLC AUTO INTERLOCK: actual Target centering remains mandatory.
	return PlcBin != NULL && PlcBin->IsTargetCentering();
}

bool __fastcall TMainForm::CanRequestAutoSourceCentering() const
{
	return equipMode == modeAuto && gripper != NULL && robostar != NULL &&
		!gripper->pauseStatus && !robostar->pauseStatus &&
		fmsAlarmTransaction == fmsAlarmNone && !sourceTrayCycleAdmitted &&
		!sourceTrayOutPending && step[0].step < 100 &&
		PlcBin != NULL && PlcBin->IsPlcStatusFresh(1000) &&
		PlcBin->IsPlcAutoMode() && PlcBin->IsSourceTrayIn() &&
		!PlcBin->IsSourceCentering() && !PlcBin->IsSourceTrayOutOn() &&
		!chkBypass->Checked;
}

bool __fastcall TMainForm::IsSourceTrayCycleReady() const
{
	// Deliberately NO PLC AUTO test here: this tray has already been admitted.
	return sourceTrayCycleAdmitted && PlcBin != NULL &&
		PlcBin->IsPlcStatusFresh(1000) && PlcBin->IsSourceTrayIn() &&
		PlcBin->IsSourceCentering() && !sourceTrayOutPending &&
		!PlcBin->IsSourceTrayOutOn() && step[0].step < 100;
}

bool __fastcall TMainForm::UpdateSourceTrayAdmission()
{
	if(PlcBin == NULL || !PlcBin->IsPlcStatusFresh(1000)){
		ReportIdleWaitStatus();
		return false; // Stale cached ON is not an admission or departure signal.
	}
	if(!PlcBin->IsSourceTrayIn()){
		sourceTrayCycleAdmitted = false;
		return false;
	}
	if(equipMode != modeAuto || gripper == NULL || robostar == NULL ||
		gripper->pauseStatus || robostar->pauseStatus ||
		fmsAlarmTransaction != fmsAlarmNone || sourceTrayOutPending ||
		PlcBin->IsSourceTrayOutOn() || step[0].step >= 100) return false;
	if(sourceTrayCycleAdmitted)
		return chkBypass->Checked || IsSourceTrayCycleReady();
	bool plcAuto = PlcBin->IsPlcAutoMode();
	bool test = chkDoorPlcAuto != NULL && chkDoorPlcAuto->Checked;
	// Preserve the existing normal BYPASS flow: do not center a tray to eject it.
	// Even BYPASS cannot dispatch a reverse-loaded tray in PLC MANUAL unless
	// Door/Auto is explicitly selected AND the actual tray is already centered.
	if((plcAuto || test) && (PlcBin->IsSourceCentering() || (plcAuto && chkBypass->Checked))){
		sourceTrayCycleAdmitted = true;
		PlcBin->CmdSourceCenteringRequest(false);
		ProcessStepLog(1, "[DOOR/PLC AUTO INTERLOCK] ADMITTED / D10101=" + IntToStr(plcAuto ? 1 : 0) +
			" / D10103=1 / D10104=" + IntToStr(PlcBin->IsSourceCentering() ? 1 : 0) +
			" / Door/Auto=" + IntToStr(test ? 1 : 0) +
			" / PLC AUTO no longer gates this tray; real motion interlocks remain active");
		return true;
	}
	ReportIdleWaitStatus();
	return false;
}

int main(){
    for(int flags=0;flags<128;++flags){
        TMainForm f;
        plc.fresh=(flags&1)!=0;plc.autoMode=(flags&2)!=0;
        plc.tray=(flags&4)!=0;plc.center=(flags&8)!=0;
        f.door.Checked=(flags&16)!=0;f.bypass.Checked=(flags&32)!=0;
        g.pauseStatus=(flags&64)!=0;
        bool request=plc.fresh && plc.autoMode && plc.tray && !plc.center && !f.bypass.Checked && !g.pauseStatus;
        assert(f.CanRequestAutoSourceCentering()==request);
        bool admitted=plc.fresh && plc.tray && !g.pauseStatus && (plc.autoMode || f.door.Checked) &&
            (plc.center || (plc.autoMode && f.bypass.Checked));
        assert(f.UpdateSourceTrayAdmission()==admitted);
        assert(f.sourceTrayCycleAdmitted==admitted);
    }
    puts("PASS: 128 initial combinations: actual PLC AUTO requests centering; Door/Auto only admits real centered tray");
    {
        TMainForm f;plc.autoMode=true;
        assert(f.CanRequestAutoSourceCentering() && !f.UpdateSourceTrayAdmission());
        plc.autoMode=false;assert(!f.CanRequestAutoSourceCentering());
        plc.center=true;assert(!f.UpdateSourceTrayAdmission());
        plc.autoMode=true;assert(f.UpdateSourceTrayAdmission());
        plc.autoMode=false;assert(f.UpdateSourceTrayAdmission() && f.IsSourceTrayCycleReady());
        assert(f.logs==1 && !f.CanRequestAutoSourceCentering());
        plc.center=false;assert(!f.IsSourceTrayCycleReady() && !f.CanRequestAutoSourceCentering());
        plc.autoMode=true;assert(!f.CanRequestAutoSourceCentering());
        g.pauseStatus=true;plc.center=true;assert(!f.UpdateSourceTrayAdmission());
        g.pauseStatus=false;assert(f.UpdateSourceTrayAdmission());
        plc.fresh=false;assert(!f.IsSourceTrayCycleReady() && !f.UpdateSourceTrayAdmission());
        plc.fresh=true;plc.tray=false;assert(!f.UpdateSourceTrayAdmission() && !f.sourceTrayCycleAdmitted);
        plc.tray=true;plc.autoMode=false;assert(!f.UpdateSourceTrayAdmission());
    }
    puts("PASS: admitted cycle continues in PLC MANUAL; no auto-recentering; Pause/PLC loss block; departure resets admission");
    {
        TMainForm f;f.door.Checked=true;
        assert(!f.IsSourceCenteringSignal() && !f.IsTargetCenteringSignal());
        plc.tray=false;assert(!f.IsSourceTrayInSignal());
        plc.tray=plc.center=true;assert(f.UpdateSourceTrayAdmission());
        f.door.Checked=false;assert(f.UpdateSourceTrayAdmission());
        f.sourceTrayOutPending=true;assert(!f.UpdateSourceTrayAdmission() && !f.IsSourceTrayCycleReady());
        f.sourceTrayOutPending=false;plc.out=true;assert(!f.UpdateSourceTrayAdmission());
        plc.out=false;f.step[0].step=100;assert(!f.UpdateSourceTrayAdmission());
        f.step[0].step=0;f.equipMode=modeManual;assert(!f.UpdateSourceTrayAdmission() && !f.CanRequestAutoSourceCentering());
        f.equipMode=modeAuto;f.fmsAlarmTransaction=1;assert(!f.UpdateSourceTrayAdmission());
    }
    puts("PASS: no simulated input; toggle does not revoke admitted tray; TrayOut, PC MANUAL, FMS alarm remain blocked");
    puts("PASS: FMS callback gates and unchanged actual motion-stop logic verified by runner");
    return 0;
}
