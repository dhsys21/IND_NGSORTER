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

static const UnicodeString TAG_SOURCE = L"NGS.F1NGS01.Location1";
static const UnicodeString TAG_TARGET = L"NGS.F1NGS01.Location2";
static AnsiString GTrayLoadValidationError[2];

static void SetTrayLoadValidationError(bool SourceTray, const AnsiString &Message)
{
	GTrayLoadValidationError[SourceTray ? 0 : 1] = Message;
}

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
static UnicodeString TrackInTag(const UnicodeString &Location, const UnicodeString &Name)
{
	return Location + L".TrackInCellInformation." + Name;
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
static UnicodeString CellRecordTag(const UnicodeString &Root, int Index)
{
	return Root + L".Cell.Cell[" + IntToStr(Index) + L"]";
}
//---------------------------------------------------------------------------
static UnicodeString CellTag(const UnicodeString &Root, int Index, const UnicodeString &Name)
{
	return CellRecordTag(Root, Index) + L"." + Name;
}
//---------------------------------------------------------------------------
static bool IsCycleResponseBypass(void)
{
	return MainForm != NULL && MainForm->cbCycle != NULL && MainForm->cbCycle->Checked;
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
		SetTrayLoadValidationError(true,
			"Location1 TrayInformation validation failed: missing " + Name + ".");
		if(LogFailure) LogOpcEvent("VALIDATION FAIL missing " + Name, true);
		return false;
	}

	Value = GetFmsString(Key).Trim();
	if (Value.IsEmpty())
	{
		SetTrayLoadValidationError(true,
			"Location1 TrayInformation validation failed: " + Name + " is empty.");
		if(LogFailure) LogOpcEvent("VALIDATION FAIL empty " + Name, true);
		return false;
	}

	return true;
}
//---------------------------------------------------------------------------
static bool ValidateSourceTrackInCells(bool LogFailure)
{
	SetTrayLoadValidationError(true, "");
	if (!HasFmsTag(TrackInTag(L"CellCount")))
	{
		SetTrayLoadValidationError(true,
			"Location1 TrackIn validation failed: missing TrackInCellInformation.CellCount.");
		if(LogFailure) LogOpcEvent("VALIDATION FAIL missing TrackInCellInformation.CellCount", false);
		return false;
	}

	int Count = GetFmsInt(TrackInTag(L"CellCount"));
	// CellCount is the number of TrackIn array records, not the number of NG cells.
	// Zero means that the Source tray data has not been supplied and must never
	// be accepted as a valid empty/good tray.
	if (Count <= 0 || Count > 96)
	{
		SetTrayLoadValidationError(true,
			"Location1 TrackIn validation failed: CellCount=" + IntToStr(Count) +
			". Expected CellCount=1..96.");
		if(LogFailure)
			LogOpcEvent("VALIDATION FAIL TrackInCellInformation.CellCount / EXPECTED=1..96 / CURRENT=" +
				IntToStr(Count), true);
		return false;
	}

	for (int i = 0; i < Count; ++i)
	{
		UnicodeString Prefix = L"TrackInCellInformation.Cell[" + IntToStr(i) + L"]";
		if (!HasFmsTag(CellTag(TAG_SOURCE + L".TrackInCellInformation", i, L"CellId")) ||
			!HasFmsTag(CellTag(TAG_SOURCE + L".TrackInCellInformation", i, L"CellNo")) ||
			!HasFmsTag(CellTag(TAG_SOURCE + L".TrackInCellInformation", i, L"CellExist")) ||
			!HasFmsTag(CellTag(TAG_SOURCE + L".TrackInCellInformation", i, L"WorkFlag")) ||
			!HasFmsTag(CellTag(TAG_SOURCE + L".TrackInCellInformation", i, L"LotId")) ||
			!HasFmsTag(CellTag(TAG_SOURCE + L".TrackInCellInformation", i, L"Grade")) ||
			!HasFmsTag(CellTag(TAG_SOURCE + L".TrackInCellInformation", i, L"NGCode")))
		{
			SetTrayLoadValidationError(true,
				"Location1 TrackIn validation failed: required tag missing at Cell[" +
				IntToStr(i) + "]. Required tags: CellId, CellNo, CellExist, "
				"WorkFlag, LotId, Grade, NGCode.");
			if(LogFailure) LogOpcEvent("VALIDATION FAIL missing " + AnsiString(Prefix), false);
			return false;
		}

		int CellNo = GetFmsInt(CellTag(TAG_SOURCE + L".TrackInCellInformation", i, L"CellNo"));
		bool CellExist = GetFmsBool(CellTag(TAG_SOURCE + L".TrackInCellInformation", i, L"CellExist"));
		UnicodeString CellId = GetFmsString(CellTag(TAG_SOURCE + L".TrackInCellInformation", i, L"CellId")).Trim();

		// FMS can keep CellCount at the fixed tray capacity while unused records
		// remain zero/empty. Those records represent empty cells, not bad data.
		if (!CellExist && CellNo == 0 && CellId.IsEmpty())
			continue;

		if (CellNo < 1 || CellNo > 96)
		{
			SetTrayLoadValidationError(true,
				"Location1 TrackIn validation failed: Cell[" + IntToStr(i) +
				"] CellNo=" + IntToStr(CellNo) + " CellExist=" +
				AnsiString(CellExist ? "true" : "false") + " CellId=" +
				(CellId.IsEmpty() ? AnsiString("<empty>") : AnsiString(CellId)) +
				". Expected CellNo=1..96.");
			if(LogFailure) LogOpcEvent("VALIDATION FAIL " + AnsiString(Prefix) + ".CellNo=" + IntToStr(CellNo), false);
			return false;
		}

		if (CellExist && CellId.IsEmpty())
		{
			SetTrayLoadValidationError(true,
				"Location1 TrackIn validation failed: Cell[" + IntToStr(i) +
				"] CellExist=true but CellId is empty.");
			if(LogFailure) LogOpcEvent("VALIDATION FAIL empty " + AnsiString(Prefix) + ".CellId", false);
			return false;
		}
	}

	return true;
}
//---------------------------------------------------------------------------
static bool ValidateTargetTrackInCells(bool LogFailure)
{
	SetTrayLoadValidationError(false, "");
	UnicodeString Root = TAG_TARGET + L".TrackInCellInformation";
	UnicodeString CountKey = TrackInTag(TAG_TARGET, L"CellCount");
	if (!HasFmsTag(CountKey))
	{
		SetTrayLoadValidationError(false,
			"Location2 TrackIn validation failed: missing TrackInCellInformation.CellCount.");
		if(LogFailure)
			LogOpcEvent(GTrayLoadValidationError[1], true);
		return false;
	}

	int Count = GetFmsInt(CountKey);
	// An empty Target tray can legitimately have zero TrackIn records.
	if (Count < 0 || Count > 96)
	{
		SetTrayLoadValidationError(false,
			"Location2 TrackIn validation failed: CellCount=" + IntToStr(Count) +
			". Expected CellCount=0..96.");
		if(LogFailure)
			LogOpcEvent(GTrayLoadValidationError[1], true);
		return false;
	}

	for (int Record = 0; Record < Count; ++Record)
	{
		UnicodeString Prefix = L"Location2.TrackInCellInformation.Cell[" +
			IntToStr(Record) + L"]";
		if (!HasFmsTag(CellTag(Root, Record, L"CellId")) ||
			!HasFmsTag(CellTag(Root, Record, L"CellNo")) ||
			!HasFmsTag(CellTag(Root, Record, L"CellExist")) ||
			!HasFmsTag(CellTag(Root, Record, L"WorkFlag")) ||
			!HasFmsTag(CellTag(Root, Record, L"LotId")) ||
			!HasFmsTag(CellTag(Root, Record, L"Grade")) ||
			!HasFmsTag(CellTag(Root, Record, L"NGCode")))
		{
			SetTrayLoadValidationError(false,
				"Location2 TrackIn validation failed: required tag missing at Cell[" +
				IntToStr(Record) + "]. Required tags: CellId, CellNo, CellExist, "
				"WorkFlag, LotId, Grade, NGCode.");
			if(LogFailure) LogOpcEvent("VALIDATION FAIL missing " + AnsiString(Prefix), true);
			return false;
		}

		int CellNo = GetFmsInt(CellTag(Root, Record, L"CellNo"));
		bool CellExist = GetFmsBool(CellTag(Root, Record, L"CellExist"));
		UnicodeString CellId = GetFmsString(CellTag(Root, Record, L"CellId")).Trim();
		if (!CellExist && CellNo == 0 && CellId.IsEmpty())
			continue;
		if (CellNo < 1 || CellNo > 96 || (CellExist && CellId.IsEmpty()))
		{
			UnicodeString LotId = GetFmsString(CellTag(Root, Record, L"LotId")).Trim();
			UnicodeString Grade = GetFmsString(CellTag(Root, Record, L"Grade")).Trim();
			UnicodeString NGCode = GetFmsString(CellTag(Root, Record, L"NGCode")).Trim();
			bool WorkFlag = GetFmsBool(CellTag(Root, Record, L"WorkFlag"));
			AnsiString Detail =
				"Location2 TrackIn validation failed\r\n"
				"Array Index : " + IntToStr(Record) + "\r\n" +
				"CellCount   : " + IntToStr(Count) + "\r\n" +
				"CellNo      : " + IntToStr(CellNo) + "\r\n" +
				"CellExist   : " + AnsiString(CellExist ? "true" : "false") + "\r\n" +
				"CellId      : " + (CellId.IsEmpty() ? AnsiString("<empty>") : AnsiString(CellId)) + "\r\n" +
				"LotId       : " + (LotId.IsEmpty() ? AnsiString("<empty>") : AnsiString(LotId)) + "\r\n" +
				"Grade       : " + (Grade.IsEmpty() ? AnsiString("<empty>") : AnsiString(Grade)) + "\r\n" +
				"NGCode      : " + (NGCode.IsEmpty() ? AnsiString("<empty>") : AnsiString(NGCode)) + "\r\n" +
				"WorkFlag    : " + AnsiString(WorkFlag ? "true" : "false") + "\r\n" +
				"Expected    : CellNo=1..96; CellId required when CellExist=true.";
			SetTrayLoadValidationError(false, Detail);
			if(LogFailure)
				LogOpcEvent(Detail, true);
			return false;
		}
	}
	return true;
}
//---------------------------------------------------------------------------
static int CountExistingTrayCells(TRAY_INFO *Tray)
{
	if (Tray == NULL)
		return 0;

	int Count = 0;
	for (int Channel = 0; Channel < 96; ++Channel)
	{
		if (Tray->CELL_EXIST[Channel])
			++Count;
	}
	return Count;
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
		Tray->CELL_EXIST[i] = false;
		Tray->WORK_FLAG[i] = false;
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
	int RecordCount = GetFmsInt(TrackInTag(L"CellCount"));
	// The physical Source tray always has 96 channels. FMS array order is not
	// guaranteed to match the channel, so every record is mapped by CellNo.
	Tray->SLOT_COUNT = 96;
	Tray->empTray = true;
	for (int Channel = 0; Channel < 96; ++Channel)
	{
		Tray->SLOT_POSITION[Channel] = IntToStr(Channel + 1);
		Tray->SLOT_ID[Channel] = "";
		Tray->CELL_LOT_ID[Channel] = "";
		Tray->CELL_EXIST[Channel] = false;
		Tray->WORK_FLAG[Channel] = false;
		Tray->LOSS_CD[Channel] = "";
		Tray->RANK[Channel] = "";
		Tray->PICK[Channel] = "N";
	}

	for (int Record = 0; Record < RecordCount && Record < 96; ++Record)
	{
		int CellNo = GetFmsInt(CellTag(Root, Record, L"CellNo"));
		if (CellNo < 1 || CellNo > 96)
			continue;

		int Channel = CellNo - 1;
		bool CellExist = GetFmsBool(CellTag(Root, Record, L"CellExist"));
		UnicodeString Grade = GetFmsString(CellTag(Root, Record, L"Grade")).Trim();
		Tray->CELL_EXIST[Channel] = CellExist;
		Tray->WORK_FLAG[Channel] = GetFmsBool(CellTag(Root, Record, L"WorkFlag"));
		if (CellExist)
			Tray->empTray = false;

		Tray->SLOT_ID[Channel] = AnsiString(
			GetFmsString(CellTag(Root, Record, L"CellId")).Trim());
		Tray->CELL_LOT_ID[Channel] = AnsiString(GetFmsString(CellTag(Root, Record, L"LotId")));
		Tray->LOSS_CD[Channel] = AnsiString(GetFmsString(CellTag(Root, Record, L"NGCode"))).Trim();
		Tray->RANK[Channel] = AnsiString(Grade);

		// NG definition: the cell physically exists and Grade is exactly NG.
		// NGCode is the defect description only and never decides whether a cell is NG.
		Tray->PICK[Channel] = (CellExist && Grade.UpperCase() == L"NG") ? "Y" : "N";
	}
}
//---------------------------------------------------------------------------
static void ApplyTargetTrackInCells(TRAY_INFO *Tray)
{
	if (Tray == NULL)
		return;

	UnicodeString Root = TAG_TARGET + L".TrackInCellInformation";
	int RecordCount = GetFmsInt(TrackInTag(TAG_TARGET, L"CellCount"));
	Tray->SLOT_COUNT = 96;
	Tray->empTray = true;
	Tray->remainCnt = 96;
	for (int Channel = 0; Channel < 96; ++Channel)
	{
		Tray->SLOT_POSITION[Channel] = IntToStr(Channel + 1);
		Tray->SLOT_ID[Channel] = "";
		Tray->CELL_LOT_ID[Channel] = "";
		Tray->CELL_EXIST[Channel] = false;
		Tray->WORK_FLAG[Channel] = false;
		Tray->LOSS_CD[Channel] = "";
		Tray->RANK[Channel] = "";
		Tray->PICK[Channel] = "N";
	}

	for (int Record = 0; Record < RecordCount && Record < 96; ++Record)
	{
		int CellNo = GetFmsInt(CellTag(Root, Record, L"CellNo"));
		if (CellNo < 1 || CellNo > 96)
			continue;

		int Channel = CellNo - 1;
		bool CellExist = GetFmsBool(CellTag(Root, Record, L"CellExist"));
		Tray->SLOT_ID[Channel] = AnsiString(
			GetFmsString(CellTag(Root, Record, L"CellId")).Trim());
		Tray->CELL_LOT_ID[Channel] = AnsiString(
			GetFmsString(CellTag(Root, Record, L"LotId")));
		Tray->CELL_EXIST[Channel] = CellExist;
		Tray->WORK_FLAG[Channel] = GetFmsBool(CellTag(Root, Record, L"WorkFlag"));
		Tray->LOSS_CD[Channel] = AnsiString(
			GetFmsString(CellTag(Root, Record, L"NGCode")).Trim());
		Tray->RANK[Channel] = AnsiString(
			GetFmsString(CellTag(Root, Record, L"Grade")).Trim());
		// PICK=R is equipment-only transient state and never comes from FMS.
		Tray->PICK[Channel] = CellExist ? "Y" : "N";
		if (CellExist)
		{
			Tray->empTray = false;
			--Tray->remainCnt;
		}
	}
}
//---------------------------------------------------------------------------
static bool MergeTargetSavedCells(TRAY_INFO *Target,
	const SAVE_TRAY_INFO &Saved, const UnicodeString &TargetTrayId)
{
	if (Target == NULL)
		return false;

	if (!SameText(Saved.LOT_ID.Trim(), AnsiString(TargetTrayId).Trim()))
		return true;

	int CompletedMerged = 0;
	int ReservationsCleared = 0;
	for (int Channel = 0; Channel < 96; ++Channel)
	{
		// INIT WORK/reload must never restore an unfinished reservation.
		if (Saved.PICK[Channel] == "R" && !Saved.CELL_EXIST[Channel])
		{
			++ReservationsCleared;
			continue;
		}
		if (!Saved.CELL_EXIST[Channel] || Saved.SLOT_ID[Channel].IsEmpty())
			continue;

		if (Target->CELL_EXIST[Channel])
		{
			if (!Target->SLOT_ID[Channel].IsEmpty() &&
				Target->SLOT_ID[Channel] != Saved.SLOT_ID[Channel])
			{
				LogOpcEvent("TARGET TRACKIN/LOCAL CONFLICT Channel=" +
					IntToStr(Channel + 1) + " FmsCellId=" +
					Target->SLOT_ID[Channel] + " LocalCellId=" +
					Saved.SLOT_ID[Channel], true);
				SetTrayLoadValidationError(false,
					"Location2 TrackIn/local data conflict\r\nChannel      : " +
					IntToStr(Channel + 1) + "\r\nFMS CellId   : " +
					Target->SLOT_ID[Channel] + "\r\nLocal CellId : " +
					Saved.SLOT_ID[Channel]);
				return false;
			}
			continue;
		}

		// Preserve cells physically inserted during this Source cycle. FMS
		// Location2 TrackIn remains the initial snapshot until final TrackOut.
		Target->SLOT_POSITION[Channel] = IntToStr(Channel + 1);
		Target->SLOT_ID[Channel] = Saved.SLOT_ID[Channel];
		Target->CELL_LOT_ID[Channel] = Saved.CELL_LOT_ID[Channel];
		Target->CELL_EXIST[Channel] = true;
		Target->WORK_FLAG[Channel] = Saved.WORK_FLAG[Channel];
		Target->LOSS_CD[Channel] = Saved.LOSS_CD[Channel];
		Target->RANK[Channel] = Saved.RANK[Channel];
		Target->PICK[Channel] = "Y";
		++CompletedMerged;
	}

	Target->remainCnt = 0;
	Target->empTray = true;
	for (int Channel = 0; Channel < 96; ++Channel)
	{
		if (Target->CELL_EXIST[Channel])
			Target->empTray = false;
		else if (Target->PICK[Channel] == "N")
			++Target->remainCnt;
	}
	LogOpcEvent("TARGET WORKING MAP READY CompletedMerged=" +
		IntToStr(CompletedMerged) + " ReservationsCleared=" +
		IntToStr(ReservationsCleared) + " Remaining=" +
		IntToStr(Target->remainCnt), true);
	return true;
}
//---------------------------------------------------------------------------
static void ApplyTrayDisplay(TRAY_INFO *Tray, const UnicodeString &TrayId,
	const UnicodeString &ProductModel, const UnicodeString &RouteId,
	const UnicodeString &ProcessId, const UnicodeString &LotId, int CellCount)
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
		TrayIdPanel->Caption = TrayId;

	if (Tray == &MainForm->tray_source)
	{
		MainForm->pKIND->Caption = ProductModel;
		MainForm->pDATE->Caption = RouteId;
		MainForm->pPROCESS->Caption = ProcessId;
		MainForm->pOPER->Caption = LotId;
		MainForm->pSLOT_COUNT->Caption = IntToStr(CellCount);
	}
	else
	{
		MainForm->pKIND_target->Caption = ProductModel;
		MainForm->pBYPASS->Caption = RouteId;
		MainForm->pPROCESS_target->Caption = ProcessId;
		MainForm->pDATE_target->Caption = LotId;
		MainForm->pSLOT_COUNT_target->Caption = IntToStr(CellCount);
	}
}
//---------------------------------------------------------------------------
__fastcall TMesOpc::TMesOpc(TComponent* Owner)
	: TDataModule(Owner),
	  FShutdown(false)
{
	FTrayLoadResponseRevision[0] = 0;
	FTrayLoadResponseRevision[1] = 0;
	FTrayLoadWaitResponseIdle[0] = false;
	FTrayLoadWaitResponseIdle[1] = false;
	FProcessStartResponseRevision = 0;
	FProcessStartWaitResponseIdle = false;
	FProcessEndResponseRevision = 0;
	FProcessEndWaitResponseIdle = false;
	FCellTrackOutResponseRevision = 0;
	FCellTrackOutWaitResponseIdle = false;
	FTrayUnloadResponseRevision = 0;
	FTrayUnloadWaitResponseIdle = false;
}
//---------------------------------------------------------------------------
AnsiString __fastcall TMesOpc::TRAY_LOAD_VALIDATION_ERROR(bool SourceTray) const
{
	return GTrayLoadValidationError[SourceTray ? 0 : 1];
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::Shutdown()
{
	if(FShutdown)
		return;
	FShutdown = true;

	// Gracefully clear every outstanding EQP request before the Gateway stops.
	SetPcBool(TrayProcessTag(TAG_SOURCE, L"TrayLoad"), false);
	SetPcBool(TrayProcessTag(TAG_TARGET, L"TrayLoad"), false);
	SetPcBool(TrayProcessTag(TAG_SOURCE, L"ProcessStart"), false);
	SetPcBool(TrayProcessTag(TAG_SOURCE, L"ProcessEnd"), false);
	SetPcBool(CellTrackOutTag(L"CellUnloadComplete"), false);
	SetPcBool(TrayProcessTag(TAG_TARGET, L"TrayUnloadRequest"), false);
	if(Mod_Fms != NULL)
		Mod_Fms->FlushPendingPcTags(false);
}
//---------------------------------------------------------------------------
static void AddDisplayBadCode(const AnsiString &Code)
{
	if(MainForm == NULL || MainForm->badList == NULL)
		return;

	int FoundIndex = -1;
	for(int i = 0; i < MainForm->badList->Items->Count; ++i)
	{
		TListItem *Item = MainForm->badList->Items->Item[i];
		if(Item != NULL && Item->SubItems->Count > 0 &&
			Item->SubItems->Strings[0] == Code)
		{
			FoundIndex = i;
			break;
		}
	}

	if(FoundIndex < 0)
	{
		TListItem *Item = MainForm->badList->Items->Add();
		Item->Caption = MainForm->badList->Items->Count;
		Item->SubItems->Add(Code);
		Item->SubItems->Add(1);
	}
	else
	{
		TListItem *Item = MainForm->badList->Items->Item[FoundIndex];
		Item->SubItems->Strings[1] =
			Item->SubItems->Strings[1].ToIntDef(0) + 1;
	}

	MainForm->pbad_sum->Caption = MainForm->pbad_sum->Caption.ToIntDef(0) + 1;
}
//---------------------------------------------------------------------------
bool __fastcall TMesOpc::DISPLAY_TRACK_IN_TRAYS()
{
	if(MainForm == NULL || Mod_Fms == NULL || !ValidateSourceTrackInCells(true))
		return false;

	TRAY_INFO *Source = &MainForm->tray_source;
	ClearTrayCells(Source);
	ApplySourceTrackInCells(Source);
	Source->PASS = "N";
	Source->startTime = Now();

	UnicodeString ProductModel = GetFmsString(TrayInfoTag(TAG_SOURCE, L"ProductModel"));
	UnicodeString RouteId = GetFmsString(TrayInfoTag(TAG_SOURCE, L"RouteId"));
	UnicodeString ProcessId = GetFmsString(TrayInfoTag(TAG_SOURCE, L"ProcessId"));
	UnicodeString LotId = GetFmsString(TrayInfoTag(TAG_SOURCE, L"LotId"));
	UnicodeString SourceTrayId = GetFmsString(TrayInfoTag(TAG_SOURCE, L"TrayId")).Trim();
	if(SourceTrayId.IsEmpty())
		SourceTrayId = Mod_Fms->GetPcTagString(
			TrayInfoTag(TAG_SOURCE, L"TrayId"), L"").Trim();
	if(SourceTrayId.IsEmpty())
		SourceTrayId = MainForm->pTrayid_source->Caption.Trim();
	if(!SourceTrayId.IsEmpty())
		MainForm->pTrayid_source->Caption = SourceTrayId;
	int SourceExistCount = CountExistingTrayCells(Source);
	ApplyTrayDisplay(Source, SourceTrayId, ProductModel, RouteId,
		ProcessId, LotId, SourceExistCount);

	MainForm->pbad_sum->Caption = "0";
	MainForm->badList->Clear();
	Source->remainCnt = 0;
	for(int i = 0; i < 96; ++i)
	{
		MainForm->psort_bad[i]->Color = clWhite;
		MainForm->psort_ing[i]->Color = clWhite;
		if(i >= Source->SLOT_COUNT)
		{
			MainForm->psort_bad[i]->Caption = "";
			MainForm->psort_ing[i]->Caption = "";
			continue;
		}

		bool CellExist = Source->CELL_EXIST[i];
		MainForm->psort_bad[i]->Caption =
			(CellExist && Source->PICK[i] == "Y") ? Source->LOSS_CD[i] : AnsiString("");
		if(CellExist && Source->PICK[i] == "Y")
		{
			++Source->remainCnt;
			MainForm->psort_ing[i]->Caption = "NG";
			AddDisplayBadCode(Source->LOSS_CD[i]);
		}
		else
			MainForm->psort_ing[i]->Caption = CellExist ? "**" : "";
	}

	// Location2 TrackIn is the authoritative initial snapshot. Merge only
	// completed local inserts; transient PICK=R reservations are discarded.
	TRAY_INFO *Target = &MainForm->tray_target;
	if(!ValidateTargetTrackInCells(true))
		return false;
	SAVE_TRAY_INFO SavedTarget = MainForm->m_saveTrayInfo[1];
	UnicodeString TargetTrayId = GetFmsString(TrayInfoTag(TAG_TARGET, L"TrayId")).Trim();
	if(TargetTrayId.IsEmpty())
		TargetTrayId = Mod_Fms->GetPcTagString(
			TrayInfoTag(TAG_TARGET, L"TrayId"), L"").Trim();
	if(TargetTrayId.IsEmpty())
		TargetTrayId = MainForm->pTrayid_target->Caption.Trim();
	ClearTrayCells(Target);
	ApplyTargetTrackInCells(Target);
	if(!MergeTargetSavedCells(Target, SavedTarget, TargetTrayId))
		return false;
	MainForm->setTrayInfo(1);
	int TargetRecordCount = GetFmsInt(TrackInTag(TAG_TARGET, L"CellCount"));
	int TargetExistCount = CountExistingTrayCells(Target);
	ApplyTrayDisplay(Target, TargetTrayId, ProductModel, RouteId,
		ProcessId, LotId, TargetExistCount);

	if(Target->SLOT_COUNT < 1 || Target->SLOT_COUNT > 96)
		Target->SLOT_COUNT = 96;
	Target->remainCnt = 0;
	for(int i = 0; i < 96; ++i)
	{
		if(i < Target->SLOT_COUNT &&
			(Target->PICK[i] == "R" || Target->PICK[i] == "Y" ||
			 !Target->SLOT_ID[i].IsEmpty()))
		{
			MainForm->DisplayTargetCell(-1, i);
			MainForm->DisplayTargetCellInfo(-1, i);
		}
		else
		{
			if(i < Target->SLOT_COUNT)
				++Target->remainCnt;
			MainForm->color_target[i / 24][23 - (i % 24)] = clWhite;
			MainForm->targetGrid->Cells[i / 24][23 - (i % 24)] = "";
			MainForm->pTarget_bad[i]->Caption = "";
			MainForm->pTarget_bad[i]->Color = clWhite;
		}
	}
	MainForm->targetGrid->Invalidate();
	MainForm->tray = &MainForm->tray_target;

	LogOpcEvent("DISPLAY TRAY Source CellExistCount=" +
		IntToStr(SourceExistCount) + " Empty=" +
		AnsiString(Source->empTray ? "true" : "false") +
		" / Target TrackIn CellCount=" + IntToStr(TargetRecordCount) +
		" CellExistCount=" + IntToStr(TargetExistCount), true);
	return true;
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

	// TrayLoad uses the current response value directly. The previous transaction
	// is already guaranteed to have completed Response=0 before advancing.
	// Rejecting an unchanged Revision hid a visible Location2 Response=1.
	int LocationIndex = SourceTray ? 0 : 1;
	SetTrayLoadValidationError(SourceTray, "");
	UnicodeString RequestKey = TrayProcessTag(Location, L"TrayLoad");
	UnicodeString ResponseKey = TrayProcessTag(Location, L"TrayLoadResponse");
	int InitialResponse = GetFmsInt(ResponseKey);
	FTrayLoadResponseRevision[LocationIndex] = Mod_Fms != NULL ?
		Mod_Fms->GetFmsTagRevision(ResponseKey) : 0;
	FTrayLoadWaitResponseIdle[LocationIndex] = false;
	SetPcBool(TrayInfoTag(Location, L"TrayExist"), true);
	SetPcString(TrayInfoTag(Location, L"TrayId"), TrayId);
	SetPcBool(RequestKey, true);
	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
	LogOpcEvent("TRAY_LOAD_REQUEST " + AnsiString(Location) + " TrayId=" + AnsiString(TrayId) +
		" InitialResponse=" + IntToStr(InitialResponse) +
		" Revision=" + IntToStr((__int64)FTrayLoadResponseRevision[LocationIndex]) +
		" / Request=ON / current Response value is accepted", true);
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::TRAY_LOAD_CANCEL(bool SourceTray)
{
	UnicodeString Location = LocationFor(SourceTray);
	SetPcBool(TrayProcessTag(Location, L"TrayLoad"), false);
	FTrayLoadWaitResponseIdle[SourceTray ? 0 : 1] = false;
	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
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
	int LocationIndex = SourceTray ? 0 : 1;
	unsigned __int64 CurrentRevision = Mod_Fms != NULL ? Mod_Fms->GetFmsTagRevision(ResponseKey) : 0;

	int Response = GetFmsInt(ResponseKey);
	// Read the current response directly. Revision remains diagnostic only.
	if (Response == 0)
		return 0;
	if (Response != 1 && Response != 2)
	{
		LogOpcEvent("VALIDATION FAIL TrayLoadResponse=" + IntToStr(Response), true);
		SetPcBool(TrayProcessTag(Location, L"TrayLoad"), false);
		if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
		FTrayLoadResponseRevision[LocationIndex] = CurrentRevision;
		return -1;
	}
	if (Response == 2)
	{
		LogOpcEvent("TRAY_LOAD_RESPONSE FAIL " + AnsiString(Location), true);
		SetPcBool(TrayProcessTag(Location, L"TrayLoad"), false);
		if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
		FTrayLoadResponseRevision[LocationIndex] = CurrentRevision;
		return 2;
	}

	TRAY_INFO *Tray = TrayFor(SourceTray);
	if (Tray != NULL)
	{
		UnicodeString TrayId;
		UnicodeString ProductModel;
		UnicodeString RouteId;
		UnicodeString ProcessId;
		UnicodeString LotId;
		int DisplayCellCount = 0;
		SAVE_TRAY_INFO SavedTarget;
		if (!SourceTray && MainForm != NULL)
			SavedTarget = MainForm->m_saveTrayInfo[1];

		if (SourceTray)
		{
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

			TrayId = GetFmsString(TrayInfoTag(Location, L"TrayId")).Trim();
			if(TrayId.IsEmpty() && MainForm != NULL)
				TrayId = MainForm->pTrayid_source->Caption.Trim();
		}
		else
		{
			// Target metadata follows the Source tray. TrayId and the complete
			// initial cell map are read from Location2 TrackInCellInformation.
			if (MainForm != NULL)
			{
				ProductModel = UnicodeString(MainForm->tray_source.KIND);
				RouteId = MainForm->pDATE->Caption.Trim();
				ProcessId = UnicodeString(MainForm->tray_source.WORK_CODE);
				LotId = MainForm->pOPER->Caption.Trim();
			}
			TrayId = GetFmsString(TrayInfoTag(Location, L"TrayId")).Trim();
			TPanel *TrayIdPanel = TrayIdPanelFor(false);
			if (TrayId.IsEmpty() && TrayIdPanel != NULL)
				TrayId = TrayIdPanel->Caption.Trim();
			if (TrayId.IsEmpty() || !ValidateTargetTrackInCells(false))
				return 0;
		}

		ClearTrayCells(Tray);
		if (SourceTray)
		{
			ApplySourceTrackInCells(Tray);
			Tray->PASS = "N";
			DisplayCellCount = CountExistingTrayCells(Tray);
		}
		else
		{
			ApplyTargetTrackInCells(Tray);
			if (!MergeTargetSavedCells(Tray, SavedTarget, TrayId))
			{
				SetPcBool(TrayProcessTag(Location, L"TrayLoad"), false);
				if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
				return -1;
			}
			DisplayCellCount = CountExistingTrayCells(Tray);
			// Save the merged working snapshot immediately. The subsequent display
			// restore therefore cannot resurrect stale PICK=R reservations.
			if (MainForm != NULL)
				MainForm->setTrayInfo(1);
		}

		Tray->TRAY_GUBUN = IntToStr(Tray->SLOT_COUNT);
		ApplyTrayDisplay(Tray, TrayId, ProductModel, RouteId,
			ProcessId, LotId, DisplayCellCount);
	}

	SetPcBool(TrayProcessTag(Location, L"TrayLoad"), false);
	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
	FTrayLoadResponseRevision[LocationIndex] = CurrentRevision;
	LogOpcEvent("TRAY_LOAD_RESPONSE SUCCESS " + AnsiString(Location));
	return 1;
}
//---------------------------------------------------------------------------
int __fastcall TMesOpc::TRAY_LOAD_RESPONSE_VALUE(bool SourceTray)
{
	if(Mod_Fms == NULL)
		return -1;

	UnicodeString RawValue;
	if(!Mod_Fms->GetFmsTagJson(
		TrayProcessTag(LocationFor(SourceTray), L"TrayLoadResponse"), RawValue))
		return -1;
	return StrToIntDef(RawValue.Trim(), -1);
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::LogTrayLoadTimeout(bool SourceTray)
{
	UnicodeString Location = LocationFor(SourceTray);
	UnicodeString ResponseJson;
	bool HasResponse = Mod_Fms != NULL &&
		Mod_Fms->GetFmsTagJson(TrayProcessTag(Location, L"TrayLoadResponse"), ResponseJson);
	int LocationIndex = SourceTray ? 0 : 1;
	unsigned __int64 CurrentRevision = Mod_Fms != NULL ?
		Mod_Fms->GetFmsTagRevision(TrayProcessTag(Location, L"TrayLoadResponse")) : 0;
	AnsiString Message = "TRAY_LOAD_TIMEOUT " + AnsiString(Location) +
		" TrayLoadResponse=" + (HasResponse ? AnsiString(ResponseJson) : AnsiString("<missing>")) +
		" Revision=" + IntToStr((__int64)CurrentRevision) +
		" RequestRevision=" + IntToStr((__int64)FTrayLoadResponseRevision[LocationIndex]) +
		" UpdatedAfterRequest=" + AnsiString(CurrentRevision > FTrayLoadResponseRevision[LocationIndex] ? "true" : "false");
	Message += " TrackInCellInformation.CellCount=" +
		IntToStr(GetFmsInt(TrackInTag(Location, L"CellCount")));

	if (MainForm != NULL)
		MainForm->WriteOpcUaLog("ERROR", Message, true);

	// Emit the exact missing/invalid source field only once, at timeout.
	if (SourceTray && GetFmsInt(TrayProcessTag(Location, L"TrayLoadResponse")) == 1)
	{
		UnicodeString Value;
		ValidateSourceTrackInCells(true);
		ReadRequiredString(TrayInfoTag(Location, L"ProductModel"), Value, "TrayInformation.ProductModel", true);
		ReadRequiredString(TrayInfoTag(Location, L"RouteId"), Value, "TrayInformation.RouteId", true);
		ReadRequiredString(TrayInfoTag(Location, L"ProcessId"), Value, "TrayInformation.ProcessId", true);
		ReadRequiredString(TrayInfoTag(Location, L"LotId"), Value, "TrayInformation.LotId", true);
	}
	else if (!SourceTray && GetFmsInt(TrayProcessTag(Location, L"TrayLoadResponse")) == 1)
	{
		ValidateTargetTrackInCells(true);
	}
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::LogTrayLoadResponseOffTimeout(bool SourceTray)
{
	UnicodeString Location = LocationFor(SourceTray);
	UnicodeString ResponseKey = TrayProcessTag(Location, L"TrayLoadResponse");
	UnicodeString RequestJson;
	UnicodeString ResponseJson;
	bool HasRequest = Mod_Fms != NULL &&
		Mod_Fms->GetPcTagJson(TrayProcessTag(Location, L"TrayLoad"), RequestJson);
	bool HasResponse = Mod_Fms != NULL &&
		Mod_Fms->GetFmsTagJson(ResponseKey, ResponseJson);
	TPanel *TrayIdPanel = TrayIdPanelFor(SourceTray);
	AnsiString TrayId = "";
	if(TrayIdPanel != NULL)
		TrayId = AnsiString(TrayIdPanel->Caption);
	AnsiString Message = "TRAY_LOAD_RESPONSE_OFF_TIMEOUT " + AnsiString(Location) +
		" TrayId=" + TrayId +
		" TrayLoad=" + (HasRequest ? AnsiString(RequestJson) : AnsiString("<missing>")) +
		" TrayLoadResponse=" + (HasResponse ? AnsiString(ResponseJson) : AnsiString("<missing>")) +
		" WaitMs=10000";
	if(MainForm != NULL)
		MainForm->WriteOpcUaLog("ERROR", Message, true);
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
	const UnicodeString RequestKey = TrayProcessTag(TAG_SOURCE, L"ProcessStart");
	const UnicodeString ResponseKey = TrayProcessTag(TAG_SOURCE, L"ProcessStartResponse");
	int InitialResponse = GetFmsInt(ResponseKey);
	FProcessStartResponseRevision = Mod_Fms != NULL ? Mod_Fms->GetFmsTagRevision(ResponseKey) : 0;
	FProcessStartWaitResponseIdle = !IsCycleResponseBypass() && (InitialResponse != 0);
	SetPcBool(RequestKey, !FProcessStartWaitResponseIdle);
	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
	LogOpcEvent("PROCESS_START_REQUEST InitialResponse=" + IntToStr(InitialResponse) +
		" BaselineRevision=" + IntToStr((__int64)FProcessStartResponseRevision) +
		(FProcessStartWaitResponseIdle ?
			AnsiString(" / hold Request=OFF until stale Response=0") :
			AnsiString(" / Request=ON")));
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::PROCESS_START_CANCEL()
{
	SetPcBool(TrayProcessTag(TAG_SOURCE, L"ProcessStart"), false);
	FProcessStartWaitResponseIdle = false;
	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
	LogOpcEvent("PROCESS_START_CANCEL Request=OFF");
}
//---------------------------------------------------------------------------
bool __fastcall TMesOpc::PROCESS_START_RESPONSE()
{
	return PROCESS_START_RESPONSE_RESULT() == 1;
}
//---------------------------------------------------------------------------
int __fastcall TMesOpc::PROCESS_START_RESPONSE_RESULT()
{
	const UnicodeString RequestKey = TrayProcessTag(TAG_SOURCE, L"ProcessStart");
	const UnicodeString ResponseKey = TrayProcessTag(TAG_SOURCE, L"ProcessStartResponse");
	int Response = GetFmsInt(ResponseKey);
	unsigned __int64 CurrentRevision = Mod_Fms != NULL ? Mod_Fms->GetFmsTagRevision(ResponseKey) : 0;
	if(FProcessStartWaitResponseIdle)
	{
		if(Response != 0) return 0;
		FProcessStartWaitResponseIdle = false;
		FProcessStartResponseRevision = CurrentRevision;
		SetPcBool(RequestKey, true);
		if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
		LogOpcEvent("PROCESS_START_RESPONSE IDLE confirmed / Request=ON / BaselineRevision=" +
			IntToStr((__int64)FProcessStartResponseRevision));
		return 3;
	}
	if(Response == 0)
		return 0;
	if(!IsCycleResponseBypass() && CurrentRevision <= FProcessStartResponseRevision)
		return 0;

	// Clear the PC request after either ACK result. Completion is handled only
	// after the FMS response also returns to zero.
	SetPcBool(RequestKey, false);
	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
	if(Response == 1){
		LogOpcEvent("PROCESS_START_RESPONSE=1 / Request=OFF / wait Response=0");
		return 1;
	}
	if(Response == 2){
		LogOpcEvent("PROCESS_START_RESPONSE=2 / Request=OFF / wait Response=0", true);
		return 2;
	}

	LogOpcEvent("VALIDATION FAIL ProcessStartResponse=" + IntToStr(Response), true);
	return -1;
}
//---------------------------------------------------------------------------
int __fastcall TMesOpc::PROCESS_START_RESPONSE_VALUE()
{
	if(Mod_Fms == NULL) return -1;
	UnicodeString RawValue;
	if(!Mod_Fms->GetFmsTagJson(
		TrayProcessTag(TAG_SOURCE, L"ProcessStartResponse"), RawValue))
		return -1;
	return StrToIntDef(RawValue.Trim(), -1);
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::LogProcessStartResponseOffTimeout()
{
	const UnicodeString RequestKey = TrayProcessTag(TAG_SOURCE, L"ProcessStart");
	const UnicodeString ResponseKey = TrayProcessTag(TAG_SOURCE, L"ProcessStartResponse");
	UnicodeString RequestJson, ResponseJson;
	bool HasRequest = Mod_Fms != NULL && Mod_Fms->GetPcTagJson(RequestKey, RequestJson);
	bool HasResponse = Mod_Fms != NULL && Mod_Fms->GetFmsTagJson(ResponseKey, ResponseJson);
	if(MainForm != NULL)
		MainForm->WriteOpcUaLog("ERROR", "PROCESS_START_RESPONSE_OFF_TIMEOUT Request=" +
			(HasRequest ? AnsiString(RequestJson) : AnsiString("<missing>")) +
			" Response=" + (HasResponse ? AnsiString(ResponseJson) : AnsiString("<missing>")) +
			" WaitMs=10000", true);
}
//---------------------------------------------------------------------------
bool __fastcall TMesOpc::READ_TRACK_IN_CELL(int SourceCellNo, AnsiString &CellId,
	AnsiString &LotId, AnsiString &NGCode, AnsiString &Grade)
{
	CellId = "";
	LotId = "";
	NGCode = "";
	Grade = "";
	if(Mod_Fms == NULL || SourceCellNo < 1 || SourceCellNo > 96)
		return false;

	// Resolve the actual source cell by CellNo, not by array index. Scan all
	// records so a stale or missing CellCount does not hide valid test data.
	UnicodeString Root = TAG_SOURCE + L".TrackInCellInformation";
	for(int Index = 0; Index < 96; ++Index)
	{
		UnicodeString CellRoot = CellRecordTag(Root, Index);
		UnicodeString CellNoJson;
		if(!Mod_Fms->GetFmsTagJson(CellRoot + L".CellNo", CellNoJson))
			continue;
		if(Mod_Fms->GetFmsTagInt(CellRoot + L".CellNo", 0) != SourceCellNo)
			continue;

		bool CellExist = Mod_Fms->GetFmsTagBool(CellRoot + L".CellExist", false);
		CellId = AnsiString(Mod_Fms->GetFmsTagString(CellRoot + L".CellId", L"").Trim());
		LotId = AnsiString(Mod_Fms->GetFmsTagString(CellRoot + L".LotId", L""));
		NGCode = AnsiString(Mod_Fms->GetFmsTagString(CellRoot + L".NGCode", L""));
		Grade = AnsiString(Mod_Fms->GetFmsTagString(CellRoot + L".Grade", L""));
		return CellExist && !CellId.IsEmpty();
	}

	// Commissioning fallback: some test data is entered by array position
	// before CellNo/CellCount is populated.
	UnicodeString CellRoot = CellRecordTag(Root, SourceCellNo - 1);
	CellId = AnsiString(Mod_Fms->GetFmsTagString(CellRoot + L".CellId", L"").Trim());
	LotId = AnsiString(Mod_Fms->GetFmsTagString(CellRoot + L".LotId", L""));
	NGCode = AnsiString(Mod_Fms->GetFmsTagString(CellRoot + L".NGCode", L""));
	Grade = AnsiString(Mod_Fms->GetFmsTagString(CellRoot + L".Grade", L""));
	if(!CellId.IsEmpty())
		return Mod_Fms->GetFmsTagBool(CellRoot + L".CellExist", false);

	return false;
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::PROCESS_DATA_WRITE()
{
	if (MainForm == NULL || Mod_Fms == NULL)
		return;

	TRAY_INFO *Tray = &MainForm->tray_target;
	UnicodeString Root = TAG_TARGET + L".TrackOutCellInformation";
	UnicodeString TargetTrayId = Mod_Fms->GetPcTagString(
		TrayInfoTag(TAG_TARGET, L"TrayId"), L"").Trim();

	// Full cumulative report: include every existing target cell restored from
	// the local tray file plus the cell that has just been inserted.
	int CellCount = 0;
	for (int TargetIndex = 0; TargetIndex < 96; ++TargetIndex)
	{
		if (!Tray->CELL_EXIST[TargetIndex] || Tray->SLOT_ID[TargetIndex].IsEmpty())
			continue;

		int OutputIndex = CellCount++;
		SetPcString(CellTag(Root, OutputIndex, L"CellId"), Tray->SLOT_ID[TargetIndex]);
		SetPcInt(CellTag(Root, OutputIndex, L"CellNo"), TargetIndex + 1);
		SetPcString(CellTag(Root, OutputIndex, L"LotId"), Tray->CELL_LOT_ID[TargetIndex]);
		SetPcBool(CellTag(Root, OutputIndex, L"CellExist"), true);
		SetPcString(CellTag(Root, OutputIndex, L"NGCode"), Tray->LOSS_CD[TargetIndex]);
		SetPcString(CellTag(Root, OutputIndex, L"Grade"), Tray->RANK[TargetIndex]);
		SetPcBool(CellTag(Root, OutputIndex, L"WorkFlag"), Tray->WORK_FLAG[TargetIndex]);

		MainForm->WriteOpcUaLog("TRACK_OUT_DETAIL",
			"Cell[" + IntToStr(OutputIndex) + "] CellNo=" + IntToStr(TargetIndex + 1) +
			" CellId=" + Tray->SLOT_ID[TargetIndex] +
			" LotId=" + Tray->CELL_LOT_ID[TargetIndex] +
			" Grade=" + Tray->RANK[TargetIndex] +
			" NGCode=" + Tray->LOSS_CD[TargetIndex] +
			" WorkFlag=" + IntToStr(Tray->WORK_FLAG[TargetIndex] ? 1 : 0), false);
	}

	for (int OutputIndex = CellCount; OutputIndex < 96; ++OutputIndex)
	{
		SetPcString(CellTag(Root, OutputIndex, L"CellId"), L"");
		SetPcInt(CellTag(Root, OutputIndex, L"CellNo"), 0);
		SetPcString(CellTag(Root, OutputIndex, L"LotId"), L"");
		SetPcBool(CellTag(Root, OutputIndex, L"CellExist"), false);
		SetPcString(CellTag(Root, OutputIndex, L"NGCode"), L"");
		SetPcString(CellTag(Root, OutputIndex, L"Grade"), L"");
		SetPcBool(CellTag(Root, OutputIndex, L"WorkFlag"), false);
	}

	SetPcInt(TrackOutTag(L"CellCount"), CellCount);
	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
	LogOpcEvent("TRACK_OUT_CELL_INFORMATION WRITE TrayId=" + AnsiString(TargetTrayId) +
		" Count=" + IntToStr(CellCount), true);
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::CELL_TRACK_OUT_REQUEST(int SourceChannel, int TargetChannel,
	const AnsiString &CellId)
{
	if(MainForm == NULL || Mod_Fms == NULL)
		return;

	// Production path uses the current TrayInformation.TrayId values.
	UnicodeString SourceTrayId = Mod_Fms->GetPcTagString(
		TrayInfoTag(TAG_SOURCE, L"TrayId"), L"").Trim();
	UnicodeString TargetTrayId = Mod_Fms->GetPcTagString(
		TrayInfoTag(TAG_TARGET, L"TrayId"), L"").Trim();
	CELL_TRACK_OUT_REQUEST(SourceChannel, TargetChannel, CellId,
		SourceTrayId, TargetTrayId);
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::CELL_TRACK_OUT_REQUEST(int SourceChannel, int TargetChannel,
	const AnsiString &CellId, const UnicodeString &SourceTrayIdValue,
	const UnicodeString &TargetTrayIdValue)
{
	if(MainForm == NULL || Mod_Fms == NULL)
		return;

	UnicodeString SourceTrayId = SourceTrayIdValue.Trim();
	UnicodeString TargetTrayId = TargetTrayIdValue.Trim();
	if(SourceTrayId.IsEmpty() || TargetTrayId.IsEmpty())
	{
		LogOpcEvent("CELL_TRACK_OUT REQUEST FAIL TrayInformation.TrayId is empty" +
			AnsiString(" Source=") + AnsiString(SourceTrayId) +
			AnsiString(" Target=") + AnsiString(TargetTrayId), true);
		return;
	}


	const UnicodeString ResponseKey = CellTrackOutTag(L"CellUnloadCompleteResponse");
	int InitialResponse = GetFmsInt(ResponseKey);
	FCellTrackOutResponseRevision = Mod_Fms->GetFmsTagRevision(ResponseKey);
	FCellTrackOutWaitResponseIdle = !IsCycleResponseBypass() && (InitialResponse != 0);

	SetPcInt(CellTrackOutTag(L"CellNoFrom"), SourceChannel);
	SetPcString(CellTrackOutTag(L"TrayIdFrom"), SourceTrayId);
	SetPcInt(CellTrackOutTag(L"CellNoTo"), TargetChannel);
	SetPcString(CellTrackOutTag(L"TrayIdTo"), TargetTrayId);
	SetPcString(CellTrackOutTag(L"CellId"), UnicodeString(CellId));
	SetPcBool(CellTrackOutTag(L"CellUnloadComplete"), !FCellTrackOutWaitResponseIdle);
	Mod_Fms->FlushPendingPcTags(false);

	LogOpcEvent("CELL_TRACK_OUT REQUEST CellId=" + CellId +
		" From=" + AnsiString(SourceTrayId) + "/" + IntToStr(SourceChannel) +
		" To=" + AnsiString(TargetTrayId) + "/" + IntToStr(TargetChannel) +
		" RequestTag=NGS.F1NGS01.Location2.CellTrackOut.CellUnloadComplete=true" +
		" WaitingTag=NGS.F1NGS01.Location2.CellTrackOut.CellUnloadCompleteResponse" +
		" InitialResponse=" + IntToStr(InitialResponse) +
		" BaselineRevision=" + IntToStr((__int64)FCellTrackOutResponseRevision) +
		(FCellTrackOutWaitResponseIdle ?
			AnsiString(" / hold Request=OFF until stale Response=0") :
			AnsiString(" / Request=ON")) +
		" Expected=1(Success),2(Fail)", true);
}
//---------------------------------------------------------------------------
int __fastcall TMesOpc::CELL_TRACK_OUT_RESPONSE_RESULT()
{
	const UnicodeString RequestKey = CellTrackOutTag(L"CellUnloadComplete");
	const UnicodeString ResponseKey = CellTrackOutTag(L"CellUnloadCompleteResponse");
	int Response = GetFmsInt(ResponseKey);
	unsigned __int64 CurrentRevision = Mod_Fms != NULL ? Mod_Fms->GetFmsTagRevision(ResponseKey) : 0;
	if(FCellTrackOutWaitResponseIdle)
	{
		if(Response != 0) return 0;
		FCellTrackOutWaitResponseIdle = false;
		FCellTrackOutResponseRevision = CurrentRevision;
		SetPcBool(RequestKey, true);
		if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
		LogOpcEvent("CELL_TRACK_OUT RESPONSE IDLE confirmed / Request=ON / BaselineRevision=" +
			IntToStr((__int64)FCellTrackOutResponseRevision), true);
		return 3;
	}
	if(Response == 0)
		return 0;
	if(!IsCycleResponseBypass() && CurrentRevision <= FCellTrackOutResponseRevision)
		return 0;

	SetPcBool(RequestKey, false);
	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
	if(Response == 1){
		LogOpcEvent("CELL_TRACK_OUT RESPONSE=1 / Request=OFF / wait Response=0", true);
		return 1;
	}
	if(Response == 2){
		LogOpcEvent("CELL_TRACK_OUT RESPONSE=2 / Request=OFF / wait Response=0", true);
		return 2;
	}
	LogOpcEvent("VALIDATION FAIL CellUnloadCompleteResponse=" + IntToStr(Response), true);
	return -1;
}
//---------------------------------------------------------------------------
int __fastcall TMesOpc::CELL_TRACK_OUT_RESPONSE_VALUE()
{
	if(Mod_Fms == NULL) return -1;
	UnicodeString RawValue;
	if(!Mod_Fms->GetFmsTagJson(CellTrackOutTag(L"CellUnloadCompleteResponse"), RawValue))
		return -1;
	return StrToIntDef(RawValue.Trim(), -1);
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
		" RequestTag=NGS.F1NGS01.Location2.CellTrackOut.CellUnloadComplete(true)";

	if(MainForm != NULL)
		MainForm->WriteOpcUaLog("ERROR", Message, true);
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::LogCellTrackOutResponseOffTimeout()
{
	const UnicodeString RequestKey = CellTrackOutTag(L"CellUnloadComplete");
	const UnicodeString ResponseKey = CellTrackOutTag(L"CellUnloadCompleteResponse");
	UnicodeString RequestJson, ResponseJson;
	bool HasRequest = Mod_Fms != NULL && Mod_Fms->GetPcTagJson(RequestKey, RequestJson);
	bool HasResponse = Mod_Fms != NULL && Mod_Fms->GetFmsTagJson(ResponseKey, ResponseJson);
	if(MainForm != NULL)
		MainForm->WriteOpcUaLog("ERROR", "CELL_TRACK_OUT_RESPONSE_OFF_TIMEOUT Request=" +
			(HasRequest ? AnsiString(RequestJson) : AnsiString("<missing>")) +
			" Response=" + (HasResponse ? AnsiString(ResponseJson) : AnsiString("<missing>")) +
			" WaitMs=10000", true);
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::CELL_TRACK_OUT_CANCEL()
{
	SetPcBool(CellTrackOutTag(L"CellUnloadComplete"), false);
	FCellTrackOutWaitResponseIdle = false;
	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
	LogOpcEvent("CELL_TRACK_OUT CANCEL", true);
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::TRAY_UNLOAD_REQUEST()
{
	const UnicodeString RequestKey = TrayProcessTag(TAG_TARGET, L"TrayUnloadRequest");
	const UnicodeString ResponseKey = TrayProcessTag(TAG_TARGET, L"TrayUnloadResponse");
	int InitialResponse = GetFmsInt(ResponseKey);
	FTrayUnloadResponseRevision = Mod_Fms != NULL ? Mod_Fms->GetFmsTagRevision(ResponseKey) : 0;
	FTrayUnloadWaitResponseIdle = !IsCycleResponseBypass() && (InitialResponse != 0);
	SetPcBool(RequestKey, !FTrayUnloadWaitResponseIdle);
	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
	LogOpcEvent("TRAY_UNLOAD_REQUEST Location2 InitialResponse=" + IntToStr(InitialResponse) +
		" BaselineRevision=" + IntToStr((__int64)FTrayUnloadResponseRevision) +
		(FTrayUnloadWaitResponseIdle ?
			AnsiString(" / hold Request=OFF until stale Response=0") :
			AnsiString(" / Request=ON")), false);
}
//---------------------------------------------------------------------------
int __fastcall TMesOpc::TRAY_UNLOAD_RESPONSE_RESULT()
{
	const UnicodeString RequestKey = TrayProcessTag(TAG_TARGET, L"TrayUnloadRequest");
	const UnicodeString ResponseKey = TrayProcessTag(TAG_TARGET, L"TrayUnloadResponse");
	int Response = GetFmsInt(ResponseKey);
	unsigned __int64 CurrentRevision = Mod_Fms != NULL ? Mod_Fms->GetFmsTagRevision(ResponseKey) : 0;
	if(FTrayUnloadWaitResponseIdle)
	{
		if(Response != 0) return 0;
		FTrayUnloadWaitResponseIdle = false;
		FTrayUnloadResponseRevision = CurrentRevision;
		SetPcBool(RequestKey, true);
		if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
		LogOpcEvent("TRAY_UNLOAD_RESPONSE IDLE confirmed / Request=ON / BaselineRevision=" +
			IntToStr((__int64)FTrayUnloadResponseRevision), false);
		return 3;
	}
	if(Response == 0) return 0;
	if(!IsCycleResponseBypass() && CurrentRevision <= FTrayUnloadResponseRevision) return 0;

	SetPcBool(RequestKey, false);
	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
	if(Response == 1){
		LogOpcEvent("TRAY_UNLOAD_RESPONSE=1 / Request=OFF / wait Response=0", false);
		return 1;
	}
	if(Response == 2){
		LogOpcEvent("TRAY_UNLOAD_RESPONSE=2 / Request=OFF / wait Response=0", false);
		return 2;
	}
	LogOpcEvent("VALIDATION FAIL TrayUnloadResponse=" + IntToStr(Response), false);
	return -1;
}
//---------------------------------------------------------------------------
int __fastcall TMesOpc::TRAY_UNLOAD_RESPONSE_VALUE()
{
	if(Mod_Fms == NULL) return -1;
	UnicodeString RawValue;
	if(!Mod_Fms->GetFmsTagJson(
		TrayProcessTag(TAG_TARGET, L"TrayUnloadResponse"), RawValue))
		return -1;
	return StrToIntDef(RawValue.Trim(), -1);
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
void __fastcall TMesOpc::LogTrayUnloadResponseOffTimeout()
{
	const UnicodeString RequestKey = TrayProcessTag(TAG_TARGET, L"TrayUnloadRequest");
	const UnicodeString ResponseKey = TrayProcessTag(TAG_TARGET, L"TrayUnloadResponse");
	UnicodeString RequestJson, ResponseJson;
	bool HasRequest = Mod_Fms != NULL && Mod_Fms->GetPcTagJson(RequestKey, RequestJson);
	bool HasResponse = Mod_Fms != NULL && Mod_Fms->GetFmsTagJson(ResponseKey, ResponseJson);
	if(MainForm != NULL)
		MainForm->WriteOpcUaLog("ERROR", "TRAY_UNLOAD_RESPONSE_OFF_TIMEOUT Request=" +
			(HasRequest ? AnsiString(RequestJson) : AnsiString("<missing>")) +
			" Response=" + (HasResponse ? AnsiString(ResponseJson) : AnsiString("<missing>")) +
			" WaitMs=10000", true);
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::TRAY_UNLOAD_CANCEL()
{
	SetPcBool(TrayProcessTag(TAG_TARGET, L"TrayUnloadRequest"), false);
	FTrayUnloadWaitResponseIdle = false;
	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
	LogOpcEvent("TRAY_UNLOAD_CANCEL Request=OFF", false);
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::PROCESS_END_REQUEST()
{
	const UnicodeString RequestKey = TrayProcessTag(TAG_SOURCE, L"ProcessEnd");
	const UnicodeString ResponseKey = TrayProcessTag(TAG_SOURCE, L"ProcessEndResponse");
	int InitialResponse = GetFmsInt(ResponseKey);
	FProcessEndResponseRevision = Mod_Fms != NULL ? Mod_Fms->GetFmsTagRevision(ResponseKey) : 0;
	FProcessEndWaitResponseIdle = !IsCycleResponseBypass() && (InitialResponse != 0);
	SetPcBool(RequestKey, !FProcessEndWaitResponseIdle);
	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
	LogOpcEvent("PROCESS_END_REQUEST InitialResponse=" + IntToStr(InitialResponse) +
		" BaselineRevision=" + IntToStr((__int64)FProcessEndResponseRevision) +
		(FProcessEndWaitResponseIdle ?
			AnsiString(" / hold Request=OFF until stale Response=0") :
			AnsiString(" / Request=ON")), true);
}
//---------------------------------------------------------------------------
bool __fastcall TMesOpc::PROCESS_END_RESPONSE()
{
	return PROCESS_END_RESPONSE_RESULT() == 1;
}
//---------------------------------------------------------------------------
int __fastcall TMesOpc::PROCESS_END_RESPONSE_RESULT()
{
	const UnicodeString RequestKey = TrayProcessTag(TAG_SOURCE, L"ProcessEnd");
	const UnicodeString ResponseKey = TrayProcessTag(TAG_SOURCE, L"ProcessEndResponse");
	int Response = GetFmsInt(ResponseKey);
	unsigned __int64 CurrentRevision = Mod_Fms != NULL ? Mod_Fms->GetFmsTagRevision(ResponseKey) : 0;
	if(FProcessEndWaitResponseIdle)
	{
		if(Response != 0) return 0;
		FProcessEndWaitResponseIdle = false;
		FProcessEndResponseRevision = CurrentRevision;
		SetPcBool(RequestKey, true);
		if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
		LogOpcEvent("PROCESS_END_RESPONSE IDLE confirmed / Request=ON / BaselineRevision=" +
			IntToStr((__int64)FProcessEndResponseRevision), true);
		return 3;
	}
	if(Response == 0)
		return 0;
	if(!IsCycleResponseBypass() && CurrentRevision <= FProcessEndResponseRevision)
		return 0;

	SetPcBool(RequestKey, false);
	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
	if(Response == 1){
		LogOpcEvent("PROCESS_END_RESPONSE=1 / Request=OFF / wait Response=0", true);
		return 1;
	}
	if(Response == 2){
		LogOpcEvent("PROCESS_END_RESPONSE=2 / Request=OFF / wait Response=0", true);
		return 2;
	}
	LogOpcEvent("VALIDATION FAIL ProcessEndResponse=" + IntToStr(Response), true);
	return -1;
}
//---------------------------------------------------------------------------
int __fastcall TMesOpc::PROCESS_END_RESPONSE_VALUE()
{
	if(Mod_Fms == NULL) return -1;
	UnicodeString RawValue;
	if(!Mod_Fms->GetFmsTagJson(
		TrayProcessTag(TAG_SOURCE, L"ProcessEndResponse"), RawValue))
		return -1;
	return StrToIntDef(RawValue.Trim(), -1);
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
		" RequestTag=NGS.F1NGS01.Location1.TrayProcess.ProcessEnd(true)";

	if(MainForm != NULL)
		MainForm->WriteOpcUaLog("ERROR", Message, true);
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::LogProcessEndResponseOffTimeout()
{
	const UnicodeString RequestKey = TrayProcessTag(TAG_SOURCE, L"ProcessEnd");
	const UnicodeString ResponseKey = TrayProcessTag(TAG_SOURCE, L"ProcessEndResponse");
	UnicodeString RequestJson, ResponseJson;
	bool HasRequest = Mod_Fms != NULL && Mod_Fms->GetPcTagJson(RequestKey, RequestJson);
	bool HasResponse = Mod_Fms != NULL && Mod_Fms->GetFmsTagJson(ResponseKey, ResponseJson);
	if(MainForm != NULL)
		MainForm->WriteOpcUaLog("ERROR", "PROCESS_END_RESPONSE_OFF_TIMEOUT Request=" +
			(HasRequest ? AnsiString(RequestJson) : AnsiString("<missing>")) +
			" Response=" + (HasResponse ? AnsiString(ResponseJson) : AnsiString("<missing>")) +
			" WaitMs=10000", true);
}
//---------------------------------------------------------------------------
void __fastcall TMesOpc::PROCESS_END_CANCEL()
{
	SetPcBool(TrayProcessTag(TAG_SOURCE, L"ProcessEnd"), false);
	FProcessEndWaitResponseIdle = false;
	if(Mod_Fms != NULL) Mod_Fms->FlushPendingPcTags(false);
	LogOpcEvent("PROCESS_END_CANCEL", true);
}
//---------------------------------------------------------------------------
