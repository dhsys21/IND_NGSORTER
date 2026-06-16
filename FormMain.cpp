//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TMainForm *MainForm;

//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
	: TForm(Owner)
{
	this->Parent = BaseForm;
	this->Visible = true;
	this->Left = 15;
	this->Top = 90;

	this->Width = 1900;
	this->Height = 1000;

	MakePanel();
	MakePanel_TargetTray();
	setMapping();
	tray = &tray_target;
	equipMode = modeManual;
    nowLampMode = LampManual;
	InitStep(&step[0]);
	InitStep(&step[1]);

	status_on[1] = pOnX1;
	status_on[2] = pOnY;
	status_on[3] = pOnZ;

	status_org[1] = pOrgX1;
	status_org[2] = pOrgY;
	status_org[3] = pOrgZ;

	status_error[1] = pErrorX1;
	status_error[2] = pErrorY;
	status_error[3] = pErrorZ;

	status_lsp[1] = pLspX1;
	status_lsp[2] = pLspY;
	status_lsp[3] = pLspZ;

	status_lsn[1] = pLsnX1;
	status_lsn[2] = pLsnY;
	status_lsn[3] = pLsnZ;

	status_pos[1] = px1;
	status_pos[2] = py;
	status_pos[3] = pz;


	m_ServoOpen = false;
	m_ServoON = false;
	m_ServoHome = false;
	m_ServoHomeEmg = false;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormShow(TObject *Sender)
{
	for(int i=0; i<2; ++i){
		comBcr[i] = new TBarcode(Owner);
	}
	comBcr[0]->CommOpen("COM2", 0);     	// test
	comBcr[1]->CommOpen("COM1", 1);

	tx = new TX_DATA;
	mes->savePath = (AnsiString)SOCK_LOG;

	stage.init = true;
	stage.limitCnt = 10;
	mdOpen(81,-1,&path);	/*	open 1st CC-Link board		*/
	robostar->config.path = path;
	robostar->io_Init();

	for(int i=0; i<4; ++i){
		color_target[0][i] = clWhite;
		color_target[1][i] = clWhite;
		color_target[2][i] = clWhite;
		color_target[3][i] = clWhite;
	}

	for(int i = 0; i < 24; i++)
		color_target2[i] = clWhite;

	zoneBtn[0] = zone1;
	zoneBtn[1] = zone2;
	zoneBtn[2] = zone3;
	zoneBtn[3] = zone4;
	ReadZoneList();

	senTimer->Enabled = true;
	stepTimer->Enabled = true;
    LampCount = 0;

}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CmdTrayOut(int pos)
{
	Sleep(3000);

	if(robostar->getGripperUpStatus() == false)
	{
		AlarmForm->ShowError("[C_Maint] 그리퍼가 DOWN 상태 입니다.", "상태를 확인하고 트레이를 배출하세요.");
        return;
	}

    if(pos == 0){
		NotifyEquipStatus("IDLE");

		if(tray_source.empTray && tray_source.remainCnt == 0) plcOutput.SRC_EMP = 1;
		else plcOutput.SRC_EMP = 0;

		plcOutput.SRC_OUT = 1;
		plcOutput.SRC_WORK = 0;
	}else{
		plcOutput.TARGET_OUT = 1;
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::targetGridDrawCell(TObject *Sender, int ACol,
	  int ARow, TRect &Rect, TGridDrawState State)
{

	targetGrid->Canvas->Font = targetGrid->Font; // 지정하지 않으면 System 폰트가 됨
	if( State.Contains(gdSelected)){
		targetGrid->Canvas->Brush->Color = clHighlight;
		targetGrid->Canvas->Font->Color = clHighlightText;
	}else{
		targetGrid->Canvas->Brush->Color = color_target[ACol][ARow];
	}
	targetGrid->Canvas->FillRect(Rect);

	DrawText(targetGrid->Canvas->Handle, targetGrid->Cells[ACol][ARow].c_str(), -1, &Rect,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::pause_startBtnClick(TObject *Sender)
{
	if(equipMode == modeAuto)
	{
        if(gripper->pauseStatus && robostar->pauseStatus)
		{
			gripper->req_Pause(false);
			robostar->req_Pause(false);
        }
	}
	else ShowMessage("[C_Maint] 자동-시작 모드가 아닙니다. 시작 모드로 변경하고 재시작 해주세요.");
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::pause_stopBtnClick(TObject *Sender)
{
	gripper->req_Pause(true);
	robostar->req_Pause(true);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::teachingBtnClick(TObject *Sender)
{
    teachForm->Left = 300;
	teachForm->ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::btnScanTargetTrayClick(TObject *Sender)
{
	pTrayid_source->Caption = "스캔중...";
	comBcr[0]->GetBarcode();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::btnScanSourceTrayClick(TObject *Sender)
{
	pTrayid_target->Caption = "스캔중...";
	comBcr[1]->GetBarcode();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::setBarcode(int pos, AnsiString strBcr)
{
	memoMainLineAdd(" 바코드 스캔 완료 : " + strBcr);
	if(strBcr.Length() == 7)
	{
		switch(pos){
			case 0:
				this->pTrayid_source->Caption = strBcr;
				if(plcInput.SRC_ARRIVE){
					NotifyTrayInfo(strBcr, true);
				}
				break;
			case 1:
				this->pTrayid_target->Caption = strBcr;
				if(plcInput.TARGET_READY)NotifyTrayInfo(strBcr, false);
				break;
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::EnableButton_auto(bool benable)
{
	// 자동상태  true인 것들
	playBtn->Enabled = benable;
	stopBtn->Enabled = benable;

	// 자동상태 false인 것들
	openBtn->Enabled = !benable;
	trayout_srcBtn->Enabled = !benable;
	trayout_targetBtn->Enabled = !benable;
	teachingBtn->Enabled = !benable;
	homeBtn->Enabled = !benable;

}
//---------------------------------------------------------------------------
void __fastcall TMainForm::mesTimerTimer(TObject *Sender)
{
	ErrorForm_mes->ShowError(tx->LOT_ID, "[C_Maint] MES 응답 없음", tx->errMsg);
	mesTimer->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::autoBtnClick(TObject *Sender)
{
	if(m_ServoHome)   						  // test
	{
        int servo_speed = teachForm->speedEdit->Text.ToInt();
		int servo_accl_speed = teachForm->acclSpeedEdit->Text.ToInt();
		int servo_dccl_speed = teachForm->dcclSpeedEdit->Text.ToInt();
		robostar->req_Speed(servo_speed, servo_accl_speed, servo_dccl_speed);
		if(manualBtn->Down == true){
			equipMode = modeAutoStop;
			autoBtn->Down = true;
			manualBtn->Down = false;
			playBtn->Down = false;
			stopBtn->Down = true;
			EnableButton_auto(true);
		}else{
			autoBtn->Down = true;
		}
	}
	else
	{
        autoBtn->Down = false;
		AlarmForm->ShowError("[C_Maint] 서보가 준비되지 않았습니다.", "확인하고 재시작 해주세요.");
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::manualBtnClick(TObject *Sender)
{
	if(autoBtn->Down == true){
		if(robostar->input.SAFETY_DOOR_1 || robostar->input.SAFETY_DOOR_2 || robostar->input.SAFETY_DOOR_3 || robostar->input.EMS_SWITCH)
		{
			plcOutput.SRC_MANUAL_WORK = plcInput.SRC_READY;

			equipMode = modeManual;
			nowLampMode = LampManual;
			autoBtn->Down = false;
			manualBtn->Down = true;
			EnableButton_auto(false);
		}
		else
		{
			if(MessageBox(Handle, L"[C_Maint] 모든 작업이 중단되었습니다.\r\n수동 상태로 변환 하시겠습니까?", L"수동", MB_YESNO|MB_ICONQUESTION) == ID_YES){
				gripper->req_Pause(true);
				robostar->req_Pause(true);

				plcOutput.SRC_MANUAL_WORK = plcInput.SRC_READY;

				equipMode = modeManual;
                nowLampMode = LampManual;
				autoBtn->Down = false;
				manualBtn->Down = true;
				EnableButton_auto(false);

			}else{
				autoBtn->Down = true;
				manualBtn->Down = false;
			}
		}
	}else{
		manualBtn->Down = true;
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::playBtnClick(TObject *Sender)
{
	equipMode = modeAuto;
	nowLampMode = LampAuto;
	playBtn->Down = true;
	stopBtn->Down = false;
   
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::stopBtnClick(TObject *Sender)
{
	equipMode = modeAutoStop;
	nowLampMode = LampManual;
	playBtn->Down = false;
	stopBtn->Down = true;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::target_idEditKeyDown(TObject *Sender, WORD &Key,
	  TShiftState Shift)
{
	if(Key == VK_RETURN){
		pTrayid_target->Caption = target_idEdit->Text;
		NotifyTrayInfo(pTrayid_target->Caption, false);	// 대상 트레이 mes 보고
		target_idEdit->Visible = false;
	}
	else if(Key == VK_ESCAPE){
		target_idEdit->Visible = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::pTrayid_sourceDblClick(TObject *Sender)
{
	src_idEdit->Text = pTrayid_source->Caption;
	src_idEdit->Visible = true;
	src_idEdit->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::src_idEditKeyDown(TObject *Sender, WORD &Key,
	  TShiftState Shift)
{
	if(Key == VK_RETURN){
		pTrayid_source->Caption = src_idEdit->Text;
		NotifyTrayInfo(pTrayid_source->Caption, true);	// UF 트레이 mes 보고
		src_idEdit->Visible = false;
	}
	else if(Key == VK_ESCAPE){
    	src_idEdit->Visible = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::pTrayid_targetDblClick(TObject *Sender)
{
	target_idEdit->Text = pTrayid_target->Caption;
	target_idEdit->Visible = true;
	target_idEdit->SetFocus();
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::buzzerBtnClick(TObject *Sender)
{
	BuzzerOn(false);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::openBtnClick(TObject *Sender)
{
	robostar->req_AutoRun();
	if(gripper->seq == 4) gripper->step.step = 0;   //  seqPause
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::ReceivePLC(TMessage &Msg)
{
	AnsiString *data, param;
	int cmd = 0, axis = 0, addr = 0;

	data = (AnsiString*)Msg.LParam;
	addr = Msg.WParam;

	switch(addr){
		case 0x4000:	// PLC WRITE 영역
		case 0x4001:	// PLC WRITE 영역

		case 0x4010:	// PLC WRITE 영역
		case 0x4011:	// PLC WRITE 영역
			plcReadData(*data, addr);
			break;
	}

}
//---------------------------------------------------------------------------
void __fastcall TMainForm::plcReadData(AnsiString str, int addr)
{
	WORD data;
	if(str.IsEmpty())return;

	int row = 0;
	WORD *ptr;
	AnsiString s194 = str.SubString(19,4);
	if(s194 == "0000" && str.Length() >=26){
		str.Delete(1, 22);
		data = ("0x" + str.SubString(1, 4)).ToInt();
		if(addr == 0x4000 || addr == 0x4010){
			ptr = (WORD*)&plcInput;
		   *ptr =data;
        	row = 0;
			for(int col = 0; col < 16; col++){
				if(data & (1 << col))BaseForm->pRead[row][col]->Color = clYellow;
				else BaseForm->pRead[row][col]->Color = clSilver;
			}
		}
		else if(addr == 0x4001 || addr == 0x4011){
//			ptr = (WORD*)&plcOutput;
//		   *ptr =data;
        	row = 1;
			for(int col = 0; col < 16; col++){
				if(data & (1 << col))
					BaseForm->pRead[row][col]->Color = clYellow;
				else
					BaseForm->pRead[row][col]->Color = clSilver;
			}
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::trayout_srcBtnClick(TObject *Sender)
{
	if(plcInput.SRC_ARRIVE){
		if(MessageBox(Handle, L"선별 트레이를 배출하시겠습니까?", L"트레이 배출", MB_YESNO|MB_ICONQUESTION) == ID_YES){
            plcOutput.SRC_MANUAL_WORK = 0;
			CmdTrayOut(0);
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::trayout_targetBtnClick(TObject *Sender)
{
	int reply;
	if(plcInput.TARGET_READY){
		if(MessageBox(Handle, L"대상트레이를 배출 하시겠습니까?", L"트레이 배출", MB_YESNO|MB_ICONQUESTION) == ID_YES){
			reply = MessageBox(Handle, L"MES에 요청 하시겠습니까?", L"MES", MB_YESNOCANCEL|MB_ICONQUESTION);
			if(reply == ID_YES){
				NotifyTransferOut(pTrayid_target->Caption);
				CmdTrayOut(1);
			}else if(reply == ID_NO){
				CmdTrayOut(1);
			}

		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::InitStep(STEP *data)
{
	data->step = 0;
	data->cnt = 0;
	data->timeout = 0;
}
//---------------------------------------------------------------------------
// 원본
void __fastcall TMainForm::stepTimerTimer(TObject *Sender)
{
	if(equipMode != modeAuto){
		memoMainLineAdd("[C_Maint] AUTO 모드가 아닙니다.");
		return;
	}

	if(plcInput.SRC_ARRIVE == 0)InitStep(&step[0]);             // test
	if(plcInput.TARGET_READY == 0)InitStep(&step[1]);           // test

    if(!gripper->pauseStatus && !robostar->pauseStatus)
	{
		switch(step[0].step){
			case 0:
				if(plcInput.SRC_ARRIVE){
					NotifyEquipStatus("PROCESS");
					if(chkBypass->Checked == false){
						pTrayid_source->Caption = "";       // test
						pTrayid_source2->Caption = "";      // test
						memoMainLineAdd("선별 트레이 바코드 스캔.");
						comBcr[0]->GetBarcode();	// 작업1. 선별 바코드 읽고  -> DisplayTrayInfo  	// test
						step[0].step += 1;
					}else{
						memoMainLineAdd("바이패스 설정 - 트레이 배출.");
						CmdTrayOut(0);
						step[0].step += 100;
					}
				}
				else{
					if(plcInput.TARGET_READY && pTrayid_target->Caption.IsEmpty()){
						memoMainLineAdd("대상 트레이 바코드 스캔.");
						comBcr[1]->GetBarcode();                                            // test
					}else{
						memoMainLineAdd("선별 트레이 도착 기다림.");
					}
				}
				break;
			case 1:
				if(plcInput.SRC_READY){
					memoMainLineAdd("선별 트레이 센터링 완료.");
					NotifyTransferIn(pTrayid_source->Caption);	// 작업3. 센터링을 치면 작업시작 보고를 한다.
					step[0].step += 1;
				}else{
					memoMainLineAdd("선별 트레이 센터링.");
				}
				break;
			case 2:
				if(plcInput.TARGET_READY){
					memoMainLineAdd("대상 트레이 센터링 완료.");
					pTrayid_target->Caption = "";       // test
					pTrayid_target2->Caption = "";      // test
					comBcr[1]->GetBarcode();		// 작업4. 센터링이 되어 있으면 바코드를 읽고 -> DisplayTrayInfo 	// test
					step[0].step += 1;
					step[1].step = 1;
				}else{
					memoMainLineAdd("대상트레이가 없거나 센터링 되지 않았습니다.");
					AlarmForm->ShowError("[C_Maint] 대상 트레이 없음", L"대상 트레이 또는 센터링 상태 확인.");
				}
			default:
				break;
		}


		 switch(step[1].step){
			case 0:
				if(plcInput.TARGET_READY && pwork1->Color == clLime){
					memoMainLineAdd("More target trays arrived.");
					pTrayid_target->Caption = "";
					pTrayid_target2->Caption = "";
					comBcr[1]->GetBarcode();		// 작업4. 센터링이 되어 있으면 바코드를 읽고 -> DisplayTrayInfo     // test
					step[1].step += 1;
				}
			default:
				break;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::chkBypassClick(TObject *Sender)
{
	if(chkBypass->Checked)this->InitStep(&step[0]);	
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::resetBtnClick(TObject *Sender)
{
	BuzzerOn(false);
	MainForm->LampModeChange(MainForm->beforeLampMode);
	robostar->req_Reset();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::homeBtnClick(TObject *Sender)
{
    UnicodeString msg1 = "[C_Maint] 대기상태로 이동 후 다시 시작하세요. \r\n부하율 : 58%, z축 위치 : 55000";
    MainForm->memoRobostarLineAdd("Z 축 이동실패" + msg1);
    AlarmForm->ShowError("[C_Maint] Z 축 이동실패", msg1);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::senTimerTimer(TObject *Sender)
{
	for(int i=0; i<=3; ++i)GetZoneCount(i);

	sensorColor(pcell1, robostar->input.GRIPPER1_CELL_DETECT);
	sensorColor(pup1, robostar->input.GRIPPER1_UP);
	sensorColor(pdn1, robostar->input.GRIPPER1_DOWN);
	sensorColor(pflow1, robostar->input.GRIPPER1_BUFFER);
	sensorColor(popen1, !robostar->output.GRIPPER1_CHUCK);
	sensorColor(pclose1, robostar->output.GRIPPER1_CHUCK);

    //* 2025 09 25 추가
	sensorColor(pRun, robostar->output.SERVO_RUNNING);
	if(robostar->output.SERVO_RUNNING == true)
		pRun->Color = clLime;
	else
		pRun->Color = clGray;

	setLamp();

	int flowValue = robostar->CheckFlow();
	if(flowValue != 0)
	{
		robostar->req_JogMove(-1);
		AlarmForm->ShowError("[B_Ignition] No." + IntToStr(flowValue) + " 그리퍼 쿠션 센서가 감지되었습니다.", "확인하고 재시작 해주세요.");
	}

	if(robostar->mr2.system_status == SSC_STS_CODE_RUNNING) popen->Color = clLime;
	else popen->Color = clSilver;

	bool NGflag = false;
	if(robostar->mr2.system_detail == 0)
	{
		if(robostar->mr2.speed[Axis_x] < 0) robostar->mr2.speed[Axis_x] *= -1;
		if(robostar->mr2.speed[Axis_y] < 0) robostar->mr2.speed[Axis_y] *= -1;
		pspeed->Caption = FloatToStr(robostar->mr2.speed[Axis_x]) + " / " + FloatToStr(robostar->mr2.speed[Axis_y]);
        //* servoCnt = 3, X, Y, Z 총3개
		for(int i = 1; i <= servoCnt; ++i){
			loadfactorForm->Panel_Position[i]->Caption = FormatFloat("0 %", robostar->mr2.mondata[i][0]);
			teachForm->lblLoadFactor[i]->Caption = FormatFloat("0 %", robostar->mr2.mondata[i][0]);
			status_pos[i]->Caption = FloatToStr(robostar->mr2.pos[i]);
			if(robostar->mr2.servo[i]) status_on[i]->Color = clLime;
			else status_on[i]->Color = clSilver;

			if(!robostar->mr2.zero[i] && popen->Color == clLime) status_org[i]->Color = clLime;
			else status_org[i]->Color = clSilver;

			if(robostar->mr2.limit[i] & SSC_BIT_LSP) status_lsp[i]->Color = clSilver;
			else status_lsp[i]->Color = clRed;

			if(robostar->mr2.limit[i] & SSC_BIT_LSN) status_lsn[i]->Color = clSilver;
			else status_lsn[i]->Color = clRed;
		}

		for(int i = 1; i <= servoCnt; ++i){
			if(robostar->mr2.servo_alarm[i] > 0){
				perr->Color = clRed;
				perr->Caption = IntToHex(robostar->mr2.servo_alarm[i], 2);
				status_error[i]->Color = clRed;
				NGflag = true;
				break;
			}
			else status_error[i]->Color = clSilver;
			if(robostar->mr2.oper_alarm[i] > 0){
				perr->Color = clRed;
				perr->Caption = IntToHex(robostar->mr2.oper_alarm[i], 2);
				status_error[i]->Color = clRed;
				NGflag = true;
				break;
			}
			else status_error[i]->Color = clSilver;
		}
	}
	else
	{
		NGflag = true;
		perr->Color = clRed;
		popen->Color = clRed;
		perr->Caption = IntToHex(robostar->mr2.system_detail, 8);
	}

	for(int i = 1; i <= servoCnt; ++i){
		if(robostar->mr2.mondata[i][0] > loadfactorForm->m_SetLimit)
		{
			loadfactorForm->m_Count++;
			if(loadfactorForm->m_Count > 10)
			{
				loadfactor_AlarmForm->ShowError("[C_Maint] 부하율이 제한설정을 넘었습니다.", "서보 상태를 확인해 주세요.");
				break;
			}
		}
	}

	if(popen->Color == clLime)
	{
		m_ServoOpen = true;

		if(status_on[Axis_x]->Color == clLime && status_on[Axis_y]->Color == clLime
			&& status_on[Axis_z]->Color == clLime)
		{
			m_ServoON = true;

			if(status_org[Axis_x]->Color == clLime && status_org[Axis_y]->Color == clLime
				&& status_org[Axis_z]->Color == clLime) {
				m_ServoHome = true;
			}
			else {
				m_ServoHome = false;
			}
            if(status_org[Axis_x]->Color == clLime && status_org[Axis_y]->Color == clLime
				&& status_org[Axis_z]->Color == clLime) {
				m_ServoHomeEmg = true;
			}
			else {
				m_ServoHomeEmg = false;
			}
		}
		else
		{
			m_ServoON = false;
			m_ServoHome = false;
            m_ServoHomeEmg = false;
		}
	}
	else
	{
		m_ServoOpen = false;
		m_ServoON = false;
		m_ServoHome = false;
        m_ServoHomeEmg = false;
	}

	if(teachForm->Visible){
		teachForm->pcell1->Color = pcell1->Color;
		teachForm->pcell2->Color = pcell2->Color;

		teachForm->pup1->Color = pup1->Color;
		teachForm->pup2->Color = pup2->Color;

		teachForm->pdn1->Color = pdn1->Color;
		teachForm->pdn2->Color = pdn2->Color;

		teachForm->pflow1->Color = pflow1->Color;
		teachForm->pflow2->Color = pflow2->Color;

		teachForm->popen1->Color = popen1->Color;
		teachForm->popen2->Color = popen2->Color;

		teachForm->pclose1->Color = pclose1->Color;
		teachForm->pclose2->Color = pclose2->Color;

		teachForm->popen->Color = popen->Color;
		teachForm->pOnX1->Color = pOnX1->Color;
		teachForm->pOnY->Color = pOnY->Color;
		teachForm->pOnZ->Color = pOnZ->Color;
		teachForm->pOnG1->Color = pOnG1->Color;
		teachForm->pOrgX1->Color = pOrgX1->Color;
		teachForm->pOrgY->Color = pOrgY->Color;
		teachForm->pOrgZ->Color = pOrgY->Color;
		teachForm->pOrgG1->Color = pOrgG1->Color;
		teachForm->pErrorX1->Color = pErrorX1->Color;
		teachForm->pErrorY->Color = pErrorY->Color;
		teachForm->pErrorZ->Color = pErrorZ->Color;
		teachForm->pErrorG1->Color = pErrorG1->Color;
		teachForm->pLspX1->Color = pLspX1->Color;
		teachForm->pLspY->Color = pLspY->Color;
		teachForm->pLspZ->Color = pLspZ->Color;
		teachForm->pLspG1->Color = pLspG1->Color;
		teachForm->pLsnX1->Color = pLsnX1->Color;
		teachForm->pLsnY->Color = pLsnY->Color;
		teachForm->pLsnZ->Color = pLsnZ->Color;
		teachForm->pLsnG1->Color = pLsnG1->Color;

		teachForm->px1->Caption = px1->Caption;
		teachForm->py->Caption = py->Caption;
		teachForm->pz->Caption = pz->Caption;
		teachForm->pg1->Caption = pg1->Caption;
		teachForm->pspeed->Caption = pspeed->Caption;
	}

	if(ErrorForm_eject->Visible){
		ErrorForm_eject->pcell1->Color = pcell1->Color;
		ErrorForm_eject->pcell2->Color = pcell2->Color;

		ErrorForm_eject->pup1->Color = pup1->Color;
		ErrorForm_eject->pup2->Color = pup2->Color;

		ErrorForm_eject->pdn1->Color = pdn1->Color;
		ErrorForm_eject->pdn2->Color = pdn2->Color;

		ErrorForm_eject->pflow1->Color = pflow1->Color;
		ErrorForm_eject->pflow2->Color = pflow2->Color;

		ErrorForm_eject->popen1->Color = popen1->Color;
		ErrorForm_eject->popen2->Color = popen2->Color;

		ErrorForm_eject->pclose1->Color = pclose1->Color;
		ErrorForm_eject->pclose2->Color = pclose2->Color;
	}

	if(ErrorForm_insert->Visible){
		ErrorForm_insert->pcell1->Color = pcell1->Color;
		ErrorForm_insert->pcell2->Color = pcell2->Color;

		ErrorForm_insert->pup1->Color = pup1->Color;
		ErrorForm_insert->pup2->Color = pup2->Color;

		ErrorForm_insert->pdn1->Color = pdn1->Color;
		ErrorForm_insert->pdn2->Color = pdn2->Color;

		ErrorForm_insert->pflow1->Color = pflow1->Color;
		ErrorForm_insert->pflow2->Color = pflow2->Color;

		ErrorForm_insert->popen1->Color = popen1->Color;
		ErrorForm_insert->popen2->Color = popen2->Color;

		ErrorForm_insert->pclose1->Color = pclose1->Color;
		ErrorForm_insert->pclose2->Color = pclose2->Color;
	}

    for(int i = 0; i < gripCnt; i++){
        pcode1->Caption = gripper->tool[i].code;
        psource_ch1->Caption = gripper->tool[i].source_ch;
        ptarget_ch1->Caption = gripper->tool[i].target_ch;
        puse1->Visible = gripper->tool[i].disable;
    }

	if( robostar->input.SAFETY_DOOR_1)pdoor_left->Color = clRed;
	else pdoor_left->Color = clSilver;

	if(robostar->input.SAFETY_DOOR_1)pdoor_right->Color = clRed;
	else pdoor_right->Color = clSilver;

	if(robostar->input.EMS_SWITCH)pemergency->Color = clRed;
	else pemergency->Color = clSilver;

	if(gripper->pauseStatus)ppause->Color = clRed;
	else ppause->Color = clSilver;

	if(NGflag) AlarmForm->ShowError("[C_Maint] ROBOT Alarm Occurred (Error Code : " +  perr->Caption + ")", "Please RESET.");
	else
	{
        perr->Caption = "";
		perr->Color = clWhite;
	}

	//* 테스트 위해 주석처리 2019 05 15
	if(robostar->input.SAFETY_DOOR_1)
		doorForm->ShowError("[C_Maint] DOOR #1 Open", "문을 닫아 주세요.", 0);
	if(robostar->input.SAFETY_DOOR_2)
		doorForm->ShowError("[C_Maint] DOOR #2 Open", "문을 닫아 주세요.", 1);
	if(robostar->input.SAFETY_DOOR_3)
		doorForm->ShowError("[C_Maint] DOOR #3 Open", "문을 닫아 주세요.", 3);
	if(robostar->input.EMS_SWITCH)
		doorForm->ShowError("[A_Safety] 비상정지", "비상정지 스위치를 확인해 주세요.", 2);
	if(!robostar->gripper.DOOR_OPEN_SELECT)
	{
		if(equipMode == modeAuto)
			doorForm->ShowError("[C_Maint] KEYLOCK 해제", "KEYLOCK 을 설정해 주세요.", 4);
		else
            robostar->req_Speed(200, 3000, 3000);
	}

	if(popen->Color != clLime)
		doorForm->ShowError("[C_Maint] RESET", "서보를 켜고 OPEN을 클릭 해주세요", 5);


	pejectremainCnt->Caption = tray_source.remainCnt;
	pinsertremainCnt->Caption = tray_target.remainCnt;

	//* for mes test START
	if(CheckBox1->Checked){
		plcInput.SRC_READY = 1;
		plcInput.TARGET_READY = 1;

		plcInput.SRC_ARRIVE = 1;
		psrcArrive->Color = clLime;
		psrcReady->Color = clLime;

		plcInput.TARGET_READY = 1;
		ptargetReady->Color = clLime;

		pTrayid_source2->Caption = "MPA0001";
		pTrayid_target2->Caption = "NG10006";
	}
	//* FOR MES TEST END

	/*
	* 테스트 위해 아래 코드 주석처리
	*/
	if(plcInput.SRC_OUT)
	{
        plcOutput.SRC_EMP = 0;
		plcOutput.SRC_OUT = 0;
	}
	if(plcInput.TARGET_OUT)plcOutput.TARGET_OUT = 0;

	if(plcInput.SRC_READY)
		plcOutput.SRC_WORK = 0;

	if(plcInput.SRC_ARRIVE)psrcArrive->Color = clLime;   		   // test
	else{
		psrcArrive->Color = clSilver;
		pwork1->Color = clSilver;
		pwork2->Color = clSilver;
		psrcReady->Font->Color = clBlack;
	}
	if(plcInput.SRC_READY)  // gsm test 2018 09 14
	{
		psrcReady->Color = clLime;
	}
	else{
		psrcReady->Color = clSilver;
		psrcReady->Font->Color = clBlack;
	}
	if(plcInput.TARGET_READY)ptargetReady->Color = clLime;
	else{
		ptargetReady->Color = clSilver;
		pwork2->Color = clSilver;
	}

	if(plcOutput.SRC_OUT)psrcOut->Color = clLime;
	else	psrcOut->Color = clSilver;

	if(plcOutput.TARGET_OUT)ptargetOut->Color = clLime;
	else	ptargetOut->Color = clSilver;

	if(plcInput.SRC_OUT)psrcOut->Font->Color = clRed;
	else	psrcOut->Font->Color = clBlack;

	if(plcInput.TARGET_OUT)ptargetOut->Font->Color = clRed;
	else	ptargetOut->Font->Color = clBlack;

	if(equipMode == modeAuto)
	{
		plcOutput.AUTO_RUN = 1;
		plcOutput.SRC_MANUAL_WORK = 0;   // for TEST
	}
	else plcOutput.AUTO_RUN = 0;

	WORD nData = 0;

	nData = *((WORD *)&plcOutput + 0);
	plc->WriteWordData(0x4001, 1, IntToHex(nData, 4));

}
//---------------------------------------------------------------------------




//---------------------------------------------------------------------------
void __fastcall TMainForm::BuzzerOn(bool on)
{
	robostar->gripper.TOWER_LAMP_BUZZER = on;
	if(on && (nowLampMode != LampEmergency) && (nowLampMode != LampAlarm))
		beforeLampMode = nowLampMode;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::LampModeChange(LampMode mode)
{
	nowLampMode = mode;
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::btnApplyNgLimitCountClick(TObject *Sender)
{
	try{
		stage.limitCnt = limitEdit->Text.ToInt();
		MessageBox(Handle, L"[C_Maint] NG limit 값이 설정 되었습니다.", L"NG limt", MB_OK|MB_ICONINFORMATION);
	}
	catch(...){
		MessageBox(Handle, L"[C_Maint] 값을 확인해 주세요.", L"경고", MB_OK|MB_ICONWARNING);
	}

}
//---------------------------------------------------------------------------

void __fastcall TMainForm::zone1Click(TObject *Sender)
{
	TAdvSmoothButton *btn;
	btn = (TAdvSmoothButton*)Sender;
	TGridRect sRec;
	sRec = targetGrid->Selection;


	int nch = 0;
	if(MessageBox(Handle, L"Do you want to set it to the area you selected?", L"Area select", MB_YESNO|MB_ICONQUESTION) == ID_YES){

		for(int nrow = sRec.Left; nrow<=sRec.Right; ++nrow){
			for(int ncol = sRec.Top; ncol<=sRec.Bottom; ++ncol){
				nch = ( (3 - nrow) *6) + (ncol%6);
				pt_ch[nch]->Fill->Color = btn->Color;
				pt_ch[nch]->Fill->ColorMirror = btn->Color;
				pt_ch[nch]->Fill->ColorMirrorTo = btn->Color;
				pt_ch[nch]->Fill->ColorTo = btn->Color;
			}
		}
		WriteZoneList();
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::WriteZoneList()
{
	AnsiString file;
	file = (AnsiString)BIN + "zone.ini";

	ini = new TIniFile(file);
		for(int i=0; i<12; ++i){
			ini->WriteInteger("COLOR", i, pt_ch[i]->Fill->Color);

		}
	delete ini;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ReadZoneList()
{
	AnsiString file;
	file = (AnsiString)BIN + "zone.ini";
	TColor clr;
	ini = new TIniFile(file);
		for(int i=0; i<12; ++i){
			clr = (TColor)ini->ReadInteger("COLOR", i, clWhite);
			pt_ch[i]->Fill->Color = clr;
			pt_ch[i]->Fill->ColorMirror = clr;
			pt_ch[i]->Fill->ColorMirrorTo = clr;
			pt_ch[i]->Fill->ColorTo = clr;
		}
	delete ini;
}
//---------------------------------------------------------------------------

int __fastcall TMainForm::GetZoneCount(int zone)
{
	int ncnt = 0;
	int ntotal = 0;

	for(int i=0; i<12; ++i){
		if(pt_ch[i]->Fill->Color == zoneBtn[zone]->Color){
			ntotal += 1;	// zone 총 수량
			if(tray_target.SLOT_ID[i].IsEmpty() && tray_target.PICK[i] == "N")
				ncnt += 1;	// empty 수량
		}
	}

	if(ntotal == 0)ncnt = -1;
	return ncnt;
}
//---------------------------------------------------------------------------
bool __fastcall TMainForm::GetZoneChannel(int zone, int ch)
{
	if(tray_target.PICK[ch] == "N" && pt_ch[ch]->Fill->Color == zoneBtn[zone]->Color)return true;
	else return false;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMainForm::GetAlarmMsg(int code)
{
	switch(code){
		case 1: return "Warning_MES로부터 선별 TRAY정보응답 ERROR"; break;
		case 2: return "Warning_MES로부터 선별 TRAY_REPLY_S 응답시간 초과"; break;
		case 3: return "Warning_MES로부터 대상 TRAY정보응답 ERROR"; break;
		case 4: return "Warning_MES로부터 대상 TRAY_REPLY_T 응답시간 초과"; break;
		case 5: return "Warning_MES로부터 대상 ID_MATCHING 응답 Error"; break;
		case 6: return "Warning_MES로부터 대상 ID_MATCHING 응답시간 초과"; break;
		case 7: return "Warning_MES로부터 선별 TRAY TRANSFER_OUT 응답 Error"; break;
		case 8: return "Warning_MES로부터 선별 TRAY TRANSFER_OUT 응답시간 초과"; break;
		case 9: return "Warning_MES로부터 대상 TRAY TRANSFER_OUT 응답 Error"; break;
		case 10: return "Warning_MES로부터 대상 TRAY TRANSFER_OUT 응답시간 초과"; break;
		case 11: return "Warning_MES로부터 SEND_EVENT ERROR"; break;
		case 12: return "Warning_MES로부터 SEND_EVENT 응답시간 초과"; break;
		case 13: return "Warning_MES로부터 선별 ID_MATCHING 응답 Error"; break;
		case 14: return "Warning_MES로부터 선별 ID_MATCHING 응답시간 초과"; break;
		case 15: return "C_Maint_[선별TRAY] GRIP DOWN센서 감지 시간초과"; break;
		case 16: return "C_Maint_[선별TRAY] GRIP UP센서 감지 시간초과"; break;
		case 17: return "C_Maint_[선별TRAY] GRIP DOWN시 충돌되었습니다"; break;
		case 18: return "C_Maint_[대상TRAY] GRIP UNCHUCK 센서 감지시간 초과"; break;
		case 19: return "C_Maint_[대상TRAY] GRIP UP 센서 감지시간 초과"; break;
		case 20: return "C_Maint_[선별TRAY] Cell No, Grip No에 셀이 없음"; break;
		case 21: return "C_Maint_[대상TRAY] GRIP DOWN시 충돌되었습니다"; break;
		case 22: return "C_Maint_[대상TRAY] GRIP DOWN시간 초과"; break;
		case 23: return "C_Maint_[대상TRAY] Cell No, Grip No 에 셀이 없음"; break;
		case 24: return "Warning_불량수가 설정치 보다 많습니다"; break;
		case 25: return "Warning_DOOR #1 Open"; break;
		case 26: return "Warning_DOOR #2 Open"; break;
		case 27: return "A_Safety_Emergency stop"; break;
		case 28: return "Warning_등록되지 않은 기종 입니다"; break;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::memoMainLineAdd(AnsiString msg)
{
	if(pmainMsg->Hint != msg){
		pmainMsg->Caption = msg;
		pmainMsg->Hint = msg;
		WriteProgLog(msg);
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::memoGripperLineAdd(AnsiString msg)
{
	if(pgripperMsg->Hint != msg){
		pgripperMsg->Caption = msg;
		pgripperMsg->Hint = msg;
		WriteProgLog(msg);
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::memoRobostarLineAdd(AnsiString msg)
{
	if(probostarMsg->Hint != msg){
		probostarMsg->Caption = msg;
		probostarMsg->Hint = msg;
		WriteProgLog(msg);
	}
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::AdvSmoothToggleButton_InitWorkClick(TObject *Sender)
{
	if(gripper->pauseStatus && robostar->pauseStatus)
	{
		if(MessageBox(Handle, L"작업을 초기화 하시겠습니까?",
			L"초기화", MB_YESNO|MB_ICONQUESTION) == ID_YES)
		{
            gripper->seq_save = seqIdle;
			robostar->seq_save = seqIdle;

            plcOutput.SRC_WORK = 0;

			InitStep(&step[0]);
			InitStep(&step[1]);

			pwork1->Color = clSilver;
			pwork2->Color = clSilver;
		}
	}else ShowMessage("[C_Maint] 일시정지 상태에서만 작업초기화를 할 수 있습니다.");
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TMainForm::AdvSmoothToggleButton2Click(TObject *Sender)
{
    plcOutput.AUTO_RUN = 1;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AdvSmoothToggleButton3Click(TObject *Sender)
{
    plcOutput.SRC_WORK = 1;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AdvSmoothToggleButton7Click(TObject *Sender)
{
	plcOutput.SRC_MANUAL_WORK = !plcOutput.SRC_MANUAL_WORK;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AdvSmoothToggleButton5Click(TObject *Sender)
{
    plcOutput.SRC_OUT = 1;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AdvSmoothToggleButton8Click(TObject *Sender)
{
    plcOutput.SRC_OUT = 0;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AdvSmoothToggleButton6Click(TObject *Sender)
{
    plcOutput.TARGET_OUT = 1;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AdvSmoothToggleButton9Click(TObject *Sender)
{
    plcOutput.TARGET_OUT = 0;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AdvSmoothToggleButton17Click(TObject *Sender)
{
	comBcr[0]->GetBarcode();
	comBcr[1]->GetBarcode();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AdvSmoothToggleButton10Click(TObject *Sender)
{
    plcInput.SRC_READY = 1;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AdvSmoothToggleButton16Click(TObject *Sender)
{
    plcInput.SRC_READY = 0;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AdvSmoothToggleButton11Click(TObject *Sender)
{
    plcInput.SRC_ARRIVE = 1;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AdvSmoothToggleButton14Click(TObject *Sender)
{
    plcInput.SRC_ARRIVE = 0;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AdvSmoothToggleButton12Click(TObject *Sender)
{
    plcInput.TARGET_READY = 1;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::AdvSmoothToggleButton13Click(TObject *Sender)
{
    plcInput.TARGET_READY = 0;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::Panel70Click(TObject *Sender)
{
    badCode->Visible = !badCode->Visible;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::lblTitleClick(TObject *Sender)
{
    CheckBox1->Visible = !CheckBox1->Visible;
    if(CheckBox1->Visible == false)
        CheckBox1->Checked = false;
}
//---------------------------------------------------------------------------




