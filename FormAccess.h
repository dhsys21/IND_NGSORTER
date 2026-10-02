#ifndef FormAccessH
#define FormAccessH
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "AdvSmoothButton.hpp"
#include <Vcl.Forms.hpp>
#include "AccessControl.h"

class TAccessForm : public TForm
{
__published:
    TPanel *pnlTitle;
    TPanel *pnlLogin;
    TPanel *pnlLevel;
    TPanel *pnlLoginPassword;
    TPanel *pnlPasswordEntry;
    TAdvSmoothButton *btnOperator;
    TAdvSmoothButton *btnEngineer;
    TAdvSmoothButton *btnAdmin;
    TAdvSmoothButton *btnPasswordChange;
    TAdvSmoothButton *btnLogout;
    TEdit *editLoginPassword;
    TPanel *pnlNewPasswordTitle;
    TPanel *pnlNewPassword;
    TPanel *pnlNewPasswordInput;
    TPanel *pnlNewPasswordEntry;
    TEdit *editNewPassword;
    TAdvSmoothButton *btnPasswordSave;
    TLabel *lblStatus;
    void __fastcall pnlTitleClick(TObject *Sender);
    void __fastcall btnOperatorClick(TObject *Sender);
    void __fastcall btnEngineerClick(TObject *Sender);
    void __fastcall btnAdminClick(TObject *Sender);
    void __fastcall btnLogoutClick(TObject *Sender);
    void __fastcall btnPasswordChangeClick(TObject *Sender);
    void __fastcall btnPasswordSaveClick(TObject *Sender);
    void __fastcall editCredentialKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
private:
    bool FChangingPassword;
    TAccessLevel FSelectedLevel;
    void SelectLevel(TAccessLevel Level);
    void SetPasswordChangeMode(bool Editing);
    void Login();
public:
    __fastcall TAccessForm(TComponent *Owner);
    void Prepare();
};
extern PACKAGE TAccessForm *AccessForm;
#endif
