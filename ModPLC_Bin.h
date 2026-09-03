//---------------------------------------------------------------------------

#ifndef ModPLC_BinH
#define ModPLC_BinH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <System.Win.ScktComp.hpp>
#include "Define.h"

#include <deque>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//	디바이스 코드
//---------------------------------------------------------------------------
const int DEVCODE_M			=	0x90;		//	내부 릴레이
const int DEVCODE_L			=	0x92;	  	//	래치 릴레이
const int DEVCODE_D			=	0xA8;		//	데이터 레지스터
const int DEVCODE_W			=	0xB4;		//	링크 레지스터
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//	Index 구분
//---------------------------------------------------------------------------
const int PLC_INDEX_INTERFACE					=		1;
const int PC_INDEX_INTERFACE					=		11;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//	시작 번지
//---------------------------------------------------------------------------
const int PLC_D_INTERFACE_START_DEV_NUM	 			=	10100;
const int PLC_D_INTERFACE_LEN 						= 	7;
//---------------------------------------------------------------------------
const int PC_D_INTERFACE_START_DEV_NUM				=	10150;
const int PC_D_INTERFACE_LEN	 					= 	9;

//---------------------------------------------------------------------------
//	PLC - PC Interface
//---------------------------------------------------------------------------
const int PLC_D_HEART_BEAT				   			=	0;
const int PLC_D_AUTO_MANUAL			                =   1;
const int PLC_D_ERROR    	  				        =   2;
const int PLC_D_SOURCE_TRAY_IN 				        =   3;
const int PLC_D_SOURCE_CENTERING                    =   4;
const int PLC_D_TARGET_TRAY_IN 				        =   5;
const int PLC_D_TARGET_CENTERING                    =   6;

//---------------------------------------------------------------------------
//	PC - PLC Interface
//---------------------------------------------------------------------------
const int PC_D_HEART_BEAT			  				=	0;
const int PC_D_AUTO_MANUAL			     	    	=   1;
const int PC_D_ERROR    	  			    		=   2;
const int PC_D_TRAY_IN_READY                        =   3;
const int PC_D_SOURCE_CENTERING_REQ                 =   4;
const int PC_D_SOURCE_TRAY_OUT 		    			=   5;
const int PC_D_TARGET_TRAY_OUT                      =   6;
const int PC_D_EMERGENCY                            =   7;
const int PC_D_DOOR_OPEN                            =   8;

//---------------------------------------------------------------------------
//	PLC Header
//---------------------------------------------------------------------------
typedef struct
{
	unsigned char SubHeader[2];			//	서브 헤더
	unsigned char NetNum;		 		//	네트워크 번호
	unsigned char PlcNum;	   			//	PLC 번호
	unsigned char ReqIONum[2];			//	IO 번호
	unsigned char ReqOfficeNum;	  		//	국 번호
	unsigned char ReqDataLen[2];		//  요구 데이터 길이(CPU 감시 타이머 ~ 디바이스 길이)
	unsigned char CpuTime[2];			//	CPU 감시 타이머
	unsigned char Command[2];			//	커맨드
	unsigned char SubCommand[2];		//	서브 커맨드 (0 = 비트-16단위, 워드-1단위 / 1 = 비트-1단위)
	unsigned char StartDevNum[3];		//	선두 디바이스
	unsigned char DevCode;  			//	디바이스 코드
	unsigned char DevLen[2];			//	디바이스 길이
} PLC_DATA;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//	PC Header
//---------------------------------------------------------------------------
typedef struct
{
	unsigned char SubHeader[2];					//	서브 헤더
	unsigned char NetNum;						//	네트워크 번호
	unsigned char PlcNum;	    				//	PLC 번호
	unsigned char ReqIONum[2];					//	IO 번호
	unsigned char ReqOfficeNum;	  				//	국 번호
	unsigned char ReqDataLen[2];				//  요구 데이터 길이(CPU 감시 타이머 ~ 디바이스 길이)
	unsigned char CpuTime[2];					//	CPU 감시 타이머
	unsigned char Command[2];					//	커맨드
	unsigned char SubCommand[2];				//	서브 커맨드
	unsigned char StartDevNum[3];				//	선두 디바이스
	unsigned char DevCode;  					//	디바이스 코드
	unsigned char DevLen[2];					//	디바이스 길이
} PC_DATA;
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
class TPlcBin : public TDataModule
{
__published:	// IDE-managed Components
	TTimer *Timer_PLC_WriteMsg;
	TClientSocket *ClientSocket_PLC;
	TClientSocket *ClientSocket_PC;
	TTimer *Timer_PLC_AutoConnect;
	TTimer *Timer_PC_AutoConnect;
	TTimer *Timer_PC_WriteMsg;
	void __fastcall Timer_PC_AutoConnectTimer(TObject *Sender);
	void __fastcall Timer_PLC_AutoConnectTimer(TObject *Sender);
	void __fastcall ClientSocket_PCConnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall ClientSocket_PCDisconnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall ClientSocket_PCError(TObject *Sender, TCustomWinSocket *Socket,
          TErrorEvent ErrorEvent, int &ErrorCode);
	void __fastcall ClientSocket_PCRead(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall ClientSocket_PLCConnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall ClientSocket_PLCDisconnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall ClientSocket_PLCError(TObject *Sender, TCustomWinSocket *Socket,
          TErrorEvent ErrorEvent, int &ErrorCode);
	void __fastcall ClientSocket_PLCRead(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall Timer_PC_WriteMsgTimer(TObject *Sender);
	void __fastcall Timer_PLC_WriteMsgTimer(TObject *Sender);


private:	// User declarations
    bool bClose;
    PLC_DATA plc_Data;
    PC_DATA pc_Data;

    bool plc_ReadFlag;
    AnsiString plc_Read, plc_Read_Temp;
	int plc_ReadCount, plc_index;
	DWORD lastPlcStatusTick; // Last successfully parsed D10100-D10106 response.
    void __fastcall PLC_Initialization();
    void __fastcall PLC_DataChange(int subCommand, int address, int devCode, int devLen);
    void __fastcall PLC_Recv_Interface();

    bool pc_ReadFlag;
    AnsiString pc_Read, pc_Read_Temp;
	int pc_ReadCount, pc_index;
	DWORD lastPcHeartBeatTick; // D10150 changes once every 1 second; communication remains 200ms.
    void __fastcall PC_Initialization();
    void __fastcall PC_DataChange(int subCommand, int address, int devCode, int devLen);
public:		// User declarations
	__fastcall TPlcBin(TComponent* Owner);
    void __fastcall Connect(AnsiString ip, int port1, int port2);
	void __fastcall DisConnect();


    unsigned char plc_Interface_Data[PLC_D_INTERFACE_LEN][2];
	unsigned char pc_Interface_Data[PC_D_INTERFACE_LEN][2];

    void __fastcall SetData(unsigned char (*data)[2], int column, int num, bool flag);
	void __fastcall SetDouble(unsigned char (*data)[2], int column, double value);
	void __fastcall SetString(unsigned char (*data)[2], int column, AnsiString msg);

	int __fastcall GetData(unsigned char (*data)[2], int column, int num);
	double __fastcall GetDouble(unsigned char (*data)[2], int column);
	AnsiString __fastcall GetString(unsigned char (*data)[2], int column, int count);

    AnsiString __fastcall GetPlcValue(int plc_address, int size);
    double __fastcall GetPlcValue(int plc_address);
    int __fastcall GetPlcData(int plc_address, int bit_num);
    double __fastcall GetPcValue(int pc_address);
    void __fastcall SetPcData(int pc_address, int bit_num, bool bValue);
    void __fastcall SetPcValue(int pc_address, int value);

    // PLC -> PC status words.
    bool __fastcall IsPlcHeartBeatOn();
    bool __fastcall IsPlcAutoMode();
    bool __fastcall IsPlcError();
    bool __fastcall IsSourceTrayIn();
    bool __fastcall IsSourceCentering();
    bool __fastcall IsTargetTrayIn();
    bool __fastcall IsTargetCentering();
    bool __fastcall IsPlcStatusFresh(DWORD maxAgeMs = 1000);

    // PC -> PLC command buffer values.
    bool __fastcall IsPcHeartBeatOn();
    bool __fastcall IsPcAutoMode();
    bool __fastcall IsPcErrorOn();
    bool __fastcall IsTrayInReadyOn();
    bool __fastcall IsSourceCenteringRequestOn();
    bool __fastcall IsSourceTrayOutOn();
    bool __fastcall IsTargetTrayOutOn();
    bool __fastcall IsPcEmergencyOn();
    bool __fastcall IsPcDoorOpenOn();
    void __fastcall CmdPcHeartBeat(bool bOn);
    void __fastcall CmdPcAutoMode(bool bAuto);
    void __fastcall CmdPcError(bool bOn);
    void __fastcall CmdTrayInReady(bool bOn);
    void __fastcall CmdSourceCenteringRequest(bool bOn);
    void __fastcall CmdSourceTrayOut(bool bOn);
    void __fastcall CmdTargetTrayOut(bool bOn);
    void __fastcall CmdPcEmergency(bool bOn);
    void __fastcall CmdPcDoorOpen(bool bOn);
};
//---------------------------------------------------------------------------
extern PACKAGE TPlcBin *PlcBin;
//---------------------------------------------------------------------------
#endif
