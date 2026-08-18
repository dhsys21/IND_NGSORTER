#ifndef RVMO_define
#define RVMO_define

#define APP_PATH					"D:\\NGSORTER_IND\\"
#define SAVE_PATH					"D:\\NGSORTER_IND\\"

#define BIN							APP_PATH"Bin\\"
#define CONFIG_PATH                 APP_PATH"Config\\"
#define TRAY_TEACHING_FILE          CONFIG_PATH"TrayTeaching96.ini"
#define LOG                         SAVE_PATH"Log\\"
#define SOCK_LOG					LOG"MES_LOG\\"
#define PROG_LOG					LOG"PROG_LOG\\"
#define ERROR_LOG					LOG"ERROR_LOG\\"

typedef struct
{
	AnsiString MSG_ID;
	AnsiString MSG_LEN;
	AnsiString SYSTEM_BYTES;
	AnsiString EQUIP_ID;
	AnsiString LOT_ID;
	AnsiString FROM;
	AnsiString TO;
	AnsiString RECIPE_ID;
	AnsiString USER_ID;
	AnsiString OPER;
	AnsiString PROCESS;
	AnsiString DATE;
	AnsiString TRAY_GUBUN;
	AnsiString DATA;
}RX_DATA;
//---------------------------------------------------------------------------
typedef struct
{
	AnsiString MSG_ID;
	AnsiString MSG_LEN;
	AnsiString SYSTEM_BYTES;
	AnsiString EQUIP_ID;
	AnsiString LOT_ID;
	AnsiString FROM;
	AnsiString TO;
	AnsiString RECIPE_ID;
	AnsiString USER_ID;
	AnsiString OPER;
	AnsiString PROCESS;
	AnsiString DATE;
	AnsiString DATA;
	AnsiString errMsg;
}TX_DATA;
//---------------------------------------------------------------------------
typedef struct{
	 AnsiString RETURN_VALUE;	         			//정상;1, 오류;0
	 AnsiString ERROR_MSG;	         				//Error Message
	 int SLOT_COUNT;                     	//SLOT COUNT ( 기본24 EA )
	AnsiString SLOT_POSITION[96];          	//현재 TRAY 안의 CELL 위치 ( 1, 2, 3, 4 … ) , 공백 없는 것으로 판단.
	AnsiString TARGET_SLOT_POSITION[96];      //대상 TRAY 안의 CELL 위치 ( 1. 3. 8, 9 … )
	AnsiString SLOT_ID[96];                   //SLOT cell ID
	AnsiString CELL_LOT_ID[96];               //TrackIn cell LotId (preserved for TrackOut)
	AnsiString LOSS_CD[96];                   //SLOT 불량코드
	AnsiString LOSS_DESC[96];                 //SLOT 의 불량 명
	AnsiString PICK[96];                      //배출요청 ; Y (불량 선별기, 랭크 선별 장비 이거보고 처리 )
	AnsiString RANK[96];                      //CELL 랭크 정보 ( A, B, C, D … ) , 없을 수 있음
	AnsiString SAMPLE_CODE[96];            	//sample 취출 구분 코드(sample선별기와 코드가 일치하면 취출함)
	 AnsiString PASS;                               //Y  ; 현재 위치 장비 정상 진행하지 않고 다음 물류 이동 ( 예 ; IR/OCV 검사하지 않고 다음공정 진행 )
	 AnsiString CONTACT_FLAG;	         			//Contact 진행 여부, Y 일 경우 진행, N 일 경우 진행하지 않음 (충방전기, 출력 선별 설비 전용)
	 AnsiString KIND;                               //기종 ( 2층 격벽체결’ 해체 사용 )
	 AnsiString CH_GUBUN;                       // 96/962/48/482 - 선별트레이  48채널중 롱셀은 2열. 482, 두꺼운 셀은 4열. 48, 96채널중 Audi PPE는 대상트레이 2열 962, 나머지는 4열 96
	 AnsiString STOPPERTYPE;                   		// StopperType; RIVET,BOLT
	 AnsiString BATCH;                              // 화성 배치(Rank선별기에서 사용)
	 AnsiString SHIPBATCH;                          // 출하 소 배치 (Rank 선별기에서 사용)
	 AnsiString EMPTY_FLAG;
	 AnsiString TYPE;
	 AnsiString WORK_TYPE;
	 AnsiString WORK_CODE;
	 AnsiString NUTEJOIN_FLAG;
	 AnsiString TRAY_GUBUN;
	 TDateTime startTime;
	 int remainCnt;
     bool empTray;
}TRAY_INFO;
//---------------------------------------------------------------------------
/* Legacy ASCII PLC input structure. Use TPlcBin status APIs.
typedef struct{
	uint8_t AUTO:1; 		//	물류 자동
	uint8_t PLC_ERROR:1; 	//	PLC ERROR
	uint8_t SRC_ARRIVE:1; 	//	선별 도착
	uint8_t SRC_READY:1; 	//	선별 센터링
	uint8_t SRC_OUT:1; 		//	선별 배출
	uint8_t TARGET_READY:1; //	대상 센터링
	uint8_t TARGET_OUT:1; 	//	대상 배출
	uint8_t TARGET_ARRIVE:1;//	대상 도착

	uint8_t IN_09:1;
	uint8_t IN_10:1;
	uint8_t IN_11:1;
	uint8_t IN_12:1;
	uint8_t IN_13:1;
	uint8_t IN_14:1;
	uint8_t IN_15:1;
	uint8_t IN_16:1;
}PLC_INPUT;
*/
//---------------------------------------------------------------------------
/* Legacy ASCII PLC output structure. Use TPlcBin command/status APIs.
typedef struct{
	uint8_t OUT_01:1;
	uint8_t OUT_02:1;

	uint8_t SRC_WORK:1; 	//	선별 센터링 명령
	uint8_t SRC_OUT:1; 		//	선별 배출

	uint8_t SRC_EMP:1;
	uint8_t TARGET_OUT:1; 	//	대상 배출
	uint8_t OUT_07:1;
	uint8_t OUT_08:1;

	uint8_t OUT_09:1;
	uint8_t OUT_10:1;
	uint8_t OUT_11:1;
	uint8_t OUT_12:1;
	uint8_t OUT_13:1;
	uint8_t OUT_14:1;
	uint8_t OUT_15:1;
	uint8_t SRC_MANUAL_WORK:1;
}PLC_OUTPUT;
*/
//---------------------------------------------------------------------------

const int SEND = 1;
const int RECEIVE = 2;
const int ETC = 3;

const char nAuto 	= 0;
const char nLocal 	= 1;
const int servoCnt 	= 3;  //* X, Y, Z 3개. 헝가리는 X1, X2, Y, Z 총 4개
const int gripCnt 	= 1;  //* gripper 1개

const int Axis_x = 1;
const int Axis_y = 2;
const int Axis_z = 3;
const int Axis_zUp = 0;
const int AxisCnt = 4;  //* System=0, X=1, Y=2, Z=3
const int Wait_xAxis = 0; //* 원점센서 위치에 따라 대기 장소가 달라짐.
const int Wait_yAxis = 0;
const int TraySlotCount = 96;
const int TrayTeachingGroupSize = 12;
const int TrayCellPitch = 45000;

typedef struct{
	int arl;
	int stage;
	bool init;
	int err;
	int alarm_status;
	int alarm_cnt;
	int now_status;
	int limitCnt;
}STAGE_INFO;

enum TrayAxisEdit
{
    asSourceX,
    asSourceY,
    asTargetX,
    asTargetY
};


#endif
