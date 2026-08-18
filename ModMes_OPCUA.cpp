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
static UnicodeString CellTrackOutTag(const UnicodeString &Name)
{
	return TAG_TARGET + L".CellTrackOut." + Name;
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
static UnicodeString LocationFor(bool SourceTray)
{
	return SourceTray ? TAG_SOURCE : TAG_TARGET;
}
//---------------------------------------------------------------------------
static UnicodeString ActiveLocation(void)
{
	return IsTargetTrayActive() ? TAG_TARGET : TAG_SOURCE;
}
//---------------------------------------------------------------------------
static TRAY_INFO* TrayFor(bool SourceTray)
{
	if (MainForm == NULL)
		return NULL;
	return SourceTray ? &MainForm->tray_source : &MainForm->tray_target;
}
//---------------------------------------------------------------------------
static TRAY_INFO* ActiveTray(void)
{
	return TrayFor(!IsTargetTrayActive());
}
//---------------------------------------------------------------------------
static TPanel* TrayIdPanelFor(bool SourceTray)
{
	if (MainForm == NULL)
		return NULL;
	return SourceTray ? MainForm->pTrayid_source : MainForm->pTrayid_target;
}
//---------------------------------------------------------------------------
static TPanel* ActiveTrayIdPanel(void)
{
	return TrayIdPanelFor(!IsTargetTrayActive());
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
static bool ReadRequiredString(const UnicodeString &Key, UnicodeString &Value, const AnsiString &Name, bool LogFailure)
{
	if (!HasFmsTag(Key))
	{
		if(LogFailure) LogOpcEvent("VALIDATION FAIL missing " + Name, true);
		return false;
	}

	Value = GetFmsString(Key).Trim();
	if (Value.IsEmpty())
	{
		if(LogFailure) LogOpcEvent("VALIDATION FAIL empty " + Name, true);
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------
static bool ValidateSourceTrackInCells(bool LogFailure)
{
	if (!HasFmsTag(TrackInTag(L"CellCount")))
	{
		if(LogFailure) LogOpcEvent("VALIDATION FAIL missing TrackInCellInformation.CellCount", false);
		return false;
	}

	int Count = GetFmsInt(TrackInTag(L"CellCount"));
	if (Count <= 0 || Count > 96)
	{
		if(LogFailure) LogOpcEvent("VALIDATION FAIL TrackInCellInformation.CellCount=" + IntToStr(Count), false);
		return false;
	}

	for (int i = 0; i < Count; ++i)
	{
		UnicodeString Prefix = L"TrackInCellInformation.Cell[" + IntToStr(i) + L"]";
		if (!HasFmsTag(CellTag(TAG_SOURCE + L".TrackInCellInformation", i, L"CellNo")) ||
			!HasFmsTag(CellTag(TAG_SOURCE + L".TrackInCellInformation", i, L"CellExist")) ||
			!HasFmsTag(CellTag(TAG_SOURCE + L".TrackInCellInformation", i, L"WorkFlag")) ||
			!HasFmsTag(CellTag(TAG_SOURCE + L".TrackInCellInformation", i, L"LotId")) ||
			!HasFmsTag(CellTag(TAG_SOURCE + L".TrackInCellInformation", i, L"Grade")) ||
			!HasFmsTag(CellTag(TAG_SOURCE + L".TrackInCellInformation", i, L"NGCode")))
		{
			if(LogFailure) LogOpcEvent("VALIDATION FAIL missing " + AnsiString(Prefix), false);
			return false;
		}

		int CellNo = GetFmsInt(CellTag(TAG_SOURCE + L".TrackInCellInformation", i, L"CellNo"));
		if (CellNo < 1 || CellNo > 96)
		{
			if(LogFailure) LogOpcEvent("VALIDATION FAIL " + AnsiString(Prefix) + ".CellNo=" + IntToStr(CellNo), false);
			return false;
		}

		bool CellExist = GetFmsBool(CellTag(TAG_SOURCE + L".TrackInCellInformation", i, L"CellExist"));
		UnicodeString CellId = GetFmsString(CellTag(TAG_SOURCE + L".TrackInCellInformation", i, L"CellId")).Trim();
		if (CellExist && CellId.IsEmpty())
		{
			if(LogFailure) LogOpcEvent("VALIDATION FAIL empty " + AnsiString(Prefix) + ".CellId", false);
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
		Tray->CELL_LOT_ID[i] = "";
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
		UnicodeString Grade = GetFmsString(CellTag(Root, i, L"Grade"));

		Tray->SLOT_POSITION[i] = AnsiString(IntToStr(CellNo));
		if (CellExist)
			Tray->SLOT_ID[i] = AnsiString(GetFmsString(CellTag(Root, i, L"CellId")).Trim());
		else
			Tray->SLOT_ID[i] = "";
		// Preserve these TrackIn values per cell without substituting target-tray data.
		Tray->CELL_LOT_ID[i] = AnsiString(GetFmsString(CellTag(Root, i, L"LotId")));
		Tray->LOSS_CD[i] = AnsiString(GetFmsString(CellTag(Root, i, L"NGCode")));
		Tray->RANK[i] = AnsiString(Grade);
		Tray->PICK[i] = (CellExist && Grade.Trim().UpperCase() == L"NG") ? "Y" : "N";
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

	TPanel *TrayIdPanel = Tray == &MainForm->tray_source ?
		MainForm->pTrayid_source2 : MainForm->pTrayid_target2;
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
	TRAY_LOAD_REQUEST(!IsTargetTrayActive());
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::TRAY_LOAD_REQUEST(bool SourceTray)
{
	if (MainForm == NULL)
		return;

	UnicodeString Location = LocationFor(SourceTray);
	TPanel *TrayIdPanel = TrayIdPanelFor(SourceTray);
	UnicodeString TrayId = L"";
	if (TrayIdPanel != NULL)
		TrayId = TrayIdPanel->Caption;

	// In production, discard a stale response so only a new FMS handshake is accepted.
	// MES test mode intentionally preserves the response/data preloaded in Gateway UI.
	bool MesTestMode = MainForm->cbMES != NULL && MainForm->cbMES->Checked;
	if (Mod_Fms != NULL && !MesTestMode)
		Mod_Fms->ClearFmsTag(TrayProcessTag(Location, L"TrayLoadResponse"));
	else if (MesTestMode)
		LogOpcEvent("MES TEST: preserve preloaded TrayLoadResponse " + AnsiString(Location));
	SetPcBool(TrayInfoTag(Location, L"TrayExist"), true);
	SetPcString(TrayInfoTag(Location, L"TrayId"), TrayId);
	SetPcBool(TrayProcessTag(Location, L"TrayLoad"), true);
	LogOpcEvent("TRAY_LOAD_REQUEST " + AnsiString(Location) + " TrayId=" + AnsiString(TrayId), true);
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::TRAY_LOAD_CANCEL(bool SourceTray)
{
	UnicodeString Location = LocationFor(SourceTray);
	SetPcBool(TrayProcessTag(Location, L"TrayLoad"), false);
	LogOpcEvent("TRAY_LOAD_CANCEL " + AnsiString(Location));
}
//---------------------------------------------------------------------------
bool __fastcall TMesOpc::TRAY_LOAD_RESPONSE()
{
	return TRAY_LOAD_RESPONSE(!IsTargetTrayActive()) == 1;
}
//---------------------------------------------------------------------------
int __fastcall TMesOpc::TRAY_LOAD_RESPONSE(bool SourceTray)
{
	UnicodeString Location = LocationFor(SourceTray);
	UnicodeString ResponseKey = TrayProcessTag(Location, L"TrayLoadResponse");
	int Response = GetFmsInt(ResponseKey);
	if (Response == 0)
		return 0;
	if (Response != 1 && Response != 2)
	{
		LogOpcEvent("VALIDATION FAIL TrayLoadResponse=" + IntToStr(Response), true);
		SetPcBool(TrayProcessTag(Location, L"TrayLoad"), false);
		if (Mod_Fms != NULL) Mod_Fms->ClearFmsTag(ResponseKey);
		return -1;
	}
	if (Response == 2)
	{
		LogOpcEvent("TRAY_LOAD_RESPONSE FAIL " + AnsiString(Location), true);
		SetPcBool(TrayProcessTag(Location, L"TrayLoad"), false);
		if (Mod_Fms != NULL) Mod_Fms->ClearFmsTag(ResponseKey);
		return 2;
	}

	TRAY_INFO *Tray = TrayFor(SourceTray);
	if (Tray != NULL)
	{
		UnicodeString ProductModel;
		UnicodeString ProcessId;
		UnicodeString LotId;

		if (SourceTray)
		{
			UnicodeString RouteId;
			// TrayLoadResponse and the 96-cell payload can arrive in separate
			// FMS_CHANGED messages. Keep waiting instead of deleting the response.
			bool ValidTrayLoad =
				ReadRequiredString(TrayInfoTag(Location, L"ProductModel"), ProductModel, "TrayInformation.ProductModel", false) &&
				ReadRequiredString(TrayInfoTag(Location, L"RouteId"), RouteId, "TrayInformation.RouteId", false) &&
				ReadRequiredString(TrayInfoTag(Location, L"ProcessId"), ProcessId, "TrayInformation.ProcessId", false) &&
				ReadRequiredString(TrayInfoTag(Location, L"LotId"), LotId, "TrayInformation.LotId", false) &&
				ValidateSourceTrackInCells(false);
			if (!ValidTrayLoad)
				return 0;
		}
		else
		{
			// Location2 has only TrayExist/TrayId in the deployed NodeSet.
			// Requiring Location2 ProductModel/RouteId/ProcessId/LotId caused a
			// permanent validation failure even when TrayLoadResponse was 1.
			if (MainForm != NULL)
			{
				ProductModel = UnicodeString(MainForm->tray_source.KIND);
				ProcessId = UnicodeString(MainForm->tray_source.WORK_CODE);
			}
			TPanel *TrayIdPanel = TrayIdPanelFor(false);
			if (TrayIdPanel != NULL)
				LotId = TrayIdPanel->Caption.Trim();
			if (LotId.IsEmpty())
				return 0;
		}

		ClearTrayCells(Tray);
		if (SourceTray)
		{
			ApplySourceTrackInCells(Tray);
			Tray->PASS = "N";
		}
		else
		{
			Tray->SLOT_COUNT = 96;
			for (int i = 0; i < Tray->SLOT_COUNT; ++i)
			{
				Tray->SLOT_POSITION[i] = IntToStr(i + 1);
				Tray->PICK[i] = "N";
			}
		}

		Tray->TRAY_GUBUN = IntToStr(Tray->SLOT_COUNT);
		ApplyTrayDisplay(Tray, ProductModel, ProcessId, LotId);
	}

	SetPcBool(TrayProcessTag(Location, L"TrayLoad"), false);
	bool MesTestMode = MainForm != NULL && MainForm->cbMES != NULL && MainForm->cbMES->Checked;
	if (Mod_Fms != NULL && !MesTestMode) Mod_Fms->ClearFmsTag(ResponseKey);
	LogOpcEvent("TRAY_LOAD_RESPONSE SUCCESS " + AnsiString(Location));
	return 1;
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::LogTrayLoadTimeout(bool SourceTray)
{
	UnicodeString Location = LocationFor(SourceTray);
	UnicodeString ResponseJson;
	bool HasResponse = Mod_Fms != NULL &&
		Mod_Fms->GetFmsTagJson(TrayProcessTag(Location, L"TrayLoadResponse"), ResponseJson);
	AnsiString Message = "TRAY_LOAD_TIMEOUT " + AnsiString(Location) +
		" TrayLoadResponse=" + (HasResponse ? AnsiString(ResponseJson) : AnsiString("<missing>"));
	if (SourceTray)
		Message += " TrackInCellInformation.CellCount=" + IntToStr(GetFmsInt(TrackInTag(L"CellCount")));

	if (MainForm != NULL)
		MainForm->WriteOpcUaLog("ERROR", Message, true);

	// Emit the exact missing/invalid source field only once, at timeout.
	if (SourceTray && GetFmsInt(TrayProcessTag(Location, L"TrayLoadResponse")) == 1)
	{
		UnicodeString Value;
		ReadRequiredString(TrayInfoTag(Location, L"ProductModel"), Value, "TrayInformation.ProductModel", true);
		ReadRequiredString(TrayInfoTag(Location, L"RouteId"), Value, "TrayInformation.RouteId", true);
		ReadRequiredString(TrayInfoTag(Location, L"ProcessId"), Value, "TrayInformation.ProcessId", true);
		ReadRequiredString(TrayInfoTag(Location, L"LotId"), Value, "TrayInformation.LotId", true);
		ValidateSourceTrackInCells(true);
	}
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
	bool MesTestMode = MainForm != NULL && MainForm->cbMES != NULL && MainForm->cbMES->Checked;
	if (Mod_Fms != NULL && !MesTestMode)
		Mod_Fms->ClearFmsTag(TrayProcessTag(TAG_SOURCE, L"ProcessStartResponse"));
	else if (MesTestMode)
		LogOpcEvent("MES TEST: preserve preloaded ProcessStartResponse");
	SetPcBool(TrayProcessTag(TAG_SOURCE, L"ProcessStart"), true);
	LogOpcEvent("PROCESS_START_REQUEST");
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::PROCESS_START_CANCEL()
{
	SetPcBool(TrayProcessTag(TAG_SOURCE, L"ProcessStart"), false);
	LogOpcEvent("PROCESS_START_CANCEL");
}
//---------------------------------------------------------------------------
bool __fastcall TMesOpc::PROCESS_START_RESPONSE()
{
	return PROCESS_START_RESPONSE_RESULT() == 1;
}
//---------------------------------------------------------------------------
int __fastcall TMesOpc::PROCESS_START_RESPONSE_RESULT()
{
	UnicodeString ResponseKey = TrayProcessTag(TAG_SOURCE, L"ProcessStartResponse");
	int Response = GetFmsInt(ResponseKey);
	if (Response == 0)
		return 0;

	SetPcBool(TrayProcessTag(TAG_SOURCE, L"ProcessStart"), false);
	bool MesTestMode = MainForm != NULL && MainForm->cbMES != NULL && MainForm->cbMES->Checked;
	if (Mod_Fms != NULL && !MesTestMode) Mod_Fms->ClearFmsTag(ResponseKey);
	if (Response == 1)
	{
		LogOpcEvent("PROCESS_START_RESPONSE SUCCESS");
		return 1;
	}
	if (Response == 2)
	{
		LogOpcEvent("PROCESS_START_RESPONSE FAIL", true);
		return 2;
	}

	LogOpcEvent("VALIDATION FAIL ProcessStartResponse=" + IntToStr(Response), true);
	return -1;
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
	UnicodeString TargetTrayId = MainForm->pTrayid_target2->Caption.Trim();
	if(TargetTrayId.IsEmpty()) TargetTrayId = MainForm->pTrayid_target->Caption.Trim();
	SetPcInt(TrackOutTag(L"CellCount"), Count);
	for (int i = 0; i < Count; ++i)
	{
		bool CellExist = !Tray->SLOT_ID[i].IsEmpty();
		SetPcString(CellTag(Root, i, L"CellId"), Tray->SLOT_ID[i]);
		SetPcInt(CellTag(Root, i, L"CellNo"), i + 1);
		// LotId/Grade/NGCode must be the original TrackInCellInformation values.
		SetPcString(CellTag(Root, i, L"LotId"), Tray->CELL_LOT_ID[i]);
		SetPcBool(CellTag(Root, i, L"CellExist"), CellExist);
		SetPcString(CellTag(Root, i, L"NGCode"), Tray->LOSS_CD[i]);
		SetPcString(CellTag(Root, i, L"Grade"), Tray->RANK[i]);
		SetPcBool(CellTag(Root, i, L"WorkFlag"), CellExist);
		// Full TrackOut array payload is intentionally file-only.
		if(MainForm != NULL)
			MainForm->WriteOpcUaLog("TRACK_OUT_DETAIL",
				"Cell[" + IntToStr(i) + "] CellNo=" + IntToStr(i + 1) +
				" CellExist=" + IntToStr(CellExist ? 1 : 0) +
				" CellId=" + Tray->SLOT_ID[i] +
				" LotId=" + Tray->CELL_LOT_ID[i] +
				" Grade=" + Tray->RANK[i] +
				" NGCode=" + Tray->LOSS_CD[i] +
				" WorkFlag=" + IntToStr(CellExist ? 1 : 0), false);
	}

	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
	LogOpcEvent("TRACK_OUT_CELL_INFORMATION WRITE TrayId=" + AnsiString(TargetTrayId) +
		" Count=" + IntToStr(Count), true);
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::CELL_TRACK_OUT_REQUEST(int SourceChannel, int TargetChannel,
	const AnsiString &CellId)
{
	if(MainForm == NULL || Mod_Fms == NULL)
		return;

	UnicodeString SourceTrayId = MainForm->pTrayid_source->Caption.Trim();
	if(SourceTrayId.IsEmpty()) SourceTrayId = MainForm->pTrayid_source2->Caption.Trim();
	UnicodeString TargetTrayId = MainForm->pTrayid_target->Caption.Trim();
	if(TargetTrayId.IsEmpty()) TargetTrayId = MainForm->pTrayid_target2->Caption.Trim();

	bool MesTestMode = MainForm->cbMES != NULL && MainForm->cbMES->Checked;
	if(!MesTestMode)
		Mod_Fms->ClearFmsTag(CellTrackOutTag(L"CellUnloadCompleteResponse"));

	SetPcInt(CellTrackOutTag(L"CellNoFrom"), SourceChannel);
	SetPcString(CellTrackOutTag(L"TrayIdFrom"), SourceTrayId);
	SetPcInt(CellTrackOutTag(L"CellNoTo"), TargetChannel);
	SetPcString(CellTrackOutTag(L"TrayIdTo"), TargetTrayId);
	SetPcString(CellTrackOutTag(L"CellId"), UnicodeString(CellId));
	SetPcBool(CellTrackOutTag(L"CellUnloadComplete"), true);
	Mod_Fms->FlushPendingPcTags(false);

	LogOpcEvent("CELL_TRACK_OUT REQUEST CellId=" + CellId +
		" From=" + AnsiString(SourceTrayId) + "/" + IntToStr(SourceChannel) +
		" To=" + AnsiString(TargetTrayId) + "/" + IntToStr(TargetChannel) +
		" RequestTag=F1NGS01.Location2.CellTrackOut.CellUnloadComplete=true" +
		" WaitingTag=F1NGS01.Location2.CellTrackOut.CellUnloadCompleteResponse" +
		" Expected=1(Success),2(Fail)", true);
}
//---------------------------------------------------------------------------
int __fastcall TMesOpc::CELL_TRACK_OUT_RESPONSE_RESULT()
{
	int Response = GetFmsInt(CellTrackOutTag(L"CellUnloadCompleteResponse"));
	if(Response == 0)
		return 0;

	SetPcBool(CellTrackOutTag(L"CellUnloadComplete"), false);
	bool MesTestMode = MainForm != NULL && MainForm->cbMES != NULL && MainForm->cbMES->Checked;
	if(Mod_Fms != NULL){
		if(!MesTestMode)
			Mod_Fms->ClearFmsTag(CellTrackOutTag(L"CellUnloadCompleteResponse"));
		Mod_Fms->FlushPendingPcTags(false);
	}
	if(Response == 1){
		LogOpcEvent("CELL_TRACK_OUT RESPONSE SUCCESS", true);
		return 1;
	}
	if(Response == 2){
		LogOpcEvent("CELL_TRACK_OUT RESPONSE FAIL", true);
		return 2;
	}
	LogOpcEvent("VALIDATION FAIL CellUnloadCompleteResponse=" + IntToStr(Response), true);
	return -1;
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::LogCellTrackOutTimeout()
{
	const UnicodeString ResponseKey = CellTrackOutTag(L"CellUnloadCompleteResponse");
	UnicodeString RawValue;
	bool GatewayConnected = Mod_Fms != NULL && Mod_Fms->IsGatewayConnected();
	bool SnapshotReceived = Mod_Fms != NULL && Mod_Fms->SnapshotReceived;
	bool TagPresent = Mod_Fms != NULL && Mod_Fms->GetFmsTagJson(ResponseKey, RawValue);
	int ParsedValue = TagPresent ? StrToIntDef(RawValue.Trim(), -9999) : -9999;

	TFmsTagDefinition Definition;
	bool DefinitionFound = Mod_Fms != NULL &&
		Mod_Fms->GetTagDefinitionInfo(ResponseKey, Definition);
	UnicodeString NodeId = L"<definition missing>";
	UnicodeString DataType = L"<unknown>";
	UnicodeString LastEquipment = L"<none>";
	UnicodeString LastTimestamp = L"<none>";
	if(DefinitionFound){
		NodeId = Definition.NodeId;
		DataType = Definition.DataType;
	}
	if(Mod_Fms != NULL){
		LastEquipment = Mod_Fms->LastEquipment;
		LastTimestamp = Mod_Fms->LastTimestamp;
	}
	AnsiString RawText = "<missing>";
	AnsiString ParsedText = "<not read>";
	if(TagPresent){
		RawText = AnsiString(RawValue);
		ParsedText = AnsiString(IntToStr(ParsedValue));
	}

	AnsiString Message =
		"CELL_TRACK_OUT_TIMEOUT WaitingTag=" + AnsiString(ResponseKey) +
		" NodeId=" + AnsiString(NodeId) +
		" DataType=" + AnsiString(DataType) +
		" Expected=1(Success),2(Fail)" +
		" ActualRaw=" + RawText +
		" Parsed=" + ParsedText +
		" TagPresent=" + AnsiString(TagPresent ? "true" : "false") +
		" GatewayConnected=" + AnsiString(GatewayConnected ? "true" : "false") +
		" SnapshotReceived=" + AnsiString(SnapshotReceived ? "true" : "false") +
		" LastEquipment=" + AnsiString(LastEquipment) +
		" LastTimestamp=" + AnsiString(LastTimestamp) +
		" RequestTag=F1NGS01.Location2.CellTrackOut.CellUnloadComplete(true)";

	if(MainForm != NULL)
		MainForm->WriteOpcUaLog("ERROR", Message, true);
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::CELL_TRACK_OUT_CANCEL()
{
	SetPcBool(CellTrackOutTag(L"CellUnloadComplete"), false);
	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
	LogOpcEvent("CELL_TRACK_OUT CANCEL", true);
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::TRAY_UNLOAD_REQUEST()
{
	const UnicodeString RequestKey = TrayProcessTag(TAG_TARGET, L"TrayUnloadRequest");
	const UnicodeString ResponseKey = TrayProcessTag(TAG_TARGET, L"TrayUnloadResponse");
	bool MesTestMode = MainForm != NULL && MainForm->cbMES != NULL && MainForm->cbMES->Checked;
	if(Mod_Fms != NULL && !MesTestMode) Mod_Fms->ClearFmsTag(ResponseKey);
	SetPcBool(RequestKey, true);
	LogOpcEvent("TRAY_UNLOAD_REQUEST Location2 Request=true Waiting=TrayUnloadResponse", false);
}
//---------------------------------------------------------------------------
int __fastcall TMesOpc::TRAY_UNLOAD_RESPONSE_RESULT()
{
	const UnicodeString RequestKey = TrayProcessTag(TAG_TARGET, L"TrayUnloadRequest");
	const UnicodeString ResponseKey = TrayProcessTag(TAG_TARGET, L"TrayUnloadResponse");
	int Response = GetFmsInt(ResponseKey);
	if(Response == 0) return 0;
	SetPcBool(RequestKey, false);
	bool MesTestMode = MainForm != NULL && MainForm->cbMES != NULL && MainForm->cbMES->Checked;
	if(Mod_Fms != NULL && !MesTestMode) Mod_Fms->ClearFmsTag(ResponseKey);
	if(Response == 1){
		LogOpcEvent("TRAY_UNLOAD_RESPONSE SUCCESS Value=1", false);
		return 1;
	}
	if(Response == 2){
		LogOpcEvent("TRAY_UNLOAD_RESPONSE FAIL Value=2", false);
		return 2;
	}
	LogOpcEvent("VALIDATION FAIL TrayUnloadResponse=" + IntToStr(Response), false);
	return -1;
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::LogTrayUnloadTimeout()
{
	const UnicodeString ResponseKey = TrayProcessTag(TAG_TARGET, L"TrayUnloadResponse");
	UnicodeString ResponseJson;
	bool HasResponse = Mod_Fms != NULL && Mod_Fms->GetFmsTagJson(ResponseKey, ResponseJson);
	if(MainForm != NULL)
		MainForm->WriteOpcUaLog("ERROR", "TRAY_UNLOAD_TIMEOUT Response=" +
			(HasResponse ? AnsiString(ResponseJson) : AnsiString("<missing>")), false);
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::TRAY_UNLOAD_CANCEL()
{
	SetPcBool(TrayProcessTag(TAG_TARGET, L"TrayUnloadRequest"), false);
	LogOpcEvent("TRAY_UNLOAD_CANCEL", false);
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::PROCESS_END_REQUEST()
{
	UnicodeString ResponseKey = TrayProcessTag(TAG_SOURCE, L"ProcessEndResponse");
	bool MesTestMode = MainForm != NULL && MainForm->cbMES != NULL && MainForm->cbMES->Checked;
	if(Mod_Fms != NULL && !MesTestMode)
		Mod_Fms->ClearFmsTag(ResponseKey);
	else if(MesTestMode)
		LogOpcEvent("MES TEST: preserve preloaded ProcessEndResponse");

	SetPcBool(TrayProcessTag(TAG_SOURCE, L"ProcessEnd"), true);
	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
	LogOpcEvent("PROCESS_END_REQUEST RequestTag=F1NGS01.Location1.TrayProcess.ProcessEnd=true "
		"WaitingTag=F1NGS01.Location1.TrayProcess.ProcessEndResponse Expected=1(Success),2(Fail)", true);
}
//---------------------------------------------------------------------------
bool __fastcall TMesOpc::PROCESS_END_RESPONSE()
{
	return PROCESS_END_RESPONSE_RESULT() == 1;
}
//---------------------------------------------------------------------------
int __fastcall TMesOpc::PROCESS_END_RESPONSE_RESULT()
{
	UnicodeString ResponseKey = TrayProcessTag(TAG_SOURCE, L"ProcessEndResponse");
	int Response = GetFmsInt(ResponseKey);
	if(Response == 0)
		return 0;

	SetPcBool(TrayProcessTag(TAG_SOURCE, L"ProcessEnd"), false);
	bool MesTestMode = MainForm != NULL && MainForm->cbMES != NULL && MainForm->cbMES->Checked;
	if(Mod_Fms != NULL){
		if(!MesTestMode)
			Mod_Fms->ClearFmsTag(ResponseKey);
		Mod_Fms->FlushPendingPcTags(false);
	}

	if(Response == 1){
		LogOpcEvent("PROCESS_END_RESPONSE SUCCESS Value=1", true);
		return 1;
	}
	if(Response == 2){
		LogOpcEvent("PROCESS_END_RESPONSE FAIL Value=2", true);
		return 2;
	}

	LogOpcEvent("VALIDATION FAIL ProcessEndResponse=" + IntToStr(Response), true);
	return -1;
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::LogProcessEndTimeout()
{
	const UnicodeString ResponseKey = TrayProcessTag(TAG_SOURCE, L"ProcessEndResponse");
	UnicodeString RawValue;
	bool GatewayConnected = Mod_Fms != NULL && Mod_Fms->IsGatewayConnected();
	bool SnapshotReceived = Mod_Fms != NULL && Mod_Fms->SnapshotReceived;
	bool TagPresent = Mod_Fms != NULL && Mod_Fms->GetFmsTagJson(ResponseKey, RawValue);
	int ParsedValue = TagPresent ? StrToIntDef(RawValue.Trim(), -9999) : -9999;

	TFmsTagDefinition Definition;
	bool DefinitionFound = Mod_Fms != NULL &&
		Mod_Fms->GetTagDefinitionInfo(ResponseKey, Definition);
	UnicodeString NodeId = L"<definition missing>";
	UnicodeString DataType = L"<unknown>";
	if(DefinitionFound){
		NodeId = Definition.NodeId;
		DataType = Definition.DataType;
	}

	AnsiString RawText = TagPresent ? AnsiString(RawValue) : AnsiString("<missing>");
	AnsiString Message =
		"PROCESS_END_TIMEOUT WaitingTag=" + AnsiString(ResponseKey) +
		" NodeId=" + AnsiString(NodeId) +
		" DataType=" + AnsiString(DataType) +
		" Expected=1(Success),2(Fail)" +
		" ActualRaw=" + RawText +
		" Parsed=" + IntToStr(ParsedValue) +
		" TagPresent=" + AnsiString(TagPresent ? "true" : "false") +
		" GatewayConnected=" + AnsiString(GatewayConnected ? "true" : "false") +
		" SnapshotReceived=" + AnsiString(SnapshotReceived ? "true" : "false") +
		" RequestTag=F1NGS01.Location1.TrayProcess.ProcessEnd(true)";

	if(MainForm != NULL)
		MainForm->WriteOpcUaLog("ERROR", Message, true);
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::PROCESS_END_CANCEL()
{
	SetPcBool(TrayProcessTag(TAG_SOURCE, L"ProcessEnd"), false);
	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
	LogOpcEvent("PROCESS_END_CANCEL", true);
}
//---------------------------------------------------------------------------
