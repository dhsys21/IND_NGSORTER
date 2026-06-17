//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TErrorForm_eject *ErrorForm_eject;
//---------------------------------------------------------------------------
__fastcall TErrorForm_eject::TErrorForm_eject(TComponent* Owner)
	: TForm(Owner)
{
	this->Parent = BaseForm;

    this->Position = poDesigned;
    this->Height = 234;
    this->Left = 600;
    this->Top = 300;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TErrorForm_eject::ShowError(AnsiString str1, AnsiString str2, int toolNo, int mesCode)
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
void __fastcall TErrorForm_eject::retryBtnClick(TObject *Sender)
{
	int map = 0;

	MainForm->memoMainLineAdd("Retry");
    map = psource_ch1->Caption.ToInt();

	if(robostar->move.pallet == 1 && robostar->move.channel == map){
		gripper->req_Pause(false);
		robostar->req_Pause(false);
		MainForm->playBtnClick(Sender);
		this->Visible = false;
	}else{
		MessageBox(Handle, L"Move the robot to the source tray.", L"Sorting", MB_OK|MB_ICONQUESTION);
	}
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm_eject::ignoreBtnClick(TObject *Sender)
{
	MainForm->memoMainLineAdd("Eject complete");
	gripper->req_Pause(false);
	robostar->req_Pause(false);
	robostar->req_EjectComplete();
	MainForm->playBtnClick(Sender);
	this->Visible = false;
}
//---------------------------------------------------------------------------


void __fastcall TErrorForm_eject::manualBtnClick(TObject *Sender)
{
    if(this->Height < 240) this->Height = 650;
    else this->Height= 234;

	MainForm->stopBtnClick(Sender);
	MainForm->memoMainLineAdd("Manual switching");
}
//---------------------------------------------------------------------------


void __fastcall TErrorForm_eject::AdvSmoothButton13Click(TObject *Sender)
{
	TAdvSmoothButton *btn;
	btn = (TAdvSmoothButton*)Sender;

	robostar->GripperDown(btn->Tag, false, true);	
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm_eject::AdvSmoothButton14Click(TObject *Sender)
{
	TAdvSmoothButton *btn;
	btn = (TAdvSmoothButton*)Sender;

	robostar->GripperDown(btn->Tag, true, false);	
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm_eject::AdvSmoothButton12Click(TObject *Sender)
{
	TAdvSmoothButton *btn;
	btn = (TAdvSmoothButton*)Sender;

	robostar->GripperChuck(btn->Tag, true, false);	
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm_eject::AdvSmoothButton11Click(TObject *Sender)
{
	TAdvSmoothButton *btn;
	btn = (TAdvSmoothButton*)Sender;

	robostar->GripperChuck(btn->Tag, false, true);	
}
//---------------------------------------------------------------------------
void __fastcall TErrorForm_eject::btnMoveTargetClick(TObject *Sender)
{
	int map = 0;

	map = psource_ch1->Caption.ToInt();
	robostar->req_AutoMove(1, toolNum+1, map, 96);
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm_eject::btnMoveSourceClick(TObject *Sender)
{
	robostar->req_AutoMove(2, toolNum+1, ptarget_ch1->Caption.ToInt(), 96);
}
//---------------------------------------------------------------------------
void __fastcall TErrorForm_eject::AdvSmoothButton5Click(TObject *Sender)
{
	MainForm->BuzzerOn(false);
}
//---------------------------------------------------------------------------
void __fastcall TErrorForm_eject::FormHide(TObject *Sender)
{
	MainForm->NotifyAlarm(false, this->Tag);
	MainForm->BuzzerOn(false);
    MainForm->LampModeChange(MainForm->beforeLampMode);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------



