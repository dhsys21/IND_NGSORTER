//---------------------------------------------------------------------------


#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
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
	plc_Data.NetNum = 0x03;
	plc_Data.PlcNum = 0x01;
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

	// PC
	pc_Data.SubHeader[0] = 0x50;
	pc_Data.SubHeader[1] = 0x00;
	pc_Data.NetNum = 0x03;
	pc_Data.PlcNum = 0x01;
	pc_Data.ReqIONum[0] = 0xff;
	pc_Data.ReqIONum[1] = 0x03;
	pc_Data.ReqOfficeNum = 0x00;

	pc_Data.CpuTime[0] = 0x10;
	pc_Data.CpuTime[1] = 0x00;
	pc_Data.Command[0] = 0x01;
	pc_Data.Command[1] = 0x14;

	pc_index = PC_INDEX_INTERFACE;

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
	plc_Read = "";
	plc_ReadCount = 0;
	plc_ReadFlag = true;

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
    int length = Socket->ReceiveLength();
	plc_Read_Temp = Socket->ReceiveText();

	for(int i = 0; i < length; i++)
		plc_Read += IntToHex((unsigned char)plc_Read_Temp[i + 1], 2);
   //	TotalForm->Memo1->Lines->Add(plc_Read);
	while(!plc_Read.IsEmpty() && (plc_Read.Length() >= 54) && (plc_Read.Pos("D000")))
	{
		int index = plc_Read.Pos("D000");

		if(index != 1) plc_Read.Delete(1, index - 1);		//	헤더 위치 인지 확인

		if(plc_Read.SubString(19, 4) == "0000")		// 종료 코드 확인(에러)
		{
			int length = 18 + StrToInt("0x" + plc_Read.SubString(15, 2))
						+ (StrToInt("0x" + plc_Read.SubString(17, 2)) * 256);		//	데이터 길이 확인

			if(plc_Read.Length() >= length)
			{
				if(plc_index == PLC_INDEX_INTERFACE) PLC_Recv_Interface();
				else plc_index = PLC_INDEX_INTERFACE;
			}
			else break;
		}
		plc_Read = "";
		break;
	}

	plc_ReadCount = 0;
	plc_ReadFlag = true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//  Client Socket PC
//---------------------------------------------------------------------------
void __fastcall TPlcBin::PC_Initialization()
{
	pc_Read = "";
	pc_ReadFlag = true;
	pc_ReadCount = 0;

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
    int length = Socket->ReceiveLength();
	pc_Read_Temp = Socket->ReceiveText();

	for(int i = 0; i < length; i++)
		pc_Read += IntToHex((unsigned char)pc_Read_Temp[i + 1], 2);

	while(!pc_Read.IsEmpty() && (pc_Read.Length() >= 22) && (pc_Read.Pos("D000")))
	{
		int index = pc_Read.Pos("D000");

		if(index == 1)
		{
			if(pc_Read.SubString(19, 4) == "0000")	//	무조껀 읽어야 함, 그래서 생략
			{
				pc_ReadFlag = true;
				pc_ReadCount = 0;
			}
			pc_Read = "";
			break;
		}
		else pc_Read.Delete(1, index - 1);
	}
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//	Write
//---------------------------------------------------------------------------


void __fastcall TPlcBin::Timer_PC_WriteMsgTimer(TObject *Sender)
{
    bool flag = false;
    if(ClientSocket_PC->Active)
	{
		if(pc_ReadFlag)
		{
			if(pc_index == PC_INDEX_INTERFACE)
			{
				PC_DataChange(0, PC_D_INTERFACE_START_DEV_NUM, DEVCODE_D, PC_D_INTERFACE_LEN);

				if(MainForm->Client->Active)
				{
					bool flag = GetData(pc_Interface_Data, PC_D_HEART_BEAT, 0);
					SetDouble(pc_Interface_Data, PC_D_HEART_BEAT, (int)!flag);
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
    SetDouble(pc_Interface_Data, pc_address, value);
}
//---------------------------------------------------------------------------
