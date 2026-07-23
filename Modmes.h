//---------------------------------------------------------------------------

#ifndef ModmesH
#define ModmesH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <System.Win.ScktComp.hpp>
#include <queue>
#include <string>

//---------------------------------------------------------------------------
#include "DEFINE.h"
using namespace std;
struct Tx_Thread_Data{
	TCustomWinSocket *tx;
	queue<string> *q;
	queue<string> *comment;
};
//---------------------------------------------------------------------------
struct Rx_Thread_Data{
	queue<string> *q;
};
//---------------------------------------------------------------------------


class Tmes : public TDataModule
{
__published:	// IDE-managed Components
	TServerSocket *ServerSocket;
	TTimer *rxTimer;
	void __fastcall ServerSocketClientConnect(TObject *Sender,
		  TCustomWinSocket *Socket);
	void __fastcall ServerSocketClientDisconnect(TObject *Sender,
		  TCustomWinSocket *Socket);
	void __fastcall ServerSocketClientRead(TObject *Sender,
		  TCustomWinSocket *Socket);
	void __fastcall DataModuleDestroy(TObject *Sender);
	void __fastcall ServerSocketClientError(TObject *Sender,
		  TCustomWinSocket *Socket, TErrorEvent ErrorEvent, int &ErrorCode);
	void __fastcall DataModuleCreate(TObject *Sender);
	void __fastcall rxTimerTimer(TObject *Sender);
private:	// User declarations
	HANDLE tx_thread;
	HANDLE rx_thread;
	queue<string> Tx_queue;
	queue<string> Rx_queue;

//	Tx_Thread_Data *tx_data;
	Rx_Thread_Data *rx_data;

	AnsiString line;    		//라인번호
	void __fastcall EndThread();

	int systemByte;
public:		// User declarations

	AnsiString pcName;
	AnsiString mesMsg;
	AnsiString savePath;
	RX_DATA mes_rx;
	bool bConnect;

	bool __fastcall DataCheck(AnsiString msg);
	void __fastcall WritemesLog(AnsiString flow, AnsiString msg);

	void __fastcall Configure(AnsiString bindIp, int bindPort);
	void __fastcall Start();
	void __fastcall Stop();
	void __fastcall SendMsg(TX_DATA *msg, bool reply = false);


	void __fastcall RecvMsg(AnsiString msg);
	bool __fastcall RecvMsg_GetMesMessage(AnsiString msg);
	bool __fastcall RecvMsg_CheckMesMessageLength(AnsiString msg);
	AnsiString __fastcall getData(AnsiString msg, AnsiString strFind);


	void __fastcall RecvMsg_LINK_TEST_EVENT();
	void __fastcall RecvMsg_TRAY_REPLY();
	void __fastcall RecvMsg_DATE_EVENT();
	void __fastcall RecvMsg_SEND_EVENT();
	void __fastcall RecvMsg_TRANSFER_IN_REPLY();
	void __fastcall RecvMsg_ID_MATCHING_REPLY();
	void __fastcall RecvMsg_etc();

	Tx_Thread_Data *tx_data;
    bool ThreadFlag;

	__fastcall Tmes(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE Tmes *mes;
//---------------------------------------------------------------------------
#endif
