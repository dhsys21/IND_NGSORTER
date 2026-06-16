//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
#pragma link "wininet.lib"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TBaseForm *BaseForm;
//---------------------------------------------------------------------------
__fastcall TBaseForm::TBaseForm(TComponent* Owner)
	: TForm(Owner)
{
	DeleteDay = 90;
	DeleteIndex = 0;

    GetWindowThreadProcessId(this->Handle, &PID);
	hProcess = OpenProcess(PROCESS_SET_QUOTA, FALSE, PID);
}
//---------------------------------------------------------------------------

void __fastcall TBaseForm::FormShow(TObject *Sender)
{
	int nx = 1000;
	int ny = 5;
	for(int row = 0; row < 2; row++) {
		for(int col = 0; col < 16; col++) {
			pRead[row][col] = new TPanel(Owner);
			pRead[row][col]->Parent = MainForm->pnlPLCTEST;
            pRead[row][col]->ParentBackground = false;
			//pRead[row][col]->Parent = this;
			pRead[row][col]->Width = 50;
			pRead[row][col]->Height = 25;
			pRead[row][col]->Color = clSilver;
			pRead[row][col]->Top = ny;
			pRead[row][col]->Left = nx;
			pRead[row][col]->Caption = col;
			pRead[row][col]->Visible = true;

			nx = nx - 50 - 2;

		}
		ny += 30;
		nx = 1000;
	}
	pRead[0][0]->Caption = "자동";
	pRead[0][1]->Caption = "비상";
	pRead[0][2]->Caption = "선별도착";
	pRead[0][3]->Caption = "선별센터";
	pRead[0][4]->Caption = "선별배출";
	pRead[0][5]->Caption = "대센터1";
	pRead[0][6]->Caption = "대배출1";

	pRead[1][1]->Caption = "작업가능";
	pRead[1][2]->Caption = "선별작업";
	pRead[1][3]->Caption = "선별배출";
	pRead[1][5]->Caption = "대배출1";


	if(config.file_exists == false){		// test
		ConfigForm->Visible = true;
	}

	loadfactorForm->LoadInfo();

	this->Left = 0;//Screen->Width - this->Width;
	this->Top = 0;

	RadioButton2->Checked = true;
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::setColor(TAdvSmoothPanel *pnl, bool bon)
{
	if(bon){
		pnl->Fill->Color = pon->Color;
		pnl->Fill->ColorMirror = pon->Color;
		pnl->Fill->ColorMirrorTo = pon->Color;
		pnl->Fill->ColorTo = pon->Color;
	}else{
		pnl->Fill->Color = poff->Color;
		pnl->Fill->ColorMirror = poff->Color;
		pnl->Fill->ColorMirrorTo = poff->Color;
		pnl->Fill->ColorTo = poff->Color;
	}

}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::ClockTimerTimer(TObject *Sender)
{
	TDateTime dt;
	dt = Now();

	setColor(pims, mes->bConnect);
	setColor(pplc, plc->client->Active);
	if(MainForm->path  == 81)setColor(pcclink, true);
	else setColor(pcclink, false);

	setColor(pbcr1, MainForm->comBcr[0]->Comm->Connected);
	setColor(pbcr2, MainForm->comBcr[1]->Comm->Connected);

	if(dt.FormatString("hhnn") == "0700") {
		DeleteDay = 90;	// 12시가 되면 DeleteDay를 90으로 초기화
	}
}
//---------------------------------------------------------------------------
bool __fastcall TBaseForm::DeleteLogFile(AnsiString FileName)
{
	if(FileExists(FileName))
	{
		DeleteFile(FileName);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
bool __fastcall TBaseForm::DeleteLogFolder(AnsiString FolderName)
{
	if(DirectoryExists(FolderName)) {
		SHFILEOPSTRUCTA FileOp;

		FileOp.hwnd = this->WindowHandle;
		FileOp.wFunc = FO_DELETE;
		FileOp.pFrom = FolderName.c_str();
		FileOp.pTo = NULL;
		FileOp.fFlags = FOF_NOCONFIRMATION |FOF_NOERRORUI;
		FileOp.fAnyOperationsAborted = FALSE;
		FileOp.lpszProgressTitle = "Delete Logging";
		SHFileOperationA(&FileOp);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------

void __fastcall TBaseForm::FileDeleteTimerTimer(TObject *Sender)
{
	SetProcessWorkingSetSize( hProcess, -1, -1 );

	TDateTime  dt;

	// 1시간에 파일 하나씩 삭제

	dt = Now() - DeleteDay;
	switch(DeleteIndex){
	case 0:   // IMS 로그
		DeleteLogFolder((AnsiString)SOCK_LOG + dt.FormatString("yymmdd"));
		DeleteIndex +=3;
		break;
	case 1:   // COMM 로그
//		DeleteLogFolder((AnsiString)COMM_LOG + dt.FormatString("yymmdd"));
		DeleteIndex +=1;
		break;
	case 2:
//		DeleteLogFolder((AnsiString)REM_PATH + dt.FormatString("yyyy_mm_dd"));
		DeleteIndex += 1;
		break;
	case 3:
		DeleteLogFile((AnsiString)ERROR_LOG + dt.FormatString("yymmdd") + ".csv");
		DeleteIndex += 1;
		break;
	case 4:    // 검사결과 파일
		DeleteLogFile((AnsiString)PROG_LOG + dt.FormatString("yymmdd") + ".csv");
		DeleteIndex = 0;
		DeleteDay += 1;
		break;
	}
	if(DeleteDay > 300)DeleteDay = 90;
}
//---------------------------------------------------------------------------
void __fastcall TBaseForm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
	if(MessageBox(Handle, L"프로그램을 종료 하시겠습니까?", L"프로그램 종료", MB_YESNO|MB_ICONQUESTION) == ID_YES){
		CanClose = true;
	}else{
		CanClose = false;
	}
}
//---------------------------------------------------------------------------


void __fastcall TBaseForm::Button1Click(TObject *Sender)
{
	if(ConfigForm->Visible == true)ConfigForm->Visible = false;
	ConfigForm->FormStyle = fsStayOnTop;
	ConfigForm->Visible = true;
}
//---------------------------------------------------------------------------



void __fastcall TBaseForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	MainForm->Close();
}
//---------------------------------------------------------------------------




void __fastcall TBaseForm::AdvSmoothButton2Click(TObject *Sender)
{
	Application->Minimize();
}
//---------------------------------------------------------------------------

void __fastcall TBaseForm::AdvSmoothButton1Click(TObject *Sender)
{
    this->Close();
}
//---------------------------------------------------------------------------

void __fastcall TBaseForm::RadioButton1Click(TObject *Sender)
{
   	TRadioButton *rbt = (TRadioButton *)Sender;
	if(rbt->Checked)
	{
        if(rbt->Tag == 0) MainForm->LanguageChange("KO");
        else if(rbt->Tag == 1) MainForm->LanguageChange("EN");

//		teachForm->LanguageChange(rbt->Tag);
//		ErrorForm_insert->LanguageChange(rbt->Tag);
//		ErrorForm_eject->LanguageChange(rbt->Tag);
//		ServoAlarmListForm->LanguageChange(rbt->Tag);
//		loadfactorForm->LanguageChange(rbt->Tag);
	}
}
//---------------------------------------------------------------------------

void __fastcall TBaseForm::AdvSmoothButton3Click(TObject *Sender)
{
	ServoAlarmListForm->Left = AdvSmoothButton3->Left;
	ServoAlarmListForm->Top = AdvSmoothButton3->Top + 80;
	ServoAlarmListForm->BringToFront();
	ServoAlarmListForm->Show();
}
//---------------------------------------------------------------------------


void __fastcall TBaseForm::pplcClick(TObject *Sender)
{
    MainForm->pnlPLCTEST->Visible = !MainForm->pnlPLCTEST->Visible;
}
//---------------------------------------------------------------------------

void __fastcall TBaseForm::btnKeyLockClick(TObject *Sender)
{
	if(MainForm->equipMode != modeManual)
	{
        ShowMessage("[C_Maint] 수동 모드가 아닙니다. 수동 모드로 변경하고 키락을 해제 해주세요.");
	}
	else{
		robostar->KeyLock(1);
	}
}
//---------------------------------------------------------------------------

int __fastcall TBaseForm::StringToInt(UnicodeString str, int def)
{
	int iVal;
	if(TryStrToInt(str, iVal) == true) return iVal;
	else return def;
}
//---------------------------------------------------------------------------
double __fastcall TBaseForm::StringToDouble(UnicodeString str, double def)
{
	double dVal;
	if(TryStrToFloat(str, dVal) == true) return dVal;
	else return def;
}
//---------------------------------------------------------------------------


void __fastcall TBaseForm::btnKeyUnLockClick(TObject *Sender)
{
    if(MainForm->equipMode != modeManual)
	{
        ShowMessage("[C_Maint] 수동 모드가 아닙니다. 수동 모드로 변경하고 키락을 해제 해주세요.");
	}
	else{
		robostar->KeyLock(2);
	}
}
//---------------------------------------------------------------------------

