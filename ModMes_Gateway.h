//---------------------------------------------------------------------------

#ifndef ModMes_GatewayH
#define ModMes_GatewayH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <System.JSON.hpp>
#include <System.SyncObjs.hpp>
#include <System.SysUtils.hpp>
#include <IdContext.hpp>
#include <IdTCPServer.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdCustomTCPServer.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Xml.XMLIntf.hpp>
#include <deque>
#include <map>

//---------------------------------------------------------------------------
enum TFmsTagDirection
{
	ftdUnknown,
	ftdEqpOnly,
	ftdFmsOnly,
	ftdBoth
};

struct TFmsTagDefinition
{
	System::UnicodeString NodeId;
	System::UnicodeString FullKey;
	System::UnicodeString ShortKey;
	System::UnicodeString DataType;
	System::UnicodeString ArrayDimensions;
	int ValueRank;
	int AccessLevel;
	TFmsTagDirection Direction;

	__fastcall TFmsTagDefinition(void)
		: ValueRank(-1),
		  AccessLevel(0),
		  Direction(ftdUnknown)
	{
	}
};

class TMod_Fms : public TDataModule
{
__published:	// IDE-managed Components
	TIdTCPServer *TcpServer;
	TTimer *Timer_Alive;
	TTimer *Timer_Reconnect;
	TTimer *Timer_LogDispatch;
	void __fastcall TcpServerConnect(TIdContext *AContext);
	void __fastcall TcpServerDisconnect(TIdContext *AContext);
	void __fastcall TcpServerExecute(TIdContext *AContext);
	void __fastcall Timer_AliveTimer(TObject *Sender);
	void __fastcall Timer_ReconnectTimer(TObject *Sender);
	void __fastcall Timer_LogDispatchTimer(TObject *Sender);

private:	// User declarations
	typedef std::map<System::UnicodeString, System::UnicodeString> TTagMap;
	typedef std::map<System::UnicodeString, unsigned __int64> TTagRevisionMap;
	typedef std::map<System::UnicodeString, TFmsTagDefinition> TTagDefinitionMap;
	struct TFmsQueuedLog
	{
		System::UnicodeString Type;
		System::UnicodeString Message;
		bool Display;
	};

	System::Syncobjs::TCriticalSection *FLock;
	System::Syncobjs::TCriticalSection *FSendLock;
	System::Syncobjs::TCriticalSection *FLogLock;
	std::deque<TFmsQueuedLog> FQueuedLogs;
	TTagMap FFmsTags;
	TTagRevisionMap FFmsTagRevisions;
	unsigned __int64 FFmsRevisionCounter;
	TTagMap FPcTags;
	TTagMap FPendingPcTags;
	TTagMap FStagedPcTags;
	TIdContext *FActiveContext;
	TTagMap FLocalPcTags;
	TTagMap FInFlightPcTags;
	void AcknowledgePcTags();
	TTagDefinitionMap FTagDefinitions;
	bool FSnapshotReceived;
	bool FTagConfigLoaded;
	bool FGatewayConnected;
	bool FStopping;
	System::UnicodeString FLastEquipment;
	System::UnicodeString FLastTimestamp;
	System::UnicodeString FBindIp;
	int FBindPort;
	bool FAutoStartEnabled;

	bool __fastcall TryStartServer(void);
	void __fastcall LogOpcUa(const System::UnicodeString &Type, const System::UnicodeString &Msg, bool bDisplay = true);
	void __fastcall LoadTagConfig(const System::UnicodeString &FileName);
	void __fastcall ReadTagNodes(Xml::Xmlintf::_di_IXMLNode Node);
	void __fastcall RegisterTagDefinition(const TFmsTagDefinition &Definition);
	void __fastcall RegisterTagAlias(const System::UnicodeString &Alias, const TFmsTagDefinition &Definition);
	bool __fastcall FindTagDefinition(const System::UnicodeString &Key, TFmsTagDefinition &Definition);
	bool __fastcall FindTagDefinitionForDirection(const System::UnicodeString &Key, TFmsTagDirection Direction, TFmsTagDefinition &Definition);
	bool __fastcall KeyMatchesDefinition(const System::UnicodeString &Key, const TFmsTagDefinition &Definition);
	System::UnicodeString __fastcall NormalizeTagKey(const System::UnicodeString &Key);
	System::UnicodeString __fastcall NormalizeTagKeyForDirection(const System::UnicodeString &Key, TFmsTagDirection Direction);
	bool __fastcall ValidateJsonValue(const TFmsTagDefinition &Definition, System::Json::TJSONValue *Value);
	System::UnicodeString __fastcall HandleMessage(const System::UnicodeString &Line);
	void __fastcall ApplySnapshot(System::Json::TJSONObject *Json);
	void __fastcall ApplyChangedTags(System::Json::TJSONObject *Json);
	void __fastcall CopyTags(System::Json::TJSONObject *Tags, TTagMap &Target, TFmsTagDirection Direction);
	void __fastcall StoreFmsTag(const System::UnicodeString &Key, const System::UnicodeString &Value);
	void __fastcall CopyChangedTags(System::Json::TJSONObject *Tags);

	System::UnicodeString __fastcall BuildSuccessResponse(void);
	System::UnicodeString __fastcall BuildErrorResponse(const System::UnicodeString &ErrorText);
	System::Json::TJSONValue* __fastcall CreateJsonValue(const System::UnicodeString &JsonText);
	void __fastcall AddTagRows(System::Classes::TStrings *Rows, TFmsTagDirection Direction, const TTagMap &Values);
	bool __fastcall IsImplicitDirectionTag(const TFmsTagDefinition &Definition, TFmsTagDirection Direction);

public:		// User declarations
	__fastcall TMod_Fms(TComponent* Owner);
	__fastcall virtual ~TMod_Fms(void);

	void __fastcall Start(void);
	void __fastcall Stop(void);
	void __fastcall Configure(const System::UnicodeString &BindIp, int BindPort);

	void __fastcall SetPcTag(const System::UnicodeString &Key, bool Value);
	void __fastcall SetPcTag(const System::UnicodeString &Key, int Value);
	void __fastcall SetPcTag(const System::UnicodeString &Key, double Value);
	void __fastcall SetPcTag(const System::UnicodeString &Key, const System::UnicodeString &Value);
	void __fastcall SetPcTagJson(const System::UnicodeString &Key, const System::UnicodeString &JsonValue);
	void __fastcall FlushPendingPcTags(bool LogTx = true);
	void SetPcEnvStatus(const System::UnicodeString &Prefix, double Temperature,
		bool Smoke, bool Warning, bool Danger, bool Running);
	bool __fastcall GetPcTagJson(const System::UnicodeString &Key, System::UnicodeString &JsonValue);
	System::UnicodeString __fastcall GetPcTagString(const System::UnicodeString &Key, const System::UnicodeString &DefaultValue = L"");
	bool __fastcall GetFmsTagJson(const System::UnicodeString &Key, System::UnicodeString &JsonValue);
	unsigned __int64 __fastcall GetFmsTagRevision(const System::UnicodeString &Key);
	bool __fastcall GetFmsTagBool(const System::UnicodeString &Key, bool DefaultValue = false);
	int __fastcall GetFmsTagInt(const System::UnicodeString &Key, int DefaultValue = 0);
	System::UnicodeString __fastcall GetFmsTagString(const System::UnicodeString &Key, const System::UnicodeString &DefaultValue = L"");
	void __fastcall ClearFmsTag(const System::UnicodeString &Key);
	bool __fastcall IsGatewayConnected(void);
	bool __fastcall GetTagDefinitionInfo(const System::UnicodeString &Key, TFmsTagDefinition &Definition);
	void __fastcall GetTagRows(System::Classes::TStrings *FmsRows, System::Classes::TStrings *PcRows);
	void __fastcall GetPcTagNames(System::Classes::TStrings *Tags);

	__property bool SnapshotReceived = {read=FSnapshotReceived};
	__property System::UnicodeString LastEquipment = {read=FLastEquipment};
	__property System::UnicodeString LastTimestamp = {read=FLastTimestamp};
};
//---------------------------------------------------------------------------
extern PACKAGE TMod_Fms *Mod_Fms;
//---------------------------------------------------------------------------
#endif
