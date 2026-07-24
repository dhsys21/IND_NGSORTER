//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma classgroup "Vcl.Controls.TControl"
#pragma link "CPort"
#pragma resource "*.dfm"
TSmokeDetector *SmokeDetector;
//---------------------------------------------------------------------------
__fastcall TSmokeDetector::TSmokeDetector(TComponent* Owner)
	: TDataModule(Owner)
{
	retryCnt = 3;
	Tag = 0;
	slaveId = 1;
	protocolMode = 0;
	bWaitingResponse = false;
	failCount = 0;
	SmokeOutput = false;
	DangerOutput = false;
	WarningOutput = false;
	RunOutput = false;
	SmokeAlarm = false;
	DangerAlarm = false;
	WarningAlarm = false;
	TemperaturePV = 0;
	TemperatureOffset = 0;
	TemperatureWarningSV = 0;
	TemperatureDangerSV = 0;
}
//---------------------------------------------------------------------------
unsigned short __fastcall TSmokeDetector::get_crc16(unsigned char *pBuf, int nLen)
{
	unsigned short crc = 0xFFFF;
	for(int i = 0; i < nLen; i++) {
		crc ^= pBuf[i];
		for(int j = 0; j < 8; j++) {
			if(crc & 0x01) {
				crc >>= 1;
				crc ^= 0xA001;
			}
			else {
				crc >>= 1;
			}
		}
	}
	return crc;
}
//---------------------------------------------------------------------------
void __fastcall TSmokeDetector::CommOpen(AnsiString port, int sep, int id, int mode, int baudRate)
{
	try {
		if(Comm->Connected) Comm->Close();

		Tag = sep;
		slaveId = id;
		protocolMode = mode;
		failCount = 0;
		bWaitingResponse = false;
		rxBuffer.clear();

		Comm->Port = port;
		if(baudRate <= 0) baudRate = (protocolMode == 0) ? 115200 : 9600;

		switch(baudRate) {
			case 9600: Comm->BaudRate = br9600; break;
			case 19200: Comm->BaudRate = br19200; break;
			case 38400: Comm->BaudRate = br38400; break;
			case 57600: Comm->BaudRate = br57600; break;
			case 115200: Comm->BaudRate = br115200; break;
			default:
				Comm->BaudRate = brCustom;
				Comm->CustomBaudRate = baudRate;
				break;
		}

		Comm->FlowControl->ControlDTR = dtrEnable;
		Comm->FlowControl->ControlRTS = rtsEnable;
		Comm->EventChar = 0x00;
		Comm->Open();
		Comm->ClearBuffer(true, true);

		chkTimer->Interval = 1000;
		chkTimer->Enabled = true;
		if(MainForm != NULL)
			MainForm->memoMainLineAdd("Smoke detector connected: " + port);
	}
	catch(...) {
		Comm->Close();
		chkTimer->Enabled = false;
		if(AlarmForm != NULL)
			AlarmForm->ShowError("TSD-V50 COM Port", "Can not open " + port + " port.");
		else if(MainForm != NULL)
			MainForm->memoMainLineAdd("Smoke detector open fail: " + port);
	}
}
//---------------------------------------------------------------------------
void __fastcall TSmokeDetector::CommClose()
{
	chkTimer->Enabled = false;
	if(Comm->Connected) Comm->Close();
}
//---------------------------------------------------------------------------
void __fastcall TSmokeDetector::Reconnect()
{
	try {
		if(Comm->Connected) Comm->Close();
		Sleep(500);
		Comm->Open();
		Comm->ClearBuffer(true, true);
		failCount = 0;
		bWaitingResponse = false;
		if(MainForm != NULL)
			MainForm->memoMainLineAdd("Smoke detector reconnected.");
	}
	catch(...) {
		if(MainForm != NULL)
			MainForm->memoMainLineAdd("Smoke detector reconnect failed.");
	}
}
//---------------------------------------------------------------------------
void __fastcall TSmokeDetector::ClearAlarm()
{
	setTsdData(0x1005, 0x1234);
}
//---------------------------------------------------------------------------
void __fastcall TSmokeDetector::setTsdData(short regAddr, short writeValue)
{
	unsigned char txBuf[8];
	txBuf[0] = (unsigned char)slaveId;
	txBuf[1] = 0x06;
	txBuf[2] = (unsigned char)((regAddr >> 8) & 0xFF);
	txBuf[3] = (unsigned char)(regAddr & 0xFF);
	txBuf[4] = (unsigned char)((writeValue >> 8) & 0xFF);
	txBuf[5] = (unsigned char)(writeValue & 0xFF);

	unsigned short crc = get_crc16(txBuf, 6);
	txBuf[6] = (unsigned char)(crc & 0xFF);
	txBuf[7] = (unsigned char)((crc >> 8) & 0xFF);
	Comm->Write((void*)txBuf, 8);
}
//---------------------------------------------------------------------------
void __fastcall TSmokeDetector::GetTsdData()
{
	if(!Comm->Connected) return;
	if(protocolMode == 0) GetTsdData_Modbus();
	else GetTsdData_HumanAuto();
}
//---------------------------------------------------------------------------
void __fastcall TSmokeDetector::GetTsdData_Modbus()
{
	unsigned char txBuf[8];
	txBuf[0] = (unsigned char)slaveId;
	txBuf[1] = 0x03;
	txBuf[2] = 0x10;
	txBuf[3] = 0x00;
	txBuf[4] = 0x00;
	txBuf[5] = 0x05;

	unsigned short crc = get_crc16(txBuf, 6);
	txBuf[6] = (unsigned char)(crc & 0xFF);
	txBuf[7] = (unsigned char)((crc >> 8) & 0xFF);
	Comm->Write((void*)txBuf, 8);
}
//---------------------------------------------------------------------------
void __fastcall TSmokeDetector::GetTsdData_HumanAuto()
{
	unsigned char txBuf[7];
	txBuf[0] = 0x02;
	txBuf[1] = (unsigned char)(slaveId + 0x40);
	txBuf[2] = 2;
	txBuf[3] = 0x41;
	txBuf[4] = 0x00;

	unsigned char sum = 0;
	for(int i = 1; i <= 4; i++) sum += txBuf[i];
	txBuf[5] = sum;
	txBuf[6] = 0x03;
	Comm->Write((void*)txBuf, 7);
}
//---------------------------------------------------------------------------
void __fastcall TSmokeDetector::CommRxFlag(TObject *Sender)
{
	unsigned char data[256];
	int cnt = Comm->InputCount();
	if(cnt <= 0) return;
	if(cnt > 256) cnt = 256;

	Comm->Read(data, cnt);
	if(protocolMode != 0) {
		Parse_HumanAuto(data, cnt);
		return;
	}

	for(int i = 0; i < cnt; i++) rxBuffer.push_back(data[i]);

	while(rxBuffer.size() >= 3) {
		unsigned char expectedSlave = (unsigned char)slaveId;
		if(rxBuffer[0] != expectedSlave) {
			rxBuffer.erase(rxBuffer.begin());
			continue;
		}

		unsigned char funcCode = rxBuffer[1];
		int expectedLen = 0;
		if(funcCode == 0x03) {
			if(rxBuffer[2] != 0x0A) {
				rxBuffer.erase(rxBuffer.begin());
				continue;
			}
			expectedLen = 5 + rxBuffer[2];
		}
		else if(funcCode == 0x06 || funcCode == 0x10) expectedLen = 8;
		else if((funcCode & 0x80) == 0x80) expectedLen = 5;
		else {
			rxBuffer.erase(rxBuffer.begin());
			continue;
		}

		if((int)rxBuffer.size() < expectedLen) break;

		unsigned short recvCRC = rxBuffer[expectedLen - 2] | (rxBuffer[expectedLen - 1] << 8);
		if(get_crc16(&rxBuffer[0], expectedLen - 2) != recvCRC) {
			rxBuffer.erase(rxBuffer.begin());
			continue;
		}

		Parse_Modbus(&rxBuffer[0], expectedLen);
		rxBuffer.erase(rxBuffer.begin(), rxBuffer.begin() + expectedLen);
	}
}
//---------------------------------------------------------------------------
void __fastcall TSmokeDetector::Parse_Modbus(unsigned char* data, int len)
{
	if(len < 5 || data[0] != slaveId) return;

	if(data[1] == 0x03 && len >= 15) {
		bWaitingResponse = false;
		failCount = 0;

		unsigned short status = (data[3] << 8) | data[4];
		short rawTemp = (data[5] << 8) | data[6];
		short rawOffset = (data[7] << 8) | data[8];
		unsigned short rawWarning = (data[9] << 8) | data[10];
		unsigned short rawDanger = (data[11] << 8) | data[12];

		UpdateState(
			(status & (1 << 7)) != 0,
			(status & (1 << 6)) != 0,
			(status & (1 << 5)) != 0,
			(status & (1 << 4)) != 0,
			(status & (1 << 2)) != 0,
			(status & (1 << 1)) != 0,
			(status & (1 << 0)) != 0,
			rawTemp / 10.0,
			rawOffset / 10.0,
			rawWarning / 10.0,
			rawDanger / 10.0);
	}
	else if(data[1] == 0x06) {
		bWaitingResponse = false;
		failCount = 0;
	}
}
//---------------------------------------------------------------------------
void __fastcall TSmokeDetector::Parse_HumanAuto(unsigned char* data, int len)
{
	if(len < 7) return;

	unsigned char expectedAddr = (unsigned char)(slaveId + 0x40);
	for(int pos = 0; pos <= len - 7; pos++) {
		if(data[pos] != 0x02 || data[pos + 1] != expectedAddr) continue;

		int dataLen = data[pos + 2];
		int packetLen = dataLen + 5;
		if(pos + packetLen > len) return;

		unsigned char sum = 0;
		for(int i = pos + 1; i < pos + packetLen - 2; i++) sum += data[i];
		if(sum != data[pos + packetLen - 2] || data[pos + packetLen - 1] != 0x03) return;

		if(dataLen >= 4) {
			short rawTemp = (data[pos + 4] << 8) | data[pos + 5];
			TemperaturePV = rawTemp / 10.0;
			bWaitingResponse = false;
			failCount = 0;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TSmokeDetector::UpdateState(bool outSmoke, bool outDanger, bool outWarning,
	bool outRun, bool alarmSmoke, bool alarmDanger, bool alarmWarning,
	double temperature, double offset, double warningSv, double dangerSv)
{
	bool stateChanged =
		SmokeOutput != outSmoke ||
		DangerOutput != outDanger ||
		WarningOutput != outWarning ||
		RunOutput != outRun ||
		SmokeAlarm != alarmSmoke ||
		DangerAlarm != alarmDanger ||
		WarningAlarm != alarmWarning;

	SmokeOutput = outSmoke;
	DangerOutput = outDanger;
	WarningOutput = outWarning;
	RunOutput = outRun;
	SmokeAlarm = alarmSmoke;
	DangerAlarm = alarmDanger;
	WarningAlarm = alarmWarning;
	TemperaturePV = temperature;
	TemperatureOffset = offset;
	TemperatureWarningSV = warningSv;
	TemperatureDangerSV = dangerSv;

	if(BaseForm != NULL && BaseForm->pnlTempPV != NULL) {
		BaseForm->pnlStatusSmoke->Caption->Text = UnicodeString(outSmoke ? "ON" : "OFF");
		BaseForm->pnlStatusDanger->Caption->Text = UnicodeString(outDanger ? "ON" : "OFF");
		BaseForm->pnlStatusWarning->Caption->Text = UnicodeString(outWarning ? "ON" : "OFF");
		BaseForm->pnlStatusRun->Caption->Text = UnicodeString(outRun ? "ON" : "OFF");

		BaseForm->pnlAlarmReserve->Caption->Text = "-";
		BaseForm->pnlAlarmSmoke->Caption->Text = UnicodeString(alarmSmoke ? "ON" : "OFF");
		BaseForm->pnlAlarmDanger->Caption->Text = UnicodeString(alarmDanger ? "ON" : "OFF");
		BaseForm->pnlAlarmWarning->Caption->Text = UnicodeString(alarmWarning ? "ON" : "OFF");

		BaseForm->pnlTempPV->Caption->Text = FormatFloat("0.0", TemperaturePV);
		BaseForm->pnlTempOffset->Caption->Text = FormatFloat("0.0", TemperatureOffset);
		BaseForm->pnlTempWarning->Caption->Text = FormatFloat("0.0", TemperatureWarningSV);
		BaseForm->pnlTempDanger->Caption->Text = FormatFloat("0.0", TemperatureDangerSV);
	}

	if(stateChanged && MainForm != NULL) {
		MainForm->memoMainLineAdd(AnsiString("Smoke detector state: smoke=") +
			AnsiString(SmokeAlarm ? "ON" : "OFF") +
			", temp=" + AnsiString(FormatFloat("0.0", TemperaturePV)));
	}
}
//---------------------------------------------------------------------------
void __fastcall TSmokeDetector::chkTimerTimer(TObject *Sender)
{
	if(bWaitingResponse) {
		failCount++;
		if(failCount >= retryCnt) {
			Reconnect();
			return;
		}
	}

	if(Comm->Connected) {
		bWaitingResponse = true;
		GetTsdData();
	}
	else {
		bWaitingResponse = true;
	}
}
//---------------------------------------------------------------------------
