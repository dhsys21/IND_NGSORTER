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

typedef struct{
	uint8_t EMS_SWITCH:1; //RX00
	uint8_t SAFETY_DOOR_1:1; //RX01
	uint8_t SAFETY_DOOR_2:1; //RX02
	uint8_t SAFETY_DOOR_3:1; //RX03
	uint8_t RX04:1; //RX04
	uint8_t SAFETY_CONT_ERROR:1; //RX05
	uint8_t RX06:1; //RX06 0p
	uint8_t RX07:1; //RX07
	uint8_t CP04_TRIP:1; //RX08
	uint8_t CP05_TRIP:1; //RX09
	uint8_t CP06_TRIP:1; //RX0A	/INRNG
	uint8_t CP07_TRIP:1; //RX0B
	uint8_t CP08_TRIP:1; //RX0C
	uint8_t RX0D:1; //RX0D
	uint8_t RX0E:1; //RX0E
	uint8_t RX0F:1; //RX0F
//--------------------------
	uint8_t RX10:1; //RX10
	uint8_t RX11:1; //RX11
	uint8_t CP11_TRIP:1; //RX12
	uint8_t CP12_TRIP:1; //RX13
	uint8_t CP13_TRIP:1; //RX14
	uint8_t CP14_TRIP:1; //RX15
	uint8_t CP15_TRIP:1; //RX16
	uint8_t CP16_TRIP:1; //RX17
	uint8_t RX18:1; //RX18
	uint8_t RX19:1; //RX19
	uint8_t RX1A:1; //RX1A
	uint8_t CP21_TRIP:1; //RX1B
	uint8_t CP22_TRIP:1; //RX1C
	uint8_t RX1D:1; //RX1D
	uint8_t RX1E:1; //RX1E
	uint8_t RX1F:1; //RX1F
//--------------------------
	uint8_t RX20:1; //RX20
	uint8_t RX21:1; //RX21
	uint8_t RX22:1; //RX22
	uint8_t RX23:1; //RX23
	uint8_t RX24:1; //RX24
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
	uint8_t GRIPPER1_UP:1; //RX40
	uint8_t GRIPPER1_DOWN:1; //RX41
	uint8_t GRIPPER1_BUFFER:1; //RX42
	uint8_t GRIPPER1_CELL_DETECT:1; //RX43
	uint8_t GRIPPER1_UNCHUCK:1; //RX44
	uint8_t GRIPPER1_FLOAT:1; //RX45
	uint8_t RX46:1; //RX46
	uint8_t RX47:1; //RX47
	uint8_t RX48:1; //RX48
	uint8_t RX49:1; //RX49
	uint8_t RX4A:1; //RX4A
	uint8_t RX4B:1; //RX4B
	uint8_t RX4C:1; //RX4C
	uint8_t RX4D:1; //RX4D
	uint8_t RX4E:1; //RX4E
	uint8_t RX4F:1; //RX4F
//--------------------------
	uint8_t RX50:1; //RX50
	uint8_t RX51:1; //RX51
	uint8_t RX52:1; //RX52
	uint8_t RX53:1; //RX53
	uint8_t RX54:1; //RX54
	uint8_t RX55:1; //RX55
	uint8_t RX56:1; //RX56
	uint8_t RX57:1; //RX57
	uint8_t RX58:1; //RX58
	uint8_t RX59:1; //RX59
	uint8_t RX5A:1; //RX5A
	uint8_t RX5B:1; //RX5B
	uint8_t RX5C:1; //RX5C
	uint8_t RX5D:1; //RX5D
	uint8_t RX5E:1; //RX5E
	uint8_t RX5F:1; //RX5F
//--------------------------
}INPUT_ROBOT;
//---------------------------------------------------------------------------
typedef struct{
	uint8_t SERVO_ON[4];
	uint8_t SERVO_HOME[4];
	uint8_t CYLINDER_Z:1;

	uint8_t SERVO03_OK_HOME:1; //RX09
	uint8_t SERVO04_INPOS:1; //RX0A	/INRNG
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
	uint8_t GRIPPER1_UP:1; //RX20
	uint8_t GRIPPER1_DN:1; //RX21
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

typedef struct{
//--------------------------
	uint8_t TOWER_LAMP_RED:1; //RY20
	uint8_t TOWER_LAMP_YELLOW:1; //RY51
	uint8_t TOWER_LAMP_GREEN:1; //RY52
	uint8_t TOWER_LAMP_BUZZER:1; //RY53
	uint8_t RY24:1; //RY24
	uint8_t RY25:1; //RY25
	uint8_t RY26:1; //RY26
	uint8_t RY27:1; //RY27
	uint8_t RY28:1; //RY28
	uint8_t RY29:1; //RY29
	uint8_t RY2A:1; //RY2A
	uint8_t RY2B:1; //RY2B
	uint8_t RY2C:1; //RY2C
	uint8_t RY2D:1; //RY2D
	uint8_t RY2E:1; //RY2E
	uint8_t RY2F:1; //RY2F
//--------------------------
	uint8_t RY30:1; //RY30
	uint8_t RY31:1; //RY31
	uint8_t RY32:1; //RY32
	uint8_t RY33:1; //RY33
	uint8_t RY34:1; //RY34
	uint8_t RY35:1; //RY35
	uint8_t RY36:1; //RY36
	uint8_t RY37:1; //RY37
	uint8_t RY38:1; //RY38
	uint8_t RY39:1; //RY39
	uint8_t RY3A:1; //RY3A
	uint8_t RY3B:1; //RY3B
	uint8_t RY3C:1; //RY3C
	uint8_t RY3D:1; //RY3D
	uint8_t RY3E:1; //RY3E
	uint8_t DOOR_OPEN_SELECT:1; //RY3F
    //--------------------------
	uint8_t RY40:1; //RY40
	uint8_t RY41:1; //RY41
	uint8_t RY42:1; //RY42
	uint8_t RY43:1; //RY43
	uint8_t RY44:1; //RY44
	uint8_t RY45:1; //RY45
	uint8_t RY46:1; //RY46
	uint8_t RY47:1; //RY47
	uint8_t RY48:1; //RY48
	uint8_t RY49:1; //RY49
	uint8_t RY4A:1; //RY4A
	uint8_t RY4B:1; //RY4B
	uint8_t RY4C:1; //RY4C
	uint8_t RY4D:1; //RY4D
	uint8_t RY4E:1; //RY4E
	uint8_t RY4F:1; //RY4F
	//--------------------------
	uint8_t RY50:1; //RY50
	uint8_t RY51:1; //RY51
	uint8_t RY52:1; //RY52
	uint8_t RY53:1; //RY53
	uint8_t RY54:1; //RY54
	uint8_t RY55:1; //RY55
	uint8_t RY56:1; //RY56
	uint8_t RY57:1; //RY57
	uint8_t RY58:1; //RY58
	uint8_t RY59:1; //RY59
	uint8_t RY5A:1; //RY5A
	uint8_t RY5B:1; //RY5B
	uint8_t RY5C:1; //RY5C
	uint8_t RY5D:1; //RY5D
	uint8_t RY5E:1; //RY5E
	uint8_t RY5F:1; //RY5F
//--------------------------
    uint8_t RY60:1; //RY60
	uint8_t RY61:1; //RY61
	uint8_t GRIPPER1_DOWN_SOL:1; //RY62
	uint8_t GRIPPER1_UP_SOL:1; //RY63
	uint8_t RY64:1; //RY64
	uint8_t RY65:1; //RY65
	uint8_t RY66:1; //RY66
	uint8_t RY67:1; //RY67
	uint8_t RY68:1; //RY68
	uint8_t RY69:1; //RY69
	uint8_t RY6A:1; //RY6A
	uint8_t RY6B:1; //RY6B
	uint8_t RY6C:1; //RY6C
	uint8_t RY6D:1; //RY6D
	uint8_t RY6E:1; //RY6E
	uint8_t RY6F:1; //RY6F
//--------------------------
	uint8_t GRIPPER1_UNCHUCK:1; //RY70
	uint8_t GRIPPER1_CHUCK:1; //RY71
	uint8_t RY72:1; //RY72
	uint8_t RY73:1; //RY73
	uint8_t RY74:1; //RY74
	uint8_t RY75:1; //RY75
	uint8_t RY76:1; //RY76
	uint8_t RY77:1; //RY77
	uint8_t RY78:1; //RY78
	uint8_t RY79:1; //RY79
	uint8_t RY7A:1; //RY7A
	uint8_t RY7B:1; //RY7B
	uint8_t RY7C:1; //RY7C
	uint8_t RY7D:1; //RY7D
	uint8_t RY7E:1; //RY7E
	uint8_t RY7F:1; //RY7F
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
	seqPause,		// 일시 중지 상태 : 에러발생시
	seqAutoRun,
    seqEmgAutoRun
} robotSequence;

typedef struct{
	short int system_status;
	long int pos[AxisCnt];
	long int speed[AxisCnt];
	int status[AxisCnt];
	int servo[AxisCnt];
	int zero[AxisCnt];
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
	void __fastcall Timer_zUpTestTimer(TObject *Sender);
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
	void __fastcall WaitPosition();
	void __fastcall zUp();

	STEP step;
	STEP step_save;

	// IO 용
	bool __fastcall CheckEjectUnchuck(int pos);
	bool __fastcall CheckEjectDown(int pos);
	bool __fastcall CheckEjectChuck(int pos);
	bool __fastcall CheckEjectUp(int pos);
	bool __fastcall CheckEjectCell_after(int pos);

	bool __fastcall CheckInsertDown(int pos);
	bool __fastcall CheckInsertUnchuck(int pos);
	bool __fastcall CheckInsertUp(int pos);

	bool btx;

	bool __fastcall WriteLog(int status, UnicodeString msg);
	int board_id;   // mr2 제어 보드용
	int channel_id;
	int timeout;
    PNT_DATA_EX point[AxisCnt];
    bool bSetPoint;
	bool __fastcall setPoint(int axnum_id, unsigned long int pos);
	bool __fastcall rangeCheck(int axnum_id);
	void __fastcall mr2Sensing();

	void __fastcall SetPositionValue();


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
	void __fastcall req_JogMove(int ntype);
	void __fastcall req_AutoRun();
    void __fastcall req_EmgAutoRun();
	void __fastcall req_AutoMove(int pallet, int tool, int channel, int type);
	void __fastcall req_AutoEject(int pallet, int tool, int channel, int cnt, int type);
	void __fastcall req_AutoInsert(int pallet, int tool, int channel, int cnt, int type);

	void __fastcall req_EjectComplete();
	void __fastcall req_InsertComplete();

	void __fastcall req_Reset();
	void __fastcall req_WaitPosition();
	void __fastcall req_zUp();

	void __fastcall GripperDown(int num, bool down, bool up);
	void __fastcall GripperChuck(int num, bool open, bool close);

	bool __fastcall getGripperDownStatus();
	bool __fastcall getGripperUpStatus();
	bool __fastcall getGripperChuckStatus();
	bool __fastcall getCellDetectStatus();
	bool __fastcall getCellDetectStatus(int pos);

	int __fastcall CheckFlow();
    int zUpCount;
    int xyMoveCount;
    int zUpStep;

	robotSequence seq_save;
	bool pauseStatus;
	void __fastcall req_Pause(bool stop);
	void __fastcall setTx();
	bool __fastcall CheckEjectCell_before(int pos);
	bool __fastcall KeyLock(int pos);
	bool m_bInsertSave;

	__fastcall Trobostar(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE Trobostar *robostar;
//---------------------------------------------------------------------------
#endif


