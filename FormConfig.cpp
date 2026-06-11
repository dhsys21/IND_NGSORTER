//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TConfigForm *ConfigForm;
//---------------------------------------------------------------------------
__fastcall TConfigForm::TConfigForm(TComponent* Owner)
	: TForm(Owner)
{

}
//---------------------------------------------------------------------------
void __fastcall TConfigForm::ApplyConfig()
{
	mes->pcName = pcEdit->Text;

	plc->Tag = 2;
	plc->client->Port = 8197;

	if(plc->client->Active == false){
    	plc->client->Active = true;
	}

	if(mes->ServerSocket->Active == false){
		mes->ServerSocket->Port = PortEdit->Text.ToInt();
		mes->ServerSocket->Active = true;
	}
}


void __fastcall TConfigForm::FormCreate(TObject *Sender)
{
	if(!DirectoryExists((AnsiString)APP_PATH))MkDir((AnsiString)APP_PATH);

	if(!DirectoryExists((AnsiString)BIN))MkDir((AnsiString)BIN);

	if(!DirectoryExists((AnsiString)LOG))MkDir((AnsiString)LOG);
	if(!DirectoryExists((AnsiString)SOCK_LOG))MkDir((AnsiString)SOCK_LOG);
	if(!DirectoryExists((AnsiString)PROG_LOG))MkDir((AnsiString)PROG_LOG);
	if(!DirectoryExists((AnsiString)ERROR_LOG))MkDir((AnsiString)ERROR_LOG);

	if(ReadSystemInfo()){
		ApplyConfig();
		BaseForm->config.file_exists = true;
	}
	else{
		BaseForm->config.file_exists = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TConfigForm::WriteSystemInfo(AnsiString type)
{
	TIniFile *ini;

	AnsiString file;
	file = (AnsiString)BIN + "MainSystemInfo.inf";

	ini = new TIniFile(file);

    if(type == "recipe")
		ini->WriteString("RECIPE", "NO", editRecipe->Text);
	else if(type == "speed")
	{
		ini->WriteString("SPEED", "SPEED", teachForm->speedEdit->Text);
		ini->WriteString("SPEED", "ACCL_SPEED", teachForm->acclSpeedEdit->Text);
		ini->WriteString("SPEED", "DCCL_SPEED", teachForm->dcclSpeedEdit->Text);
    }
	else
	{
		ini->WriteString("INFO", "PC", pcEdit->Text);
		ini->WriteString("COMMUNICATION", "IMS_PORT", PortEdit->Text);
	}

	if(chkZAxisUp->Checked == true)
		ini->WriteString("ZAXIS", "UP", "1");
	else
		ini->WriteString("ZAXIS", "UP", "0");

	delete ini;

}
//---------------------------------------------------------------------------
bool __fastcall TConfigForm::ReadSystemInfo()
{
	TIniFile *ini;

	AnsiString file;
	file = (AnsiString)BIN + "MainSystemInfo.inf";

	if(FileExists(file) == false){
		return false;
	}
	ini = new TIniFile(file);

	if(ini->ReadString("ZAXIS", "UP", "1") == "1")
		chkZAxisUp->Checked = true;
	else
        chkZAxisUp->Checked = false;

	pcEdit->Text = ini->ReadString("INFO", "PC", "H1DIF01A");
	PortEdit->Text = ini->ReadString("COMMUNICATION", "IMS_PORT", "7000");
	editRecipe->Text = ini->ReadString("RECIPE", "NO", "1");
	AnsiString recipe = editRecipe->Text;

	//* Servo Speed, Accl Speed, Dccl Speed
	teachForm->speedEdit->Text = ini->ReadString("SPEED", "SPEED", "500");
	teachForm->Panel_speedEdit->Caption = ini->ReadString("SPEED", "SPEED", "500");
	teachForm->acclSpeedEdit->Text = ini->ReadString("SPEED", "ACCL_SPEED", "1000");
	teachForm->dcclSpeedEdit->Text = ini->ReadString("SPEED", "DCCL_SPEED", "1000");

	if(recipe.ToInt() <= teachForm->searchList->Items->Count)
		teachForm->ChangeTeaching(recipe.ToInt() - 1);

	delete ini;
	return true;
}
//---------------------------------------------------------------------------
void __fastcall TConfigForm::FormShow(TObject *Sender)
{
	this->BringToFront();
}
//---------------------------------------------------------------------------
void __fastcall TConfigForm::Button1Click(TObject *Sender)
{
	mes->ServerSocket->Port = PortEdit->Text.ToInt();
	mes->ServerSocket->Active = true;	
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::AdvSmoothButton1Click(TObject *Sender)
{
	mes->ServerSocket->Active = false;

	if(mes->ThreadFlag){
		mes->ThreadFlag = false;

		Sleep(1000);

		delete mes->tx_data;

	}
	mes->bConnect = false;
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::AdvSmoothButton2Click(TObject *Sender)
{
	if(MessageBox(Handle, L"적용 하시겠습니까?", L"옵션", MB_YESNO|MB_ICONQUESTION) == ID_YES){
		ApplyConfig();
		this->WriteSystemInfo();
		this->Visible = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::AdvSmoothButton3Click(TObject *Sender)
{
	if(MessageBox(Handle, L"취소 하시겠습니까??", L"옵션", MB_YESNO|MB_ICONQUESTION) == ID_YES){
		this->ReadSystemInfo();
		this->Visible = false;
	}	
}
//---------------------------------------------------------------------------


void __fastcall TConfigForm::WritePlcData()
{
	WORD nData = 0;

	nData = *((WORD *)&MainForm->plcOutput + 0);
	plc->WriteWordData(0x4001, 1, IntToHex(nData, 4));
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::AdvSmoothButton6Click(TObject *Sender)
{
	MainForm->plcOutput.AUTO_RUN = 	1;
	WritePlcData();
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::AdvSmoothButton7Click(TObject *Sender)
{
    MainForm->plcOutput.AUTO_RUN = 	0;
	WritePlcData();
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::AdvSmoothButton9Click(TObject *Sender)
{
	MainForm->plcOutput.SRC_WORK = 	1;
	WritePlcData();
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::AdvSmoothButton8Click(TObject *Sender)
{
    MainForm->plcOutput.SRC_WORK = 	0;
	WritePlcData();
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::AdvSmoothButton11Click(TObject *Sender)
{
	MainForm->plcOutput.SRC_OUT = 	1;
	WritePlcData();
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::AdvSmoothButton10Click(TObject *Sender)
{
    MainForm->plcOutput.SRC_OUT = 	0;
	WritePlcData();
}
//---------------------------------------------------------------------------


void __fastcall TConfigForm::AdvSmoothButton14Click(TObject *Sender)
{
   MainForm->plcOutput.TARGET_OUT = 	1;
	WritePlcData();
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::AdvSmoothButton15Click(TObject *Sender)
{
	MainForm->plcOutput.TARGET_OUT = 	0;
	WritePlcData();
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::AdvSmoothButton16Click(TObject *Sender)
{
	 MainForm->plcOutput.SRC_MANUAL_WORK = 	1;
	WritePlcData();
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::AdvSmoothButton17Click(TObject *Sender)
{
  MainForm->plcOutput.SRC_MANUAL_WORK = 	0;
	WritePlcData();
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::AdvSmoothButton12Click(TObject *Sender)
{
	MainForm->plcOutput.SRC_EMP = 	1;
	WritePlcData();
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::AdvSmoothButton13Click(TObject *Sender)
{
   MainForm->plcOutput.SRC_EMP = 0;
	WritePlcData();
}
//---------------------------------------------------------------------------



