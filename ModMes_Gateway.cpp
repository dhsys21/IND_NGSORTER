//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "ModMes_Gateway.h"
#include "FormMain.h"

#include <IdGlobal.hpp>
#include <IdIOHandler.hpp>
#include <IdSocketHandle.hpp>
#include <System.Variants.hpp>
#include <Xml.XMLDoc.hpp>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma classgroup "Vcl.Controls.TControl"
#pragma resource "*.dfm"

using namespace System;
using namespace System::Json;
using namespace System::Sysutils;
using namespace System::Syncobjs;
using namespace System::Variants;
using namespace Xml::Xmlintf;

static const UnicodeString FMS_BIND_IP = L"127.0.0.1";
static const int FMS_BIND_PORT = 18080;
static const int FMS_MAX_JSON_LINE_LENGTH = 4 * 1024 * 1024;
static const int FMS_RECONNECT_INTERVAL_MS = 5000;
static const int FMS_ALIVE_INTERVAL_MS = 30000;

TMod_Fms *Mod_Fms;

static UnicodeString ResolveFmsTagConfigFile()
{
	UnicodeString ExeDir = ExtractFilePath(Application->ExeName);
	UnicodeString Candidates[] =
	{
		// Use the deployed OPC UA Gateway NodeSet first.
		L"D:\\Program\\DHS.Equipment.OpcUaGateway\\DHS.Equipment.OpcUaGateway\\Config\\NGSORTER.Config.xml",
		ExeDir + L"Config\\NGSORTER.Config.xml",
		ExeDir + L"NGSORTER.Config.xml",
		L"D:\\NGSORTER_IND\\Config\\NGSORTER.Config.xml",
		L"D:\\Project\\2026\\03.Jeng(India)\\02.Program\\05.OpcFoundation\\"
			L"DHS.Equipment.OpcUaGateway\\DHS.Equipment.OpcUaGateway\\Config\\"
			L"NGSORTER.Config.xml"
	};

	for (unsigned int i = 0; i < sizeof(Candidates) / sizeof(Candidates[0]); ++i)
	{
		if (FileExists(Candidates[i]))
			return Candidates[i];
	}

	return Candidates[0];
}

static bool IsNumericText(const UnicodeString &Text)
{
	if (Text.IsEmpty())
		return false;

	for (int i = 1; i <= Text.Length(); ++i)
	{
		if (Text[i] < L'0' || Text[i] > L'9')
			return false;
	}
	return true;
}

static UnicodeString CanonicalCellTagKey(const UnicodeString &Key, bool DisplayToInternal = false)
{
	UnicodeString Result = Key;
	int SearchFrom = 1;

	while (true)
	{
		UnicodeString Tail = Result.SubString(SearchFrom, Result.Length());
		int RelativePos = Tail.Pos(L".Cell.Cell[");
		if (RelativePos <= 0)
			break;

		int CellPos = SearchFrom + RelativePos - 1;
		int NumberStart = CellPos + 11;
		int NumberEnd = NumberStart;
		while (NumberEnd <= Result.Length() && Result[NumberEnd] != L']')
			++NumberEnd;

		UnicodeString NumberText = Result.SubString(NumberStart, NumberEnd - NumberStart);
		if (IsNumericText(NumberText))
		{
			UnicodeString Replacement = L".Cell." + IntToStr(NumberText.ToIntDef(0));
			Result.Delete(CellPos, NumberEnd - CellPos + 1);
			Result.Insert(Replacement, CellPos);
			SearchFrom = CellPos + Replacement.Length();
		}
		else
			SearchFrom = NumberEnd;
	}

	SearchFrom = 1;
	while (true)
	{
		UnicodeString Tail = Result.SubString(SearchFrom, Result.Length());
		int RelativePos = Tail.Pos(L".Cell.");
		if (RelativePos <= 0)
			break;

		int CellPos = SearchFrom + RelativePos - 1;
		int NumberStart = CellPos + 6;
		int NumberEnd = NumberStart;
		while (NumberEnd <= Result.Length() && Result[NumberEnd] != L'.')
			++NumberEnd;

		UnicodeString NumberText = Result.SubString(NumberStart, NumberEnd - NumberStart);
		if (IsNumericText(NumberText))
		{
			int Number = NumberText.ToIntDef(0);
			if (DisplayToInternal && NumberText.Length() >= 2 && Number > 0)
				--Number;
			UnicodeString Canonical = IntToStr(Number);
			Result.Delete(NumberStart, NumberEnd - NumberStart);
			Result.Insert(Canonical, NumberStart);
			NumberEnd = NumberStart + Canonical.Length();
		}

		SearchFrom = NumberEnd;
	}

	return Result;
}

static UnicodeString CompatibilityTagKey(const UnicodeString &Key)
{
	UnicodeString Result = CanonicalCellTagKey(Key);
	const UnicodeString RootPrefix = L"NGS.";
	if(Result.SubString(1, RootPrefix.Length()) == RootPrefix)
		Result = Result.SubString(RootPrefix.Length() + 1, Result.Length());
	return Result;
}

static bool CompatibilityTagKeyEquals(const UnicodeString &Left, const UnicodeString &Right)
{
	return CompatibilityTagKey(Left) == CompatibilityTagKey(Right);
}

static bool TagKeyEndsWith(const UnicodeString &FullKey, const UnicodeString &Key)
{
	if (Key.IsEmpty() || FullKey.Length() < Key.Length())
		return false;

	int Start = FullKey.Length() - Key.Length() + 1;
	if (FullKey.SubString(Start, Key.Length()) != Key)
		return false;

	return Start == 1 || FullKey[Start - 1] == L'.';
}

static bool TextEndsWith(const UnicodeString &Text, const UnicodeString &Suffix)
{
	if (Text.Length() < Suffix.Length())
		return false;
	return Text.SubString(Text.Length() - Suffix.Length() + 1, Suffix.Length()) == Suffix;
}

static bool IsSnapshotLine(const UnicodeString &Line)
{
	return Line.UpperCase().Pos(L"SNAPSHOT") > 0;
}

static UnicodeString FormatGatewayTagKey(const UnicodeString &Key)
{
	UnicodeString Result = CanonicalCellTagKey(Key);
	int SearchFrom = 1;

	while (true)
	{
		UnicodeString Tail = Result.SubString(SearchFrom, Result.Length());
		int RelativePos = Tail.Pos(L".Cell.");
		if (RelativePos <= 0)
			break;

		int CellPos = SearchFrom + RelativePos - 1;
		int NumberStart = CellPos + 6;
		int NumberEnd = NumberStart;
		while (NumberEnd <= Result.Length() && Result[NumberEnd] != L'.')
			++NumberEnd;

		UnicodeString NumberText = Result.SubString(NumberStart, NumberEnd - NumberStart);
		if (IsNumericText(NumberText))
		{
			int Number = NumberText.ToIntDef(0) + 1;
			UnicodeString Formatted = FormatFloat(L"00", Number);
			Result.Delete(NumberStart, NumberEnd - NumberStart);
			Result.Insert(Formatted, NumberStart);
			NumberEnd = NumberStart + Formatted.Length();
		}

		SearchFrom = NumberEnd;
	}

	return Result;
}

static UnicodeString FormatProtocolTagKey(const UnicodeString &Key)
{
	// TCP JSON uses the equipment contract key, independent of the OPC UA
	// browse root: F1NGS01... and zero-based Cell.<index> records.
	return CompatibilityTagKey(Key);
}

//---------------------------------------------------------------------------
class TLockGuard
{
private:
	TCriticalSection *FSection;

public:
	__fastcall TLockGuard(TCriticalSection *Section)
		: FSection(Section)
	{
		FSection->Acquire();
	}

	~TLockGuard(void)
	{
		FSection->Release();
	}
};

//---------------------------------------------------------------------------
static UnicodeString XmlAttrToStr(_di_IXMLNode Node, const UnicodeString &Name)
{
	if (Node == NULL || !Node->HasAttribute(Name))
		return L"";

	OleVariant OleValue = Node->GetAttribute(Name);
	Variant Value = OleValue.AsType(varOleStr);
	return VarToStr(Value);
}

//---------------------------------------------------------------------------
__fastcall TMod_Fms::TMod_Fms(TComponent* Owner)
	: TDataModule(Owner),
	  FLock(NULL),
	  FSendLock(NULL),
	  FLogLock(NULL),
	  FFmsRevisionCounter(0),
	  FSnapshotReceived(false),
	  FTagConfigLoaded(false),
	  FGatewayConnected(false),
	  FStopping(false),
	  FBindIp(FMS_BIND_IP),
	  FBindPort(FMS_BIND_PORT),
	  FAutoStartEnabled(false)
{
	FLock = new TCriticalSection();
	FSendLock = new TCriticalSection();
	FLogLock = new TCriticalSection();

	Timer_Alive->Enabled = false;
	// OPC UA Alive handshake interval: EQP sets ON every 30 seconds.
	Timer_Alive->Interval = FMS_ALIVE_INTERVAL_MS;
	Timer_Reconnect->Enabled = false;
	Timer_Reconnect->Interval = FMS_RECONNECT_INTERVAL_MS;
	//* FMS RECONNECT : Worker callbacks never touch VCL controls directly.
	//* This main-thread timer drains their queued file/UI log records.
	Timer_LogDispatch->Enabled = false;
	Timer_LogDispatch->Interval = 100;
	Timer_LogDispatch->OnTimer = Timer_LogDispatchTimer;
	TcpServer->Active = false;
	TcpServer->DefaultPort = FBindPort;
	// Keep application shutdown responsive even if a gateway worker is blocked.
	TcpServer->TerminateWaitTime = 1000;
	TcpServer->Bindings->Clear();

	TIdSocketHandle *Binding = TcpServer->Bindings->Add();
	Binding->IP = FBindIp;
	Binding->Port = FBindPort;
}
//---------------------------------------------------------------------------
__fastcall TMod_Fms::~TMod_Fms(void)
{
	Stop();
	delete FLogLock;
	FLogLock = NULL;
	delete FLock;
	FLock = NULL;
	delete FSendLock;
	FSendLock = NULL;
}
//---------------------------------------------------------------------------
bool __fastcall TMod_Fms::TryStartServer(void)
{
	if (TcpServer == NULL)
		return false;
	if (TcpServer->Active)
		return true;

	try
	{
		if (!FTagConfigLoaded)
			LoadTagConfig(ResolveFmsTagConfigFile());

		TcpServer->Active = true;
		Timer_Alive->Enabled = true;
		LogOpcUa(L"SERVER", L"LISTEN " + FBindIp + L":" + IntToStr(FBindPort));
		return true;
	}
	catch (Exception &E)
	{
		Timer_Alive->Enabled = false;
		LogOpcUa(L"ERROR", L"Gateway listen failed; retry in 5 seconds: " + E.Message);
		return false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::Start(void)
{
	FStopping = false;
	FAutoStartEnabled = true;
	Timer_LogDispatch->Enabled = true;
	Timer_Reconnect->Enabled = true;
	TryStartServer();
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::Stop(void)
{
	// Shutdown order: stop producers first, mark the worker callbacks as stopping,
	// disconnect live contexts, and only then deactivate the Indy server.
	FStopping = true;
	FAutoStartEnabled = false;
	Timer_Reconnect->Enabled = false;
	Timer_Alive->Enabled = false;
	Timer_LogDispatch->Enabled = false;
	{
		TLockGuard Guard(FLock);
		FGatewayConnected = false;
	}

	bool WasActive = TcpServer != NULL && TcpServer->Active;
	if (WasActive)
	{
		TList *List = TcpServer->Contexts->LockList();
		try
		{
			for (int i = 0; i < List->Count; ++i)
			{
				TIdContext *Context = static_cast<TIdContext*>(List->Items[i]);
				try
				{
					if (Context != NULL && Context->Connection != NULL &&
						Context->Connection->Connected())
						Context->Connection->Disconnect(false);
				}
				catch (...)
				{
					// A peer can close between Connected() and Disconnect(); ignore on shutdown.
				}
			}
		}
		__finally
		{
			TcpServer->Contexts->UnlockList();
		}

		try
		{
			TcpServer->Active = false;
		}
		catch (...)
		{
			// Indy may report an already-closed context while deactivating the server.
		}
		LogOpcUa(L"SERVER", L"STOP");
	}
	if (FLogLock != NULL)
	{
		TLockGuard LogGuard(FLogLock);
		FQueuedLogs.clear();
	}
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::Configure(const UnicodeString &BindIp, int BindPort)
{
	if (TcpServer == NULL)
		return;

	bool WasActive = TcpServer->Active;
	bool WasAutoStartEnabled = FAutoStartEnabled;
	if (WasActive)
	{
		Timer_Alive->Enabled = false;
		TcpServer->Active = false;
	}

	FBindIp = BindIp;
	FBindPort = BindPort;
	TcpServer->DefaultPort = FBindPort;
	TcpServer->Bindings->Clear();

	TIdSocketHandle *Binding = TcpServer->Bindings->Add();
	Binding->IP = FBindIp;
	Binding->Port = FBindPort;

	if (WasActive || WasAutoStartEnabled)
		Start();
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::Timer_ReconnectTimer(TObject *Sender)
{
	if (FAutoStartEnabled && (TcpServer == NULL || !TcpServer->Active))
		TryStartServer();
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::Timer_LogDispatchTimer(TObject *Sender)
{
	//* FMS RECONNECT : TTimer runs in the VCL main thread. Drain a bounded
	//* batch so a large Snapshot cannot freeze the GUI with thousands of logs.
	if (FLogLock == NULL || MainForm == NULL)
		return;

	std::deque<TFmsQueuedLog> Batch;
	{
		TLockGuard Guard(FLogLock);
		for (int i = 0; i < 50 && !FQueuedLogs.empty(); ++i)
		{
			Batch.push_back(FQueuedLogs.front());
			FQueuedLogs.pop_front();
		}
	}

	while (!Batch.empty())
	{
		const TFmsQueuedLog &Entry = Batch.front();
		MainForm->WriteOpcUaLog(AnsiString(Entry.Type), AnsiString(Entry.Message), Entry.Display);
		Batch.pop_front();
	}
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::Timer_AliveTimer(TObject *Sender)
{
	// OPC UA Alive handshake:
	// EQP only sets Alive ON every 30 seconds. FMS owns the OFF reset,
	// so EQP must never toggle/write Alive OFF.
	SetPcTag(L"NGS.F1NGS01.Common.Alive", true);
	FlushPendingPcTags(false);
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::TcpServerConnect(TIdContext *AContext)
{
	if (AContext != NULL && AContext->Connection != NULL &&
		AContext->Connection->IOHandler != NULL)
	{
		AContext->Connection->IOHandler->DefStringEncoding = Idglobal::IndyTextEncoding_UTF8();
	}

	{
		TLockGuard Guard(FLock);
		FGatewayConnected = true;
	}
	LogOpcUa(L"CONNECT", L"Gateway connected");
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::TcpServerDisconnect(TIdContext *AContext)
{
	{
		TLockGuard Guard(FLock);
		FGatewayConnected = false;
	}
	if (!FStopping)
		LogOpcUa(L"DISCONNECT", L"Gateway disconnected");
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::TcpServerExecute(TIdContext *AContext)
{
	if (AContext == NULL || AContext->Connection == NULL ||
		AContext->Connection->IOHandler == NULL)
		return;

	try
	{
		UnicodeString Line = AContext->Connection->IOHandler->ReadLn(
			L"\n", -1, FMS_MAX_JSON_LINE_LENGTH, Idglobal::IndyTextEncoding_UTF8());

		if (Line.Length() > 0 && Line[Line.Length()] == L'\r')
			Line = Line.SubString(1, Line.Length() - 1);

		UnicodeString Response = HandleMessage(Line);
		bool Snapshot = IsSnapshotLine(Line);
		bool ChangedUpdate = Line.UpperCase().Pos(L"FMS_CHANGED") > 0;
		if (Snapshot)
		{
			int FmsTagCount = 0;
			int PcTagCount = 0;
			{
				TLockGuard Guard(FLock);
				FmsTagCount = (int)FFmsTags.size();
				PcTagCount = (int)FPcTags.size();
			}

			UTF8String SnapshotUtf8(Line);
			UnicodeString Summary = L"SNAPSHOT: FmsTags=" + IntToStr(FmsTagCount) +
				L", PcTags=" + IntToStr(PcTagCount) +
				L", Bytes=" + IntToStr(SnapshotUtf8.Length() + 1);
			LogOpcUa(L"RX", Summary);
			LogOpcUa(L"TX", Response, false);
		}
		else if (ChangedUpdate)
		{
			// TrackIn/TrackOut/CellTrackOut payloads can be very large: file only.
			LogOpcUa(L"RX_DETAIL", Line, false);
			LogOpcUa(L"TX_DETAIL", Response, false);
		}
		else
		{
			LogOpcUa(L"RX", Line);
			LogOpcUa(L"TX", Response);
		}
		{
			TLockGuard SendGuard(FSendLock);
			if (FStopping || !AContext->Connection->Connected())
				return;
			AContext->Connection->IOHandler->Write(
				Response + L"\n", Idglobal::IndyTextEncoding_UTF8());
		}
	}
	catch (Exception &E)
	{
		// A disconnect is expected while Stop() interrupts the blocking ReadLn.
		// Never call Disconnect() again on an already disconnected Indy context.
		if (!FStopping)
		{
			LogOpcUa(L"ERROR", L"TCP execute: " + E.Message);
			try
			{
				if (AContext != NULL && AContext->Connection != NULL &&
					AContext->Connection->Connected())
					AContext->Connection->Disconnect(false);
			}
			catch (...)
			{
			}
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::LogOpcUa(const UnicodeString &Type, const UnicodeString &Msg, bool bDisplay)
{
	if (MainForm == NULL)
		return;

	//* FMS RECONNECT : Indy OnConnect/OnDisconnect/OnExecute run on worker
	//* threads. Calling MainForm/Memo while holding FLock can deadlock the GUI.
	if (::GetCurrentThreadId() == System::MainThreadID)
	{
		MainForm->WriteOpcUaLog(AnsiString(Type), AnsiString(Msg), bDisplay);
		return;
	}

	if (FStopping || FLogLock == NULL)
		return;

	TFmsQueuedLog Entry;
	Entry.Type = Type;
	Entry.Message = Msg;
	Entry.Display = bDisplay;
	{
		TLockGuard Guard(FLogLock);
		// Bound memory even if a malformed Snapshot generates many warnings.
		if (FQueuedLogs.size() >= 2000)
			FQueuedLogs.pop_front();
		FQueuedLogs.push_back(Entry);
	}
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::LoadTagConfig(const UnicodeString &FileName)
{
	try
	{
		if (!FileExists(FileName))
		{
			LogOpcUa(L"ERROR", L"Tag config not found: " + FileName);
			return;
		}

		{
			TLockGuard Guard(FLock);
			FTagDefinitions.clear();
		}

		_di_IXMLDocument Document = Xml::Xmldoc::LoadXMLDocument(FileName);
		ReadTagNodes(Document->DocumentElement);

		{
			TLockGuard Guard(FLock);
			FTagConfigLoaded = !FTagDefinitions.empty();
			LogOpcUa(L"CONFIG", L"Loaded tag definitions: " + IntToStr((int)FTagDefinitions.size()));
		}
	}
	catch (Exception &E)
	{
		LogOpcUa(L"ERROR", L"Load tag config failed: " + E.Message);
	}
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::ReadTagNodes(_di_IXMLNode Node)
{
	if (Node == NULL)
		return;

	_di_IXMLNodeList Children = Node->ChildNodes;

	if (Node->LocalName == L"UAVariable")
	{
		TFmsTagDefinition Definition;

		Definition.NodeId = XmlAttrToStr(Node, L"NodeId");
		int Pos = Definition.NodeId.Pos(L";s=");
		if (Pos > 0)
			Definition.FullKey = Definition.NodeId.SubString(Pos + 3, Definition.NodeId.Length());

		int Dot = Definition.FullKey.Pos(L".");
		if (Dot > 0)
			Definition.ShortKey = Definition.FullKey.SubString(Dot + 1, Definition.FullKey.Length());

		Definition.DataType = XmlAttrToStr(Node, L"DataType");
		Definition.AccessLevel = StrToIntDef(XmlAttrToStr(Node, L"AccessLevel"), 0);
		Definition.ValueRank = StrToIntDef(XmlAttrToStr(Node, L"ValueRank"), -1);
		Definition.ArrayDimensions = XmlAttrToStr(Node, L"ArrayDimensions");

		_di_IXMLNode Description;
		for (int i = 0; i < Children->Count; ++i)
		{
			_di_IXMLNode Child = Children->Get(i);
			if (Child != NULL && Child->LocalName == L"Description")
			{
				Description = Child;
				break;
			}
		}

		UnicodeString Desc = L"";
		if (Description != NULL)
			Desc = Description->Text.UpperCase();
		if (Desc.Pos(L"WRITE: EQP/FMS BOTH") > 0 || Desc.Pos(L"WRITE: BOTH") > 0)
			Definition.Direction = ftdBoth;
		else if (Desc.Pos(L"WRITE: EQP ONLY") > 0)
			Definition.Direction = ftdEqpOnly;
		else if (Desc.Pos(L"WRITE: FMS ONLY") > 0)
			Definition.Direction = ftdFmsOnly;
		else
			Definition.Direction = ftdUnknown;

		RegisterTagDefinition(Definition);
	}

	for (int i = 0; i < Children->Count; ++i)
		ReadTagNodes(Children->Get(i));
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::RegisterTagDefinition(const TFmsTagDefinition &Definition)
{
	if (Definition.FullKey.IsEmpty())
		return;

	TLockGuard Guard(FLock);
	FTagDefinitions[Definition.FullKey] = Definition;
	RegisterTagAlias(Definition.ShortKey, Definition);

	// Accept both the current NGS.F1NGS01 path and the legacy F1NGS01 path.
	const UnicodeString RootedPrefix = L"NGS.F1NGS01.";
	const UnicodeString LegacyPrefix = L"F1NGS01.";
	if(Definition.FullKey.SubString(1, RootedPrefix.Length()) == RootedPrefix)
		RegisterTagAlias(Definition.FullKey.SubString(5, Definition.FullKey.Length()), Definition);
	else if(Definition.FullKey.SubString(1, LegacyPrefix.Length()) == LegacyPrefix)
		RegisterTagAlias(L"NGS." + Definition.FullKey, Definition);

	int LocationPos = Definition.FullKey.Pos(L".Location1.");
	if (LocationPos <= 0)
		LocationPos = Definition.FullKey.Pos(L".Location2.");
	if (LocationPos > 0)
	{
		UnicodeString Tail = Definition.FullKey.SubString(LocationPos + 1, Definition.FullKey.Length());
		RegisterTagAlias(Tail, Definition);

		int FirstDot = Definition.FullKey.Pos(L".");
		if (FirstDot > 0)
		{
			UnicodeString LinePrefix = Definition.FullKey.SubString(1, FirstDot - 1);
			RegisterTagAlias(LinePrefix + L"." + Tail, Definition);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::RegisterTagAlias(const UnicodeString &Alias, const TFmsTagDefinition &Definition)
{
	if (Alias.IsEmpty())
		return;

	UnicodeString Canonical = CanonicalCellTagKey(Alias);
	if (FTagDefinitions.find(Alias) == FTagDefinitions.end())
		FTagDefinitions[Alias] = Definition;
	if (Canonical != Alias && FTagDefinitions.find(Canonical) == FTagDefinitions.end())
		FTagDefinitions[Canonical] = Definition;
}
//---------------------------------------------------------------------------
bool __fastcall TMod_Fms::FindTagDefinition(const UnicodeString &Key, TFmsTagDefinition &Definition)
{
	TTagDefinitionMap::iterator it = FTagDefinitions.find(Key);
	if (it == FTagDefinitions.end())
	{
		UnicodeString Canonical = CanonicalCellTagKey(Key);
		it = FTagDefinitions.find(Canonical);
		if (it == FTagDefinitions.end())
		{
			Canonical = CanonicalCellTagKey(Key, true);
			it = FTagDefinitions.find(Canonical);
		}
		if (it == FTagDefinitions.end())
			return false;
	}

	Definition = it->second;
	return true;
}
//---------------------------------------------------------------------------
bool __fastcall TMod_Fms::KeyMatchesDefinition(const UnicodeString &Key, const TFmsTagDefinition &Definition)
{
	UnicodeString CanonicalKey = CanonicalCellTagKey(Key, true);
	UnicodeString CanonicalFullKey = CanonicalCellTagKey(Definition.FullKey);

	if (CanonicalKey == CanonicalFullKey)
		return true;
	if (!Definition.ShortKey.IsEmpty() && CanonicalKey == CanonicalCellTagKey(Definition.ShortKey))
		return true;
	if (TagKeyEndsWith(CanonicalFullKey, CanonicalKey))
		return true;

	int KeyDot = CanonicalKey.Pos(L".");
	int FullDot = CanonicalFullKey.Pos(L".");
	if (KeyDot > 0 && FullDot > 0)
	{
		UnicodeString KeyLine = CanonicalKey.SubString(1, KeyDot - 1);
		UnicodeString FullLine = CanonicalFullKey.SubString(1, FullDot - 1);
		UnicodeString KeyTail = CanonicalKey.SubString(KeyDot + 1, CanonicalKey.Length());
		if (KeyLine == FullLine && TagKeyEndsWith(CanonicalFullKey, KeyTail))
			return true;
	}

	return false;
}
//---------------------------------------------------------------------------
bool __fastcall TMod_Fms::FindTagDefinitionForDirection(const UnicodeString &Key, TFmsTagDirection Direction, TFmsTagDefinition &Definition)
{
	// Most runtime calls use a full key or a registered alias. Resolve that in
	// O(log N) before using the compatibility scan over the full NodeSet.
	TFmsTagDefinition Direct;
	if (FindTagDefinition(Key, Direct) &&
		(Direct.Direction == Direction || Direct.Direction == ftdBoth ||
		 IsImplicitDirectionTag(Direct, Direction)))
	{
		Definition = Direct;
		return true;
	}

	for (TTagDefinitionMap::iterator it = FTagDefinitions.begin(); it != FTagDefinitions.end(); ++it)
	{
		const TFmsTagDefinition &Candidate = it->second;
		if (it->first != Candidate.FullKey)
			continue;
		if (!KeyMatchesDefinition(Key, Candidate))
			continue;
		if (Candidate.Direction == Direction || Candidate.Direction == ftdBoth ||
			IsImplicitDirectionTag(Candidate, Direction))
		{
			Definition = Candidate;
			return true;
		}
	}

	return FindTagDefinition(Key, Definition);
}
//---------------------------------------------------------------------------
UnicodeString __fastcall TMod_Fms::NormalizeTagKey(const UnicodeString &Key)
{
	TFmsTagDefinition Definition;
	if (FindTagDefinition(Key, Definition))
		return Definition.FullKey;

	LogOpcUa(L"WARN", L"Unknown tag key: " + Key);
	return Key;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall TMod_Fms::NormalizeTagKeyForDirection(const UnicodeString &Key, TFmsTagDirection Direction)
{
	TFmsTagDefinition Definition;
	if (FindTagDefinitionForDirection(Key, Direction, Definition))
		return Definition.FullKey;

	LogOpcUa(L"WARN", L"Unknown tag key: " + Key);
	return Key;
}
//---------------------------------------------------------------------------
bool __fastcall TMod_Fms::ValidateJsonValue(const TFmsTagDefinition &Definition, TJSONValue *Value)
{
	if (Value == NULL || Definition.DataType.IsEmpty())
		return true;

	UnicodeString DataType = Definition.DataType.UpperCase();
	UnicodeString Text = Value->ToString().Trim().LowerCase();

	if (DataType == L"BOOLEAN")
		return Text == L"true" || Text == L"false";

	if (DataType == L"UINT16" || DataType == L"INT32" || DataType == L"UINT32" ||
		DataType == L"FLOAT" || DataType == L"DOUBLE")
	{
		if (Text.IsEmpty())
			return false;

		for (int i = 1; i <= Text.Length(); ++i)
		{
			wchar_t Ch = Text[i];
			if ((Ch < L'0' || Ch > L'9') && Ch != L'-' && Ch != L'+' &&
				Ch != L'.' && Ch != L'e')
				return false;
		}
		return true;
	}

	if (DataType == L"STRING")
		return Text.Length() >= 2 && Text[1] == L'"';

	return true;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall TMod_Fms::HandleMessage(const UnicodeString &Line)
{
	TJSONValue *Value = NULL;
	UnicodeString Result;

	try
	{
		Value = TJSONObject::ParseJSONValue(Line, true);
		TJSONObject *Json = dynamic_cast<TJSONObject*>(Value);
		if (Json == NULL)
			Result = BuildErrorResponse(L"invalid json");
		else
		{
			TJSONString *TypeValue = dynamic_cast<TJSONString*>(Json->GetValue(L"type"));
			if (TypeValue == NULL)
				Result = BuildErrorResponse(L"missing type");
			else
			{
				UnicodeString Type = TypeValue->Value();
				if (Type == L"SNAPSHOT")
				{
					ApplySnapshot(Json);
					Result = BuildSuccessResponse();
				}
				else if (Type == L"FMS_CHANGED")
				{
					ApplyChangedTags(Json);
					Result = BuildSuccessResponse();
				}
				else
					Result = BuildErrorResponse(L"unknown type");
			}
		}
	}
	catch (Exception &E)
	{
		Result = BuildErrorResponse(E.Message);
	}

	delete Value;
	return Result;
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::ApplySnapshot(TJSONObject *Json)
{
	TLockGuard Guard(FLock);

	TJSONString *Equipment = dynamic_cast<TJSONString*>(Json->GetValue(L"equipment"));
	TJSONString *Timestamp = dynamic_cast<TJSONString*>(Json->GetValue(L"timestamp"));
	TJSONObject *FmsTags = dynamic_cast<TJSONObject*>(Json->GetValue(L"fmsTags"));
	TJSONObject *PcTags = dynamic_cast<TJSONObject*>(Json->GetValue(L"pcTags"));

	if (Equipment != NULL)
		FLastEquipment = Equipment->Value();
	if (Timestamp != NULL)
		FLastTimestamp = Timestamp->Value();

	FFmsTags.clear();
	FFmsTagRevisions.clear();
	FPcTags.clear();
	if (FmsTags != NULL)
		CopyTags(FmsTags, FFmsTags, ftdFmsOnly);
	if (PcTags != NULL)
		CopyTags(PcTags, FPcTags, ftdEqpOnly);

	FSnapshotReceived = true;
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::ApplyChangedTags(TJSONObject *Json)
{
	TLockGuard Guard(FLock);

	TJSONString *Equipment = dynamic_cast<TJSONString*>(Json->GetValue(L"equipment"));
	TJSONString *Timestamp = dynamic_cast<TJSONString*>(Json->GetValue(L"timestamp"));
	TJSONObject *Tags = dynamic_cast<TJSONObject*>(Json->GetValue(L"tags"));
	TJSONObject *PcTags = dynamic_cast<TJSONObject*>(Json->GetValue(L"pcTags"));

	if (Equipment != NULL)
		FLastEquipment = Equipment->Value();
	if (Timestamp != NULL)
		FLastTimestamp = Timestamp->Value();
	if (Tags != NULL)
		CopyChangedTags(Tags);
	if (PcTags != NULL)
		CopyTags(PcTags, FPcTags, ftdEqpOnly);
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::CopyTags(TJSONObject *Tags, TTagMap &Target, TFmsTagDirection Direction)
{
	if (Tags == NULL)
		return;

	for (int i = 0; i < Tags->Count; ++i)
	{
		TJSONPair *Pair = Tags->Pairs[i];
		if (Pair != NULL && Pair->JsonString != NULL && Pair->JsonValue != NULL)
		{
			UnicodeString Key = Pair->JsonString->Value();
			TFmsTagDefinition Definition;
			bool KnownTag = FindTagDefinitionForDirection(Key, Direction, Definition);
			if (KnownTag && !ValidateJsonValue(Definition, Pair->JsonValue))
				LogOpcUa(L"WARN", L"Type mismatch: " + Key + L" expected " +
					Definition.DataType + L" actual=" + Pair->JsonValue->ToString());

			UnicodeString NormalizedKey = KnownTag ? Definition.FullKey : NormalizeTagKey(Key);
			UnicodeString Value = Pair->JsonValue->ToString();
			if (&Target == &FFmsTags)
				StoreFmsTag(NormalizedKey, Value);
			else
				Target[NormalizedKey] = Value;
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::StoreFmsTag(const UnicodeString &Key, const UnicodeString &Value)
{
	// Keep the last received value visible and record a monotonic receive revision.
	// A request can therefore reject a stale response without deleting the tag value.
	FFmsTags[Key] = Value;
	FFmsTagRevisions[Key] = ++FFmsRevisionCounter;
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::CopyChangedTags(TJSONObject *Tags)
{
	if (Tags == NULL)
		return;

	for (int i = 0; i < Tags->Count; ++i)
	{
		TJSONPair *Pair = Tags->Pairs[i];
		if (Pair == NULL || Pair->JsonString == NULL || Pair->JsonValue == NULL)
			continue;

		UnicodeString Value = Pair->JsonValue->ToString();
		UnicodeString Key = Pair->JsonString->Value();
		bool Applied = false;

		// Changed messages normally contain a registered full key or alias.
		// Resolve it directly instead of scanning the complete XML NodeSet.
		TFmsTagDefinition Direct;
		if (FindTagDefinition(Key, Direct))
		{
			if (!ValidateJsonValue(Direct, Pair->JsonValue))
				LogOpcUa(L"WARN", L"Type mismatch: " + Key + L" expected " +
					Direct.DataType + L" actual=" + Pair->JsonValue->ToString());

			if (Direct.Direction == ftdEqpOnly || IsImplicitDirectionTag(Direct, ftdEqpOnly))
				FPcTags[Direct.FullKey] = Value;
			else if (Direct.Direction == ftdBoth)
			{
				StoreFmsTag(Direct.FullKey, Value);
				FPcTags[Direct.FullKey] = Value;
			}
			else
				StoreFmsTag(Direct.FullKey, Value);
			Applied = true;
		}

		for (TTagDefinitionMap::iterator it = FTagDefinitions.begin(); !Applied && it != FTagDefinitions.end(); ++it)
		{
			const TFmsTagDefinition &Definition = it->second;
			if (it->first != Definition.FullKey || !KeyMatchesDefinition(Key, Definition))
				continue;

			if (!ValidateJsonValue(Definition, Pair->JsonValue))
				LogOpcUa(L"WARN", L"Type mismatch: " + Key + L" expected " +
					Definition.DataType + L" actual=" + Pair->JsonValue->ToString());

			if (Definition.Direction == ftdEqpOnly || IsImplicitDirectionTag(Definition, ftdEqpOnly))
				FPcTags[Definition.FullKey] = Value;
			else if (Definition.Direction == ftdBoth)
			{
				StoreFmsTag(Definition.FullKey, Value);
				FPcTags[Definition.FullKey] = Value;
			}
			else
				StoreFmsTag(Definition.FullKey, Value);

			Applied = true;
		}

		if (!Applied)
		{
			UnicodeString NormalizedKey = NormalizeTagKey(Key);
			StoreFmsTag(NormalizedKey, Value);
		}
	}
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Request/response clear timing is owned by TMesOpc. Cached FMS values must
// never change a PC request behind the active four-phase handshake state.
//---------------------------------------------------------------------------
UnicodeString __fastcall TMod_Fms::BuildSuccessResponse(void)
{
	TJSONObject *Response = new TJSONObject();
	UnicodeString Result;

	Response->AddPair(L"success", new TJSONBool(true));

	TTagMap Pending;
	{
		TLockGuard Guard(FLock);
		Pending = FPendingPcTags;
		FPendingPcTags.clear();
	}

	if (!Pending.empty())
	{
		TJSONObject *Tags = new TJSONObject();
		TTagMap FormattedPending;
		for (TTagMap::iterator it = Pending.begin(); it != Pending.end(); ++it)
			FormattedPending[FormatProtocolTagKey(it->first)] = it->second;
		for (TTagMap::iterator it = FormattedPending.begin(); it != FormattedPending.end(); ++it)
			Tags->AddPair(it->first, CreateJsonValue(it->second));

		Response->AddPair(L"tags", Tags);
	}

	Result = Response->ToString();
	delete Response;
	return Result;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall TMod_Fms::BuildErrorResponse(const UnicodeString &ErrorText)
{
	TJSONObject *Response = new TJSONObject();
	UnicodeString Result;

	Response->AddPair(L"success", new TJSONBool(false));
	Response->AddPair(L"error", ErrorText);
	Result = Response->ToString();

	delete Response;
	return Result;
}
//---------------------------------------------------------------------------
TJSONValue* __fastcall TMod_Fms::CreateJsonValue(const UnicodeString &JsonText)
{
	TJSONValue *Value = TJSONObject::ParseJSONValue(JsonText, true);
	if (Value != NULL)
		return Value;

	return new TJSONString(JsonText);
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::SetPcTag(const UnicodeString &Key, bool Value)
{
	SetPcTagJson(Key, Value ? L"true" : L"false");
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::SetPcTag(const UnicodeString &Key, int Value)
{
	SetPcTagJson(Key, IntToStr(Value));
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::SetPcTag(const UnicodeString &Key, double Value)
{
	TJSONNumber *JsonNumber = new TJSONNumber(Value);
	SetPcTagJson(Key, JsonNumber->ToString());
	delete JsonNumber;
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::SetPcTag(const UnicodeString &Key, const UnicodeString &Value)
{
	TJSONString *JsonString = new TJSONString(Value);
	SetPcTagJson(Key, JsonString->ToString());
	delete JsonString;
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::SetPcTagJson(const UnicodeString &Key, const UnicodeString &JsonValue)
{
	UnicodeString NormalizedKey = NormalizeTagKeyForDirection(Key, ftdEqpOnly);
	TLockGuard Guard(FLock);
	FPcTags[NormalizedKey] = JsonValue;
	FPendingPcTags[NormalizedKey] = JsonValue;
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::FlushPendingPcTags(bool LogTx)
{
	if (TcpServer == NULL || !TcpServer->Active)
		return;
	if (!IsGatewayConnected())
		return;

	UnicodeString Response = BuildSuccessResponse();
	if (Response.Pos(L"\"tags\"") <= 0)
		return;

	TList *List = TcpServer->Contexts->LockList();
	try
	{
		TLockGuard SendGuard(FSendLock);
		for (int i = 0; i < List->Count; ++i)
		{
			TIdContext *Context = static_cast<TIdContext*>(List->Items[i]);
			if (!FStopping && Context != NULL && Context->Connection != NULL &&
				Context->Connection->IOHandler != NULL &&
				Context->Connection->Connected())
			{
				try
				{
					Context->Connection->IOHandler->Write(
						Response + L"\n", Idglobal::IndyTextEncoding_UTF8());
				}
				catch (Exception &E)
				{
					if (!FStopping)
						LogOpcUa(L"ERROR", L"TCP flush: " + E.Message);
				}
			}
		}
	}
	__finally
	{
		TcpServer->Contexts->UnlockList();
	}

	if (LogTx)
		LogOpcUa(L"TX", Response);
}
//---------------------------------------------------------------------------
bool __fastcall TMod_Fms::GetPcTagJson(const UnicodeString &Key, UnicodeString &JsonValue)
{
	UnicodeString SearchKey = NormalizeTagKeyForDirection(Key, ftdEqpOnly);
	TLockGuard Guard(FLock);

	TTagMap::iterator it = FPcTags.find(SearchKey);
	if (it == FPcTags.end())
	{
		for(it = FPcTags.begin(); it != FPcTags.end(); ++it)
		{
			if(CompatibilityTagKeyEquals(it->first, Key))
				break;
		}
		if(it == FPcTags.end())
			return false;
	}

	JsonValue = it->second;
	return true;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall TMod_Fms::GetPcTagString(const UnicodeString &Key, const UnicodeString &DefaultValue)
{
	UnicodeString JsonValue;
	if (!GetPcTagJson(Key, JsonValue))
		return DefaultValue;

	TJSONValue *Value = TJSONObject::ParseJSONValue(JsonValue, true);
	TJSONString *TextValue = dynamic_cast<TJSONString*>(Value);
	UnicodeString Result = TextValue != NULL ? TextValue->Value() : JsonValue;
	delete Value;
	return Result;
}
//---------------------------------------------------------------------------
bool __fastcall TMod_Fms::GetFmsTagJson(const UnicodeString &Key, UnicodeString &JsonValue)
{
	UnicodeString SearchKey = NormalizeTagKeyForDirection(Key, ftdFmsOnly);
	TLockGuard Guard(FLock);

	TTagMap::iterator it = FFmsTags.find(SearchKey);
	if (it == FFmsTags.end())
	{
		for(it = FFmsTags.begin(); it != FFmsTags.end(); ++it)
		{
			if(CompatibilityTagKeyEquals(it->first, Key))
				break;
		}
		if(it == FFmsTags.end())
			return false;
	}

	JsonValue = it->second;
	return true;
}
//---------------------------------------------------------------------------
unsigned __int64 __fastcall TMod_Fms::GetFmsTagRevision(const UnicodeString &Key)
{
	UnicodeString SearchKey = NormalizeTagKeyForDirection(Key, ftdFmsOnly);
	TLockGuard Guard(FLock);

	TTagRevisionMap::iterator it = FFmsTagRevisions.find(SearchKey);
	if(it != FFmsTagRevisions.end())
		return it->second;

	for(it = FFmsTagRevisions.begin(); it != FFmsTagRevisions.end(); ++it)
	{
		if(CompatibilityTagKeyEquals(it->first, Key))
			return it->second;
	}
	return 0;
}
//---------------------------------------------------------------------------
bool __fastcall TMod_Fms::GetFmsTagBool(const UnicodeString &Key, bool DefaultValue)
{
	UnicodeString JsonValue;
	if (!GetFmsTagJson(Key, JsonValue))
		return DefaultValue;

	UnicodeString Text = JsonValue.Trim().LowerCase();
	if (Text == L"true" || Text == L"1")
		return true;
	if (Text == L"false" || Text == L"0")
		return false;

	return DefaultValue;
}
//---------------------------------------------------------------------------
int __fastcall TMod_Fms::GetFmsTagInt(const UnicodeString &Key, int DefaultValue)
{
	UnicodeString JsonValue;
	if (!GetFmsTagJson(Key, JsonValue))
		return DefaultValue;

	return StrToIntDef(JsonValue.Trim(), DefaultValue);
}
//---------------------------------------------------------------------------
UnicodeString __fastcall TMod_Fms::GetFmsTagString(const UnicodeString &Key, const UnicodeString &DefaultValue)
{
	UnicodeString JsonValue;
	if (!GetFmsTagJson(Key, JsonValue))
		return DefaultValue;

	TJSONValue *Value = TJSONObject::ParseJSONValue(JsonValue, true);
	TJSONString *TextValue = dynamic_cast<TJSONString*>(Value);
	UnicodeString Result = TextValue != NULL ? TextValue->Value() : JsonValue;
	delete Value;
	return Result;
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::ClearFmsTag(const UnicodeString &Key)
{
	UnicodeString SearchKey = NormalizeTagKeyForDirection(Key, ftdFmsOnly);
	TLockGuard Guard(FLock);
	FFmsTags.erase(SearchKey);
	FFmsTagRevisions.erase(SearchKey);
}
//---------------------------------------------------------------------------
bool __fastcall TMod_Fms::IsGatewayConnected(void)
{
	TLockGuard Guard(FLock);
	return FGatewayConnected;
}
//---------------------------------------------------------------------------
bool __fastcall TMod_Fms::GetTagDefinitionInfo(const UnicodeString &Key, TFmsTagDefinition &Definition)
{
	TLockGuard Guard(FLock);
	if (FindTagDefinitionForDirection(Key, ftdEqpOnly, Definition))
		return true;
	if (FindTagDefinitionForDirection(Key, ftdFmsOnly, Definition))
		return true;
	return FindTagDefinition(Key, Definition);
}
//---------------------------------------------------------------------------
bool __fastcall TMod_Fms::IsImplicitDirectionTag(const TFmsTagDefinition &Definition, TFmsTagDirection Direction)
{
	UnicodeString Key = Definition.FullKey.LowerCase();

	if (Key.Pos(L".common.alive") > 0)
		return Direction == ftdFmsOnly || Direction == ftdEqpOnly;

	if (Key.Pos(L".fmsstatus.") > 0)
		return Direction == ftdFmsOnly;
	if (Key.Pos(L".equipmentstatus.") > 0)
		return Direction == ftdEqpOnly;

	if (Key.Pos(L".equipmentcontrol.commandresponse") > 0)
		return Direction == ftdEqpOnly;
	if (Key.Pos(L".equipmentcontrol.command") > 0)
		return Direction == ftdFmsOnly;

	if (Key.Pos(L".trackincellinformation.") > 0)
		return Direction == ftdFmsOnly;
	if (Key.Pos(L".trackoutcellinformation.") > 0)
		return Direction == ftdEqpOnly;

	if (Key.Pos(L".recipe.") > 0)
		return Direction == ftdFmsOnly;

	if (Key.Pos(L".trayprocess.") > 0)
	{
		if (TextEndsWith(Key, L"response"))
			return Direction == ftdFmsOnly;
		return Direction == ftdEqpOnly;
	}

	if (Key.Pos(L".trayinformation.") > 0)
	{
		if (TextEndsWith(Key, L".productmodel") ||
			TextEndsWith(Key, L".routeid") ||
			TextEndsWith(Key, L".processid") ||
			TextEndsWith(Key, L".lotid"))
		{
			return Direction == ftdFmsOnly;
		}

		if (TextEndsWith(Key, L".trayexist") || TextEndsWith(Key, L".trayid"))
			return Direction == ftdEqpOnly;
	}

	if (Key.Pos(L".celltrackout.") > 0 || Key.Pos(L".manualcellout.") > 0)
	{
		if (TextEndsWith(Key, L"response"))
			return Direction == ftdFmsOnly;
		return Direction == ftdEqpOnly;
	}

	return false;
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::AddTagRows(TStrings *Rows, TFmsTagDirection Direction, const TTagMap &Values)
{
	if (Rows == NULL)
		return;

	for (TTagDefinitionMap::iterator it = FTagDefinitions.begin(); it != FTagDefinitions.end(); ++it)
	{
		const TFmsTagDefinition &Definition = it->second;
		if (it->first != Definition.FullKey)
			continue;
		if (Definition.Direction != Direction && Definition.Direction != ftdBoth &&
			!IsImplicitDirectionTag(Definition, Direction))
			continue;

		UnicodeString Value = L"";
		TTagMap::const_iterator ValueIt = Values.find(Definition.FullKey);
		if (ValueIt != Values.end())
			Value = ValueIt->second;

		Rows->Add(FormatGatewayTagKey(Definition.FullKey) + L"\t" + Definition.DataType + L"\t" + Value);
	}
	TStringList *List = dynamic_cast<TStringList*>(Rows);
	if (List != NULL)
		List->Sort();
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::GetTagRows(TStrings *FmsRows, TStrings *PcRows)
{
	TLockGuard Guard(FLock);

	if (FmsRows != NULL)
	{
		FmsRows->Clear();
		AddTagRows(FmsRows, ftdFmsOnly, FFmsTags);
	}

	if (PcRows != NULL)
	{
		PcRows->Clear();
		AddTagRows(PcRows, ftdEqpOnly, FPcTags);
	}
}
//---------------------------------------------------------------------------
void __fastcall TMod_Fms::GetPcTagNames(TStrings *Tags)
{
	if (Tags == NULL)
		return;

	TLockGuard Guard(FLock);
	Tags->Clear();
	for (TTagDefinitionMap::iterator it = FTagDefinitions.begin(); it != FTagDefinitions.end(); ++it)
	{
		const TFmsTagDefinition &Definition = it->second;
		if (it->first == Definition.FullKey &&
			(Definition.Direction == ftdEqpOnly || Definition.Direction == ftdBoth ||
			 IsImplicitDirectionTag(Definition, ftdEqpOnly)))
		{
			Tags->Add(FormatGatewayTagKey(Definition.FullKey));
		}
	}
	TStringList *List = dynamic_cast<TStringList*>(Tags);
	if (List != NULL)
		List->Sort();
}
//---------------------------------------------------------------------------
