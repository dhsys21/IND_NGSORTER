//---------------------------------------------------------------------------

#ifndef SmokeDetector_commH
#define SmokeDetector_commH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include "CPort.hpp"
#include <Vcl.ExtCtrls.hpp>
#include <vector>
//---------------------------------------------------------------------------
class TSmokeDetector : public TDataModule
{
__published:	// IDE-managed Components
	TComPort *Comm;
	TTimer *chkTimer;
	void __fastcall CommRxFlag(TObject *Sender);
	void __fastcall chkTimerTimer(TObject *Sender);
private:	// User declarations
    int retryCnt;
    int retryInterval;
    int tryCnt;
    int Tag;          // 설비 내 센서 구분용 (sep)
    int slaveId;      // TSD-V50 국번 (기본값 1)
    int protocolMode;  // ?? 0: Modbus RTU, 1: HumanAutomation
    std::vector<unsigned char> g_rxBuffer;

    AnsiString m_savedPort;
    int m_savedSep;
    int m_savedId;
    int m_savedMode;

    // 통신 상태 체크용 플래그
    bool bWaitingResponse; // 현재 응답을 기다리는 중인지 여부
    int failCount;         // 연속 타임아웃 발생 횟수

    void __fastcall Reconnect(); // 재연결 전용 함수

    unsigned short __fastcall get_crc16(unsigned char *pBuf, int nLen);
    void __fastcall Parse_Modbus(unsigned char* rxBuf, int cnt);
    bool __fastcall Parse_Modbus_Write(unsigned char *rxBuf, int nLen);
    void __fastcall Parse_HumanAuto(unsigned char* rxBuf, int cnt);
public:		// User declarations
	__fastcall TSmokeDetector(TComponent* Owner);
    unsigned short __fastcall CalculateCRC(unsigned char *buf, int len);
    void __fastcall QueryTSD50(int slaveId);
    void __fastcall ParseTSD50Response(unsigned char *rcvBuf, int rcvLen);
    void __fastcall CommOpen(AnsiString port, int sep, int id, int mode, int baudRate = 0);
    void __fastcall CommClose();
    void __fastcall ClearAlarm();
    void __fastcall setTsdData(short regAddr, short writeValue);
    void __fastcall GetTsdData();
    void __fastcall GetTsdData_Modbus();
    void __fastcall GetTsdData_HumanAuto();
    void __fastcall setTsdData(int tag, double temp);
};
//---------------------------------------------------------------------------
extern PACKAGE TSmokeDetector *SmokeDetector;
//---------------------------------------------------------------------------
#endif
