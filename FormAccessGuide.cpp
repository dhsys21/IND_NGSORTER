#include <vcl.h>
#pragma hdrstop
#include "FormAccessGuide.h"
#include "AccessControl.h"
#pragma package(smart_init)
#pragma resource "*.dfm"
TAccessGuideForm *AccessGuideForm;

struct TAccessGuideRow {
    const char *Key;
    const char *Description;
    TAccessLevel Required;
};

// Read-only documentation of the existing guards. This table grants no permissions.
// Keep these rows aligned with the source handlers listed beside each entry.
static const TAccessGuideRow AccessGuideRows[] = {
    {"ACCESS_GUIDE_VIEW", "Status / I-O / PLC-FMS / CONFIG / production viewing", alGuest},
    {"ACCESS_GUIDE_STOP", "Pause / stop moving / servo OFF / buzzer off", alGuest},
    {"ACCESS_GUIDE_BARCODE", "Barcode scan / Tray ID / retry", alOperator},
    {"ACCESS_GUIDE_TRAYOUT", "Normal tray out / target tray exchange", alOperator},
    {"NGS_ACCESS_START", "Start / restart automatic work", alOperator},
    {"ACCESS_GUIDE_MODE", "AUTO / MANUAL mode selection", alEngineer},
    {"NGS_ACCESS_MOTION", "Servo OPEN / ON / HOME / jog / channel moves", alEngineer},
    {"ACCESS_GUIDE_LOCK", "Key lock / bypass solenoid / safety reset", alEngineer},
    {"ACCESS_GUIDE_RESET", "Servo reset / smoke alarm clear", alEngineer},
    {"NGS_ACCESS_RECOVERY", "Manual completion / gripper / error recovery", alEngineer},
    {"NGS_ACCESS_WORK", "Work reset / NG limit / dry run / speeds", alEngineer},
    {"ACCESS_GUIDE_CONFIG", "System configuration save / apply", alAdmin},
    {"ACCESS_GUIDE_CONNECT", "Device connect / disconnect in CONFIG", alAdmin},
    {"ACCESS_GUIDE_WRITE", "PLC / FMS writes and test commands", alAdmin},
    {"NGS_ACCESS_TEACHING", "Teaching save / auto calculation / load-factor limit", alAdmin},
    {"ACCESS_GUIDE_SMOKETEMP", "Smoke detector temperature setting", alAdmin},
    {"ACCESS_GUIDE_PASSWORD", "Selected account password change *", alGuest}
};

__fastcall TAccessGuideForm::TAccessGuideForm(TComponent *Owner) : TForm(Owner) {}

void TAccessGuideForm::Prepare()
{
    Caption = AccessText("ACCESS_GUIDE_TITLE", "Access permissions");
    lblTitle->Caption = Caption;
    btnClose->Caption = AccessText("ACCESS_GUIDE_CLOSE", "Close");
    lblNotes->Caption = AccessText("ACCESS_GUIDE_NOTES",
        "Allowed = level permission only; machine safety/interlocks still apply.\r\n"
        "Viewing and stopping are also available without login.\r\n"
        "* Password changes require the selected account's current password. DOOR/CYCLE service access keeps its separate password.");
    const int rows = sizeof(AccessGuideRows) / sizeof(AccessGuideRows[0]);
    gridPermissions->RowCount = rows + 1;
    gridPermissions->Cells[0][0] = AccessText("ACCESS_GUIDE_FEATURE", "Function");
    for(int col = 1; col <= 3; ++col)
        gridPermissions->Cells[col][0] = TAccessControl::LevelName(static_cast<TAccessLevel>(col));
    for(int row = 0; row < rows; ++row){
        gridPermissions->Cells[0][row + 1] = AccessText(AccessGuideRows[row].Key, AccessGuideRows[row].Description);
        for(int col = 1; col <= 3; ++col){
            bool allowed = col >= static_cast<int>(AccessGuideRows[row].Required);
            gridPermissions->Cells[col][row + 1] = allowed ?
                AccessText("ACCESS_GUIDE_ALLOWED", "Allowed") : AccessText("ACCESS_GUIDE_BLOCKED", "Blocked");
        }
    }
}
