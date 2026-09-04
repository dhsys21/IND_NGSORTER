//---------------------------------------------------------------------------

#include <vcl.h>
#include <vector>
#pragma hdrstop

#include "FormBase.h"
#include "FormMain.h"
#include "FormDryRun.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
Trobostar *robostar;
// KEYLOCK outputs are sequenced without Sleep so the UI and I/O scan keep running.
static const DWORD KEYLOCK_OUTPUT_DELAY_MS = 500;
//---------------------------------------------------------------------------
//* max speed mode - need remove
static bool UseFatOptimizeSequenceDelay()
{
	// This FAT option removes only intermediate timer scans. Every physical
	// position, cell, gripper-feedback, and tray-centering check is retained.
	return BaseForm != NULL && BaseForm->config.optimizeSequenceDelay;
}
//---------------------------------------------------------------------------
//* max speed mode - need remove
static bool UseFatSkipGripStabilization()
{
	// This independent FAT option removes only the 0.2 second dwell after
	// CHUCK/OPEN feedback has already been physically confirmed.
	return BaseForm != NULL && BaseForm->config.skipGripStabilization;
}
//---------------------------------------------------------------------------
const int errCnt = 100;//300;
static const wchar_t *MR_MC2XX_PARAMETER_FILE = L"D:\\IND_NGSORTER_SERVO\\SampleData.prm2";

struct MR_MC2XX_PARAMETER_ENTRY{
	int lineNumber;
	int axis;
	short number;
	short value;
};

static bool TryParseParameterWord(const UnicodeString &text, short &value)
{
	UnicodeString token = text.Trim();
	if(token.Length() >= 2 &&
		(token.SubString(1, 2) == L"0x" || token.SubString(1, 2) == L"0X"))
	{
		token = token.SubString(3, token.Length() - 2);
	}

	int parsed = 0;
	if(token.IsEmpty() || !TryStrToInt(L"0x" + token, parsed) || parsed < 0 || parsed > 0xFFFF)
		return false;

	value = (short)(unsigned short)parsed;
	return true;
}

__fastcall Trobostar::Trobostar(TComponent* Owner)
	: TDataModule(Owner)
{
	config.stno = 255;
	config.devno = 0;
	btx = false;
	seq = seqIdle;
	pauseStatus = false;

	board_id = 0;
	channel_id = 1;
	timeout = 10000;
	sscOpened = false;
	m_ccLinkOpened = false;
	m_ccLinkRunning = false;
	keyLockSetPending = false;
	keyLockReleasePending = false;
	previousBypassSwitchOn = false;
	keyLockSetSafetyBypassOffTick = 0;
	keyLockReleaseOutputOffTick = 0;
	jogSpeed = 100;
	safetyResetPulseUntilTick = 0;
	move.pallet = 0;
	move.channel = 0;
	activeMoveValid = false;
	directXYPositionReady = false;
	homeRequiredAfterServoOff = false;
	centeringMotionMonitorActive = false;
	centeringMotionMonitorSeq = seqIdle;
	centeringRequireSource = false;
	centeringRequireTarget = false;
	for(int i = 0; i < AxisCnt; ++i)
		activeTarget[i] = 0;

	point[0].position = 0;
}
//---------------------------------------------------------------------------
int __fastcall Trobostar::io_Init()
{
	if(sscOpened){
		short num[4] = {0x024E, 0, 0, 0};
		for(int i = 1; i <= servoCnt; i++)
			sscSetMonitor(board_id, channel_id, i, &num[0]);
	}

	senTimer->Enabled = true;

	if(m_ccLinkOpened){
		short size = sizeof(gripper);
		mdReceive(config.path, config.stno, DevY, 0x0020, &size, &gripper);
	}	// IO 상태는 초기 상태를 불러온다 : 셀을 놓을 수 있기 때문에
	return 0;

}
//---------------------------------------------------------------------------
int __fastcall Trobostar::io_Read()
{
	if(!m_ccLinkOpened){
		m_ccLinkRunning = false;
		return -1;
	}

	short led[6] = {0, 0, 0, 0, 0, 0};
	int ledResult = mdBdLedRead(config.path, &led[0]);
	if(ledResult != 0){
		m_ccLinkRunning = false;
		return ledResult;
	}

	unsigned short led0 = (unsigned short)led[0];
	unsigned short led3 = (unsigned short)led[3];
	unsigned short led4 = (unsigned short)led[4];
	bool boardRun = (led0 & 0x0001) != 0;
	bool allStationError = (led0 & 0x0100) != 0;
	bool lineError = (led3 & 0x0100) != 0;
	bool timeoutError = (led3 & 0x0001) != 0;
	bool linkError = (led4 & 0x0100) != 0;
	bool linkRun = (led4 & 0x0001) != 0;

	// CC-Link inputs are valid only while the board and cyclic data link are healthy.
	m_ccLinkRunning = boardRun && linkRun && !allStationError &&
		!lineError && !timeoutError && !linkError;
	if(!m_ccLinkRunning)
		return -1;

	INPUT_ROBOT receivedInput = input;
	short size = sizeof(receivedInput);
	int receiveResult = mdReceive(config.path, config.stno, DevX, 0x0000,
		&size, &receivedInput);
	if(receiveResult == 0)
		input = receivedInput;
	else
		m_ccLinkRunning = false;

	return receiveResult;
}
//---------------------------------------------------------------------------
int __fastcall Trobostar::io_Write()
{
	short size = sizeof(output);
	return mdSend(config.path, config.stno, DevY, 0x0000, &size, &output);
}
//---------------------------------------------------------------------------
int __fastcall Trobostar::io_WriteGripper()
{
	short size = sizeof(gripper);
	return mdSend(config.path, config.stno, DevY, 0x0020, &size, &gripper);
}
//---------------------------------------------------------------------------

void __fastcall Trobostar::req_Pause(bool stop)
{
	step.timeout = 0;
	step.delay = 0;

	if(stop != pauseStatus){
		if(stop){
			seq_save = seq;
			seq = seqPause;
			step_save.step = step.step;
			pauseStatus = true;
			MainForm->memoRobostarLineAdd("[C_Maint] [로보트]일시정지 상태입니다.");
		}else{
			seq = seq_save;
			step.step = step_save.step;
			pauseStatus = false;
			MainForm->memoRobostarLineAdd("[C_Maint] [로보트]일시정지가 해제되었습니다.");
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::InitSequence(robotSequence data, robotSequence reserve)
{
	// Keep MELSEC I/O alive when the position board cannot be opened, but allow
	// seqInit so the Servo Open button can retry sscOpen().
	if(!sscOpened && data != seqIdle && data != seqPause && data != seqInit) return;

	// This machine has no gripper vertical cylinder. Always close the moving
	// overlay when a servo sequence returns to idle, including error/stop paths.
	if(data == seqIdle && teachForm != NULL){
		teachForm->pnlMovingAlarm->Visible = false;
		teachForm->pnlMovingAlarm2->Visible = false;
	}

	seq = data;
	step.step = 0;
	step.delay = 0;
	step.timeout = 0;
	step.reserve = reserve;
}
//---------------------------------------------------------------------------


bool __fastcall Trobostar::WriteLog(int status, UnicodeString msg)
{
	bool success = (status == SSC_OK);
	UnicodeString logMsg;

	if(success)
		logMsg = msg + " 성공";
	else
		logMsg = msg + " 실패 : " + IntToHex(sscGetLastError(), 6);

	if(MainForm != NULL)
		MainForm->memoRobostarLineAdd(logMsg);
	return success;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::StartServoSystemFromParameterFile()
{
	// Reuse the controller state left RUNNING by manual operation or a previous
	// program instance. Parameter reset/reboot is needed only when it is stopped.
	if(RestoreServoState()){
		if(MainForm != NULL)
			MainForm->memoRobostarLineAdd("[SERVO OPEN] Existing RUNNING system reused; reboot skipped.");
		return true;
	}
	if(!sscOpened) return false;

	std::vector<MR_MC2XX_PARAMETER_ENTRY> parameters;
	parameters.reserve(4000);
	bool foundExternalForcedStop = false;
	bool foundOperationControl[4] = {false, false, false, false};
	bool foundServoNetwork[4] = {false, false, false, false};
	UnicodeString validationError;
	UnicodeString parameterPath = MR_MC2XX_PARAMETER_FILE;

	if(!FileExists(parameterPath)){
		MainForm->memoRobostarLineAdd("[PARAMETER FILE] not found: " + parameterPath);
		return false;
	}

	TStringList *lines = new TStringList();
	TStringList *fields = new TStringList();
	fields->StrictDelimiter = true;
	fields->Delimiter = ',';
	try{
		lines->LoadFromFile(parameterPath);
		for(int i = 0; i < lines->Count; ++i){
			UnicodeString rawLine = lines->Strings[i].Trim();
			if(rawLine.IsEmpty()) continue;

			fields->DelimitedText = rawLine;
			int axis = 0;
			short number = 0;
			short value = 0;
			if(fields->Count != 3 ||
				!TryStrToInt(fields->Strings[0].Trim(), axis) ||
				!TryParseParameterWord(fields->Strings[1], number) ||
				!TryParseParameterWord(fields->Strings[2], value))
			{
				validationError = "invalid record at line " + IntToStr(i + 1) + ": " + rawLine;
				break;
			}

			// The machine uses only the system and axes 1-3. Ignore axes 4-32 and RIO records.
			if(axis != 0 && (axis < 1 || axis > servoCnt)) continue;

			MR_MC2XX_PARAMETER_ENTRY entry;
			entry.lineNumber = i + 1;
			entry.axis = axis;
			entry.number = number;
			entry.value = value;
			parameters.push_back(entry);

			if(axis == 0 && (unsigned short)number == 0x000E){
				foundExternalForcedStop = ((unsigned short)value == 0x5AE1);
				if(!foundExternalForcedStop){
					validationError = "system parameter 000E must be 5AE1";
					break;
				}
			}
			if(axis >= 1 && axis <= servoCnt){
				if((unsigned short)number == 0x0200 && (unsigned short)value == 0x0001)
					foundOperationControl[axis] = true;
				if((unsigned short)number == 0x0203 && (unsigned short)value == (unsigned short)axis)
					foundServoNetwork[axis] = true;
			}
		}
	}
	catch(Exception &exception){
		validationError = exception.Message;
	}
	delete fields;
	delete lines;

	if(validationError.IsEmpty() && !foundExternalForcedStop)
		validationError = "required system parameter 000E=5AE1 is missing";
	for(int axis = 1; validationError.IsEmpty() && axis <= servoCnt; ++axis){
		if(!foundOperationControl[axis])
			validationError = "axis " + IntToStr(axis) + " parameter 0200=0001 is missing";
		else if(!foundServoNetwork[axis])
			validationError = "axis " + IntToStr(axis) + " parameter 0203=" +
				IntToHex(axis, 4) + " is missing";
	}

	if(!validationError.IsEmpty()){
		MainForm->memoRobostarLineAdd("[PARAMETER FILE] validation failed: " + validationError);
		return false;
	}

	MainForm->memoRobostarLineAdd("[PARAMETER FILE] validated: path=" + parameterPath +
		", applyCount=" + IntToStr((int)parameters.size()));
	MainForm->memoRobostarLineAdd("[SAFETY CONFIG] system 000E=5AE1; external CC-Link/Safety Relay must remain operational.");

	int sts = sscReboot(board_id, channel_id, timeout);
	if(!WriteLog(sts, "REBOOT")) return false;
	sts = sscResetAllParameter(board_id, channel_id, timeout);
	if(!WriteLog(sts, "RESET PARAMETER")) return false;

	DWORD applyStarted = GetTickCount();
	int appliedCount = 0;
	for(size_t i = 0; i < parameters.size(); ++i){
		const MR_MC2XX_PARAMETER_ENTRY &parameter = parameters[i];
		if(parameter.axis != 0) continue;
		sts = sscChangeParameter(board_id, channel_id, parameter.axis,
			parameter.number, parameter.value);
		if(sts != SSC_OK){
			MainForm->memoRobostarLineAdd("[PARAMETER FILE] APPLY failed: line=" +
				IntToStr(parameter.lineNumber) + ", axis=0, parameter=0x" +
				IntToHex((int)(unsigned short)parameter.number, 4) + ", value=0x" +
				IntToHex((int)(unsigned short)parameter.value, 4) + ", return=0x" +
				IntToHex(sts, 8) + ", lastError=0x" + IntToHex(sscGetLastError(), 8));
			return false;
		}
		++appliedCount;
	}

	for(int axis = 1; axis <= servoCnt; ++axis){
		std::vector<MR_MC2XX_PARAMETER_ENTRY> axisParameters;
		axisParameters.reserve(1200);
		for(size_t i = 0; i < parameters.size(); ++i){
			if(parameters[i].axis == axis) axisParameters.push_back(parameters[i]);
		}

		for(size_t i = 0; i < axisParameters.size(); i += 2){
			const MR_MC2XX_PARAMETER_ENTRY &first = axisParameters[i];
			if(i + 1 >= axisParameters.size()){
				sts = sscChangeParameter(board_id, channel_id, axis, first.number, first.value);
				if(sts != SSC_OK){
					MainForm->memoRobostarLineAdd("[PARAMETER FILE] APPLY failed: line=" +
						IntToStr(first.lineNumber) + ", axis=" + IntToStr(axis) +
						", parameter=0x" + IntToHex((int)(unsigned short)first.number, 4) +
						", value=0x" + IntToHex((int)(unsigned short)first.value, 4) +
						", return=0x" + IntToHex(sts, 8) + ", lastError=0x" +
						IntToHex(sscGetLastError(), 8));
					return false;
				}
				++appliedCount;
			}
			else{
				const MR_MC2XX_PARAMETER_ENTRY &second = axisParameters[i + 1];
				short numbers[2] = {first.number, second.number};
				short values[2] = {first.value, second.value};
				char status[2] = {0, 0};
				sts = sscChange2Parameter(board_id, channel_id, axis, numbers, values, status);
				if(sts != SSC_OK){
					MainForm->memoRobostarLineAdd("[PARAMETER FILE] PAIR APPLY failed: axis=" +
						IntToStr(axis) + ", lines=" + IntToStr(first.lineNumber) + "/" +
						IntToStr(second.lineNumber) + ", parameters=0x" +
						IntToHex((int)(unsigned short)first.number, 4) + "/0x" +
						IntToHex((int)(unsigned short)second.number, 4) + ", return=0x" +
						IntToHex(sts, 8) + ", lastError=0x" + IntToHex(sscGetLastError(), 8));
					return false;
				}
				appliedCount += 2;
			}

			if(appliedCount > 0 && appliedCount % 500 == 0)
				MainForm->memoRobostarLineAdd("[PARAMETER FILE] applying: " +
					IntToStr(appliedCount) + "/" + IntToStr((int)parameters.size()));
		}
	}

	if(appliedCount != (int)parameters.size()){
		MainForm->memoRobostarLineAdd("[PARAMETER FILE] APPLY count mismatch: applied=" +
			IntToStr(appliedCount) + ", expected=" + IntToStr((int)parameters.size()));
		return false;
	}
	MainForm->memoRobostarLineAdd("[PARAMETER FILE] apply success: count=" +
		IntToStr(appliedCount) + ", elapsedMs=" + IntToStr((int)(GetTickCount() - applyStarted)) +
		", FlashROM=unchanged");

	// Read back the same safety, operation-control and network-number values used by the x64 program.
	short checkedValue = 0;
	sts = sscCheckParameter(board_id, channel_id, 0, (short)0x000E, &checkedValue);
	MainForm->memoRobostarLineAdd("[WORKING PARAMETER] axis=0, parameter=0x000E, value=0x" +
		IntToHex((int)(unsigned short)checkedValue, 4) + ", expected=0x5AE1, read=0x" + IntToHex(sts, 8));
	for(int axis = 1; axis <= servoCnt; ++axis){
		short checkNumbers[2] = {(short)0x0200, (short)0x0203};
		short expectedValues[2] = {(short)0x0001, (short)axis};
		for(int index = 0; index < 2; ++index){
			checkedValue = 0;
			sts = sscCheckParameter(board_id, channel_id, axis, checkNumbers[index], &checkedValue);
			MainForm->memoRobostarLineAdd("[WORKING PARAMETER] axis=" + IntToStr(axis) +
				", parameter=0x" + IntToHex((int)(unsigned short)checkNumbers[index], 4) +
				", value=0x" + IntToHex((int)(unsigned short)checkedValue, 4) +
				", expected=0x" + IntToHex((int)(unsigned short)expectedValues[index], 4) +
				", read=0x" + IntToHex(sts, 8));
		}
	}

	sts = sscSystemStart(board_id, channel_id, timeout);
	if(!WriteLog(sts, "SERVO SYSTEM START COMMAND")) return false;

	DWORD waitStarted = GetTickCount();
	do{
		sts = sscGetSystemStatusCode(board_id, channel_id, &mr2.system_status);
		if(sts != SSC_OK){
			WriteLog(sts, "GET SYSTEM STATUS");
			return false;
		}
		if(mr2.system_status == SSC_STS_CODE_RUNNING) break;
		Sleep(100);
	}while((DWORD)(GetTickCount() - waitStarted) < (DWORD)timeout);

	if(mr2.system_status != SSC_STS_CODE_RUNNING){
		MainForm->memoRobostarLineAdd("SERVO SYSTEM START timeout: status=0x" +
			IntToHex((int)(unsigned short)mr2.system_status, 4));
		return false;
	}
	MainForm->memoRobostarLineAdd("SERVO SYSTEM RUNNING: status=0x000A");

	int mesv[4] = {-1, -1, -1, -1};
	waitStarted = GetTickCount();
	bool allConnected = false;
	do{
		allConnected = true;
		for(int axis = 1; axis <= servoCnt; ++axis){
			sts = sscGetStatusBitSignalEx(board_id, channel_id, axis,
				SSC_STSBIT_AX_MESV, &mesv[axis]);
			if(sts != SSC_OK){
				WriteLog(sts, "[" + IntToStr(axis) + "] GET SERVO COMMUNICATION STATUS");
				return false;
			}
			if(mesv[axis] != SSC_BIT_OFF) allConnected = false;
		}
		if(allConnected) break;
		Sleep(100);
	}while((DWORD)(GetTickCount() - waitStarted) < (DWORD)timeout);

	if(!allConnected){
		short emergencyStatus = 0;
		sscGetEmgStatus(board_id, channel_id, &emergencyStatus);
		MainForm->memoRobostarLineAdd("SSCNET servo communication timeout: AXIS1 MESV=" +
			IntToStr(mesv[1]) + ", AXIS2 MESV=" + IntToStr(mesv[2]) +
			", AXIS3 MESV=" + IntToStr(mesv[3]) + ", EMIO=" + IntToStr(emergencyStatus));
		return false;
	}
	MainForm->memoRobostarLineAdd("SSCNET servo amplifier communication ready: MESV=0 (AXIS 1/2/3)");

	short monitorNum[4] = {0x024E, 0, 0, 0};
	for(int axis = 1; axis <= servoCnt; ++axis)
		WriteLog(sscSetMonitor(board_id, channel_id, axis, &monitorNum[0]),
			"[" + IntToStr(axis) + "] MONITOR");

	short emergencyStatus = 0;
	sts = sscGetEmgStatus(board_id, channel_id, &emergencyStatus);
	if(sts == SSC_OK && emergencyStatus != 0)
		MainForm->memoRobostarLineAdd("SERVO SYSTEM OPEN completed with forced stop active: EMIO=" +
			IntToStr(emergencyStatus));

	MainForm->memoRobostarLineAdd("SERVO SYSTEM OPEN complete: status=0x000A");
	return true;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::RestoreServoState()
{
	// Fail closed: a board/status read failure must never reuse stale ready flags.
	if(MainForm != NULL){
		MainForm->m_ServoOpen = false;
		MainForm->m_ServoON = false;
		MainForm->m_ServoHome = false;
		MainForm->m_ServoHomeEmg = false;
	}

	// sscOpen() creates this process's board handle. It does not mean that the
	// controller must be rebooted; an already RUNNING system can be reused.
	if(!sscOpened){
		int openStatus = sscOpen(board_id);
		if(openStatus != SSC_OK){
			if(MainForm != NULL)
				MainForm->memoRobostarLineAdd("[SERVO RESTORE] Position board attach failed: return=0x" +
					IntToHex(openStatus, 8) + ", lastError=0x" + IntToHex(sscGetLastError(), 8));
			return false;
		}
		sscOpened = true;
		if(MainForm != NULL)
			MainForm->memoRobostarLineAdd("[SERVO RESTORE] Position board attached.");
	}

	short systemStatus = 0;
	int statusResult = sscGetSystemStatusCode(board_id, channel_id, &systemStatus);
	if(statusResult != SSC_OK){
		if(MainForm != NULL)
			MainForm->memoRobostarLineAdd("[SERVO RESTORE] System status read failed: return=0x" +
				IntToHex(statusResult, 8) + ", lastError=0x" + IntToHex(sscGetLastError(), 8));
		return false;
	}

	mr2.system_status = systemStatus;
	if(systemStatus != SSC_STS_CODE_RUNNING){
		if(MainForm != NULL){
			MainForm->m_ServoOpen = false;
			MainForm->m_ServoON = false;
			MainForm->m_ServoHome = false;
			MainForm->m_ServoHomeEmg = false;
			MainForm->memoRobostarLineAdd("[SERVO RESTORE] Existing system is not RUNNING: status=0x" +
				IntToHex((int)(unsigned short)systemStatus, 4));
		}
		return false;
	}

	bool allServoOn = true;
	bool allOriginComplete = true;
	bool allAtWaitPosition = true;
	short monitorNum[4] = {0x024E, 0, 0, 0};
	for(int axis = 1; axis <= servoCnt; ++axis){
		sscSetMonitor(board_id, channel_id, axis, &monitorNum[0]);

		int ready = SSC_BIT_OFF;
		int zeroRequest = SSC_BIT_ON;
		long position = 0;
		if(sscGetStatusBitSignalEx(board_id, channel_id, axis,
			SSC_STSBIT_AX_RDY, &ready) != SSC_OK)
			ready = SSC_BIT_OFF;
		if(sscGetStatusBitSignalEx(board_id, channel_id, axis,
			SSC_STSBIT_AX_ZREQ, &zeroRequest) != SSC_OK)
			zeroRequest = SSC_BIT_ON;
		if(sscGetCurrentCmdPositionFast(board_id, channel_id, axis, &position) != SSC_OK)
			allAtWaitPosition = false;

		mr2.servo[axis] = ready;
		mr2.zero[axis] = zeroRequest;
		mr2.pos[axis] = position;
		if(ready != SSC_BIT_ON) allServoOn = false;
		if(zeroRequest != SSC_BIT_OFF) allOriginComplete = false;
		if(position != 0) allAtWaitPosition = false;
	}

	if(MainForm != NULL){
		MainForm->m_ServoOpen = true;
		MainForm->m_ServoON = allServoOn;
		MainForm->m_ServoHomeEmg = allServoOn && allOriginComplete
			&& !homeRequiredAfterServoOff;
		MainForm->m_ServoHome = allServoOn && allOriginComplete && allAtWaitPosition
			&& !homeRequiredAfterServoOff;
		MainForm->memoRobostarLineAdd("[SERVO RESTORE] Existing RUNNING state restored: ServoON=" +
			IntToStr(allServoOn ? 1 : 0) + ", Origin=" + IntToStr(allOriginComplete ? 1 : 0) +
			", XYZ0=" + IntToStr(allAtWaitPosition ? 1 : 0));
	}
	return true;
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::Init()
{
	int sts = SSC_OK;

	// Servo Open requires both safety-ready inputs. Report every failed input.
	if(!IsSafetyReady()){
		UnicodeString message = L"Servo Open blocked by safety I/O.";
		if(!input.SAFETY_EMG_READY)
			message += L"\r\nX002B SAFETY EMG READY: OFF";
		if(!input.SAFETY_DOOR_READY)
			message += L"\r\nX002C SAFETY DOOR READY: OFF";
		MainForm->memoRobostarLineAdd(message);
		ShowMessage(message);
		InitSequence(seqIdle);
		return;
	}

	//* 2026 08 07 천안 불량선별기처럼 Servo Open 시퀀스의 첫 단계에서 보드를 Open
	// 재실행 시에는 이미 Open된 보드에 sscOpen()을 중복 호출하지 않는다.
	if(!sscOpened){
		sts = sscOpen(board_id);
		sscOpened = WriteLog(sts, "POSITION BOARD OPEN");
		if(!sscOpened){
			mr2.system_status = 0;
			InitSequence(seqIdle);
			return;
		}
	}

	if(!StartServoSystemFromParameterFile()) goto open_failed;
	InitSequence(seqIdle);
	return;

open_failed:
	mr2.system_status = 0;
	InitSequence(seqIdle);
}
//---------------------------------------------------------------------------
// 2019 07 05 홈위치 이동시 그리퍼 움직이지 않고 x, y, z 축 원점 후에 그리퍼에 셀이 있는지 확인하는 버전
void __fastcall Trobostar::Home()
{
	if(!MainForm->m_ServoOpen)
	{
		InitSequence(seqIdle);
		MainForm->memoRobostarLineAdd("[C_Maint] 서보 연결 상태를 확인하세요.");
		AlarmForm->ShowError("[C_Maint] 서보가 연결되지 않았습니다.", "확인하고 재시작 하세요.");
		return;
	}
	else if(!MainForm->m_ServoON)
	{
		InitSequence(seqIdle);
		MainForm->memoRobostarLineAdd("[C_Maint] 서보 ON 상태를 확인하세요.");
		AlarmForm->ShowError("[C_Maint] 서보가 켜지지 않았습니다.", "확인하고 재시작 하세요.");
		return;
	}

	//* Z LIMIT RECOVERY: Never start HOME while either Z travel limit is active.
	// Reset the alarm, jog DOWN(+) away from the upper LSN limit, and retry HOME
	// only after both limit inputs return to their normal state.
	if(IsZLimitActive())
	{
		InitSequence(seqIdle);
		MainForm->memoRobostarLineAdd(
			"[Z LIMIT RECOVERY] HOME blocked: clear the Z limit with JOG DOWN, then retry HOME.");
		AlarmForm->ShowError("Z Axis limit is active.",
			"Reset alarm and JOG DOWN until the limit clears, then run HOME.");
		return;
	}

	int sts = 0;
	int bitInfo = 0;

	switch(step.step){
		case 0: // Z축 원점 시작
            teachForm->pnlMovingAlarm->Visible = true;
			teachForm->pnlMovingAlarm->BringToFront();
			teachForm->pnlMovingAlarm2->Visible = true;
			teachForm->pnlMovingAlarm2->BringToFront();
			sts = sscHomeReturnStart(board_id, channel_id, Axis_z);
			WriteLog(sts, "Z Axis Servo Home - Request");
			step.step = 3;
			break;
		case 3: // Z축 완료 확인
			sts = sscGetStatusBitSignalEx(board_id, channel_id, Axis_z, SSC_STSBIT_AX_ZREQ, &bitInfo);
			if(bitInfo){
				MainForm->memoRobostarLineAdd("[C_Maint] Z Axis Servo Home - Moving");
			}
			else{
				MainForm->memoRobostarLineAdd("[C_Maint] Z Axis Servo Home - Complete");
				step.step += 1;
			}
			break;
		case 4: // X축 원점
			sts = sscHomeReturnStart(board_id, channel_id, Axis_x);
			WriteLog(sts, "X Axis Servo Home - Request");
			step.step += 1;
			break;
		case 5: // Y축 원점
			sts = sscHomeReturnStart(board_id, channel_id, Axis_y);
			WriteLog(sts, "Y Axis Servo Home - Request");
			step.step += 1;
			break;
		case 6:
			sts = sscGetStatusBitSignalEx(board_id, channel_id, Axis_y, SSC_STSBIT_AX_ZREQ, &bitInfo);
			if(bitInfo){
				MainForm->memoRobostarLineAdd("Y Axis Servo Home - Moving");
			}
			else{
				MainForm->memoRobostarLineAdd("Y Axis Servo Home - Complete");
				step.step += 1;
			}
			break;
		case 7:
			sts = sscGetStatusBitSignalEx(board_id, channel_id, Axis_x, SSC_STSBIT_AX_ZREQ, &bitInfo);
			if(bitInfo){
				MainForm->memoRobostarLineAdd("X Axis Servo Home - Moving");
			}
			else{
				// Z, Y and X home-return completion re-enables the HOME state.
				homeRequiredAfterServoOff = false;
				MainForm->memoRobostarLineAdd("X Axis Servo Home - Complete / HOME state restored");
				step.step += 1;
			}
			break;
		case 8:
			teachForm->pnlMovingAlarm->Visible = false;
			teachForm->pnlMovingAlarm2->Visible = false;

			// X0022 ON means no cell. No cell is the normal HOME completion state.
			// A detected cell is logged, but either result must terminate seqHome.
			if(getCellDetectStatus())
				MainForm->memoRobostarLineAdd("[HOME COMPLETE CHECK] X0022=0 / cell detected on gripper");
			else
				MainForm->memoRobostarLineAdd("[HOME COMPLETE CHECK] X0022=1 / gripper cell clear");
			step.step = 99;
			break;
		default:
			InitSequence(seqIdle);
			break;
	}
}

//---------------------------------------------------------------------------
void __fastcall Trobostar::ServoOn()
{
	if(!IsSafetyReady()){
		MainForm->memoRobostarLineAdd("[Safety] Servo ON blocked: X002B/X002C must be ON.");
		InitSequence(seqIdle);
		return;
	}

	if(!sscOpened || mr2.system_status != SSC_STS_CODE_RUNNING){
		MainForm->memoRobostarLineAdd("Servo ON 차단: 먼저 Servo System Open을 확인하세요.");
		InitSequence(seqIdle);
		return;
	}

	short emergencyStatus = 0;
	int emergencyResult = sscGetEmgStatus(board_id, channel_id, &emergencyStatus);
	if(emergencyResult != SSC_OK || emergencyStatus != 0){
		MainForm->memoRobostarLineAdd("Servo ON blocked: forced stop status EMIO=" +
			IntToStr(emergencyStatus) + ", return=0x" + IntToHex(emergencyResult, 8));
		InitSequence(seqIdle);
		return;
	}

	int sts = 0;
	//* 2026 08 10 Position Board Utility/legacy test program과 동일한 전용 Servo ON API 사용
	//* 모든 축에 ON 명령을 먼저 전송한 뒤 Ready를 확인하여 축별 대기 때문에 후속 축이 지연되지 않게 한다.
	for(int i=1; i<=servoCnt; ++i){
		sts = sscServoOn(board_id, channel_id, i);
		if(sts != SSC_OK){
			WriteLog(sts, "[" + IntToStr(i) + "] Servo ON COMMAND");
			mr2.servo[i] = SSC_BIT_OFF;
			continue;
		}
		MainForm->memoRobostarLineAdd("[" + IntToStr(i) + "] Servo ON 명령 접수");
	}

	for(int i=1; i<=servoCnt; ++i){
		//* 앰프 초기 통신(Ab -> Cxx/dxx)에 필요한 시간을 고려하여 최대 10초 대기한다.
		int readySts = sscWaitStatusBitSignalEx(board_id, channel_id, i,
			SSC_STSBIT_AX_RDY, SSC_BIT_ON, 10000);
		int ready = SSC_BIT_OFF;
		int readSts = sscGetStatusBitSignalEx(board_id, channel_id, i,
			SSC_STSBIT_AX_RDY, &ready);
		bool servoReadyOn = readySts == SSC_OK && readSts == SSC_OK &&
			ready == SSC_BIT_ON;
		mr2.servo[i] = servoReadyOn ? SSC_BIT_ON : SSC_BIT_OFF;

		if(servoReadyOn)
			MainForm->memoRobostarLineAdd("[" + IntToStr(i) + "] Servo ON 확인 성공 (RDY=1)");
		else
			MainForm->memoRobostarLineAdd("[" + IntToStr(i) + "] Servo ON 확인 실패 (RDY=" +
				IntToStr(ready) + ", WAIT=" + IntToHex(readySts, 8) +
				", READ=" + IntToHex(readSts, 8) + ")");
	}
	InitSequence(seqIdle);
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::ServoOff()
{
	int sts = 0;
	for(int i=1; i<=servoCnt; ++i){
		sts = sscServoOff(board_id, channel_id, i);
		WriteLog(sts, "[" + IntToStr(i) +  "] 서보 OFF");
		//* 전용 Servo OFF API의 완료 상태를 확인하여 화면에 즉시 반영한다.
		if(sts == SSC_OK){
			int offSts = sscCheckServoOff(board_id, channel_id, i);
			WriteLog(offSts, "[" + IntToStr(i) + "] Servo OFF CHECK");
			if(offSts == SSC_OK)
				mr2.servo[i] = SSC_BIT_OFF;
		}
	}
	InitSequence(seqIdle);
}
//---------------------------------------------------------------------------

void __fastcall Trobostar::MoveJog(int ntype)
{
	long speed_id = jogSpeed;
	short actime_id = 300;
	short dctime_id = 300;
	int axnum_id = 0;
	int sts = 0;
	//SSC_DIR_PLUS
	//SSC_DIR_MINUS

	switch(ntype){
		case 0:
			axnum_id = Axis_x;
			sts = sscJogStart(board_id, channel_id, axnum_id, speed_id, actime_id, dctime_id, SSC_DIR_PLUS);
			WriteLog(sts, "[" + IntToStr(axnum_id) +  "] JOG PLUS");
			break;
		case 1:
			axnum_id = Axis_x;
			sts = sscJogStart(board_id, channel_id, axnum_id, speed_id, actime_id, dctime_id, SSC_DIR_MINUS);
			WriteLog(sts, "[" + IntToStr(axnum_id) +  "] JOG MINUS");
			break;
		case 2:
			axnum_id = Axis_y;
			sts = sscJogStart(board_id, channel_id, axnum_id, speed_id, actime_id, dctime_id, SSC_DIR_PLUS);
			WriteLog(sts, "[" + IntToStr(axnum_id) +  "] JOG PLUS");
			break;
		case 3:
			axnum_id = Axis_y;
			sts = sscJogStart(board_id, channel_id, axnum_id, speed_id, actime_id, dctime_id, SSC_DIR_MINUS);
			WriteLog(sts, "[" + IntToStr(axnum_id) +  "] JOG MINUS");
			break;
		case 4:
			axnum_id = Axis_z;
			sts = sscJogStart(board_id, channel_id, axnum_id, speed_id, actime_id, dctime_id, SSC_DIR_PLUS);
			WriteLog(sts, "[" + IntToStr(axnum_id) +  "] JOG PLUS");
			break;
		case 5:
			axnum_id = Axis_z;
			sts = sscJogStart(board_id, channel_id, axnum_id, speed_id, actime_id, dctime_id, SSC_DIR_MINUS);
			WriteLog(sts, "[" + IntToStr(axnum_id) +  "] JOG MINUS");
			break;
		default:
			sts = sscJogStop(board_id, channel_id, Axis_x);
			sts = sscJogStop(board_id, channel_id, Axis_y);
			sts = sscJogStop(board_id, channel_id, Axis_z);
    		WriteLog(sts, "[" + IntToStr(axnum_id) +  "] JOG STOP");
			break;
	}
    InitSequence(seqIdle);
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::SetJogSpeed(int speed)
{
	if(speed < 1 || speed > 200)
		return false;

	jogSpeed = speed;
	return true;
}
//---------------------------------------------------------------------------
int __fastcall Trobostar::GetJogSpeed() const
{
	return jogSpeed;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::IsZLimitActive() const
{
	return !(mr2.limit[Axis_z] & SSC_BIT_LSP) ||
		!(mr2.limit[Axis_z] & SSC_BIT_LSN);
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::IsZDownLimitRecoveryAllowed() const
{
	bool positiveLimitActive = !(mr2.limit[Axis_z] & SSC_BIT_LSP);
	bool negativeLimitActive = !(mr2.limit[Axis_z] & SSC_BIT_LSN);

	// On this machine Z DOWN is PLUS. It is the escape direction only from
	// the upper/minus (LSN) limit. Never override LSP or simultaneous limits.
	return negativeLimitActive && !positiveLimitActive;
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::Reset()
{
	int sts = 0;
	sts = sscResetAlarm(board_id, channel_id, 0, SSC_ALARM_SYSTEM);
	WriteLog(sts, "[0] SSC_ALARM_SYSTEM RESET");
	for(int i = 1; i <= servoCnt; ++i){
		sts = sscResetAlarm(board_id, channel_id, i, SSC_ALARM_SERVO);
		WriteLog(sts, "[" + IntToStr(i) +  "] SSC_ALARM_SERVO RESET");
		sts = sscResetAlarm(board_id, channel_id, i, SSC_ALARM_OPERATION);
		WriteLog(sts, "[" + IntToStr(i) +  "] SSC_ALARM_OPERATION RESET");
	}
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::setPoint(int axnum_id, unsigned long int pos)
{
	bool pointAccepted = false;
	bool moveAccepted = false;
	int sts = SSC_OK;

	if(axnum_id == Axis_zUp){
		sts = sscSetPointDataEx(board_id, channel_id, Axis_z, 0, &point[0]);
		pointAccepted = WriteLog(sts, "[" + IntToStr(Axis_z) +  "] POINT");
		sts = sscAutoStart(board_id, channel_id, Axis_z, 0, 0);
		moveAccepted = WriteLog(sts, "[" + IntToStr(Axis_z) +  "] MOVE");
	}else{
		point[axnum_id].position = pos;
		sts = sscSetPointDataEx(board_id, channel_id, axnum_id, 0, &point[axnum_id]);
		pointAccepted = WriteLog(sts, "[" + IntToStr(axnum_id) +  "] POINT");
		sts = sscAutoStart(board_id, channel_id, axnum_id, 0, 0);
		moveAccepted = WriteLog(sts, "[" + IntToStr(axnum_id) +  "] MOVE");
	}

	return pointAccepted && moveAccepted;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::rangeCheck(int axnum_id)
{
	if(axnum_id == Axis_zUp){
		if(mr2.pos[Axis_z] == 0){
			step.step += 1;
			return true;
		}
	}else{
		if(mr2.pos[axnum_id] == point[axnum_id].position){
			step.step += 1;
			return true;
		}
	}
    return false;
}

//---------------------------------------------------------------------------
bool __fastcall Trobostar::CheckTrayCenteringMotionInterlock()
{
	// Production pickup/insert always requires its tray centering contact.
	// WAIT POSITION keeps the existing centering latch. MANUAL HOME is allowed
	// without tray centering only when CC-Link confirms X0022 ON (no cell).
	bool ejectMotion = seq == seqAutoEject ||
		(seq == seqAutoMove && step.reserve == seqAutoEject);
	bool insertMotion = seq == seqAutoInsert ||
		(seq == seqAutoMove && step.reserve == seqAutoInsert);
	bool waitMotion = seq == seqWait;
	bool homeMotion = seq == seqHome;
	//* MANUAL HOME INTERLOCK: X0022 ON is the physical no-cell confirmation.
	bool manualHomeCellClear = homeMotion && MainForm != NULL &&
		MainForm->equipMode == modeManual && IsCcLinkReady() &&
		input.GRIPPER1_CELL_DETECT;
	bool generalMotion = waitMotion || (homeMotion && !manualHomeCellClear);
	bool dryRunActive = DryRunForm != NULL && DryRunForm->IsRunning();
	bool directSourceMove = !dryRunActive && seq == seqAutoMove &&
		step.reserve == seqIdle && activeMoveValid && activeMove.pallet == 1;
	bool directTargetMove = !dryRunActive && seq == seqAutoMove &&
		step.reserve == seqIdle && activeMoveValid && activeMove.pallet == 2;
	bool directManualMove = directSourceMove || directTargetMove;

	// Preserve the HOME/WAIT latch while paused. Restart must re-enter step 0
	// with the same required tray signals instead of silently accepting a tray
	// whose Tray-In and Centering signals both disappeared after the stop.
	if(seq == seqPause && centeringMotionMonitorActive)
		return true;

	if(!ejectMotion && !insertMotion && !generalMotion && !directManualMove){
		centeringMotionMonitorActive = false;
		centeringMotionMonitorSeq = seqIdle;
		centeringRequireSource = false;
		centeringRequireTarget = false;
		return true;
	}

	bool plcFresh = PlcBin != NULL && PlcBin->IsPlcStatusFresh(1000);
	if(!centeringMotionMonitorActive || centeringMotionMonitorSeq != seq){
		centeringMotionMonitorActive = true;
		centeringMotionMonitorSeq = seq;
		centeringRequireSource = ejectMotion || directSourceMove;
		centeringRequireTarget = insertMotion || directTargetMove;
	}

	if(generalMotion && plcFresh){
		// Latch upward only. Once a tray needs collision protection, dropping
		// Tray-In together with Centering cannot cancel the interlock.
		centeringRequireSource = centeringRequireSource ||
			PlcBin->IsSourceTrayIn() || PlcBin->IsSourceCentering();
		centeringRequireTarget = centeringRequireTarget ||
			PlcBin->IsTargetTrayIn() || PlcBin->IsTargetCentering();
	}

	bool sourceCentered = plcFresh && PlcBin->IsSourceCentering();
	bool targetCentered = plcFresh && PlcBin->IsTargetCentering();
	bool sourceOkay = !centeringRequireSource || sourceCentered;
	bool targetOkay = !centeringRequireTarget || targetCentered;
	if(plcFresh && sourceOkay && targetOkay)
		return true;

	robotSequence interruptedSeq = seq;
	int interruptedStep = step.step;
	int toolNo = activeMove.tool;
	if(toolNo < 1 || toolNo > gripCnt)
		toolNo = move.tool >= 1 && move.tool <= gripCnt ? move.tool : 1;

	AnsiString signalName;
	if(!plcFresh)
		signalName = "PLC D10100-D10106 status";
	else if(!sourceOkay && !targetOkay)
		signalName = "D10104/D10106 Source/Target Centering";
	else if(!sourceOkay)
		signalName = "D10104 Source Centering";
	else
		signalName = "D10106 Target Centering";

	AnsiString motionName = ejectMotion ? "EJECT" :
		(insertMotion ? "INSERT" : (waitMotion ? "WAIT POSITION" :
		(homeMotion ? "HOME" : (directSourceMove ?
		"MANUAL SOURCE CHANNEL MOVE" : "MANUAL TARGET CHANNEL MOVE"))));
	AnsiString recoveryAction = directManualMove ?
		"Correct the centering condition, close this error, and click the channel again." :
		"Correct the centering condition and use the operator Restart control; motion will restart with Z UP.";
	AnsiString detail = signalName +
		" became OFF or PLC data became stale during " + motionName +
		" servo motion. DriveStop was issued immediately. " + recoveryAction;

	MainForm->memoRobostarLineAdd("[CENTERING MOTION INTERLOCK] STOP - " + motionName +
		" / SIGNAL=" + signalName +
		" / PLC_FRESH=" + IntToStr(plcFresh ? 1 : 0) +
		" / REQUIRE_SRC/TGT=" + IntToStr(centeringRequireSource ? 1 : 0) + "/" +
		IntToStr(centeringRequireTarget ? 1 : 0) +
		" / CENTER_SRC/TGT=" + IntToStr(sourceCentered ? 1 : 0) + "/" +
		IntToStr(targetCentered ? 1 : 0) +
		" / ROBOT_SEQ=" + IntToStr((int)interruptedSeq) +
		" / STEP=" + IntToStr(interruptedStep) +
		" / XYZ=" + IntToStr((__int64)mr2.pos[Axis_x]) + "/" +
		IntToStr((__int64)mr2.pos[Axis_y]) + "/" +
		IntToStr((__int64)mr2.pos[Axis_z]));

	// DriveStop must precede Pause/error display. Pause alone does not stop a
	// position-board move that has already been issued.
	req_Stop();

	if(generalMotion || directManualMove){
		// Do not let a stopped position be interpreted as completed motion. Normal
		// WAIT/HOME Restart resumes step 0. A manual channel move is intentionally
		// discarded and must be requested again after centering is restored.
		seq_save = directManualMove ? seqIdle : interruptedSeq;
		step_save.step = 0;
		step_save.delay = 0;
		step_save.timeout = 0;
		step_save.reserve = seqIdle;
		seq = seqPause;
		pauseStatus = true;
		ShowCommonError(motionName + " stopped by centering interlock", detail);
	}else if(ejectMotion){
		if(ErrorForm_eject != NULL)
			ErrorForm_eject->ShowError("Source centering released during EJECT motion",
				detail, toolNo, 17);
		else
			ShowCommonError("Source centering released during EJECT motion", detail);
	}else{
		if(ErrorForm_insert != NULL)
			ErrorForm_insert->ShowError("Target centering released during INSERT motion",
				detail, toolNo, 21);
		else
			ShowCommonError("Target centering released during INSERT motion", detail);
	}
	return false;
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::AutoMove()
{
	AnsiString loadfactor = "", px = "", py = "", zpoint = "", msg1 = "", msg2 = "";
	UnicodeString msg = "";
	int moveStepNo = step.reserve == seqAutoEject ? 8 :
		(step.reserve == seqAutoInsert ? 10 : 0);
	int actionStepNo = step.reserve == seqAutoEject ? 9 :
		(step.reserve == seqAutoInsert ? 11 : 0);
	if(!MainForm->m_ServoOpen)
	{
		InitSequence(seqIdle);
		MainForm->memoRobostarLineAdd("Check the servo connection status.");
		AlarmForm->ShowError("The servo is not connected.", "Please check and restart.");
		return;
	}
	else if(!MainForm->m_ServoON)
	{
		InitSequence(seqIdle);
		MainForm->memoRobostarLineAdd("Check the servo ON status.");
		AlarmForm->ShowError("The servo is not turned ON.", "Please check and restart.");
		return;
	}
	else if(!MainForm->m_ServoHomeEmg)
	{
		InitSequence(seqIdle);
		MainForm->memoRobostarLineAdd("Please check servo origin-return status.");
		AlarmForm->ShowError("The servo is not HOME.", "Please check and restart.");
		return;
	}

	if(!activeMoveValid){
		MainForm->memoRobostarLineAdd("[Z DOWN INTERLOCK] No valid tray move snapshot. Motion stopped.");
		AlarmForm->ShowError("Servo move interlock", "No valid tray/channel move request. Select the channel again.");
		req_Stop();
		return;
	}

	switch(step.step){
		case 0: // Always confirm that Z is raised before any X/Y positioning move.
		{
			teachForm->pnlMovingAlarm->Visible = true;
			teachForm->pnlMovingAlarm->BringToFront();
			teachForm->pnlMovingAlarm2->Visible = true;
			teachForm->pnlMovingAlarm2->BringToFront();

			// EJECT/INSERT completion has already raised Z and confirmed position 0.
			// Refresh it once here and, when the fast option is enabled, start X/Y
			// immediately instead of issuing another no-op Z UP and waiting one scan.
			//* max speed mode - need remove
			if(UseFatOptimizeSequenceDelay() && sscOpened){
				long currentZ = mr2.pos[Axis_z];
				if(sscGetCurrentCmdPositionFast(board_id, channel_id, Axis_z, &currentZ) == SSC_OK)
					mr2.pos[Axis_z] = currentZ;
			}
			//* max speed mode - need remove
			if(UseFatOptimizeSequenceDelay() &&
				mr2.pos[Axis_z] == 0){
				setPoint(Axis_x, activeTarget[Axis_x]);
				setPoint(Axis_y, activeTarget[Axis_y]);
				step.step = 11;
				MainForm->memoRobostarLineAdd(
					"[FAST OPTION] Z already at 0 / start X/Y without redundant Z UP");
				MainForm->memoRobostarLineAdd("[MOVE] X/Y pallet=" + IntToStr(activeMove.pallet) +
					", channel=" + IntToStr(activeMove.channel) + ", target=" +
					IntToStr((__int64)activeTarget[Axis_x]) + "/" +
					IntToStr((__int64)activeTarget[Axis_y]));
				break;
			}

			zUpCount = 0;
			bSetPoint = setPoint(Axis_zUp, 0);
			MainForm->memoRobostarLineAdd("[MOVE ORDER] Z UP before X/Y");
			step.step = 1;
			break;
		}
		case 1:
			//* max speed mode - need remove
			if(UseFatOptimizeSequenceDelay() && sscOpened){
				long currentZ = mr2.pos[Axis_z];
				if(sscGetCurrentCmdPositionFast(board_id, channel_id, Axis_z, &currentZ) == SSC_OK)
					mr2.pos[Axis_z] = currentZ;
			}
			if(moveStepNo > 0)
				MainForm->SetProcessOperationStatus(moveStepNo, "Z UP BEFORE X/Y",
					"Servo Z position", "0", IntToStr((__int64)mr2.pos[Axis_z]));
			//* Z축 이동 확인. 20초 이내 0으로 이동하지 않으면
            zUpCount++;

			if(bSetPoint == false){
				MainForm->memoRobostarLineAdd("setPoint(Axis_zUp, 0) 에러");
				req_Stop();
				return;
			}

			if(zUpCount > 200){
				loadfactor = teachForm->lblLoadFactor3->Caption;
				zpoint = MainForm->pz->Caption;
				msg1 = "대기상태로 이동 후 다시 시작하세요. \r\n(부하율 : " + loadfactor	+ ", z축 위치 : " + zpoint + ")";
				MainForm->memoRobostarLineAdd("Z 축 이동실패" + msg1);
				AlarmForm->ShowError("Z 축 이동실패", msg1);
				zUpCount = 0;
				req_Stop();
				return;
			}
			if(!rangeCheck(Axis_zUp))
				break;
			step.step = 10;
			//* max speed mode - need remove
			if(!UseFatOptimizeSequenceDelay())
				break;
			MainForm->memoRobostarLineAdd(
				"[FAST OPTION] Z=0 confirmed / start X/Y in same scan");
			// fall through

		case 10:
			bSetPoint = false;
			setPoint(Axis_x, activeTarget[Axis_x]);
			setPoint(Axis_y, activeTarget[Axis_y]);
			step.step += 1;
			MainForm->memoRobostarLineAdd("[MOVE] X/Y pallet=" + IntToStr(activeMove.pallet) +
				", channel=" + IntToStr(activeMove.channel) + ", target=" +
				IntToStr((__int64)activeTarget[Axis_x]) + "/" +
				IntToStr((__int64)activeTarget[Axis_y]));
			break;
		case 11:
		{
			//* max speed mode - need remove
			if(UseFatOptimizeSequenceDelay() && sscOpened){
				long currentX = mr2.pos[Axis_x];
				long currentY = mr2.pos[Axis_y];
				if(sscGetCurrentCmdPositionFast(board_id, channel_id, Axis_x, &currentX) == SSC_OK)
					mr2.pos[Axis_x] = currentX;
				if(sscGetCurrentCmdPositionFast(board_id, channel_id, Axis_y, &currentY) == SSC_OK)
					mr2.pos[Axis_y] = currentY;
			}
			if(moveStepNo > 0)
				MainForm->SetProcessOperationStatus(moveStepNo, "MOVE TO TRAY CHANNEL",
					"Servo X position", IntToStr((__int64)activeTarget[Axis_x]),
					IntToStr((__int64)mr2.pos[Axis_x]));
			if(!rangeCheck(Axis_x) ||
				//* max speed mode - need remove
				!UseFatOptimizeSequenceDelay())
				break;
			MainForm->memoRobostarLineAdd(
				"[FAST OPTION] X complete / check Y in same scan");
		}
		// fall through
		case 12:
		{
			//* max speed mode - need remove
			if(UseFatOptimizeSequenceDelay() && sscOpened){
				long currentY = mr2.pos[Axis_y];
				if(sscGetCurrentCmdPositionFast(board_id, channel_id, Axis_y, &currentY) == SSC_OK)
					mr2.pos[Axis_y] = currentY;
			}
			if(moveStepNo > 0)
				MainForm->SetProcessOperationStatus(moveStepNo, "MOVE TO TRAY CHANNEL",
					"Servo Y position", IntToStr((__int64)activeTarget[Axis_y]),
					IntToStr((__int64)mr2.pos[Axis_y]));
			if(!rangeCheck(Axis_y))
				break;
			if(step.reserve == seqIdle){
				step.step = 15; // Teaching channel move ends after X/Y.
				break;
			}
			//* max speed mode - need remove
			if(!UseFatOptimizeSequenceDelay())
				break;
			MainForm->memoRobostarLineAdd(
				"[FAST OPTION] X/Y complete / start Z DOWN in same scan");
		}
		// fall through
		case 13:
		{
			// Recalculate the accepted pallet/tool/channel immediately before Z DOWN.
			// A changed teaching value, overwritten request, or X/Y mismatch blocks descent.
			if(!ValidateActiveMoveTarget()){
				MainForm->memoRobostarLineAdd(
					"[Z DOWN INTERLOCK] Target validation failed. request pallet/tool/channel=" +
					IntToStr(activeMove.pallet) + "/" + IntToStr(activeMove.tool) + "/" +
					IntToStr(activeMove.channel) + ", actual X/Y=" +
					IntToStr((__int64)mr2.pos[Axis_x]) + "/" + IntToStr((__int64)mr2.pos[Axis_y]) +
					", accepted X/Y=" + IntToStr((__int64)activeTarget[Axis_x]) + "/" +
					IntToStr((__int64)activeTarget[Axis_y]));
				AlarmForm->ShowError("Z DOWN blocked", "Tray/channel position changed or X/Y is not at the accepted target. Retry from Z UP.");
				req_Stop();
				return;
			}
			if(activeMove.pallet == 1 && getGripperChuckStatus()){
				MainForm->memoRobostarLineAdd("[Source Tray Z Down] blocked in automatic move: gripper is CHUCK");
				AlarmForm->ShowError("Source Tray Z Down Interlock", "Open the gripper before lowering Z at the Source Tray.");
				req_Stop();
				return;
			}
			// Show the physical cell action as soon as Z DOWN starts at the accepted channel.
			if(step.reserve == seqAutoEject){
				MainForm->CompleteProcessStep(8, "Source channel position complete");
				MainForm->BeginProcessStep(9, "Cell eject / Z DOWN / Gripper CHUCK");
			}else if(step.reserve == seqAutoInsert){
				MainForm->CompleteProcessStep(10, "Target channel position complete");
				MainForm->BeginProcessStep(11, "Cell insert / Z DOWN / Gripper UNCHUCK");
			}
			setPoint(Axis_z, activeTarget[Axis_z]);
			MainForm->memoRobostarLineAdd("[Z DOWN APPROVED] pallet=" + IntToStr(activeMove.pallet) +
				", channel=" + IntToStr(activeMove.channel) + ", X/Y/Z=" +
				IntToStr((__int64)activeTarget[Axis_x]) + "/" +
				IntToStr((__int64)activeTarget[Axis_y]) + "/" +
				IntToStr((__int64)activeTarget[Axis_z]));
			step.step += 1;
			break;
		}
		case 14:
			if(actionStepNo > 0)
				MainForm->SetProcessOperationStatus(actionStepNo, "Z DOWN TO TRAY",
					"Servo Z position", IntToStr((__int64)activeTarget[Axis_z]),
					IntToStr((__int64)mr2.pos[Axis_z]));

			// Refresh Z on every 100 ms motion scan while waiting for DOWN completion.
			// The general monitor refreshes each axis less frequently, which added latency.
			//* max speed mode - need remove
			if(UseFatOptimizeSequenceDelay() && sscOpened){
				long currentZ = mr2.pos[Axis_z];
				if(sscGetCurrentCmdPositionFast(board_id, channel_id, Axis_z, &currentZ) == SSC_OK)
					mr2.pos[Axis_z] = currentZ;
			}

			if(rangeCheck(Axis_z)){
				robotSequence completedReserve = step.reserve;
				move = activeMove;
				for(int i = 0; i < AxisCnt; ++i)
					point[i].position = activeTarget[i];
				directXYPositionReady = (completedReserve == seqIdle);
				teachForm->pnlMovingAlarm->Visible = false;
				teachForm->pnlMovingAlarm2->Visible = false;
				InitSequence(completedReserve);

				//* max speed mode - need remove
				if(UseFatOptimizeSequenceDelay()){
					MainForm->memoRobostarLineAdd("[FAST TRANSITION] Z DOWN complete / start gripper action immediately");
					// Run the first safe gripper step in this same scan instead of waiting
					// for another 100 ms timer event.
					if(completedReserve == seqAutoEject)
						AutoEject();
					else if(completedReserve == seqAutoInsert)
						AutoInsert();
				}
			}
			break;
		default:
		{
			robotSequence completedReserve = step.reserve;
			move = activeMove;
			for(int i = 0; i < AxisCnt; ++i)
				point[i].position = activeTarget[i];
			directXYPositionReady = (completedReserve == seqIdle);
			MainForm->memoRobostarLineAdd("[FINISH] MOVE COMPLETE pallet=" +
				IntToStr(activeMove.pallet) + ", channel=" + IntToStr(activeMove.channel));

			teachForm->pnlMovingAlarm->Visible = false;
			teachForm->pnlMovingAlarm2->Visible = false;
			InitSequence(completedReserve);
			break;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::WaitPosition()
{
	if(!MainForm->m_ServoOpen)
	{
		InitSequence(seqIdle);
		MainForm->memoRobostarLineAdd("Check the servo connection status.");
		AlarmForm->ShowError("The servo is not connected.", "Please check and restart.");
		return;
	}
	else if(!MainForm->m_ServoON)
	{
		InitSequence(seqIdle);
		MainForm->memoRobostarLineAdd("Check the servo ON status.");
		AlarmForm->ShowError("The servo is not turned ON.", "Please check and restart.");
		return;
	}
	else if(!MainForm->m_ServoHomeEmg)
	{
		InitSequence(seqIdle);
		MainForm->memoRobostarLineAdd("Please check servo origin-return status.");
		AlarmForm->ShowError("The servo is not HOME.", "Please check and restart.");
		return;
	}

	switch(step.step){
		case 0: // Z축 상승
			zUpCount = 0;
			teachForm->pnlMovingAlarm->Visible = true;
			teachForm->pnlMovingAlarm->BringToFront();
			teachForm->pnlMovingAlarm2->Visible = true;
			teachForm->pnlMovingAlarm2->BringToFront();
			setPoint(Axis_zUp, 0);
			step.step += 1;
			break;
		case 1:
			zUpCount++;
			MainForm->SetProcessOperationStatus(13, "MOVE TO WAIT POSITION",
				"Servo Z position", "0", IntToStr((__int64)mr2.pos[Axis_z]));
			if(rangeCheck(Axis_zUp)){
				zUpCount = 0;
				break;
			}
			if(zUpCount > 200){
				MainForm->memoRobostarLineAdd("WaitPosition Z Axis Up timeout");
				AlarmForm->ShowError("Z 축 이동실패", "Z축 상승 완료 신호를 확인하세요.");
				req_Stop();
				return;
			}
			break;
		case 2:
			setPoint(Axis_x, Wait_xAxis);   // x 위치 이동
			setPoint(Axis_y, Wait_yAxis);   // y 위치이동
			step.step += 1;
			MainForm->memoRobostarLineAdd("[MOVE] X and Y");
			break;
		case 3:
			MainForm->SetProcessOperationStatus(13, "MOVE TO WAIT POSITION",
				"Servo X position", IntToStr((__int64)Wait_xAxis),
				IntToStr((__int64)mr2.pos[Axis_x]));
			rangeCheck(Axis_x);
			break;
		case 4:
			MainForm->SetProcessOperationStatus(13, "MOVE TO WAIT POSITION",
				"Servo Y position", IntToStr((__int64)Wait_yAxis),
				IntToStr((__int64)mr2.pos[Axis_y]));
			rangeCheck(Axis_y);
			break;
		case 5:
			MainForm->SetProcessOperationStatus(13, "WAIT POSITION COMPLETE CHECK",
				"X0022 gripper cell clear", "1 (NO CELL)",
				IntToStr(input.GRIPPER1_CELL_DETECT ? 1 : 0));
            teachForm->pnlMovingAlarm->Visible = false;
			teachForm->pnlMovingAlarm2->Visible = false;
			if(getCellDetectStatus())
				step.step = 9;
			else
				step.step += 1;
			break;
		default:
			InitSequence(seqIdle);
			break;
	}
}
// ============================================================================
//* DRY RUN : Dedicated Z-up and X/Y HOME return. No X0022 or process STEP use.
// ============================================================================
void __fastcall Trobostar::DryRunWaitPosition()
{
	switch(step.step){
		case 0:
			zUpCount = 0;
			bSetPoint = setPoint(Axis_zUp, 0);
			if(!bSetPoint){
				MainForm->memoRobostarLineAdd("[DRY RUN] HOME return Z UP command failed");
				req_Stop();
				return;
			}
			step.step = 1;
			break;
		case 1:
			zUpCount++;
			if(rangeCheck(Axis_zUp)){
				zUpCount = 0;
				break;
			}
			if(zUpCount > 200){
				MainForm->memoRobostarLineAdd("[DRY RUN] HOME return Z UP timeout");
				req_Stop();
			}
			break;
		case 2:
		{
			bool xAccepted = setPoint(Axis_x, Wait_xAxis);
			bool yAccepted = setPoint(Axis_y, Wait_yAxis);
			if(!xAccepted || !yAccepted){
				MainForm->memoRobostarLineAdd("[DRY RUN] HOME return X/Y command failed");
				req_Stop();
				return;
			}
			step.step = 3;
			break;
		}
		case 3:
			rangeCheck(Axis_x);
			break;
		case 4:
			rangeCheck(Axis_y);
			break;
		default:
			MainForm->memoRobostarLineAdd("[DRY RUN] HOME return complete X/Y/Z=0");
			InitSequence(seqIdle);
			break;
	}
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::req_DryRunWaitPosition()
{
	//* DRY RUN : Request boundary keeps only servo/safety/motion interlocks.
	if(seq != seqIdle || MainForm == NULL || !MainForm->m_ServoOpen ||
		!MainForm->m_ServoON || !MainForm->m_ServoHomeEmg || !IsSafetyReady())
		return false;
	InitSequence(seqDryRunWait);
	return seq == seqDryRunWait;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall Trobostar::zUp()
{
	switch(step.step){
		case 0: // Z축 상승
			zUpCount = 0;
			bSetPoint = setPoint(Axis_zUp, 0);
			teachForm->pnlMovingAlarm->Visible = true;
			teachForm->pnlMovingAlarm->BringToFront();
			teachForm->pnlMovingAlarm2->Visible = true;
			teachForm->pnlMovingAlarm2->BringToFront();
			step.step += 1;
			break;
		case 1:
			if(!bSetPoint){
				MainForm->memoRobostarLineAdd("Z Axis Up command failed");
				req_Stop();
				return;
			}
			zUpCount++;
			if(rangeCheck(Axis_zUp)) break;
			if(zUpCount > 200){
				AlarmForm->ShowError("Z 축 이동실패", "Z축 상승 완료 신호를 확인하세요.");
				req_Stop();
				return;
			}
			break;
		case 2:
			InitSequence(seqIdle);
    		break;
	}
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::zDown()
{
	switch(step.step){
		case 0:
			if(!directXYPositionReady || !ValidateActiveMoveTarget()){
				MainForm->memoRobostarLineAdd("[Z Axis Teaching Down] blocked at execution: accepted X/Y target is no longer valid");
				InitSequence(seqIdle);
				return;
			}
			if(activeMove.pallet == 1 && getGripperChuckStatus()){
				MainForm->memoRobostarLineAdd("[Source Tray Z Down] blocked at execution: gripper is CHUCK");
				InitSequence(seqIdle);
				return;
			}
			zUpCount = 0;
			bSetPoint = setPoint(Axis_z, activeTarget[Axis_z]);
			teachForm->pnlMovingAlarm->Visible = true;
			teachForm->pnlMovingAlarm->BringToFront();
			teachForm->pnlMovingAlarm2->Visible = true;
			teachForm->pnlMovingAlarm2->BringToFront();
			step.step += 1;
			break;
		case 1:
			if(!bSetPoint){
				MainForm->memoRobostarLineAdd("Z Axis Teaching Down command failed");
				req_Stop();
				return;
			}
			zUpCount++;
			if(rangeCheck(Axis_z)) break;
			if(zUpCount > 200){
				AlarmForm->ShowError("Z Axis move timeout", "Check the Z teaching value and servo state.");
				req_Stop();
				return;
			}
			break;
		case 2:
			directXYPositionReady = false;
			InitSequence(seqIdle);
			break;
	}
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::req_Init()
{
	// Validate at the button/request boundary so the operator sees the error immediately.
	if(!IsSafetyReady()){
		UnicodeString message = L"Servo Open blocked by safety I/O.";
		if(!input.SAFETY_EMG_READY)
			message += L"\r\nX002B SAFETY EMG READY: OFF";
		if(!input.SAFETY_DOOR_READY)
			message += L"\r\nX002C SAFETY DOOR READY: OFF";
		MainForm->memoRobostarLineAdd(message);
		ShowMessage(message);
		return;
	}

	InitSequence(seqInit);
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::req_Home()
{
	directXYPositionReady = false;
	InitSequence(seqHome);
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::req_ServoOn()
{
	InitSequence(seqServoOn);
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::req_ServoOff()
{
	// Servo OFF always requires a new home-return before AUTO/motion readiness.
	homeRequiredAfterServoOff = true;
	directXYPositionReady = false;
	if(MainForm != NULL){
		MainForm->m_ServoON = false;
		MainForm->m_ServoHome = false;
		MainForm->m_ServoHomeEmg = false;
		MainForm->memoRobostarLineAdd("[SERVO OFF] HOME state cleared; run HOME again after Servo ON.");
	}
	InitSequence(seqServoOff);
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::req_JogMove(int ntype)
{
	// Mouse-up JOG STOP must always be accepted, including during limit recovery.
	if(ntype < 0){
		InitSequence(seqJogStop);
		return;
	}

	//* Z LIMIT RECOVERY: While a Z limit is active, allow only DOWN(+) away
	// from the upper LSN limit. All other jog directions remain interlocked.
	if(IsZLimitActive()){
		if(ntype != 4 || !IsZDownLimitRecoveryAllowed()){
			MainForm->memoRobostarLineAdd(
				"[Z LIMIT RECOVERY] JOG blocked: only Z DOWN(+) is allowed while Z limit is active. "
				"LSP=" + IntToStr((mr2.limit[Axis_z] & SSC_BIT_LSP) ? 0 : 1) +
				" LSN=" + IntToStr((mr2.limit[Axis_z] & SSC_BIT_LSN) ? 0 : 1));
			return;
		}

		MainForm->memoRobostarLineAdd(
			"[Z LIMIT RECOVERY] Z DOWN(+) jog allowed. Release the button after LSN clears, then run HOME.");
		InitSequence(seqJOGz_Plus);
		return;
	}

	if(ntype >= 0 && ntype <= 3)
		directXYPositionReady = false;
	// Tag 4 is Z DOWN(+). Source Tray must never descend while the gripper is CHUCK.
	if(ntype == 4 && move.pallet == 1 && getGripperChuckStatus()){
		MainForm->memoRobostarLineAdd("[Source Tray Z Down] blocked: gripper is CHUCK");
		return;
	}

	switch(ntype){
		case 0: InitSequence(seqJOGx_Plus);break;
		case 1: InitSequence(seqJOGx_Minus);break;
		case 2: InitSequence(seqJOGy_Plus);break;
		case 3: InitSequence(seqJOGy_Minus);break;
		case 4: InitSequence(seqJOGz_Plus);break;
		case 5: InitSequence(seqJOGz_Minus);break;
		default: InitSequence(seqJogStop);break;
	}
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::req_Reset()
{
	InitSequence(seqReset);
}
//---------------------------------------------------------------------------
/*
* 채널간 거리 : 45,000
* 그리퍼간 거리 : 90,000
* 대상 트레이 채널 간 거리 : 45,000
*/
//---------------------------------------------------------------------------
bool __fastcall Trobostar::CalculatePositionValue(int pallet, int tool, int channel,
	long &x, long &y, long &z)
{
	if(channel < 1 || channel > TraySlotCount)
		return false;
	if(tool < 1 || tool > gripCnt)
		return false;
	if(pallet != 1 && pallet != 2)
		return false;

	TrayAxisEdit xEditType = pallet == 1 ? asSourceX : asTargetX;
	TrayAxisEdit yEditType = pallet == 1 ? asSourceY : asTargetY;
	TEdit* xEdit = teachForm->GetTrayEdit(channel, xEditType);
	TEdit* yEdit = teachForm->GetTrayEdit(channel, yEditType);
	TEdit* zEdit = pallet == 1 ? teachForm->edit_SZ : teachForm->edit_TZ;
	int baseX = 0;
	int baseY = 0;
	int zPosition = 0;

	if(xEdit == NULL || yEdit == NULL || zEdit == NULL)
		return false;
	if(!TryStrToInt(xEdit->Text.Trim(), baseX)
		|| !TryStrToInt(yEdit->Text.Trim(), baseY)
		|| !TryStrToInt(zEdit->Text.Trim(), zPosition))
		return false;

	long channelOffset = ((channel - 1) % TrayTeachingGroupSize) * (long)TrayCellPitch;
	long toolOffset = (tool - 1) * 90000L;
	x = (long)baseX - toolOffset;
	y = (long)baseY + channelOffset;
	z = zPosition;
	return true;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::SetPositionValue()
{
	long x = 0;
	long y = 0;
	long z = 0;
	if(!CalculatePositionValue(move.pallet, move.tool, move.channel, x, y, z))
		return false;

	point[Axis_x].position = x;
	point[Axis_y].position = y;
	point[Axis_z].position = z;
	return true;
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::CaptureMoveRequest()
{
	activeMove = move;
	for(int i = 0; i < AxisCnt; ++i)
		activeTarget[i] = point[i].position;
	activeMoveValid = true;
	directXYPositionReady = false;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::ValidateActiveMoveTarget()
{
	long expectedX = 0;
	long expectedY = 0;
	long expectedZ = 0;
	if(!activeMoveValid ||
		!CalculatePositionValue(activeMove.pallet, activeMove.tool, activeMove.channel,
			expectedX, expectedY, expectedZ))
		return false;

	if(move.pallet != activeMove.pallet || move.tool != activeMove.tool ||
		move.channel != activeMove.channel || move.type != activeMove.type)
		return false;

	if(activeTarget[Axis_x] != expectedX || activeTarget[Axis_y] != expectedY ||
		activeTarget[Axis_z] != expectedZ)
		return false;

	return mr2.pos[Axis_x] == activeTarget[Axis_x]
		&& mr2.pos[Axis_y] == activeTarget[Axis_y];
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::req_AutoRun()
{
	InitSequence(seqAutoRun);
}
void __fastcall Trobostar::req_EmgAutoRun()
{
    InitSequence(seqEmgAutoRun);
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::req_AutoMove(int pallet, int tool, int channel, int type)
{
	bSetPoint = false;
	if(!MainForm->m_ServoOpen || !MainForm->m_ServoON || !MainForm->m_ServoHomeEmg){
		UnicodeString message = L"Channel move is not ready.";
		if(!MainForm->m_ServoOpen) message += L"\r\nServo Open: OFF";
		if(!MainForm->m_ServoON) message += L"\r\nServo ON: OFF";
		if(!MainForm->m_ServoHomeEmg) message += L"\r\nOrigin return: NOT COMPLETE";
		MainForm->memoRobostarLineAdd(message);
		ShowMessage(message);
		return;
	}

	if(seq != seqIdle && seq != seqPause &&
		seq != seqAutoEjectComplete && seq != seqAutoInsertComplete){
		ShowMessage(L"Another servo sequence is running.");
		return;
	}
	if(seq == seqPause)
		req_Stop(); // Discard the paused automatic step before manual positioning.

	move.pallet = pallet;
	move.type = type;
	move.tool = tool;
	move.channel = channel;
	move.cnt = 0;
	if(!SetPositionValue()){
		MainForm->memoMainLineAdd("[Robot] Invalid tray move request.");
		ShowMessage(L"Invalid teaching value or channel.");
		return;
	}
	CaptureMoveRequest();
	// A new manual channel click is a new recovery transaction. Do not reuse the
	// latched Source/Target requirement from a previously stopped direct move.
	centeringMotionMonitorActive = false;

	MainForm->memoRobostarLineAdd("[CHANNEL MOVE] tray=" + IntToStr(pallet) +
		", tool=" + IntToStr(tool) + ", channel=" + IntToStr(channel) +
		", target X/Y/Z=" + IntToStr((__int64)activeTarget[Axis_x]) + "/" +
		IntToStr((__int64)activeTarget[Axis_y]) + "/" +
		IntToStr((__int64)activeTarget[Axis_z]));
	InitSequence(seqAutoMove, seqIdle); // Direct channel selection moves X/Y only.
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::req_AutoEject(int pallet, int tool, int channel, int cnt, int type)
{
	if(seq != seqIdle && seq != seqPause &&
		seq != seqAutoEjectComplete && seq != seqAutoInsertComplete){
		MainForm->memoRobostarLineAdd("[AUTO EJECT] blocked: another servo sequence is running");
		return;
	}
	if(seq == seqPause)
		req_Stop();

	move.pallet = pallet;
	move.type = type;
	move.tool = tool;
	move.channel = channel;
	move.cnt = cnt;
	if(!SetPositionValue()){
		MainForm->memoMainLineAdd("[Robot] Invalid eject position request.");
		return;
	}
	CaptureMoveRequest();
	MainForm->memoRobostarLineAdd("[AUTO EJECT REQUEST] pallet/tool/channel=" +
		IntToStr(pallet) + "/" + IntToStr(tool) + "/" + IntToStr(channel) +
		", X/Y/Z=" + IntToStr((__int64)activeTarget[Axis_x]) + "/" +
		IntToStr((__int64)activeTarget[Axis_y]) + "/" + IntToStr((__int64)activeTarget[Axis_z]));
	InitSequence(seqAutoMove, seqAutoEject);
	//* max speed mode - need remove
	if(UseFatOptimizeSequenceDelay() &&
		CheckTrayCenteringMotionInterlock()){
		MainForm->memoRobostarLineAdd(
			"[FAST OPTION] Start Source move immediately in request call");
		AutoMove();
	}
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::req_AutoInsert(int pallet, int tool, int channel, int cnt, int type)
{
	if(seq != seqIdle && seq != seqPause &&
		seq != seqAutoEjectComplete && seq != seqAutoInsertComplete){
		MainForm->memoRobostarLineAdd("[AUTO INSERT] blocked: another servo sequence is running");
		return;
	}
	if(seq == seqPause)
		req_Stop();

	move.pallet = pallet;
	move.type = type;
	move.tool = tool;
	move.channel = channel;
	move.cnt = cnt;
	if(!SetPositionValue()){
		MainForm->memoMainLineAdd("[Robot] Invalid insert position request.");
		return;
	}
	CaptureMoveRequest();
	MainForm->memoRobostarLineAdd("[AUTO INSERT REQUEST] pallet/tool/channel=" +
		IntToStr(pallet) + "/" + IntToStr(tool) + "/" + IntToStr(channel) +
		", X/Y/Z=" + IntToStr((__int64)activeTarget[Axis_x]) + "/" +
		IntToStr((__int64)activeTarget[Axis_y]) + "/" + IntToStr((__int64)activeTarget[Axis_z]));
	InitSequence(seqAutoMove, seqAutoInsert);
	//* max speed mode - need remove
	if(UseFatOptimizeSequenceDelay() &&
		CheckTrayCenteringMotionInterlock()){
		MainForm->memoRobostarLineAdd(
			"[FAST OPTION] Start Target move immediately in request call");
		AutoMove();
	}
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::req_WaitPosition()
{
	directXYPositionReady = false;
	InitSequence(seqWait);
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::req_zUp()
{
	InitSequence(seqZup);
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::req_zDown()
{
	if(seq != seqIdle && seq != seqPause){
		MainForm->memoRobostarLineAdd("[Z Axis Teaching Down] blocked: another servo sequence is running");
		return false;
	}
	if(!MainForm->m_ServoOpen || !MainForm->m_ServoON){
		MainForm->memoRobostarLineAdd("[Z Axis Teaching Down] blocked: Servo Open/ON is not ready");
		return false;
	}
	if(!directXYPositionReady || !activeMoveValid){
		MainForm->memoRobostarLineAdd("[Z Axis Teaching Down] blocked: move to the selected channel first");
		return false;
	}
	if(activeMove.pallet == 1 && getGripperChuckStatus()){
		MainForm->memoRobostarLineAdd("[Source Tray Z Down] blocked: gripper is CHUCK");
		return false;
	}
	if(!ValidateActiveMoveTarget()){
		MainForm->memoRobostarLineAdd("[Z Axis Teaching Down] blocked: request/teaching/X/Y changed. actual=" +
			IntToStr((__int64)mr2.pos[Axis_x]) + "/" + IntToStr((__int64)mr2.pos[Axis_y]) +
			", accepted=" + IntToStr((__int64)activeTarget[Axis_x]) + "/" +
			IntToStr((__int64)activeTarget[Axis_y]));
		return false;
	}

	point[Axis_z].position = activeTarget[Axis_z];
	InitSequence(seqZdown);
	MainForm->memoRobostarLineAdd("[Z Axis Teaching Down] approved pallet/channel=" +
		IntToStr(activeMove.pallet) + "/" + IntToStr(activeMove.channel) + ", target Z=" +
		IntToStr((__int64)activeTarget[Axis_z]));
	return true;
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::req_Speed(int speed, int accl, int dccl)
{
	for(int i=0; i<=servoCnt; ++i){
		point[i].speed = speed;
		point[i].actime = accl;
		point[i].dctime = dccl;
		point[i].dwell = 0;
		point[i].subcmd = 0;
		point[i].s_curve = 0;
	}
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::req_Stop()
{
	int sts = 0;
	pauseStatus = false;
	seq_save = seqIdle;
	activeMoveValid = false;
	directXYPositionReady = false;
	InitSequence(seqIdle);
	if(!sscOpened) return;

	for(int i=1; i<=servoCnt; ++i){
		sts = sscDriveStop(board_id, channel_id, i, 0);
		WriteLog(sts, "DriveStop");
	}
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::GripperChuck(int num, bool open, bool close)
{
	switch(num){
		case 1:
			gripper.GRIPPER1_CHUCK = close;
			gripper.GRIPPER1_UNCHUCK = open;
			break;
	}
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::EmgAutoRun()
{
	int sts = 0;
	int bitInfo = 0;
	short emergencyStatus = 0;

	switch(step.step){
		case 0: //  서보 시스템 초기화
			if(!StartServoSystemFromParameterFile()){
				InitSequence(seqIdle);
				return;
			}
			step.step += 1;
			break;
		case 1: //  서보 ON
			if(!IsSafetyReady() ||
				sscGetEmgStatus(board_id, channel_id, &emergencyStatus) != SSC_OK ||
				emergencyStatus != 0)
			{
				MainForm->memoRobostarLineAdd("[Safety] Auto Servo ON blocked: X002B/X002C or EMIO is not ready.");
				InitSequence(seqIdle);
				return;
			}
			for(int i = 1; i <= servoCnt; ++i){
				sts = sscServoOn(board_id, channel_id, i);
				if(sts == SSC_OK)
					MainForm->memoRobostarLineAdd("[" + IntToStr(i) + "] Servo ON 명령 접수");
				else WriteLog(sts, "[" + IntToStr(i) + "] Servo ON COMMAND");
			}
			step.delay = 0;
			step.step += 1;
			break;
		case 2:
			if(step.delay >= 10) step.step += 1;
			else step.delay += 1;
			break;
		case 3:	// Z축 서보 HOME
			sts = sscHomeReturnStart(board_id, channel_id, Axis_z);
			WriteLog(sts, "Z Axis Servo Home - Request");
			step.step = 6;
			break;
		case 6: // Z축 완료 확인
			sts = sscGetStatusBitSignalEx(board_id, channel_id, Axis_z, SSC_STSBIT_AX_ZREQ, &bitInfo);
			if(bitInfo){
				MainForm->memoRobostarLineAdd("Z Axis Servo Home - Moving");
			}
			else{
				MainForm->memoRobostarLineAdd("Z Axis Servo Home - Complete");
				step.step += 1;
			}
			break;
		case 7: // X축 원점
			sts = sscHomeReturnStart(board_id, channel_id, Axis_x);
			WriteLog(sts, "X Axis Servo Home - Request");
			step.step += 1;
			break;
		case 8: // Y축 원점
			sts = sscHomeReturnStart(board_id, channel_id, Axis_y);
			WriteLog(sts, "Y Axis Servo Home - Request");
			step.step += 1;
			break;
		case 9:
			sts = sscGetStatusBitSignalEx(board_id, channel_id, Axis_y, SSC_STSBIT_AX_ZREQ, &bitInfo);
			if(bitInfo){
				MainForm->memoRobostarLineAdd("Y Axis Servo Home - Moving");
			}
			else{
				MainForm->memoRobostarLineAdd("Y Axis Servo Home - Complete");
				step.step += 1;
			}
			break;
		case 10:
			sts = sscGetStatusBitSignalEx(board_id, channel_id, Axis_x, SSC_STSBIT_AX_ZREQ, &bitInfo);
			if(bitInfo){
				MainForm->memoRobostarLineAdd("X Axis Servo Home - Moving");
			}
			else{
				MainForm->memoRobostarLineAdd("X Axis Servo Home - Complete");
				step.step += 1;
			}
			break;
		default:
			InitSequence(seqIdle);
			break;
	}
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::AutoRun()
{
	int sts = 0;
	int bitInfo = 0;
	short emergencyStatus = 0;
	switch(step.step){
		case 0: //  서보 시스템 초기화
			if(!StartServoSystemFromParameterFile()){
				InitSequence(seqIdle);
				return;
			}
			step.step += 1;
			break;
		case 1: //  서보 ON
			if(!IsSafetyReady() ||
				sscGetEmgStatus(board_id, channel_id, &emergencyStatus) != SSC_OK ||
				emergencyStatus != 0)
			{
				MainForm->memoRobostarLineAdd("[Safety] Auto Servo ON blocked: X002B/X002C or EMIO is not ready.");
				InitSequence(seqIdle);
				return;
			}
			for(int i = 1; i <= servoCnt; ++i){
				sts = sscServoOn(board_id, channel_id, i);
				if(sts == SSC_OK)
					MainForm->memoRobostarLineAdd("[" + IntToStr(i) + "] Servo ON 명령 접수");
				else WriteLog(sts, "[" + IntToStr(i) + "] Servo ON COMMAND");
			}
			step.delay = 0;
			step.step += 1;
			break;
		case 2:
			if(step.delay >= 10) step.step += 1;
			else step.delay += 1;
			break;
		case 3:	// Z축 서보 HOME
			sts = sscHomeReturnStart(board_id, channel_id, Axis_z);
			WriteLog(sts, "Z Axis Servo Home - Request");
			step.step = 6;
			break;
		case 6: // Z축 완료 확인
			sts = sscGetStatusBitSignalEx(board_id, channel_id, Axis_z, SSC_STSBIT_AX_ZREQ, &bitInfo);
			if(bitInfo){
				MainForm->memoRobostarLineAdd("Z Axis Servo Home - Moving");
			}
			else{
				MainForm->memoRobostarLineAdd("Z Axis Servo Home - Complete");
				step.step += 1;
			}
			break;
		case 7: // X축 원점
			sts = sscHomeReturnStart(board_id, channel_id, Axis_x);
			WriteLog(sts, "X Axis Servo Home - Request");
			step.step += 1;
			break;
		case 8: // Y축 원점
			sts = sscHomeReturnStart(board_id, channel_id, Axis_y);
			WriteLog(sts, "Y Axis Servo Home - Request");
			step.step += 1;
			break;
		case 9:
			sts = sscGetStatusBitSignalEx(board_id, channel_id, Axis_y, SSC_STSBIT_AX_ZREQ, &bitInfo);
			if(bitInfo){
				MainForm->memoRobostarLineAdd("Y Axis Servo Home - Moving");
			}
			else{
				MainForm->memoRobostarLineAdd("Y Axis Servo Home - Complete");
				step.step += 1;
			}
			break;
		case 10:
			sts = sscGetStatusBitSignalEx(board_id, channel_id, Axis_x, SSC_STSBIT_AX_ZREQ, &bitInfo);
			if(bitInfo){
				MainForm->memoRobostarLineAdd("X Axis Servo Home - Moving");
			}
			else{
				MainForm->memoRobostarLineAdd("X Axis Servo Home - Complete");
				step.step += 1;
			}
			break;
		case 11: // 2019 07 05 x, y, z 축 원점 이동 후 그리퍼에 셀이 있는지 확인
			if(getCellDetectStatus())
			{
				MainForm->memoRobostarLineAdd("[C_Maint] 그리퍼에 셀이 있습니다. 셀을 제거 후 원점으로 이동 해 주세요");
			}
			else
			{
				step.step += 1;
			}
			break;
		default:
			InitSequence(seqIdle);
			break;
	}
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::AutoEject()
{
	int nresult = 0;
	AnsiString msg;

	if(nresult == 0){
		msg = "Gripper #" + IntToStr(move.tool);

		switch(step.step){
			case 0:
			{
				// Normal mode evaluates each prerequisite in its own 100 ms scan.
				// Optimize safe transitions may continue through the same checks in
				// one scan, but it never treats a failed check as successful.
				MainForm->SetProcessOperationStatus(9, "CELL EJECT",
					"X0022 Source cell detect", "0 (CELL DETECTED)",
					IntToStr(input.GRIPPER1_CELL_DETECT ? 1 : 0));
				for(int i=0; i<move.cnt; ++i)
					nresult += CheckEjectCell_after(move.tool + i);
				if(nresult != move.cnt){
					step.timeout += 1;
					if(step.timeout == errCnt)
						ErrorForm_eject->ShowError(msg + " cannot detect the Source tray cell.",
							"Eject step 1. Source cell detect error", move.tool, 20);
					MainForm->memoRobostarLineAdd("Eject step 1. Waiting for Source cell detection");
					break;
				}
				step.timeout = 0;
				step.step = 1;
				//* max speed mode - need remove
				if(!UseFatOptimizeSequenceDelay()){
					MainForm->memoRobostarLineAdd("[NORMAL EJECT] Source cell confirmed / next scan checks OPEN");
					break;
				}
				MainForm->memoRobostarLineAdd("[FAST OPTION] Source cell confirmed / check OPEN in same scan");
			}
			// fall through only when Optimize safe transitions is enabled
			case 1:
			{
				nresult = 0;
				MainForm->SetProcessOperationStatus(9, "CELL EJECT",
					"X0021 Gripper OPEN", "1 (OPEN)",
					IntToStr(input.GRIPPER1_UNCHUCK ? 1 : 0));
				for(int i=0; i<move.cnt; ++i)
					nresult += CheckEjectUnchuck(move.tool + i);
				if(nresult != move.cnt){
					// Fast mode transmits the OPEN selection immediately. Normal mode
					// leaves transmission to the next periodic I/O scan.
					//* max speed mode - need remove
					if(UseFatOptimizeSequenceDelay())
						io_WriteGripper();
					step.timeout += 1;
					if(step.timeout == errCnt)
						ErrorForm_eject->ShowError("[B_Ignition] " + msg + " UNCHUCK sensor detection failed",
							"Eject step 2. Gripper open error", move.tool, 0);
					MainForm->memoRobostarLineAdd("[EJECT] Waiting for physical OPEN confirmation");
					break;
				}
				step.timeout = 0;
				step.step = 2;
				//* max speed mode - need remove
				if(!UseFatOptimizeSequenceDelay())
					break;
				MainForm->memoRobostarLineAdd("[FAST OPTION] OPEN confirmed / check Source centering in same scan");
			}
			// fall through only when Optimize safe transitions is enabled
			case 2:
			{
				bool sourceCenteringReady = PlcBin != NULL &&
					PlcBin->IsPlcStatusFresh(1000) && PlcBin->IsSourceCentering();
				MainForm->SetProcessOperationStatus(9, "CELL EJECT",
					"D10104 Source Centering", "1 (ON)", sourceCenteringReady ? "1" : "0");
				if(!sourceCenteringReady){
					CheckTrayCenteringMotionInterlock();
					return;
				}

				step.timeout = 0;
				step.step = 3;
				//* max speed mode - need remove
				if(UseFatOptimizeSequenceDelay()){
					// All prerequisites passed in the current scan. Send CHUCK now;
					// case 3 still waits for the physical X0020 confirmation.
					GripperChuck(move.tool, false, true);
					io_WriteGripper();
					MainForm->memoRobostarLineAdd("[FAST OPTION] Eject checks complete / CHUCK output sent immediately");
				}
				break;
			}
			case 3:
				MainForm->SetProcessOperationStatus(9, "CELL EJECT",
					"X0020 Gripper CHUCK", "1 (CLOSE)",
					IntToStr(input.GRIPPER1_CHUCK ? 1 : 0));
				for(int i=0; i<move.cnt; ++i)
					nresult += CheckEjectChuck(move.tool + i);
				MainForm->memoRobostarLineAdd("[C_Maint] 취출4. GRIPPER CHUCK");
				if(nresult != move.cnt)
					break;
				step.step = 4;
				//* max speed mode - need remove
				if(!UseFatSkipGripStabilization())
					break;
				// TEST option: physical CHUCK is confirmed; continue without stabilization.
				// fall through
			case 4:
				MainForm->SetProcessOperationStatus(9, "CELL EJECT",
					"Gripper CHUCK stabilization", "200 ms after sensor confirmation", IntToStr(step.delay));
				//* max speed mode - need remove
				if(!UseFatSkipGripStabilization() && step.delay < 1){
					step.delay += 1;
					MainForm->memoRobostarLineAdd("[C_Maint] 취출4. 척 안정화 대기");
					break;
				}
				//* max speed mode - need remove
				if(UseFatSkipGripStabilization())
					MainForm->memoRobostarLineAdd(
						"[TEST OPTION] CHUCK stabilization 0.2 s skipped");
				step.delay = 0;
				step.step = 6;
				//* max speed mode - need remove
				if(!UseFatOptimizeSequenceDelay())
					break;
				// The 0.2 s stabilization has completed; perform the cell check now.
				// fall through
			case 6:
			{
				MainForm->SetProcessOperationStatus(9, "CELL EJECT",
					"X0022 Cell held after CHUCK", "0 (CELL DETECTED)",
					IntToStr(input.GRIPPER1_CELL_DETECT ? 1 : 0));
				for(int i=0; i<move.cnt; ++i)
					nresult += CheckEjectCell_after(move.tool + i);
				if(nresult == move.cnt){
					bool commitOk = true;
					for(int i=0; i<move.cnt; ++i){
						if(::gripper == NULL || !::gripper->CommitEjectTrayState(move.tool + i))
							commitOk = false;
					}
					if(commitOk){
						step.step = 7;
						step.timeout = 0;
					}else{
						ErrorForm_eject->ShowError(msg + " Source tray state save failed.",
							"Eject local state commit error", move.tool, 20);
					}
				}else{
					step.timeout += 1;
					if(step.timeout == errCnt){
						ErrorForm_eject->ShowError(msg + " lost the cell after CHUCK.",
							"Eject cell confirmation error", move.tool, 20);
					}
					MainForm->memoRobostarLineAdd("Eject step 6. Waiting for cell confirmation after CHUCK");
				}
				if(step.step != 7) break;
			}
			//* max speed mode - need remove
			if(!UseFatOptimizeSequenceDelay())
				break;
			// Cell confirmation passed; issue Z UP in this same scan.
			// fall through
			case 7:
				zUpCount = 0;
				bSetPoint = setPoint(Axis_zUp, 0);
				if(!bSetPoint){
					ErrorForm_eject->ShowError(msg + " Z UP command failed.",
						"Eject Z UP command error", move.tool, 17);
				}else{
					MainForm->memoRobostarLineAdd("[EJECT] Z UP command / target=0");
					step.step = 8;
				}
				break;
			case 8:
			{
				MainForm->SetProcessOperationStatus(9, "CELL EJECT COMPLETE",
					"Servo Z position", "0", IntToStr((__int64)mr2.pos[Axis_z]));
				zUpCount += 1;

				// Refresh Z every 100 ms while waiting for UP completion. Once Z=0 is
				// confirmed, continue directly to the final cell check in this scan.
				//* max speed mode - need remove
				if(UseFatOptimizeSequenceDelay() && sscOpened){
					long currentZ = mr2.pos[Axis_z];
					if(sscGetCurrentCmdPositionFast(board_id, channel_id, Axis_z, &currentZ) == SSC_OK)
						mr2.pos[Axis_z] = currentZ;
				}
				if(!rangeCheck(Axis_zUp)){
					if(zUpCount > 200){
						zUpCount = 0;
						ErrorForm_eject->ShowError(msg + " Z UP timeout.",
							"Eject Z axis did not reach position 0", move.tool, 17);
					}
					break;
				}
				zUpCount = 0;
				step.timeout = 0;
				step.step = 9;
				//* max speed mode - need remove
				if(!UseFatOptimizeSequenceDelay())
					break;
				MainForm->memoRobostarLineAdd(
					"[FAST EJECT COMPLETE] Z=0 confirmed / check picked cell in same scan");
			}
			// fall through
			case 9:
				MainForm->SetProcessOperationStatus(9, "CELL EJECT COMPLETE CHECK",
					"X0022 Cell held at Z=0", "0 (CELL DETECTED)",
					IntToStr(input.GRIPPER1_CELL_DETECT ? 1 : 0));
				if(getCellDetectStatus()){
					step.timeout = 0;
					MainForm->memoRobostarLineAdd(
						"[FAST EJECT COMPLETE] Z=0 / X0022 OFF / target move ready");
					MainForm->CompleteProcessStep(9,
						"Cell pickup saved / Z position 0 / cell detected");
					InitSequence(seqAutoEjectComplete);
				}else{
					step.timeout += 1;
					if(step.timeout == errCnt){
						ErrorForm_eject->ShowError(
							msg + " cannot detect the picked cell after Z UP.",
							"Eject cell detect error at Z=0", move.tool, 20);
					}
					MainForm->memoRobostarLineAdd(
						"[EJECT] Z=0 / waiting X0022 OFF / no picked cell detected");
				}
				break;
			default:
				MainForm->CompleteProcessStep(9,
					"Cell pickup saved / Z position 0 / cell detected");
				InitSequence(seqAutoEjectComplete);
				break;
		}
	}
	else{
		msg = "그리퍼 #" + IntToStr(nresult);
		ErrorForm_eject->ShowError("[B_Ignition] " + msg + " 완충센서가 감지되었습니다.", msg + " Z축 이동 충돌 확인", move.tool, 17);

	}
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::AutoInsert()
{

	int nresult = 0;
	AnsiString msg;

	if(nresult == 0){
		msg = "그리퍼 #" + IntToStr(move.tool);

		switch(step.step){
			case 0:
			{
				// Normal mode checks the held cell and Target centering in separate
				// scans. The optimize option may combine the successful checks only.
				MainForm->SetProcessOperationStatus(11, "CELL INSERT",
					"X0022 Held cell detect", "0 (CELL DETECTED)",
					IntToStr(input.GRIPPER1_CELL_DETECT ? 1 : 0));
				for(int i=0; i<move.cnt; ++i)
					nresult += CheckEjectCell_after(move.tool + i);
				if(nresult != move.cnt){
					step.timeout += 1;
					if(step.timeout == errCnt)
						ErrorForm_insert->ShowError("[B_Ignition] " + msg + " has no cell.",
							"Insert step 1. Cell check error", move.tool, 23);
					MainForm->memoRobostarLineAdd("[INSERT] Waiting for held-cell confirmation");
					break;
				}
				step.timeout = 0;
				step.step = 1;
				//* max speed mode - need remove
				if(!UseFatOptimizeSequenceDelay())
					break;
				MainForm->memoRobostarLineAdd("[FAST OPTION] Held cell confirmed / check Target centering in same scan");
			}
			// fall through only when Optimize safe transitions is enabled
			case 1:
			{
				bool targetCenteringReady = PlcBin != NULL &&
					PlcBin->IsPlcStatusFresh(1000) && PlcBin->IsTargetCentering();
				MainForm->SetProcessOperationStatus(11, "CELL INSERT",
					"D10106 Target Centering", "1 (ON)", targetCenteringReady ? "1" : "0");
				if(!targetCenteringReady){
					CheckTrayCenteringMotionInterlock();
					return;
				}

				step.timeout = 0;
				step.step = 2;
				//* max speed mode - need remove
				if(UseFatOptimizeSequenceDelay()){
					// Send OPEN immediately after all checks pass. The following state
					// still requires the physical X0021 feedback before proceeding.
					GripperChuck(move.tool, true, false);
					io_WriteGripper();
					MainForm->memoRobostarLineAdd("[FAST OPTION] Insert checks complete / UNCHUCK output sent immediately");
				}
				break;
			}
			case 2:
				MainForm->SetProcessOperationStatus(11, "CELL INSERT",
					"X0021 Gripper OPEN", "1 (OPEN)",
					IntToStr(input.GRIPPER1_UNCHUCK ? 1 : 0));
				for(int i=0; i<move.cnt; ++i)
					nresult += CheckInsertUnchuck(move.tool + i);
				if(nresult != move.cnt){
					step.timeout += 1;
					if(step.timeout == errCnt){
						ErrorForm_insert->ShowError("[B_Ignition] " + msg + " UNCHUCK 센서 감지 실패", "이재 3단계. 그리퍼 열기 에러", move.tool, 0);
					}
					MainForm->memoRobostarLineAdd("[C_Maint] 삽입3. GRIPPER UNCHUCK");
					break;
				}
				step.step = 4;
				step.timeout = 0;
				//* max speed mode - need remove
				if(!UseFatSkipGripStabilization())
					break;
				// TEST option: physical OPEN is confirmed; continue without stabilization.
				// fall through
			case 4:
				MainForm->SetProcessOperationStatus(11, "CELL INSERT",
					"Gripper OPEN stabilization", "200 ms after sensor confirmation", IntToStr(step.delay));
				//* max speed mode - need remove
				if(!UseFatSkipGripStabilization() && step.delay < 1){
					step.delay += 1;
					MainForm->memoRobostarLineAdd("[INSERT] Gripper OPEN stabilization wait");
					break;
				}
				//* max speed mode - need remove
				if(UseFatSkipGripStabilization())
					MainForm->memoRobostarLineAdd(
						"[TEST OPTION] UNCHUCK stabilization 0.2 s skipped");
				step.delay = 0;
				step.step = 5;
				//* max speed mode - need remove
				if(!UseFatOptimizeSequenceDelay())
					break;
				// The 0.2 s stabilization has completed; issue Z UP in this same scan.
				// fall through
			case 5:
				// The tray cell can keep X0022 active while Z is down even after OPEN.
				// Raise the gripper first; cell-clear confirmation is performed at Z=0.
				zUpCount = 0;
				bSetPoint = setPoint(Axis_zUp, 0);
				if(!bSetPoint){
					ErrorForm_insert->ShowError(msg + " Z UP command failed.",
						"Insert Z UP command error", move.tool, 21);
				}else{
					MainForm->memoRobostarLineAdd("[INSERT] Gripper OPEN complete / Z UP command / target=0");
					step.step = 6;
				}
				break;
			case 6:
			{
				MainForm->SetProcessOperationStatus(11, "CELL INSERT",
					"Servo Z position after release", "0", IntToStr((__int64)mr2.pos[Axis_z]));
				zUpCount += 1;

				// Refresh Z every 100 ms while waiting for UP completion. Once Z=0 is
				// confirmed, continue directly to the cell-clear check in this scan.
				//* max speed mode - need remove
				if(UseFatOptimizeSequenceDelay() && sscOpened){
					long currentZ = mr2.pos[Axis_z];
					if(sscGetCurrentCmdPositionFast(board_id, channel_id, Axis_z, &currentZ) == SSC_OK)
						mr2.pos[Axis_z] = currentZ;
				}
				if(!rangeCheck(Axis_zUp)){
					if(zUpCount > 200){
						zUpCount = 0;
						ErrorForm_insert->ShowError(msg + " Z UP timeout.",
							"Insert Z axis did not reach position 0", move.tool, 21);
					}
					break;
				}
				zUpCount = 0;
				step.timeout = 0;
				step.step = 7;
				//* max speed mode - need remove
				if(!UseFatOptimizeSequenceDelay())
					break;
				MainForm->memoRobostarLineAdd(
					"[FAST INSERT COMPLETE] Z=0 confirmed / check cell clear in same scan");
			}
			// fall through
			case 7:
			{
				MainForm->SetProcessOperationStatus(11, "CELL INSERT COMPLETE CHECK",
					"X0022 Gripper cell clear", "1 (NO CELL)",
					IntToStr(input.GRIPPER1_CELL_DETECT ? 1 : 0));
				// X0022 is checked only after the gripper has reached Z=0.
				for(int i=0; i<move.cnt; ++i)
					nresult += CheckInsertCellReleased(move.tool + i);
				if(nresult == move.cnt){
					bool commitOk = true;
					for(int i=0; i<move.cnt; ++i){
						if(::gripper == NULL || !::gripper->CommitInsertTrayState(move.tool + i))
							commitOk = false;
					}
					if(commitOk){
						MainForm->memoRobostarLineAdd(
							"[FAST INSERT COMPLETE] Z=0 / X0022 ON / next action ready");
						step.timeout = 0;
						MainForm->CompleteProcessStep(11,
							"Cell insert saved / Z position 0 / cell clear confirmed");
						InitSequence(seqAutoInsertComplete);
					}else{
						ErrorForm_insert->ShowError(msg + " Target tray state save failed.",
							"Insert local state commit error", move.tool, 23);
					}
				}else{
					step.timeout += 1;
					if(step.timeout == errCnt){
						ErrorForm_insert->ShowError(msg + " still detects a cell after Z UP.",
							"Insert cell remains on gripper at Z=0", move.tool, 23);
					}
					MainForm->memoRobostarLineAdd(
						"[INSERT] Z=0 / waiting X0022 ON / gripper cell clear");
				}
				break;
			}
			default:
				MainForm->CompleteProcessStep(11, "Cell insert saved / Z position 0 / cell clear confirmed");
				InitSequence(seqAutoInsertComplete);
				break;
		}
	}else{
		msg = "그리퍼 #" + IntToStr(nresult);
		ErrorForm_insert->ShowError("[B_Ignition] " + msg + " 완충센서가 감지되었습니다.", msg + " Z축 이동 충돌 확인", move.tool, 21);

	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool __fastcall Trobostar::getCellDetectStatus()
{
	// X0022 is active-low and is meaningful only while CC-Link is ready.
	return IsCcLinkReady() && !input.GRIPPER1_CELL_DETECT;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::getCellDetectStatus(int pos)
{
	// FormTeaching uses gripper number 1; the legacy Door button uses tag 7.
	if(pos == 1 || pos == 7)
		return getCellDetectStatus();
	return false;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::getGripperChuckStatus()
{
	// X0020 is the physical CHUCK confirmation; Y0030 covers the command-to-feedback gap.
	return input.GRIPPER1_CHUCK || gripper.GRIPPER1_CHUCK;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::getGripperOpenStatus()
{
	// AUTO entry requires physical OPEN confirmation: X0021 ON and X0020 OFF.
	// The output command alone is not accepted as a completed interlock condition.
	return input.GRIPPER1_UNCHUCK && !input.GRIPPER1_CHUCK;
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::mr2Sensing()
{
	if(!sscOpened) return;

	static int seq = 0;
	UnicodeString msg;

	switch(seq){
		case 0:
			sscGetSystemStatusCode(board_id, channel_id, &mr2.system_status);
			sscGetAlarm(board_id, channel_id, 0, SSC_ALARM_SYSTEM, &mr2.system_alarm, &mr2.system_detail);
					// 서보 알람 정보
			for(int i=1; i<=servoCnt; ++i){
				sscGetAlarm(board_id, channel_id, i, SSC_ALARM_SERVO, &mr2.servo_alarm[i], &mr2.servo_detail[i]);
				sscGetAlarm(board_id, channel_id, i, SSC_ALARM_OPERATION, &mr2.oper_alarm[i], &mr2.oper_detail[i]);
			}
			break;
		case 1:
			for(int i=1; i<=servoCnt; ++i){
				sscGetCurrentCmdPositionFast(board_id, channel_id, i, &mr2.pos[i]);  // 현재위치 get
				sscGetCmdSpeedFast(board_id, channel_id, i, &mr2.speed[i]);            // 현재속도 get
				sscGetDriveFinStatus(board_id, channel_id, i, SSC_FIN_TYPE_INP, &mr2.status[i]);    		// 동작 상태
				sscGetIoStatusFast(board_id, channel_id, i, &mr2.limit[i]);
				sscGetMonitor(board_id, channel_id, i, &mr2.monnum[i][0], &mr2.mondata[i][0]);
			}
			break;
		case 2:
			//* 2026 08 07 천안 불량선별기와 동일하게 AX_RDY와 AX_ZREQ를 직접 갱신
			for(int i=1; i<=servoCnt; ++i){
				sscGetStatusBitSignalEx(board_id, channel_id, i,
					SSC_STSBIT_AX_RDY, &mr2.servo[i]);
				sscGetStatusBitSignalEx(board_id, channel_id, i, SSC_STSBIT_AX_ZREQ, &mr2.zero[i]);
				sscGetStatusBitSignalEx(board_id, channel_id, i, SSC_STSBIT_AX_OP, &mr2.running[i]);
			}
			seq = -1;
			break;
	}
    seq += 1;
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::senTimerTimer(TObject *Sender)
{

	MainForm->Caption = step.step;

	this->io_Read();

	// While the door recovery form is open, guide the operator to the physical
	// X0025 reset key whenever either safety-ready circuit is not established.
	// X0025 is the switch input; Y0035 is its OP box lamp output.
	gripper.OPBOX_RESET_LAMP = doorForm != NULL && doorForm->Visible
		&& (!input.SAFETY_EMG_READY || !input.SAFETY_DOOR_READY);

	// Y003C is normally OFF and mechanically locks the BYPASS key in ON position.
	// A KEYLOCK release/door opening always forces it OFF. A new hardware BYPASS-ON
	// transition also resets it OFF; only the UI command after confirmed KEYLOCK set
	// may turn it ON long enough to rotate the hardware key to OFF.
	bool bypassSwitchOn = IsBypassActive();
	if(input.SAFETY_DOOR_1 || input.SAFETY_DOOR_2
		|| keyLockReleasePending || !IsKeyLockActive()
		|| !bypassSwitchOn || (bypassSwitchOn && !previousBypassSwitchOn))
		Bypass(false);
	previousBypassSwitchOn = bypassSwitchOn;

	DWORD nowTick = GetTickCount();

	// A door may open during the non-blocking set delay. Cancel the set sequence
	// before Y003D can turn OFF: force Y003D ON first, then release Y0033/Y0034
	// after the same safety delay used by a normal KEYLOCK release.
	if(keyLockSetPending && !CanSetKeyLock()){
		Bypass(false);
		gripper.SAFETY_BYPASS_ON = true;
		keyLockSetPending = false;
		keyLockSetSafetyBypassOffTick = 0;
		keyLockReleasePending = true;
		io_WriteGripper();
		keyLockReleaseOutputOffTick = nowTick + KEYLOCK_OUTPUT_DELAY_MS;
		MainForm->memoRobostarLineAdd("[KEYLOCK] Set cancelled: X0026/X0027 door input ON; Y003D ON first.");
		ShowMessage(L"KEYLOCK setting was cancelled because Door #1 or Door #2 is open.\n\nX0026/X0027 must both be OFF.");
	}

	// KEYLOCK set: Y0033/Y0034 ON first, then Y003D OFF after the configured delay.
	if(keyLockSetPending && keyLockSetSafetyBypassOffTick != 0
		&& (LONG)(nowTick - keyLockSetSafetyBypassOffTick) >= 0){
		gripper.SAFETY_BYPASS_ON = false;
		keyLockSetSafetyBypassOffTick = 0;
		MainForm->memoRobostarLineAdd("[KEYLOCK] Y003D OFF (500ms after Y0033/Y0034 ON).");
	}

	// KEYLOCK release: Y003D ON first, then Y0033/Y0034 OFF after 500 ms.
	if(keyLockReleasePending && keyLockReleaseOutputOffTick != 0
		&& (LONG)(nowTick - keyLockReleaseOutputOffTick) >= 0){
		gripper.DOOR_LEFT_CLOSE = false;
		gripper.DOOR_RIGHT_CLOSE = false;
		keyLockReleaseOutputOffTick = 0;
		MainForm->memoRobostarLineAdd("[KEYLOCK] Y0033/Y0034 OFF (500ms after Y003D ON).");
	}

	// Complete each request only after its delayed output step and contact confirmation.
	if(keyLockSetPending && keyLockSetSafetyBypassOffTick == 0 && IsKeyLockActive()){
		keyLockSetPending = false;
		MainForm->memoRobostarLineAdd("[KEYLOCK] Set confirmed; Y003D OFF.");
	}
	if(keyLockReleasePending && keyLockReleaseOutputOffTick == 0
		&& input.SAFETY_DOOR_1 && input.SAFETY_DOOR_2){
		Bypass(false);
		keyLockReleasePending = false;
		MainForm->memoRobostarLineAdd("[KEYLOCK] Release confirmed; Y003D ON, Y003C OFF.");
	}
	bool softwareSafetyResetActive = IsSoftwareSafetyResetActive();
	bool softwareSafetyResetCompleted = safetyResetPulseUntilTick != 0
		&& !softwareSafetyResetActive;
	gripper.SAFETY_RESET = input.OPBOX_RESET_SWITCH
		|| input.SAFETY_RESET_SW_ON || softwareSafetyResetActive;
	this->io_WriteGripper();

	if(softwareSafetyResetCompleted){
		safetyResetPulseUntilTick = 0;
		if(input.SAFETY_DOOR_READY)
			MainForm->memoRobostarLineAdd("[SAFETY RESET] Y0032 pulse complete: X002C=1 (READY)");
		else
			MainForm->memoRobostarLineAdd("[SAFETY RESET] Y0032 pulse complete: X002C=0 (NOT READY)");
	}
	if(sscOpened) mr2Sensing();

	// Collision prevention: continuously stop EJECT/INSERT/HOME/WAIT POSITION
	// and manual channel moves when a required centering contact is lost or PLC
	// status data is stale. Dry Run keeps its dedicated runtime interlock.
	if(!CheckTrayCenteringMotionInterlock())
		return;

	if(seq == seqInit)Init();
	else if(seq == seqHome)Home();
	if(seq == seqServoOn)ServoOn();
	else if(seq == seqServoOff)ServoOff();
	else if(seq == seqReset)Reset();
	else if(seq == seqAutoRun)AutoRun();
    else if(seq == seqEmgAutoRun)EmgAutoRun();
	else if(seq == seqAutoMove)AutoMove();
	else if(seq == seqAutoEject)AutoEject();
	else if(seq == seqAutoInsert)AutoInsert();

	else if(seq == seqWait)WaitPosition();
	else if(seq == seqZup)zUp();
	//* DRY RUN : Dedicated return is dispatched independently of production WAIT.
	else if(seq == seqDryRunWait)DryRunWaitPosition();
	else if(seq == seqZdown)zDown();

	else if(seq == seqJOGx_Plus)MoveJog(0);
	else if(seq == seqJOGx_Minus)MoveJog(1);
	else if(seq == seqJOGy_Plus)MoveJog(2);
	else if(seq == seqJOGy_Minus)MoveJog(3);
	else if(seq == seqJOGz_Plus)MoveJog(4);
	else if(seq == seqJOGz_Minus)MoveJog(5);
	else if(seq == seqJogStop)MoveJog(10);
//	else MainForm->memoRobostarLineAdd("[로보트] 대기상태");
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::req_EjectComplete()
{
	if(!IsCcLinkReady()){
		MainForm->memoRobostarLineAdd(
			"[EJECT COMPLETE INTERLOCK] Request rejected: CC-Link is not ready.");
		return false;
	}
	if(!getCellDetectStatus()){
		MainForm->memoRobostarLineAdd(
			"[EJECT COMPLETE INTERLOCK] Request rejected: gripper cell sensor is OFF.");
		return false;
	}
	if(::gripper == NULL || !::gripper->CommitEjectTrayState(move.tool)){
		MainForm->memoRobostarLineAdd(
			"[EJECT COMPLETE INTERLOCK] Source tray state commit failed.");
		return false;
	}

	// Forced recovery still raises Z and confirms position 0 before completion.
	InitSequence(seqAutoEject);
	step.step = 7;
	return true;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::req_InsertComplete()
{
	if(::gripper == NULL || !::gripper->CommitInsertTrayState(move.tool)){
		MainForm->memoRobostarLineAdd(
			"[INSERT COMPLETE INTERLOCK] Target tray state commit failed.");
		return false;
	}

	// Forced recovery still issues Z UP and confirms position 0 before completion.
	InitSequence(seqAutoInsert);
	step.step = 5;
	return true;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::CheckEjectCell_after(int pos)
{
	// 6.셀을 들고 있는지 확인한다.
	bool bresult = false;
	switch(pos){
		case 1:
			if(getCellDetectStatus()){
				bresult = true;
			}
			break;
	}
	return bresult;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::CheckEjectCell_before(int pos)
{
	// Communication loss must not be interpreted as an empty gripper.
	if(!IsCcLinkReady())
		return false;

	// Pre-move/initialization check: X0022 ON means the gripper has no cell.
	bool bresult = false;
	switch(pos){
		case 1:
			if(!getCellDetectStatus()){
				bresult = true;
			}
			break;
	}
	return bresult;
}
//---------------------------------------------------------------------------

bool __fastcall Trobostar::CheckEjectUnchuck(int pos)
{
	// 언척 센서 확인 후 언척 출력
	bool bresult = false;
	switch(pos){
		case 1:
			if(input.GRIPPER1_UNCHUCK){
				bresult = true;
			}else{
				gripper.GRIPPER1_CHUCK = false;
				gripper.GRIPPER1_UNCHUCK = true;
			}
			break;
	}
	return bresult;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::CheckEjectChuck(int pos)
{
	// 척 센서 확인 후 척 출력
	bool bresult = false;
	switch(pos){
		case 1:
			if(input.GRIPPER1_CHUCK){
				bresult = true;
			}else{
				gripper.GRIPPER1_CHUCK = true;
				gripper.GRIPPER1_UNCHUCK = false;
			}
			break;
	}
	return bresult;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::CheckInsertUnchuck(int pos)
{
	// 언척 센서 확인 후 언척 출력
	bool bresult = false;
	switch(pos){
		case 1:
			if(input.GRIPPER1_UNCHUCK){
					bresult = true;
			}else{
				gripper.GRIPPER1_CHUCK = false;
				gripper.GRIPPER1_UNCHUCK = true;
			}
			break;
	}
	return bresult;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::CheckInsertCellReleased(int pos)
{
	// Communication loss must not confirm that the cell was released.
	if(!IsCcLinkReady())
		return false;

	// X0022 is active-low: ON means the gripper is clear after releasing the cell.
	if(pos == 1)
		return !getCellDetectStatus();
	return false;
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::DataModuleCreate(TObject *Sender)
{
	//* 2026 08 07 sscOpen()은 Servo Open 버튼의 Init()에서 실행하여 중복 호출 방지
	//* 2026 08 07 CC-Link mdOpen()/io_Init()은 FormMain::FormShow() 한 곳에서만 실행
	req_Speed(1000, 1000, 1000);
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::DataModuleDestroy(TObject *Sender)
{
	if(sscOpened){
		int sts = sscClose(board_id);
		WriteLog(sts, "SSC_CLOSE");
		sscOpened = false;
	}
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::KeyLock(bool on)
{
	if(!on){
		// KEYLOCK release interlocks:
		// 1) automatic operation must never release the key lock;
		// 2) X002A must confirm that the hardware BYPASS key is in ON position.
		if(MainForm == NULL || MainForm->equipMode != modeManual)
			return false;
		// X002A ON confirms that the hardware BY-PASS switch is in ON position.
		if(!IsBypassActive())
			return false;
//		if(keyLockReleasePending)
//			return true;

		// Release sequence: lock the BYPASS key with Y003C OFF, then send Y003D ON.
		// Y0033/Y0034 are released only after the configured delay.
		Bypass(false);
		gripper.SAFETY_BYPASS_ON = true;
		keyLockSetPending = false;
		keyLockSetSafetyBypassOffTick = 0;
		keyLockReleasePending = true;
		io_WriteGripper();
		keyLockReleaseOutputOffTick = GetTickCount() + KEYLOCK_OUTPUT_DELAY_MS;
		MainForm->memoRobostarLineAdd("[KEYLOCK] Release requested: Y003D ON; wait 500ms.");
		return true;
	}

	// KEYLOCK set interlock: X0026/X0027 ON means Door #1/#2 is open.
	// Never energize Y0033/Y0034 unless both door inputs are OFF (doors closed).
	if(!CanSetKeyLock()){
		if(MainForm != NULL)
			MainForm->memoRobostarLineAdd("[KEYLOCK] Set rejected: X0026/X0027 door input is ON.");
		return false;
	}

	if(keyLockSetPending)
		return true;

	// KEYLOCK set output order: Y0033/Y0034 ON first, then Y003D OFF after 500 ms.
	// The timer re-checks X0026/X0027 throughout the delay and safely cancels if opened.
	gripper.DOOR_LEFT_CLOSE = true;
	gripper.DOOR_RIGHT_CLOSE = true;
	keyLockReleasePending = false;
	keyLockReleaseOutputOffTick = 0;
	keyLockSetPending = true;
	io_WriteGripper();
	keyLockSetSafetyBypassOffTick = GetTickCount() + KEYLOCK_OUTPUT_DELAY_MS;
	if(MainForm != NULL)
		MainForm->memoRobostarLineAdd("[KEYLOCK] Set requested: Y0033/Y0034 ON; wait 500ms.");
	return true;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::CanSetKeyLock() const
{
	// X0026/X0027 are active-high door-open contacts. Both must be OFF.
	return !input.SAFETY_DOOR_1 && !input.SAFETY_DOOR_2;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::Bypass(bool on)
{
	// Y003C ON releases the mechanical lock so the hardware BYPASS key can turn OFF.
	// It is allowed only after the complete KEYLOCK set sequence and while the
	// hardware BYPASS key is still confirmed in its ON position.
	if(on && !CanEnableBypassSol())
		return false;

	gripper.DOOR_OPEN_SELECT = on;
	return gripper.DOOR_OPEN_SELECT == on;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::CanEnableBypassSol() const
{
	return gripper.DOOR_LEFT_CLOSE && gripper.DOOR_RIGHT_CLOSE
		&& IsKeyLockActive()
		&& !keyLockSetPending && !keyLockReleasePending
		&& IsBypassActive();
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::RequestSafetyResetPulse()
{
	// Follow the C# utility: X0025/X0028 remain physical inputs and the
	// software button drives the same final Y0032 output for 1.5 seconds.
	if(MainForm == NULL || MainForm->path != 81)
		return false;

	safetyResetPulseUntilTick = GetTickCount() + 1500;
	MainForm->memoRobostarLineAdd("[SAFETY RESET] Software Y0032 pulse accepted (1500ms)");
	return true;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::IsSoftwareSafetyResetActive() const
{
	if(safetyResetPulseUntilTick == 0)
		return false;
	return (LONG)(safetyResetPulseUntilTick - GetTickCount()) > 0;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::IsEmergencyStopActive() const
{
	return !input.EMS_SWITCH;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::IsSafetyReady() const
{
	return input.SAFETY_EMG_READY && input.SAFETY_DOOR_READY;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::IsSafetyDoorOpen(int doorNo) const
{
	if(doorNo == 1) return input.SAFETY_DOOR_1;
	if(doorNo == 2) return input.SAFETY_DOOR_2;
	return false;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::IsKeyLockActive() const
{
	// X0026/X0027: both OFF confirms KEYLOCK set.
	return !input.SAFETY_DOOR_1 && !input.SAFETY_DOOR_2;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::IsBypassActive() const
{
	// X002A(BYPASS_SW_ON)=ON confirms actual hardware BY-PASS ON.
	return !input.BYPASS_SW_OFF && input.BYPASS_SW_ON;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::IsSscOpened() const
{
	return sscOpened;
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::SetCcLinkOpenResult(short result, long openedPath)
{
	config.ret = result;
	m_ccLinkOpened = result == 0;
	m_ccLinkRunning = false;
	config.path = m_ccLinkOpened ? openedPath : 0;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::IsCcLinkReady() const
{
	return m_ccLinkOpened && m_ccLinkRunning;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::IsHomeRequiredAfterServoOff() const
{
	return homeRequiredAfterServoOff;
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::Y003D(bool bOn)
{
//	if(bOn)
//		gripper.SAFETY_BYPASS_ON = true;
//	else
//		gripper.SAFETY_BYPASS_ON = false;
    gripper.SAFETY_BYPASS_ON = bOn;
	io_WriteGripper();
}
