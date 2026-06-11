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
	sTray[0] = s1;
	sTray[1] = s2;
	sTray[2] = s3;
	sTray[3] = s4;
	sTray[4] = s5;
	sTray[5] = s6;
	sTray[6] = s7;
	sTray[7] = s8;
	sTray[8] = s9;
	sTray[9] = s10;
	sTray[10] = s11;
	sTray[11] = s12;
	sTray[12] = s13;
	sTray[13] = s14;
	sTray[14] = s15;
	sTray[15] = s16;
	sTray[16] = s17;
	sTray[17] = s18;
	sTray[18] = s19;
	sTray[19] = s20;
	sTray[20] = s21;
	sTray[21] = s22;
	sTray[22] = s23;
	sTray[23] = s24;
	sTray[24] = s25;
	sTray[25] = s26;
	sTray[26] = s27;
	sTray[27] = s28;
	sTray[28] = s29;
	sTray[29] = s30;
	sTray[30] = s31;
	sTray[31] = s32;
	sTray[32] = s33;
	sTray[33] = s34;
	sTray[34] = s35;
	sTray[35] = s36;
	sTray[36] = s37;
	sTray[37] = s38;
	sTray[38] = s39;
	sTray[39] = s40;
	sTray[40] = s41;
	sTray[41] = s42;
	sTray[42] = s43;
	sTray[43] = s44;
	sTray[44] = s45;
	sTray[45] = s46;
	sTray[46] = s47;
	sTray[47] = s48;
	sTray[48] = s49;
	sTray[49] = s50;
	sTray[50] = s51;
	sTray[51] = s52;
	sTray[52] = s53;
	sTray[53] = s54;
	sTray[54] = s55;
	sTray[55] = s56;
	sTray[56] = s57;
	sTray[57] = s58;
	sTray[58] = s59;
	sTray[59] = s60;
	sTray[60] = s61;
	sTray[61] = s62;
	sTray[62] = s63;
	sTray[63] = s64;
	sTray[64] = s65;
	sTray[65] = s66;
	sTray[66] = s67;
	sTray[67] = s68;
	sTray[68] = s69;
	sTray[69] = s70;
	sTray[70] = s71;
	sTray[71] = s72;
	sTray[72] = s73;
	sTray[73] = s74;
	sTray[74] = s75;
	sTray[75] = s76;
	sTray[76] = s77;
	sTray[77] = s78;
	sTray[78] = s79;
	sTray[79] = s80;
	sTray[80] = s81;
	sTray[81] = s82;
	sTray[82] = s83;
	sTray[83] = s84;
	sTray[84] = s85;
	sTray[85] = s86;
	sTray[86] = s87;
	sTray[87] = s88;
	sTray[88] = s89;
	sTray[89] = s90;
	sTray[90] = s91;
	sTray[91] = s92;
	sTray[92] = s93;
	sTray[93] = s94;
	sTray[94] = s95;
	sTray[95] = s96;

	tTray[0] = t1;
	tTray[1] = t2;
	tTray[2] = t3;
	tTray[3] = t4;
	tTray[4] = t5;
	tTray[5] = t6;
	tTray[6] = t7;
	tTray[7] = t8;
	tTray[8] = t9;
	tTray[9] = t10;
	tTray[10] = t11;
	tTray[11] = t12;
	tTray[12] = t13;
	tTray[13] = t14;
	tTray[14] = t15;
	tTray[15] = t16;
	tTray[16] = t17;
	tTray[17] = t18;
	tTray[18] = t19;
	tTray[19] = t20;
	tTray[20] = t21;
	tTray[21] = t22;
	tTray[22] = t23;
	tTray[23] = t24;

	// source tray teaching
	teachEdit[0][0] = gpEdit1;
	teachEdit[1][0] = gpEdit2;	// gp11
	teachEdit[0][1] = gpEdit3;
	teachEdit[1][1] = gpEdit4;	// gp12
	teachEdit[0][2] = gpEdit5;
	teachEdit[1][2] = gpEdit6;	// gp13
	teachEdit[0][3] = gpEdit7;
	teachEdit[1][3] = gpEdit8;	// gp14

    // target tray teaching
	teachEdit[0][4] = gpEdit9;
	teachEdit[1][4] = gpEdit10;	// gp21
	teachEdit[0][5] = gpEdit11;
	teachEdit[1][5] = gpEdit12;	// gp22
	teachEdit[0][6] = gpEdit13;
	teachEdit[1][6] = gpEdit14;	// gp23
	teachEdit[0][7] = gpEdit15;
	teachEdit[1][7] = gpEdit16;	// gp24

	teachEdit_z[0] = gpEdit18;
	teachEdit_z[1] = gpEdit17;

	teachEdit_CHUCK[0] = gpEdit19; // unchuck
	teachEdit_CHUCK[1] = gpEdit20; // chuck

	//* Load Factor
	lblLoadFactor[1] = lblLoadFactor1;
	lblLoadFactor[2] = lblLoadFactor2;
	lblLoadFactor[3] = lblLoadFactor3;
	lblLoadFactor[4] = lblLoadFactor4;

	for(int i = 0; i < 96; ++i){
		sTray[i]->Tag = i + 1;
		sTray[i]->OnClick = sClick;
	}

	for(int i = 0; i < 12; ++i){
        tTray[i]->Width = 280;
		tTray[i]->Visible = true;
		tTray[i]->Tag = i+1;
		tTray[i]->OnClick = tClick;

        if(i >= 6) tTray[i]->Left = tTray[12]->Left;
	}

	LoadTeaching();
	LoadFromFile();
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::FormShow(TObject *Sender)
{
	this->Left = 0;
	this->Top = 0;

    pnlMovingAlarm2->Align = alClient;
    pnlMovingAlarm->Align = alClient;

    robostar->io_Init();
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::sClick(TObject *Sender)
{
	TAdvSmoothPanel *pnl;
	pnl = (TAdvSmoothPanel*)Sender;
	UnicodeString str;

	if(MainForm->psrcReady->Color != clLime)
	{
		if(MessageBox(Handle, L"[C_Maint] 선별트레이가 센터링되지 않았습니다. 센터링하시겠습니까?",
											L"Centering DOWN", MB_YESNO|MB_ICONQUESTION) == ID_YES)
		{
			MainForm->plcOutput.SRC_MANUAL_WORK = 1;
		}
	}
	else
	{
		int ch = BaseForm->StringToInt(pnl->Tag, 0);
        if(CheckMoveSourceChannel() == false){
			ShowMessage("[C_Maint] 그리퍼 대상 채널" + IntToStr(ch) + " 에 셀이 있어 이동을 할 수 없습니다.");
        } else{
            str = "[" + sCombo->Text + "] 선별 트레이 채널" + pnl->Caption->Text + " 으로 이동 하시겠습니까?";
            if(MessageBox(Handle, str.c_str(), L"이동", MB_YESNO|MB_ICONQUESTION) == ID_YES){
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
		if(MessageBox(Handle, L"[C_Maint] 선별트레이가 센터링되지 않았습니다. 센터링하시겠습니까?",
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
			ShowMessage("[C_Maint] 그리퍼 대상 채널" + IntToStr(ch) + " 에 셀이 있어 이동을 할 수 없습니다.");
        } else{
			str = "[" + sCombo->Text + "] 대상 트레이 채널 " + pnl->Caption->Text + " 으로 이동하시겠습니까?";
            if(MessageBox(Handle, str.c_str(), L"이동", MB_YESNO|MB_ICONQUESTION) == ID_YES){
                robostar->req_AutoMove(2, sCombo->ItemIndex + 1 , pnl->Tag, 96);
                for(int i = 0; i < 24; ++i){
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
bool __fastcall TteachForm::CheckMoveTargetChannel(int channel)
{
    //* 2026 06 96채널로 변경해야 함.
    bool isPossible = true;

    if(MainForm->color_target[channel/6][5-(channel%6)] == clInactiveCaption
    	|| MainForm->color_target[channel/6][5-(channel%6)] == clSilver)
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
    if(robostar->input.GRIPPER1_CELL_DETECT == true == true)
			msg = "[B_Ignition] 그리퍼에 셀이 있습니다. 대기 위치로 이동하시겠습니까?";
		else
			msg = "[C_Maint] 대기 위치로 이동하시겠습니까?";

	if(MessageBox(Handle, msg.c_str(), L"대기 위치 이동", MB_YESNO|MB_ICONQUESTION) == ID_YES){
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
void __fastcall TteachForm::AdvSmoothButton13Click(TObject *Sender)
{
	TAdvSmoothButton *btn;
	btn = (TAdvSmoothButton*)Sender;

	if(!gripper->disable_gripper[btn->Tag - 1])
		robostar->GripperDown(btn->Tag, false, true);
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::AdvSmoothButton14Click(TObject *Sender)
{
	TAdvSmoothButton *btn;
	btn = (TAdvSmoothButton*)Sender;

    if(MainForm->psrcReady->Color != clLime)
	{
		if(MessageBox(Handle, L"[C_Maint] 선별 트레이가 DOWN 상태가 아닙니다. down 하시겠습니까?",
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
				ShowMessage("[C_Maint] 그리퍼 Chuck/Unchuck 상태를 확인 해 주세요.");
		}
		else ShowMessage("[C_Maint] 현재위치에서 그리퍼를 DOWN 시킬 수 없습니다. 위치를 확인 해 주세요.");
	}
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::AdvSmoothButton12Click(TObject *Sender)
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
void __fastcall TteachForm::AdvSmoothButton12MouseDown(TObject *Sender, TMouseButton Button,
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
void __fastcall TteachForm::AdvSmoothButton12MouseUp(TObject *Sender, TMouseButton Button,
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
                int pos_z = robostar->mr2.pos[4];
                if(pos_z >= 20000 || MainForm->m_ServoHome)
                {
                    if(MessageBox(Handle, ("[B_Ignition] 셀이 감지 되었습니다.\r\nGripper #" + IntToStr(nCurrentTag) + " 을 [열기] 하시겠습니까?").c_str(),
                        L"열기", MB_YESNO|MB_ICONWARNING) == ID_YES)
                        robostar->GripperChuck(nCurrentTag, true, false);
                } else
                {
                   ShowMessage("[C_Maint] 그리퍼 [열기]를 하기 위해 먼저 Z축을 이동시켜야 합니다.");
                }
            }
            //* unchuck end
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::AdvSmoothButton11Click(TObject *Sender)
{
	TAdvSmoothButton *btn;
	btn = (TAdvSmoothButton*)Sender;

	if(!gripper->disable_gripper[btn->Tag - 1])
		robostar->GripperChuck(btn->Tag, false, true);
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::AdvSmoothButton36Click(TObject *Sender)
{
	for(int i=1; i<=gripCnt; ++i)
	{
		if(!gripper->disable_gripper[i - 1])
			robostar->GripperDown(i, false, true);
	}
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::AdvSmoothButton35Click(TObject *Sender)
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
void __fastcall TteachForm::AdvSmoothButton37Click(TObject *Sender)
{
    // delay 3초로 수정
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::AdvSmoothButton37MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    downTime = Now();
    isButtonPressed = true;
    unchuckAllTimer->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TteachForm::AdvSmoothButton37MouseUp(TObject *Sender, TMouseButton Button,
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

void __fastcall TteachForm::AdvSmoothButton38Click(TObject *Sender)
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
void __fastcall TteachForm::ApplyTeaching()
{
	AnsiString str;
	tea_memo->Clear();
	for(int i=0; i<4; ++i){
		str = "GP1" + IntToStr(i+1) + "," + teachEdit[0][i]->Text + "," + teachEdit[1][i]->Text + ",0";
		tea_memo->Lines->Add(str);
	}

	for(int i=4; i<8; ++i){
		str = "GP2" + IntToStr(i-3) + "," +  teachEdit[0][i]->Text + "," + teachEdit[1][i]->Text + ",0";
		tea_memo->Lines->Add(str);
	}
	tea_memo->Lines->Add("GP201,0,0," + teachEdit_z[0]->Text);	// 선별 z좌표
	tea_memo->Lines->Add("GP202,0,0," + teachEdit_z[1]->Text);	// 대상 z좌표

	SetTrayMaxPosition();
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::LoadTeaching()
{
	int pos;
	int channel;
	AnsiString  str;
	for(int i=0; i< tea_memo->Lines->Count; ++i){
		str = tea_memo->Lines->Strings[i];
		str = StringReplace(str, "GP", "", TReplaceFlags() << rfReplaceAll);

		pos = str.Pos(",");
		teach.point = str.SubString(1, pos-1);
		str.Delete(1, pos);
		pos = str.Pos(",");
		teach.str_x = str.SubString(1, pos-1);
		str.Delete(1, pos);
		pos = str.Pos(",");
		teach.str_y = str.SubString(1, pos-1);
		str.Delete(1, pos);
		teach.str_z = str;

		channel = teach.point.ToInt();
		if(channel > 10 && channel < 20){
			channel -= 11; // GP 11, 12, 13, 14
			teachEdit[0][channel]->Text = teach.str_x;
			teachEdit[1][channel]->Text = teach.str_y;
		}else if(channel > 20 && channel < 30){
			channel -= 17; // GP 21, 22, 23, 24
			teachEdit[0][channel]->Text = teach.str_x;
			teachEdit[1][channel]->Text = teach.str_y;
		}else if(channel == 201){ // GP 201     대상 z 축, chuck, unchuck
			teachEdit_z[0]->Text = teach.str_z;
			gpEdit19->Text = teach.str_x;
            gpEdit20->Text = teach.str_y;
		}else if(channel == 202){ // GP 202     선별 z 축, chuck, unchuck
			teachEdit_z[1]->Text = teach.str_z;
		}
	}

	SetTrayMaxPosition();
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::AdvSmoothButton4Click(TObject *Sender)
{

	if(OpenDialog->Execute()){
		tea_memo->Lines->LoadFromFile(OpenDialog->FileName);
		LoadTeaching();
	}
}
//---------------------------------------------------------------------------

void __fastcall TteachForm::AdvSmoothButton3Click(TObject *Sender)
{
	ApplyTeaching();
	if(SaveDialog->Execute()){
		tea_memo->Lines->SaveToFile(SaveDialog->FileName);
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
void __fastcall TteachForm::ChangeTrayMap(int channel)
{
    int nx, ny, nw, nh;
	nx = 15;
	ny = 585;
	nw = 140;

	if(channel == 96){
		// Source Tray Map
		nh = 20;
		for(int index=0; index<96;){
			sTray[index]->Caption->Text = MainForm->mapSort[0][index];
			sTray[index]->Tag = MainForm->mapSort[0][index];

			if(index < 48){
				sTray[index]->Left = nx;
				sTray[index]->Top = ny;
				sTray[index]->Height = nh;
				sTray[index]->Width = nw;
				sTray[index]->Fill->Color = clWhite;
				sTray[index]->Fill->ColorTo = clWhite;
				sTray[index]->Fill->ColorMirror = clWhite;
				sTray[index]->Fill->ColorMirrorTo = clWhite;
                sTray[index]->Visible = true;

				ny = ny - nh - 1;
				index += 1;

				if(index % 12 == 0) ny -= 4;
				if(index % 24 == 0){
					nx = nx + nw + 1;
					ny = 585;
				}
			}else{
				sTray[index]->Visible = true;
				index += 1;
			}
		}

        for(int i = 0; i < 12; i++)
			tTray[i]->Width = 140;

		for(int i = 6; i < 12; i++)
		{
			tTray[i]->Left = 156;
			tTray[i]->BringToFront();
		}

		Panel14->Caption = "CH 1";
		Panel17->Caption = "CH 25";
		Panel21->Caption = "CH 49";
		Panel24->Caption = "CH 73";

		Panel7->Caption = "CH 13";
		Panel11->Caption = "CH 19";

		for(int i = 0; i < 7; i++)
		{
			teachEdit[0][i]->Text = "";
			teachEdit[1][i]->Text = "";
		}
	}
}
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
		else if((btn->Tag < 4) && robostar->mr2.pos[4] != 0)
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
			int pos_z = robostar->mr2.pos[4];
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
void __fastcall TteachForm::SetTrayMaxPosition()
{
	sTray_Position.Top = 0;
	sTray_Position.Bottom = 0;
	sTray_Position.Left = 0;
	sTray_Position.Right = 0;

    sTray_Position.Left = teachEdit[1][0]->Text.ToInt();
    sTray_Position.Right = teachEdit[1][3]->Text.ToInt();

    for(int i = 0; i < 4; i++)   // 선별트레이 4열
    {
        if(sTray_Position.Bottom == 0 || sTray_Position.Bottom > teachEdit[0][i]->Text.ToInt())
            sTray_Position.Bottom = teachEdit[0][i]->Text.ToInt();
        if(sTray_Position.Top < teachEdit[0][i]->Text.ToInt())
            sTray_Position.Top = teachEdit[0][i]->Text.ToInt();
    }

	sTray_Position.Top += 1111000; // 선별 1열 X + 24열 X

	tTray_Position.Top = 0;
	tTray_Position.Bottom = 0;
	tTray_Position.Left = 0;
	tTray_Position.Right = 0;

	tTray_Position.Left = teachEdit[1][4]->Text.ToInt();
    tTray_Position.Right = teachEdit[1][7]->Text.ToInt();

    for(int i = 4; i < 8; i++)    // 대상트레이 2열
    {
        if(tTray_Position.Bottom == 0 || tTray_Position.Bottom > teachEdit[0][i]->Text.ToInt())
            tTray_Position.Bottom = teachEdit[0][i]->Text.ToInt();
        if(tTray_Position.Top < teachEdit[0][i]->Text.ToInt())
            tTray_Position.Top = teachEdit[0][i]->Text.ToInt();
    }

	tTray_Position.Top += 395000; // 대상 1열 X + 6열 X
}
//---------------------------------------------------------------------------
bool __fastcall TteachForm::CheckPositionDown(int gripperIndex)
{
	int pos_x = robostar->mr2.pos[1];
	int pos_y = robostar->mr2.pos[3];

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
	int gripper_position = robostar->mr2.pos[gripperIndex + 5];
	int chuck_position = teachEdit_CHUCK[1]->Text.ToIntDef(0);    // chuck
	int unchuck_position = teachEdit_CHUCK[0]->Text.ToIntDef(0);    // Unchuck

	int pos_x = robostar->mr2.pos[1];
	int pos_y = robostar->mr2.pos[3];

    if(((pos_x + (gripperIndex) * 90000) <= sTray_Position.Top && (pos_x + (gripperIndex) * 90000) >= sTray_Position.Bottom
		&& gripper_position == unchuck_position )
		|| (pos_x + (gripperIndex * 90000) <= tTray_Position.Top && pos_x + (gripperIndex * 90000) >= tTray_Position.Bottom && gripper_position == chuck_position)
		)
		return true;

	return false;
}
bool __fastcall TteachForm::CheckChuckPosition(int gripperIndex)
{
	int gripper_position = robostar->mr2.pos[gripperIndex + 5];
	int chuck_position = teachEdit_CHUCK[1]->Text.ToIntDef(0);    // chuck
	int unchuck_position = teachEdit_CHUCK[0]->Text.ToIntDef(0);    // Unchuck

	int pos_x = robostar->mr2.pos[1];
	int pos_y = robostar->mr2.pos[3];

	if(pos_x <= tTray_Position.Top && pos_x >= tTray_Position.Bottom
		&& gripper_position == chuck_position)
		return true;

	return false;
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::AdvSmoothButton_LoadFactorInfoClick(TObject *Sender)
{
	loadfactorForm->Left = pnlManualControl->Left + 120;
	loadfactorForm->Top = pnlManualControl->Top + 200;
	loadfactorForm->ShowModal();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void __fastcall TteachForm::LanguageChange(int index)
{
	TMemo *mm;
	if(index == 0) mm = Memo_Ko;
	else if(index == 1) mm = Memo_En;
	else if(index == 2) mm = Memo_Hu;

	AnsiString temp;

	Label1->Caption = mm->Lines->Strings[0];
	Label7->Caption = mm->Lines->Strings[2];
	Panel28->Caption = mm->Lines->Strings[3];
	Label2->Caption = mm->Lines->Strings[4];
	Panel27->Caption = mm->Lines->Strings[3];
	Panel20->Caption = mm->Lines->Strings[5];
	sCombo->Clear();
	for(int i = 0; i < 2; i++) sCombo->Items->Add(mm->Lines->Strings[6 + i]);
	sCombo->ItemIndex = 0;

	Label5->Caption = mm->Lines->Strings[15];
	Label4->Caption = mm->Lines->Strings[16];
	Panel35->Caption = mm->Lines->Strings[17];

	Panel31->Caption = mm->Lines->Strings[19];
	Panel33->Caption = mm->Lines->Strings[20];
	Panel37->Caption = mm->Lines->Strings[21];
	Panel29->Caption = mm->Lines->Strings[22];
	temp = mm->Lines->Strings[23];
	waitBtn->Caption = temp.SubString(1, temp.Pos(" ")) + "\n" + temp.SubString(temp.Pos(" ") + 1, temp.Length());
	AdvSmoothButton_Zup->Caption = mm->Lines->Strings[24];
	temp = mm->Lines->Strings[25];
	stopBtn->Caption = temp.SubString(1, temp.Pos(" ")) + "\n" + temp.SubString(temp.Pos(" ") + 1, temp.Length());
	Panel39->Caption = mm->Lines->Strings[26];
	Panel41->Caption = mm->Lines->Strings[27];
	Label6->Caption = mm->Lines->Strings[28];
	Label8->Caption = mm->Lines->Strings[29];
	Label9->Caption = mm->Lines->Strings[19];
	Label10->Caption = mm->Lines->Strings[20];
	Label29->Caption = mm->Lines->Strings[31];
	Label30->Caption = mm->Lines->Strings[32];
	Label31->Caption = mm->Lines->Strings[33];
	Label32->Caption = mm->Lines->Strings[34];
	CLR1->Caption = mm->Lines->Strings[35] + " #1";
	Label14->Caption = mm->Lines->Strings[35] + " #2";
	Label17->Caption = mm->Lines->Strings[35] + " #3";
	Label20->Caption = mm->Lines->Strings[35] + " #4";
	Label23->Caption = mm->Lines->Strings[35] + " #5";
	Label26->Caption = mm->Lines->Strings[35] + " #6";
	pup1->Caption = mm->Lines->Strings[36];
	pup2->Caption = mm->Lines->Strings[36];
	pup3->Caption = mm->Lines->Strings[36];
	pup4->Caption = mm->Lines->Strings[36];
	pup5->Caption = mm->Lines->Strings[36];
	pup6->Caption = mm->Lines->Strings[36];
	pflow1->Caption = mm->Lines->Strings[37];
	pflow2->Caption = mm->Lines->Strings[37];
	pflow3->Caption = mm->Lines->Strings[37];
	pflow4->Caption = mm->Lines->Strings[37];
	pflow5->Caption = mm->Lines->Strings[37];
	pflow6->Caption = mm->Lines->Strings[37];
	popen1->Caption = mm->Lines->Strings[38];
	popen2->Caption = mm->Lines->Strings[38];
	popen3->Caption = mm->Lines->Strings[38];
	popen4->Caption = mm->Lines->Strings[38];
	popen5->Caption = mm->Lines->Strings[38];
	popen6->Caption = mm->Lines->Strings[38];
	pclose1->Caption = mm->Lines->Strings[39];
	pclose2->Caption = mm->Lines->Strings[39];
	pclose3->Caption = mm->Lines->Strings[39];
	pclose4->Caption = mm->Lines->Strings[39];
	pclose5->Caption = mm->Lines->Strings[39];
	pclose6->Caption = mm->Lines->Strings[39];
	pdn1->Caption = mm->Lines->Strings[40];
	pdn2->Caption = mm->Lines->Strings[40];
	pdn3->Caption = mm->Lines->Strings[40];
	pdn4->Caption = mm->Lines->Strings[40];
	pdn5->Caption = mm->Lines->Strings[40];
	pdn6->Caption = mm->Lines->Strings[40];
	pcell1->Caption = mm->Lines->Strings[41];
	pcell2->Caption = mm->Lines->Strings[41];
	pcell3->Caption = mm->Lines->Strings[41];
	pcell4->Caption = mm->Lines->Strings[41];
	pcell5->Caption = mm->Lines->Strings[41];
	pcell6->Caption = mm->Lines->Strings[41];
	Label12->Caption = mm->Lines->Strings[42];
	Label15->Caption = mm->Lines->Strings[42];
	Label18->Caption = mm->Lines->Strings[42];
	Label21->Caption = mm->Lines->Strings[42];
	Label24->Caption = mm->Lines->Strings[42];
	Label27->Caption = mm->Lines->Strings[42];
	Label13->Caption = mm->Lines->Strings[43];
	Label16->Caption = mm->Lines->Strings[43];
	Label19->Caption = mm->Lines->Strings[43];
	Label22->Caption = mm->Lines->Strings[43];
	Label25->Caption = mm->Lines->Strings[43];
	Label28->Caption = mm->Lines->Strings[43];
	disableChk1->Caption = mm->Lines->Strings[44];
	disableChk2->Caption = mm->Lines->Strings[44];
	disableChk3->Caption = mm->Lines->Strings[44];
	disableChk4->Caption = mm->Lines->Strings[44];
	disableChk5->Caption = mm->Lines->Strings[44];
	disableChk6->Caption = mm->Lines->Strings[44];
	AdvSmoothButton_LoadFactorInfo->Caption = mm->Lines->Strings[46];
	Panel41->Caption = mm->Lines->Strings[47];
}
//---------------------------------------------------------------------------

void __fastcall TteachForm::btnApplyTeachingClick(TObject *Sender)
{
	try{
        if(typeEdit1->Text.Trim().IsEmpty())MessageBox(Handle, L"[C_Maint] KIND 정보를 확인하세요.", L"경고", MB_OK|MB_ICONWARNING);
        else if(typeEdit2->Text.Trim().IsEmpty())MessageBox(Handle, L"[C_Maint] 선별 트레이 CH1 정보를 확인하세요.", L"경고", MB_OK|MB_ICONWARNING);
        else if(typeEdit3->Text.Trim().IsEmpty())MessageBox(Handle, L"[C_Maint] 선별 트레이 CH25 정보를 확인하세요.", L"경고", MB_OK|MB_ICONWARNING);
        else if(typeEdit4->Text.Trim().IsEmpty())MessageBox(Handle, L"[C_Maint] 선별 트레이 CH49 정보를 확인하세요.", L"경고", MB_OK|MB_ICONWARNING);
        else if(typeEdit5->Text.Trim().IsEmpty())MessageBox(Handle, L"[C_Maint] 선별 트레이 CH73 정보를 확인하세요.", L"경고", MB_OK|MB_ICONWARNING);
        else if(typeEdit7->Text.Trim().IsEmpty())MessageBox(Handle, L"[C_Maint] 대상 트레이 CH1 정보를 확인하세요.", L"경고", MB_OK|MB_ICONWARNING);
        else if(typeEdit8->Text.Trim().IsEmpty())MessageBox(Handle, L"[C_Maint] 대상 트레이 CH7 정보를 확인하세요.", L"경고", MB_OK|MB_ICONWARNING);
        else if(typeEdit9->Text.Trim().IsEmpty())MessageBox(Handle, L"[C_Maint] 대상 트레이 CH13 정보를 확인하세요.", L"경고", MB_OK|MB_ICONWARNING);
        else if(typeEdit10->Text.Trim().IsEmpty())MessageBox(Handle, L"[C_Maint] 대상 트레이 CH19 정보를 확인하세요.", L"경고", MB_OK|MB_ICONWARNING);
        else if(typeEdit6->Text.Trim().IsEmpty())MessageBox(Handle, L"[C_Maint] GRIP1 정보를 확인하세요.", L"경고", MB_OK|MB_ICONWARNING);
        else if(typeEdit11->Text.Trim().IsEmpty())MessageBox(Handle, L"[C_Maint] GRIP2 정보를 확인하세요.", L"경고", MB_OK|MB_ICONWARNING);
        else{
            if(MessageBox(Handle, L"티칭 값을 저장 하시겠습니까?", L"저장", MB_YESNO|MB_ICONWARNING) == ID_YES){
                ApplyTeaching();
                tea_memo->Lines->SaveToFile(file);
            }
        }
	}catch(...){
		MessageBox(Handle, L"[C_Maint] 정보를 입력 하세요.", L"경고", MB_OK|MB_ICONWARNING);
	}
}
//---------------------------------------------------------------------------

void __fastcall TteachForm::SaveToFile()
{
	//* 2026 06 티칭값 파일 저장
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::LoadFromFile()
{
    //* 2026 06 티칭값 불러 오기
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::AddList()
{
	SaveToFile();
}
//---------------------------------------------------------------------------
void __fastcall TteachForm::ChangeTeaching()
{
	LoadTeaching();
}
//---------------------------------------------------------------------------

void __fastcall TteachForm::Panel47Click(TObject *Sender)
{
    tea_memo->Visible = !tea_memo->Visible;
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


void __fastcall TteachForm::btnKeyLockClick(TObject *Sender)
{
	if(BaseForm->btnKeyLock->Caption == "키락 해제")
		 {
			BaseForm->btnKeyLock->Caption = "키락 설정";
			robostar->KeyLock(2);
		 }
		 else
		 {
			BaseForm->btnKeyLock->Caption = "키락 해제";
			robostar->KeyLock(1);
		 }
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



