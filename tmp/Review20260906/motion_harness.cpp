#include <string>
#include <cstdio>
#include <cassert>
typedef std::string AnsiString;
AnsiString IntToStr(int n) { char b[32]; sprintf(b,"%d",n); return b; }
enum { SSC_OK=0, Axis_x=1, Axis_y=2, Axis_z=3, Axis_zUp=4 };
int pointResult=0, startCalls=0, stopCalls=0;
bool boardMoving=false;
struct Point { unsigned long position; };
int sscSetPointDataEx(int,int,int,int,Point*) { return pointResult; }
int sscAutoStart(int,int,int,int,int) { ++startCalls; boardMoving=true; return SSC_OK; }
int sscDriveStop(int,int,int,int) { ++stopCalls; boardMoving=false; return SSC_OK; }
struct FakeMain { void memoRobostarLineAdd(AnsiString) {} } form,*MainForm=&form;
class Trobostar {
public:
    enum Sequence {seqIdle,seqAutoMove,seqPause};
    struct Step { int step,timeout,delay; } step,step_save;
    Sequence seq,seq_save;
    bool pauseStatus;
    int board_id,channel_id;
    Point point[4];
    Trobostar():seq(seqAutoMove),seq_save(seqIdle),pauseStatus(false),board_id(0),channel_id(1) {
        step.step=11;step.timeout=step.delay=0;
        for(int i=0;i<4;++i)point[i].position=0;
    }
    bool WriteLog(int sts,AnsiString) { return sts==SSC_OK; }
    bool __fastcall setPoint(int,unsigned long int);
    void __fastcall req_Pause(bool);
};
bool __fastcall Trobostar::setPoint(int axnum_id, unsigned long int pos)
{
	bool pointAccepted = false;
	bool moveAccepted = false;
	int sts = SSC_OK;

	if(axnum_id == Axis_zUp){
		sts = sscSetPointDataEx(board_id, channel_id, Axis_z, 0, &point[0]);
		pointAccepted = WriteLog(sts, "[" + IntToStr(Axis_z) +  "] POINT");
		sts = sscAutoStart(board_id, channel_id, Axis_z, 0, 0);
		moveAccepted = WriteLog(sts, "[" + IntToStr(Axis_z) +  "] MOVE");
	}else{
		point[axnum_id].position = pos;
		sts = sscSetPointDataEx(board_id, channel_id, axnum_id, 0, &point[axnum_id]);
		pointAccepted = WriteLog(sts, "[" + IntToStr(axnum_id) +  "] POINT");
		sts = sscAutoStart(board_id, channel_id, axnum_id, 0, 0);
		moveAccepted = WriteLog(sts, "[" + IntToStr(axnum_id) +  "] MOVE");
	}

	return pointAccepted && moveAccepted;
}

void __fastcall Trobostar::req_Pause(bool stop)
{
	step.timeout = 0;
	step.delay = 0;

	if(stop != pauseStatus){
		if(stop){
			seq_save = seq;
			seq = seqPause;
			step_save.step = step.step;
			pauseStatus = true;
			MainForm->memoRobostarLineAdd("[C_Maint] [로보트]일시정지 상태입니다.");
		}else{
			seq = seq_save;
			step.step = step_save.step;
			pauseStatus = false;
			MainForm->memoRobostarLineAdd("[C_Maint] [로보트]일시정지가 해제되었습니다.");
		}
	}
}

int main() {
    for(int axis=1;axis<=4;++axis) {
        Trobostar robot;
        pointResult=1;startCalls=0;boardMoving=false;
        bool accepted=robot.setPoint(axis,1000);
        assert(!accepted && startCalls==1 && boardMoving);
    }
    puts("REPRODUCED: setPoint returns failure but still calls AutoStart for X/Y/Z/Zup");
    Trobostar robot;
    boardMoving=true;stopCalls=0;
    robot.req_Pause(true);
    assert(robot.pauseStatus && robot.seq==Trobostar::seqPause);
    assert(boardMoving && stopCalls==0);
    puts("REPRODUCED: req_Pause saves the sequence but leaves already-issued board motion running");
    puts("No hardware/API DLL was loaded; SSC functions above are local test doubles.");
    return 0;
}
