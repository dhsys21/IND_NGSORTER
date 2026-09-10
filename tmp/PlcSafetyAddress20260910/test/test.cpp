#include <string>
#include <cstring>
#include <cassert>
#include <cstdio>
#include "ProductionProtocol.h"
typedef unsigned long DWORD;
struct TObject {};
DWORD nowTickValue=1000;
DWORD GetTickCount(){ return nowTickValue; }
struct TCustomWinSocket {
    std::string sent, incoming;
    bool closed, shortSend;
    TCustomWinSocket():closed(false),shortSend(false){}
    int SendBuf(void* p,int n){sent.assign((char*)p,n);return shortSend?n-1:n;}
    int ReceiveBuf(void* p,int n){int k=incoming.size();if(k>n)k=n;memcpy(p,incoming.data(),k);incoming.erase(0,k);return k;}
    void Close(){closed=true;}
};
struct Client {
    bool Active; TCustomWinSocket* Socket;
    void Close(){Active=false;Socket->Close();}
};
struct MainStub { void memoMainLineAdd(const char*){} } mainStub;
MainStub* MainForm=&mainStub;
const int DEVCODE_D			=	0xA8;
const int PC_INDEX_INTERFACE					=		11;
const int PC_D_INTERFACE_START_DEV_NUM				=	10150;
const int PC_D_INTERFACE_LEN = 12;
const int PC_D_PROCESS_WRITE_LEN = 7;
const int PC_D_SAFETY_WRITE_LEN = 2;
const int PC_D_HEART_BEAT			  				=	0;
const int PC_D_AUTO_MANUAL			     	    	=   1;
const int PC_D_ERROR    	  			    		=   2;
const int PC_D_TRAY_IN_READY                        =   3;
const int PC_D_SOURCE_CENTERING_REQ                 =   4;
const int PC_D_SOURCE_TRAY_OUT 		    			=   5;
const int PC_D_TARGET_TRAY_OUT                      =   6;
const int PC_D_EMERGENCY                            =   10;
const int PC_D_DOOR_OPEN                            =   11;
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
class TPlcBin {
public:
    PC_DATA pc_Data;
    unsigned char pc_Interface_Data[PC_D_INTERFACE_LEN][2];
    Client* ClientSocket_PC;
    bool pc_ReadFlag;
    int pcPendingWriteResponses,pc_ReadCount,pc_index;
    DWORD lastPcHeartBeatTick;
    std::string pcRxBytes;
    TPlcBin(Client* c):ClientSocket_PC(c),pc_ReadFlag(true),pcPendingWriteResponses(0),pc_ReadCount(0),pc_index(PC_INDEX_INTERFACE),lastPcHeartBeatTick(0){memset(&pc_Data,0,sizeof(pc_Data));memset(pc_Interface_Data,0,sizeof(pc_Interface_Data));}
    bool IsPcHeartBeatOn(){return pc_Interface_Data[PC_D_HEART_BEAT][0]!=0;}
    void CmdPcHeartBeat(bool b){pc_Interface_Data[PC_D_HEART_BEAT][0]=b;}
    bool IsSourceTrayOutOn(){return pc_Interface_Data[PC_D_SOURCE_TRAY_OUT][0]!=0;}
    bool IsSourceCenteringRequestOn(){return pc_Interface_Data[PC_D_SOURCE_CENTERING_REQ][0]!=0;}
    void SetDouble(unsigned char (*p)[2],int i,int v){p[i][0]=v;p[i][1]=v>>8;}
    void __fastcall PC_DataChange(int,int,int,int);
    void __fastcall Timer_PC_WriteMsgTimer(TObject*);
    void __fastcall ClientSocket_PCRead(TObject*,TCustomWinSocket*);
};
void __fastcall TPlcBin::PC_DataChange(int subCommand, int address, int devCode, int devLen)
{
	pc_Data.SubCommand[0] = subCommand;
	pc_Data.SubCommand[1] = 0x00;

	pc_Data.ReqDataLen[0] = (0x0c + (devLen * 2)) % 256;
	pc_Data.ReqDataLen[1] = (0x0c + (devLen * 2)) / 256;;

	pc_Data.StartDevNum[0] = address % 256;
	pc_Data.StartDevNum[1] = (address / 256) % 256;
	pc_Data.StartDevNum[2] = address / (256 * 256);
	pc_Data.DevCode = devCode;

	pc_Data.DevLen[0] = devLen % 256;
	pc_Data.DevLen[1] = devLen / 256;
}
void __fastcall TPlcBin::Timer_PC_WriteMsgTimer(TObject *Sender)
{
    if(ClientSocket_PC->Active)
	{
		if(pc_ReadFlag)
		{
			if(pc_index == PC_INDEX_INTERFACE)
			{
				// D10150 PC HEART BEAT: toggle once per second. Keep the 200ms
				// interface transmission cycle so the remaining PLC commands stay responsive.
				DWORD nowTick = GetTickCount();
				if(lastPcHeartBeatTick == 0 ||
					(DWORD)(nowTick - lastPcHeartBeatTick) >= 1000){
					CmdPcHeartBeat(!IsPcHeartBeatOn());
					lastPcHeartBeatTick = nowTick;
				}

				// Last-line safety check immediately before the PLC write frame.
				// D10155 has priority, so an impossible ON/ON buffer is corrected to OFF/ON.
				if(IsSourceTrayOutOn() && IsSourceCenteringRequestOn()){
					SetDouble(pc_Interface_Data, PC_D_SOURCE_CENTERING_REQ, 0);
					if(MainForm != NULL)
						MainForm->memoMainLineAdd("[PLC SAFETY] ON/ON conflict corrected before PC interface transmission.");
				}
                // PLC SAFETY ADDRESS 2026-09-10: two complete MC frames in one
                // send, preserving the 200ms cycle. D10157-D10159 are untouched.
                PC_DataChange(0, PC_D_INTERFACE_START_DEV_NUM, DEVCODE_D, PC_D_PROCESS_WRITE_LEN);
                std::string writes(reinterpret_cast<const char*>(&pc_Data), sizeof(pc_Data));
                writes.append(reinterpret_cast<const char*>(&pc_Interface_Data[0]), PC_D_PROCESS_WRITE_LEN * 2);
                PC_DataChange(0, PC_D_INTERFACE_START_DEV_NUM + PC_D_EMERGENCY, DEVCODE_D, PC_D_SAFETY_WRITE_LEN);
                writes.append(reinterpret_cast<const char*>(&pc_Data), sizeof(pc_Data));
                writes.append(reinterpret_cast<const char*>(&pc_Interface_Data[PC_D_EMERGENCY]), PC_D_SAFETY_WRITE_LEN * 2);
                pcPendingWriteResponses = 2;
                pc_ReadFlag = false;
                if(ClientSocket_PC->Socket->SendBuf(&writes[0], writes.size()) != (int)writes.size()){
                    ClientSocket_PC->Close();
                    return;
                }
			}
		}
		else if(pc_ReadCount > 20) 	//	200ms -> 4초동안 응답확인
		{
			ClientSocket_PC->Close();
		}

		pc_ReadCount++;
	}
	else
	{
		ClientSocket_PC->Close();
	}
}
void __fastcall TPlcBin::ClientSocket_PCRead(TObject *Sender, TCustomWinSocket *Socket)

{
	char bytes[4096];
	int count = Socket->ReceiveBuf(bytes, sizeof(bytes));
	if(count <= 0) return;
	pcRxBytes.append(bytes, count);
	std::string frame;
	int result;
	while((result = TakeMcResponse(pcRxBytes, frame)) == 1){
		if(frame.size() != 11 || frame[9] != 0 || frame[10] != 0){
			Socket->Close(); return;
		}
        // PLC SAFETY ADDRESS: wait for BOTH process and safety write replies.
        if(pcPendingWriteResponses <= 0){ Socket->Close(); return; }
        --pcPendingWriteResponses;
        pc_ReadFlag = (pcPendingWriteResponses == 0);
        if(pc_ReadFlag) pc_ReadCount = 0;
	}
	if(result < 0) Socket->Close();
}
std::string ack(){ const unsigned char a[]={0xd0,0,0,0xff,0xff,3,0,2,0,0,0};return std::string((const char*)a,sizeof(a)); }
void checkFrame(const std::string& data,int offset,int addr,int words){
    PC_DATA h;memcpy(&h,data.data()+offset,sizeof(h));
    assert(h.StartDevNum[0]+256*h.StartDevNum[1]+65536*h.StartDevNum[2]==addr);
    assert(h.DevLen[0]+256*h.DevLen[1]==words);
    assert(h.ReqDataLen[0]+256*h.ReqDataLen[1]==12+2*words);
}
int main(){
    TCustomWinSocket socket;Client client;client.Active=true;client.Socket=&socket;TPlcBin plc(&client);
    assert(sizeof(PC_DATA)==21);
    assert(PC_D_INTERFACE_START_DEV_NUM+PC_D_EMERGENCY==10160);
    assert(PC_D_INTERFACE_START_DEV_NUM+PC_D_DOOR_OPEN==10161);
    plc.SetDouble(plc.pc_Interface_Data,PC_D_EMERGENCY,1);
    plc.SetDouble(plc.pc_Interface_Data,PC_D_DOOR_OPEN,1);
    plc.SetDouble(plc.pc_Interface_Data,PC_D_SOURCE_CENTERING_REQ,1);
    plc.SetDouble(plc.pc_Interface_Data,PC_D_SOURCE_TRAY_OUT,1);
    plc.Timer_PC_WriteMsgTimer(0);
    assert(socket.sent.size()==60);
    checkFrame(socket.sent,0,10150,7);checkFrame(socket.sent,35,10160,2);
    assert(socket.sent[29]==0 && socket.sent[31]==1); // centering/out exclusion
    assert(socket.sent[56]==1 && socket.sent[58]==1);
    assert(!plc.pc_ReadFlag && plc.pcPendingWriteResponses==2);
    socket.incoming=ack();plc.ClientSocket_PCRead(0,&socket);
    assert(!plc.pc_ReadFlag && plc.pcPendingWriteResponses==1);
    socket.incoming=ack().substr(0,5);plc.ClientSocket_PCRead(0,&socket);
    assert(!plc.pc_ReadFlag);
    socket.incoming=ack().substr(5);plc.ClientSocket_PCRead(0,&socket);
    assert(plc.pc_ReadFlag && !socket.closed);
    plc.SetDouble(plc.pc_Interface_Data,PC_D_EMERGENCY,0);
    plc.SetDouble(plc.pc_Interface_Data,PC_D_DOOR_OPEN,0);
    nowTickValue=1200;plc.Timer_PC_WriteMsgTimer(0);
    assert(socket.sent[56]==0 && socket.sent[58]==0 && plc.IsPcHeartBeatOn());
    socket.incoming=ack()+ack();plc.ClientSocket_PCRead(0,&socket);
    assert(plc.pc_ReadFlag && !socket.closed);
    nowTickValue=2000;plc.Timer_PC_WriteMsgTimer(0);assert(!plc.IsPcHeartBeatOn());
    socket.incoming=ack();socket.incoming[9]=1;plc.ClientSocket_PCRead(0,&socket);
    assert(socket.closed); // MC error must not count as success
    socket.closed=false;plc.pc_ReadFlag=false;plc.pc_ReadCount=21;
    plc.Timer_PC_WriteMsgTimer(0);assert(socket.closed); // missing second ACK timeout
    client.Active=true;socket.closed=false;socket.shortSend=true;plc.pc_ReadFlag=true;
    plc.Timer_PC_WriteMsgTimer(0);assert(socket.closed);
    puts("PASS: addresses, packet bounds/gap, EMG ON/OFF, mutual exclusion, heartbeat, split/coalesced ACKs, error/timeout/short-send");
}