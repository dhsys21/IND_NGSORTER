//---------------------------------------------------------------------------

#ifndef ModplcH
#define ModplcH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <System.Win.ScktComp.hpp>
//---------------------------------------------------------------------------
#include <deque>
using namespace std;

#define UM_PLC	WM_USER+3000

typedef struct
{
	BYTE SubHeader[2];
	BYTE NetNum;
	BYTE PlcNum;
	WORD ReqIONum;
	BYTE ReqOfficeNum;
	WORD ReqDataLen;
	WORD CpuTime;

	WORD Command;
	WORD SubCommand;
	BYTE StartDevNum[3];
	BYTE DevCode;
	WORD DevLen;
} TPLCReadCommand;

typedef struct
{
	BYTE SubHeader[2];
	BYTE NetNum;
	BYTE PlcNum;
	WORD ReqIONum;
	BYTE ReqOfficeNum;
	WORD ReqDataLen;
	WORD CpuTime;

	WORD Command;
	WORD SubCommand;
	BYTE StartDevNum[3];
	BYTE DevCode;
	WORD DevLen;
	WORD *Data;
} TPLCWriteCommand;

typedef enum {
	dcDataRegister = 0xa8,
	dcLinkRegister = 0xb4
} TDeviceCode;

typedef enum {
	cmBinary,
	cmAscii
} TCommMode;




class Tplc : public TDataModule
{
__published:	// IDE-managed Components
	TTimer *reConnectTimer;
	TTimer *scanTimer;
	TClientSocket *client;
	void __fastcall clientError(TObject *Sender, TCustomWinSocket *Socket,
          TErrorEvent ErrorEvent, int &ErrorCode);
	void __fastcall clientDisconnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall DataModuleCreate(TObject *Sender);
	void __fastcall reConnectTimerTimer(TObject *Sender);
	void __fastcall clientRead(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall scanTimerTimer(TObject *Sender);
	void __fastcall clientConnect(TObject *Sender, TCustomWinSocket *Socket);
private:	// User declarations

	TPLCReadCommand ReadCmd;
	TPLCWriteCommand WriteCmd;
	TCommMode FCommMode;
	int NetworkNum, PLCNum;

	void __fastcall InitReadCmd(TPLCReadCommand &cmd);
	void __fastcall InitWriteCmd(TPLCWriteCommand &cmd);



	AnsiString rx_header;
	AnsiString rx_data;
	AnsiString rx_tail;


public:		// User declarations

	deque<string> txq;		// 송신 큐
	deque<unsigned int> plcAddr;
	deque<string> rxq;		// 수신 큐
	AnsiString Txstr;
	AnsiString Rxstr;


	AnsiString __fastcall ReadWordData(DWORD Address, WORD Len);
	void __fastcall WriteWordData(DWORD Address, WORD Len, AnsiString data);

	__fastcall Tplc(TComponent* Owner);
	TCommMode __property CommMode = {write=FCommMode, read=FCommMode};
};
//---------------------------------------------------------------------------
extern PACKAGE Tplc *plc;
//---------------------------------------------------------------------------
#endif
