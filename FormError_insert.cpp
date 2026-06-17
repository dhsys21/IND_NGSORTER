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
	int map = 0;

	MainForm->memoMainLineAdd("Retry");
	map = psource_ch1->Caption.ToInt();

	if(robostar->move.pallet == 2 && robostar->move.channel == map){
		gripper->req_Pause(false);
		robostar->req_Pause(false);
		MainForm->playBtnClick(Sender);
		this->Visible = false;
	}else{
		MessageBox(Handle, L"Move the robot to the target tray.", L"Sorting", MB_OK|MB_ICONQUESTION);
	}
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

void __fastcall TErrorForm_insert::AdvSmoothButton2Click(TObject *Sender)
{
	int map = 0;

	map = psource_ch1->Caption.ToInt();
	robostar->req_AutoMove(1, toolNum+1, map, 96);
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm_insert::AdvSmoothButton3Click(TObject *Sender)
{
	robostar->req_AutoMove(2, toolNum+1, ptarget_ch1->Caption.ToInt(), 96);
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm_insert::AdvSmoothButton13Click(TObject *Sender)
{
	TAdvSmoothButton *btn;
	btn = (TAdvSmoothButton*)Sender;

	robostar->GripperDown(btn->Tag, false, true);	
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm_insert::AdvSmoothButton14Click(TObject *Sender)
{
	TAdvSmoothButton *btn;
	btn = (TAdvSmoothButton*)Sender;

	robostar->GripperDown(btn->Tag, true, false);	
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm_insert::AdvSmoothButton12Click(TObject *Sender)
{
	TAdvSmoothButton *btn;
	btn = (TAdvSmoothButton*)Sender;

	robostar->GripperChuck(btn->Tag, true, false);	
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm_insert::AdvSmoothButton11Click(TObject *Sender)
{
	TAdvSmoothButton *btn;
	btn = (TAdvSmoothButton*)Sender;

	robostar->GripperChuck(btn->Tag, false, true);	
}
//---------------------------------------------------------------------------

void __fastcall TErrorForm_insert::FormHide(TObject *Sender)
{
	MainForm->NotifyAlarm(false, this->Tag);
	MainForm->BuzzerOn(false);
	MainForm->LampModeChange(MainForm->beforeLampMode);
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void __fastcall TErrorForm_insert::LanguageChange(int index)
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
	AdvSmoothButton3->Caption = mm->Lines->Strings[12];
	Panel25->Caption = mm->Lines->Strings[14];
}
//---------------------------------------------------------------------------

