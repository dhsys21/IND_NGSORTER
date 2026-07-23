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
	int Tag;
	int slaveId;
	int protocolMode;
	bool bWaitingResponse;
	int failCount;
	std::vector<unsigned char> rxBuffer;

	void __fastcall Reconnect();
	unsigned short __fastcall get_crc16(unsigned char *pBuf, int nLen);
	void __fastcall Parse_Modbus(unsigned char* data, int len);
	void __fastcall Parse_HumanAuto(unsigned char* data, int len);
	void __fastcall UpdateState(bool outSmoke, bool outDanger, bool outWarning,
		bool outRun, bool alarmSmoke, bool alarmDanger, bool alarmWarning,
		double temperature, double offset, double warningSv, double dangerSv);

public:		// User declarations
	__fastcall TSmokeDetector(TComponent* Owner);
	bool SmokeOutput;
	bool DangerOutput;
	bool WarningOutput;
	bool RunOutput;
	bool SmokeAlarm;
	bool DangerAlarm;
	bool WarningAlarm;
	double TemperaturePV;
	double TemperatureOffset;
	double TemperatureWarningSV;
	double TemperatureDangerSV;

	void __fastcall CommOpen(AnsiString port, int sep, int id, int mode, int baudRate = 0);
	void __fastcall CommClose();
	void __fastcall ClearAlarm();
	void __fastcall setTsdData(short regAddr, short writeValue);
	void __fastcall GetTsdData();
	void __fastcall GetTsdData_Modbus();
	void __fastcall GetTsdData_HumanAuto();
};
//---------------------------------------------------------------------------
extern PACKAGE TSmokeDetector *SmokeDetector;
//---------------------------------------------------------------------------
#endif
