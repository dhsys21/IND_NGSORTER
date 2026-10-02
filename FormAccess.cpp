#include <vcl.h>
#pragma hdrstop
#include "FormAccess.h"
#include "FormAccessGuide.h"
#pragma package(smart_init)
#pragma link "AdvSmoothButton"
#pragma resource "*.dfm"
TAccessForm *AccessForm;

__fastcall TAccessForm::TAccessForm(TComponent *Owner) : TForm(Owner),
    FChangingPassword(false), FSelectedLevel(alOperator) {}

void TAccessForm::Prepare()
{
    // Layout, colours and event bindings remain editable in the form designer.
    Caption = AccessText("ACCESS_TITLE", "User access");
    pnlTitle->Hint = AccessText("ACCESS_GUIDE_HINT", "View permissions by level");
    for(int i = 0; i < ComponentCount; ++i){
        TComponent *c = Components[i];
        UnicodeString text = AccessText("UI_ACCESSFORM_" + c->Name.UpperCase(), "");
        if(text.IsEmpty()) continue;
        if(TPanel *panel = dynamic_cast<TPanel*>(c)) panel->Caption = text;
        else if(TAdvSmoothButton *button = dynamic_cast<TAdvSmoothButton*>(c)) button->Caption = text;
    }
    // Always start with Operator selected; the current session is unchanged until login/logout.
    SelectLevel(alOperator);
    btnLogout->Enabled = AccessControl().Can(alOperator);
    editLoginPassword->Hint = AccessText("ACCESS_LOGIN_HINT", "Select a level, enter the password and press Enter.");
    ActiveControl = editLoginPassword;
}

void TAccessForm::SelectLevel(TAccessLevel Level)
{
    // A highlighted level is only a selection, not authorization.
    FSelectedLevel = Level;
    TAdvSmoothButton *buttons[] = {btnOperator, btnEngineer, btnAdmin};
    for(int i = 0; i < 3; ++i){
        bool selected = static_cast<int>(Level) == i + 1;
        buttons[i]->Color = static_cast<TColor>(selected ? RGB(137,197,45) : RGB(100,104,109));
        buttons[i]->Appearance->Font->Color = selected ? clBlack : clWhite;
    }
    editLoginPassword->Clear();
    editNewPassword->Clear();
    SetPasswordChangeMode(false);
    lblStatus->Caption = "";
    if(Visible) editLoginPassword->SetFocus();
}

void TAccessForm::SetPasswordChangeMode(bool Editing)
{
    // The expanded layout stays in the DFM. Derive both runtime heights from its panels.
    // Move messages with the bottom panel so login/save errors remain visible in either state.
    if(!Editing && Visible) editLoginPassword->SetFocus();
    FChangingPassword = Editing;
    pnlNewPasswordTitle->Visible = Editing;
    pnlNewPassword->Visible = Editing;
    editNewPassword->Enabled = Editing;
    btnPasswordSave->Enabled = Editing;
    TPanel *bottomPanel = Editing ? pnlNewPassword : pnlLogin;
    lblStatus->Top = bottomPanel->Top + bottomPanel->Height + 8;
    ClientHeight = lblStatus->Top + lblStatus->Height + 8;
}

void __fastcall TAccessForm::btnOperatorClick(TObject *Sender) { SelectLevel(alOperator); }
void __fastcall TAccessForm::btnEngineerClick(TObject *Sender) { SelectLevel(alEngineer); }
void __fastcall TAccessForm::btnAdminClick(TObject *Sender) { SelectLevel(alAdmin); }

void __fastcall TAccessForm::pnlTitleClick(TObject *Sender)
{
    // Information only: leave the selected role, passwords and active session unchanged.
    TAccessGuideForm *guide = new TAccessGuideForm(this);
    try{
        guide->Prepare();
        guide->ShowModal();
    }__finally{
        delete guide;
    }
}

void TAccessForm::Login()
{
    if(AccessControl().Login(FSelectedLevel, editLoginPassword->Text)){
        editLoginPassword->Clear();
        ModalResult = mrOk;
    }else{
        editLoginPassword->Clear();
        lblStatus->Caption = AccessText("ACCESS_BAD_LOGIN", "Check the selected level and password.");
        if(Visible) editLoginPassword->SetFocus();
    }
}

void __fastcall TAccessForm::btnLogoutClick(TObject *Sender)
{
    AccessControl().Logout();
    editLoginPassword->Clear();
    editNewPassword->Clear();
    ModalResult = mrOk; // Does not stop measurement or change machine mode.
}

void __fastcall TAccessForm::btnPasswordChangeClick(TObject *Sender)
{
    // Upper Password = current password of the selected role; lower Input = new password.
    // Selecting Admin or changing its password never grants the Admin role by itself.
    SetPasswordChangeMode(true);
    editNewPassword->Clear();
    lblStatus->Caption = AccessText("ACCESS_PASSWORD_INPUT", "Enter the current password above and the new password below.");
    if(Visible) editLoginPassword->SetFocus();
}

void __fastcall TAccessForm::btnPasswordSaveClick(TObject *Sender)
{
    if(!FChangingPassword) return;
    try{
        AccessControl().ChangePassword(FSelectedLevel, editLoginPassword->Text, editNewPassword->Text);
        SelectLevel(FSelectedLevel);
        lblStatus->Caption = AccessText("ACCESS_SAVED", "Saved.");
    }catch(const Exception &){
        editLoginPassword->Clear();
        editNewPassword->Clear();
        lblStatus->Caption = AccessText("ACCESS_PASSWORD_FAILED", "Cannot change password. Check the current/new password and account file.");
    }
}

void __fastcall TAccessForm::editCredentialKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if(Key != VK_RETURN) return;
    Key = 0;
    if(FChangingPassword) btnPasswordSaveClick(btnPasswordSave);
    else Login();
}

void __fastcall TAccessForm::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if(Key == VK_ESCAPE){ Key = 0; ModalResult = mrCancel; }
}
