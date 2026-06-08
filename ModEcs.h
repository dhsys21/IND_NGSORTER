//---------------------------------------------------------------------------

#ifndef ModEcsH
#define ModEcsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <System.Win.ScktComp.hpp>
//---------------------------------------------------------------------------

class TEcsMod : public TDataModule
{
__published:	// IDE-managed Components
	TClientSocket *ecsSock;
	TTimer *ReContactTimer;
	void __fastcall ReContactTimerTimer(TObject *Sender);
	void __fastcall ecsSockConnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall ecsSockDisconnect(TObject *Sender, TCustomWinSocket *Socket);
	void __fastcall ecsSockError(TObject *Sender, TCustomWinSocket *Socket, TErrorEvent ErrorEvent,
          int &ErrorCode);
	void __fastcall ecsSockRead(TObject *Sender, TCustomWinSocket *Socket);

private:	// User declarations
public:		// User declarations

	bool auto_contact;
	TCustomWinSocket *ecs_sock;

	void __fastcall TEcsMod::SendMsg(AnsiString stage, AnsiString status, AnsiString in, AnsiString out);
    void __fastcall TEcsMod::WriteEcsLog(int type, AnsiString msg);
	__fastcall TEcsMod(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TEcsMod *EcsMod;
//---------------------------------------------------------------------------
#endif
