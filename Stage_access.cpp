#include <vcl.h>
#pragma hdrstop
#include "FormBase.h"
#include "FormProduction.h"
#pragma package(smart_init)

void __fastcall TBaseForm::UpdateAccessDisplay()
{
    SetAccessLanguage(LangDict);
    bool loggedIn = AccessControl().Can(alOperator);
    lblAccessUser->Caption = "User: " + (loggedIn ? AccessControl().UserId() : UnicodeString("Guest")) +
        " | " + TAccessControl::LevelName(AccessControl().Level());
    lblAccessUser->Hint = lblAccessUser->Caption;
    lblAccessTime->Caption = "Login: " + (loggedIn ?
        AccessControl().LoginAt().FormatString("yyyy-mm-dd hh:nn:ss") : UnicodeString("-"));
    btnProduction->Caption = AccessText("PRODUCTION_TITLE", "Production");
    // Do not change motion, stop, safety, or running timer state on login/logout.
    if(MainForm != NULL) MainForm->chkBypass->Enabled = AccessControl().Can(alEngineer);
    if(teachForm != NULL){
        for(int i = 0; i < teachForm->ComponentCount; ++i){
            TEdit *edit = dynamic_cast<TEdit*>(teachForm->Components[i]);
            if(edit != NULL) edit->ReadOnly = !AccessControl().Can(alEngineer);
        }
    }
    if(ProductionForm != NULL && ProductionForm->Visible) ProductionForm->Prepare();
}
