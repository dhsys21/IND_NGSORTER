//---------------------------------------------------------------------------

#ifndef ModRobostarH
#define ModRobostarH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
#include "mc2xxstd.h"


typedef struct{
	long 	path;
	short	ret;
	long	netno;
	long	stno;
	long 	devno;
}CONFIG_ROBOSTAR;

// CC-Link input map. mdReceive() reads this block from DevX X0000.
typedef struct{
	uint8_t CP01_TRIP:1; //X0000 CP01 TRIP, PL LAMP Trip
	uint8_t CP02_TRIP:1; //X0001 CP02 TRIP, SPD Trip
	uint8_t CP03_TRIP:1; //X0002 CP03 TRIP, SMPS1 Trip
	uint8_t CP04_TRIP:1; //X0003 CP04 TRIP, SMPS2 Trip
	uint8_t CP05_TRIP:1; //X0004 CP05 TRIP, SMPS3 Trip
	uint8_t CP06_TRIP:1; //X0005 CP06 TRIP, SMPS1 DC Trip
	uint8_t CP07_TRIP:1; //X0006 CP07 TRIP, SMPS2 DC Trip
	uint8_t CP08_TRIP:1; //X0007 CP08 TRIP, SMPS3 DC Trip
	uint8_t CP09_TRIP:1; //X0008 CP09 TRIP, FAN Trip
	uint8_t CP10_TRIP:1; //X0009 CP10 TRIP, SERVO1 Trip
	uint8_t CP11_TRIP:1; //X000A CP11 TRIP, SERVO2 Trip
	uint8_t CP12_TRIP:1; //X000B CP12 TRIP, SERVO3 Trip
	uint8_t CP13_TRIP:1; //X000C CP13 TRIP, BCR01 Trip
	uint8_t CP14_TRIP:1; //X000D CP14 TRIP, BCR02 Trip
	uint8_t MS01_TRIP:1; //X000E MS01 TRIP
	uint8_t X000F:1; //X000F
//--------------------------
	uint8_t SERVO01_INPOS:1; //X0010 SERVO01 INPOS
	uint8_t SERVO01_ALARM:1; //X0011 SERVO01 ALARM
	uint8_t SERVO01_OK_HOME:1; //X0012 SERVO01 OK HOME
	uint8_t SERVO02_INPOS:1; //X0013 SERVO02 INPOS
	uint8_t SERVO02_ALARM:1; //X0014 SERVO02 ALARM
	uint8_t SERVO02_OK_HOME:1; //X0015 SERVO02 OK HOME
	uint8_t SERVO03_INPOS:1; //X0016 SERVO03 INPOS
	uint8_t SERVO03_ALARM:1; //X0017 SERVO03 ALARM
	uint8_t SERVO03_OK_HOME:1; //X0018 SERVO03 OK HOME
	uint8_t X0019:1; //X0019
	uint8_t X001A:1; //X001A
	uint8_t X001B:1; //X001B
	uint8_t X001C:1; //X001C
	uint8_t X001D:1; //X001D
	uint8_t X001E:1; //X001E
	uint8_t X001F:1; //X001F
//--------------------------
	uint8_t GRIPPER1_CHUCK:1; //X0020 GRIPPER1 CHUCK
	uint8_t GRIPPER1_UNCHUCK:1; //X0021 GRIPPER1 UNCHUCK
	uint8_t GRIPPER1_CELL_DETECT:1; //X0022 active-low: ON=no cell, OFF=cell detected
	uint8_t GRIPPER1_BUFFER:1; //X0023 GRIPPER1 BUFFER
	uint8_t EMS_SWITCH:1; //X0024 ON: normal, OFF: emergency stop
	uint8_t OPBOX_RESET_SWITCH:1; //X0025 OPBOX RESET SWITCH
	uint8_t SAFETY_DOOR_1:1; //X0026 ON: unlocked/open, OFF: keylock locked
	uint8_t SAFETY_DOOR_2:1; //X0027 ON: unlocked/open, OFF: keylock locked
	uint8_t SAFETY_RESET_SW_ON:1; //X0028 SAFETY RESET SW ON
	uint8_t BYPASS_SW_OFF:1; //X0029 contact ON: actual hardware BY-PASS OFF confirmation
	uint8_t BYPASS_SW_ON:1; //X002A contact ON: actual hardware BY-PASS ON confirmation
	uint8_t SAFETY_EMG_READY:1; //X002B ON: emergency safety circuit ready
	uint8_t SAFETY_DOOR_READY:1; //X002C ON: door safety circuit ready
	uint8_t SAFETY_DOOR_3:1; //X002D not used
	uint8_t X002E:1; //X002E
	uint8_t X002F:1; //X002F
//--------------------------
	uint8_t X0030:1; //X0030
	uint8_t X0031:1; //X0031
	uint8_t X0032:1; //X0032
	uint8_t X0033:1; //X0033
	uint8_t X0034:1; //X0034
	uint8_t X0035:1; //X0035
	uint8_t X0036:1; //X0036
	uint8_t X0037:1; //X0037
	uint8_t X0038:1; //X0038
	uint8_t X0039:1; //X0039
	uint8_t X003A:1; //X003A
	uint8_t X003B:1; //X003B
	uint8_t X003C:1; //X003C
	uint8_t X003D:1; //X003D
	uint8_t X003E:1; //X003E
	uint8_t X003F:1; //X003F
//--------------------------
}INPUT_ROBOT;
//---------------------------------------------------------------------------

// Internal robot status bits used by the existing motion sequence.
typedef struct{
	uint8_t SERVO_ON[3];
	uint8_t SERVO_HOME[3];
	uint8_t RX02_RESERVED:1;

	uint8_t SERVO03_OK_HOME:1; //RX09
	uint8_t SERVO04_INPOS:1; //RX0A
	uint8_t SERVO04_ALARM:1; //RX0B
	uint8_t SERVO04_OK_HOME:1; //RX0C
	uint8_t SERVO_RUNNING:1; //RX0D
	uint8_t RX0E:1; //RX0E
	uint8_t RX0F:1; //RX0F
//--------------------------
	uint8_t RX10:1; //RX10
	uint8_t RX11:1; //RX11
	uint8_t RX12:1; //RX12
	uint8_t RX13:1; //RX13
	uint8_t RX14:1; //RX14
	uint8_t RX15:1; //RX15
	uint8_t RX16:1; //RX16
	uint8_t RX17:1; //RX17
	uint8_t RX18:1; //RX18
	uint8_t RX19:1; //RX19
	uint8_t RX1A:1; //RX1A
	uint8_t RX1B:1; //RX1B
	uint8_t RX1C:1; //RX1C
	uint8_t RX1D:1; //RX1D
	uint8_t RX1E:1; //RX1E
	uint8_t RX1F:1; //RX1F
//--------------------------
	uint8_t RX20_RESERVED:1; //RX20
	uint8_t RX21_RESERVED:1; //RX21
	uint8_t GRIPPER1_CHUCK:1; //RX22
	uint8_t GRIPPER1_FLOAT:1; //RX23
	uint8_t GRIPPER1_DETECT:1; //RX24
	uint8_t RX25:1; //RX25
	uint8_t RX26:1; //RX26
	uint8_t RX27:1; //RX27
	uint8_t RX28:1; //RX28
	uint8_t RX29:1; //RX29
	uint8_t RX2A:1; //RX2A
	uint8_t RX2B:1; //RX2B
	uint8_t RX2C:1; //RX2C
	uint8_t RX2D:1; //RX2D
	uint8_t RX2E:1; //RX2E
	uint8_t RX2F:1; //RX2F
//--------------------------
	uint8_t RX30:1; //RX30
	uint8_t RX31:1; //RX31
	uint8_t RX32:1; //RX32
	uint8_t RX33:1; //RX33
	uint8_t RX34:1; //RX34
	uint8_t RX35:1; //RX35
	uint8_t RX36:1; //RX36
	uint8_t RX37:1; //RX37
	uint8_t RX38:1; //RX38
	uint8_t RX39:1; //RX39
	uint8_t RX3A:1; //RX3A
	uint8_t RX3B:1; //RX3B
	uint8_t RX3C:1; //RX3C
	uint8_t RX3D:1; //RX3D
	uint8_t RX3E:1; //RX3E
	uint8_t RX3F:1; //RX3F
//--------------------------
	uint8_t EMERGENCY_ON:1; //RX40
	uint8_t RX41:1; //RX41
	uint8_t RX42:1; //RX42
	uint8_t RX43:1; //RX43
	uint8_t SAFETY_DOOR:1; //RX44
	uint8_t RX45:1; //RX45
	uint8_t RX46:1; //RX46
	uint8_t SAFETY_CONT_ERR:1; //RX47
	uint8_t RX48:1; //RX48
	uint8_t RX49:1; //RX49
	uint8_t RX4A:1; //RX4A
	uint8_t RX4B:1; //RX4B
	uint8_t RX4C:1; //RX4C
	uint8_t RX4D:1; //RX4D
	uint8_t RX4E:1; //RX4E
	uint8_t RX4F:1; //RX4F
//--------------------------
}OUTPUT_ROBOT;
//---------------------------------------------------------------------------

// CC-Link output map. mdSend() writes this block from DevY Y0020.
typedef struct{
//--------------------------
	uint8_t Y0020:1; //Y0020
	uint8_t Y0021:1; //Y0021
	uint8_t Y0022:1; //Y0022 reserved (gripper vertical cylinder not used)
	uint8_t Y0023:1; //Y0023 reserved (gripper vertical cylinder not used)
	uint8_t Y0024:1; //Y0024
	uint8_t Y0025:1; //Y0025
	uint8_t Y0026:1; //Y0026
	uint8_t Y0027:1; //Y0027
	uint8_t Y0028:1; //Y0028
	uint8_t Y0029:1; //Y0029
	uint8_t Y002A:1; //Y002A
	uint8_t Y002B:1; //Y002B
	uint8_t Y002C:1; //Y002C
	uint8_t Y002D:1; //Y002D
	uint8_t Y002E:1; //Y002E
	uint8_t Y002F:1; //Y002F
//--------------------------
	uint8_t GRIPPER1_CHUCK:1; //Y0030 GRIPPER CHUCK SOL
	uint8_t GRIPPER1_UNCHUCK:1; //Y0031 GRIPPER UNCHUCK SOL
	uint8_t SAFETY_RESET:1; //Y0032 SAFETY RESET
	uint8_t DOOR_LEFT_CLOSE:1; //Y0033 KEYLOCK LEFT
	uint8_t DOOR_RIGHT_CLOSE:1; //Y0034 KEYLOCK RIGHT
	uint8_t OPBOX_RESET_LAMP:1; //Y0035 OPBOX RESET LAMP
	uint8_t SAFETY_RESET_SW_LAMP:1; //Y0036 SAFETY RESET SW LAMP
	uint8_t OPBOX_EMERGENCY_LAMP:1; //Y0037 OPBOX EMERGENCY LAMP
	uint8_t TOWER_LAMP_RED:1; //Y0038 TOWER LAMP RED
	uint8_t TOWER_LAMP_YELLOW:1; //Y0039 TOWER LAMP YELLOW
	uint8_t TOWER_LAMP_GREEN:1; //Y003A TOWER LAMP GREEN
	uint8_t TOWER_LAMP_BUZZER:1; //Y003B TOWER LAMP BUZZER
	uint8_t DOOR_OPEN_SELECT:1; //Y003C BYPASS SOL: OFF=lock key ON, ON=allow key OFF after KEYLOCK set
	uint8_t SAFETY_BYPASS_ON:1; //Y003D servo on when door open and keylock off
	uint8_t Y003E:1; //Y003E
	uint8_t Y003F:1; //Y003F
}OUTPUT_IO;
//---------------------------------------------------------------------------
typedef enum Sequence
{
	seqIdle,
    seqInit,
	seqHome,
	seqServoOn,
	seqServoOff,
	seqAutoMove,
	seqAutoEject,
	seqAutoEjectComplete,
	seqAutoInsert,
	seqAutoInsertComplete,
	seqJOGx_Plus,
	seqJOGy_Plus,
	seqJOGz_Plus,
	seqJOGg1_Plus,
	seqJOGg2_Plus,
	seqJOGx_Minus,
	seqJOGy_Minus,
	seqJOGz_Minus,
	seqJOGg1_Minus,
	seqJOGg2_Minus,
	seqJogStop,
	seqReset,
	seqWait,
	seqZup,
	seqZdown,
	seqPause,		// 일시 중지 상태 : 에러발생시
	seqAutoRun,
    seqEmgAutoRun,
	//* DRY RUN : Dedicated HOME return without production panel or cell checks.
	seqDryRunWait
} robotSequence;

typedef struct{
	short int system_status;
	long int pos[AxisCnt];
	long int speed[AxisCnt];
	int status[AxisCnt];
	int servo[AxisCnt];
	int zero[AxisCnt];
	int running[AxisCnt];
	unsigned short int system_alarm;
	unsigned short int system_detail;
	unsigned short int servo_alarm[AxisCnt];
	unsigned short int servo_detail[AxisCnt];
	unsigned short int oper_alarm[AxisCnt];
	unsigned short int oper_detail[AxisCnt];

	short int limit[AxisCnt];
	short int monnum[AxisCnt][AxisCnt];
	short int mondata[AxisCnt][AxisCnt];
}MR2;


class Trobostar : public TDataModule
{
__published:	// IDE-managed Components
	TTimer *senTimer;
	TTimer *Timer_zUpTest;
	void __fastcall senTimerTimer(TObject *Sender);
	void __fastcall DataModuleCreate(TObject *Sender);
	void __fastcall DataModuleDestroy(TObject *Sender);
private:	// User declarations
	typedef struct{
		int step;
		int delay;
		int timeout;
		robotSequence reserve;
	}STEP;

	typedef struct{
		int tool;
		int pallet;
		int type;
		int channel;
		int cnt;
	}MOVE;

	int __fastcall io_Read();
	int __fastcall io_Write();
	int __fastcall io_WriteGripper();

	void __fastcall Init();
	void __fastcall Home();
	void __fastcall ServoOn();
	void __fastcall ServoOff();
	void __fastcall MoveJog(int ntype);

	void __fastcall AutoRun();
	void __fastcall EmgAutoRun();
	void __fastcall AutoMove();
	void __fastcall AutoEject();
	void __fastcall AutoInsert();

	void __fastcall Reset();
	//* DRY RUN : Motion-only HOME return state machine.
	void __fastcall DryRunWaitPosition();
	void __fastcall WaitPosition();
	void __fastcall zUp();
	void __fastcall zDown();

	STEP step;
	STEP step_save;

	// IO 용
	bool __fastcall CheckEjectUnchuck(int pos);
	bool __fastcall CheckEjectChuck(int pos);
	bool __fastcall CheckEjectCell_after(int pos);

	bool __fastcall CheckInsertUnchuck(int pos);
	bool __fastcall CheckInsertCellReleased(int pos);

	bool btx;

	bool __fastcall WriteLog(int status, UnicodeString msg);
	bool __fastcall StartServoSystemFromParameterFile();
	int board_id;   // mr2 제어 보드용
	int channel_id;
	int timeout;
	bool sscOpened;
	bool m_ccLinkOpened;
	bool m_ccLinkRunning;
	bool keyLockSetPending;                // Y0033/Y0034 ON; waiting to turn Y003D OFF.
	bool keyLockReleasePending;            // Y003D ON; waiting to turn Y0033/Y0034 OFF.
	bool previousBypassSwitchOn;           // Detect the hardware BYPASS-key ON edge.
	DWORD keyLockSetSafetyBypassOffTick;   // Non-blocking KEYLOCK-set delay deadline.
	DWORD keyLockReleaseOutputOffTick;     // Non-blocking KEYLOCK-release delay deadline.
	DWORD safetyResetPulseUntilTick;
	PNT_DATA_EX point[AxisCnt];
	// Accepted targets are set by setPoint(), cleared on completion/req_Stop().
	PNT_DATA_EX acceptedPoint[AxisCnt];
	bool acceptedMove[AxisCnt];
	bool motionFaultLatched;
	bool StopAxes();
	void MotionFault(const AnsiString &reason);
	// Immutable snapshot of the accepted tray move. Z DOWN must match this snapshot.
	MOVE activeMove;
	long activeTarget[AxisCnt];
	bool activeMoveValid;
	bool directXYPositionReady;
	// Explicit Servo OFF invalidates HOME until a new home-return completes.
	bool homeRequiredAfterServoOff;
    bool bSetPoint;
	long jogSpeed;
	bool __fastcall setPoint(int axnum_id, unsigned long int pos);
	bool __fastcall rangeCheck(int axnum_id);
	void __fastcall mr2Sensing();

	bool __fastcall CalculatePositionValue(int pallet, int tool, int channel,
		long &x, long &y, long &z);
	bool __fastcall SetPositionValue();
	void __fastcall CaptureMoveRequest();
	bool __fastcall ValidateActiveMoveTarget();
	bool __fastcall CheckTrayCenteringMotionInterlock();
	bool centeringMotionMonitorActive;
	robotSequence centeringMotionMonitorSeq;
	bool centeringRequireSource;
	bool centeringRequireTarget;


public:		// User declarations

	MOVE move;
	robotSequence seq;

	CONFIG_ROBOSTAR config;
	OUTPUT_ROBOT output;
	OUTPUT_IO	gripper;
	INPUT_ROBOT input;
	MR2 mr2;

	int __fastcall io_Init();
	void __fastcall InitSequence(robotSequence data, robotSequence reserve = seqIdle);

	void __fastcall req_Init();
	void __fastcall req_ServoOn();
	void __fastcall req_ServoOff();
	void __fastcall req_Home();
	void __fastcall req_Speed(int speed, int accl, int dccl);
	void __fastcall req_Stop();
	//* Z LIMIT RECOVERY: LSN active permits only Z DOWN(+) jog until the limit clears.
	bool __fastcall IsZLimitActive() const;
	bool __fastcall IsZDownLimitRecoveryAllowed() const;
	void __fastcall req_JogMove(int ntype);
	void __fastcall req_AutoRun();
    void __fastcall req_EmgAutoRun();
	void __fastcall req_AutoMove(int pallet, int tool, int channel, int type);
	void __fastcall req_AutoEject(int pallet, int tool, int channel, int cnt, int type);
	void __fastcall req_AutoInsert(int pallet, int tool, int channel, int cnt, int type);

	bool __fastcall req_EjectComplete(int toolNo = 1);
	bool __fastcall req_InsertComplete(int toolNo = 1);
	bool __fastcall PrepareCellRecovery(bool cellHeld);
	bool IsRecoveryStandby();

	void __fastcall req_Reset();
	void __fastcall req_WaitPosition();
	void __fastcall req_zUp();
	//* DRY RUN : Returns Z/X/Y to zero without using production cell conditions.
	bool __fastcall req_DryRunWaitPosition();
	bool __fastcall req_zDown();
	bool __fastcall SetJogSpeed(int speed);
	int __fastcall GetJogSpeed() const;

	void __fastcall GripperChuck(int num, bool open, bool close);

	bool __fastcall getGripperChuckStatus();
	bool __fastcall getGripperOpenStatus();
	bool __fastcall IsTargetTrayUnloadSafe();
	bool __fastcall getCellDetectStatus();
	bool __fastcall getCellDetectStatus(int pos);

    int zUpCount;
    int xyMoveCount;
    int zUpStep;

	robotSequence seq_save;
	bool pauseStatus;
	void __fastcall req_Pause(bool stop);
	bool AreAxesStopped();
	bool CanResumeMotion();
	void __fastcall setTx();
	bool __fastcall CheckEjectCell_before(int pos);
	bool __fastcall KeyLock(bool on);
	bool __fastcall CanSetKeyLock() const;
	bool __fastcall Bypass(bool on);
	bool __fastcall CanEnableBypassSol() const;
	bool __fastcall RequestSafetyResetPulse();
	bool __fastcall IsSoftwareSafetyResetActive() const;
	bool __fastcall IsEmergencyStopActive() const;
	bool __fastcall IsSafetyReady() const;
	bool __fastcall IsSafetyDoorOpen(int doorNo) const;
	bool __fastcall IsKeyLockActive() const;
	bool __fastcall IsBypassActive() const;
	bool __fastcall RestoreServoState(); // Attach to the board and reuse an already RUNNING servo system.
	bool __fastcall IsSscOpened() const;
	void __fastcall SetCcLinkOpenResult(short result, long openedPath);
	bool __fastcall IsCcLinkReady() const;
	bool __fastcall IsHomeRequiredAfterServoOff() const;
	bool m_bInsertSave;
    void __fastcall Y003D(bool bOn);
	__fastcall Trobostar(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE Trobostar *robostar;
//---------------------------------------------------------------------------
#endif

