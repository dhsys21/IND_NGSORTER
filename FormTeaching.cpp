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

    for(int i = 0; i < 96;)
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
        sTray[i]->Tag = i;
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

    for(int i = 0; i < 96;)
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
        tTray[i]->Tag = i;
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
		int ch = BaseForm->StringToInt(pnl->Tag, 0);
        if(CheckMoveSourceChannel() == false){
			ShowMessage(BaseForm->GetLangStr("MSG_GRIPPER_MOVE_ERR") + IntToStr(ch));
        } else{
            str = "[" + sCombo->Text + "] " + BaseForm->GetLangStr("MSG_SOURCETRAY_MOVE_Q") + pnl->Caption->Text;
            if(MessageBox(Handle, str.c_str(), L"MOVE", MB_YESNO|MB_ICONQUESTION) == ID_YES){
				robostar->req_AutoMove(1, sCombo->ItemIndex + 1 , pnl->Tag, 962);

                for(int i=0; i< 96; ++i){
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
		int ch = BaseForm->StringToInt(pnl->Tag, 0);
        if(CheckMoveTargetChannel(ch-1) == false){
			ShowMessage(BaseForm->GetLangStr("MSG_GRIPPER_MOVE_ERR2")  + IntToStr(ch));
        } else{
			str = "[" + sCombo->Text + "] " + BaseForm->GetLangStr("MSG_TARGETTRAY_MOVE_Q") + pnl->Caption->Text;
            if(MessageBox(Handle, str.c_str(), L"이동", MB_YESNO|MB_ICONQUESTION) == ID_YES){
                robostar->req_AutoMove(2, sCombo->ItemIndex + 1 , pnl->Tag, 96);
                for(int i = 0; i < 96; ++i){
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
	MainForm->BuzzerOn(false);
	MainForm->LampModeChange(MainForm->beforeLampMode);
	robostar->req_Reset();
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
                if(pos_z >= 20000 || MainForm->m_ServoHome)
                {
                    if(MessageBox(Handle, ("셀이 감지 되었습니다.\r\nGripper #" + IntToStr(nCurrentTag) + " 을 [열기] 하시겠습니까?").c_str(),
                        L"열기", MB_YESNO|MB_ICONWARNING) == ID_YES)
                        robostar->GripperChuck(nCurrentTag, true, false);
                } else
                {
                   ShowMessage("그리퍼 [열기]를 하기 위해 먼저 Z축을 상승시켜야 합니다.");
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
		if(MessageBox(Handle, L"[C_Maint] 선별 트레이가 DOWN 상태가 아닙니다. down 하시겠습니까?",
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
		else ShowMessage("[C_Maint] 현재위치에서 그리퍼를 DOWN 시킬 수 없습니다. 위치를 확인 해 주세요.");
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
				ShowMessage("[B_Ignition] 셀이 감지되어 그리퍼를 열 수 없습니다.");
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
		if(speed >= 200 && speed <= 1200)
		{
			Panel_speedEdit->Caption = speed;
			robostar->req_Speed(speed, acclSpeed, dcclSpeed);
		}
		else ShowMessage("[C_Maint] 속도값은 200 과 1200 사이값을 입력 해주세요.");
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
		if(MessageBox(Handle, L"[C_Maint] 선별 트레이가 DOWN 상태가 아닙니다. down 하시겠습니까?",
			L"Centering DOWN", MB_YESNO|MB_ICONQUESTION) == ID_YES){
				MainForm->plcOutput.SRC_MANUAL_WORK = 1;
		}
	}
	else {
        //* 2025 05 21
		if(robostar->input.GRIPPER1_CELL_DETECT == true)
			msg = "[B_Ignition] 그리퍼에 셀이 있습니다. 대기 위치로 이동하시겠습니까?";
		else
			msg = "[C_Maint] 대기 위치로 이동하시겠습니까?";

		if(MessageBox(Handle, msg.c_str(), L"대기 위치 이동", MB_YESNO|MB_ICONQUESTION) == ID_YES){
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
			if(MessageBox(Handle, L"[C_Maint] 선별 트레이가 DOWN 상태가 아닙니다. down 하시겠습니까?",
				L"Centering DOWN", MB_YESNO|MB_ICONQUESTION) == ID_YES){
					MainForm->plcOutput.SRC_MANUAL_WORK = 1;
				}
		}
		else
		if(!robostar->getGripperUpStatus())
			ShowMessage("[C_Maint] 그리퍼가 상승하지 않았습니다. 상승(UP) 후에 이동하세요.");
		else if((btn->Tag < 4) && robostar->mr2.pos[Axis_z] != 0)
			ShowMessage("Z 축 위치가 0이 아닙니다. Z 축을 상승(UP) 한 후에 이동하세요.");
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
			else ShowMessage("[C_Maint] Z 축을 DOWN 할 수 없는 위치입니다. 위치를 확인 해주세요.");
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
// 2019 07 05 btn->Tag 6,7,8,9 일때 (gripper 1, 2 => +, -) 셀 있으면 z축이 내려가 있을 때만 동작.
void __fastcall TteachForm::Button2MouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y)
{
	TButton *btn;
	btn = (TButton*)Sender;

	if(Button == mbLeft){
		if(MainForm->psrcReady->Color != clLime)
		{
			if(MessageBox(Handle, L"[C_Maint] 선별 트레이가 DOWN 상태가 아닙니다. down 하시겠습니까?",
				L"Centering DOWN", MB_YESNO|MB_ICONQUESTION) == ID_YES){
					MainForm->plcOutput.SRC_MANUAL_WORK = 1;
				}
		}
		else
		if(btn->Tag == 4)
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
			else ShowMessage("[C_Maint] Z 축을 DOWN 할 수 없는 위치입니다. 위치를 확인 해주세요.");
		}
		// 2019 07 05 btn->Tag 6,7,8,9 일때 (gripper 1, 2 => +, -) 셀 있으면 z축이 내려가 있을 때만 동작.
		else if(btn->Tag == 6 || btn->Tag == 7)
		{
			int pos_z = robostar->mr2.pos[Axis_z];
			if(robostar->input.GRIPPER1_CELL_DETECT == true && (pos_z < 20000 && !MainForm->m_ServoHome))
			{
				ShowMessage("[B_Ignition] 그리퍼 1에 셀이 있습니다. Z축을 이동 시킨 후 그리퍼를 움직 일 수 있습니다.");
			}
			else if(robostar->input.GRIPPER1_CELL_DETECT == true && (pos_z >= 20000 || MainForm->m_ServoHome))
			{
				if(isGripperOpen1 == true)
					robostar->req_JogMove(btn->Tag);
				else if(MessageBox(Handle, L"[B_Ignition] 그리퍼 1에 셀이 있습니다.", L"위치 확인", MB_YESNO|MB_ICONWARNING) == ID_YES)
				{
					robostar->req_JogMove(-1);
                    isGripperOpen1 = true;
				}
			}
			else
				robostar->req_JogMove(btn->Tag);
		}
		else
		{
			robostar->req_JogMove(btn->Tag);
		}
	}
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
        	MessageBox(Handle, L"Please check Source Tray Ch01 or Ch13 value.", L"Warning", MB_OK|MB_ICONWARNING);
            return;
        }
        else if(editCh25_SX->Text.Trim().IsEmpty() || editCh25_SY->Text.Trim().IsEmpty()
        	|| editCh37_SX->Text.Trim().IsEmpty() || editCh37_SY->Text.Trim().IsEmpty()){
        	MessageBox(Handle, L"Please check Source Tray Ch25 or Ch37 value.", L"Warning", MB_OK|MB_ICONWARNING);
            return;
        }
        else if(editCh49_SX->Text.Trim().IsEmpty() || editCh49_SY->Text.Trim().IsEmpty()
        	|| editCh61_SX->Text.Trim().IsEmpty() || editCh61_SY->Text.Trim().IsEmpty()){
        	MessageBox(Handle, L"Please check Source Tray Ch49 or Ch61 value.", L"Warning", MB_OK|MB_ICONWARNING);
            return;
        }
        else if(editCh73_SX->Text.Trim().IsEmpty() || editCh73_SY->Text.Trim().IsEmpty()
        	|| editCh85_SX->Text.Trim().IsEmpty() || editCh85_SY->Text.Trim().IsEmpty()){
        	MessageBox(Handle, L"Please check Source Tray Ch73 or Ch85 value.", L"Warning", MB_OK|MB_ICONWARNING);
            return;
        }

        //* target tray teaching value
        if(editCh01_TX->Text.Trim().IsEmpty() || editCh01_TY->Text.Trim().IsEmpty()
        	|| editCh13_TX->Text.Trim().IsEmpty() || editCh13_TY->Text.Trim().IsEmpty()){
        	MessageBox(Handle, L"Please check Target Tray Ch01 or Ch13 value.", L"Warning", MB_OK|MB_ICONWARNING);
            return;
        }
        else if(editCh25_TX->Text.Trim().IsEmpty() || editCh25_TY->Text.Trim().IsEmpty()
        	|| editCh37_TX->Text.Trim().IsEmpty() || editCh37_TY->Text.Trim().IsEmpty()){
        	MessageBox(Handle, L"Please check Target Tray Ch25 or Ch37 value.", L"Warning", MB_OK|MB_ICONWARNING);
            return;
        }
        else if(editCh49_TX->Text.Trim().IsEmpty() || editCh49_TY->Text.Trim().IsEmpty()
        	|| editCh61_TX->Text.Trim().IsEmpty() || editCh61_TY->Text.Trim().IsEmpty()){
        	MessageBox(Handle, L"Please check Target Tray Ch49 or Ch61 value.", L"Warning", MB_OK|MB_ICONWARNING);
            return;
        }
        else if(editCh73_TX->Text.Trim().IsEmpty() || editCh73_TY->Text.Trim().IsEmpty()
        	|| editCh85_TX->Text.Trim().IsEmpty() || editCh85_TY->Text.Trim().IsEmpty()){
        	MessageBox(Handle, L"Please check Target Tray Ch73 or Ch85 value.", L"Warning", MB_OK|MB_ICONWARNING);
            return;
        }

        //* 파일에 저장
        if(MessageBox(Handle, L"Do you want to save the teaching value?", L"SAVE", MB_YESNO|MB_ICONWARNING) == ID_YES){

            SaveTeaching(teachingFilePath);

            SetTrayMaxPosition();
        }
	}catch(...){
		MessageBox(Handle, L"[C_Maint] 정보를 입력 하세요.", L"경고", MB_OK|MB_ICONWARNING);
	}
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::SaveTeaching(AnsiString filePath)
{
    std::unique_ptr<TIniFile> ini(new TIniFile(filePath));

    // 1. 공통 설정 저장
    ini->WriteString("COMMON", "KIND", "STANDARD");
    ini->WriteString("COMMON", "SLOT_COUNT", "96");

    // 2. Source Tray 티칭값 저장
    for(int i = 0; i < 8; i++){
        ini->WriteString("SOURCE_TRAY", "XAxis_CH" + IntToStr(i * 12 + 1), IntToStr(GetTrayPosValue(i * 12 + 1, asSourceX)));
        ini->WriteString("SOURCE_TRAY", "YAxis_CH" + IntToStr(i * 12 + 1), IntToStr(GetTrayPosValue(i * 12 + 1, asSourceY)));
    }
    ini->WriteString("SOURCE_TRAY", "ZAxis", edit_SZ->Text);

    //3. Target Tray 티칭값 저장
    for(int i = 0; i < 8; i++){
        ini->WriteString("TARGET_TRAY", "XAxis_CH" + IntToStr(i * 12 + 1), IntToStr(GetTrayPosValue(i * 12 + 1, asTargetX)));
        ini->WriteString("TARGET_TRAY", "YAxis_CH" + IntToStr(i * 12 + 1), IntToStr(GetTrayPosValue(i * 12 + 1, asTargetY)));
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
	sTray_Position.Top = 0;
	sTray_Position.Bottom = 0;
	sTray_Position.Left = 0;
	sTray_Position.Right = 0;

    //* Source Tray 최대값 저장
    sTray_Position.Left = editCh01_SY->Text.ToIntDef(0);//teachEdit[1][0]->Text.ToInt();
    sTray_Position.Right = editCh73_SY->Text.ToIntDef(0);//teachEdit[1][3]->Text.ToInt();

	sTray_Position.Top = editCh13_SX->Text.ToIntDef(0) + (12 - 1) * 45000; // 선별 1열 X + 11열 X
    sTray_Position.Bottom = editCh01_SX->Text.ToIntDef(0);

	tTray_Position.Top = 0;
	tTray_Position.Bottom = 0;
	tTray_Position.Left = 0;
	tTray_Position.Right = 0;

    //* Target Tray 최대값 저장
	tTray_Position.Left = editCh01_TY->Text.ToIntDef(0);// teachEdit[1][4]->Text.ToInt();
    tTray_Position.Right = editCh73_TY->Text.ToIntDef(0);//teachEdit[1][7]->Text.ToInt();

	tTray_Position.Top = editCh13_TX->Text.ToIntDef(0) + (12 - 1) * 45000; // 대상 13열 X + 11열 X
    tTray_Position.Bottom = editCh01_TX->Text.ToIntDef(0);
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::zup()
{
	if(!gripper->disable_gripper[1 - 1])
		robostar->GripperDown(1, false, true);
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::zdown()
{

    if(MainForm->psrcReady->Color != clLime)
	{
		if(MessageBox(Handle, L"선별 트레이가 DOWN 상태가 아닙니다. down 하시겠습니까?",
			L"Centering DOWN", MB_YESNO|MB_ICONQUESTION) == ID_YES){
				MainForm->plcOutput.SRC_MANUAL_WORK = 1;
			}
	}
	else
	{
		if(CheckPositionDown(1 - 1))
		{
            //* 2022 04 20 선별 트레이는 unchuck 상태에서, 대상 트레이는 chuck 상태에서만 그리퍼 다운 가능
			if(CheckUnchuckPosition(1 - 1)){
				if(!gripper->disable_gripper[1 - 1])
					robostar->GripperDown(1, true, false);
			}
			else
				ShowMessage("[C_Maint] 그리퍼 Chuck/Unchuck 상태를 확인 해 주세요.");
		}
		else ShowMessage("[C_Maint] 현재위치에서 그리퍼를 DOWN 시킬 수 없습니다. 위치를 확인 해 주세요.");
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
TEdit* __fastcall TteachForm::GetTrayEdit(int channel, TrayAxisEdit editType)
{
    if (channel < 1 || channel > 96)
        return NULL;

    int group = (channel - 1) / 12;

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
