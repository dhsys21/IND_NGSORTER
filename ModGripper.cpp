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
	tool[6].disable = false;	// 7번 그리퍼는 항상 false로 마지막 지점 체크로 사용한다.
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
			MainForm->memoGripperLineAdd("[Waiting] ");
		else
			MainForm->memoGripperLineAdd("[Waiting] Auto mode.");
	}
	else if(MainForm->equipMode == modeManual){
		MainForm->memoGripperLineAdd("[Waiting] Manual mode.");
	}
	else if(MainForm->equipMode == modeAutoStop){
		MainForm->memoGripperLineAdd("[Waiting] Auto stop mode.");
	}
	else if(MainForm->equipMode == modeEmergency){
		MainForm->memoGripperLineAdd("[Waiting] Emergency stop mode.");
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
		AlarmForm->ShowError("[C_Maint] 선별 트레이가 준비되지 않았습니다.", "확인하고 재시작 하세요.");
		return;
	}

	switch(step.step){
		case 0:	// 그리퍼 정보 초기화
			if(MainForm->pwork2->Color != clLime){
				MainForm->memoGripperLineAdd("[C_Maint] [Init step 0] 대상 트레이가 준비되지 않았습니다.");
				return;
			}
			if(MainForm->tray_target.remainCnt == 0){
				if(!waitTimer->Enabled)
				{
					MainForm->NotifyIdMatching_target("2");		// 대상 트레이 선별완료 보고하고
					MainForm->NotifyTransferOut(MainForm->pTrayid_target->Caption);
					MainForm->CmdTrayOut(1);
				}
				MainForm->memoGripperLineAdd("[C_Maint] [Init step 0] 대상 트레이가 가득 참.");

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
						MainForm->memoGripperLineAdd("[B_Ignition] [Init step 0] Gripper  #" + IntToStr(i+1) + " - 셀을 감지 하여 선별을 중단 하였습니다.");
						AlarmForm->ShowError("[B_Ignition] Gripper  #" + IntToStr(i+1) + " - 셀을 감지 하여 선별을 중단 하였습니다.", "확인하고 재시작 하세요.");
						return;
					}

				}
				step.step += 1;
			}else{
				MainForm->memoGripperLineAdd("[Init step 0] Automatic - Stop 상태.");
			}
			waitTimer->Enabled = false;
			break;
		case 1:
        	// 1. 선별 트레이 채널 할당
            // 2. 대상 트레이 채널 할당
            // 3. step.chCnt는 InitSequence에서 0으로 초기화 => 그리퍼 사용변수
			for(int j = step.chCnt; j < 2; j++)
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
							for(int ch = 0; ch < step.chCnt; ++ch){		// 채널이 이미 할당 되지는 않았는지 확인한다
								if(ch < 2)
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

                                //* 2026 03 06 대상 트레이 바깥쪽 부터 채운다.
                                //* 기존 1 -> 6, 7 -> 12 ==> 6 -> 1, 12 -> 7 순으로 변경
                                for(int tch = 11; tch >= 0; --tch){
//								for(int tch=0; tch<12; ++tch){
                                    //* 4번째(nzone == 3) zone 색상이 white.
                                    //* 현재 대상 채널 모두 white 색이기때문에 nzone = 3만 동작 => white가 아니면(이미 셀이 담겨있으면) false.
									if(MainForm->GetZoneChannel(nzone, tch)){
                                        //* 불량셀이 2개 이상, 대상트레이 빈곳 2개 이상인 경우
//                                        if(MainForm->tray_source.remainCnt >= 2 && MainForm->tray_target.remainCnt >= 2){
//                                            bcheck = true;  	// 모든 그리퍼 가능
//                                            switch(tch%2){
//                                                case 0:
//                                                    if(step.chCnt < 0) bcheck = false;
//                                                    break;		// 1번 그리퍼 불가 => 홀수 채널에 1번 그리퍼 사용못함.
//                                                case 1:
//                                                    if(step.chCnt < 1)bcheck = false;
//                                                    break;		// 2번 그리퍼 불가  => 짝수 채널에 2번 그리퍼 사용못함.
//                                            }
//                                        }
                                        //* 불량셀이 2개 이상, 대상트레이 빈곳 2개 이상인 경우
                                        //* 불량셀이 1개면 아래 코드 건너뛰고 모든 그리퍼 가능.
                                        if(MainForm->tray_source.remainCnt >= 2 && MainForm->tray_target.remainCnt >= 2){
                                            bcheck = true; 	// 모든 그리퍼 가능
                                        	if(MainForm->tray_target.remainCnt % 2 == 0){
                                                switch(tch % 2){
                                                    case 0: //* tch = ch - 1, 홀수채널
                                                        if(step.chCnt < 0)
                                                            bcheck = false;
                                                        break;
                                                    case 1: //* tch = ch - 1, 짝수채널,
                                                        if(step.chCnt < 1)
                                                            bcheck = false;
                                                        break;
                                                }
                                            } else if(MainForm->tray_target.remainCnt % 2 == 1){
                                                switch(tch % 2){
                                                    case 0: //* tch = ch - 1, 홀수채널, 1번 그리퍼 불가
                                                        if(step.chCnt < 1)
                                                            bcheck = false;
                                                        break;
                                                    case 1: //* tch = ch - 1, 짝수채널, 모든 그리퍼 가능
                                                        if(step.chCnt < 0)
                                                            bcheck = false;
                                                        break;
                                                }
                                            }

                                        }
										if(bcheck){
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
											MainForm->memoGripperLineAdd("[Init step 1] 선별 채널 : "
												+ MainForm->psort_ch[i]->Caption + " / 대상 채널 : " + IntToStr(tch+1));
											break;
										}
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
					MainForm->memoGripperLineAdd("[Init step 2] 취출 시작.");
				}else{
					MainForm->memoGripperLineAdd("[Init step 2] 취출 할 수 없습니다. : 그리퍼 설정을 확인 하세요.");
					AlarmForm->ShowError("[C_Maint] 선별 트레이를 취출 할 수 없습니다.", "그리퍼 설정을 확인 하세요.");
					req_Init();

				}
			}
			else{
				MainForm->memoGripperLineAdd("[Init step 2] 선별 종료.");
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
		AlarmForm->ShowError("[C_Maint] 선별 트레이가 준비되지 않았습니다.", "확인하고 재시작 하세요.");
		return;
	}

	switch(step.step){
		case 0:
			memset(&eject, 0, sizeof(eject));
			for(int i=0 ; i<gripCnt; ++i){	// 그리퍼 6개
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

					i+=1;	// 취출 그리퍼 다음 그리퍼부터 확인을 하는데
					if(MainForm->tray_source.SLOT_COUNT == 48 && MainForm->tray->CH_GUBUN != 482){		// 48채널 트레이일 경우 한번에 여러셀 뽑기 확인한다.
						for(; i<gripCnt; ++i){							// 선별 선택된 그리퍼를 기준으로
							try{
								pos2 = tool[i].source_ch.ToInt();
							}catch(...){
								pos2 = 0;
							}

                            //* 2025 05 23 한번에 여러셀뽑기 삭제 ( 자기 위치가 아닌데 내려가는 경우 발생함)
//							if( (pos2 - pos1 == 1) && (tool[i].eject_end == false) ){		// NEXT 선별 채널과 한채널 차이가 나고
//								div2 = (pos2-1)/ (MainForm->tray_source.SLOT_COUNT/8);		// 6개 단위로 나뉘어진 트레이 블럭이 같으면
//								if(div1 == div2){
//									eject.conCnt += 1;		// 한번에 여러셀을 뽑는다.
//                                	pos1 = pos2;
//								}
//							}
						}
					}
					break;
				}
			}


			if(eject.pos > 0){	// 취출 시작
				if(MainForm->tray_source.SLOT_COUNT == 96){
					 MainForm->memoGripperLineAdd("[Eject step 0] 96 Tray / Gripper #" + IntToStr(eject.gripper) + " / Channel #" + IntToStr(eject.pos) + " / Continuous eject #" + IntToStr(eject.conCnt));
					 robostar->req_AutoEject(1, eject.gripper , MainForm->mapSort[0][eject.pos-1], eject.conCnt, 962);
					 MainForm->memoGripperLineAdd("[Eject step 0] 96 Channel 취출시작");
					 step.step += 1;
				}
                //* Gamma
				//else if(MainForm->tray_source.SLOT_COUNT == 48 && MainForm->tray->CH_GUBUN == 482){
				else if(MainForm->tray_source.SLOT_COUNT == 48 && MainForm->tray_source.CH_GUBUN == 482){
					MainForm->memoGripperLineAdd("[Eject step 0] 48 Tray / Gripper #" + IntToStr(eject.gripper) + " / Channel #" + IntToStr(eject.pos) + " / Continuous eject #" + IntToStr(eject.conCnt));
					robostar->req_AutoEject(1, eject.gripper , MainForm->mapSort[0][eject.pos-1], eject.conCnt, 482);
					MainForm->memoGripperLineAdd("[Eject step 0] 48 Channel 취출시작");
					step.step += 1;
				}
				//* New Kind (HKMC) 2023 06 26
				//else if(MainForm->tray_source.SLOT_COUNT == 24 && MainForm->tray->CH_GUBUN == 242){
				else if(MainForm->tray_source.SLOT_COUNT == 24 && MainForm->tray_source.CH_GUBUN == 242){
					MainForm->memoGripperLineAdd("[Eject step 0] 24 Tray / Gripper #" + IntToStr(eject.gripper) + " / Channel #" + IntToStr(eject.pos) + " / Continuous eject #" + IntToStr(eject.conCnt));
					robostar->req_AutoEject(1, eject.gripper , MainForm->mapSort[0][eject.pos-1], eject.conCnt, 242);
					MainForm->memoGripperLineAdd("[Eject step 0] 24 Channel 취출시작");
					step.step += 1;
				}
				else if(MainForm->tray_source.SLOT_COUNT == 48 && MainForm->tray->CH_GUBUN == 48){
					MainForm->memoGripperLineAdd("[Eject step 0] 48 Tray / Gripper #" + IntToStr(eject.gripper) + " / Channel #" + IntToStr(eject.pos) + " / Continuous eject #" + IntToStr(eject.conCnt));
					robostar->req_AutoEject(1, eject.gripper , MainForm->mapSort[1][eject.pos-1], eject.conCnt, 48);
					step.step += 1;
				}else{
					MainForm->memoGripperLineAdd("[MES] 등록되지 않은 트레이.");
				}
			}else{
				step.step = 5;
			}
			break;
		case 1:	// 취출중 확인
			MainForm->memoGripperLineAdd("[Eject step 1] 취출...(로봇 작동중)");
			if(robostar->seq == seqAutoEject || robostar->seq == seqAutoEjectComplete)step.step += 1;

			break;
		case 2:	// 취출 완료 확인
			if(robostar->seq == seqAutoEjectComplete){
				MainForm->memoGripperLineAdd("[Eject step 2] 취출 완료.");
				for(int i=eject.gripper; i<eject.gripper + eject.conCnt; ++i){
					tool[i-1].eject_end = true;
					MainForm->DisplaySourceCell(-1, tool[i-1].source_ch.ToInt()-1);	// 화면 show
				}
				step.step += 1;
			}else{
				MainForm->memoGripperLineAdd("[Eject step 2] 취출...(로봇 작동중)");
			}
			break;
		case 3:
			MainForm->memoGripperLineAdd("[Eject step 3] 다음 취출 대상 확인.");
			InitSequence(seqSorting);
			break;
		default:
			MainForm->memoGripperLineAdd("[Eject complete] 삽입 준비.");
			InitSequence(seqInserting);
			break;
	}
}
//---------------------------------------------------------------------------
void __fastcall Tgripper::Inserting()
{
	if(MainForm->pwork2->Color != clLime)
	{
		AlarmForm->ShowError("[C_Maint] 대상 트레이가 준비되지 않았습니다.", "확인 하고 재시작 하세요.");
		return;
	}

	int pos1 = 0, pos2 = 0;
	int div1 = 0, div2 = 0;
	switch(step.step){
		case 0:
			memset(&insert, 0, sizeof(insert));
			for(int i=0; i < gripCnt; ++i){		// 그리퍼 6개
				try{
					pos1 = tool[i].target_ch.ToInt();
				}catch(...){
					pos1 = 0;
				}
				div1 = (pos1-1)/6;       // 대상 트레이 1열에 6개

				if(pos1 > 0 && tool[i].insert_end == false){
					insert.pos = pos1;		// 취출 채널
					insert.conCnt = 1;		// 연속 수량
					insert.gripper = i+1;	// 그리퍼 번호

					for(int cnt=i+1; cnt < gripCnt; ++cnt){
						pos2 = tool[cnt].target_ch.ToInt();
						div2 = (pos2-1)/6; // 대상 트레이 1열에 6개
						if( pos2 - pos1 == 1 && div1 == div2){
							insert.conCnt+= 1;
							pos1 = pos2;
							i++;
						}else{
							i = 100;
							cnt = 100;
						}
					}
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
			MainForm->memoGripperLineAdd("[Insert step 1] 삽입...(로봇 작동중)");
			if(robostar->seq == seqAutoInsert || robostar->seq == seqAutoInsertComplete)
			{
				robostar->m_bInsertSave = false;
				step.step += 1;        // test
			}
//            step.step += 1;
			break;
		case 2:	// 이재 완료 확인
			if(robostar->seq == seqAutoInsertComplete){
				MainForm->memoGripperLineAdd("[Insert step 2] 삽입 완료.");
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
			}else if(robostar->m_bInsertSave){
                for(int i=insert.gripper; i<insert.gripper + insert.conCnt; ++i){
					tool[i-1].insert_end = true;
					MainForm->tray_target.SLOT_ID[tool[i-1].target_ch.ToInt()-1] = MainForm->tray_source.SLOT_ID[tool[i-1].source_ch.ToInt()-1];
					MainForm->tray_target.LOSS_CD[tool[i-1].target_ch.ToInt()-1] = MainForm->tray_source.LOSS_CD[tool[i-1].source_ch.ToInt()-1];
					MainForm->tray_target.PICK[tool[i-1].target_ch.ToInt()-1] = MainForm->tray_source.PICK[tool[i-1].source_ch.ToInt()-1];
					MainForm->tray_target.RANK[tool[i-1].target_ch.ToInt()-1] = MainForm->tray_source.RANK[tool[i-1].source_ch.ToInt()-1];
					MainForm->DisplayTargetCell(-1, tool[i-1].target_ch.ToInt()-1);	// 화면 show
				}
				MainForm->setTrayInfo(1);
				robostar->m_bInsertSave = false;
			}else{
				MainForm->memoGripperLineAdd("[Insert step 2] 삽입...(로봇 작동중)");
			}
			break;
		case 3:
			MainForm->memoGripperLineAdd("[Insert step 3] 다음 삽입 대상을 확인하세요.");
//			MainForm->setTrayInfo(1);
			InitSequence(seqInserting);
			break;
		default:
			MainForm->memoGripperLineAdd("[Insert complete] 다음 취출을 위한 초기화 준비.");
//			MainForm->NotifyIdMatching_target("0");	// 이재 완료시마다 보고
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
	AlarmForm->ShowError("[C_Maint] 대상트레이가 없습니다.", "대상 트레이를 넣어주세요.");
	MainForm->memoGripperLineAdd("대상트레이가 없습니다.");
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




