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
	text[3] = MainErr5;

	perr[0] = perr1;
	perr[1] = perr2;
	perr[2] = perr3;
	perr[3] = perr5;

	for(int i = 0; i < 4; ++i){
		text[i]->Visible = false;
		perr[i]->ParentBackground = false;
		perr[i]->Color = clRed;
		perr[i]->Font->Color = clWhite;
		perr[i]->Visible = false;
	}

	isGripperOpen1 = false;
	isGripperOpen2 = false;

	// Keep the compact safety I/O indicators visible inside AdvSmoothPanel4.
	pSafetyEmgReady->Parent = AdvSmoothPanel4;
	pSafetyDoorReady->Parent = AdvSmoothPanel4;
	pSafetyEmgReady->Visible = true;
	pSafetyDoorReady->Visible = true;
	pSafetyEmgReady->BringToFront();
	// Load the large drawing after DFM streaming. Keeping Picture.Data out of
	// the DFM prevents image-property EReadError during form construction.
	AnsiString drawingPath = ExtractFilePath(Application->ExeName) + "NGSORTER.png";
	if(!FileExists(drawingPath))
		drawingPath = "D:\\Program\\NGSORTER.png";
	if(FileExists(drawingPath)){
		try{
			TPngImage *drawing = new TPngImage();
			try{
				drawing->LoadFromFile(drawingPath);
				imgMachineDrawing->Picture->Assign(drawing);
			}
			__finally{
				delete drawing;
			}
		}
		catch(const Exception &){
			imgMachineDrawing->Picture->Assign(NULL);
		}
	}
	imgMachineDrawing->SendToBack();
	pSafetyDoorReady->BringToFront();
	lblRecoverySequence->BringToFront();
	errTimer->Enabled = false;
}
//---------------------------------------------------------------------------
void __fastcall TdoorForm::ShowError(AnsiString MainStr, AnsiString SubStr, int errCode)
{
	if(errCode < 0 || errCode > 5 || errCode == 3) return;
	if(errCode == 5 && !robostar->IsSscOpened()) return;
	if(errCode == 0 && !robostar->IsSafetyDoorOpen(1)) return;
	if(errCode == 1 && !robostar->IsSafetyDoorOpen(2)) return;

	MainForm->pause_stopBtnClick(this);

	FormStyle = fsStayOnTop;
	if(errCode >= 0 && errCode <= 2)
		text[errCode]->Visible = true;
	else if(errCode == 4)
		text[3]->Visible = true;

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
	for(int i = 0; i < 4; ++i){
		text[i]->Visible = false;
		perr[i]->ParentBackground = false;
		perr[i]->Color = clRed;
		perr[i]->Font->Color = clWhite;
		perr[i]->Visible = false;
	}

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
	// KEYLOCK release is an alarm; a confirmed locked state is normal.
	text[3]->Visible = !robostar->IsKeyLockActive();

	bool keyLockSetOutput = robostar->gripper.DOOR_LEFT_CLOSE
		&& robostar->gripper.DOOR_RIGHT_CLOSE;
	bool keyLockReleaseOutput = !robostar->gripper.DOOR_LEFT_CLOSE
		&& !robostar->gripper.DOOR_RIGHT_CLOSE;
	btnSetKEYLOCK->Color = keyLockSetOutput ? clLime : (TColor)16744448;
	btnKeyUnlock->Color = keyLockReleaseOutput ? clLime : (TColor)16744448;
	btnSetBypass->Color = robostar->gripper.DOOR_OPEN_SELECT ? clLime : (TColor)16744448;

	bool safetyEmgReady = robostar->input.SAFETY_EMG_READY;
	bool safetyDoorReady = robostar->input.SAFETY_DOOR_READY;
	btnSafetyResetDoor->Enabled = MainForm->path == 81
		&& !robostar->IsSoftwareSafetyResetActive();
	btnSafetyResetDoor->Color = robostar->gripper.SAFETY_RESET ? clLime : clWhite;
	pSafetyEmgReady->Color = safetyEmgReady ? clLime : clSilver;
	pSafetyDoorReady->Color = safetyDoorReady ? clLime : clSilver;
	lblSafetyResetGuide->Visible = !(safetyEmgReady && safetyDoorReady);
	//btnSetKEYLOCK->Visible = robostar->output.SAFETY_DOOR;
	//btnSetKEYLOCK->Visible = robostar->gripper.DOOR_OPEN_SELECT;
	if(MainForm->popen->Color != clLime) btnServoOpen->Visible = true;
    else btnServoOpen->Visible = false;

	// Flash every diagram alarm by inverting its own colors. This keeps the
	// effect even when no separate AdvSmoothPanel background exists.
	for(int i = 0; i < 4; ++i){
		if(text[i]->Visible){
			bool redPhase = (perr[i]->Color == clRed);
			perr[i]->Color = redPhase ? clWhite : clRed;
			perr[i]->Font->Color = redPhase ? clRed : clWhite;
			perr[i]->Visible = true;
			perr[i]->BringToFront();
		}
		else{
			perr[i]->Visible = false;
			perr[i]->Color = clRed;
			perr[i]->Font->Color = clWhite;
		}
	}
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
void __fastcall TdoorForm::btnSetBypassClick(TObject *Sender)
{
	if(!robostar->Bypass(true))
		ShowMessage(L"Close both doors and set KEYLOCK before turning BY-PASS ON.");
}
//---------------------------------------------------------------------------

void __fastcall TdoorForm::btnServoOpenClick(TObject *Sender)
{
	robostar->req_Init();
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
	if(robostar->getCellDetectStatus(btn->Tag)) // active-low sensor: true => cell detected
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
	if(robostar->getCellDetectStatus(btn->Tag)) // active-low sensor: true => cell detected
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
	if(!robostar->KeyLock(false))
		ShowMessage(L"KEYLOCK release requires manual mode and the hardware BY-PASS switch ON.");
}
//---------------------------------------------------------------------------
void __fastcall TdoorForm::btnSafetyResetDoorClick(TObject *Sender)
{
	if(!robostar->RequestSafetyResetPulse())
		ShowMessage(L"Cannot output SAFETY RESET because CC-Link is not connected.");
}//---------------------------------------------------------------------------
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

