//---------------------------------------------------------------------------


#pragma hdrstop

#include "FormBase.h"
#include "ModMes_OPCUA.h"
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
	retryInterval = 1000; // Retry every one second.
	tryCnt = 0;
	failCount = 0;        // Reset the consecutive failure count.
	bWaitingResponse = false; // Clear the response-wait state.
	slaveId = 1;
	protocolMode = 0;
	m_temperature = 0.0;
	m_smokeDetected = false;
	m_tempWarning = false;
	m_tempDanger = false;
	m_running = false;
	m_communicationSettingsApplied = false;
}
//---------------------------------------------------------------------------
void __fastcall TSmokeDetector::UpdateFmsEnvStatus(double Temperature,
	bool SmokeDetected, bool TempWarning, bool TempDanger, bool Running)
{
	// FMS EnvStatus retains the last valid detector values so connection-loss
	// reports can change only Running=false without discarding measurements.
	m_temperature = Temperature;
	m_smokeDetected = SmokeDetected;
	m_tempWarning = TempWarning;
	m_tempDanger = TempDanger;
	m_running = Running;

	if(MesOpc != NULL)
		MesOpc->PublishEnvStatus(Temperature, SmokeDetected, TempWarning,
			TempDanger, Running);
}
//---------------------------------------------------------------------------
unsigned short __fastcall TSmokeDetector::get_crc16(unsigned char *pBuf, int nLen)
{
	int i, j;
	unsigned short crc = 0xFFFF;
	for (i = 0; i < nLen; i++)
	{
		crc ^= pBuf[i];
		for (j = 0; j < 8; j++)
		{
			if (crc & 0x01)
			{
				crc >>= 1;
				crc ^= 0xA001;
			}
			else
			{
				crc >>= 1;
			}
		}
	}
	return crc;
}
//---------------------------------------------------------------------------
bool __fastcall TSmokeDetector::HasCommunicationSettings(AnsiString port, int sep,
    int id, int mode, int baudRate) const
{
    if(baudRate <= 0) baudRate = (mode == 0) ? 115200 : 9600;
    return m_communicationSettingsApplied && m_savedPort == port &&
        m_savedSep == sep && m_savedId == id && m_savedMode == mode &&
        m_savedBaudRate == baudRate;
}
//---------------------------------------------------------------------------
void __fastcall TSmokeDetector::CommOpen(AnsiString port, int sep, int id, int mode, int baudRate)
{
	// Remember an attempted configuration even if the device is absent. SAVE
	// must not retry it for unrelated edits; the explicit CONNECT button can.
	if(baudRate <= 0) baudRate = (mode == 0) ? 115200 : 9600;
	m_savedPort = port;
	m_savedSep = sep;
	m_savedId = id;
	m_savedMode = mode;
	m_savedBaudRate = baudRate;
	m_communicationSettingsApplied = true;
	// An empty port disables this device without reopening the previous port.
	if(port.IsEmpty()){
		CommClose();
		return;
	}
	try{
		if(Comm->Connected){
			Comm->Close();
		}

        retryCnt = 3;
        retryInterval = 3000; // Retry every three seconds.
        tryCnt = 0;
        failCount = 0;        // Reset the consecutive failure count.
        bWaitingResponse = false; // Clear the response-wait state.
        slaveId = 1;
        protocolMode = 0;

		Tag = sep;
		slaveId = id; // 센서의 Modbus 국번 저장
        protocolMode = mode;
		Comm->Port = port;

        if(baudRate <= 0)
            baudRate = (protocolMode == 0) ? 115200 : 9600;

        switch(baudRate){
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

        //* RTS, DTR enable해주지 않으면 데이터 수신 안됨.
        Comm->FlowControl->ControlDTR = dtrEnable;
        Comm->FlowControl->ControlRTS = rtsEnable;

		// Modbus RTU is binary. Disable the event terminator so a data byte
		// such as 0x0A cannot split a valid packet.
		Comm->EventChar = 0x00;
		Comm->Open();
		Comm->ClearBuffer(true, true);

        // Start the one-second polling timer after the port opens.
		failCount = 0;
		bWaitingResponse = false;
		chkTimer->Interval = 1000; // 1초 주기 고정
		chkTimer->Enabled = true;
	}
	catch(...){
		Comm->Close();
        chkTimer->Enabled = false; // 실패 시 타이머 중지
		UpdateFmsEnvStatus(m_temperature, m_smokeDetected, m_tempWarning,
			m_tempDanger, false);
		AlarmForm->ShowError("TSD-V50 COM Port", "Can not open " + port + " port.");
	}
}
//---------------------------------------------------------------------------
void __fastcall TSmokeDetector::CommClose()
{
	chkTimer->Enabled = false;
	bWaitingResponse = false;
    if(Comm->Connected)
        Comm->Close();
	UpdateFmsEnvStatus(m_temperature, m_smokeDetected, m_tempWarning,
		m_tempDanger, false);
}
//---------------------------------------------------------------------------
// Reopen the serial port after the detector connection is lost.
void __fastcall TSmokeDetector::Reconnect()
{
	UpdateFmsEnvStatus(m_temperature, m_smokeDetected, m_tempWarning,
		m_tempDanger, false);
    BaseForm->Memo1->Lines->Clear();
    BaseForm->Memo1->Lines->Add("=========================================");
    BaseForm->Memo1->Lines->Add("?? 통신 단절 감지! 포트 재연결 시도 중... ");
    BaseForm->Memo1->Lines->Add("=========================================");

    try {
        // 1. 기존 연결 안전하게 종료
        if(Comm->Connected) {
            Comm->Close();
        }

        // 2. RS-485 칩셋 및 OS가 포트를 정리할 수 있도록 잠시 대기 (0.5초)
        Sleep(500);

        // 3. 기존 정보로 재오픈 시도
        Comm->Open();
        Comm->ClearBuffer(true, true);

        // 상태 초기화
        failCount = 0;
        bWaitingResponse = false;
        BaseForm->Memo1->Lines->Add("? 포트 재오픈 성공! 통신을 재개합니다.");
    }
    catch(const Exception &e)
    {
        BaseForm->Memo1->Lines->Add("통신오류 연결 종료중 예외방생 : " );
    }
    catch(...) {
        // 재오픈 실패 시 다음 1초 뒤 타이머에서 다시 Reconnect를 타게 됨
        BaseForm->Memo1->Lines->Add("? 포트 재오픈 실패. 1초 후 재시도합니다.");
    }
}

//---------------------------------------------------------------------------
// Request TSD-V50 measurement data as the master.
void __fastcall TSmokeDetector::GetTsdData()
{
    if(!Comm->Connected) {
		AlarmForm->ShowError("TSD-V50 COM Port", "Can not open " + Comm->Port + " port.");
		return;
	}

    // Dispatch the request according to the configured protocol.
	if(protocolMode == 0) {
		GetTsdData_Modbus();
	} else {
		GetTsdData_HumanAuto();
	}
}
//---------------------------------------------------------------------------
// Clear TSD-V50 alarms.
void __fastcall TSmokeDetector::ClearAlarm()
{
    short addr = 0x1005;
    short tempvalue = 0x1234;

    setTsdData(addr, tempvalue);
}
//---------------------------------------------------------------------------
// Write a TSD-V50 register value.
void __fastcall TSmokeDetector::setTsdData(short regAddr, short writeValue)
{
    unsigned char txBuf[8];
	txBuf[0] = (unsigned char)slaveId;
	txBuf[1] = 0x06;                   // Write Single Registers
	// Split the register address into high and low bytes.
    txBuf[2] = (unsigned char)((regAddr >> 8) & 0xFF); // 상위 바이트
    txBuf[3] = (unsigned char)(regAddr & 0xFF);        // 하위 바이트

    // Split the register value into high and low bytes.
    // 음수(예: -0.3도 -> Offset -3)도 정상적으로 비트 처리되어 들어갑니다.
    txBuf[4] = (unsigned char)((writeValue >> 8) & 0xFF); // 상위 바이트
    txBuf[5] = (unsigned char)(writeValue & 0xFF);        // 하위 바이트

    // CRC-16 계산 (앞 6바이트)
    unsigned short crc = get_crc16(txBuf, 6);
    txBuf[6] = (unsigned char)(crc & 0xFF);        // CRC 하위(Low) 먼저
    txBuf[7] = (unsigned char)((crc >> 8) & 0xFF); // CRC 상위(High) 나중에

    // Send all bytes through a pointer so embedded 0x00 bytes are preserved.
    Comm->Write((void*)txBuf, 8);
}
//---------------------------------------------------------------------------
// [Tx 함수 1] Modbus RTU 요청 패킷 송신
void __fastcall TSmokeDetector::GetTsdData_Modbus()
{
	unsigned char txBuf[8];
	txBuf[0] = (unsigned char)slaveId;
	txBuf[1] = 0x03;                   // Read Holding Registers
	txBuf[2] = 0x10;                   // 시작 주소 High (1000h)
	txBuf[3] = 0x00;                   // 시작 주소 Low
	txBuf[4] = 0x00;                   // 레지스터 개수 High
	txBuf[5] = 0x05;                   // 레지스터 개수 Low (5개 요구)

	unsigned short crc = get_crc16(txBuf, 6);
	txBuf[6] = (unsigned char)(crc & 0x00FF);
	txBuf[7] = (unsigned char)((crc >> 8) & 0x00FF);

	Comm->Write(txBuf, 8);
}

//---------------------------------------------------------------------------
// [Tx 함수 2] HumanAutomation 전용 프로토콜 요청 패킷 송신
void __fastcall TSmokeDetector::GetTsdData_HumanAuto()
{
	unsigned char txBuf[7];
	txBuf[0] = 0x02;                             // STX
	txBuf[1] = (unsigned char)(slaveId + 0x40);  // Address (ID + 0x40)
	txBuf[2] = 2;                                // Length (Cmd + Dummy Data = 2)
	txBuf[3] = 0x41;                             // Command (0x41)
	txBuf[4] = 0x00;                             // Data 0 (Dummy 0x00)

	unsigned char sum = 0;
	for(int i = 1; i <= 4; i++) { sum += txBuf[i]; }
	txBuf[5] = sum;                              // Sum Check
	txBuf[6] = 0x03;                             // ETX

	Comm->Write(txBuf, 7);
}
//---------------------------------------------------------------------------
void __fastcall TSmokeDetector::CommRxFlag(TObject *Sender)
{
    const int RX_BUF_SIZE = 256;
    unsigned char rxBuf[RX_BUF_SIZE];

    int cnt = Comm->InputCount();
    if (cnt <= 0) return;

    if (cnt > RX_BUF_SIZE) cnt = RX_BUF_SIZE;

	Comm->Read(rxBuf, cnt); // 우선 버퍼 전체를 안전하게 읽음
    for (int i = 0; i < cnt; i++)
    {
        g_rxBuffer.push_back(rxBuf[i]);
    }

	// 설정된 모드에 맞는 수신 파싱 전용 함수 호출
	if(protocolMode == 0) {
        while (g_rxBuffer.size() >= 3)
        {
            unsigned char expectedSlave = (unsigned char)slaveId;

            // 이전 수신 찌꺼기가 섞이면 slave id가 나올 때까지 버리고 프레임 위치를 다시 맞춘다.
            if(g_rxBuffer[0] != expectedSlave){
                unsigned int syncPos = 1;
                while(syncPos < g_rxBuffer.size() && g_rxBuffer[syncPos] != expectedSlave)
                    syncPos++;

                if(syncPos >= g_rxBuffer.size()){
                    g_rxBuffer.clear();
                    break;
                }

                g_rxBuffer.erase(g_rxBuffer.begin(), g_rxBuffer.begin() + syncPos);
                if(g_rxBuffer.size() < 3)
                    break;
            }

            unsigned char funcCode = g_rxBuffer[1]; // 2번째 바이트: 기능 코드
            int expectedLen = 0;

            if (funcCode == 0x03) // [Read 명령 정상응답]
            {
                // TSD-V50 정상 read 응답은 01 03 0A로 시작한다. 아니면 다음 정상 시작점까지 버린다.
                if(g_rxBuffer[2] != 0x0A){
                    unsigned int syncPos = 1;
                    while(syncPos + 2 < g_rxBuffer.size()){
                        if(g_rxBuffer[syncPos] == expectedSlave && g_rxBuffer[syncPos + 1] == 0x03 && g_rxBuffer[syncPos + 2] == 0x0A)
                            break;
                        syncPos++;
                    }

                    if(syncPos + 2 < g_rxBuffer.size())
                        g_rxBuffer.erase(g_rxBuffer.begin(), g_rxBuffer.begin() + syncPos);
                    else
                        g_rxBuffer.erase(g_rxBuffer.begin());
                    continue;
                }

                expectedLen = 5 + g_rxBuffer[2]; // slave + func + byte count + data + crc(2)
            }
            else if (funcCode == 0x06 || funcCode == 0x10) // [Write 정상 응답]
            {
                expectedLen = 8;
            }
            else if ((funcCode & 0x80) == 0x80) // [Modbus Exception 응답]
            {
                expectedLen = 5;
            }
            else
            {
                g_rxBuffer.erase(g_rxBuffer.begin());
                continue;
            }

            // 계산된 예상 길이만큼 데이터가 다 왔는지 확인
            if (g_rxBuffer.size() < (unsigned int)expectedLen) {
                break; // 데이터가 아직 덜 왔으므로 다음 수신 이벤트 때까지 대기
            }

            unsigned short receivedCRC = g_rxBuffer[expectedLen - 2] | (g_rxBuffer[expectedLen - 1] << 8);
            if(get_crc16(&g_rxBuffer[0], expectedLen - 2) != receivedCRC){
                // CRC가 틀리면 현재 시작점만 버리고 다음 01 03 0A 프레임을 다시 찾는다.
                g_rxBuffer.erase(g_rxBuffer.begin());
                continue;
            }

            // 온전한 하나의 Modbus 패킷 분리 완료
            Parse_Modbus(&g_rxBuffer[0], expectedLen);

            // 처리 완료된 패킷 크기만큼 버퍼에서 삭제
            g_rxBuffer.erase(g_rxBuffer.begin(), g_rxBuffer.begin() + expectedLen);
        }
	} else {
		Parse_HumanAuto(rxBuf, cnt);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// [Rx 함수 1] Modbus RTU 응답 데이터 파싱
void __fastcall TSmokeDetector::Parse_Modbus(unsigned char* rxBuf, int cnt)
{
    if (cnt < 5 || rxBuf[0] != slaveId) return;
    unsigned char funcCode = rxBuf[1];

    // Validate CRC over every byte except the final two CRC bytes.
    unsigned short receivedCRC = rxBuf[cnt - 2] | (rxBuf[cnt - 1] << 8);
    if (get_crc16(rxBuf, cnt - 2) != receivedCRC)
    {
        BaseForm->Memo1->Lines->Clear(); // 이전 내용 청소
        BaseForm->Memo1->Lines->Add("=========================================");
        BaseForm->Memo1->Lines->Add("          CRC ERROR          ");
        BaseForm->Memo1->Lines->Add("=========================================");
        return;
    }

	// 4개 레지스터 응답 정상 데이터 길이는 13바이트 고정
    if(funcCode == 0x03 && cnt >= 15)
    {
        bWaitingResponse = false; // Clear the response-wait state.
		failCount = 0;            // Reset the consecutive failure count.

        // Parse each value according to the TSD-V50 register table.
        // rxBuf[3],[4] : 1000h Status (상태 비트)
        // rxBuf[5],[6] : 1001h Temperature PV (현재 온도)
        // rxBuf[7],[8] : 1002h Temperature Offset (온도 오프셋 - 음수 가능하므로 short)
        // rxBuf[9],[10]: 1003h Temp Warning SV (경보 온도 설정값)
        // rxBuf[11],[12] : 1004h Temp Danger SV (위험 온도 설정값)
        // 1005h (쓰기전용) : alarm 해제
        unsigned short status     = (rxBuf[3] << 8) | rxBuf[4];
        short rawTemp             = (rxBuf[5] << 8) | rxBuf[6];
        short rawOffset           = (rxBuf[7] << 8) | rxBuf[8];  // 2의 보수(음수) 표현을 위해 signed short 사용
        unsigned short rawWarning = (rxBuf[9] << 8) | rxBuf[10];
        unsigned short rawDanger = (rxBuf[11] << 8) | rxBuf[12];

        // 스케일(10.0) 적용하여 실제 물리량으로 변환
        double finalTemperature  = rawTemp / 10.0;
        double temperatureOffset = rawOffset / 10.0;
        double tempWarningSV     = rawWarning / 10.0;
        double tempDangerSV      = rawDanger / 10.0;

        // Convert the 1000h status word to a binary diagnostic string.
        UnicodeString binStr = "";
        for(int i = 15; i >= 0; i--) {
            binStr += ((status >> i) & 1) ? "1" : "0";
            if(i == 8) binStr += " "; // 가독성을 위해 상위/하위 8비트 사이에 공백 추가
        }

        // Decode output-state and alarm bits from the 1000h status word.
        bool outSmoke   = (status & (1 << 7)); // Bit 7 : Output State - Smoke
        bool outDanger  = (status & (1 << 6)); // Bit 6 : Output State - Danger
        bool outWarning = (status & (1 << 5)); // Bit 5 : Output State - Warning
        bool outRun     = (status & (1 << 4)); // Bit 4 : Output State - Run (0:off, 1:on)

        bool alarmSmoke = (status & (1 << 2)); // Bit 2 : Alarm Flag - Smoke
        bool alarmTempD = (status & (1 << 1)); // Bit 1 : Alarm Flag - Temp Danger
        bool alarmTempW = (status & (1 << 0)); // Bit 0 : Alarm Flag - Temp Warning

		// FMS EnvStatus Modbus mapping: Temperature=1001h PV,
		// Smoke/Warning/Danger=alarm bits 2/0/1, Running=output bit 4.
		UpdateFmsEnvStatus(finalTemperature, alarmSmoke, alarmTempW,
			alarmTempD, outRun);

        BaseForm->Memo1->Lines->Clear(); // 이전 내용 청소
        BaseForm->Memo1->Lines->Add("=========================================");
        BaseForm->Memo1->Lines->Add("          TSD-V50 MODBUS REPORT          ");
        BaseForm->Memo1->Lines->Add("=========================================");

        // 4. [raw hex] 수신된 전체 Raw 데이터를 16진수 문자열로 예쁘게 출력
        UnicodeString strRawHex = "[RAW HEX] : ";
        for(int i = 0; i < 13; i++) {
            strRawHex += IntToHex(rxBuf[i], 2) + " ";
        }
        BaseForm->Memo1->Lines->Add(strRawHex);
        BaseForm->Memo1->Lines->Add("-----------------------------------------");

        // Display detailed detector status information.
        BaseForm->Memo1->Lines->Add("1000h Status       : 0x" + IntToHex(status, 4) + " (Bin: " + binStr + ")");
        BaseForm->Memo1->Lines->Add("  [Output State]");
        BaseForm->Memo1->Lines->Add("   - Smoke   (Bit7) : " + UnicodeString(outSmoke ? "ON" : "OFF"));
        BaseForm->Memo1->Lines->Add("   - Danger  (Bit6) : " + UnicodeString(outDanger ? "ON" : "OFF"));
        BaseForm->Memo1->Lines->Add("   - Warning (Bit5) : " + UnicodeString(outWarning ? "ON" : "OFF"));
        BaseForm->Memo1->Lines->Add("   - Run     (Bit4) : " + UnicodeString(outRun ? "ON" : "OFF"));
        BaseForm->Memo1->Lines->Add("  [Alarm Flag]");
        BaseForm->Memo1->Lines->Add("   - Smoke   (Bit2) : " + UnicodeString(alarmSmoke ? "Event" : "Normal"));
        BaseForm->Memo1->Lines->Add("   - Temp(D) (Bit1) : " + UnicodeString(alarmTempD ? "Event" : "Normal"));
        BaseForm->Memo1->Lines->Add("   - Temp(W) (Bit0) : " + UnicodeString(alarmTempW ? "Event" : "Normal"));
        BaseForm->Memo1->Lines->Add("-----------------------------------------");

        BaseForm->pnlStatusSmoke->Caption->Text = UnicodeString(outSmoke ? "ON" : "OFF");
        BaseForm->pnlStatusDanger->Caption->Text = UnicodeString(outDanger ? "ON" : "OFF");
        BaseForm->pnlStatusWarning->Caption->Text = UnicodeString(outWarning ? "ON" : "OFF");
        BaseForm->pnlStatusRun->Caption->Text = UnicodeString(outRun ? "ON" : "OFF");

        BaseForm->pnlAlarmReserve->Caption->Text = "-";
        BaseForm->pnlAlarmSmoke->Caption->Text = UnicodeString(alarmSmoke ? "ON" : "OFF");
        BaseForm->pnlAlarmDanger->Caption->Text = UnicodeString(alarmTempD ? "ON" : "OFF");
        BaseForm->pnlAlarmWarning->Caption->Text = UnicodeString(alarmTempW ? "ON" : "OFF");

        // 온도 데이터 출력
        BaseForm->Memo1->Lines->Add("1001h Temp PV      : " + FormatFloat("0.0", finalTemperature) + " C");
        BaseForm->Memo1->Lines->Add("1002h Temp Offset  : " + FormatFloat("0.0", temperatureOffset) + " C");
        BaseForm->Memo1->Lines->Add("1003h Warning SV   : " + FormatFloat("0.0", tempWarningSV) + " C");
        BaseForm->Memo1->Lines->Add("1004h Danger SV   : " + FormatFloat("0.0", tempDangerSV) + " C");
        BaseForm->Memo1->Lines->Add("=========================================");

        BaseForm->pnlTempPV->Caption->Text = FormatFloat("0.0", finalTemperature);
        BaseForm->pnlTempOffset->Caption->Text = FormatFloat("0.0", temperatureOffset);
        BaseForm->pnlTempWarning->Caption->Text = FormatFloat("0.0", tempWarningSV);
        BaseForm->pnlTempDanger->Caption->Text = FormatFloat("0.0", tempDangerSV);
    }
    // -------------------------------------------------------------
    // [Case B] Write Single Register (06h) 정상 응답 처리 (Echo 백)
    // -------------------------------------------------------------
    else if (funcCode == 0x06)
    {
        if (cnt < 8) return;

        unsigned short echoedAddr = (rxBuf[2] << 8) | rxBuf[3];
        short echoedData          = (rxBuf[4] << 8) | rxBuf[5];

        BaseForm->Memo1->Lines->Clear();
        BaseForm->Memo1->Lines->Add("=========================================");
        BaseForm->Memo1->Lines->Add("       MODBUS WRITE SUCCESS (06h)        ");
        BaseForm->Memo1->Lines->Add("=========================================");
        BaseForm->Memo1->Lines->Add("Target Register : 0x" + IntToHex(echoedAddr, 4));
        BaseForm->Memo1->Lines->Add("Written Value   : " + IntToStr(echoedData) + " (Scaled: " + FormatFloat("0.0", echoedData / 10.0) + ")");
        BaseForm->Memo1->Lines->Add("=========================================");
    }
    // -------------------------------------------------------------
    // [Case C] 이상/에러 응답 처리 (매뉴얼 기준 88h, 표준 기준 86h/83h)
    // -------------------------------------------------------------
    else if (funcCode == 0x88 || funcCode == 0x86 || funcCode == 0x83)
    {
        if (cnt < 5) return;

        unsigned char errorCode = rxBuf[2]; // 3번째 바이트가 에러 코드

        BaseForm->Memo1->Lines->Clear();
        BaseForm->Memo1->Lines->Add("=========================================");
        BaseForm->Memo1->Lines->Add("         MODBUS ERROR RESPONSE           ");
        BaseForm->Memo1->Lines->Add("=========================================");
        BaseForm->Memo1->Lines->Add("Failed Function : 0x" + IntToHex(funcCode, 2));
        BaseForm->Memo1->Lines->Add("Modbus Error Code: " + IntToStr(errorCode));
        BaseForm->Memo1->Lines->Add("=========================================");
    }
}
//---------------------------------------------------------------------------
// [Rx 함수 2] HumanAutomation 전용 프로토콜 응답 데이터 파싱
void __fastcall TSmokeDetector::Parse_HumanAuto(unsigned char* rxBuf, int cnt)
{
	if (cnt < 5) return;

	int pos = 0;
	unsigned char expectedAddr = (unsigned char)(slaveId + 0x40);

	while (pos <= cnt - 5)
	{
		if (rxBuf[pos] == 0x02 && rxBuf[pos + 1] == expectedAddr)
		{
			int cmdDataLen = rxBuf[pos + 2];
			int packetSize = cmdDataLen + 5;

			if (pos + packetSize <= cnt)
			{
				// 에코(내가 보낸 요청문) 패킷 필터링
				if (cmdDataLen == 2) {
					pos += packetSize;
					continue;
				}

				// Sum Check 검증
				unsigned char calcSum = 0;
				for (int i = pos + 1; i < pos + packetSize - 2; i++) {
					calcSum += rxBuf[i];
				}

				unsigned char receivedSum = rxBuf[pos + packetSize - 2];
				unsigned char etx         = rxBuf[pos + packetSize - 1];

				if (calcSum == receivedSum && etx == 0x03)
				{
					//chkTimer->Enabled = false; // 성공 시 타이머 해제

					// 예시 파싱 구조 (Data 0, Data 1을 상하위 온도 데이터로 임시 지정)
					short rawTemp = (rxBuf[pos + 4] << 8) | rxBuf[pos + 5];
					double finalTemperature = rawTemp / 10.0;

					// FMS EnvStatus HumanAutomation mapping: this protocol has no
					// individual alarm bits, so report temperature and Running=true.
					UpdateFmsEnvStatus(finalTemperature, false, false, false, true);

					if(ErrorForm_bcr->Visible) {
						ErrorForm_bcr->ShowError(FormatFloat("0.0", finalTemperature) + " C", true);
					}
					return;
				}
			}
		}
		pos++;
	}
}
//---------------------------------------------------------------------------
bool __fastcall TSmokeDetector::Parse_Modbus_Write(unsigned char *rxBuf, int nLen)
{
    // Write(06h) 응답은 정상일 때 정확히 8바이트입니다.
    if (nLen < 8) return false;

    // 1. CRC 검증
    unsigned short calcCRC = get_crc16(rxBuf, nLen - 2);
    unsigned short recvCRC = rxBuf[nLen - 2] | (rxBuf[nLen - 1] << 8);

    if (calcCRC != recvCRC) {
        // CRC 오류
        return false;
    }

    // 2. Modbus 에러 응답(Exception) 체크
    // 사양서 우측 '응답 메시지(이상 시)' 부분: Function Code가 0x86 (0x06 + 0x80)으로 옴
    if (rxBuf[1] == 0x86) {
        // 에러 코드 처리 로직 (예: memo->Lines->Add("쓰기 에러 발생: " + IntToStr(errorCode));)
        return false;
    }

    // 3. 정상 응답(0x06) 처리
    if (rxBuf[1] == 0x06) {
        // 장비가 응답한(실제 쓰기가 적용된) 주소와 데이터 확인
        unsigned short echoedAddr = (rxBuf[2] << 8) | rxBuf[3];
        short echoedData          = (rxBuf[4] << 8) | rxBuf[5];

        // 디버깅/로그용 출력 (TMemo)
        UnicodeString msg = Format("쓰기 성공! [주소: 0x%04X, 값: %d]", ARRAYOFCONST((echoedAddr, echoedData)));
        // memo->Lines->Add(msg);

        return true; // 설정 성공!
    }

    return false;
}
//---------------------------------------------------------------------------
void __fastcall TSmokeDetector::chkTimerTimer(TObject *Sender)
{
    // 1. [상태 체크] 이전 턴에 보낸 요청에 대해 아직 응답을 못 받았다면? (타임아웃 발생)
    if (bWaitingResponse)
    {
        failCount++;
        BaseForm->Memo1->Lines->Add("? 응답 타임아웃 발생 (" + IntToStr(failCount) + "/" + IntToStr(retryCnt) + ")");

        // 연속 실패 횟수가 지정된 횟수(3회)를 초과하면 재연결 루틴 실행
        if (failCount >= retryCnt)
        {
            Reconnect();
            return; // 재연결을 시도했으므로 이번 턴은 무시하고 다음 초에 다시 요청함
        }
    }

    // 2. [데이터 요청] 정상 상태이거나 재시도 중이라면 새로운 요청 패킷 전송
    if (Comm->Connected)
    {
        bWaitingResponse = true; // 응답 대기 상태로 전환
        GetTsdData();
    }else{
        bWaitingResponse = true;
    }

}
//---------------------------------------------------------------------------
