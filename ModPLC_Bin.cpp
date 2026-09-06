//---------------------------------------------------------------------------


#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
#include "ProductionProtocol.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma classgroup "Vcl.Controls.TControl"
#pragma resource "*.dfm"
TPlcBin *PlcBin;
//---------------------------------------------------------------------------
__fastcall TPlcBin::TPlcBin(TComponent* Owner)
	: TDataModule(Owner)
{
    // PLC
	plc_Data.SubHeader[0] = 0x50;
	plc_Data.SubHeader[1] = 0x00;
	plc_Data.NetNum = 0x00;
	plc_Data.PlcNum = 0xff;
	plc_Data.ReqIONum[0] = 0xff;
	plc_Data.ReqIONum[1] = 0x03;
	plc_Data.ReqOfficeNum = 0x00;
	plc_Data.ReqDataLen[0] = 0x0c;
	plc_Data.ReqDataLen[1] = 0x00;
	plc_Data.CpuTime[0] = 0x10;
	plc_Data.CpuTime[1] = 0x00;
	plc_Data.Command[0] = 0x01;
	plc_Data.Command[1] = 0x04;

	plc_index = PLC_INDEX_INTERFACE;
	lastPlcStatusTick = 0;

	// PC
	pc_Data.SubHeader[0] = 0x50;
	pc_Data.SubHeader[1] = 0x00;
	pc_Data.NetNum = 0x00;
	pc_Data.PlcNum = 0xff;
	pc_Data.ReqIONum[0] = 0xff;
	pc_Data.ReqIONum[1] = 0x03;
	pc_Data.ReqOfficeNum = 0x00;

	pc_Data.CpuTime[0] = 0x10;
	pc_Data.CpuTime[1] = 0x00;
	pc_Data.Command[0] = 0x01;
	pc_Data.Command[1] = 0x14;

	pc_index = PC_INDEX_INTERFACE;
	lastPcHeartBeatTick = 0;
	sourceTrayOutInterlockActive = false;

    // Init
    memset(plc_Interface_Data, 0, sizeof(unsigned char) * PLC_D_INTERFACE_LEN * 2);
	memset(pc_Interface_Data, 0, sizeof(unsigned char) * PC_D_INTERFACE_LEN * 2);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//	접속 & 해제 Start
//---------------------------------------------------------------------------
void __fastcall TPlcBin::Connect(AnsiString ip, int port1, int port2)
{
	try
	{
		bClose = false;
		Timer_PLC_AutoConnect->Enabled = false;
		Timer_PC_AutoConnect->Enabled = false;

		ClientSocket_PLC->Address = ip;
		ClientSocket_PLC->Port = port1;
		if(!ClientSocket_PLC->Active)
			ClientSocket_PLC->Open();

		ClientSocket_PC->Address = ip;
		ClientSocket_PC->Port = port2;
		if(!ClientSocket_PC->Active)
			ClientSocket_PC->Open();
	}
	catch(...)
	{
		ShowMessage("PLC -> PC : 통신 설정 실패.");
	}
}
//---------------------------------------------------------------------------
void __fastcall TPlcBin::DisConnect()
{
	bClose = true;
	// Prevent reconnect callbacks while the application is shutting down.
	Timer_PLC_AutoConnect->Enabled = false;
	Timer_PC_AutoConnect->Enabled = false;
	Timer_PLC_WriteMsg->Enabled = false;
	Timer_PC_WriteMsg->Enabled = false;
	ClientSocket_PLC->Close();
	ClientSocket_PC->Close();
}
//---------------------------------------------------------------------------
void __fastcall TPlcBin::Timer_PC_AutoConnectTimer(TObject *Sender)
{
    ClientSocket_PC->Active = true;
	Timer_PC_AutoConnect->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TPlcBin::Timer_PLC_AutoConnectTimer(TObject *Sender)
{
    ClientSocket_PLC->Active = true;
	Timer_PLC_AutoConnect->Enabled = false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//	접속 & 해제 End
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//  Client Socket PLC
//---------------------------------------------------------------------------
void __fastcall TPlcBin::PLC_Initialization()
{
	plcRxBytes.clear();
	plc_Read = "";
	plc_ReadCount = 0;
	plc_ReadFlag = true;
	lastPlcStatusTick = 0;

	Timer_PLC_WriteMsg->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TPlcBin::ClientSocket_PLCConnect(TObject *Sender, TCustomWinSocket *Socket)

{
    PLC_Initialization();
}
//---------------------------------------------------------------------------

void __fastcall TPlcBin::ClientSocket_PLCDisconnect(TObject *Sender, TCustomWinSocket *Socket)

{
	lastPlcStatusTick = 0;
	if(!bClose) Timer_PLC_AutoConnect->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TPlcBin::ClientSocket_PLCError(TObject *Sender, TCustomWinSocket *Socket,
          TErrorEvent ErrorEvent, int &ErrorCode)
{
    ErrorCode = 0;
	Socket->Close();
}
//---------------------------------------------------------------------------

void __fastcall TPlcBin::ClientSocket_PLCRead(TObject *Sender, TCustomWinSocket *Socket)

{
	char bytes[4096];
	int count = Socket->ReceiveBuf(bytes, sizeof(bytes));
	if(count <= 0) return;
	plcRxBytes.append(bytes, count);
	std::string frame;
	int result;
	while((result = TakeMcResponse(plcRxBytes, frame)) == 1){
		if(frame[9] != 0 || frame[10] != 0 || frame.size() != 11 + PLC_D_INTERFACE_LEN * 2){
			lastPlcStatusTick = 0;
			Socket->Close();
			return;
		}
		plc_Read = "";
		for(unsigned i = 0; i < frame.size(); ++i)
			plc_Read += IntToHex((unsigned char)frame[i], 2);
		if(plc_index == PLC_INDEX_INTERFACE) PLC_Recv_Interface();
		plc_ReadCount = 0;
		plc_ReadFlag = true;
	}
	if(result < 0){ lastPlcStatusTick = 0; Socket->Close(); }
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//  Client Socket PC
//---------------------------------------------------------------------------
void __fastcall TPlcBin::PC_Initialization()
{
	pcRxBytes.clear();
	pc_Read = "";
	pc_ReadFlag = true;
	pc_ReadCount = 0;
	lastPcHeartBeatTick = 0;

	Timer_PC_WriteMsg->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TPlcBin::ClientSocket_PCConnect(TObject *Sender, TCustomWinSocket *Socket)

{
    PC_Initialization();
}
//---------------------------------------------------------------------------

void __fastcall TPlcBin::ClientSocket_PCDisconnect(TObject *Sender, TCustomWinSocket *Socket)

{
	if(!bClose) Timer_PC_AutoConnect->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TPlcBin::ClientSocket_PCError(TObject *Sender, TCustomWinSocket *Socket,
          TErrorEvent ErrorEvent, int &ErrorCode)
{
    ErrorCode = 0;
	Socket->Close();
}
//---------------------------------------------------------------------------

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
		pc_ReadFlag = true;
		pc_ReadCount = 0;
	}
	if(result < 0) Socket->Close();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//	Write
//---------------------------------------------------------------------------


void __fastcall TPlcBin::Timer_PC_WriteMsgTimer(TObject *Sender)
{
    if(ClientSocket_PC->Active)
	{
		if(pc_ReadFlag)
		{
			if(pc_index == PC_INDEX_INTERFACE)
			{
				PC_DataChange(0, PC_D_INTERFACE_START_DEV_NUM, DEVCODE_D, PC_D_INTERFACE_LEN);

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
                ClientSocket_PC->Socket->SendBuf(&pc_Data, sizeof(pc_Data));        // should comment for emulator
				ClientSocket_PC->Socket->SendBuf(&pc_Interface_Data, sizeof(pc_Interface_Data));

				pc_ReadFlag = false;
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
//---------------------------------------------------------------------------

void __fastcall TPlcBin::Timer_PLC_WriteMsgTimer(TObject *Sender)
{
    if(ClientSocket_PLC->Active)
	{
		if(plc_ReadFlag)
		{
			if(plc_index == PLC_INDEX_INTERFACE)
			{
				PLC_DataChange(0, PLC_D_INTERFACE_START_DEV_NUM, DEVCODE_D, PLC_D_INTERFACE_LEN);
				ClientSocket_PLC->Socket->SendBuf(&plc_Data, sizeof(plc_Data));
				plc_ReadFlag = false;
			}
		}
		else if(plc_ReadCount > 20)		//	200ms -> 4초동안 응답확인
        {
			ClientSocket_PLC->Close();
        }

		plc_ReadCount++;
	}
	else ClientSocket_PLC->Close();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//	데이터 변경
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
void __fastcall TPlcBin::PLC_DataChange(int subCommand, int address, int devCode, int devLen)
{
	plc_Data.SubCommand[0] = subCommand;
	plc_Data.SubCommand[1] = 0x00;

	plc_Data.StartDevNum[0] = address % 256;
	plc_Data.StartDevNum[1] = (address / 256) % 256;
	plc_Data.StartDevNum[2] = address / (256 * 256);

	plc_Data.DevCode = devCode;

	plc_Data.DevLen[0] = devLen % 256;
	plc_Data.DevLen[1] = devLen / 256;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//  변수(배열)에 PLC에서 읽어온 값 할당
//---------------------------------------------------------------------------
void __fastcall TPlcBin::PLC_Recv_Interface()
{
	int num = 0;
	//plc_Interface = plc_Read.SubString(23, PLC_D_INTERFACE_LEN);

	for(int i = 0; i < PLC_D_INTERFACE_LEN; i++)
	{
		plc_Interface_Data[i][0] = StrToInt("0x" + plc_Read.SubString(23 + num, 2));
		plc_Interface_Data[i][1] = StrToInt("0x" + plc_Read.SubString(23 + num + 2, 2));
		num += 4;
	}
	lastPlcStatusTick = GetTickCount();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//	데이터 쓰기 & 읽기
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TPlcBin::SetData(unsigned char (*data)[2], int column, int num, bool flag)
{
	int num1 = num / 8;
	int num2 = num % 8;

	if(flag) data[column][num1] |= (1 << num2);
	else data[column][num1] &= ~(1 << num2);
}
//---------------------------------------------------------------------------
void __fastcall TPlcBin::SetDouble(unsigned char (*data)[2], int column, double value)
{
    short temp = static_cast<short>(value); // signed 16-bit 정수로 형변환

    data[column][0] = temp & 0xFF;         // LSB (저장 순서에 따라 다름)
    data[column][1] = (temp >> 8) & 0xFF;  // MSB
}
//---------------------------------------------------------------------------
void __fastcall TPlcBin::SetString(unsigned char (*data)[2], int column, AnsiString msg)
{
	if(msg.Length() % 2) msg += (char)0x0;

	int num = 0;
	for(int i = 0; i < msg.Length() / 2; i++)
	{
		data[column + i][0] = msg[1 + num];
		data[column + i][1] = msg[2 + num];
		num += 2;
	}
}
//---------------------------------------------------------------------------
int __fastcall TPlcBin::GetData(unsigned char (*data)[2], int column, int num)
{
	bool value = false;
	int num1 = num / 8;
	int num2 = num % 8;

	value = data[column][num1] & (1 << num2);

	return value;
}
//---------------------------------------------------------------------------
double __fastcall TPlcBin::GetDouble(unsigned char (*data)[2], int column)
{
	double value = -1;

	value = (data[column][1] * 256) + data[column][0];

	return value;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TPlcBin::GetString(unsigned char (*data)[2], int column, int count)
{
	AnsiString m_GetStringValue = "";
	for(int i = 0; i < count; i++)
	{
		m_GetStringValue += (char)data[column + i][0];
		m_GetStringValue += (char)data[column + i][1];
	}

	return m_GetStringValue.Trim();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//	데이터 쓰기 & 읽기
//---------------------------------------------------------------------------
double __fastcall TPlcBin::GetPlcValue(int plc_address)
{
    double value = GetDouble(plc_Interface_Data, plc_address);
    return value;
}
//---------------------------------------------------------------------------
int __fastcall TPlcBin::GetPlcData(int plc_address, int bit_num)
{
    int value = GetData(plc_Interface_Data, plc_address, bit_num);
    return value;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TPlcBin::GetPlcValue(int plc_address, int size)
{
    AnsiString value = GetString(plc_Interface_Data, plc_address, size);
    return value;
}
//---------------------------------------------------------------------------
double __fastcall TPlcBin::GetPcValue(int pc_address)
{
    double value = GetDouble(pc_Interface_Data, pc_address);
    return value;
}
//---------------------------------------------------------------------------
void __fastcall TPlcBin::SetPcData(int pc_address, int bit_num, bool bValue)
{
    SetData(pc_Interface_Data, pc_address, bit_num, bValue);
}
//---------------------------------------------------------------------------
void __fastcall TPlcBin::SetPcValue(int pc_address, int value)
{
	// FINAL D10154/D10155 MUTUAL EXCLUSION:
	// Source Tray Out has priority. This also protects direct FormInterface
	// writes which do not pass through the normal command wrapper functions.
	if(pc_address == PC_D_SOURCE_TRAY_OUT && value != 0){
		sourceTrayOutInterlockActive = true;
		if((int)GetPcValue(PC_D_SOURCE_CENTERING_REQ) != 0){
			SetDouble(pc_Interface_Data, PC_D_SOURCE_CENTERING_REQ, 0);
			if(MainForm != NULL)
				MainForm->memoMainLineAdd("[PLC SAFETY] D10155 ON forced D10154 OFF before transmission.");
		}
	}else if(pc_address == PC_D_SOURCE_CENTERING_REQ && value != 0 &&
		(sourceTrayOutInterlockActive || IsSourceTrayOutOn())){
		value = 0;
		if(MainForm != NULL)
			MainForm->memoMainLineAdd("[PLC SAFETY] D10154 ON blocked while Source Tray Out is active.");
	}

    if((int)GetPcValue(pc_address) == value)
        return;

    SetDouble(pc_Interface_Data, pc_address, value);
}
//---------------------------------------------------------------------------
bool __fastcall TPlcBin::IsPlcHeartBeatOn()
{
    return GetPlcValue(PLC_D_HEART_BEAT) != 0;
}
//---------------------------------------------------------------------------
bool __fastcall TPlcBin::IsPlcAutoMode()
{
    return GetPlcValue(PLC_D_AUTO_MANUAL) != 0;
}
//---------------------------------------------------------------------------
bool __fastcall TPlcBin::IsPlcError()             { return GetPlcValue(PLC_D_ERROR) != 0; }
bool __fastcall TPlcBin::IsSourceTrayIn()         { return GetPlcValue(PLC_D_SOURCE_TRAY_IN) != 0; }
bool __fastcall TPlcBin::IsSourceCentering()      { return GetPlcValue(PLC_D_SOURCE_CENTERING) != 0; }
bool __fastcall TPlcBin::IsTargetTrayIn()         { return GetPlcValue(PLC_D_TARGET_TRAY_IN) != 0; }
bool __fastcall TPlcBin::IsTargetCentering()      { return GetPlcValue(PLC_D_TARGET_CENTERING) != 0; }
bool __fastcall TPlcBin::IsPlcStatusFresh(DWORD maxAgeMs)
{
	return ClientSocket_PLC != NULL && ClientSocket_PLC->Active &&
		lastPlcStatusTick != 0 &&
		(DWORD)(GetTickCount() - lastPlcStatusTick) <= maxAgeMs;
}
//---------------------------------------------------------------------------
bool __fastcall TPlcBin::IsPcHeartBeatOn()
{
    return GetPcValue(PC_D_HEART_BEAT) != 0;
}
//---------------------------------------------------------------------------
bool __fastcall TPlcBin::IsPcAutoMode()
{
    return GetPcValue(PC_D_AUTO_MANUAL) != 0;
}
//---------------------------------------------------------------------------
bool __fastcall TPlcBin::IsPcErrorOn()                    { return GetPcValue(PC_D_ERROR) != 0; }
bool __fastcall TPlcBin::IsTrayInReadyOn()                { return GetPcValue(PC_D_TRAY_IN_READY) != 0; }
bool __fastcall TPlcBin::IsSourceCenteringRequestOn()     { return GetPcValue(PC_D_SOURCE_CENTERING_REQ) != 0; }
bool __fastcall TPlcBin::IsSourceTrayOutOn()              { return GetPcValue(PC_D_SOURCE_TRAY_OUT) != 0; }
bool __fastcall TPlcBin::IsTargetTrayOutOn()              { return GetPcValue(PC_D_TARGET_TRAY_OUT) != 0; }
bool __fastcall TPlcBin::IsPcEmergencyOn()                { return GetPcValue(PC_D_EMERGENCY) != 0; }
bool __fastcall TPlcBin::IsPcDoorOpenOn()                 { return GetPcValue(PC_D_DOOR_OPEN) != 0; }
//---------------------------------------------------------------------------
void __fastcall TPlcBin::CmdPcHeartBeat(bool bOn)
{
    SetPcValue(PC_D_HEART_BEAT, bOn ? 1 : 0);
}
//---------------------------------------------------------------------------
void __fastcall TPlcBin::CmdPcAutoMode(bool bAuto)
{
    SetPcValue(PC_D_AUTO_MANUAL, bAuto ? 1 : 0);
}
//---------------------------------------------------------------------------
void __fastcall TPlcBin::CmdPcError(bool bOn)                  { SetPcValue(PC_D_ERROR, bOn ? 1 : 0); }
void __fastcall TPlcBin::CmdTrayInReady(bool bOn)              { SetPcValue(PC_D_TRAY_IN_READY, bOn ? 1 : 0); }
void __fastcall TPlcBin::PrepareSourceTrayOut()
{
	// Latch the safety interlock before the timer delay begins. This blocks
	// every D10154 ON call path, including FMS and mode-change code.
	sourceTrayOutInterlockActive = true;
	SetPcValue(PC_D_SOURCE_CENTERING_REQ, 0);
}
void __fastcall TPlcBin::CmdSourceCenteringRequest(bool bOn)
{
	// D10154 must remain OFF throughout preparation and while D10155 is ON.
	if(bOn && (sourceTrayOutInterlockActive || IsSourceTrayOutOn())){
		SetPcValue(PC_D_SOURCE_CENTERING_REQ, 0);
		return;
	}
	SetPcValue(PC_D_SOURCE_CENTERING_REQ, bOn ? 1 : 0);
}
void __fastcall TPlcBin::CmdSourceTrayOut(bool bOn)
{
	// Clear centering first so the same PC write frame never contains
	// D10154=ON together with D10155=ON.
	if(bOn){
		sourceTrayOutInterlockActive = true;
		SetPcValue(PC_D_SOURCE_CENTERING_REQ, 0);
	}else{
		sourceTrayOutInterlockActive = false;
	}
	SetPcValue(PC_D_SOURCE_TRAY_OUT, bOn ? 1 : 0);
}
void __fastcall TPlcBin::CmdTargetTrayOut(bool bOn)            { SetPcValue(PC_D_TARGET_TRAY_OUT, bOn ? 1 : 0); }
void __fastcall TPlcBin::CmdPcEmergency(bool bOn)              { SetPcValue(PC_D_EMERGENCY, bOn ? 1 : 0); }
void __fastcall TPlcBin::CmdPcDoorOpen(bool bOn)               { SetPcValue(PC_D_DOOR_OPEN, bOn ? 1 : 0); }
//---------------------------------------------------------------------------
