#include <vcl.h>
#pragma hdrstop
#include "AccessControl.h"
#include <System.JSON.hpp>
#include <System.SysUtils.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.StdCtrls.hpp>
#include <windows.h>
#include <memory>
#pragma package(smart_init)

static TStrings *AccessLanguage = NULL;
void SetAccessLanguage(TStrings *Strings) { AccessLanguage = Strings; }
UnicodeString AccessText(const UnicodeString &Key, const UnicodeString &Fallback)
{
    if(AccessLanguage != NULL){
        int index = AccessLanguage->IndexOfName(Key);
        if(index >= 0 && !AccessLanguage->ValueFromIndex[index].IsEmpty())
            return AccessLanguage->ValueFromIndex[index];
    }
    return Fallback;
}

// Constructed at module initialization; access is confined to the VCL UI thread.
static TAccessControl AccessInstance;
TAccessControl &AccessControl() { return AccessInstance; }

TAccessControl::TAccessControl() : FLevel(alGuest), FLoginAt(0.0),
    FInitialized(false), FAuditWarning(false), OnSessionChanged(NULL) {}

UnicodeString TAccessControl::LevelName(TAccessLevel Level)
{
    switch(Level){
        case alOperator: return "Operator";
        case alEngineer: return "Engineer";
        case alAdmin: return "Admin";
        default: return "Guest";
    }
}

void TAccessControl::ValidateUser(const UnicodeString &Id, const UnicodeString &Password, TAccessLevel Level)
{
    if(Id.IsEmpty() || Id.Length() > 32 || Id != Id.Trim())
        throw Exception("User ID must contain 1-32 characters.");
    for(int i = 1; i <= Id.Length(); ++i){
        wchar_t c = Id[i];
        if(!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
             (c >= '0' && c <= '9') || c == '_' || c == '-' || c == '.'))
            throw Exception("User ID: use letters, numbers, dot, underscore or hyphen.");
    }
    if(Password.IsEmpty() || Password.Length() > 128 || Password.Pos("\r") || Password.Pos("\n"))
        throw Exception("Password must contain 1-128 characters without a line break.");
    if(Level < alOperator || Level > alAdmin || Id != LevelName(Level).LowerCase())
        throw Exception("Only operator, engineer and admin accounts are supported.");
}

void TAccessControl::Initialize(const UnicodeString &FileName, const UnicodeString &LogFolder)
{
    FFileName = FileName;
    FLogFolder = IncludeTrailingPathDelimiter(LogFolder);
    FUsers.clear();
    FUserId = "";
    FLevel = alGuest;
    FLoginAt = 0.0;
    FLoadError = "";
    FInitialized = true;
    try{
        // Fixed site accounts: defaults are used only for a new/missing role, never on login failure.
        std::vector<TAccessUser> fixedUsers;
        for(int i = alOperator; i <= alAdmin; ++i){
            TAccessUser user;
            user.Level = static_cast<TAccessLevel>(i);
            user.Id = LevelName(user.Level).LowerCase();
            user.Password = user.Id; // Site-requested initial password, changeable in USER.
            fixedUsers.push_back(user);
        }
        if(!FileExists(FileName)){
            SaveUsers(fixedUsers);
            Audit("ACCOUNTS_INITIALIZED", "Fixed Operator / Engineer / Admin accounts");
            return;
        }
        std::auto_ptr<TStringList> text(new TStringList());
        text->LoadFromFile(FileName, TEncoding::UTF8);
        std::auto_ptr<TJSONValue> value(TJSONObject::ParseJSONValue(text->Text));
        TJSONObject *root = dynamic_cast<TJSONObject*>(value.get());
        TJSONArray *users = root == NULL ? NULL : dynamic_cast<TJSONArray*>(root->GetValue("users"));
        if(users == NULL || users->Count == 0) throw Exception("Invalid account file.");
        TJSONNumber *schema = dynamic_cast<TJSONNumber*>(root->GetValue("schema"));
        bool fixedFormat = schema != NULL && schema->Value() == "2";
        if(schema != NULL && !fixedFormat) throw Exception("Unsupported account file version.");
        bool found[3] = {false, false, false};
        for(int i = 0; i < users->Count; ++i){
            TJSONObject *item = dynamic_cast<TJSONObject*>(users->Items[i]);
            if(item == NULL) throw Exception("Invalid account entry.");
            TJSONString *id = dynamic_cast<TJSONString*>(item->GetValue("id"));
            TJSONString *password = dynamic_cast<TJSONString*>(item->GetValue("password"));
            TJSONNumber *level = dynamic_cast<TJSONNumber*>(item->GetValue("level"));
            if(id == NULL || password == NULL || level == NULL) throw Exception("Incomplete account entry.");
            int number = StrToIntDef(level->Value(), -1);
            TAccessUser user;
            if(number < alOperator || number > alAdmin) throw Exception("Invalid access level.");
            user.Level = static_cast<TAccessLevel>(number);
            user.Id = LevelName(user.Level).LowerCase();
            user.Password = password->Value();
            ValidateUser(user.Id, user.Password, user.Level);
            if(found[number - 1]) throw Exception("Multiple legacy accounts share a level. Select the password to retain before migration.");
            if(fixedFormat && id->Value() != user.Id) throw Exception("Invalid fixed account ID.");
            found[number - 1] = true;
            fixedUsers[number - 1] = user;
        }
        if(!found[2]) throw Exception("No Admin account is defined.");
        if(fixedFormat){
            if(users->Count != 3 || !found[0] || !found[1]) throw Exception("All three fixed accounts are required.");
            FUsers = fixedUsers;
        }else{
            // Preserve unique legacy passwords by level; keep the original file before conversion.
            UnicodeString backup = FileName + ".legacy.bak";
            if(!FileExists(backup) && !CopyFileW(FileName.c_str(), backup.c_str(), TRUE))
                throw Exception("Cannot back up the legacy account file.");
            SaveUsers(fixedUsers);
            Audit("ACCOUNTS_MIGRATED", "Fixed IDs; existing level passwords preserved");
        }
    }catch(const Exception &e){
        FUsers.clear();
        FLoadError = "Unable to load accounts: " + e.Message;
        // A damaged file is NOT treated as first-use enrollment.
        Audit("ACCOUNT_LOAD_FAILED", FLoadError);
    }
}

bool TAccessControl::Can(TAccessLevel Required) const
{
    return FInitialized && FLoadError.IsEmpty() && FLevel >= Required;
}

bool TAccessControl::Require(TAccessLevel Required, const UnicodeString &Action)
{
    if(Can(Required)){
        Audit("ACTION_REQUEST", Action);
        return true;
    }
    Audit("ACCESS_DENIED", Action + " Required=" + LevelName(Required));
    ShowMessage(AccessText("ACCESS_DENIED", "Access denied. Required level:") + " " +
        LevelName(Required) + "\r\n" + Action);
    return false;
}

bool TAccessControl::Login(TAccessLevel Level, const UnicodeString &Password)
{
    int index = static_cast<int>(Level) - 1;
    if(!FInitialized || !FLoadError.IsEmpty() || index < 0 || index >= static_cast<int>(FUsers.size()) ||
       FUsers[index].Level != Level || FUsers[index].Password != Password){
        Audit("LOGIN_FAILED", "Invalid level or password", LevelName(Level).LowerCase());
        return false;
    }
    if(FLevel != alGuest) Logout();
    FUserId = FUsers[index].Id;
    FLevel = FUsers[index].Level;
    FLoginAt = Now();
    Audit("LOGIN_SUCCESS", "Level=" + LevelName(FLevel));
    if(OnSessionChanged != NULL) OnSessionChanged(FUserId, FLevel, true);
    return true;
}

void TAccessControl::Logout()
{
    if(FLevel != alGuest){
        Audit("LOGOUT", "Level=" + LevelName(FLevel));
        if(OnSessionChanged != NULL) OnSessionChanged(FUserId, FLevel, false);
    }
    FUserId = "";
    FLevel = alGuest;
    FLoginAt = 0.0;
    // No machine commands, mode reset, timers, expiry, or inactivity timeout here.
}

void TAccessControl::SaveUsers(const std::vector<TAccessUser> &Users)
{
    std::auto_ptr<TJSONObject> root(new TJSONObject());
    root->AddPair("schema", new TJSONNumber(2));
    TJSONArray *array = new TJSONArray();
    root->AddPair("users", array);
    for(unsigned int i = 0; i < Users.size(); ++i){
        TJSONObject *user = new TJSONObject();
        user->AddPair("id", Users[i].Id);
        user->AddPair("password", Users[i].Password);
        user->AddPair("level", new TJSONNumber(static_cast<int>(Users[i].Level)));
        array->AddElement(user);
    }
    UnicodeString temp = FFileName + ".tmp";
    if(!ForceDirectories(ExtractFilePath(FFileName))) throw Exception("Cannot create account folder.");
    std::auto_ptr<TStringList> text(new TStringList());
    text->Text = root->ToJSON();
    text->SaveToFile(temp, TEncoding::UTF8);
    // Same-volume atomic replacement; failed writes keep the existing account file intact.
    if(!MoveFileExW(temp.c_str(), FFileName.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
        throw Exception("Cannot save account file. " + SysErrorMessage(GetLastError()));
    FUsers = Users;
}

void TAccessControl::ChangePassword(TAccessLevel Level, const UnicodeString &OldPassword, const UnicodeString &NewPassword)
{
    int index = static_cast<int>(Level) - 1;
    if(!FInitialized || !FLoadError.IsEmpty() || index < 0 || index >= static_cast<int>(FUsers.size()) ||
       FUsers[index].Password != OldPassword){
        Audit("PASSWORD_CHANGE_FAILED", "Current password mismatch", LevelName(Level).LowerCase());
        throw Exception("Current password is incorrect.");
    }
    // Password change authenticates the selected fixed account, without changing the active role.
    ValidateUser(FUsers[index].Id, NewPassword, Level);
    std::vector<TAccessUser> users = FUsers;
    users[index].Password = NewPassword;
    SaveUsers(users);
    Audit("PASSWORD_CHANGED", "Selected fixed account", LevelName(Level).LowerCase());
}

static UnicodeString SingleLine(UnicodeString Text)
{
    Text = StringReplace(Text, "\r", " ", TReplaceFlags() << rfReplaceAll);
    Text = StringReplace(Text, "\n", " ", TReplaceFlags() << rfReplaceAll);
    return StringReplace(Text, "\t", " ", TReplaceFlags() << rfReplaceAll);
}

void TAccessControl::Audit(const UnicodeString &Event, const UnicodeString &Detail, const UnicodeString &Actor)
{
    if(FLogFolder.IsEmpty()) return;
    try{
        if(!ForceDirectories(FLogFolder)) throw Exception("Cannot create audit folder.");
        UnicodeString user = Actor.IsEmpty() ? (FUserId.IsEmpty() ? UnicodeString("Guest") : FUserId) : Actor;
        UnicodeString path = FLogFolder + "access_" + Now().FormatString("yyyymmdd") + ".log";
        UTF8String line(Now().FormatString("yyyy-mm-dd hh:nn:ss.zzz") + "\t[ACCESS]\t" +
            SingleLine(Event) + "\tUser=" + SingleLine(user) + "\t" + SingleLine(Detail) + "\r\n");
        HANDLE file = CreateFileW(path.c_str(), FILE_APPEND_DATA, FILE_SHARE_READ,
            NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if(file == INVALID_HANDLE_VALUE) throw Exception("Cannot open audit log.");
        DWORD written = 0;
        bool ok = WriteFile(file, line.c_str(), line.Length(), &written, NULL) != 0;
        CloseHandle(file);
        if(!ok || written != static_cast<DWORD>(line.Length())) throw Exception("Cannot write audit log.");
        FAuditWarning = false;
    }catch(const Exception &e){
        if(!FAuditWarning){
            FAuditWarning = true;
            ShowMessage("Access audit log error: " + e.Message);
        }
    }
}

void AuditControlValues(TComponent *Owner, const UnicodeString &Action)
{
    // An applied-settings snapshot. Password/account controls are deliberately excluded.
    for(int i = 0; i < Owner->ComponentCount; ++i){
        TComponent *c = Owner->Components[i];
        UnicodeString name = c->Name.LowerCase();
        if(name.Pos("password") || name.Pos("passedit") || name.Pos("secret")) continue;
        UnicodeString value;
        if(TCustomEdit *edit = dynamic_cast<TCustomEdit*>(c)){
            if(dynamic_cast<TMemo*>(c)) continue;
            value = edit->Text;
        }else if(TComboBox *combo = dynamic_cast<TComboBox*>(c)) value = combo->Text;
        else if(TCheckBox *check = dynamic_cast<TCheckBox*>(c)) value = check->Checked ? "true" : "false";
        else continue;
        AccessControl().Audit("SETTING_CHANGE", Action + " " + Owner->Name + "." + c->Name + "=" + value);
    }
}
