//---------------------------------------------------------------------------

#pragma hdrstop

#include "ModMes_OPCUA.h"
#include "FormMain.h"
#include "ModMes_Gateway.h"

#include <SysUtils.hpp>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma classgroup "Vcl.Controls.TControl"
#pragma resource "*.dfm"
TMesOpc *MesOpc;

static const UnicodeString TAG_SOURCE = L"F1NGS01.Location1";
static const UnicodeString TAG_TARGET = L"F1NGS01.Location2";

static UnicodeString TrayInfoTag(const UnicodeString &Location, const UnicodeString &Name)
{
	return Location + L".TrayInformation." + Name;
}
//---------------------------------------------------------------------------
static UnicodeString TrayProcessTag(const UnicodeString &Location, const UnicodeString &Name)
{
	return Location + L".TrayProcess." + Name;
}
//---------------------------------------------------------------------------
static UnicodeString TrackInTag(const UnicodeString &Name)
{
	return TAG_SOURCE + L".TrackInCellInformation." + Name;
}
//---------------------------------------------------------------------------
static UnicodeString TrackOutTag(const UnicodeString &Name)
{
	return TAG_TARGET + L".TrackOutCellInformation." + Name;
}
//---------------------------------------------------------------------------
static UnicodeString CellTag(const UnicodeString &Root, int Index, const UnicodeString &Name)
{
	return Root + L".Cell." + IntToStr(Index) + L"." + Name;
}
//---------------------------------------------------------------------------
static bool IsTargetTrayActive(void)
{
	return MainForm != NULL && MainForm->tray == &MainForm->tray_target;
}
//---------------------------------------------------------------------------
static UnicodeString ActiveLocation(void)
{
	return IsTargetTrayActive() ? TAG_TARGET : TAG_SOURCE;
}
//---------------------------------------------------------------------------
static TRAY_INFO* ActiveTray(void)
{
	if (MainForm == NULL)
		return NULL;
	return IsTargetTrayActive() ? &MainForm->tray_target : &MainForm->tray_source;
}
//---------------------------------------------------------------------------
static TPanel* ActiveTrayIdPanel(void)
{
	if (MainForm == NULL)
		return NULL;
	return IsTargetTrayActive() ? MainForm->pTrayid_target : MainForm->pTrayid_source;
}
//---------------------------------------------------------------------------
static TPanel* ActiveMesTrayIdPanel(void)
{
	if (MainForm == NULL)
		return NULL;
	return IsTargetTrayActive() ? MainForm->pTrayid_target2 : MainForm->pTrayid_source2;
}
//---------------------------------------------------------------------------
static void SetPcBool(const UnicodeString &Key, bool Value)
{
	if (Mod_Fms != NULL)
		Mod_Fms->SetPcTag(Key, Value);
}
//---------------------------------------------------------------------------
static void SetPcInt(const UnicodeString &Key, int Value)
{
	if (Mod_Fms != NULL)
		Mod_Fms->SetPcTag(Key, Value);
}
//---------------------------------------------------------------------------
static void SetPcString(const UnicodeString &Key, const UnicodeString &Value)
{
	if (Mod_Fms != NULL)
		Mod_Fms->SetPcTag(Key, Value);
}
//---------------------------------------------------------------------------
static bool GetFmsBool(const UnicodeString &Key)
{
	return Mod_Fms != NULL && Mod_Fms->GetFmsTagBool(Key, false);
}
//---------------------------------------------------------------------------
static int GetFmsInt(const UnicodeString &Key)
{
	if (Mod_Fms != NULL)
		return Mod_Fms->GetFmsTagInt(Key, 0);
	return 0;
}
//---------------------------------------------------------------------------
static UnicodeString GetFmsString(const UnicodeString &Key)
{
	if (Mod_Fms != NULL)
		return Mod_Fms->GetFmsTagString(Key, L"");
	return L"";
}
//---------------------------------------------------------------------------
static bool HasFmsTag(const UnicodeString &Key)
{
	UnicodeString JsonValue;
	return Mod_Fms != NULL && Mod_Fms->GetFmsTagJson(Key, JsonValue);
}
//---------------------------------------------------------------------------
static void LogOpcEvent(const AnsiString &Message, bool bDisplay = false)
{
	if (MainForm != NULL)
		MainForm->WriteOpcUaLog("EVENT", Message, bDisplay);
}
//---------------------------------------------------------------------------
static bool ReadRequiredString(const UnicodeString &Key, UnicodeString &Value, const AnsiString &Name)
{
	if (!HasFmsTag(Key))
	{
		LogOpcEvent("VALIDATION FAIL missing " + Name, true);
		return false;
	}

	Value = GetFmsString(Key).Trim();
	if (Value.IsEmpty())
	{
		LogOpcEvent("VALIDATION FAIL empty " + Name, true);
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------
static bool ValidateSourceTrackInCells(void)
{
	if (!HasFmsTag(TrackInTag(L"CellCount")))
	{
		LogOpcEvent("VALIDATION FAIL missing TrackInCellInformation.CellCount", true);
		return false;
	}

	int Count = GetFmsInt(TrackInTag(L"CellCount"));
	if (Count <= 0 || Count > 96)
	{
		LogOpcEvent("VALIDATION FAIL TrackInCellInformation.CellCount=" + IntToStr(Count), true);
		return false;
	}

	for (int i = 0; i < Count; ++i)
	{
		UnicodeString Prefix = L"TrackInCellInformation.Cell[" + IntToStr(i) + L"]";
		if (!HasFmsTag(CellTag(TAG_SOURCE + L".TrackInCellInformation", i, L"CellNo")) ||
			!HasFmsTag(CellTag(TAG_SOURCE + L".TrackInCellInformation", i, L"CellExist")) ||
			!HasFmsTag(CellTag(TAG_SOURCE + L".TrackInCellInformation", i, L"WorkFlag")))
		{
			LogOpcEvent("VALIDATION FAIL missing " + AnsiString(Prefix), true);
			return false;
		}

		int CellNo = GetFmsInt(CellTag(TAG_SOURCE + L".TrackInCellInformation", i, L"CellNo"));
		if (CellNo < 1 || CellNo > 96)
		{
			LogOpcEvent("VALIDATION FAIL " + AnsiString(Prefix) + ".CellNo=" + IntToStr(CellNo), true);
			return false;
		}

		bool CellExist = GetFmsBool(CellTag(TAG_SOURCE + L".TrackInCellInformation", i, L"CellExist"));
		UnicodeString CellId = GetFmsString(CellTag(TAG_SOURCE + L".TrackInCellInformation", i, L"CellId")).Trim();
		if (CellExist && CellId.IsEmpty())
		{
			LogOpcEvent("VALIDATION FAIL empty " + AnsiString(Prefix) + ".CellId", true);
			return false;
		}
	}

	return true;
}
//---------------------------------------------------------------------------
static void ClearTrayCells(TRAY_INFO *Tray)
{
	if (Tray == NULL)
		return;

	for (int i = 0; i < 96; ++i)
	{
		Tray->SLOT_POSITION[i] = "";
		Tray->TARGET_SLOT_POSITION[i] = "";
		Tray->SLOT_ID[i] = "";
		Tray->LOSS_CD[i] = "";
		Tray->LOSS_DESC[i] = "";
		Tray->PICK[i] = "";
		Tray->RANK[i] = "";
		Tray->SAMPLE_CODE[i] = "";
	}
}
//---------------------------------------------------------------------------
static void ApplySourceTrackInCells(TRAY_INFO *Tray)
{
	if (Tray == NULL)
		return;

	UnicodeString Root = TAG_SOURCE + L".TrackInCellInformation";
	Tray->SLOT_COUNT = GetFmsInt(TrackInTag(L"CellCount"));
	for (int i = 0; i < Tray->SLOT_COUNT && i < 96; ++i)
	{
		bool CellExist = GetFmsBool(CellTag(Root, i, L"CellExist"));
		int CellNo = GetFmsInt(CellTag(Root, i, L"CellNo"));
		UnicodeString Grade = GetFmsString(CellTag(Root, i, L"Grade")).Trim();

		Tray->SLOT_POSITION[i] = AnsiString(IntToStr(CellNo));
		if (CellExist)
			Tray->SLOT_ID[i] = AnsiString(GetFmsString(CellTag(Root, i, L"CellId")).Trim());
		else
			Tray->SLOT_ID[i] = "";
		Tray->LOSS_CD[i] = AnsiString(GetFmsString(CellTag(Root, i, L"NGCode")).Trim());
		Tray->RANK[i] = AnsiString(Grade);
		Tray->PICK[i] = (CellExist && Grade.UpperCase() == L"NG") ? "Y" : "N";
	}
}
//---------------------------------------------------------------------------
static void ApplyTrayDisplay(TRAY_INFO *Tray, const UnicodeString &ProductModel,
	const UnicodeString &ProcessId, const UnicodeString &LotId)
{
	if (MainForm == NULL || Tray == NULL)
		return;

	Tray->KIND = AnsiString(ProductModel);
	Tray->WORK_CODE = AnsiString(ProcessId);
	Tray->RETURN_VALUE = "1";
	Tray->ERROR_MSG = "";

	TPanel *TrayIdPanel = ActiveMesTrayIdPanel();
	if (TrayIdPanel != NULL)
		TrayIdPanel->Caption = LotId;

	if (Tray == &MainForm->tray_source)
	{
		MainForm->pPROCESS->Caption = ProcessId;
		MainForm->pKIND->Caption = ProductModel;
		MainForm->pSLOT_COUNT->Caption = IntToStr(Tray->SLOT_COUNT);
	}
	else
	{
		MainForm->pPROCESS_target->Caption = ProcessId;
		MainForm->pKIND_target->Caption = ProductModel;
		MainForm->pSLOT_COUNT_target->Caption = IntToStr(Tray->SLOT_COUNT);
	}
}
//---------------------------------------------------------------------------
__fastcall TMesOpc::TMesOpc(TComponent* Owner)
	: TDataModule(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::TRAY_LOAD_REQUEST()
{
	if (MainForm == NULL)
		return;

	UnicodeString Location = ActiveLocation();
	TPanel *TrayIdPanel = ActiveTrayIdPanel();
	UnicodeString TrayId = L"";
	if (TrayIdPanel != NULL)
		TrayId = TrayIdPanel->Caption;

	SetPcBool(TrayInfoTag(Location, L"TrayExist"), true);
	SetPcString(TrayInfoTag(Location, L"TrayId"), TrayId);
	SetPcBool(TrayProcessTag(Location, L"TrayLoad"), true);
	LogOpcEvent("TRAY_LOAD_REQUEST " + AnsiString(Location) + " TrayId=" + AnsiString(TrayId), true);
}
//---------------------------------------------------------------------------
bool __fastcall TMesOpc::TRAY_LOAD_RESPONSE()
{
	UnicodeString Location = ActiveLocation();
	int Response = GetFmsInt(TrayProcessTag(Location, L"TrayLoadResponse"));
	if (Response == 0)
		return false;
	if (Response != 1 && Response != 2)
	{
		LogOpcEvent("VALIDATION FAIL TrayLoadResponse=" + IntToStr(Response), true);
		return false;
	}

	TRAY_INFO *Tray = ActiveTray();
	if (Tray != NULL)
	{
		UnicodeString ProductModel;
		UnicodeString RouteId;
		UnicodeString ProcessId;
		UnicodeString LotId;

		bool ValidTrayLoad = ReadRequiredString(TrayInfoTag(Location, L"ProductModel"), ProductModel, "TrayInformation.ProductModel") &&
			ReadRequiredString(TrayInfoTag(Location, L"RouteId"), RouteId, "TrayInformation.RouteId") &&
			ReadRequiredString(TrayInfoTag(Location, L"ProcessId"), ProcessId, "TrayInformation.ProcessId") &&
			ReadRequiredString(TrayInfoTag(Location, L"LotId"), LotId, "TrayInformation.LotId");
		if (!ValidTrayLoad)
			return false;

		ClearTrayCells(Tray);
		if (Tray == &MainForm->tray_source)
		{
			if (!ValidateSourceTrackInCells())
				return false;
			ApplySourceTrackInCells(Tray);
		}
		else
			Tray->SLOT_COUNT = 96;

		Tray->TRAY_GUBUN = IntToStr(Tray->SLOT_COUNT);
		ApplyTrayDisplay(Tray, ProductModel, ProcessId, LotId);
	}

	SetPcBool(TrayProcessTag(Location, L"TrayLoad"), false);
	LogOpcEvent("TRAY_LOAD_RESPONSE " + AnsiString(Location) + " Response=" + IntToStr(Response));
	return true;
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::RECIPE_REQUEST()
{
	LogOpcEvent("RECIPE_REQUEST skipped");
}
//---------------------------------------------------------------------------
bool __fastcall TMesOpc::RECIPE_RESPONSE()
{
	return true;
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::PROCESS_START_REQUEST()
{
	SetPcBool(TrayProcessTag(TAG_SOURCE, L"ProcessStart"), true);
	LogOpcEvent("PROCESS_START_REQUEST");
}
//---------------------------------------------------------------------------
bool __fastcall TMesOpc::PROCESS_START_RESPONSE()
{
	if (!GetFmsBool(TrayProcessTag(TAG_SOURCE, L"ProcessStartResponse")))
		return false;

	SetPcBool(TrayProcessTag(TAG_SOURCE, L"ProcessStart"), false);
	LogOpcEvent("PROCESS_START_RESPONSE");
	return true;
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::PROCESS_DATA_WRITE()
{
	if (MainForm == NULL)
		return;

	TRAY_INFO *Tray = &MainForm->tray_target;
	int Count = Tray->SLOT_COUNT;
	if (Count <= 0 || Count > 96)
		Count = 96;

	UnicodeString Root = TAG_TARGET + L".TrackOutCellInformation";
	SetPcInt(TrackOutTag(L"CellCount"), Count);
	for (int i = 0; i < Count; ++i)
	{
		bool CellExist = !Tray->SLOT_ID[i].IsEmpty();
		SetPcString(CellTag(Root, i, L"CellId"), Tray->SLOT_ID[i]);
		SetPcInt(CellTag(Root, i, L"CellNo"), i + 1);
		SetPcString(CellTag(Root, i, L"LotId"), MainForm->pTrayid_target2->Caption);
		SetPcBool(CellTag(Root, i, L"CellExist"), CellExist);
		SetPcString(CellTag(Root, i, L"NGCode"), Tray->LOSS_CD[i]);
		SetPcString(CellTag(Root, i, L"Grade"), Tray->RANK[i]);
		SetPcBool(CellTag(Root, i, L"WorkFlag"), CellExist);
	}

	LogOpcEvent("PROCESS_DATA_WRITE Count=" + IntToStr(Count), true);
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::PROCESS_END_REQUEST()
{
	SetPcBool(TrayProcessTag(TAG_SOURCE, L"ProcessEnd"), true);
	LogOpcEvent("PROCESS_END_REQUEST");
}
//---------------------------------------------------------------------------
bool __fastcall TMesOpc::PROCESS_END_RESPONSE()
{
	if (!GetFmsBool(TrayProcessTag(TAG_SOURCE, L"ProcessEndResponse")))
		return false;

	SetPcBool(TrayProcessTag(TAG_SOURCE, L"ProcessEnd"), false);
	LogOpcEvent("PROCESS_END_RESPONSE");
	return true;
}
//---------------------------------------------------------------------------
