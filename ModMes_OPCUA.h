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
    bool __fastcall TRAY_LOAD_RESPONSE();
    void __fastcall RECIPE_REQUEST();
    bool __fastcall RECIPE_RESPONSE();
    void __fastcall PROCESS_START_REQUEST();
    bool __fastcall PROCESS_START_RESPONSE();
    void __fastcall PROCESS_DATA_WRITE();
    void __fastcall PROCESS_END_REQUEST();
    bool __fastcall PROCESS_END_RESPONSE();
};
//---------------------------------------------------------------------------
extern PACKAGE TMesOpc *MesOpc;
//---------------------------------------------------------------------------
#endif
