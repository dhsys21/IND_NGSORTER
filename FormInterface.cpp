//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FormBase.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
using namespace System::Json;
TInterfaceForm *InterfaceForm;
//---------------------------------------------------------------------------
__fastcall TInterfaceForm::TInterfaceForm(TComponent* Owner)
	: TForm(Owner)
{
	Randomize();
    this->Width = 980;
    this->Height = 890;

    SetListViewPLC();
	SetListViewPC();

    SetListViewFMSTAG();
    SetListViewPCTAG();
	SetupMesTestControls();
	RefreshMesTagLists();
	RefreshPcTagCombo();
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
        ListView_PLC->Items->Item[index++]->SubItems->Strings[1] = IntToStr(PlcBin->IsPlcHeartBeatOn() ? 1 : 0);
        ListView_PLC->Items->Item[index++]->SubItems->Strings[1] = IntToStr(PlcBin->IsPlcAutoMode() ? 1 : 0);
        ListView_PLC->Items->Item[index++]->SubItems->Strings[1] = PlcBin->GetPlcValue(PLC_D_ERROR);
        ListView_PLC->Items->Item[index++]->SubItems->Strings[1] = PlcBin->GetPlcValue(PLC_D_SOURCE_TRAY_IN);
        ListView_PLC->Items->Item[index++]->SubItems->Strings[1] = PlcBin->GetPlcValue(PLC_D_SOURCE_CENTERING);
        ListView_PLC->Items->Item[index++]->SubItems->Strings[1] = PlcBin->GetPlcValue(PLC_D_TARGET_TRAY_IN);
        ListView_PLC->Items->Item[index++]->SubItems->Strings[1] = PlcBin->GetPlcValue(PLC_D_TARGET_CENTERING);
    }

    if(PlcBin->ClientSocket_PC->Active)
    {
        index = 0;
        ListView_PC->Items->Item[index++]->SubItems->Strings[1] = IntToStr(PlcBin->IsPcHeartBeatOn() ? 1 : 0);
        ListView_PC->Items->Item[index++]->SubItems->Strings[1] = IntToStr(PlcBin->IsPcAutoMode() ? 1 : 0);
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
	if (list->Columns->Count > 2)
		tempListItem->SubItems->Add("");
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::AddListView(TListView *list, UnicodeString tagname,
	UnicodeString datatype, UnicodeString value)
{
	tempListItem = list->Items->Add();
	tempListItem->Caption = tagname;
	tempListItem->SubItems->Add(datatype);
	tempListItem->SubItems->Add(value);
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::SetListViewFMSTAG()
{
	if (ListView_FMS_TAG->Columns->Count < 3)
	{
		ListView_FMS_TAG->Columns->Items[0]->Width = 260;
		ListView_FMS_TAG->Columns->Items[1]->Caption = "DataType";
		ListView_FMS_TAG->Columns->Items[1]->Width = 80;
		TListColumn *Column = ListView_FMS_TAG->Columns->Add();
		Column->Caption = "FMS_Value";
		Column->Width = 110;
	}

    ListView_FMS_TAG->Items->BeginUpdate();
    try
    {
        ListView_FMS_TAG->Items->Clear();
		AddListView(ListView_FMS_TAG, L"Waiting for FMS tags", L"", L"");
    }
    __finally
    {
        ListView_FMS_TAG->Items->EndUpdate();
    }
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::SetListViewPCTAG()
{
	if (ListView_PC_TAG->Columns->Count < 3)
	{
		ListView_PC_TAG->Columns->Items[0]->Width = 260;
		ListView_PC_TAG->Columns->Items[1]->Caption = "DataType";
		ListView_PC_TAG->Columns->Items[1]->Width = 80;
		TListColumn *Column = ListView_PC_TAG->Columns->Add();
		Column->Caption = "PC_Value";
		Column->Width = 110;
	}

    ListView_PC_TAG->Items->BeginUpdate();
    try
    {
        ListView_PC_TAG->Items->Clear();
		AddListView(ListView_PC_TAG, L"Waiting for PC tags", L"", L"");
    }
    __finally
    {
        ListView_PC_TAG->Items->EndUpdate();
    }
}
//---------------------------------------------------------------------------
UnicodeString __fastcall TInterfaceForm::GetRowPart(const UnicodeString &Row, int Index)
{
	UnicodeString Work = Row;
	for (int i = 0; i < Index; ++i)
	{
		int Pos = Work.Pos(L"\t");
		if (Pos <= 0)
			return L"";
		Work = Work.SubString(Pos + 1, Work.Length());
	}

	int Pos = Work.Pos(L"\t");
	if (Pos > 0)
		return Work.SubString(1, Pos - 1);
	return Work;
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::RefreshMesTagLists()
{
	if (Mod_Fms == NULL || !Visible)
		return;

	TStringList *FmsRows = new TStringList();
	TStringList *PcRows = new TStringList();
	try
	{
		Mod_Fms->GetTagRows(FmsRows, PcRows);
		if (FmsRows->Count == 0 && PcRows->Count == 0)
			return;

		SyncListViewRows(ListView_FMS_TAG, FmsRows);
		SyncListViewRows(ListView_PC_TAG, PcRows);
	}
	__finally
	{
		delete FmsRows;
		delete PcRows;
	}
}
//---------------------------------------------------------------------------
bool __fastcall TInterfaceForm::IsSameListLayout(TListView *list, TStrings *Rows)
{
	if (list == NULL || Rows == NULL || list->Items->Count != Rows->Count)
		return false;

	for (int i = 0; i < Rows->Count; ++i)
	{
		TListItem *Item = list->Items->Item[i];
		if (Item == NULL || Item->Caption != GetRowPart(Rows->Strings[i], 0))
			return false;
		if (Item->SubItems->Count < 1 ||
			Item->SubItems->Strings[0] != GetRowPart(Rows->Strings[i], 1))
			return false;
	}

	return true;
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::SyncListViewRows(TListView *list, TStrings *Rows)
{
	if (list == NULL || Rows == NULL)
		return;

	if (IsSameListLayout(list, Rows))
	{
		UpdateListViewValues(list, Rows);
		return;
	}

	UnicodeString SelectedTag = L"";
	if (list->Selected != NULL)
		SelectedTag = list->Selected->Caption;
	int TopIndex = list->TopItem != NULL ? list->TopItem->Index : 0;

	list->Items->BeginUpdate();
	try
	{
		list->Items->Clear();
		for (int i = 0; i < Rows->Count; ++i)
		{
			AddListView(list, GetRowPart(Rows->Strings[i], 0),
				GetRowPart(Rows->Strings[i], 1), GetRowPart(Rows->Strings[i], 2));
		}
	}
	__finally
	{
		list->Items->EndUpdate();
	}

	if (TopIndex >= 0 && TopIndex < list->Items->Count)
		list->Items->Item[TopIndex]->MakeVisible(false);

	for (int i = 0; i < list->Items->Count && !SelectedTag.IsEmpty(); ++i)
	{
		if (list->Items->Item[i]->Caption == SelectedTag)
		{
			list->Items->Item[i]->Selected = true;
			break;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::UpdateListViewValues(TListView *list, TStrings *Rows)
{
	if (list == NULL || Rows == NULL)
		return;

	for (int i = 0; i < Rows->Count && i < list->Items->Count; ++i)
	{
		TListItem *Item = list->Items->Item[i];
		UnicodeString Value = GetRowPart(Rows->Strings[i], 2);
		if (Item != NULL && Item->SubItems->Count > 1 &&
			Item->SubItems->Strings[1] != Value)
		{
			Item->SubItems->Strings[1] = Value;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::RefreshPcTagCombo()
{
	if (Mod_Fms == NULL)
		return;

	TStringList *Tags = new TStringList();
	try
	{
		Mod_Fms->GetPcTagNames(Tags);
		if (Tags->Count == 0)
			return;

		UnicodeString OldText = cbMesTag->Text;
		bool SameItems = cbMesTag->Items->Count == Tags->Count;
		for (int i = 0; i < Tags->Count && SameItems; ++i)
			SameItems = cbMesTag->Items->Strings[i] == Tags->Strings[i];
		if (SameItems)
			return;

		cbMesTag->Items->Assign(Tags);
		int Index = cbMesTag->Items->IndexOf(OldText);
		cbMesTag->ItemIndex = Index >= 0 ? Index : 0;
		UpdateMesValueEditor();
	}
	__finally
	{
		delete Tags;
	}
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::SetupMesTestControls()
{
	btnWriteMesValue->OnClick = btnWriteMesValueClick;
	cbMesTag->OnChange = cbMesTagChange;
	ListView_PC_TAG->OnClick = ListViewPCTagClick;
	btnSourceTrayLoad->OnClick = btnSourceTrayLoadClick;
	btnTargetTrayLoad->OnClick = btnTargetTrayLoadClick;
	btnWriteTargetData->OnClick = btnWriteTargetDataClick;

	Timer_MES_Update->OnTimer = Timer_MES_UpdateTimer;
	Timer_MES_Update->Interval = 1000;
	Timer_MES_Update->Enabled = true;

	lblFmsDataType = new TLabel(this);
	lblFmsDataType->Parent = gbFmsWriteValues;
	lblFmsDataType->Left = 247;
	lblFmsDataType->Top = 53;
	lblFmsDataType->Width = 70;
	lblFmsDataType->Caption = "TYPE: -";

	cbMesBoolValue = new TComboBox(this);
	cbMesBoolValue->Parent = gbFmsWriteValues;
	cbMesBoolValue->Left = editFmsValue->Left;
	cbMesBoolValue->Top = editFmsValue->Top;
	cbMesBoolValue->Width = editFmsValue->Width;
	cbMesBoolValue->Style = csDropDownList;
	cbMesBoolValue->Items->Add("true");
	cbMesBoolValue->Items->Add("false");
	cbMesBoolValue->ItemIndex = 0;
	cbMesBoolValue->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::UpdateMesValueEditor()
{
	TFmsTagDefinition Definition;
	UnicodeString DataType = L"";
	if (Mod_Fms != NULL && Mod_Fms->GetTagDefinitionInfo(cbMesTag->Text, Definition))
		DataType = Definition.DataType;

	lblFmsDataType->Caption = "TYPE: " + DataType;
	bool IsBool = DataType.UpperCase() == L"BOOLEAN";
	cbMesBoolValue->Visible = IsBool;
	editFmsValue->Visible = !IsBool;
	if (IsBool && cbMesBoolValue->ItemIndex < 0)
		cbMesBoolValue->ItemIndex = 0;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall TInterfaceForm::BuildJsonValue(
	const UnicodeString &DataType, const UnicodeString &Text)
{
	UnicodeString Type = DataType.UpperCase();
	UnicodeString Value = Text.Trim();

	if (Type == L"BOOLEAN")
		return Value.LowerCase() == L"true" ? L"true" : L"false";

	if (Type == L"STRING")
	{
		TJSONString *JsonString = new TJSONString(Value);
		UnicodeString Result = JsonString->ToString();
		delete JsonString;
		return Result;
	}

	if (Value.IsEmpty())
		return UnicodeString(L"0");
	return Value;
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::WriteMesValue()
{
	if (Mod_Fms == NULL || cbMesTag->Text.Trim().IsEmpty())
		return;

	TFmsTagDefinition Definition;
	UnicodeString DataType = L"";
	if (Mod_Fms->GetTagDefinitionInfo(cbMesTag->Text, Definition))
		DataType = Definition.DataType;

	UnicodeString ValueText =
		cbMesBoolValue->Visible ? cbMesBoolValue->Text : editFmsValue->Text;
	Mod_Fms->SetPcTagJson(cbMesTag->Text, BuildJsonValue(DataType, ValueText));
	Mod_Fms->FlushPendingPcTags();
	RefreshMesTagLists();

	if (MainForm != NULL)
	{
		MainForm->WriteOpcUaLog("TEST",
			AnsiString("PC TAG WRITE " + cbMesTag->Text + "=" + ValueText));
	}
}
//---------------------------------------------------------------------------
bool __fastcall TInterfaceForm::CanRunMesTest()
{
	if (MainForm == NULL || MesOpc == NULL || Mod_Fms == NULL)
		return false;

	if (!Mod_Fms->IsGatewayConnected())
	{
		Application->MessageBox(L"OPC UA Gateway is not connected.",
			L"NGSORTER MES TEST", MB_OK | MB_ICONWARNING);
		return false;
	}

	if (MainForm->equipMode != modeManual)
	{
		Application->MessageBox(L"MES test is available only in MANUAL mode.",
			L"NGSORTER MES TEST", MB_OK | MB_ICONWARNING);
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------
bool __fastcall TInterfaceForm::IsNgChannel(int Channel)
{
	TStringList *Parts = new TStringList();
	try
	{
		Parts->StrictDelimiter = true;
		Parts->Delimiter = ',';
		Parts->DelimitedText = editNgList->Text;
		for (int i = 0; i < Parts->Count; ++i)
		{
			if (Parts->Strings[i].Trim().ToIntDef(-1) == Channel)
				return true;
		}
	}
	__finally
	{
		delete Parts;
	}
	return false;
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::SendTrayLoadTest(bool TargetTray)
{
	if (!CanRunMesTest())
		return;

	UnicodeString TrayId = TargetTray ? editOCV->Text.Trim() : editIR->Text.Trim();
	if (TrayId.IsEmpty())
	{
		Application->MessageBox(L"Enter a tray ID.", L"NGSORTER MES TEST",
			MB_OK | MB_ICONWARNING);
		return;
	}

	TRAY_INFO *PreviousTray = MainForm->tray;
	MainForm->tray = TargetTray ? &MainForm->tray_target : &MainForm->tray_source;

	TPanel *TrayIdPanel =
		TargetTray ? MainForm->pTrayid_target : MainForm->pTrayid_source;
	if (TrayIdPanel != NULL)
		TrayIdPanel->Caption = TrayId;

	MesOpc->TRAY_LOAD_REQUEST();
	Mod_Fms->FlushPendingPcTags();
	MainForm->tray = PreviousTray;
	RefreshMesTagLists();
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::WriteTargetTrackOutTestData()
{
	if (!CanRunMesTest())
		return;

	UnicodeString TargetTrayId = editOCV->Text.Trim();
	if (TargetTrayId.IsEmpty())
	{
		Application->MessageBox(L"Enter a target tray ID.", L"NGSORTER MES TEST",
			MB_OK | MB_ICONWARNING);
		return;
	}

	TRAY_INFO *Tray = &MainForm->tray_target;
	Tray->SLOT_COUNT = 96;
	for (int i = 0; i < Tray->SLOT_COUNT; ++i)
	{
		bool IsNg = IsNgChannel(i + 1);
		Tray->SLOT_POSITION[i] = IntToStr(i + 1);
		Tray->TARGET_SLOT_POSITION[i] = IntToStr(i + 1);
		if (IsNg)
		{
			UnicodeString CellId = L"TEST_CELL_" + FormatFloat(L"000", i + 1);
			Tray->SLOT_ID[i] = AnsiString(CellId);
		}
		else
			Tray->SLOT_ID[i] = "";
		Tray->LOSS_CD[i] = IsNg ? "TEST_NG" : "";
		Tray->LOSS_DESC[i] = "";
		Tray->PICK[i] = IsNg ? "Y" : "N";
		Tray->RANK[i] = IsNg ? "NG" : "";
		Tray->SAMPLE_CODE[i] = "";
	}

	MainForm->pTrayid_target2->Caption = TargetTrayId;
	MesOpc->PROCESS_DATA_WRITE();
	Mod_Fms->FlushPendingPcTags();
	RefreshMesTagLists();

	MainForm->WriteOpcUaLog("TEST",
		AnsiString("TARGET TRACK OUT WRITE TrayId=" + TargetTrayId +
			" NG=" + editNgList->Text));
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::Timer_MES_UpdateTimer(TObject *Sender)
{
	RefreshMesTagLists();
	RefreshPcTagCombo();
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::btnWriteMesValueClick(TObject *Sender)
{
	WriteMesValue();
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::cbMesTagChange(TObject *Sender)
{
	UpdateMesValueEditor();
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::ListViewPCTagClick(TObject *Sender)
{
	if (ListView_PC_TAG == NULL || ListView_PC_TAG->Selected == NULL)
		return;

	UnicodeString Tag = ListView_PC_TAG->Selected->Caption;
	int Index = cbMesTag->Items->IndexOf(Tag);
	if (Index < 0)
	{
		RefreshPcTagCombo();
		Index = cbMesTag->Items->IndexOf(Tag);
	}

	if (Index >= 0)
		cbMesTag->ItemIndex = Index;
	else
		cbMesTag->Text = Tag;

	if (ListView_PC_TAG->Selected->SubItems->Count > 1)
		editFmsValue->Text = ListView_PC_TAG->Selected->SubItems->Strings[1];
	UpdateMesValueEditor();
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::btnSourceTrayLoadClick(TObject *Sender)
{
	SendTrayLoadTest(false);
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::btnTargetTrayLoadClick(TObject *Sender)
{
	SendTrayLoadTest(true);
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::btnWriteTargetDataClick(TObject *Sender)
{
	WriteTargetTrackOutTestData();
}
//---------------------------------------------------------------------------
