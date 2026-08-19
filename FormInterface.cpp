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
        AddListView(ListView_PC, "D" + IntToStr(PC_D_INTERFACE_START_DEV_NUM + PC_D_TRAY_IN_READY), "TRAY IN READY (SERVO HOME)");
        AddListView(ListView_PC, "D" + IntToStr(PC_D_INTERFACE_START_DEV_NUM + PC_D_SOURCE_CENTERING_REQ), "SOURCE CENTERING REQ");
        AddListView(ListView_PC, "D" + IntToStr(PC_D_INTERFACE_START_DEV_NUM + PC_D_SOURCE_TRAY_OUT), "SOURCE TRAY OUT");
        AddListView(ListView_PC, "D" + IntToStr(PC_D_INTERFACE_START_DEV_NUM + PC_D_TARGET_TRAY_OUT), "TARGET TRAY OUT");
        AddListView(ListView_PC, "D" + IntToStr(PC_D_INTERFACE_START_DEV_NUM + PC_D_EMERGENCY), "PC EMERGENCY");
        AddListView(ListView_PC, "D" + IntToStr(PC_D_INTERFACE_START_DEV_NUM + PC_D_DOOR_OPEN), "PC DOOR OPEN");
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
        ListView_PC->Items->Item[index++]->SubItems->Strings[1] = PlcBin->GetPcValue(PC_D_TRAY_IN_READY);
        ListView_PC->Items->Item[index++]->SubItems->Strings[1] = PlcBin->GetPcValue(PC_D_SOURCE_CENTERING_REQ);
        ListView_PC->Items->Item[index++]->SubItems->Strings[1] = PlcBin->GetPcValue(PC_D_SOURCE_TRAY_OUT);
        ListView_PC->Items->Item[index++]->SubItems->Strings[1] = PlcBin->GetPcValue(PC_D_TARGET_TRAY_OUT);
        ListView_PC->Items->Item[index++]->SubItems->Strings[1] = PlcBin->GetPcValue(PC_D_EMERGENCY);
        ListView_PC->Items->Item[index++]->SubItems->Strings[1] = PlcBin->GetPcValue(PC_D_DOOR_OPEN);
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
    int address = cbAddress->Text.ToIntDef(10152) - PC_D_INTERFACE_START_DEV_NUM;
    int value = editPcValue->Text.ToIntDef(1);
    if(address >= 0 && address < PC_D_INTERFACE_LEN)
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
UnicodeString __fastcall TInterfaceForm::FindTrackInCellId(int SourceCellNo)
{
	if (MesOpc == NULL)
		return L"";

	AnsiString CellId;
	AnsiString LotId;
	AnsiString NGCode;
	AnsiString Grade;
	if(!MesOpc->READ_TRACK_IN_CELL(SourceCellNo, CellId, LotId, NGCode, Grade))
		return L"";

	return UnicodeString(CellId).Trim();
}
//---------------------------------------------------------------------------
bool __fastcall TInterfaceForm::IsTrackOutTestChannel(int Channel)
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
void __fastcall TInterfaceForm::WriteTrackOutCellInformationTest()
{
	if (!CanRunMesTest())
		return;

	int SelectedCount = 0;
	for (int Channel = 1; Channel <= 96; ++Channel)
	{
		if (IsTrackOutTestChannel(Channel))
			++SelectedCount;
	}
	if (SelectedCount == 0)
	{
		Application->MessageBox(L"Enter at least one TrackIn channel.",
			L"NGSORTER MES TEST", MB_OK | MB_ICONWARNING);
		return;
	}

	const UnicodeString TrackInRoot = L"F1NGS01.Location1.TrackInCellInformation";
	const UnicodeString TrackOutRoot = L"F1NGS01.Location2.TrackOutCellInformation";
	int TrackInCount = Mod_Fms->GetFmsTagInt(TrackInRoot + L".CellCount", 0);
	if (TrackInCount <= 0 || TrackInCount > 96)
	{
		Application->MessageBox(L"TrackInCellInformation.CellCount is invalid.",
			L"NGSORTER MES TEST", MB_OK | MB_ICONWARNING);
		return;
	}

	int OutputIndex = 0;
	for (int Channel = 1; Channel <= 96; ++Channel)
	{
		if (!IsTrackOutTestChannel(Channel))
			continue;

		int TrackInIndex = -1;
		for (int i = 0; i < TrackInCount; ++i)
		{
			UnicodeString InputCell = TrackInRoot + L".Cell." + IntToStr(i);
			if (Mod_Fms->GetFmsTagInt(InputCell + L".CellNo", 0) == Channel)
			{
				TrackInIndex = i;
				break;
			}
		}

		if (TrackInIndex < 0)
		{
			Application->MessageBox(
				(L"Channel " + IntToStr(Channel) +
				 L" was not found in TrackInCellInformation.").c_str(),
				L"NGSORTER MES TEST", MB_OK | MB_ICONWARNING);
			return;
		}

		UnicodeString InputCell = TrackInRoot + L".Cell." + IntToStr(TrackInIndex);
		UnicodeString OutputCell = TrackOutRoot + L".Cell." + IntToStr(OutputIndex++);
		Mod_Fms->SetPcTag(OutputCell + L".CellId",
			Mod_Fms->GetFmsTagString(InputCell + L".CellId", L""));
		Mod_Fms->SetPcTag(OutputCell + L".CellNo",
			Mod_Fms->GetFmsTagInt(InputCell + L".CellNo", 0));
		Mod_Fms->SetPcTag(OutputCell + L".LotId",
			Mod_Fms->GetFmsTagString(InputCell + L".LotId", L""));
		Mod_Fms->SetPcTag(OutputCell + L".CellExist",
			Mod_Fms->GetFmsTagBool(InputCell + L".CellExist", false));
		Mod_Fms->SetPcTag(OutputCell + L".NGCode",
			Mod_Fms->GetFmsTagString(InputCell + L".NGCode", L""));
		Mod_Fms->SetPcTag(OutputCell + L".Grade",
			Mod_Fms->GetFmsTagString(InputCell + L".Grade", L""));
		Mod_Fms->SetPcTag(OutputCell + L".WorkFlag",
			Mod_Fms->GetFmsTagBool(InputCell + L".WorkFlag", false));
	}

	for (int i = OutputIndex; i < 96; ++i)
	{
		UnicodeString OutputCell = TrackOutRoot + L".Cell." + IntToStr(i);
		Mod_Fms->SetPcTag(OutputCell + L".CellId", L"");
		Mod_Fms->SetPcTag(OutputCell + L".CellNo", 0);
		Mod_Fms->SetPcTag(OutputCell + L".LotId", L"");
		Mod_Fms->SetPcTag(OutputCell + L".CellExist", false);
		Mod_Fms->SetPcTag(OutputCell + L".NGCode", L"");
		Mod_Fms->SetPcTag(OutputCell + L".Grade", L"");
		Mod_Fms->SetPcTag(OutputCell + L".WorkFlag", false);
	}

	Mod_Fms->SetPcTag(TrackOutRoot + L".CellCount", OutputIndex);
	Mod_Fms->FlushPendingPcTags();
	RefreshMesTagLists();

	MainForm->WriteOpcUaLog("TEST",
		AnsiString("TRACK OUT CELL INFORMATION TEST Count=" + IntToStr(OutputIndex) +
			" TrackInChannels=" + editNgList->Text));
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::WriteCellTrackOutTest()
{
	if (!CanRunMesTest())
		return;

	int SourceCellNo = editIR->Text.Trim().ToIntDef(0);
	int TargetCellNo = editOCV->Text.Trim().ToIntDef(0);
	if (SourceCellNo < 1 || SourceCellNo > 96 ||
		TargetCellNo < 1 || TargetCellNo > 96)
	{
		Application->MessageBox(L"Cell No From/To must be between 1 and 96.",
			L"NGSORTER MES TEST", MB_OK | MB_ICONWARNING);
		return;
	}

	// CellTrackOut TEST mapping:
	// TrayIdFrom = Location1.TrayInformation.TrayId
	// TrayIdTo   = Location2.TrayInformation.TrayId
	// CellId     = Location1.TrackInCellInformation cell matching CellNoFrom
	UnicodeString SourceTrayId = Mod_Fms->GetPcTagString(
		L"F1NGS01.Location1.TrayInformation.TrayId", L"").Trim();
	UnicodeString TargetTrayId = Mod_Fms->GetPcTagString(
		L"F1NGS01.Location2.TrayInformation.TrayId", L"").Trim();
	if (SourceTrayId.IsEmpty() || TargetTrayId.IsEmpty())
	{
		Application->MessageBox(
			L"Location1/Location2 TrayInformation.TrayId is empty.",
			L"NGSORTER MES TEST", MB_OK | MB_ICONWARNING);
		return;
	}

	UnicodeString CellId = FindTrackInCellId(SourceCellNo);
	if (CellId.IsEmpty())
	{
		Application->MessageBox(
			L"CellId was not found in Location1 TrackInCellInformation for Cell No From.",
			L"NGSORTER MES TEST", MB_OK | MB_ICONWARNING);
		return;
	}

	MesOpc->CELL_TRACK_OUT_REQUEST(SourceCellNo, TargetCellNo, AnsiString(CellId),
		SourceTrayId, TargetTrayId);
	RefreshMesTagLists();
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
	WriteTrackOutCellInformationTest();
}
//---------------------------------------------------------------------------
void __fastcall TInterfaceForm::btnTargetTrayLoadClick(TObject *Sender)
{
	WriteCellTrackOutTest();
}
//---------------------------------------------------------------------------