$ErrorActionPreference = 'Stop'
$repo = Split-Path $PSScriptRoot -Parent
$enc = [Text.Encoding]::GetEncoding(949)
$source = [IO.File]::ReadAllText((Join-Path $repo 'ModPLC_Bin.cpp'), $enc)
$header = [IO.File]::ReadAllText((Join-Path $repo 'ModPLC_Bin.h'), $enc)
function ExtractFunction([string]$name) {
    $match = [regex]::Match($source, '(?ms)^void __fastcall TPlcBin::' + $name + '\(.*?^\}')
    if (!$match.Success) { throw "Function missing: $name" }
    return $match.Value
}
$constants = ([regex]::Matches($header, '(?m)^const int (?:PC_D_\w+|PC_INDEX_INTERFACE|DEVCODE_D)\s*=\s*[^;]+;') | ForEach-Object { $_.Value }) -join "`n"
$struct = [regex]::Match($header, '(?s)typedef struct\s*\{(?:(?!typedef struct).)*?\}\s*PC_DATA;').Value
if (!$struct) { throw 'PC_DATA missing' }
$prefix = @'
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
'@
$class = @'
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
'@
$tests = @'
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
'@
$outDir = Join-Path $repo 'tmp\PlcSafetyAddress20260910\test'
New-Item -ItemType Directory -Force -Path $outDir | Out-Null
$harness = $prefix + "`n" + $constants + "`n" + $struct + "`n" + $class + "`n" +
    (ExtractFunction 'PC_DataChange') + "`n" + (ExtractFunction 'Timer_PC_WriteMsgTimer') + "`n" +
    (ExtractFunction 'ClientSocket_PCRead') + "`n" + $tests
[IO.File]::WriteAllText((Join-Path $outDir 'test.cpp'),$harness,$enc)
Push-Location $outDir
try {
    & 'C:\Program Files (x86)\Embarcadero\Studio\18.0\bin\bcc32.exe' '-tWC' ('-I'+$repo) '-etest.exe' 'test.cpp'
    if ($LASTEXITCODE -ne 0) { throw 'Harness compilation failed' }
    & .\test.exe
    if ($LASTEXITCODE -ne 0) { throw 'Harness failed' }
} finally { Pop-Location }
