//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TInterfaceForm *InterfaceForm;
//---------------------------------------------------------------------------
__fastcall TInterfaceForm::TInterfaceForm(TComponent* Owner)
	: TForm(Owner)
{
    this->Width = 980;
    this->Height = 890;

    SetListViewPLC();
	SetListViewPC();

    SetListViewFMSTAG();
    SetListViewPCTAG();
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::GroupBox_PLC_PCDblClick(TObject *Sender)
{
    if(this->Width < 1000) this->Width = 1380;
    else this->Width = 980;
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::AddListView(TListView *list, AnsiString address, AnsiString name)
{
	tempListItem = list->Items->Add();
	tempListItem->Caption = address;
	tempListItem->SubItems->Add(name);
	tempListItem->SubItems->Add("");
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// PLC ADDRESS LIST
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::SetListViewPLC()
{
    ListView_PLC->Items->BeginUpdate();
    try
    {
        ListView_PLC->Items->Clear();

        AddListView(ListView_PLC, "D" + IntToStr(PLC_D_INTERFACE_START_DEV_NUM + PLC_D_HEART_BEAT), "PLC HEART BEAT");
        AddListView(ListView_PLC, "D" + IntToStr(PLC_D_INTERFACE_START_DEV_NUM + PLC_D_AUTO_MANUAL), "PLC AUTO MANUAL");
        AddListView(ListView_PLC, "D" + IntToStr(PLC_D_INTERFACE_START_DEV_NUM + PLC_D_ERROR), "PLC ERROR");
        AddListView(ListView_PLC, "D" + IntToStr(PLC_D_INTERFACE_START_DEV_NUM + PLC_D_TRAY_IN), "TRAY IN");
        AddListView(ListView_PLC, "D" + IntToStr(PLC_D_INTERFACE_START_DEV_NUM + PLC_D_MAINTENANCE_ON), "MAINTENANCE ON");
        AddListView(ListView_PLC, "D" + IntToStr(PLC_D_INTERFACE_START_DEV_NUM + PLC_D_MAINTENANCE_TRAY_OUT), "MAINTENANCE TRAY OUT");
    }
    __finally
    {
        ListView_PLC->Items->EndUpdate();
    }

    // 보통 EndUpdate로 충분하지만, 즉시 화면 반영이 필요하면:
    ListView_PLC->Invalidate();
    ListView_PLC->Update();
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::SetListViewPC()
{
    ListView_PC->Items->BeginUpdate();
    try
    {
        ListView_PC->Items->Clear();

        AddListView(ListView_PC, "D" + IntToStr(PC_D_INTERFACE_START_DEV_NUM + PC_D_HEART_BEAT), "PC HEART BEAT");
        AddListView(ListView_PC, "D" + IntToStr(PC_D_INTERFACE_START_DEV_NUM + PC_D_AUTO_MANUAL), "PC AUTO MANUAL");
        AddListView(ListView_PC, "D" + IntToStr(PC_D_INTERFACE_START_DEV_NUM + PC_D_ERROR), "PC ERROR");
        AddListView(ListView_PC, "D" + IntToStr(PC_D_INTERFACE_START_DEV_NUM + PC_D_MEASURECOMPLETE), "MEASURING COMPLETE");
        AddListView(ListView_PC, "D" + IntToStr(PC_D_INTERFACE_START_DEV_NUM + PC_D_TRAY_OUT), "TRAY OUT");
        AddListView(ListView_PC, "D" + IntToStr(PC_D_INTERFACE_START_DEV_NUM + PC_D_MAINTENANCE_IN), "MAINTENANCE IN");
        AddListView(ListView_PC, "D" + IntToStr(PC_D_INTERFACE_START_DEV_NUM + PC_D_MAINTENANCE_OUT), "MAINTENANCE OUT");
        AddListView(ListView_PC, "D" + IntToStr(PC_D_INTERFACE_START_DEV_NUM + PC_D_PROB_OPEN), "PROB OPEN");
        AddListView(ListView_PC, "D" + IntToStr(PC_D_INTERFACE_START_DEV_NUM + PC_D_PROB_CLOSE), "PROB CLOSE");
    }
    __finally
    {
        ListView_PC->Items->EndUpdate();
    }

    // 보통 EndUpdate로 충분하지만, 즉시 화면 반영이 필요하면:
    ListView_PC->Invalidate();
    ListView_PC->Update();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// PLC TEST
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::btnPlcWriteValueClick(TObject *Sender)
{
    WritePcValue();
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::WritePcValue()
{
    int address = cbAddress->Text.ToIntDef(10052) - PC_D_INTERFACE_START_DEV_NUM;
    int value = editPcValue->Text.ToIntDef(1);
    PlcBin->SetPcValue(address, value);
}

//---------------------------------------------------------------------------
// FMS TAG LIST
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::AddListView(TListView *list, AnsiString tagname)
{
	tempListItem = list->Items->Add();
	tempListItem->Caption = tagname;
	tempListItem->SubItems->Add("");
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::SetListViewFMSTAG()
{
    ListView_FMS_TAG->Items->BeginUpdate();
    try
    {
        ListView_FMS_TAG->Items->Clear();

        AddListView(ListView_FMS_TAG, "Alive");

        AddListView(ListView_FMS_TAG, "FMS Status");
        AddListView(ListView_FMS_TAG, "  Status");
        AddListView(ListView_FMS_TAG, "  Error No");

        AddListView(ListView_FMS_TAG, "Equipment Control");
        AddListView(ListView_FMS_TAG, "  Command");

        AddListView(ListView_FMS_TAG, "Tray Information");
        AddListView(ListView_FMS_TAG, "  Tray Load Response");
        AddListView(ListView_FMS_TAG, "  Product Model");
        AddListView(ListView_FMS_TAG, "  Route Id");
        AddListView(ListView_FMS_TAG, "  Process Id");
        AddListView(ListView_FMS_TAG, "  Lot Id");

        AddListView(ListView_FMS_TAG, "Track In Cell Information");
        AddListView(ListView_FMS_TAG, "  Cell Count");
        AddListView(ListView_FMS_TAG, "  Cell");
        AddListView(ListView_FMS_TAG, "    Cell ID");
        AddListView(ListView_FMS_TAG, "    Cell No");
        AddListView(ListView_FMS_TAG, "    Lot Id");
        AddListView(ListView_FMS_TAG, "    Cell Exist");
        AddListView(ListView_FMS_TAG, "    NG Code");
        AddListView(ListView_FMS_TAG, "    Grade");
        AddListView(ListView_FMS_TAG, "    Work Flag");

        AddListView(ListView_FMS_TAG, "Tray Process");
        AddListView(ListView_FMS_TAG, "  Process Start Response");
        AddListView(ListView_FMS_TAG, "  Process End Response");
        AddListView(ListView_FMS_TAG, "  Request Recipe Response");

        AddListView(ListView_FMS_TAG, "Recipe");
        AddListView(ListView_FMS_TAG, "  Recipe Id");
        AddListView(ListView_FMS_TAG, "  SV");
        AddListView(ListView_FMS_TAG, "    Max OCV");
        AddListView(ListView_FMS_TAG, "    Min OCV");
        AddListView(ListView_FMS_TAG, "    Max IR");
        AddListView(ListView_FMS_TAG, "    Min IR");

    }
    __finally
    {
        ListView_FMS_TAG->Items->EndUpdate();
    }

    // 보통 EndUpdate로 충분하지만, 즉시 화면 반영이 필요하면:
    ListView_FMS_TAG->Invalidate();
    ListView_FMS_TAG->Update();
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::SetListViewPCTAG()
{
    ListView_PC_TAG->Items->BeginUpdate();
    try
    {
        ListView_PC_TAG->Items->Clear();

        AddListView(ListView_PC_TAG, "Alive");
        AddListView(ListView_PC_TAG, "Equipment Status");
        AddListView(ListView_PC_TAG, "  Power");
        AddListView(ListView_PC_TAG, "  Mode");
        AddListView(ListView_PC_TAG, "  Status");
        AddListView(ListView_PC_TAG, "  Error No");
        AddListView(ListView_PC_TAG, "  Error Level");

        AddListView(ListView_PC_TAG, "Equipment Control");
        AddListView(ListView_PC_TAG, "  Command Response");

        AddListView(ListView_PC_TAG, "Tray Information");
        AddListView(ListView_PC_TAG, "  Tray Exist");
        AddListView(ListView_PC_TAG, "  Tray Id");
        AddListView(ListView_PC_TAG, "  Tray Load");

        AddListView(ListView_PC_TAG, "TrackOut Cell Information");
        AddListView(ListView_PC_TAG, "  Cell Count");
        AddListView(ListView_PC_TAG, "  Cell");
        AddListView(ListView_PC_TAG, "    Cell Id");
        AddListView(ListView_PC_TAG, "    Cell No");
        AddListView(ListView_PC_TAG, "    Lot Id");
        AddListView(ListView_PC_TAG, "    Cell Exist");
        AddListView(ListView_PC_TAG, "    NG Code");
        AddListView(ListView_PC_TAG, "    Grade");
        AddListView(ListView_PC_TAG, "    WorkFlag");
        AddListView(ListView_PC_TAG, "    OCV");
        AddListView(ListView_PC_TAG, "    IR");

        AddListView(ListView_PC_TAG, "Tray Process");
        AddListView(ListView_PC_TAG, "  Process Start");
        AddListView(ListView_PC_TAG, "  Process End");
        AddListView(ListView_PC_TAG, "  Request Recipe");
    }
    __finally
    {
        ListView_PC_TAG->Items->EndUpdate();
    }

    // 보통 EndUpdate로 충분하지만, 즉시 화면 반영이 필요하면:
    ListView_PC_TAG->Invalidate();
    ListView_PC_TAG->Update();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// FMS TEST
//---------------------------------------------------------------------------

