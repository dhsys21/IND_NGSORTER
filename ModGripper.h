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


	void __fastcall Initialize();
	void __fastcall Sorting();
	void __fastcall Inserting();

	WORK eject;
	WORK insert;

//	STEP step;

//	gripperSequence seq;


	void __fastcall InitSequence(gripperSequence data, gripperSequence reserve = seqIdle);


public:		// User declarations

	bool disable_gripper[3];	// 그리퍼 사용여부
	TOOL tool[2];

	void __fastcall req_Sorting();
	void __fastcall req_Init();

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


