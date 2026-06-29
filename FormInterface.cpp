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
        AddListView(ListView_PLC, "D" + IntToStr(PLC_D_INTERFACE_START_DEV_NUM + PLC_D_SOURCE_TRAY_IN), "SOURCE TRAY IN");
        AddListView(ListView_PLC, "D" + IntToStr(PLC_D_INTERFACE_START_DEV_NUM + PLC_D_SOURCE_CENTERING), "SOURCE CENTERING");
        AddListView(ListView_PLC, "D" + IntToStr(PLC_D_INTERFACE_START_DEV_NUM + PLC_D_TARGET_TRAY_IN), "TARGET TRAY IN");
        AddListView(ListView_PLC, "D" + IntToStr(PLC_D_INTERFACE_START_DEV_NUM + PLC_D_TARGET_CENTERING), "TARGET CENTERING");
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
        AddListView(ListView_PC, "D" + IntToStr(PC_D_INTERFACE_START_DEV_NUM + PC_D_SOURCE_CENTERING_REQ), "SOURCE CENTERING REQ");
        AddListView(ListView_PC, "D" + IntToStr(PC_D_INTERFACE_START_DEV_NUM + PC_D_TARGET_CENTERING_REQ), "TARGET CENTERING REQ");
        AddListView(ListView_PC, "D" + IntToStr(PC_D_INTERFACE_START_DEV_NUM + PC_D_SOURCE_TRAY_OUT), "SOURCE TRAY OUT");
        AddListView(ListView_PC, "D" + IntToStr(PC_D_INTERFACE_START_DEV_NUM + PC_D_TARGET_TRAY_OUT), "TARGET TRAY OUT");
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
// PLC Update
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::Timer_PLC_UpdateTimer(TObject *Sender)
{
    int index;
    if(PlcBin->ClientSocket_PLC->Active)
    {
        index = 0;
        ListView_PLC->Items->Item[index++]->SubItems->Strings[1] = PlcBin->GetPlcValue(PLC_D_HEART_BEAT);
        ListView_PLC->Items->Item[index++]->SubItems->Strings[1] = PlcBin->GetPlcValue(PLC_D_AUTO_MANUAL);
        ListView_PLC->Items->Item[index++]->SubItems->Strings[1] = PlcBin->GetPlcValue(PLC_D_ERROR);
        ListView_PLC->Items->Item[index++]->SubItems->Strings[1] = PlcBin->GetPlcValue(PLC_D_SOURCE_TRAY_IN);
        ListView_PLC->Items->Item[index++]->SubItems->Strings[1] = PlcBin->GetPlcValue(PLC_D_SOURCE_CENTERING);
        ListView_PLC->Items->Item[index++]->SubItems->Strings[1] = PlcBin->GetPlcValue(PLC_D_TARGET_TRAY_IN);
        ListView_PLC->Items->Item[index++]->SubItems->Strings[1] = PlcBin->GetPlcValue(PLC_D_TARGET_CENTERING);
    }

    if(PlcBin->ClientSocket_PC->Active)
    {
        index = 0;
        ListView_PC->Items->Item[index++]->SubItems->Strings[1] = PlcBin->GetPcValue(PC_D_HEART_BEAT);
        ListView_PC->Items->Item[index++]->SubItems->Strings[1] = PlcBin->GetPcValue(PC_D_AUTO_MANUAL);
        ListView_PC->Items->Item[index++]->SubItems->Strings[1] = PlcBin->GetPcValue(PC_D_ERROR);
        ListView_PC->Items->Item[index++]->SubItems->Strings[1] = PlcBin->GetPcValue(PC_D_SOURCE_CENTERING_REQ);
        ListView_PC->Items->Item[index++]->SubItems->Strings[1] = PlcBin->GetPcValue(PC_D_TARGET_CENTERING_REQ);
        ListView_PC->Items->Item[index++]->SubItems->Strings[1] = PlcBin->GetPcValue(PC_D_SOURCE_TRAY_OUT);
        ListView_PC->Items->Item[index++]->SubItems->Strings[1] = PlcBin->GetPcValue(PC_D_TARGET_TRAY_OUT);
    }
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

        AddListView(ListView_FMS_TAG, "Cell Track Out");
        AddListView(ListView_FMS_TAG, "  Cell Unload Complete Res");

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

        AddListView(ListView_PC_TAG, "=== SOURCE TRAY ===");
        AddListView(ListView_PC_TAG, "Tray Information");
        AddListView(ListView_PC_TAG, "  Tray Exist");
        AddListView(ListView_PC_TAG, "  Tray Id");
        AddListView(ListView_PC_TAG, "  Tray Load");

        AddListView(ListView_PC_TAG, "Tray Process");
        AddListView(ListView_PC_TAG, "  Process Start");
        AddListView(ListView_PC_TAG, "  Process End");

        AddListView(ListView_PC_TAG, "=== TARGET TRAY ===");
        AddListView(ListView_PC_TAG, "Tray Information");
        AddListView(ListView_PC_TAG, "  Tray Exist");
        AddListView(ListView_PC_TAG, "  Tray Id");
        AddListView(ListView_PC_TAG, "  Tray Load");

        AddListView(ListView_PC_TAG, "Track Out Cell Info");
        AddListView(ListView_PC_TAG, "  Cell Count");
        AddListView(ListView_PC_TAG, "  Cell");
        AddListView(ListView_PC_TAG, "    Cell ID");
        AddListView(ListView_PC_TAG, "    Cell No");
        AddListView(ListView_PC_TAG, "    Lot Id");
        AddListView(ListView_PC_TAG, "    Cell Exist");
        AddListView(ListView_PC_TAG, "    NG Code");
        AddListView(ListView_PC_TAG, "    Grade");
        AddListView(ListView_PC_TAG, "    Work Flag");

        AddListView(ListView_PC_TAG, "Tray Process");
        AddListView(ListView_PC_TAG, "  Process Start");
        AddListView(ListView_PC_TAG, "  Process End");

        AddListView(ListView_PC_TAG, "Cell Track Out");
        AddListView(ListView_PC_TAG, "  Cell No. From");
        AddListView(ListView_PC_TAG, "  Tray Id From");
        AddListView(ListView_PC_TAG, "  Cell No. To");
        AddListView(ListView_PC_TAG, "  Tray Id To");
        AddListView(ListView_PC_TAG, "  Cell Id");
        AddListView(ListView_PC_TAG, "  Cell Unload Complete");
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
// FMS Update
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// FMS TEST
//---------------------------------------------------------------------------



