//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TErrorForm_insert *ErrorForm_insert;
//---------------------------------------------------------------------------
__fastcall TErrorForm_insert::TErrorForm_insert(TComponent* Owner)
	: TForm(Owner)
{
	this->Parent = BaseForm;
    this->Position = poDesigned;
    this->Height = 234;
    this->Left = 600;
    this->Top = 300;
}
//---------------------------------------------------------------------------
void __fastcall TErrorForm_insert::ShowError(AnsiString str1, AnsiString str2, int toolNo, int mesCode)
{
	errMsg1->Caption = "S_Maint_" + str1;
	errMsg2->Caption = str2;

	gripper->req_Pause(true);
	robostar->req_Pause(true);

	toolNum = toolNo - 1;
	ptoolNum->Caption = toolNo;
	pcode1->Caption = gripper->tool[toolNum].code;
	psource_ch1->Caption = gripper->tool[toolNum].source_ch;
	ptarget_ch1->Caption = gripper->tool[toolNum].target_ch;

	MainForm->NotifyAlarm(true, mesCode);
	this->Tag = mesCode;

	MainForm->WriteErrorLog(str1, str2);
	MainForm->BuzzerOn(true);
    MainForm->LampModeChange(LampAlarm);

	this->BringToFront();
	this->Show();
}
//---------------------------------------------------------------------------
void __fastcall TErrorForm_insert::AdvSmoothButton1Click(TObject *Sender)
{
    if(this->Height < 240) this->Height = 650;
    else this->Height = 234;

	MainForm->memoMainLineAdd("Manual switching");
	MainForm->stopBtnClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TErrorForm_insert::retryBtnClick(TObject *Sender)
{
	int targetChannel = ptarget_ch1->Caption.ToIntDef(0);
	if(targetChannel < 1 || targetChannel > MainForm->tray_target.SLOT_COUNT){
		MessageBox(Handle, L"Invalid Target Tray channel.", L"INSERT RETRY", MB_OK|MB_ICONWARNING);
		return;
	}

	MainForm->memoRobostarLineAdd(
		"[SAFE RETRY] INSERT restart from Z UP: Gripper=" + IntToStr(toolNum + 1) +
		" TargetCh=" + IntToStr(targetChannel));
	gripper->req_Pause(false);
	robostar->req_Stop(); // Cancel the saved failed step; retry creates a fresh position request.
	MainForm->playBtnClick(Sender);
	robostar->req_AutoInsert(2, toolNum + 1, targetChannel, 1, 96);
	this->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TErrorForm_insert::ignoreBtnClick(TObject *Sender)
{
	MainForm->memoMainLineAdd("Insert complete");
	gripper->req_Pause(false);
	robostar->req_Pause(false);
	robostar->req_InsertComplete();
	MainForm->playBtnClick(Sender);
	this->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TErrorForm_insert::AdvSmoothButton5Click(TObject *Sender)
{
	MainForm->BuzzerOn(false);
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm_insert::btnMoveSourceClick(TObject *Sender)
{
	int map = 0;

	map = psource_ch1->Caption.ToInt();
	robostar->req_AutoMove(1, toolNum+1, map, 96);
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm_insert::btnMoveTargetClick(TObject *Sender)
{
	robostar->req_AutoMove(2, toolNum+1, ptarget_ch1->Caption.ToInt(), 96);
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm_insert::btnOpenClick(TObject *Sender)
{
	TAdvSmoothButton *btn;
	btn = (TAdvSmoothButton*)Sender;

	robostar->GripperChuck(btn->Tag, true, false);	
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm_insert::btnCloseClick(TObject *Sender)
{
	TAdvSmoothButton *btn;
	btn = (TAdvSmoothButton*)Sender;

	robostar->GripperChuck(btn->Tag, false, true);	
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm_insert::btnUpClick(TObject *Sender)
{
	if(robostar->seq == seqIdle || robostar->seq == seqPause)
		robostar->req_zUp();
	else
		MessageBox(Handle, L"Another servo sequence is running.",
			L"Z Axis Up", MB_OK|MB_ICONWARNING);
}
//---------------------------------------------------------------------------
void __fastcall TErrorForm_insert::btnDownClick(TObject *Sender)
{
	if(robostar->move.pallet == 1 && robostar->getGripperChuckStatus()){
		MessageBox(Handle,
			L"The gripper is CHUCK. Z DOWN is blocked at the Source Tray.",
			L"Source Tray Z Down Interlock", MB_OK|MB_ICONWARNING);
		return;
	}
	if(!robostar->req_zDown()){
		MessageBox(Handle,
			L"Check the selected Source/Target channel, servo status, and Z teaching value.",
			L"Z Axis Down", MB_OK|MB_ICONWARNING);
	}
}
//---------------------------------------------------------------------------
void __fastcall TErrorForm_insert::FormHide(TObject *Sender)
{
	MainForm->NotifyAlarm(false, this->Tag);
	MainForm->BuzzerOn(false);
	MainForm->LampModeChange(MainForm->beforeLampMode);
}
//---------------------------------------------------------------------------

