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
	this->Width = 920;
	this->Height = 430;
}
//---------------------------------------------------------------------------
void __fastcall TConfigForm::ApplyConfig()
{
	UpdateCommunicationConfigFromEdits();
	if(editFmsIp != NULL)
		BaseForm->config.fmsIp = editFmsIp->Text;
	BaseForm->config.gatewayPort = PortEdit->Text.ToIntDef(18080);
	mes->pcName = pcEdit->Text;
	if(Mod_Fms != NULL)
		Mod_Fms->Configure(BaseForm->config.fmsIp, BaseForm->config.gatewayPort);

	if(MainForm != NULL){
		for(int i = 0; i < 2; ++i){
			if(MainForm->comBcr[i] != NULL && MainForm->comBcr[i]->ClientSocketBcr->Active)
				MainForm->comBcr[i]->Disconnect();
		}
		MainForm->InitBarcodeAndSmoke();
	}
}


void __fastcall TConfigForm::FormCreate(TObject *Sender)
{
	CreateCommunicationControls();
	if(!DirectoryExists((AnsiString)APP_PATH))MkDir((AnsiString)APP_PATH);

	if(!DirectoryExists((AnsiString)BIN))MkDir((AnsiString)BIN);
	if(!DirectoryExists((AnsiString)TRAY_PATH))MkDir((AnsiString)TRAY_PATH);

	if(!DirectoryExists((AnsiString)LOG))MkDir((AnsiString)LOG);
	if(!DirectoryExists((AnsiString)SOCK_LOG))MkDir((AnsiString)SOCK_LOG);
	if(!DirectoryExists((AnsiString)PROG_LOG))MkDir((AnsiString)PROG_LOG);
	if(!DirectoryExists((AnsiString)ERROR_LOG))MkDir((AnsiString)ERROR_LOG);

	if(ReadSystemInfo()){
		ApplyConfig();
		BaseForm->config.file_exists = true;
	}
	else{
		// Apply the defaults shown on FormConfig when the INI is absent.
		ApplyConfig();
		BaseForm->config.file_exists = false;
	}

	// Start listening automatically using the FormConfig FMS Gateway IP/port.
	// When the listen socket cannot be opened, TMod_Fms retries every 5 seconds.
	if(Mod_Fms != NULL)
		Mod_Fms->Start();
}
//---------------------------------------------------------------------------
TPanel* __fastcall TConfigForm::AddFieldLabel(TWinControl *Parent, int Left, int Top, int Width, AnsiString Caption)
{
	TPanel *Panel = new TPanel(this);
	Panel->Parent = Parent;
	Panel->Left = Left;
	Panel->Top = Top;
	Panel->Width = Width;
	Panel->Height = 24;
	Panel->BevelKind = bkFlat;
	Panel->BevelOuter = bvNone;
	Panel->Caption = Caption;
	Panel->Color = (TColor)15656921;
	Panel->ParentBackground = false;
	Panel->Font->Style = TFontStyles() << fsBold;
	return Panel;
}
//---------------------------------------------------------------------------
TEdit* __fastcall TConfigForm::AddFieldEdit(TWinControl *Parent, int Left, int Top, int Width, AnsiString Text)
{
	TEdit *Edit = new TEdit(this);
	Edit->Parent = Parent;
	Edit->Left = Left;
	Edit->Top = Top;
	Edit->Width = Width;
	Edit->Height = 24;
	Edit->Text = Text;
	return Edit;
}
//---------------------------------------------------------------------------
TButton* __fastcall TConfigForm::AddActionButton(TWinControl *Parent, int Left, int Top, int Width, AnsiString Caption, TNotifyEvent OnClick)
{
	TButton *Button = new TButton(this);
	Button->Parent = Parent;
	Button->Left = Left;
	Button->Top = Top;
	Button->Width = Width;
	Button->Height = 25;
	Button->Caption = Caption;
	Button->OnClick = OnClick;
	return Button;
}
//---------------------------------------------------------------------------
void __fastcall TConfigForm::CreateCommunicationControls()
{
	if(btnConPLC != NULL) btnConPLC->OnClick = btnPlcConnClick;
	if(btnDisconPLC != NULL) btnDisconPLC->OnClick = btnPlcDisconnClick;
	if(btnBcrSourceConn != NULL) btnBcrSourceConn->OnClick = btnBcrSourceConnClick;
	if(btnBcrSourceDisconn != NULL) btnBcrSourceDisconn->OnClick = btnBcrSourceDisconnClick;
	if(btnBcrTargetConn != NULL) btnBcrTargetConn->OnClick = btnBcrTargetConnClick;
	if(btnBcrTargetDisconn != NULL) btnBcrTargetDisconn->OnClick = btnBcrTargetDisconnClick;
	if(btnSmokeConn != NULL) btnSmokeConn->OnClick = btnSmokeConnClick;
	if(btnSmokeDisconn != NULL) btnSmokeDisconn->OnClick = btnSmokeDisconnClick;
}
//---------------------------------------------------------------------------
void __fastcall TConfigForm::LoadCommunicationEdits()
{
	if(editPLCIpaddress != NULL) editPLCIpaddress->Text = BaseForm->config.plcIp;
	if(editPlcPort1 != NULL) editPlcPort1->Text = IntToStr(BaseForm->config.plcPortPlc);
	if(editPlcPort2 != NULL) editPlcPort2->Text = IntToStr(BaseForm->config.plcPortPc);
	if(editBcrSourceIp != NULL) editBcrSourceIp->Text = BaseForm->config.bcrIp[0];
	if(editBcrSourcePort != NULL) editBcrSourcePort->Text = IntToStr(BaseForm->config.bcrPort[0]);
	if(editBcrTargetIp != NULL) editBcrTargetIp->Text = BaseForm->config.bcrIp[1];
	if(editBcrTargetPort != NULL) editBcrTargetPort->Text = IntToStr(BaseForm->config.bcrPort[1]);
	if(editSmokePort != NULL) editSmokePort->Text = BaseForm->config.smokePort;
	if(editSmokeId != NULL) editSmokeId->Text = IntToStr(BaseForm->config.smokeId);
	if(editSmokeMode != NULL) editSmokeMode->Text = IntToStr(BaseForm->config.smokeMode);
	if(editSmokeBaud != NULL) editSmokeBaud->Text = IntToStr(BaseForm->config.smokeBaudRate);
}
//---------------------------------------------------------------------------
void __fastcall TConfigForm::UpdateCommunicationConfigFromEdits()
{
	if(editPLCIpaddress != NULL) BaseForm->config.plcIp = editPLCIpaddress->Text;
	if(editPlcPort1 != NULL) BaseForm->config.plcPortPlc = editPlcPort1->Text.ToIntDef(6002);
	if(editPlcPort2 != NULL) BaseForm->config.plcPortPc = editPlcPort2->Text.ToIntDef(6003);
	if(editBcrSourceIp != NULL) BaseForm->config.bcrIp[0] = editBcrSourceIp->Text;
	if(editBcrSourcePort != NULL) BaseForm->config.bcrPort[0] = editBcrSourcePort->Text.ToIntDef(9004);
	if(editBcrTargetIp != NULL) BaseForm->config.bcrIp[1] = editBcrTargetIp->Text;
	if(editBcrTargetPort != NULL) BaseForm->config.bcrPort[1] = editBcrTargetPort->Text.ToIntDef(9004);
	if(editSmokePort != NULL) BaseForm->config.smokePort = editSmokePort->Text;
	if(editSmokeId != NULL) BaseForm->config.smokeId = editSmokeId->Text.ToIntDef(1);
	if(editSmokeMode != NULL) BaseForm->config.smokeMode = editSmokeMode->Text.ToIntDef(0);
	if(editSmokeBaud != NULL) BaseForm->config.smokeBaudRate = editSmokeBaud->Text.ToIntDef(115200);
}
//---------------------------------------------------------------------------
void __fastcall TConfigForm::WriteSystemInfo(AnsiString type)
{
	TIniFile *ini;

	AnsiString file;
	file = (AnsiString)BIN + "MainSystemInfo.inf";

	ini = new TIniFile(file);

	if(type.IsEmpty())
		UpdateCommunicationConfigFromEdits();

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
        // Stage Info
		ini->WriteString("INFO", "PC", pcEdit->Text);
        // FMS Gateway
		ini->WriteString("COMMUNICATION", "FMS_IP", editFmsIp->Text);
		ini->WriteString("COMMUNICATION", "GATEWAY_PORT", PortEdit->Text);
        // PLC
		ini->WriteString("COMMUNICATION", "PLC_IP", BaseForm->config.plcIp);
		ini->WriteInteger("COMMUNICATION", "PLC_PORT_PLC", BaseForm->config.plcPortPlc);
		ini->WriteInteger("COMMUNICATION", "PLC_PORT_PC", BaseForm->config.plcPortPc);
		ini->WriteString("PLC", "IPADDRESS", BaseForm->config.plcIp);
		ini->WriteInteger("PLC", "PORT1", BaseForm->config.plcPortPlc);
		ini->WriteInteger("PLC", "PORT2", BaseForm->config.plcPortPc);
        ini->WriteString("COMMUNICATION", "BCR_SOURCE_IP", BaseForm->config.bcrIp[0]);
        ini->WriteInteger("COMMUNICATION", "BCR_SOURCE_PORT", BaseForm->config.bcrPort[0]);
        ini->WriteString("COMMUNICATION", "BCR_TARGET_IP", BaseForm->config.bcrIp[1]);
        ini->WriteInteger("COMMUNICATION", "BCR_TARGET_PORT", BaseForm->config.bcrPort[1]);
        ini->WriteString("COMMUNICATION", "SMOKE_PORT", BaseForm->config.smokePort);
        ini->WriteInteger("COMMUNICATION", "SMOKE_ID", BaseForm->config.smokeId);
        ini->WriteInteger("COMMUNICATION", "SMOKE_MODE", BaseForm->config.smokeMode);
        ini->WriteInteger("COMMUNICATION", "SMOKE_BAUDRATE", BaseForm->config.smokeBaudRate);
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
		LoadCommunicationEdits();
		return false;
	}
	ini = new TIniFile(file);

	if(ini->ReadString("ZAXIS", "UP", "1") == "1")
		chkZAxisUp->Checked = true;
	else
        chkZAxisUp->Checked = false;

    // Stage Info
	pcEdit->Text = ini->ReadString("INFO", "PC", "H1DIF01A");
    // FMS Gateway
	editFmsIp->Text = ini->ReadString("COMMUNICATION", "FMS_IP", "127.0.0.1");
	PortEdit->Text = ini->ReadString("COMMUNICATION", "GATEWAY_PORT",
		ini->ReadString("COMMUNICATION", "IMS_PORT", "18080"));
	BaseForm->config.fmsIp = editFmsIp->Text;
	BaseForm->config.gatewayPort = PortEdit->Text.ToIntDef(18080);
    // PLC
	BaseForm->config.plcIp = ini->ReadString("COMMUNICATION", "PLC_IP",
		ini->ReadString("PLC", "IPADDRESS", BaseForm->config.plcIp));
	BaseForm->config.plcPortPlc = ini->ReadInteger("COMMUNICATION", "PLC_PORT_PLC",
		ini->ReadInteger("PLC", "PORT1", BaseForm->config.plcPortPlc));
	BaseForm->config.plcPortPc = ini->ReadInteger("COMMUNICATION", "PLC_PORT_PC",
		ini->ReadInteger("PLC", "PORT2", BaseForm->config.plcPortPc));
    BaseForm->config.bcrIp[0] = ini->ReadString("COMMUNICATION", "BCR_SOURCE_IP",
        ini->ReadString("COMMUNICATION", "BCR_IP", BaseForm->config.bcrIp[0]));
    BaseForm->config.bcrPort[0] = ini->ReadInteger("COMMUNICATION", "BCR_SOURCE_PORT",
        ini->ReadInteger("COMMUNICATION", "BCR_PORT", BaseForm->config.bcrPort[0]));
    BaseForm->config.bcrIp[1] = ini->ReadString("COMMUNICATION", "BCR_TARGET_IP", BaseForm->config.bcrIp[1]);
    BaseForm->config.bcrPort[1] = ini->ReadInteger("COMMUNICATION", "BCR_TARGET_PORT", BaseForm->config.bcrPort[1]);
    BaseForm->config.smokePort = ini->ReadString("COMMUNICATION", "SMOKE_PORT", BaseForm->config.smokePort);
    BaseForm->config.smokeId = ini->ReadInteger("COMMUNICATION", "SMOKE_ID", BaseForm->config.smokeId);
    BaseForm->config.smokeMode = ini->ReadInteger("COMMUNICATION", "SMOKE_MODE", BaseForm->config.smokeMode);
    BaseForm->config.smokeBaudRate = ini->ReadInteger("COMMUNICATION", "SMOKE_BAUDRATE", BaseForm->config.smokeBaudRate);

    // Recipe
	editRecipe->Text = ini->ReadString("RECIPE", "NO", "1");
	AnsiString recipe = editRecipe->Text;

	//* TrayTeaching96.ini가 없거나 잘못된 경우에만 기존 Servo 속도 설정을 사용한다.
	if(!teachForm->teachingFileLoaded){
		teachForm->speedEdit->Text = ini->ReadString("SPEED", "SPEED", "500");
		teachForm->Panel_speedEdit->Caption = ini->ReadString("SPEED", "SPEED", "500");
		teachForm->acclSpeedEdit->Text = ini->ReadString("SPEED", "ACCL_SPEED", "1000");
		teachForm->dcclSpeedEdit->Text = ini->ReadString("SPEED", "DCCL_SPEED", "1000");
	}

	LoadCommunicationEdits();

	delete ini;
	return true;
}
//---------------------------------------------------------------------------
void __fastcall TConfigForm::FormShow(TObject *Sender)
{
	this->BringToFront();
}
//---------------------------------------------------------------------------
void __fastcall TConfigForm::btnConMesClick(TObject *Sender)
{
	ApplyConfig();
	if(Mod_Fms != NULL)
		Mod_Fms->Start();
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::btnDisconMesClick(TObject *Sender)
{
	if(Mod_Fms != NULL)
		Mod_Fms->Stop();
}
//---------------------------------------------------------------------------
void __fastcall TConfigForm::btnPlcConnClick(TObject *Sender)
{
	UpdateCommunicationConfigFromEdits();
	if(PlcBin != NULL)
		PlcBin->Connect(BaseForm->config.plcIp,
			BaseForm->config.plcPortPlc, BaseForm->config.plcPortPc);
}
//---------------------------------------------------------------------------
void __fastcall TConfigForm::btnPlcDisconnClick(TObject *Sender)
{
	if(PlcBin != NULL)
		PlcBin->DisConnect();
}
//---------------------------------------------------------------------------
void __fastcall TConfigForm::btnBcrSourceConnClick(TObject *Sender)
{
	UpdateCommunicationConfigFromEdits();
	if(MainForm->comBcr[0] == NULL){
		MainForm->comBcr[0] = new TMod_Bcr(MainForm);
		MainForm->comBcr[0]->Tag = 0;
	}
	MainForm->comBcr[0]->Connect(BaseForm->config.bcrIp[0], BaseForm->config.bcrPort[0]);
}
//---------------------------------------------------------------------------
void __fastcall TConfigForm::btnBcrSourceDisconnClick(TObject *Sender)
{
	if(MainForm->comBcr[0] != NULL)
		MainForm->comBcr[0]->Disconnect();
}
//---------------------------------------------------------------------------
void __fastcall TConfigForm::btnBcrTargetConnClick(TObject *Sender)
{
	UpdateCommunicationConfigFromEdits();
	if(MainForm->comBcr[1] == NULL){
		MainForm->comBcr[1] = new TMod_Bcr(MainForm);
		MainForm->comBcr[1]->Tag = 1;
	}
	MainForm->comBcr[1]->Connect(BaseForm->config.bcrIp[1], BaseForm->config.bcrPort[1]);
}
//---------------------------------------------------------------------------
void __fastcall TConfigForm::btnBcrTargetDisconnClick(TObject *Sender)
{
	if(MainForm->comBcr[1] != NULL)
		MainForm->comBcr[1]->Disconnect();
}
//---------------------------------------------------------------------------
void __fastcall TConfigForm::btnSmokeConnClick(TObject *Sender)
{
	UpdateCommunicationConfigFromEdits();
	if(MainForm->comSmoke[0] == NULL)
		MainForm->comSmoke[0] = new TSmokeDetector(MainForm);
	MainForm->comSmoke[0]->CommOpen(BaseForm->config.smokePort, 0,
		BaseForm->config.smokeId,
		BaseForm->config.smokeMode,
		BaseForm->config.smokeBaudRate);
}
//---------------------------------------------------------------------------
void __fastcall TConfigForm::btnSmokeDisconnClick(TObject *Sender)
{
	if(MainForm->comSmoke[0] != NULL)
		MainForm->comSmoke[0]->CommClose();
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::AdvSmoothButton2Click(TObject *Sender)
{
	if(MessageBox(Handle, BaseForm->GetLangStr("MSG_APPLY").c_str(), L"APPLY", MB_YESNO|MB_ICONQUESTION) == ID_YES){
		ApplyConfig();
		this->WriteSystemInfo();
		this->Visible = false;
	}
}
//---------------------------------------------------------------------------

void __fastcall TConfigForm::AdvSmoothButton3Click(TObject *Sender)
{
	if(MessageBox(Handle, BaseForm->GetLangStr("MSG_CANCEL").c_str(), L"CANCEL", MB_YESNO|MB_ICONQUESTION) == ID_YES){
		this->ReadSystemInfo();
		this->Visible = false;
	}	
}
//---------------------------------------------------------------------------
