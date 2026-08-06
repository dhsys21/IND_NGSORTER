//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TdoorForm *doorForm;
//---------------------------------------------------------------------------
__fastcall TdoorForm::TdoorForm(TComponent* Owner)
	: TForm(Owner)
{
	this->Parent = BaseForm;
	text[0] = MainErr1;
	text[1] = MainErr2;
	text[2] = MainErr3;
	text[3] = MainErr4;
	text[4] = MainErr5;

	perr[0] = perr1;
	perr[1] = perr2;
	perr[2] = perr3;
	perr[3] = perr4;
	perr[4] = perr5;

	for(int i = 0; i < 5; ++i){
		text[i]->Visible = false;
		perr[i]->Visible = false;
	}
	StaticText4->Visible = false;

	isGripperOpen1 = false;
	isGripperOpen2 = false;

	errTimer->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TdoorForm::ShowError(AnsiString MainStr, AnsiString SubStr, int errCode)
{
	if(errCode < 0 || errCode > 5) return;
	if(errCode == 0 && !robostar->IsSafetyDoorOpen(1)) return;
	if(errCode == 1 && !robostar->IsSafetyDoorOpen(2)) return;

	MainForm->pause_stopBtnClick(this);

	FormStyle = fsStayOnTop;
	if(errCode == 5) StaticText4->Visible = true;
	else text[errCode]->Visible = true;

	flag = true;
	if(this->Visible == false){
		isGripperOpen1 = false;
		isGripperOpen2 = false;
	}

	if(this->Visible == false){
		m_errCode = errCode + 25;
		if(errCode == 4 || errCode == 5) m_errCode -= 3;

		MainForm->NotifyAlarm(true, m_errCode, false);

		MainForm->WriteErrorLog(MainStr, SubStr);
		MainForm->BuzzerOn(true);
		MainForm->LampModeChange(LampEmergency);
		this->BringToFront();
		if(MainForm->CheckBox1->Checked == true)
			this->Visible = false;
        else
			this->Visible = true;
		errTimer->Enabled = true;
		okBtn->Visible = false;
        MainForm->manualBtnClick(this);
	}


	MainForm->NotifyEquipStatus("DOWN");
}
//---------------------------------------------------------------------------
void __fastcall TdoorForm::AdvSmoothButton4Click(TObject *Sender)
{
	MainForm->BuzzerOn(false);
}
//---------------------------------------------------------------------------
void __fastcall TdoorForm::FormHide(TObject *Sender)
{
	MainForm->BuzzerOn(false);
	MainForm->LampModeChange(MainForm->beforeLampMode);
	MainForm->NotifyAlarm(false, m_errCode, false);
	for(int i = 0; i < 5; ++i){
		text[i]->Visible = false;
		perr[i]->Visible = false;
	}
	StaticText4->Visible = false;

	MainForm->NotifyEquipStatus("CLEAR");
}
//---------------------------------------------------------------------------
void __fastcall TdoorForm::errTimerTimer(TObject *Sender)
{
	if(MainForm->m_ServoHome || MainForm->m_ServoHomeEmg)
		pnlOpenGripper->Visible = true;
	else
		pnlOpenGripper->Visible = false;

	if(robostar->IsSafetyDoorOpen(1) || robostar->IsSafetyDoorOpen(2)
		|| robostar->IsEmergencyStopActive() || MainForm->popen->Color != clLime) // || !robostar->input.s .SERVO_POWER){
	{
		okBtn->Visible = false;
	}else{
		okBtn->Visible = true;
	}

	isDoorOpen = robostar->IsSafetyDoorOpen(1) || robostar->IsSafetyDoorOpen(2);

//	if(isDoorOpen == false)
//	{
//		isDoorOpen = false;
//		BaseForm->btnKeyLock->Caption = "키락 해제";
//		robostar->KeyLock(true);
//	}

	if(robostar->IsEmergencyStopActive())
		robostar->KeyLock(false);

//	if(isDoorOpen == true && robostar->input.SAFETY_DOOR_1 == 0 && robostar->input.SAFETY_DOOR_2 == 0 && robostar->input.SAFETY_DOOR_3 == 0)
//	{
//		isDoorOpen = false;
//		BaseForm->btnKeyLock->Caption = "키락 해제";
//		robostar->KeyLock(false);
//	}


	text[0]->Visible = robostar->IsSafetyDoorOpen(1);
	text[1]->Visible = robostar->IsSafetyDoorOpen(2);
	text[2]->Visible = robostar->IsEmergencyStopActive();
	text[3]->Visible = false;
	text[4]->Visible = robostar->IsKeyLockActive();
	StaticText4->Visible = MainForm->popen->Color != clLime;

	//btnSetKEYLOCK->Visible = robostar->output.SAFETY_DOOR;
	//btnSetKEYLOCK->Visible = robostar->gripper.DOOR_OPEN_SELECT;
	if(MainForm->popen->Color != clLime) btnServoOpen->Visible = true;
    else btnServoOpen->Visible = false;

	for(int i = 0; i < 5; ++i)
		if(text[i]->Visible){
			perr[i]->Visible = !perr[i]->Visible;
		}
		else perr[i]->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TdoorForm::okBtnClick(TObject *Sender)
{
    robostar->KeyLock(true);
	errTimer->Enabled = false;
	this->Close();
	flag = false;
}
//---------------------------------------------------------------------------


void __fastcall TdoorForm::btnSetKEYLOCKClick(TObject *Sender)
{
	robostar->KeyLock(true);
}
//---------------------------------------------------------------------------

void __fastcall TdoorForm::btnServoOpenClick(TObject *Sender)
{
	//robostar->req_AutoRun();
    robostar->req_EmgAutoRun();
	if(gripper->seq == 4) gripper->step.step = 0;   //  seqPause
}
//---------------------------------------------------------------------------


void __fastcall TdoorForm::stopBtnClick(TObject *Sender)
{
    robostar->req_Stop();
}
//---------------------------------------------------------------------------

void __fastcall TdoorForm::btnGripper1OpenMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
    if(Button == mbLeft){
		robostar->req_JogMove(-1);
	}
}
//---------------------------------------------------------------------------

void __fastcall TdoorForm::btnGripper1OpenMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
	TAdvSmoothButton *btn;
	btn = (TAdvSmoothButton*)Sender;

    if(isGripperOpen1 == true && MainForm->m_ServoHomeEmg)
		robostar->req_JogMove(btn->Tag);

    // 2019 07 05 HOME 위치에서 그리퍼 [열기]
	if(robostar->getCellDetectStatus(btn->Tag) == false) // getCellDetectStatus(btn->Tag) == false =>셀이 있을 때
	{
		if(isGripperOpen1 == false && MainForm->m_ServoHomeEmg)
		{
            UnicodeString str = BaseForm->GetLangStr("MSG_GRIPPER_UNCHUCK_ALARM1") + IntToStr(btn->Tag - 6);
			if(MessageBox(Handle, str.c_str(), L"UNCHUCK", MB_YESNO|MB_ICONWARNING) == ID_YES)
				isGripperOpen1 = true;
		}
	}
	else
	{
		if(isGripperOpen1 == false && MainForm->m_ServoHomeEmg)
		{
        	isGripperOpen1 = true;
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TdoorForm::btnGripper2OpenMouseDown(TObject *Sender, TMouseButton Button,
		  TShiftState Shift, int X, int Y)
{
    TAdvSmoothButton *btn;
	btn = (TAdvSmoothButton*)Sender;

    if(isGripperOpen2 == true && MainForm->m_ServoHomeEmg)
		robostar->req_JogMove(btn->Tag);

    // 2019 07 05 HOME 위치에서 그리퍼 [열기]
	if(robostar->getCellDetectStatus(btn->Tag) == false) // getCellDetectStatus(btn->Tag) == false =>셀이 있을 때
	{
		if(isGripperOpen2 == false && MainForm->m_ServoHomeEmg)
		{
            UnicodeString str = BaseForm->GetLangStr("MSG_GRIPPER_UNCHUCK_ALARM1") + IntToStr(btn->Tag - 7);
			if(MessageBox(Handle, str.c_str(), L"UNCHUCK", MB_YESNO|MB_ICONWARNING) == ID_YES)
				isGripperOpen2 = true;
		}
	}
	else
	{
		if(isGripperOpen2 == false && MainForm->m_ServoHomeEmg)
		{
			isGripperOpen2 = true;
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TdoorForm::btnKeyUnlockClick(TObject *Sender)
{
	robostar->KeyLock(false);
}
//---------------------------------------------------------------------------
void __fastcall TdoorForm::Label3DblClick(TObject *Sender)
{
    pPassword->Visible = !pPassword->Visible;
    PassEdit->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TdoorForm::cancelBtn2Click(TObject *Sender)
{
    pPassword->Visible = false;
}
//---------------------------------------------------------------------------

void __fastcall TdoorForm::PasswordBtnClick(TObject *Sender)
{
    if(PassEdit->Text == "9090"){
        MainForm->CheckBox1->Visible = true;
	    MainForm->CheckBox1->Checked = true;
    	this->Visible = false;
    }
}
//---------------------------------------------------------------------------

void __fastcall TdoorForm::PassEditKeyUp(TObject *Sender, WORD &Key, TShiftState Shift)

{
    if(Key == VK_RETURN)
    {
        if(PassEdit->Text == "9090"){
            MainForm->CheckBox1->Visible = true;
            MainForm->CheckBox1->Checked = true;
            this->Visible = false;
        }
    }
}
//---------------------------------------------------------------------------


