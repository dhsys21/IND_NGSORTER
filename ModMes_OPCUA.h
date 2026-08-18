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
public:		// User declarations
	__fastcall TMesOpc(TComponent* Owner);
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
    void __fastcall PROCESS_END_REQUEST();
    bool __fastcall PROCESS_END_RESPONSE();
};
//---------------------------------------------------------------------------
extern PACKAGE TMesOpc *MesOpc;
//---------------------------------------------------------------------------
#endif
