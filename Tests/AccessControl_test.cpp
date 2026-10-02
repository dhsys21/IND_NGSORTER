#include <vcl.h>
#pragma hdrstop
#include "../AccessControl.h"
#include "../FormAccess.h"
#include "../FormAccessGuide.h"
#include <System.JSON.hpp>
#include <Vcl.Imaging.pngimage.hpp>
#include <stdio.h>
#include <memory>

static int Reports = 0;
static UnicodeString ReportAccount;
static bool ReportLoggedIn = false;
static void Report(const UnicodeString &Account, TAccessLevel, bool LoggedIn)
{
    ++Reports;
    ReportAccount = Account;
    ReportLoggedIn = LoggedIn;
}
static void Check(bool ok, const char *message)
{
    if(!ok) throw Exception(message);
    FILE *trace = fopen("tmp/Access20261001/test_trace.txt", "a");
    fprintf(trace, "PASS: %s\n", message);
    fclose(trace);
}
static void Capture(TForm *form, const UnicodeString &file)
{
    form->Show();
    form->Update();
    std::auto_ptr<Graphics::TBitmap> bitmap(form->GetFormImage());
    std::auto_ptr<TPngImage> png(new TPngImage());
    png->Assign(bitmap.get());
    png->SaveToFile(file);
    form->Hide();
}
static void EnterPassword(TAccessForm *form, const UnicodeString &Password)
{
    form->editLoginPassword->Text = Password;
    WORD key = VK_RETURN;
    form->editCredentialKeyDown(form->editLoginPassword, key, TShiftState());
    Check(key == 0, "Enter consumed by credential handler");
}
static void SaveText(const UnicodeString &file, const UnicodeString &content)
{
    std::auto_ptr<TStringList> text(new TStringList());
    text->Text = content;
    text->SaveToFile(file, TEncoding::UTF8);
}
int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
    SetCurrentDir(ExpandFileName(ExtractFilePath(ParamStr(0)) + "..\\.."));
    FILE *result = fopen("tmp/Access20261001/test_result.txt", "w");
    try{
        Application->Initialize();
        UnicodeString folder = ExpandFileName("tmp/Access20261001/fixed_" + IntToStr((int)GetCurrentProcessId())) + "\\";
        ForceDirectories(folder);
        UnicodeString file = folder + "AccessUsers.json";
        TAccessControl &access = AccessControl();
        access.OnSessionChanged = Report;
        access.Initialize(file, folder);
        Check(access.LoadError().IsEmpty() && !access.Can(alOperator), "New account file starts logged out");
        for(int i = 1; i <= 3; ++i){
            TAccessLevel level = static_cast<TAccessLevel>(i);
            Check(access.Login(level, TAccessControl::LevelName(level).LowerCase()), "Default password equals lowercase fixed ID");
            Check(access.Level() == level && access.UserId() == TAccessControl::LevelName(level).LowerCase(), "Fixed ID and permissions");
            access.Logout();
        }
        Check(!access.Login(alAdmin, "incorrect") && !access.Can(alOperator), "Bad password cannot elevate");
        Check(!access.Login(alGuest, "") && !access.Login(static_cast<TAccessLevel>(4), "admin"), "Invalid role rejected");
        {
            std::auto_ptr<TAccessForm> form(new TAccessForm(NULL));
            form->Prepare();
            Check(!form->pnlNewPassword->Visible && !form->pnlNewPasswordTitle->Visible,
                "New Password section hidden on entry");
            Check(form->ClientHeight == 272 && form->lblStatus->Top == 228,
                "Login window collapsed with status message inside its bounds");
            int reports = Reports;
            form->btnAdmin->OnClick(form->btnAdmin);
            Check(!access.Can(alOperator) && Reports == reports, "Role selection does not grant rights or report login");
            EnterPassword(form.get(), "operator");
            Check(!access.Can(alOperator) && form->ModalResult != mrOk, "Selected role controls password check");
            EnterPassword(form.get(), "admin");
            Check(access.Can(alAdmin) && form->ModalResult == mrOk && ReportAccount == "admin" && ReportLoggedIn,
                "Password Enter logs in and reports fixed account");
            form->Prepare();
            Check(form->btnOperator->Color == static_cast<TColor>(RGB(137,197,45)) && access.Can(alAdmin),
                "Reopening selects Operator without changing existing session");
            EnterPassword(form.get(), "operator");
            Check(access.Level() == alOperator, "Default Operator accepts password without clicking a role");
            form->btnLogout->OnClick(form->btnLogout);
            Check(!access.Can(alOperator) && !ReportLoggedIn && ReportAccount == "operator", "Logout callback includes old account");
        }
        {
            std::auto_ptr<TAccessForm> form(new TAccessForm(NULL));
            form->Prepare();
            form->btnEngineer->OnClick(form->btnEngineer);
            form->btnPasswordChange->OnClick(form->btnPasswordChange);
            Check(form->editNewPassword->Enabled && form->btnPasswordSave->Enabled &&
                form->pnlNewPassword->Visible && form->pnlNewPasswordTitle->Visible, "Password Change shows and enables lower section");
            Check(form->ClientHeight == 452 && form->lblStatus->Top == 408,
                "Password Change expands to the complete designer layout");
            form->editLoginPassword->Text = "wrong";
            form->editNewPassword->Text = "eng-new";
            form->btnPasswordSave->OnClick(form->btnPasswordSave);
            Check(!access.Login(alEngineer, "eng-new"), "Wrong current password cannot change credential");
            Check(form->ClientHeight == 452 && form->pnlNewPassword->Visible,
                "Failed save keeps password editor expanded for correction");
            form->editLoginPassword->Text = "engineer";
            form->editNewPassword->Text = "eng-new";
            form->btnPasswordSave->OnClick(form->btnPasswordSave);
            Check(!access.Can(alOperator) && !form->editNewPassword->Enabled && !form->pnlNewPassword->Visible,
                "Password save hides the editor without logging in or elevating");
            Check(form->ClientHeight == 272 && form->lblStatus->Top + form->lblStatus->Height <= form->ClientHeight,
                "Successful Save returns to the compact window with visible confirmation");
            Capture(form.get(), "tmp/Access20261001/fixed_saved.png");
            Check(!access.Login(alEngineer, "engineer") && access.Login(alEngineer, "eng-new"), "New password stored, old password invalid");
            Check(!access.Can(alAdmin), "Engineer cannot use Admin controls");
            access.Logout();
        }
        bool rejected = false;
        try{ access.ChangePassword(alAdmin, "admin", ""); }catch(Exception &){ rejected = true; }
        Check(rejected, "Blank new password rejected");
        access.Initialize(file, folder);
        Check(!access.Can(alOperator) && access.Login(alEngineer, "eng-new"), "Reload preserves password, not session");
        access.Logout();
        {
            std::auto_ptr<TAccessForm> form(new TAccessForm(NULL));
            std::auto_ptr<TStringList> lang(new TStringList());
            for(int i = 0; i < 2; ++i){
                lang->LoadFromFile(i == 0 ? "Lang_En.ini" : "Lang_Ko.ini", TEncoding::UTF8);
                SetAccessLanguage(lang.get());
                form->Prepare();
                Check(form->pnlTitle->OnClick != NULL, "Permission title click event is bound in DFM");
                {
                    int reports = Reports;
                    TAccessLevel before = access.Level();
                    std::auto_ptr<TAccessGuideForm> guide(new TAccessGuideForm(NULL));
                    guide->Prepare();
                    Check(guide->gridPermissions->RowCount == 18 && guide->gridPermissions->ColCount == 4,
                        "Permission guide lists 17 functions for three fixed roles");
                    Check(!guide->gridPermissions->Options.Contains(goEditing), "Permission matrix is read-only");
                    for(int col = 1; col <= 3; ++col)
                        Check(guide->gridPermissions->Cells[col][3] == lang->Values["ACCESS_GUIDE_ALLOWED"],
                            "Remeasurement is allowed for every role in guide");
                    Check(guide->gridPermissions->Cells[1][6] == lang->Values["ACCESS_GUIDE_BLOCKED"] &&
                        guide->gridPermissions->Cells[2][6] == lang->Values["ACCESS_GUIDE_ALLOWED"], "Mode changes require Engineer");
                    Check(guide->gridPermissions->Cells[2][15] == lang->Values["ACCESS_GUIDE_BLOCKED"] &&
                        guide->gridPermissions->Cells[3][15] == lang->Values["ACCESS_GUIDE_ALLOWED"], "Configuration save requires Admin");
                    Capture(guide.get(), "tmp/Access20261001/access_guide_" + IntToStr(i) + ".png");
                    Check(access.Level() == before && Reports == reports, "Viewing guide does not alter session or report login");
                }
                form->btnEngineer->OnClick(form->btnEngineer);
                Check(form->btnAdmin->Caption == "Admin", "Admin caption in both languages");
                Check(form->FindComponent("editLoginId") == NULL && form->FindComponent("pnlUsers") == NULL,
                    "No arbitrary ID or account-management controls");
                Capture(form.get(), "tmp/Access20261001/fixed_login_" + IntToStr(i) + ".png");
                form->btnPasswordChange->OnClick(form->btnPasswordChange);
                Capture(form.get(), "tmp/Access20261001/fixed_password_" + IntToStr(i) + ".png");
            }
            SetAccessLanguage(NULL);
        }
        std::auto_ptr<TStringList> text(new TStringList());
        text->LoadFromFile(file, TEncoding::UTF8);
        std::auto_ptr<TJSONValue> root(TJSONObject::ParseJSONValue(text->Text));
        TJSONArray *users = dynamic_cast<TJSONArray*>(dynamic_cast<TJSONObject*>(root.get())->GetValue("users"));
        Check(users != NULL && users->Count == 3 && text->Text.Pos("eng-new") > 0, "Three fixed accounts, requested plaintext storage");
        text->LoadFromFile(folder + "access_" + Now().FormatString("yyyymmdd") + ".log", TEncoding::UTF8);
        Check(text->Text.Pos("LOGIN_SUCCESS") && text->Text.Pos("LOGIN_FAILED") && text->Text.Pos("LOGOUT") &&
            text->Text.Pos("PASSWORD_CHANGED") && text->Text.Pos("PASSWORD_CHANGE_FAILED"), "Audit events retained");
        Check(!text->Text.Pos("eng-new") && !text->Text.Pos("incorrect"), "Passwords excluded from audit log");
        UnicodeString legacy = folder + "legacy.json";
        SaveText(legacy, "{\"users\":[{\"id\":\"old-master\",\"password\":\"kept-secret\",\"level\":3}]}");
        access.Initialize(legacy, folder);
        Check(access.LoadError().IsEmpty() && access.Login(alAdmin, "kept-secret") && FileExists(legacy + ".legacy.bak"),
            "Legacy Master migrates to Admin with original password and backup");
        access.Logout();
        Check(access.Login(alOperator, "operator"), "Missing legacy role gets site default");
        UnicodeString duplicate = folder + "duplicate.json";
        SaveText(duplicate, "{\"users\":[{\"id\":\"one\",\"password\":\"a\",\"level\":3},{\"id\":\"two\",\"password\":\"b\",\"level\":3}]}");
        access.Initialize(duplicate, folder);
        Check(!access.LoadError().IsEmpty() && !access.Login(alAdmin, "admin"), "Ambiguous legacy passwords fail closed");
        UnicodeString broken = folder + "broken.json";
        SaveText(broken, "{broken");
        access.Initialize(broken, folder);
        Check(!access.LoadError().IsEmpty() && !access.Login(alAdmin, "admin"), "Corrupt file never resets to default");
        fprintf(result, "PASS: fixed roles/defaults, login/logout events, password changes, persistence, localization, audit, migration, fail-closed validation.\n");
        fclose(result);
        return 0;
    }catch(Exception &e){
        fprintf(result, "FAIL: %s\n", AnsiString(e.Message).c_str());
        fclose(result);
        return 1;
    }catch(...){
        fprintf(result, "FAIL: Unknown exception\n");
        fclose(result);
        return 1;
    }
}
