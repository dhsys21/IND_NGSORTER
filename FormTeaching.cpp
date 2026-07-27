#pragma link "AdvSmoothToggleButton"
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TteachForm *teachForm;
//---------------------------------------------------------------------------
__fastcall TteachForm::TteachForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::FormCreate(TObject *Sender)
{
	//* Load Factor
	lblLoadFactor[1] = lblLoadFactor1;
	lblLoadFactor[2] = lblLoadFactor2;
	lblLoadFactor[3] = lblLoadFactor3;
	lblLoadFactor[4] = lblLoadFactor4;

    MakePanel();
    sCombo->ItemIndex = 0;

	teachingFilePath = (AnsiString)BIN + "KindTeaching.ini";
	LoadTeaching(teachingFilePath);
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::FormShow(TObject *Sender)
{
	this->Left = 300;
	this->Top = 0;

    pnlMovingAlarm2->Align = alClient;
    pnlMovingAlarm->Align = alClient;

    robostar->io_Init();
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::MakePanel()
{
    int sx = 0, sy = 0, tx = 0, ty = 0;
    int nh = s1->Height;
    int nw = s1->Width;
    sx = s1->Left;
    sy = s1->Top;
    tx = t1->Left;
    ty = t1->Top;

    for(int i = 0; i < TraySlotCount;)
    {
        sTray[i] = new TAdvSmoothPanel(this);
        sTray[i]->Parent = pnlSourceBase;
        sTray[i]->Width = s1->Width;
        sTray[i]->Height = s1->Height;
        sTray[i]->Fill->Color = s1->Fill->Color;
        sTray[i]->Fill->ColorMirror = s1->Fill->ColorMirror;
        sTray[i]->Fill->ColorMirrorTo = s1->Fill->ColorMirrorTo;
        sTray[i]->Fill->ColorTo = s1->Fill->ColorTo;
        sTray[i]->Fill->BorderWidth = s1->Fill->BorderWidth;
        sTray[i]->Fill->Rounding = s1->Fill->Rounding;
        sTray[i]->Fill->BorderColor = s1->Fill->BorderColor;

        sTray[i]->Caption->Assign(s1->Caption);
        sTray[i]->Caption->Text = IntToStr(i + 1);
        sTray[i]->Tag = i + 1;
        sTray[i]->Left = sx;
        sTray[i]->Top = sy;
        sTray[i]->OnClick = sClick;

        sy = sy - nh - 1;
		i += 1;
        if(i % 12 == 0) sy -= 3;
		if(i % 24 == 0){
			sx = sx + nw + 1;
			sy = s1->Top;
		}
    }

    for(int i = 0; i < TraySlotCount;)
    {
        tTray[i] = new TAdvSmoothPanel(this);
        tTray[i]->Parent = pnlTargetBase;
        tTray[i]->Width = t1->Width;
        tTray[i]->Height = t1->Height;
        tTray[i]->Fill->Color = t1->Fill->Color;
        tTray[i]->Fill->ColorMirror = t1->Fill->ColorMirror;
        tTray[i]->Fill->ColorMirrorTo = t1->Fill->ColorMirrorTo;
        tTray[i]->Fill->ColorTo = t1->Fill->ColorTo;
        tTray[i]->Fill->BorderWidth = t1->Fill->BorderWidth;
        tTray[i]->Fill->Rounding = t1->Fill->Rounding;
        tTray[i]->Fill->BorderColor = t1->Fill->BorderColor;

        tTray[i]->Caption->Assign(t1->Caption);
        tTray[i]->Caption->Text = IntToStr(i + 1);
        tTray[i]->Tag = i + 1;
        tTray[i]->Left = tx;
        tTray[i]->Top = ty;
        tTray[i]->OnClick = tClick;

        ty = ty - nh - 1;
		i += 1;
        if(i % 12 == 0) ty -= 3;
		if(i % 24 == 0){
			tx = tx + nw + 1;
			ty = t1->Top;
		}
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//                  Event Handler
//---------------------------------------------------------------------------
void __fastcall TteachForm::sClick(TObject *Sender)
{
	TAdvSmoothPanel *pnl;
	pnl = (TAdvSmoothPanel*)Sender;
	UnicodeString str;

	if(MainForm->psrcReady->Color != clLime)
	{
		if(MessageBox(Handle, BaseForm->GetLangStr("MSG_SOURCETRAY_CENTERING_Q").c_str(),
			L"Centering DOWN", MB_YESNO|MB_ICONQUESTION) == ID_YES)
		{
			MainForm->plcOutput.SRC_MANUAL_WORK = 1;
		}
	}
	else
	{
		int ch = pnl->Tag;
        if(CheckMoveSourceChannel() == false){
			ShowMessage(BaseForm->GetLangStr("MSG_GRIPPER_MOVE_ERR") + IntToStr(ch));
        } else{
            str = "[" + sCombo->Text + "] " + BaseForm->GetLangStr("MSG_SOURCETRAY_MOVE_Q") + pnl->Caption->Text;
            if(MessageBox(Handle, str.c_str(), L"MOVE", MB_YESNO|MB_ICONQUESTION) == ID_YES){
				robostar->req_AutoMove(1, 1, ch, 962);

                for(int i = 0; i < TraySlotCount; ++i){
                    sTray[i]->Fill->Color = clWhite;
                    sTray[i]->Fill->ColorTo = clWhite;
                    sTray[i]->Fill->ColorMirror = clWhite;
                    sTray[i]->Fill->ColorMirrorTo = clWhite;
                }
                pnl->Fill->Color = pselect->Color;
                pnl->Fill->ColorTo = pselect->Color;
                pnl->Fill->ColorMirror = pselect->Color;
                pnl->Fill->ColorMirrorTo = pselect->Color;
            }
        }
	}
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::tClick(TObject *Sender)
{
	TAdvSmoothPanel *pnl;
	pnl = (TAdvSmoothPanel*)Sender;
	UnicodeString str;

	if(MainForm->psrcReady->Color != clLime)
	{
		if(MessageBox(Handle, BaseForm->GetLangStr("MSG_SOURCETRAY_CENTERING_Q").c_str(),
											L"Centering DOWN", MB_YESNO|MB_ICONQUESTION) == ID_YES)
			MainForm->plcOutput.SRC_MANUAL_WORK = 1;
	}
	else
	{
        //* 그리퍼 선택 그리퍼 1 : sCombo->ItemIndex = 0, 그리퍼 2 : sCombo->ItemIndex = 1
        //* 그리퍼에 셀이 있고 robostar->input.GRIPPER1_CELL_DETECT == true, robostar->input.GRIPPER2_CELL_DETECT == true
		//* 대상트레이 해당 채널에 셀이 있으면 color_target[i/6][5-(i%6)] = clSilver; color_target[ch/6][5-(ch%6)] = clInactiveCaption;
		//* 이동채널 pnl->Caption->Text
		int ch = pnl->Tag;
        if(CheckMoveTargetChannel(ch-1) == false){
			ShowMessage(BaseForm->GetLangStr("MSG_GRIPPER_MOVE_ERR2")  + IntToStr(ch));
        } else{
			str = "[" + sCombo->Text + "] " + BaseForm->GetLangStr("MSG_TARGETTRAY_MOVE_Q") + pnl->Caption->Text;
            if(MessageBox(Handle, str.c_str(), L"MOVE", MB_YESNO|MB_ICONQUESTION) == ID_YES){
                robostar->req_AutoMove(2, 1, ch, 96);
                for(int i = 0; i < TraySlotCount; ++i){
                    tTray[i]->Fill->Color = clWhite;
                    tTray[i]->Fill->ColorTo = clWhite;
                    tTray[i]->Fill->ColorMirror = clWhite;
                    tTray[i]->Fill->ColorMirrorTo = clWhite;
                }
				pnl->Fill->Color = pselect->Color;
                pnl->Fill->ColorTo = pselect->Color;
                pnl->Fill->ColorMirror = pselect->Color;
                pnl->Fill->ColorMirrorTo = pselect->Color;
            }
        }
	}
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::openBtnClick(TObject *Sender)
{
	robostar->req_AutoRun();
	if(gripper->seq == 4) gripper->step.step = 0;   //  seqPause
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::homeBtnClick(TObject *Sender)
{
    UnicodeString msg;
	//* 2025 05 21
    if(robostar->input.GRIPPER1_CELL_DETECT == true)
			msg = BaseForm->GetLangStr("MSG_HOME_MOVE_ALARM");
		else
			msg = BaseForm->GetLangStr("MSG_HOME_MOVE_Q");

	if(MessageBox(Handle, msg.c_str(), L"HOME", MB_YESNO|MB_ICONQUESTION) == ID_YES){
		if(robostar->seq == seqIdle || robostar->seq == seqPause)
			robostar->req_Home();
	}
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::AdvSmoothButton_ResetClick(TObject *Sender)
{
    if(MessageBox(Handle, BaseForm->GetLangStr("MSG_RESET_ALARM").c_str(),
        L"RESET", MB_YESNO|MB_ICONQUESTION) == ID_YES){
        MainForm->BuzzerOn(false);
        MainForm->LampModeChange(MainForm->beforeLampMode);
        robostar->req_Reset();
    }
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::AdvSmoothButton_ZupClick(TObject *Sender)
{
    //* 2025 05 21
    if(robostar->seq == seqIdle || robostar->seq == seqPause)
		robostar->req_zUp();
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::AdvSmoothButton_ServoOnClick(TObject *Sender)
{
	robostar->req_ServoOn();
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::AdvSmoothButton_ServoOffClick(TObject *Sender)
{
	robostar->req_ServoOff();
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::btnUpGripperClick(TObject *Sender)
{
	TAdvSmoothButton *btn;
	btn = (TAdvSmoothButton*)Sender;

	if(!gripper->disable_gripper[btn->Tag - 1])
		robostar->GripperDown(btn->Tag, false, true);
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::btnDownGripperClick(TObject *Sender)
{
	TAdvSmoothButton *btn;
	btn = (TAdvSmoothButton*)Sender;

    if(MainForm->psrcReady->Color != clLime)
	{
		if(MessageBox(Handle, BaseForm->GetLangStr("MSG_GRIPPER_DOWN").c_str(),
			L"Centering DOWN", MB_YESNO|MB_ICONQUESTION) == ID_YES){
				MainForm->plcOutput.SRC_MANUAL_WORK = 1;
        }
	}
	else
	{
		if(CheckPositionDown(btn->Tag - 1))
		{
            //* 2022 04 20 선별 트레이는 unchuck 상태에서, 대상 트레이는 chuck 상태에서만 그리퍼 다운 가능
			if(CheckUnchuckPosition(btn->Tag - 1)){
				if(!gripper->disable_gripper[btn->Tag - 1])
					robostar->GripperDown(btn->Tag, true, false);
			}
			else
				ShowMessage(BaseForm->GetLangStr("MSG_GRIPPER_CHECKCHUCK"));
		}
		else ShowMessage(BaseForm->GetLangStr("MSG_GRIPPER_CANNOTDOWN"));
	}
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::btnOpenGripperClick(TObject *Sender)
{
	//* delay 3초로 수정
    //* uncheck start
//	TAdvSmoothButton *btn;
//	btn = (TAdvSmoothButton*)Sender;
//
//	// 2019 07 05 그리퍼 [열기] 할 때 z 축 위치 확인
//	if(robostar->getCellDetectStatus(btn->Tag)){
//		if(!gripper->disable_gripper[btn->Tag - 1])
//			robostar->GripperChuck(btn->Tag, true, false);
//	}else{
//		int pos_z = robostar->mr2.pos[4];
//		if(pos_z >= 20000 || MainForm->m_ServoHome)
//		{
//			if(MessageBox(Handle, ("[B_Ignition] 셀이 감지 되었습니다.\r\nGripper #" + IntToStr(btn->Tag) + " 을 [열기] 하시겠습니까?").c_str(),
//				L"열기", MB_YESNO|MB_ICONWARNING) == ID_YES)
//				robostar->GripperChuck(btn->Tag, true, false);
//		} else
//		{
//		   ShowMessage("[C_Maint] 그리퍼 [열기]를 하기 위해 먼저 Z축을 이동시켜야 합니다.");
//		}
//	}
	//* unchuck end
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::btnOpenGripperMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    TAdvSmoothButton *btn;
	btn = (TAdvSmoothButton*)Sender;
    nCurrentTag = btn->Tag;
	downTime = Now();
    isButtonPressed = true;
    unchuckTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::btnOpenGripperMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    unchuckTimer->Enabled = false;
	isButtonPressed = false;
    downTime = Now();
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::unchuckTimerTimer(TObject *Sender)
{
	if(isButtonPressed == true){
		TDateTime currentTime = Now();
		int nTime = SecondsBetween(currentTime, downTime);
		if(nTime >= 2){
            downTime = Now();
            unchuckTimer->Enabled = false;
            isButtonPressed = false;
			//* uncheck start

			// 2019 07 05 그리퍼 [열기] 할 때 z 축 위치 확인
			if(robostar->getCellDetectStatus(nCurrentTag)){
				if(!gripper->disable_gripper[nCurrentTag - 1])
					robostar->GripperChuck(nCurrentTag, true, false);
            }else{
                int pos_z = robostar->mr2.pos[Axis_z];
                UnicodeString str = BaseForm->GetLangStr("MSG_GRIPPER_UNCHUCK_ALARM1") + IntToStr(nCurrentTag);
                if(pos_z >= 20000 || MainForm->m_ServoHome)
                {
                    if(MessageBox(Handle, str.c_str(), L"OPEN", MB_YESNO|MB_ICONWARNING) == ID_YES)
                        robostar->GripperChuck(nCurrentTag, true, false);
                } else
                {
                   ShowMessage(BaseForm->GetLangStr("MSG_GRIPPER_UNCHUCK_ALARM2"));
                }
            }
            //* unchuck end
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::btnCloseGripperClick(TObject *Sender)
{
	TAdvSmoothButton *btn;
	btn = (TAdvSmoothButton*)Sender;

	if(!gripper->disable_gripper[btn->Tag - 1])
		robostar->GripperChuck(btn->Tag, false, true);
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::btnUpAllGripperClick(TObject *Sender)
{
	for(int i=1; i<=gripCnt; ++i)
	{
		if(!gripper->disable_gripper[i - 1])
			robostar->GripperDown(i, false, true);
	}
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::btnDownAllGripperClick(TObject *Sender)
{
    if(MainForm->psrcReady->Color != clLime)
	{
		if(MessageBox(Handle, L"Source tray centering is not in DOWN state. Do you want to down?",
			L"Centering DOWN", MB_YESNO|MB_ICONQUESTION) == ID_YES){
				MainForm->plcOutput.SRC_MANUAL_WORK = 1;
			}
	}
	else
	{
		bool flag = true;
		for(int i=1; i<=gripCnt; ++i)
		{
			if(!CheckPositionDown(i - 1))
			{
				flag = false;
				break;
			}
		}

		if(flag)
		{
			for(int i=1; i<=gripCnt; ++i)
			{
				if(!gripper->disable_gripper[i - 1])
					robostar->GripperDown(i, true, false);
			}
		}
		else ShowMessage("The position where the gripper can not be DOWN. Please check your position.");
    }
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::btnOpenAllGripperClick(TObject *Sender)
{
    // delay 3초로 수정
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::btnOpenAllGripperMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    downTime = Now();
    isButtonPressed = true;
    unchuckAllTimer->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::btnOpenAllGripperMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    unchuckAllTimer->Enabled = false;
	isButtonPressed = false;
    downTime = Now();
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::unchuckAllTimerTimer(TObject *Sender)
{
    if(isButtonPressed == true){
        TDateTime currentTime = Now();
        //TTimeSpan elapsedTime = currentTime - downTime;
		if(SecondsBetween(currentTime, downTime) >= 2){
            unchuckTimer->Enabled = false;
            isButtonPressed = false;
            //* uncheck all start
            bool flag = true;
            for(int i=1; i<=gripCnt; ++i)
            {
                if(!robostar->getCellDetectStatus(i))
                {
                    flag = false;
                    break;
                }
            }

            if(flag)
            {
                for(int i=1; i<=gripCnt; ++i)
                {
                    if(!gripper->disable_gripper[i - 1])
                        robostar->GripperChuck(i, true, false);
                }
            }
			else {
				unchuckAllTimer->Enabled = false;
                isButtonPressed = false;
				downTime = Now();
				ShowMessage("The gripper can not be opened because the cell is detected.");
			}
            //* unchuck all end
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::btnCloseAllGripperClick(TObject *Sender)
{
	for(int i=1; i<=gripCnt; ++i)
	{
		if(!gripper->disable_gripper[i - 1])
			robostar->GripperChuck(i, false, true);
	}
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::speedEditKeyDown(TObject *Sender, WORD &Key,
	  TShiftState Shift)
{
	int speed = 600, acclSpeed = 1000, dcclSpeed = 1000;
	TEdit *edit;
	edit = (TEdit*)Sender;

	speed = speedEdit->Text.ToInt();
	acclSpeed = acclSpeedEdit->Text.ToInt();
	dcclSpeed = dcclSpeedEdit->Text.ToInt();

    ConfigForm->WriteSystemInfo("speed");

	if(Key == VK_RETURN){
		if(speed >= 200 && speed <= 1000)
		{
			Panel_speedEdit->Caption = speed;
			robostar->req_Speed(speed, acclSpeed, dcclSpeed);
		}
		else ShowMessage(BaseForm->GetLangStr("MSG_SETSPEED_ALARM"));
	}
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::disableChk1Click(TObject *Sender)
{
	TCheckBox *chk;
	chk = (TCheckBox*)Sender;

	gripper->disable_gripper[chk->Tag-1] = chk->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::waitBtnClick(TObject *Sender)
{
	UnicodeString msg;
	if(MainForm->psrcReady->Color != clLime)
	{
		if(MessageBox(Handle, BaseForm->GetLangStr("MSG_GRIPPER_DOWN").c_str(),
			L"Centering DOWN", MB_YESNO|MB_ICONQUESTION) == ID_YES){
				MainForm->plcOutput.SRC_MANUAL_WORK = 1;
		}
	}
	else {
        //* 2025 05 21
		if(robostar->input.GRIPPER1_CELL_DETECT == true)
			msg = BaseForm->GetLangStr("MSG_WAITING_MOVE_ALARM");
		else
			msg = BaseForm->GetLangStr("MSG_WAITING_MOVE_Q");

		if(MessageBox(Handle, msg.c_str(), L"MOVE", MB_YESNO|MB_ICONQUESTION) == ID_YES){
			if(robostar->seq == seqIdle || robostar->seq == seqPause)
				robostar->req_WaitPosition();
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::stopBtnClick(TObject *Sender)
{
	robostar->req_Stop();

	teachForm->pnlMovingAlarm->Visible = false;
	teachForm->pnlMovingAlarm2->Visible = false;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TteachForm::Button1MouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y)
{
	TButton *btn;
	btn = (TButton*)Sender;

	if(Button == mbLeft){
		if(MainForm->psrcReady->Color != clLime)
		{
			if(MessageBox(Handle, BaseForm->GetLangStr("MSG_SOURCETRAY_CENTERING_Q").c_str(),
				L"Centering DOWN", MB_YESNO|MB_ICONQUESTION) == ID_YES){
					MainForm->plcOutput.SRC_MANUAL_WORK = 1;
				}
		}
		else
		if(!robostar->getGripperUpStatus())
			ShowMessage(BaseForm->GetLangStr("MSG_JOG_GRIPPER_ALARM"));
		else if((btn->Tag < 4) && robostar->mr2.pos[Axis_z] != 0)
			ShowMessage(BaseForm->GetLangStr("MSG_JOG_ZAXIS_ALARM"));
		else if(btn->Tag == 4)
		{
			bool flag = true;
			for(int i=1; i<=gripCnt; ++i)
			{
				if(!CheckPositionDown(i - 1))
				{
					flag = false;
					break;
				}
			}

			if(flag) robostar->req_JogMove(btn->Tag);
			else ShowMessage(BaseForm->GetLangStr("MSG_JOG_POSITION_ALARM"));
		}
		else robostar->req_JogMove(btn->Tag);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TteachForm::Button1MouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift,
		  int X, int Y)
{
    TButton *btn;
	btn = (TButton*)Sender;
	if(Button == mbLeft){
		robostar->req_JogMove(-1);
	}

	if(btn->Tag == 6 || btn->Tag == 7)
		isGripperOpen1 = false;
	else if(btn->Tag == 8 || btn->Tag == 9)
		isGripperOpen2 = false;
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::AdvSmoothButton_LoadFactorInfoClick(TObject *Sender)
{
	loadfactorForm->Left = pnlManualControl->Left + 120;
	loadfactorForm->Top = pnlManualControl->Top + 200;
	loadfactorForm->ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::btnCloseClick(TObject *Sender)
{
    this->Close();
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::btnApplyTeachingClick(TObject *Sender)
{
	ApplyTeaching();
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::btnZAxisDownMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    TButton *btn;
	btn = (TButton*)Sender;

	if(Button == mbLeft){
		robostar->req_JogMove(btn->Tag);
	}
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::btnZAxisDownMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
	if(Button == mbLeft){
		robostar->req_JogMove(-1);
	}
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::btnZAxisUpMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    TButton *btn;
	btn = (TButton*)Sender;

	if(Button == mbLeft){
		robostar->req_JogMove(btn->Tag);
	}
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::btnZAxisUpMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    if(Button == mbLeft){
		robostar->req_JogMove(-1);
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//                  Method
//---------------------------------------------------------------------------
bool __fastcall TteachForm::CheckMoveTargetChannel(int channel)
{
    if(channel < 0 || channel >= TraySlotCount)
        return false;

    //* 2026 06 96채널로 변경해야 함.
    bool isPossible = true;

    if(MainForm->color_target[channel/24][23-(channel%24)] == clInactiveCaption
    	|| MainForm->color_target[channel/24][23-(channel%24)] == clSilver)
    {
        if(sCombo->ItemIndex == 0 && robostar->input.GRIPPER1_CELL_DETECT)
            isPossible = false;
    }

    return isPossible;
}
//---------------------------------------------------------------------------
bool __fastcall TteachForm::CheckMoveSourceChannel()
{
    bool isPossible = true;

    if(sCombo->ItemIndex == 0 && robostar->input.GRIPPER1_CELL_DETECT)
    	isPossible = false;

    return isPossible;
}
//---------------------------------------------------------------------------

bool __fastcall TteachForm::CheckPositionDown(int gripperIndex)
{
	int pos_x = robostar->mr2.pos[Axis_x];
	int pos_y = robostar->mr2.pos[Axis_y];

	if(((pos_x + (gripperIndex) * 90000) <= sTray_Position.Top
		&& (pos_x + (gripperIndex) * 90000) >= sTray_Position.Bottom
		&& pos_y <= sTray_Position.Left && pos_y >= sTray_Position.Right)
		|| (pos_x + (gripperIndex * 90000) <= tTray_Position.Top && pos_x + (gripperIndex * 90000) >= tTray_Position.Bottom
		&& pos_y <= tTray_Position.Left && pos_y >= tTray_Position.Right))
		return true;
	return false;
}
//---------------------------------------------------------------------------
bool __fastcall TteachForm::CheckUnchuckPosition(int gripperIndex)
{
	int pos_x = robostar->mr2.pos[Axis_x];
	int pos_y = robostar->mr2.pos[Axis_y];

    if(((pos_x + (gripperIndex) * 90000) <= sTray_Position.Top
    	&& (pos_x + (gripperIndex) * 90000) >= sTray_Position.Bottom)
		|| (pos_x + (gripperIndex * 90000) <= tTray_Position.Top
        && pos_x + (gripperIndex * 90000) >= tTray_Position.Bottom)
		)
		return true;

	return false;
}
//---------------------------------------------------------------------------
bool __fastcall TteachForm::CheckChuckPosition(int gripperIndex)
{
	int pos_x = robostar->mr2.pos[Axis_x];
	int pos_y = robostar->mr2.pos[Axis_y];

	if(pos_x <= tTray_Position.Top && pos_x >= tTray_Position.Bottom)
		return true;

	return false;
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::ApplyTeaching()
{
    try{
        //* source tray teaching value
        if(editCh01_SX->Text.Trim().IsEmpty() || editCh01_SY->Text.Trim().IsEmpty()
        	|| editCh13_SX->Text.Trim().IsEmpty() || editCh13_SY->Text.Trim().IsEmpty()){
        	MessageBox(Handle, BaseForm->GetLangStr("MSG_CHECK_SOURCE_CH01").c_str(), L"Warning", MB_OK|MB_ICONWARNING);
            return;
        }
        else if(editCh25_SX->Text.Trim().IsEmpty() || editCh25_SY->Text.Trim().IsEmpty()
        	|| editCh37_SX->Text.Trim().IsEmpty() || editCh37_SY->Text.Trim().IsEmpty()){
        	MessageBox(Handle, BaseForm->GetLangStr("MSG_CHECK_SOURCE_CH25").c_str(), L"Warning", MB_OK|MB_ICONWARNING);
            return;
        }
        else if(editCh49_SX->Text.Trim().IsEmpty() || editCh49_SY->Text.Trim().IsEmpty()
        	|| editCh61_SX->Text.Trim().IsEmpty() || editCh61_SY->Text.Trim().IsEmpty()){
        	MessageBox(Handle, BaseForm->GetLangStr("MSG_CHECK_SOURCE_CH49").c_str(), L"Warning", MB_OK|MB_ICONWARNING);
            return;
        }
        else if(editCh73_SX->Text.Trim().IsEmpty() || editCh73_SY->Text.Trim().IsEmpty()
        	|| editCh85_SX->Text.Trim().IsEmpty() || editCh85_SY->Text.Trim().IsEmpty()){
        	MessageBox(Handle, BaseForm->GetLangStr("MSG_CHECK_SOURCE_CH73").c_str(), L"Warning", MB_OK|MB_ICONWARNING);
            return;
        }

        //* target tray teaching value
        if(editCh01_TX->Text.Trim().IsEmpty() || editCh01_TY->Text.Trim().IsEmpty()
        	|| editCh13_TX->Text.Trim().IsEmpty() || editCh13_TY->Text.Trim().IsEmpty()){
        	MessageBox(Handle, BaseForm->GetLangStr("MSG_CHECK_TARGET_CH01").c_str(), L"Warning", MB_OK|MB_ICONWARNING);
            return;
        }
        else if(editCh25_TX->Text.Trim().IsEmpty() || editCh25_TY->Text.Trim().IsEmpty()
        	|| editCh37_TX->Text.Trim().IsEmpty() || editCh37_TY->Text.Trim().IsEmpty()){
        	MessageBox(Handle, BaseForm->GetLangStr("MSG_CHECK_TARGET_CH25").c_str(), L"Warning", MB_OK|MB_ICONWARNING);
            return;
        }
        else if(editCh49_TX->Text.Trim().IsEmpty() || editCh49_TY->Text.Trim().IsEmpty()
        	|| editCh61_TX->Text.Trim().IsEmpty() || editCh61_TY->Text.Trim().IsEmpty()){
        	MessageBox(Handle, BaseForm->GetLangStr("MSG_CHECK_TARGET_CH49").c_str(), L"Warning", MB_OK|MB_ICONWARNING);
            return;
        }
        else if(editCh73_TX->Text.Trim().IsEmpty() || editCh73_TY->Text.Trim().IsEmpty()
        	|| editCh85_TX->Text.Trim().IsEmpty() || editCh85_TY->Text.Trim().IsEmpty()){
        	MessageBox(Handle, BaseForm->GetLangStr("MSG_CHECK_TARGET_CH73").c_str(), L"Warning", MB_OK|MB_ICONWARNING);
            return;
        }

        //* 파일에 저장
        TEdit* teachingEdits[] = {
            editCh01_SX, editCh01_SY, editCh13_SX, editCh13_SY,
            editCh25_SX, editCh25_SY, editCh37_SX, editCh37_SY,
            editCh49_SX, editCh49_SY, editCh61_SX, editCh61_SY,
            editCh73_SX, editCh73_SY, editCh85_SX, editCh85_SY,
            editCh01_TX, editCh01_TY, editCh13_TX, editCh13_TY,
            editCh25_TX, editCh25_TY, editCh37_TX, editCh37_TY,
            editCh49_TX, editCh49_TY, editCh61_TX, editCh61_TY,
            editCh73_TX, editCh73_TY, editCh85_TX, editCh85_TY,
            edit_SZ, edit_TZ
        };

        for(unsigned int i = 0; i < sizeof(teachingEdits) / sizeof(teachingEdits[0]); ++i){
            int value = 0;
            if(!TryStrToInt(teachingEdits[i]->Text.Trim(), value)){
                MessageBox(Handle, L"Teaching values must be integers.", L"Warning", MB_OK|MB_ICONWARNING);
                teachingEdits[i]->SetFocus();
                return;
            }
        }
        if(MessageBox(Handle, BaseForm->GetLangStr("MSG_INPUT_VALUE").c_str(), L"SAVE", MB_YESNO|MB_ICONWARNING) == ID_YES){

            SaveTeaching(teachingFilePath);

            SetTrayMaxPosition();
        }
	}catch(...){
		MessageBox(Handle, L"정보를 입력 하세요.", L"Warning", MB_OK|MB_ICONWARNING);
	}
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::SaveTeaching(AnsiString filePath)
{
    std::unique_ptr<TIniFile> ini(new TIniFile(filePath));

    // 1. 공통 설정 저장
    ini->WriteString("COMMON", "KIND", "STANDARD");
    ini->WriteString("COMMON", "SLOT_COUNT", IntToStr(TraySlotCount));

    // 2. Source Tray 티칭값 저장
    int teachingCount = TraySlotCount / TrayTeachingGroupSize;
    for(int i = 0; i < teachingCount; ++i){
        int channel = i * TrayTeachingGroupSize + 1;
        ini->WriteString("SOURCE_TRAY", "XAxis_CH" + IntToStr(channel), IntToStr(GetTrayPosValue(channel, asSourceX)));
        ini->WriteString("SOURCE_TRAY", "YAxis_CH" + IntToStr(channel), IntToStr(GetTrayPosValue(channel, asSourceY)));
    }
    ini->WriteString("SOURCE_TRAY", "ZAxis", edit_SZ->Text);

    //3. Target Tray 티칭값 저장
    for(int i = 0; i < teachingCount; ++i){
        int channel = i * TrayTeachingGroupSize + 1;
        ini->WriteString("TARGET_TRAY", "XAxis_CH" + IntToStr(channel), IntToStr(GetTrayPosValue(channel, asTargetX)));
        ini->WriteString("TARGET_TRAY", "YAxis_CH" + IntToStr(channel), IntToStr(GetTrayPosValue(channel, asTargetY)));
    }
    ini->WriteString("TARGET_TRAY", "ZAxis", edit_TZ->Text);
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::LoadTeaching(AnsiString filePath)
{
	if (!FileExists(filePath)) return;
    std::unique_ptr<TIniFile> ini(new TIniFile(filePath));

    edit_SZ->Text = ini->ReadString("SOURCE_TRAY", "ZAxis", "0");
    editCh01_SX->Text = ini->ReadString("SOURCE_TRAY", "XAxis_CH1", "0");
    editCh01_SY->Text = ini->ReadString("SOURCE_TRAY", "YAxis_CH1", "0");
    editCh13_SX->Text = ini->ReadString("SOURCE_TRAY", "XAxis_CH13", "0");
    editCh13_SY->Text = ini->ReadString("SOURCE_TRAY", "YAxis_CH13", "0");
    editCh25_SX->Text = ini->ReadString("SOURCE_TRAY", "XAxis_CH25", "0");
    editCh25_SY->Text = ini->ReadString("SOURCE_TRAY", "YAxis_CH25", "0");
    editCh37_SX->Text = ini->ReadString("SOURCE_TRAY", "XAxis_CH37", "0");
    editCh37_SY->Text = ini->ReadString("SOURCE_TRAY", "YAxis_CH37", "0");
    editCh49_SX->Text = ini->ReadString("SOURCE_TRAY", "XAxis_CH49", "0");
    editCh49_SY->Text = ini->ReadString("SOURCE_TRAY", "YAxis_CH49", "0");
    editCh61_SX->Text = ini->ReadString("SOURCE_TRAY", "XAxis_CH61", "0");
    editCh61_SY->Text = ini->ReadString("SOURCE_TRAY", "YAxis_CH61", "0");
    editCh73_SX->Text = ini->ReadString("SOURCE_TRAY", "XAxis_CH73", "0");
    editCh73_SY->Text = ini->ReadString("SOURCE_TRAY", "YAxis_CH73", "0");
    editCh85_SX->Text = ini->ReadString("SOURCE_TRAY", "XAxis_CH85", "0");
    editCh85_SY->Text = ini->ReadString("SOURCE_TRAY", "YAxis_CH85", "0");

    edit_TZ->Text = ini->ReadString("TARGET_TRAY", "ZAxis", "0");
    editCh01_TX->Text = ini->ReadString("TARGET_TRAY", "XAxis_CH1", "0");
    editCh01_TY->Text = ini->ReadString("TARGET_TRAY", "YAxis_CH1", "0");
    editCh13_TX->Text = ini->ReadString("TARGET_TRAY", "XAxis_CH13", "0");
    editCh13_TY->Text = ini->ReadString("TARGET_TRAY", "YAxis_CH13", "0");
    editCh25_TX->Text = ini->ReadString("TARGET_TRAY", "XAxis_CH25", "0");
    editCh25_TY->Text = ini->ReadString("TARGET_TRAY", "YAxis_CH25", "0");
    editCh37_TX->Text = ini->ReadString("TARGET_TRAY", "XAxis_CH37", "0");
    editCh37_TY->Text = ini->ReadString("TARGET_TRAY", "YAxis_CH37", "0");
    editCh49_TX->Text = ini->ReadString("TARGET_TRAY", "XAxis_CH49", "0");
    editCh49_TY->Text = ini->ReadString("TARGET_TRAY", "YAxis_CH49", "0");
    editCh61_TX->Text = ini->ReadString("TARGET_TRAY", "XAxis_CH61", "0");
    editCh61_TY->Text = ini->ReadString("TARGET_TRAY", "YAxis_CH61", "0");
    editCh73_TX->Text = ini->ReadString("TARGET_TRAY", "XAxis_CH73", "0");
    editCh73_TY->Text = ini->ReadString("TARGET_TRAY", "YAxis_CH73", "0");
    editCh85_TX->Text = ini->ReadString("TARGET_TRAY", "XAxis_CH85", "0");
    editCh85_TY->Text = ini->ReadString("TARGET_TRAY", "YAxis_CH85", "0");

	SetTrayMaxPosition();
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::SetTrayMaxPosition()
{
    int sourceX = GetTrayCalculatedPosValue(1, asSourceX);
    int sourceY = GetTrayCalculatedPosValue(1, asSourceY);
    int targetX = GetTrayCalculatedPosValue(1, asTargetX);
    int targetY = GetTrayCalculatedPosValue(1, asTargetY);

    sTray_Position.Top = sourceX;
    sTray_Position.Bottom = sourceX;
    sTray_Position.Left = sourceY;
    sTray_Position.Right = sourceY;

    tTray_Position.Top = targetX;
    tTray_Position.Bottom = targetX;
    tTray_Position.Left = targetY;
    tTray_Position.Right = targetY;

    for(int channel = 2; channel <= TraySlotCount; ++channel){
        sourceX = GetTrayCalculatedPosValue(channel, asSourceX);
        sourceY = GetTrayCalculatedPosValue(channel, asSourceY);
        targetX = GetTrayCalculatedPosValue(channel, asTargetX);
        targetY = GetTrayCalculatedPosValue(channel, asTargetY);

        if(sourceX > sTray_Position.Top) sTray_Position.Top = sourceX;
        if(sourceX < sTray_Position.Bottom) sTray_Position.Bottom = sourceX;
        if(sourceY > sTray_Position.Left) sTray_Position.Left = sourceY;
        if(sourceY < sTray_Position.Right) sTray_Position.Right = sourceY;

        if(targetX > tTray_Position.Top) tTray_Position.Top = targetX;
        if(targetX < tTray_Position.Bottom) tTray_Position.Bottom = targetX;
        if(targetY > tTray_Position.Left) tTray_Position.Left = targetY;
        if(targetY < tTray_Position.Right) tTray_Position.Right = targetY;
    }
}
//---------------------------------------------------------------------------
int __fastcall TteachForm::GetTrayPosValue(int channel, TrayAxisEdit editType)
{
    TEdit* edt = GetTrayEdit(channel, editType);

    if (edt == NULL)
        return 0;

    return edt->Text.ToIntDef(0);
}
//---------------------------------------------------------------------------
int __fastcall TteachForm::GetTrayCalculatedPosValue(int channel, TrayAxisEdit editType)
{
    if(channel < 1 || channel > TraySlotCount)
        return 0;

    int value = GetTrayPosValue(channel, editType);
    if(editType == asSourceX || editType == asTargetX)
        value += ((channel - 1) % TrayTeachingGroupSize) * TrayCellPitch;

    return value;
}
//---------------------------------------------------------------------------
TEdit* __fastcall TteachForm::GetTrayEdit(int channel, TrayAxisEdit editType)
{
    if (channel < 1 || channel > TraySlotCount)
        return NULL;

    int group = (channel - 1) / TrayTeachingGroupSize;

    switch (group)
    {
        case 0: // 1 ~ 12
            switch (editType)
            {
                case asSourceX: return editCh01_SX;
                case asSourceY: return editCh01_SY;
                case asTargetX: return editCh01_TX;
                case asTargetY: return editCh01_TY;
            }
            break;

        case 1: // 13 ~ 24
            switch (editType)
            {
                case asSourceX: return editCh13_SX;
                case asSourceY: return editCh13_SY;
                case asTargetX: return editCh13_TX;
                case asTargetY: return editCh13_TY;
            }
            break;

        case 2: // 25 ~ 36
            switch (editType)
            {
                case asSourceX: return editCh25_SX;
                case asSourceY: return editCh25_SY;
                case asTargetX: return editCh25_TX;
                case asTargetY: return editCh25_TY;
            }
            break;

        case 3: // 37 ~ 48
            switch (editType)
            {
                case asSourceX: return editCh37_SX;
                case asSourceY: return editCh37_SY;
                case asTargetX: return editCh37_TX;
                case asTargetY: return editCh37_TY;
            }
            break;

        case 4: // 49 ~ 60
            switch (editType)
            {
                case asSourceX: return editCh49_SX;
                case asSourceY: return editCh49_SY;
                case asTargetX: return editCh49_TX;
                case asTargetY: return editCh49_TY;
            }
            break;

        case 5: // 61 ~ 72
            switch (editType)
            {
                case asSourceX: return editCh61_SX;
                case asSourceY: return editCh61_SY;
                case asTargetX: return editCh61_TX;
                case asTargetY: return editCh61_TY;
            }
            break;

        case 6: // 73 ~ 84
            switch (editType)
            {
                case asSourceX: return editCh73_SX;
                case asSourceY: return editCh73_SY;
                case asTargetX: return editCh73_TX;
                case asTargetY: return editCh73_TY;
            }
            break;

        case 7: // 85 ~ 96
            switch (editType)
            {
                case asSourceX: return editCh85_SX;
                case asSourceY: return editCh85_SY;
                case asTargetX: return editCh85_TX;
                case asTargetY: return editCh85_TY;
            }
            break;
    }

    return NULL;
}
//---------------------------------------------------------------------------

void __fastcall TteachForm::AdvSmoothButton1Click(TObject *Sender)
{
	ErrorForm_eject->ShowError("Gripper #1 has a cell.", "Eject step 1. Cell check error", 1, 20);
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::btnKeyLockClick(TObject *Sender)
{
    ErrorForm_insert->ShowError("Gripper #1 has a cell.", "Eject step 1. Cell check error", 1, 20);
}
//---------------------------------------------------------------------------
