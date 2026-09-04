//---------------------------------------------------------------------------

#ifndef ModGripperH
#define ModGripperH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <IniFiles.hpp>
#include "AdvSmoothMessageDialog.hpp"
//---------------------------------------------------------------------------


typedef struct{
	bool disable;
	AnsiString code;	//
	AnsiString source_ch;
	AnsiString target_ch;
	bool eject_end;
	bool insert_end;
	int conCnt;
}TOOL;

typedef struct{
	int pos;
	int conCnt;
	int gripper;
}WORK;


class Tgripper : public TDataModule
{
__published:	// IDE-managed Components
	TTimer *stepTimer;
	TTimer *waitTimer;
	void __fastcall stepTimerTimer(TObject *Sender);
	void __fastcall waitTimerTimer(TObject *Sender);
private:	// User declarations

	typedef enum Sequence
	{
		seqIdle,
		seqInit,
		seqSorting,
		seqInserting,
		seqPause,

	} gripperSequence;

	typedef struct{
		int step;
		int chCnt;		// 그리퍼 체크
		int badCnt;		// 선별 : 전체 불량 수량
		int ejectCnt;	// 취출 예정 수량
		gripperSequence reserve;
	}STEP;

	TIniFile *ini;
	bool ccLinkNotReadyReported;

	//* CELL TRANSFER RESULT : One CSV row is written after each completed transfer.
	typedef enum{
		transferPhaseNone,
		transferPhaseMoveSource,
		transferPhaseEject,
		transferPhaseMoveTarget,
		transferPhaseInsert,
		transferPhaseMoveWaiting
	} TRANSFER_PHASE;
	typedef struct{
		bool active;
		TRANSFER_PHASE phase;
		DWORD phaseStartTick;
		DWORD moveSourceChMs;
		DWORD ejectMs;
		DWORD moveTargetChMs;
		DWORD insertMs;
		DWORD moveWaitingMs;
		int sourceChannel;
		int targetChannel;
		AnsiString sourceTrayId;
		AnsiString targetTrayId;
		int peakLoad[3];
	} TRANSFER_RESULT;
	TRANSFER_RESULT transferResult;
	TRANSFER_RESULT pendingTransferResult;
	bool pendingTransferResultValid;
	bool deferTargetReservationSave;
	void __fastcall ResetTransferResult();
	void __fastcall BeginTransferResult(int toolIndex);
	void __fastcall StartTransferPhase(TRANSFER_PHASE phase);
	void __fastcall UpdateTransferResult();
	void __fastcall SaveTransferResultRecord(const TRANSFER_RESULT &result, bool waitBypassed);
	void __fastcall SavePendingTransferResult(bool waitBypassed);
	void __fastcall SaveTransferResult(bool waitBypassed);
	void __fastcall Initialize();
	void __fastcall Sorting();
	void __fastcall Inserting();
	void __fastcall StartNextCycleOrWait();

	WORK eject;
	WORK insert;

	void __fastcall InitSequence(gripperSequence data, gripperSequence reserve = seqIdle);
public:		// User declarations

	bool disable_gripper[gripCnt+1];	// 그리퍼 사용여부
	TOOL tool[gripCnt];

	void __fastcall req_Sorting();
	void __fastcall req_Init();
	bool __fastcall CommitEjectTrayState(int toolNo);
	bool __fastcall CommitInsertTrayState(int toolNo);

	gripperSequence seq_save;
	bool pauseStatus;
	void __fastcall req_Pause(bool stop);
    bool __fastcall getReadyStatus();

	bool __fastcall GetZoneCode(int zone, AnsiString code);

	STEP step;

	gripperSequence seq;

	__fastcall Tgripper(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE Tgripper *gripper;
//---------------------------------------------------------------------------
#endif


