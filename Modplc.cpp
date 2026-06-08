//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
Tplc *plc;
//---------------------------------------------------------------------------
__fastcall Tplc::Tplc(TComponent* Owner)
	: TDataModule(Owner)
{
	this->NetworkNum = 0;
	this->PLCNum = 0xff;
	this->CommMode = cmAscii;
	Rxstr = "";
	Txstr = "";
}
//---------------------------------------------------------------------------
class TxplcThread : public TThread
{
private:
	unsigned char txData[32];
	int length;
protected:
	int send_time, current_time;

	void __fastcall Execute()
	{
		while(!Terminated){
			Synchronize(&SendFunc);
			Sleep(100);
		}
	}
public:
	__fastcall TxplcThread(bool CreateSuspended) : TThread(CreateSuspended)
	{
		FreeOnTerminate = true;
	}

	void __fastcall SendFunc()
	{
		if(plc->client->Active)
		{
			if(plc->Txstr.IsEmpty()){
				if(plc->txq.empty() == false){
					plc->Txstr = plc->txq.front().data();
					plc->txq.pop_front();
					plc->client->Socket->SendText(plc->Txstr);
				}
				send_time = GetTickCount();
			}
			else{
				length = GetTickCount();
				if(abs(length - send_time) >= 1000){
						plc->plcAddr.pop_front();
						plc->Txstr = "";
						plc->client->Close();
				}
			}
		}
	}
};
//---------------------------------------------------------------------------


AnsiString __fastcall Tplc::ReadWordData(DWORD Address, WORD Len)
{
	TPLCReadCommand cmd;
	int data_cnt = 0;

	this->InitReadCmd(cmd);

	AnsiString ascii_cmd, reply;

	ascii_cmd = IntToHex(cmd.SubHeader[0], 2);
	ascii_cmd += IntToHex(cmd.SubHeader[1], 2);
	ascii_cmd += IntToHex(cmd.NetNum, 2);
	ascii_cmd += IntToHex(cmd.PlcNum, 2);
	ascii_cmd += IntToHex(cmd.ReqIONum, 4);
	ascii_cmd += IntToHex(cmd.ReqOfficeNum, 2);
	ascii_cmd += IntToHex(cmd.ReqDataLen, 4);
	ascii_cmd += IntToHex(cmd.CpuTime, 4);

	ascii_cmd += IntToHex(cmd.Command, 4);
	ascii_cmd += IntToHex(cmd.SubCommand, 4);

	ascii_cmd += "D*";

	ascii_cmd += IntToHex((int)Address , 6);

	ascii_cmd += IntToHex(Len, 4);

	plcAddr.push_back(Address);
	txq.push_back(ascii_cmd.c_str());
	return "";
}
//---------------------------------------------------------------------------

void __fastcall Tplc::WriteWordData(DWORD Address, WORD Len, AnsiString data)
{
	TPLCWriteCommand cmd;
	int data_cnt = 0;

	this->InitWriteCmd(cmd);

	AnsiString ascii_cmd, reply;

	ascii_cmd = IntToHex(cmd.SubHeader[0], 2);
	ascii_cmd += IntToHex(cmd.SubHeader[1], 2);
	ascii_cmd += IntToHex(cmd.NetNum, 2);
	ascii_cmd += IntToHex(cmd.PlcNum, 2);
	ascii_cmd += IntToHex(cmd.ReqIONum, 4);
	ascii_cmd += IntToHex(cmd.ReqOfficeNum, 2);
	ascii_cmd += IntToHex( (cmd.ReqDataLen * 2)+data.Length(), 4);
	ascii_cmd += IntToHex(cmd.CpuTime, 4);

	ascii_cmd += IntToHex(cmd.Command, 4);
	ascii_cmd += IntToHex(cmd.SubCommand, 4);

	ascii_cmd += "D*";
	ascii_cmd += IntToHex((int)Address , 6);
	ascii_cmd += IntToHex(Len, 4);
	ascii_cmd += data;

	plcAddr.push_back(1);
	txq.push_back(ascii_cmd.c_str());
}
//---------------------------------------------------------------------------
void __fastcall Tplc::InitReadCmd(TPLCReadCommand &cmd)
{
	cmd.SubHeader[0] = 0x50;  // fixed
	cmd.SubHeader[1] = 0x00; 	// fixed
	cmd.NetNum = NetworkNum;  // 네트워크 번호
	cmd.PlcNum = PLCNum;      // PLC 번호
	cmd.ReqIONum = 0x03ff;  		// fixed
	cmd.ReqOfficeNum = 0; 		// fixed
	cmd.ReqDataLen = 0x0018;			// CPU Time 포함해서 데이터 길이
	cmd.CpuTime = 0x0010;      	// Cpu Time
	cmd.Command = 0x0401;
	cmd.SubCommand = 0x0000;
}
//---------------------------------------------------------------------------
void __fastcall Tplc::InitWriteCmd(TPLCWriteCommand &cmd)
{
	cmd.SubHeader[0] = 0x50;  // fixed
	cmd.SubHeader[1] = 0x00; 	// fixed
	cmd.NetNum = NetworkNum;  // 네트워크 번호
	cmd.PlcNum = PLCNum;      // PLC 번호
	cmd.ReqIONum = 0x03ff;  		// fixed
	cmd.ReqOfficeNum = 0; 		// fixed

	cmd.ReqDataLen = 12;			// CPU Time 포함해서 데이터 길이

	cmd.CpuTime = 0x0010;      	// Cpu Time
	cmd.Command = 0x1401;
	cmd.SubCommand = 0x0000;

}
//---------------------------------------------------------------------------
void __fastcall Tplc::clientError(TObject *Sender, TCustomWinSocket *Socket,
	  TErrorEvent ErrorEvent, int &ErrorCode)
{
	ErrorCode = 0;
	Socket->Close();

}
//---------------------------------------------------------------------------


void __fastcall Tplc::clientDisconnect(TObject *Sender,
	  TCustomWinSocket *Socket)
{
	scanTimer->Enabled = false;
	reConnectTimer->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall Tplc::DataModuleCreate(TObject *Sender)
{
	TxplcThread *hTx = new TxplcThread(false);
	hTx;
}
//---------------------------------------------------------------------------

void __fastcall Tplc::reConnectTimerTimer(TObject *Sender)
{
	client->Active = true;
	reConnectTimer->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall Tplc::clientRead(TObject *Sender, TCustomWinSocket *Socket)
{
	AnsiString reply;

	int addr = plcAddr.front();
	int nLength = 0;

	Rxstr = Socket->ReceiveText();

	SendMessage(MainForm->Handle, UM_PLC, addr, (LPARAM)&Rxstr);

	Txstr = "";
	Rxstr = "";
	plcAddr.pop_front();
}
//---------------------------------------------------------------------------

void __fastcall Tplc::scanTimerTimer(TObject *Sender)
{
	static bool bflag = 1;
	if(txq.empty()){
		switch(this->Tag){
			case 1:
				if(bflag)ReadWordData(0x4010, 1);
				else ReadWordData(0x4011, 1);
				break;
			case 2:
				if(bflag)ReadWordData(0x4000, 1);
				else ReadWordData(0x4001, 1);
				break;
			default: break;
		}
				bflag = !bflag;
	}
}
//---------------------------------------------------------------------------

void __fastcall Tplc::clientConnect(TObject *Sender, TCustomWinSocket *Socket)
{
	scanTimer->Enabled = true;	
}
//---------------------------------------------------------------------------

