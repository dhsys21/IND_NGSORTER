#ifndef AccessControlH
#define AccessControlH

#include <System.hpp>
#include <System.Classes.hpp>
#include <vector>

enum TAccessLevel { alGuest = 0, alOperator = 1, alEngineer = 2, alAdmin = 3 };

struct TAccessUser {
    UnicodeString Id;
    UnicodeString Password; // Plain text, explicitly requested for this local installation.
    TAccessLevel Level;
};

// PC-only authorization. Never used to gate firmware, PLC, or safety callbacks.
class TAccessControl {
private:
    std::vector<TAccessUser> FUsers;
    UnicodeString FFileName, FLogFolder, FUserId, FLoadError;
    TAccessLevel FLevel;
    TDateTime FLoginAt;
    bool FInitialized, FAuditWarning;
    void SaveUsers(const std::vector<TAccessUser> &Users);
    void ValidateUser(const UnicodeString &Id, const UnicodeString &Password, TAccessLevel Level);
public:
    TAccessControl();
    void Initialize(const UnicodeString &FileName, const UnicodeString &LogFolder);
    bool Initialized() const { return FInitialized; }
    UnicodeString LoadError() const { return FLoadError; }
    UnicodeString UserId() const { return FUserId; }
    TAccessLevel Level() const { return FLevel; }
    TDateTime LoginAt() const { return FLoginAt; }
    bool Can(TAccessLevel Required) const;
    bool Require(TAccessLevel Required, const UnicodeString &Action);
    bool Login(TAccessLevel Level, const UnicodeString &Password);
    void Logout();
    void ChangePassword(TAccessLevel Level, const UnicodeString &OldPassword, const UnicodeString &NewPassword);
    // FMS login tag is not defined yet. The callback contains no password.
    void (*OnSessionChanged)(const UnicodeString &Account, TAccessLevel Level, bool LoggedIn);
    void Audit(const UnicodeString &Event, const UnicodeString &Detail, const UnicodeString &Actor = "");
    static UnicodeString LevelName(TAccessLevel Level);
};

TAccessControl &AccessControl();
UnicodeString AccessText(const UnicodeString &Key, const UnicodeString &Fallback);
void SetAccessLanguage(TStrings *Strings);
void AuditControlValues(TComponent *Owner, const UnicodeString &Action);
#endif
