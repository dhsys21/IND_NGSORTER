//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
#include "FormMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
Trobostar *robostar;
//---------------------------------------------------------------------------
const int errCnt = 100;//300;
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

	point[0].position = 0;
}
//---------------------------------------------------------------------------
int __fastcall Trobostar::io_Init()
{
    short num[4] = {0x024E, 0, 0, 0};
	for(int i = 1; i <= servoCnt; i++) sscSetMonitor(board_id, channel_id, i, &num[0]);

	senTimer->Enabled = true;

	short size = sizeof(gripper);
	mdReceive(config.path, config.stno, DevY, 0x0020, &size, &gripper);	// IO 상태는 초기 상태를 불러온다 : 셀을 놓을 수 있기 때문에
	for(int i = 1; i <= gripCnt; ++i)GripperDown(i, false, true);

	return 0;	// 그리퍼는 모두 상승시킨다.

}
//---------------------------------------------------------------------------
int __fastcall Trobostar::io_Read()
{
	short size = sizeof(input);
	return mdReceive(config.path, config.stno, DevX, 0x0000, &size, &input);
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
void __fastcall Trobostar::Init()
{
	int sts;
	sts = sscOpen(board_id);
	WriteLog(sts, "SSC_OPEN");
	sts = sscReboot(board_id, channel_id, timeout);
	WriteLog(sts, "REBOOT");
	sts = sscResetAllParameter(board_id, channel_id, timeout);
	WriteLog(sts, "RESET PARAMETER");
	sts = sscLoadAllParameterFromFlashROM(board_id, channel_id, timeout);
	WriteLog(sts, "LOAD PARAMETER");
	sts = sscSystemStart(board_id, channel_id, timeout);
	WriteLog(sts, "SERVO START");
	InitSequence(seqIdle);
}
//---------------------------------------------------------------------------
// 2019 07 05 홈위치 이동시 그리퍼 움직이지 않고 x, y, z 축 원점 후에 그리퍼에 셀이 있는지 확인하는 버전
void __fastcall Trobostar::Home()
{
	if(output.CYLINDER_Z == false){
		MainForm->memoRobostarLineAdd("[C_Maint] 실린더 / 도어 상태 비정상");
		InitSequence(seqIdle);
		return;
	}

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

	int sts = 0;
	int bitInfo = 0;

	switch(step.step){
		case 0: // 실린더 상승
            teachForm->pnlMovingAlarm->Visible = true;
			teachForm->pnlMovingAlarm->BringToFront();
			teachForm->pnlMovingAlarm2->Visible = true;
			teachForm->pnlMovingAlarm2->BringToFront();
			for(int i=1; i<=gripCnt; ++i)GripperDown(i, false, true);	// 그리퍼는 모두 상승시킨다.
			step.step += 1;
			break;
		case 1:
			sts = sscHomeReturnStart(board_id, channel_id, Axis_z);
			WriteLog(sts, "Z Axis Servo Home - Request");
			step.step += 1;
			break;
		case 2: // Z축 원점
			if(getGripperUpStatus())step.step += 1;
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
				MainForm->memoRobostarLineAdd("X Axis Servo Home - Complete");
				step.step += 1;
			}
			break;
		case 8: // 2019 07 05 x, y, z 축 원점 이동 후 그리퍼에 셀이 있는지 확인
			teachForm->pnlMovingAlarm->Visible = false;
			teachForm->pnlMovingAlarm2->Visible = false;

			if(input.GRIPPER1_CELL_DETECT == true)
			{
				MainForm->memoRobostarLineAdd("[B_Ignition] 그리퍼에 셀이 있습니다. 셀을 제거 후 원점으로 이동 해 주세요");
                step.step = 99;
			}
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

	int sts = 0;
	for(int i=1; i<=servoCnt; ++i){
		sts = sscSetCommandBitSignalEx(board_id, channel_id, i, SSC_CMDBIT_AX_SON, SSC_BIT_ON);
		WriteLog(sts, "[" + IntToStr(i) +  "] 서보 ON");
	}
	InitSequence(seqIdle);
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::ServoOff()
{
	int sts = 0;
	for(int i=1; i<=servoCnt; ++i){
		sts = sscSetCommandBitSignalEx(board_id, channel_id, i, SSC_CMDBIT_AX_SON, SSC_BIT_OFF);
		WriteLog(sts, "[" + IntToStr(i) +  "] 서보 OFF");
	}
	InitSequence(seqIdle);
}
//---------------------------------------------------------------------------

void __fastcall Trobostar::MoveJog(int ntype)
{
	long speed_id = 100;
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
	bool bsetPoint;
	int sts, stsZ1, stsZ2, sts1, sts2;

	if(axnum_id == Axis_zUp){
		sts = sscSetPointDataEx(board_id, channel_id, Axis_z, 0, &point[0]);
		stsZ1 = WriteLog(sts, "[" + IntToStr(Axis_z) +  "] POINT");
		sts = sscAutoStart(board_id, channel_id, Axis_z, 0, 0);
		stsZ2 = WriteLog(sts, "[" + IntToStr(Axis_z) +  "] MOVE");
	}else{
		point[axnum_id].position = pos;
		sts = sscSetPointDataEx(board_id, channel_id, axnum_id, 0, &point[axnum_id]);
		sts1 = WriteLog(sts, "[" + IntToStr(axnum_id) +  "] POINT");
		sts = sscAutoStart(board_id, channel_id, axnum_id, 0, 0);
		sts2 = WriteLog(sts, "[" + IntToStr(axnum_id) +  "] MOVE");
	}

	if(stsZ1 == true && stsZ2 == true)
		bsetPoint = true;
	else
		bsetPoint = false;
	return bsetPoint;
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
void __fastcall Trobostar::AutoMove()
{
	AnsiString loadfactor = "", px = "", py = "", zpoint = "", msg1 = "", msg2 = "";
	UnicodeString msg = "";
	if(output.CYLINDER_Z == false)
	{
		MainForm->memoRobostarLineAdd("Cylinder / Door condition abnormal");
		InitSequence(seqIdle);
		return;
	}
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
	else if(!MainForm->m_ServoHome)
	{
		InitSequence(seqIdle);
		MainForm->memoRobostarLineAdd("Please check servo HOME status.");
		AlarmForm->ShowError("The servo is not HOME.", "Please check and restart.");
		return;
	}

	switch(step.step){
		case 0: // Z축 상승
			zUpCount = 0;
			step.step = 1;
			//ch1 : 120900   (gripper1), 30900 (gripper2)
			//ch24 : 1231900
            bSetPoint = false;
			if(ConfigForm->chkZAxisUp->Checked == true){
                //* 2026 06 z축을 0위치까지 이동. => 헝가리는 티칭값만큼만 이동.
				bSetPoint = setPoint(Axis_zUp, 0);

				MainForm->memoRobostarLineAdd("[Z Axis Up] 0");
				teachForm->pnlMovingAlarm->Visible = true;
				teachForm->pnlMovingAlarm->BringToFront();
				teachForm->pnlMovingAlarm2->Visible = true;
				teachForm->pnlMovingAlarm2->BringToFront();
			}
			else{
                if(point[Axis_x].position <= 600000 && mr2.pos[Axis_x] >= 600000){
					bSetPoint = setPoint(Axis_zUp, -1000);
					MainForm->memoRobostarLineAdd("[Z Axis Up] -1000");
				}
				else if(point[Axis_x].position >= 600000 && mr2.pos[Axis_x] <= 600000){
					bSetPoint = setPoint(Axis_zUp, -1000);
					MainForm->memoRobostarLineAdd("[Z Axis Up] -1000");
				}
				else{
					bSetPoint = setPoint(Axis_z, point[Axis_z].position);
					MainForm->memoRobostarLineAdd("Z Axis Up - setPoint");
					msg = "[Z Axis Up] " + point[Axis_z].position;
					MainForm->memoRobostarLineAdd(msg);
					teachForm->pnlMovingAlarm->Visible = true;
					teachForm->pnlMovingAlarm->BringToFront();
					teachForm->pnlMovingAlarm2->Visible = true;
					teachForm->pnlMovingAlarm2->BringToFront();
					step.step = 10;
				}
			}
			break;
		case 1:
			//* Z축 이동 확인. 20초 이내 0으로 이동하지 않으면
            zUpCount++;

			if(bSetPoint == false){
				step.step = 0;
				MainForm->memoRobostarLineAdd("setPoint(Axis_zUp, 0) 에러");
			}

			if(zUpCount > 200){
				loadfactor = teachForm->lblLoadFactor4->Caption;
				zpoint = MainForm->pz->Caption;
				msg1 = "대기상태로 이동 후 다시 시작하세요. \r\n(부하율 : " + loadfactor	+ ", z축 위치 : " + zpoint + ")";
				MainForm->memoRobostarLineAdd("Z 축 이동실패" + msg1);
				AlarmForm->ShowError("Z 축 이동실패", msg1);
				zUpCount = 0;
			}
			if(rangeCheck(Axis_zUp))step.step = 10;
			break;

		case 10:
			bSetPoint = false;
			setPoint(Axis_x, point[Axis_x].position);   // x 위치 이동
			setPoint(Axis_y, point[Axis_y].position);   // y 위치 이동
			step.step += 1;
			MainForm->memoRobostarLineAdd("[MOVE] X and Y");
			break;
		case 11:
			rangeCheck(Axis_x);
			MainForm->memoRobostarLineAdd("[CHECK] X");
			break;
		case 12:
			rangeCheck(Axis_y);
			MainForm->memoRobostarLineAdd("[CHECK] Y");
			break;
		case 13:
			setPoint(Axis_z, point[Axis_z].position);   // z 위치 이동
			MainForm->memoRobostarLineAdd("[MOVE] Z");
			step.step += 1;
			break;
		case 14:
			rangeCheck(Axis_z);
			MainForm->memoRobostarLineAdd("[CHECK] Z");
			break;
		default:
			MainForm->memoRobostarLineAdd("[FINISH] MOVE COMPLETE");
			teachForm->pnlMovingAlarm->Visible = false;
			teachForm->pnlMovingAlarm2->Visible = false;
			InitSequence(step.reserve);
			break;
	}
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::WaitPosition()
{
	if(output.CYLINDER_Z == false){
		MainForm->memoRobostarLineAdd("Cylinder / Door condition abnormal");
		InitSequence(seqIdle);
		return;
	}

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
	else if(!MainForm->m_ServoHome)
	{
		InitSequence(seqIdle);
		MainForm->memoRobostarLineAdd("Please check servo HOME status.");
		AlarmForm->ShowError("The servo is not HOME.", "Please check and restart.");
		return;
	}

	switch(step.step){
		case 0: // Z축 상승
			teachForm->pnlMovingAlarm->Visible = true;
			teachForm->pnlMovingAlarm->BringToFront();
			teachForm->pnlMovingAlarm2->Visible = true;
			teachForm->pnlMovingAlarm2->BringToFront();
			setPoint(Axis_zUp, 0);
			step.step += 1;
			break;
		case 1:
			rangeCheck(Axis_zUp);
			break;
		case 2:
			setPoint(Axis_x, Wait_xAxis);   // x 위치 이동
			setPoint(Axis_y, Wait_yAxis);   // y 위치이동
			step.step += 1;
			MainForm->memoRobostarLineAdd("[MOVE] X and Y");
			break;
		case 3:
			rangeCheck(Axis_x);
			MainForm->memoRobostarLineAdd("[CHECK] X");
			break;
		case 4:
			rangeCheck(Axis_y);
			MainForm->memoRobostarLineAdd("[CHECK] Y");
			break;
		case 5:
            teachForm->pnlMovingAlarm->Visible = false;
			teachForm->pnlMovingAlarm2->Visible = false;
			if(input.GRIPPER1_CELL_DETECT == true)
				step.step = 9;
			else
				step.step += 1;
			break;
		default:
			InitSequence(seqIdle);
			break;
	}
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::zUp()
{
	switch(step.step){
		case 0: // Z축 상승
			setPoint(Axis_zUp, 0);
			step.step += 1;
			break;
		case 1:
			rangeCheck(Axis_zUp);
			break;
		case 2:
			InitSequence(seqIdle);
    		break;
	}
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::req_Init()
{
	InitSequence(seqInit);
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::req_Home()
{
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
	InitSequence(seqServoOff);
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::req_JogMove(int ntype)
{
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
bool __fastcall Trobostar::SetPositionValue()
{
    if(move.channel < 1 || move.channel > TraySlotCount)
        return false;
    if(move.tool < 1 || move.tool > gripCnt)
        return false;
    if(move.pallet != 1 && move.pallet != 2)
        return false;

    TrayAxisEdit xEditType = move.pallet == 1 ? asSourceX : asTargetX;
    TrayAxisEdit yEditType = move.pallet == 1 ? asSourceY : asTargetY;
    TEdit* xEdit = teachForm->GetTrayEdit(move.channel, xEditType);
    TEdit* yEdit = teachForm->GetTrayEdit(move.channel, yEditType);
    TEdit* zEdit = move.pallet == 1 ? teachForm->edit_SZ : teachForm->edit_TZ;
    int baseX = 0;
    int baseY = 0;
    int zPosition = 0;

    if(xEdit == NULL || yEdit == NULL || zEdit == NULL)
        return false;
    if(!TryStrToInt(xEdit->Text.Trim(), baseX)
        || !TryStrToInt(yEdit->Text.Trim(), baseY)
        || !TryStrToInt(zEdit->Text.Trim(), zPosition))
        return false;

    long position[AxisCnt] = {0};
    long channelOffset = ((move.channel - 1) % TrayTeachingGroupSize) * (long)TrayCellPitch;
    long toolOffset = (move.tool - 1) * 90000L;

    position[Axis_x] = (long)baseX + channelOffset - toolOffset;
    position[Axis_y] = baseY;
    position[Axis_z] = zPosition;

    for(int i = 1; i <= servoCnt; ++i)
        point[i].position = position[i];

    return true;
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
	if(seq == seqIdle || seq == seqPause || MainForm->equipMode == modeManual){
		move.pallet = pallet;
		move.type = type;
		move.tool = tool;
		move.channel = channel;
		move.cnt = 0;

		if(!SetPositionValue()){
			MainForm->memoMainLineAdd("[Robot] Invalid tray move request.");
			return;
		}
		InitSequence(seqAutoMove, seq);
	}
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::req_AutoEject(int pallet, int tool, int channel, int cnt, int type)
{
	move.pallet = pallet;
	move.type = type;
	move.tool = tool;
	move.channel = channel;
	move.cnt = cnt;

	if(!SetPositionValue()){
		MainForm->memoMainLineAdd("[Robot] Invalid eject position request.");
		return;
	}
	InitSequence(seqAutoMove, seqAutoEject);
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::req_AutoInsert(int pallet, int tool, int channel, int cnt, int type)
{
	move.pallet = pallet;
	move.type = type;
	move.tool = tool;
	move.channel = channel;
	move.cnt = cnt;

	if(!SetPositionValue()){
		MainForm->memoMainLineAdd("[Robot] Invalid insert position request.");
		return;
	}
	InitSequence(seqAutoMove, seqAutoInsert);
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::req_WaitPosition()
{
	InitSequence(seqWait);
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::req_zUp()
{
	InitSequence(seqZup);
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
	InitSequence(seqIdle);

	for(int i=1; i<=servoCnt; ++i){
		sts = sscDriveStop(board_id, channel_id, i, 0);
		WriteLog(sts, "DriveStop");
	}
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::GripperDown(int num, bool down, bool up)
{
	switch(num){
		case 1:
			gripper.GRIPPER1_DOWN_SOL = down;
			gripper.GRIPPER1_UP_SOL = up;
			break;
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
    if(output.CYLINDER_Z == false){
		MainForm->memoRobostarLineAdd("[C_Maint] 실린더 / 도어 상태 비정상");
		return;
	}

	int sts = 0;
	int bitInfo = 0;

	switch(step.step){
		case 0: //  서보 OPEN
			sts = sscOpen(board_id);
			WriteLog(sts, "SSC_OPEN");
			sts = sscReboot(board_id, channel_id, timeout);
			WriteLog(sts, "REBOOT");
			sts = sscResetAllParameter(board_id, channel_id, timeout);
			WriteLog(sts, "RESET PARAMETER");
			sts = sscLoadAllParameterFromFlashROM(board_id, channel_id, timeout);
			WriteLog(sts, "LOAD PARAMETER");
			sts = sscSystemStart(board_id, channel_id, timeout);
			WriteLog(sts, "SERVO START");

			step.step += 1;
			break;
		case 1: //  서보 ON
			for(int i = 1; i <= servoCnt; ++i){
				sts = sscSetCommandBitSignalEx(board_id, channel_id, i, SSC_CMDBIT_AX_SON, SSC_BIT_ON);
				WriteLog(sts, "[" + IntToStr(i) +  "] 서보 ON");
			}
			step.delay = 0;
			step.step += 1;
			break;
		case 2:
			if(step.delay >= 10) step.step += 1;
			else step.delay += 1;
			break;
		case 3:	//  서보 HOME
			for(int i=1; i<=2; ++i)GripperDown(i, false, true);	// 그리퍼는 모두 상승시킨다.
			step.step += 1;
			break;
		case 4:
			sts = sscHomeReturnStart(board_id, channel_id, Axis_z);
			WriteLog(sts, "Z Axis Servo Home - Request");
			step.step += 1;
			break;
		case 5: // Z축 원점
			if(getGripperUpStatus())step.step += 1;
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
	if(output.CYLINDER_Z == false){
		MainForm->memoRobostarLineAdd("[C_Maint] 실린더 / 도어 상태 비정상");
		return;
	}

	int sts = 0;
	int bitInfo = 0;
    short num[4] = {0x024E, 0, 0, 0};
	switch(step.step){
		case 0: //  서보 OPEN
			sts = sscOpen(board_id);
			WriteLog(sts, "SSC_OPEN");
			sts = sscReboot(board_id, channel_id, timeout);
			WriteLog(sts, "REBOOT");
			sts = sscResetAllParameter(board_id, channel_id, timeout);
			WriteLog(sts, "RESET PARAMETER");
			sts = sscLoadAllParameterFromFlashROM(board_id, channel_id, timeout);
			WriteLog(sts, "LOAD PARAMETER");
			sts = sscSystemStart(board_id, channel_id, timeout);
			WriteLog(sts, "SERVO START");

            for(int i = 1; i <= servoCnt; i++) sscSetMonitor(board_id, channel_id, i, &num[0]);

			step.step += 1;
			break;
		case 1: //  서보 ON
			for(int i = 1; i <= servoCnt; ++i){
				sts = sscSetCommandBitSignalEx(board_id, channel_id, i, SSC_CMDBIT_AX_SON, SSC_BIT_ON);
				WriteLog(sts, "[" + IntToStr(i) +  "] Servo ON");
			}
			step.delay = 0;
			step.step += 1;
			break;
		case 2:
			if(step.delay >= 10) step.step += 1;
			else step.delay += 1;
			break;
		case 3:	//  서보 HOME
			for(int i=1; i<=2; ++i)GripperDown(i, false, true);	// 그리퍼는 모두 상승시킨다.
			step.step += 1;
			break;
		case 4:
			sts = sscHomeReturnStart(board_id, channel_id, Axis_z);
			WriteLog(sts, "Z Axis Servo Home - Request");
			step.step += 1;
			break;
		case 5: // Z축 원점
			if(getGripperUpStatus())step.step += 1;
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
			if(input.GRIPPER1_CELL_DETECT == true)
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
	int nresult = CheckFlow();
	AnsiString msg;

	if(nresult == 0){
		msg = "Gripper #" + IntToStr(move.tool);

		switch(step.step){
			case 0:
				for(int i=0; i<move.cnt; ++i)
					nresult += CheckEjectCell_before(move.tool + i); // cell 이 없으면 true
				if(nresult == move.cnt){
					step.step += 1;
					step.timeout = 0;
				}
				else{
					step.timeout += 1;
					if(step.timeout == errCnt){
						ErrorForm_eject->ShowError(msg + " has a cell.", "Eject step 1. Cell check error", move.tool, 20);
					}
					MainForm->memoRobostarLineAdd("Eject step 1. Cell check");
				}
				break;
			case 1:
				for(int i=0; i<move.cnt; ++i)
					nresult += CheckEjectUnchuck(move.tool + i);
				if(nresult == move.cnt){
					step.step += 1;
					step.timeout = 0;
				}
				else{
					step.timeout += 1;
					if(step.timeout == errCnt){
						ErrorForm_eject->ShowError("[B_Ignition] " + msg + " UNCHUCK 센서 감지 실패", "취출 2단계. 그리퍼 열기 에러", move.tool, 0);
					}
					MainForm->memoRobostarLineAdd("[C_Maint] 취출2. GRIPPER UNCHUCK");
				}
				break;
			case 2:
                //* 2023 11 22 그리퍼 내리기 전 센터링 다시 확인
				if(MainForm->psrcReady->Color != clLime)
				{
					AlarmForm->ShowError("[C_Maint] 선별 트레이 센터링 여부를 확인해주세요.", "확인하고 재시작 하세요.");
				}else{
					for(int i=0; i<move.cnt; ++i)
						nresult += CheckEjectDown(move.tool + i);
					if(nresult == move.cnt){
						step.step += 1;
						step.timeout = 0;
					}
					else{
						step.timeout += 1;
						if(step.timeout == errCnt){
							ErrorForm_eject->ShowError("[B_Ignition] " + msg + " DOWN 센서 감시 실패", "취출 3단계. 그리퍼 DOWN 에러", move.tool, 15);
						}
						MainForm->memoRobostarLineAdd("[C_Maint] 취출3. GRIPPER DOWN");
					}
				}
				break;
			case 3:
				for(int i=0; i<move.cnt; ++i)
					nresult += CheckEjectChuck(move.tool + i);
				if(nresult == move.cnt){
					step.step += 1;
				}
				MainForm->memoRobostarLineAdd("[C_Maint] 취출4. GRIPPER CHUCK");
				break;
			case 4:
				if(step.delay >= 5)step.step += 1;
				else step.delay += 1;
				MainForm->memoRobostarLineAdd("[C_Maint] 취출5. 대기");
				break;
			case 5:
				for(int i=0; i<move.cnt; ++i)nresult += CheckEjectUp(move.tool + i);
				if(nresult == move.cnt){
					step.step += 1;
					step.timeout = 0;
				}
				else{
					step.timeout += 1;
					if(step.timeout == errCnt){
						ErrorForm_eject->ShowError("[B_Ignition] " + msg + " UP 센서 감시 실패", "취출 6단계. 그리퍼 UP 에러", move.tool, 16);
					}
					MainForm->memoRobostarLineAdd("[C_Maint] 취출6. GRIPPER UP");
				}

				break;
			case 6:
				for(int i=0; i<move.cnt; ++i)nresult += CheckEjectCell_after(move.tool + i);
				if(nresult == move.cnt){
					step.step += 1;
					step.timeout = 0;
				}
				else{
					step.timeout += 1;
					if(step.timeout == errCnt){
						ErrorForm_eject->ShowError("[B_Ignition] " + msg + " 셀이 없습니다.", "취출 7단계. 셀 체크 에러", move.tool, 0);
					}
					MainForm->memoRobostarLineAdd("[C_Maint] 취출7. 셀체크");
				}
				break;
			default:
				InitSequence(seqAutoEjectComplete);
				break;
		}
	}
	else{
		msg = "그리퍼 #" + IntToStr(nresult);
		ErrorForm_eject->ShowError("[B_Ignition] " + msg + " 하강시 충돌이 발생했습니다.", msg + " 완충센서 감지", move.tool, 17);

	}
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::AutoInsert()
{

	int nresult = CheckFlow();
	AnsiString msg;

	if(nresult == 0){
		msg = "그리퍼 #" + IntToStr(move.tool);

		switch(step.step){
			case 0:
				for(int i=0; i<move.cnt; ++i)nresult += CheckEjectCell_after(move.tool + i);
				if(nresult == move.cnt){
					step.step += 1;
					step.timeout = 0;
				}
				else{
					step.timeout += 1;
					if(step.timeout == errCnt){
						ErrorForm_insert->ShowError("[B_Ignition] " + msg + " 셀이 없습니다.", "이재 1단계. 셀 체크 에러", move.tool, 23);
					}
					MainForm->memoRobostarLineAdd("삽입1. 셀체크");
				}
				break;
			case 1:
				//* 2023 11 22 그리퍼 내리기 전 센터링 다시 확인
				if(MainForm->ptargetReady->Color != clLime)
				{
					AlarmForm->ShowError("[C_Maint] 대상 트레이 센터링 여부를 확인해주세요.", "확인하고 재시작 하세요.");
				}else{
					for(int i=0; i<move.cnt; ++i)nresult += CheckInsertDown(move.tool + i);
					if(nresult == move.cnt){
						step.step += 1;
						step.timeout = 0;
					}
					else{
						step.timeout += 1;
						if(step.timeout == errCnt){
							ErrorForm_insert->ShowError("[B_Ignition] " + msg + " DOWN 센서 감지 실패", "이재 2단계. 그리퍼 DOWN 에러", move.tool, 22);
						}
						MainForm->memoRobostarLineAdd("[C_Maint] 삽입2. GRIPPER DOWN");
					}
				}
				break;
			case 2:
				for(int i=0; i<move.cnt; ++i)nresult += CheckInsertUnchuck(move.tool + i);
				if(nresult == move.cnt){
					step.step += 1;
					step.timeout = 0;
				}
				else{
					step.timeout += 1;
					if(step.timeout == errCnt){
						ErrorForm_insert->ShowError("[B_Ignition] " + msg + " UNCHUCK 센서 감지 실패", "이재 3단계. 그리퍼 열기 에러", move.tool, 0);
					}
					MainForm->memoRobostarLineAdd("[C_Maint] 삽입3. GRIPPER UNCHUCK");
				}

				break;
			case 3:
				for(int i=0; i<move.cnt; ++i)nresult += CheckInsertUp(move.tool + i);
				if(nresult == move.cnt){
					step.step += 1;
					step.timeout = 0;
				}else{
					step.timeout += 1;
					if(step.timeout == errCnt){
						ErrorForm_insert->ShowError("[B_Ignition] " + msg + " UP 센서 감지 실패", "이재 4단계. 그리퍼 UP 에러", move.tool, 0);
					}
					MainForm->memoRobostarLineAdd("[C_Maint] 삽입4. GRIPPER UP");
				}
				break;
			case 4:
				if(step.delay >= 2)step.step += 1;
				else step.delay += 1;
				MainForm->memoRobostarLineAdd("[C_Maint] 삽입5. 대기");
				break;
			default:
				InitSequence(seqAutoInsertComplete);
				break;
		}
	}else{
		msg = "그리퍼 #" + IntToStr(nresult);
		ErrorForm_insert->ShowError("[B_Ignition] " + msg + " 하강시 충돌이 발생했습니다.", msg + " 완충센서 감지", move.tool, 21);

	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool __fastcall Trobostar::getGripperDownStatus()
{
	if(	input.GRIPPER1_DOWN){
		return false;
	}else{
		return true;
	}
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::getGripperUpStatus()
{
	if(	input.GRIPPER1_UP){
		return true;
	}else{
		return false;
	}
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::getCellDetectStatus()
{
	if(	input.GRIPPER1_CELL_DETECT){
		return false;
	}else{
		return true;
	}
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::getCellDetectStatus(int pos)
{
	switch(pos){
		case 1:
			if(input.GRIPPER1_CELL_DETECT && !input.GRIPPER1_DOWN)return false;
			break;
	}
	return true;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::getGripperChuckStatus()
{
	if(output.GRIPPER1_CHUCK){
		return true;
	}else{
		return false;
	}
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::mr2Sensing()
{
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
			for(int i=1; i<=servoCnt; ++i){
				sscGetStatusBitSignalEx(board_id, channel_id, i, SSC_STSBIT_AX_RDY, &mr2.servo[i]);
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
	gripper.SAFETY_RESET = input.OPBOX_RESET_SWITCH || input.SAFETY_RESET_SW_ON;
	// 그리퍼가 모두 상승되어 있을때 ON : 안전관련 추가사항
	output.CYLINDER_Z = input.GRIPPER1_UP;
	this->io_WriteGripper();
	mr2Sensing();

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
void __fastcall Trobostar::req_EjectComplete()
{
	InitSequence(seqAutoEjectComplete);
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::req_InsertComplete()
{
	InitSequence(seqAutoInsertComplete);
}
//---------------------------------------------------------------------------
int __fastcall Trobostar::CheckFlow()
{
	int nvalue = 0;
	if(input.GRIPPER1_BUFFER == true){
		for(int i=1; i<=gripCnt; ++i)	GripperDown(i, false, true);
		req_zUp();
		nvalue = 1;
	}
	return nvalue;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::CheckEjectCell_after(int pos)
{
	// 6.셀을 들고 있는지 확인한다.
	bool bresult = false;
	switch(pos){
		case 1:
			if(input.GRIPPER1_CELL_DETECT == true){
				bresult = true;
			}
			break;
	}
	return bresult;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::CheckEjectCell_before(int pos)
{
	// 1.셀이 없으면
	bool bresult = false;
	switch(pos){
		case 1:
			if(input.GRIPPER1_CELL_DETECT == false){
				bresult = true;
			}
			break;
	}
	return bresult;
}
//---------------------------------------------------------------------------

bool __fastcall Trobostar::CheckEjectUnchuck(int pos)
{
	// 2. 언척하고
	bool bresult = false;
	switch(pos){
		case 1:
			if(input.GRIPPER1_DOWN){
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
	// 4. 척하고
	bool bresult = false;
	switch(pos){
		case 1:
			if(input.GRIPPER1_DOWN == false){
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
bool __fastcall Trobostar::CheckEjectDown(int pos)
{
	// 3. 그리퍼 내리고
	bool bresult = false;
	switch(pos){
		case 1:
			if(input.GRIPPER1_DOWN){
				bresult = true;
			}else{
				gripper.GRIPPER1_DOWN_SOL = true;
				gripper.GRIPPER1_UP_SOL = false;
			}
			break;
	}
	return bresult;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::CheckEjectUp(int pos)
{
	// 5. 그리퍼 올리고
	bool bresult = false;
	switch(pos){
		case 1:
			if(input.GRIPPER1_UP){
				bresult = true;
			}else{
				gripper.GRIPPER1_DOWN_SOL = false;
				gripper.GRIPPER1_UP_SOL = true;
			}
			break;
	}
	return bresult;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::CheckInsertDown(int pos)
{
	bool bresult = false;
	switch(pos){
		case 1:
			if(input.GRIPPER1_DOWN == true){
				bresult = true;
			}else{
				gripper.GRIPPER1_DOWN_SOL = true;
				gripper.GRIPPER1_UP_SOL = false;
			}
			break;
	}
	return bresult;
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::CheckInsertUnchuck(int pos)
{
	// 셀이 없으면 언척
	bool bresult = false;
	switch(pos){
		case 1:
			if(input.GRIPPER1_DOWN == true){
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
bool __fastcall Trobostar::CheckInsertUp(int pos)
{
	// 언척이 아니면 실린더 상승
	bool bresult = false;
	switch(pos){
		case 1:
			if(input.GRIPPER1_UP){
				bresult = true;
			}
			else{
				gripper.GRIPPER1_DOWN_SOL = false;
				gripper.GRIPPER1_UP_SOL = true;
			}
			break;
	}
	return bresult;
}
//---------------------------------------------------------------------------
void __fastcall Trobostar::DataModuleCreate(TObject *Sender)
{
	int sts = 0;
	sts = sscOpen(board_id);
	WriteLog(sts, "SSC_OPEN");
	mdOpen(81,-1,&config.path);
	io_Init();
	req_Speed(1000, 1000, 1000);
}
//---------------------------------------------------------------------------
bool __fastcall Trobostar::KeyLock(bool on)
{
	gripper.DOOR_OPEN_SELECT = !on;
	return gripper.DOOR_OPEN_SELECT == !on;
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
	return !gripper.DOOR_OPEN_SELECT;
}
//---------------------------------------------------------------------------

