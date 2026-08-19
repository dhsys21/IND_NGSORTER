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
    bool FShutdown;
public:		// User declarations
	__fastcall TMesOpc(TComponent* Owner);
    void __fastcall Shutdown();
    void __fastcall TRAY_LOAD_REQUEST();
    void __fastcall TRAY_LOAD_REQUEST(bool SourceTray);
    void __fastcall TRAY_LOAD_CANCEL(bool SourceTray);
    bool __fastcall TRAY_LOAD_RESPONSE();
    int __fastcall TRAY_LOAD_RESPONSE(bool SourceTray);
	void __fastcall LogTrayLoadTimeout(bool SourceTray);
    void __fastcall RECIPE_REQUEST();
    bool __fastcall RECIPE_RESPONSE();
    void __fastcall PROCESS_START_REQUEST();
    void __fastcall PROCESS_START_CANCEL();
    bool __fastcall PROCESS_START_RESPONSE();
    int __fastcall PROCESS_START_RESPONSE_RESULT();
    void __fastcall PROCESS_DATA_WRITE();
    bool __fastcall READ_TRACK_IN_CELL(int SourceCellNo, AnsiString &CellId,
        AnsiString &LotId, AnsiString &NGCode, AnsiString &Grade);
    void __fastcall CELL_TRACK_OUT_REQUEST(int SourceChannel, int TargetChannel,
        const AnsiString &CellId);
    void __fastcall CELL_TRACK_OUT_REQUEST(int SourceChannel, int TargetChannel,
        const AnsiString &CellId, const UnicodeString &SourceTrayId,
        const UnicodeString &TargetTrayId);
    int __fastcall CELL_TRACK_OUT_RESPONSE_RESULT();
    void __fastcall LogCellTrackOutTimeout();
    void __fastcall CELL_TRACK_OUT_CANCEL();
    void __fastcall TRAY_UNLOAD_REQUEST();
    int __fastcall TRAY_UNLOAD_RESPONSE_RESULT();
    void __fastcall LogTrayUnloadTimeout();
    void __fastcall TRAY_UNLOAD_CANCEL();
    void __fastcall PROCESS_END_REQUEST();
    bool __fastcall PROCESS_END_RESPONSE();
    int __fastcall PROCESS_END_RESPONSE_RESULT();
    void __fastcall LogProcessEndTimeout();
    void __fastcall PROCESS_END_CANCEL();
};
//---------------------------------------------------------------------------
extern PACKAGE TMesOpc *MesOpc;
//---------------------------------------------------------------------------
#endif
