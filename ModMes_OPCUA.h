//---------------------------------------------------------------------------

#ifndef ModMes_OPCUAH
#define ModMes_OPCUAH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
//---------------------------------------------------------------------------
class TMesOpc : public TDataModule
{
__published:	// IDE-managed Components
private:	// User declarations
	struct TApprovedCell {
		AnsiString CellId, LotId, NGCode, Grade;
		bool Exists, WorkFlag;
	};
	TApprovedCell FApprovedSource[96];
	UnicodeString FApprovedSourceTrayId;
	void CaptureApprovedSource(const UnicodeString &TrayId);
    bool FShutdown;
	bool FEnvStatusInitialized;
	double FLastEnvTemperature;
	bool FLastEnvSmokeDetected;
	bool FLastEnvTempWarning;
	bool FLastEnvTempDanger;
	bool FLastEnvRunning;
    int FLastCellTrackOutSourceChannel;
    int FLastCellTrackOutTargetChannel;
    AnsiString FLastCellTrackOutCellId;
    UnicodeString FLastCellTrackOutSourceTrayId;
    UnicodeString FLastCellTrackOutTargetTrayId;
    bool FLastCellTrackOutValid;
public:		// User declarations
	bool ReadApprovedSource(int CellNo, AnsiString &CellId, AnsiString &LotId,
		AnsiString &NGCode, AnsiString &Grade, bool &WorkFlag);
	__fastcall TMesOpc(TComponent* Owner);
    void __fastcall Shutdown();
	// Publishes one coherent TSD-V50 state through F1NGS01.EnvStatus EQP-only tags.
	void __fastcall PublishEnvStatus(double Temperature, bool SmokeDetected,
		bool TempWarning, bool TempDanger, bool Running);
    bool __fastcall DISPLAY_TRACK_IN_TRAYS();
    void __fastcall TRAY_LOAD_REQUEST();
    void __fastcall TRAY_LOAD_REQUEST(bool SourceTray);
    void __fastcall TRAY_LOAD_CANCEL(bool SourceTray);
    bool __fastcall TRAY_LOAD_RESPONSE();
    int __fastcall TRAY_LOAD_RESPONSE(bool SourceTray);
    int __fastcall TRAY_LOAD_RESPONSE_VALUE(bool SourceTray);
    AnsiString __fastcall TRAY_LOAD_VALIDATION_ERROR(bool SourceTray) const;
	void __fastcall LogTrayLoadTimeout(bool SourceTray);
	void __fastcall LogTrayLoadResponseOffTimeout(bool SourceTray);
    void __fastcall RECIPE_REQUEST();
    bool __fastcall RECIPE_RESPONSE();
    void __fastcall PROCESS_START_REQUEST();
    void __fastcall PROCESS_START_CANCEL();
    bool __fastcall PROCESS_START_RESPONSE();
    int __fastcall PROCESS_START_RESPONSE_RESULT();
    int __fastcall PROCESS_START_RESPONSE_VALUE();
    void __fastcall LogProcessStartResponseOffTimeout();
    void __fastcall PROCESS_DATA_WRITE();
    void __fastcall CLEAR_TRACK_OUT_CELL_INFORMATION();
    void __fastcall CLEAR_CELL_TRACK_OUT_DATA();
    bool __fastcall READ_TRACK_IN_CELL(int SourceCellNo, AnsiString &CellId,
        AnsiString &LotId, AnsiString &NGCode, AnsiString &Grade);
    void __fastcall CELL_TRACK_OUT_REQUEST(int SourceChannel, int TargetChannel,
        const AnsiString &CellId);
    void __fastcall CELL_TRACK_OUT_REQUEST(int SourceChannel, int TargetChannel,
        const AnsiString &CellId, const UnicodeString &SourceTrayId,
        const UnicodeString &TargetTrayId);
    int __fastcall CELL_TRACK_OUT_RESPONSE_RESULT();
	int __fastcall CELL_TRACK_OUT_RESPONSE_VALUE();
	bool CELL_TRACK_OUT_WRITE_COMPLETE(bool requestOn);
    void __fastcall LogCellTrackOutTimeout();
    void __fastcall LogCellTrackOutResponseOffTimeout();
    bool __fastcall CELL_TRACK_OUT_RETRY();
    void __fastcall CELL_TRACK_OUT_CANCEL();
    void __fastcall TRAY_UNLOAD_REQUEST();
    int __fastcall TRAY_UNLOAD_RESPONSE_RESULT();
    int __fastcall TRAY_UNLOAD_RESPONSE_VALUE();
    void __fastcall LogTrayUnloadTimeout();
    void __fastcall LogTrayUnloadResponseOffTimeout();
    void __fastcall TRAY_UNLOAD_CANCEL();
    void __fastcall PROCESS_END_REQUEST();
    bool __fastcall PROCESS_END_RESPONSE();
    int __fastcall PROCESS_END_RESPONSE_RESULT();
    int __fastcall PROCESS_END_RESPONSE_VALUE();
    void __fastcall LogProcessEndTimeout();
    void __fastcall LogProcessEndResponseOffTimeout();
    void __fastcall PROCESS_END_CANCEL();
};
//---------------------------------------------------------------------------
extern PACKAGE TMesOpc *MesOpc;
//---------------------------------------------------------------------------
#endif
