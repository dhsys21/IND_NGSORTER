//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
Tgripper *gripper;
//---------------------------------------------------------------------------
__fastcall Tgripper::Tgripper(TComponent* Owner)
	: TDataModule(Owner)
{
	seq = seqIdle;
	tool[gripCnt].disable = false;	// 7번 그리퍼는 항상 false로 마지막 지점 체크로 사용한다.
	pauseStatus = false;
}
//---------------------------------------------------------------------------
void __fastcall Tgripper::InitSequence(gripperSequence data, gripperSequence reserve)
{
	seq = data;
	step.step = 0;
	step.chCnt = 0;
	step.badCnt = 0;
	step.ejectCnt = 0;

	step.reserve = reserve;
}
//---------------------------------------------------------------------------
void __fastcall Tgripper::req_Pause(bool stop)
{
	if(stop != pauseStatus){
		if(stop){
			seq_save = seq;
			seq = seqPause;
			pauseStatus = true;
			MainForm->memoGripperLineAdd("[Pause] Pause state.");
		}else{
			seq = seq_save;
			pauseStatus = false;
			MainForm->memoGripperLineAdd("[Restart] Release the paused state.");
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall Tgripper::stepTimerTimer(TObject *Sender)
{
	for(int i=0; i<gripCnt; ++i){
		tool[i].disable = disable_gripper[i];	// true : 사용안함, false : 사용함
	}

	if(MainForm->equipMode == modeAuto)
	{
		if(seq == seqInit)
			Initialize();
		else if(seq == seqSorting)
			Sorting();
		else if(seq == seqInserting)
			Inserting();
		else if(seq == seqIdle)
			MainForm->memoGripperLineAdd(BaseForm->GetLangStr("MSG_WAITING"));
		else
			MainForm->memoGripperLineAdd(BaseForm->GetLangStr("MSG_WAITING") + " " + BaseForm->GetLangStr("MSG_AUTOMODE"));
	}
	else if(MainForm->equipMode == modeManual){
		MainForm->memoGripperLineAdd(BaseForm->GetLangStr("MSG_WAITING") + " " + BaseForm->GetLangStr("MSG_MANUALMODE"));
	}
	else if(MainForm->equipMode == modeAutoStop){
		MainForm->memoGripperLineAdd(BaseForm->GetLangStr("MSG_WAITING") + " " + BaseForm->GetLangStr("MSG_AUTOSTOPMODE"));
	}
	else if(MainForm->equipMode == modeEmergency){
		MainForm->memoGripperLineAdd(BaseForm->GetLangStr("MSG_WAITING") + " " + BaseForm->GetLangStr("MSG_EMGSTOPMODE"));
	}
}
//---------------------------------------------------------------------------
bool __fastcall Tgripper::GetZoneCode(int zone, AnsiString code)
{
	AnsiString fileName;

	switch(zone){
		case 0:fileName = (AnsiString)BIN + "IR_CODE.txt";break;
		case 1:fileName = (AnsiString)BIN + "OCV_CODE.txt";break;
		case 2:fileName = (AnsiString)BIN + "DV_CODE.txt";break;
		default: return true;
	}

	if(FileExists(fileName)){
		MainForm->zoneCode->Lines->LoadFromFile(fileName);
		if(MainForm->zoneCode->Text.Pos(code) > 0){
			if(MainForm->GetZoneCount(zone) > 0)return true;
		}
		else return false;
	}else{
		return false;
	}
}
//---------------------------------------------------------------------------
void __fastcall Tgripper::Initialize()
{
	bool bcheck = true;
	bool repeatCheck = false;
	AnsiString strMsg;

    if(MainForm->pwork1->Color != clLime)
	{
		AlarmForm->ShowError(BaseForm->GetLangStr("MSG_SOURCETRAY_NOTREADY"), BaseForm->GetLangStr("MSG_CHECK_RESTART"));
		return;
	}

	switch(step.step){
		case 0:	// 그리퍼 정보 초기화
			if(MainForm->pwork2->Color != clLime){
				MainForm->memoGripperLineAdd("[Init step 0] " + BaseForm->GetLangStr("MSG_TARGETTRAY_NOTREADY"));
				return;
			}
			if(MainForm->tray_target.remainCnt == 0){
				if(!waitTimer->Enabled)
				{
					MainForm->NotifyIdMatching_target("2");		// 대상 트레이 선별완료 보고하고
					MainForm->NotifyTransferOut(MainForm->pTrayid_target->Caption);
					MainForm->CmdTrayOut(1);
				}
				MainForm->memoGripperLineAdd("[Init step 0] " + BaseForm->GetLangStr("MSG_TARGETTRAY_FULL"));

				waitTimer->Enabled = true;
				return;
			}
			else if(MainForm->equipMode == modeAuto){	// 자동 - 운전 모드일 경우에만 동작한다.
                int servo_speed = teachForm->speedEdit->Text.ToInt();
				int servo_accl_speed = teachForm->acclSpeedEdit->Text.ToInt();
				int servo_dccl_speed = teachForm->dcclSpeedEdit->Text.ToInt();
				robostar->req_Speed(servo_speed, servo_accl_speed, servo_dccl_speed);

				for(int i=0; i<gripCnt; ++i){
					memset(&tool[i], 0, sizeof(tool[i]));
					tool[i].disable = disable_gripper[i];	// true : 사용안함, false : 사용함
					tool[i].source_ch = "0";
					tool[i].target_ch = "0";

					if(tool[i].disable == false && robostar->CheckEjectCell_before(i+1) == false){ 	// 그리퍼 사용하는데 셀이 있으면 알람발생
						MainForm->memoGripperLineAdd("[Init step 0] " + BaseForm->GetLangStr("MSG_GRIPPER_CELLDETECT") + IntToStr(i+1));
						AlarmForm->ShowError(BaseForm->GetLangStr("MSG_GRIPPER_CELLDETECT") + IntToStr(i+1), BaseForm->GetLangStr("MSG_AUTO_ALARM2"));
						return;
					}
				}
				step.step += 1;
			}else{
				MainForm->memoGripperLineAdd("[Init step 0] " + BaseForm->GetLangStr("MSG_AUTOSTOPMODE"));
			}
			waitTimer->Enabled = false;
			break;
		case 1:
        	// 1. 선별 트레이 채널 할당
            // 2. 대상 트레이 채널 할당
            // 3. step.chCnt는 InitSequence에서 0으로 초기화 => 0부터 그리퍼 갯수만큼 증가
			for(int j = step.chCnt; j < gripCnt; j++)
			{
				for(int nzone = 0; nzone < 4; ++nzone)
				{	// 1.zone 순서대로 돌면서 확인을 한다. => 현재는 zone을 사용하지 않음.
					for(int i=0; i<MainForm->tray_source.SLOT_COUNT; ++i)
					{
						if(tool[step.chCnt].disable){	// 그리퍼 사용 여부를 확인하고
							while(tool[step.chCnt].disable)step.chCnt += 1;
						}

                        //* 1. GetZonecode는 nzone이 3일때 항상 true,  2. pick = Y이면 "NG"
						if(MainForm->psort_ing[i]->Caption == "NG" && GetZoneCode(nzone, MainForm->tray_source.LOSS_CD[i])){	// 선별 채널의 코드와 zone을 확인한다.
							repeatCheck = false; //* zone이 여러개 일때 이전 zone 에서 할당이 된 경우 확인.
							for(int ch = 0; ch < step.chCnt; ++ch){// 채널이 이미 할당되어 있는지 확인.
								if(ch < gripCnt)
								{
									if(tool[ch].source_ch == (i+1))
									{
										repeatCheck = true;
										break;
									}
								}
							}

							if(repeatCheck == true){	//	채널이 이미 할당 되었다면 (다른 ZONE에 할당된 경우임)
//								i = MainForm->tray_source.SLOT_COUNT;
								continue;
							}
                            //* 현재 그리퍼가 그리퍼 최대갯수(불량선별기는 2개)보다 적거나 불량채널이 1개 이상인 경우 할당
							else if(step.chCnt < gripCnt && (MainForm->tray_target.remainCnt > 0) ){	// 대상트레이에 투입가능한 수량만큼 2개의 그리퍼에 선별 채널 할당한다.
								step.badCnt += 1;	// 잔여 불량 수량을 확인한다.

                                for(int tch = 0; tch < 96; ++tch){
                                    //* 4번째(nzone == 3) zone 색상이 white.
                                    //* 현재 대상 채널 모두 white 색이기때문에 nzone = 3만 동작 => white가 아니면(이미 셀이 담겨있으면) false.
									if(MainForm->GetZoneChannel(nzone, tch)){
                                        tool[step.chCnt].code = MainForm->psort_bad[i]->Caption;
                                        tool[step.chCnt].source_ch = MainForm->psort_ch[i]->Caption;
                                        MainForm->DisplaySourceCell(step.chCnt, i);	// 화면 show

                                        MainForm->tray_source.remainCnt -= 1;
                                        MainForm->tray_target.remainCnt -= 1;
                                        tool[step.chCnt].target_ch = tch+1;
                                        MainForm->tray_target.PICK[tch] = "R";
                                        MainForm->DisplayTargetCell(step.chCnt, tch);	// 화면 show
                                        MainForm->DisplayTargetCellInfo(step.chCnt, tch);
                                        step.ejectCnt += 1;	// 취출 예정 수량
                                        step.chCnt += 1;
                                        MainForm->memoGripperLineAdd("[Init step 1] " + BaseForm->GetLangStr("CAP_SOURCE_CHANNEL")+ " : "
                                            + MainForm->psort_ch[i]->Caption + " / " + BaseForm->GetLangStr("CAP_TARGET_CHANNEL") + " : " + IntToStr(tch+1));
                                        break;
									}
								}
							}
						}
					}
				}
				if(j == step.chCnt)
					step.chCnt++;
			}
			step.step += 1;
			break;
		default:
			if(step.badCnt > 0){
				if(step.ejectCnt > 0){
					InitSequence(step.reserve);	// 선별시작
					MainForm->memoGripperLineAdd("[Init step 2] " + BaseForm->GetLangStr("MSG_EJECT_START"));
				}else{
					MainForm->memoGripperLineAdd("[Init step 2] " + BaseForm->GetLangStr("MSG_CANNOT_EJECT") + " : " + BaseForm->GetLangStr("MSG_CHECK_GRIPPERUSAGE"));
					AlarmForm->ShowError(BaseForm->GetLangStr("MSG_CANNOT_EJECT"), BaseForm->GetLangStr("MSG_CHECK_GRIPPERUSAGE"));
					req_Init();
				}
			}
			else{
				MainForm->memoGripperLineAdd("[Init step 2] Sorting has ended.");
				InitSequence(seqIdle);						// 선별종료
				MainForm->NotifyIdMatching_source();		// 선별 트레이 선별완료 보고하고
				MainForm->NotifyIdMatching_target("1");		// 대상 트레이 선별완료 보고하고
				robostar->req_WaitPosition();				// 로보트는 대기위치로 이동시키고
				MainForm->NotifyTransferOut(MainForm->pTrayid_source->Caption);				// 검사 종료 보고
			}
			break;
	}
}
//---------------------------------------------------------------------------
void __fastcall Tgripper::Sorting()
{
	int pos1 = 0, pos2 = 0;
	int div1 = 0, div2 = 0;
	bool bfind = false;

    if(MainForm->pwork1->Color != clLime)
	{
		AlarmForm->ShowError(BaseForm->GetLangStr("MSG_SOURCETRAY_NOTREADY"), BaseForm->GetLangStr("MSG_AUTO_ALARM2"));
		return;
	}

	switch(step.step){
		case 0:
			memset(&eject, 0, sizeof(eject));
			for(int i = 0 ; i < gripCnt; ++i){	// 그리퍼 1개
				try{
					pos1 = tool[i].source_ch.ToInt();
				}catch(...){
					pos1 = 0;
				}

				if(pos1 > 0 && tool[i].eject_end == false){
					div1 = (pos1-1)/ (MainForm->tray_source.SLOT_COUNT/8);		// 선별 트레이 48채널 : 12, 96채널 : 24
					eject.pos = pos1;		// 취출 채널
					eject.conCnt = 1;		// 연속 수량
					eject.gripper = i+1;	// 그리퍼 번호

					break;
				}
			}

			if(eject.pos > 0){	// 취출 시작
				if(MainForm->tray_source.SLOT_COUNT == 96){
					 MainForm->memoGripperLineAdd("[Eject step 0] 96 Tray / Gripper #" + IntToStr(eject.gripper) + " / Channel #" + IntToStr(eject.pos) + " / Continuous eject #" + IntToStr(eject.conCnt));
					 robostar->req_AutoEject(1, eject.gripper , MainForm->mapSort[0][eject.pos-1], eject.conCnt, 962);
					 MainForm->memoGripperLineAdd("[Eject step 0] 96 Channel " + BaseForm->GetLangStr("MSG_EJECT_START"));
					 step.step += 1;
				}else{
					MainForm->memoGripperLineAdd(BaseForm->GetLangStr("MSG_SLOTCOUNT_ERROR"));
					ErrorForm->ShowError("Can not work", BaseForm->GetLangStr("MSG_SLOTCOUNT_ERROR"));
				}
			}else{
				step.step = 5;
			}
			break;
		case 1:	// 취출중 확인
			MainForm->memoGripperLineAdd("[Eject step 1] " + BaseForm->GetLangStr("MSG_EJECTING"));
			if(robostar->seq == seqAutoEject || robostar->seq == seqAutoEjectComplete)step.step += 1;

			break;
		case 2:	// 취출 완료 확인
			if(robostar->seq == seqAutoEjectComplete){
				MainForm->memoGripperLineAdd("[Eject step 2] " + BaseForm->GetLangStr("MSG_EJECT_END"));
				for(int i=eject.gripper; i<eject.gripper + eject.conCnt; ++i){
					tool[i-1].eject_end = true;
					MainForm->DisplaySourceCell(-1, tool[i-1].source_ch.ToInt()-1);	// 화면 show
				}
				step.step += 1;
			}else{
				MainForm->memoGripperLineAdd("[Eject step 2] " + BaseForm->GetLangStr("MSG_EJECTING"));
			}
			break;
		case 3:
			MainForm->memoGripperLineAdd("[Eject step 3] " + BaseForm->GetLangStr("MSG_EJECT_CHECK"));
			InitSequence(seqSorting);
			break;
		default:
			MainForm->memoGripperLineAdd("[Eject complete] " + BaseForm->GetLangStr("MSG_PREPARE_INSERT"));
			InitSequence(seqInserting);
			break;
	}
}
//---------------------------------------------------------------------------
void __fastcall Tgripper::Inserting()
{
	if(MainForm->pwork2->Color != clLime)
	{
		AlarmForm->ShowError(BaseForm->GetLangStr("MSG_TARGETTRAY_NOTREADY"), BaseForm->GetLangStr("MSG_AUTO_ALARM2"));
		return;
	}

	int pos1 = 0, pos2 = 0;
	int div1 = 0, div2 = 0;
	switch(step.step){
		case 0:
			memset(&insert, 0, sizeof(insert));
			for(int i=0; i < gripCnt; ++i){
				try{
					pos1 = tool[i].target_ch.ToInt();
				}catch(...){
					pos1 = 0;
				}

				if(pos1 > 0 && tool[i].insert_end == false){
					insert.pos = pos1;		// 취출 채널
					insert.conCnt = 1;		// 연속 수량
					insert.gripper = i+1;	// 그리퍼 번호

					break;
				}
			}
			if(insert.pos > 0){	// 삽입 시작
				MainForm->memoGripperLineAdd("[Insert step 0] Gripper #" + IntToStr(insert.gripper) + " / Channel #" + IntToStr(insert.pos) + " / Continuous insert #" + IntToStr(insert.conCnt));
				robostar->req_AutoInsert(2, insert.gripper , insert.pos, insert.conCnt, 96);
				step.step += 1;
			}else{
				step.step = 5;
			}
			break;
		case 1:	// 이재중 확인
			MainForm->memoGripperLineAdd("[Insert step 1] " + BaseForm->GetLangStr("MSG_INSERTING"));
			if(robostar->seq == seqAutoInsert || robostar->seq == seqAutoInsertComplete)
			{
				robostar->m_bInsertSave = false;
				step.step += 1;
			}
			break;
		case 2:	// 이재 완료 확인
			if(robostar->seq == seqAutoInsertComplete){
				MainForm->memoGripperLineAdd("[Insert step 2] " + BaseForm->GetLangStr("MSG_INSERT_END"));
				for(int i=insert.gripper; i<insert.gripper + insert.conCnt; ++i){
					tool[i-1].insert_end = true;
					MainForm->tray_target.SLOT_ID[tool[i-1].target_ch.ToInt()-1] = MainForm->tray_source.SLOT_ID[tool[i-1].source_ch.ToInt()-1];
					MainForm->tray_target.LOSS_CD[tool[i-1].target_ch.ToInt()-1] = MainForm->tray_source.LOSS_CD[tool[i-1].source_ch.ToInt()-1];
					MainForm->tray_target.PICK[tool[i-1].target_ch.ToInt()-1] = MainForm->tray_source.PICK[tool[i-1].source_ch.ToInt()-1];
					MainForm->tray_target.RANK[tool[i-1].target_ch.ToInt()-1] = MainForm->tray_source.RANK[tool[i-1].source_ch.ToInt()-1];
					MainForm->DisplayTargetCell(-1, tool[i-1].target_ch.ToInt()-1);	// 화면 show
					MainForm->DisplayTargetCellInfo(-1, tool[i-1].target_ch.ToInt()-1);
				}
				step.step += 1;
			}else{
				MainForm->memoGripperLineAdd("[Insert step 2] " + BaseForm->GetLangStr("MSG_INSERTING"));
			}
			break;
		case 3:
			MainForm->memoGripperLineAdd("[Insert step 3] " + BaseForm->GetLangStr("MSG_INSERT_CHECK"));
			InitSequence(seqInserting);
			break;
		default:
			MainForm->memoGripperLineAdd("[Insert complete] " + BaseForm->GetLangStr("MSG_PERPARE_EJECT"));
			MainForm->NotifyIdMatching_target("1");	// 이재 완료시마다 보고
			InitSequence(seqInit, seqSorting);
			break;
	}
}
//---------------------------------------------------------------------------
void __fastcall Tgripper::req_Sorting()
{
	InitSequence(seqSorting);
}
//---------------------------------------------------------------------------
void __fastcall Tgripper::req_Init()
{
	// mes로 부터 트레이 정보를 수신했을때만 사용한다.
	if(seq == seqIdle)InitSequence(seqInit, seqSorting);
}
//---------------------------------------------------------------------------

void __fastcall Tgripper::waitTimerTimer(TObject *Sender)
{
	AlarmForm->ShowError(BaseForm->GetLangStr("MSG_NO_TARGETTRAY"), BaseForm->GetLangStr("MSG_INSERT_TARGETTRAY"));
	MainForm->memoGripperLineAdd(BaseForm->GetLangStr("MSG_NO_TARGETTRAY"));
	waitTimer->Enabled = false;
}
//---------------------------------------------------------------------------
bool __fastcall Tgripper::getReadyStatus()
{
	bool bresult = false;

	if(MainForm->psrcArrive->Color != clLime)
		return true;

    bresult = true;

	return bresult;
}
//---------------------------------------------------------------------------




