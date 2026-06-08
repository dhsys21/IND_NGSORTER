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
}
//---------------------------------------------------------------------------
void __fastcall TErrorForm_eject::ShowError(AnsiString str1, AnsiString str2, int toolNo, int mesCode)
{
	this->Height = 230;
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

	if(MainForm->tray_source.SLOT_COUNT == 96){
		map = psource_ch1->Caption.ToInt();
	}
	else if(MainForm->tray_source.SLOT_COUNT == 48){
		map = MainForm->mapSort[1][psource_ch1->Caption.ToInt()-1];
	}

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
	this->Height = 650;
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

void __fastcall TErrorForm_eject::AdvSmoothButton1Click(TObject *Sender)
{
	int map = 0;

	if(MainForm->tray_source.SLOT_COUNT == 96){
		map = psource_ch1->Caption.ToInt();
		robostar->req_AutoMove(1, toolNum+1, map, 96);
	}
	else if(MainForm->tray_source.SLOT_COUNT == 48){
		map = MainForm->mapSort[1][psource_ch1->Caption.ToInt()-1];
		robostar->req_AutoMove(1, toolNum+1, map, 48);
	}
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm_eject::AdvSmoothButton2Click(TObject *Sender)
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
void __fastcall TErrorForm_eject::LanguageChange(int index)
{
	TMemo *mm;
	if(index == 0) mm = Memo_Ko;
	else if(index == 1) mm = Memo_En;
	else if(index == 2) mm = Memo_Hu;

	CLR1->Caption = mm->Lines->Strings[0] + " #1";
	Label14->Caption = mm->Lines->Strings[0] + " #2";
	Label17->Caption = mm->Lines->Strings[0] + " #3";
	Label20->Caption = mm->Lines->Strings[0] + " #4";
	Label23->Caption = mm->Lines->Strings[0] + " #5";
	Label26->Caption = mm->Lines->Strings[0] + " #6";
	pup1->Caption = mm->Lines->Strings[1];
	pup2->Caption = mm->Lines->Strings[1];
	pup3->Caption = mm->Lines->Strings[1];
	pup4->Caption = mm->Lines->Strings[1];
	pup5->Caption = mm->Lines->Strings[1];
	pup6->Caption = mm->Lines->Strings[1];
	pflow1->Caption = mm->Lines->Strings[2];
	pflow2->Caption = mm->Lines->Strings[2];
	pflow3->Caption = mm->Lines->Strings[2];
	pflow4->Caption = mm->Lines->Strings[2];
	pflow5->Caption = mm->Lines->Strings[2];
	pflow6->Caption = mm->Lines->Strings[2];
	popen1->Caption = mm->Lines->Strings[3];
	popen2->Caption = mm->Lines->Strings[3];
	popen3->Caption = mm->Lines->Strings[3];
	popen4->Caption = mm->Lines->Strings[3];
	popen5->Caption = mm->Lines->Strings[3];
	popen6->Caption = mm->Lines->Strings[3];
	pclose1->Caption = mm->Lines->Strings[4];
	pclose2->Caption = mm->Lines->Strings[4];
	pclose3->Caption = mm->Lines->Strings[4];
	pclose4->Caption = mm->Lines->Strings[4];
	pclose5->Caption = mm->Lines->Strings[4];
	pclose6->Caption = mm->Lines->Strings[4];
	pdn1->Caption = mm->Lines->Strings[5];
	pdn2->Caption = mm->Lines->Strings[5];
	pdn3->Caption = mm->Lines->Strings[5];
	pdn4->Caption = mm->Lines->Strings[5];
	pdn5->Caption = mm->Lines->Strings[5];
	pdn6->Caption = mm->Lines->Strings[5];
	pcell1->Caption = mm->Lines->Strings[6];
	pcell2->Caption = mm->Lines->Strings[6];
	pcell3->Caption = mm->Lines->Strings[6];
	pcell4->Caption = mm->Lines->Strings[6];
	pcell5->Caption = mm->Lines->Strings[6];
	pcell6->Caption = mm->Lines->Strings[6];
	Label12->Caption = mm->Lines->Strings[7];
	Label15->Caption = mm->Lines->Strings[7];
	Label18->Caption = mm->Lines->Strings[7];
	Label21->Caption = mm->Lines->Strings[7];
	Label24->Caption = mm->Lines->Strings[7];
	Label27->Caption = mm->Lines->Strings[7];
	Label13->Caption = mm->Lines->Strings[8];
	Label16->Caption = mm->Lines->Strings[8];
	Label19->Caption = mm->Lines->Strings[8];
	Label22->Caption = mm->Lines->Strings[8];
	Label25->Caption = mm->Lines->Strings[8];
	Label28->Caption = mm->Lines->Strings[8];
	Label1->Caption = mm->Lines->Strings[9];
	Panel2->Caption = mm->Lines->Strings[10];
	Panel26->Caption = mm->Lines->Strings[11];
	AdvSmoothButton2->Caption = mm->Lines->Strings[12];
	Panel29->Caption = mm->Lines->Strings[13];
	AdvSmoothButton1->Caption = mm->Lines->Strings[12];
	Panel25->Caption = mm->Lines->Strings[14];
}
//---------------------------------------------------------------------------
