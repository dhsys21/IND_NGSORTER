#ifndef RecoveryPreviewH
#define RecoveryPreviewH

#include <vcl.h>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.StdCtrls.hpp>

// Recovery UI delegates to existing machine handlers; it never changes sequence data.
class TRecoveryPreviewForm : public TForm
{
private:
    bool Korean;
    TLabel *Selection;
    TLabel *WorkState, *Detail, *TrayInfo, *CellInfo, *Sensors, *Records;
    bool Dispatching;
    TTimer *RefreshTimer;
    UnicodeString Text(const wchar_t *ko, const wchar_t *en)
    {
        return Korean ? UnicodeString(ko) : UnicodeString(en);
    }
    TLabel *Label(TWinControl *parent, const UnicodeString &caption,
        int x, int y, int w, int h, int size, TColor color, bool bold = false)
    {
        TLabel *label = new TLabel(this);
        label->Parent = parent;
        label->AutoSize = false;
        label->SetBounds(x, y, w, h);
        label->WordWrap = true;
        label->Caption = caption;
        label->Font->Size = size;
        label->Font->Color = color;
        if(bold) label->Font->Style = TFontStyles() << fsBold;
        return label;
    }
    void Choice(int tag, int top, const UnicodeString &caption,
        const UnicodeString &description)
    {
        TButton *button = new TButton(this);
        button->Parent = this;
        button->SetBounds(24, top, 220, 50);
        button->Caption = caption;
        button->Tag = tag;
        button->OnClick = SelectClick;
        Label(this, description, 262, top + 5, 530, 48, 10, clWindowText);
    }
    void __fastcall SelectClick(TObject *Sender)
    {
        TButton *button = dynamic_cast<TButton*>(Sender);
        if(button == NULL) return;
        if(Dispatching) return;
        TNotifyEvent action = NULL;
        if(button->Tag == 1) action = OnRestartAction;
        if(button->Tag == 2) action = OnManualAction;
        if(button->Tag == 3) action = OnCompleteAction;
        if(action == NULL) return;
        Selection->Caption = Text(L"\uc120\ud0dd: ", L"Selected: ") + button->Caption +
            Text(L"\r\n\uae30\uc874 \ucc98\ub9ac \uc808\ucc28\uc758 \uc870\uac74 \ud655\uc778 \ubc0f \uc548\ub0b4\ub97c \ub530\ub974\uc138\uc694.",
                 L"\r\nFollow the checks and messages from the existing operation.");
        Dispatching = true;
        RefreshTimer->Enabled = false;
        Hide();
        try {
            // Non-null Sender preserves the existing access-control checks.
            action(button);
        } __finally {
            Dispatching = false;
            Show();
            RefreshState();
            RefreshTimer->Enabled = true;
        }
    }
    void __fastcall RefreshTick(TObject *Sender) { if(Visible) RefreshState(); }
    void __fastcall CloseClick(TObject *Sender) { Close(); }
    void __fastcall ReleaseOnClose(TObject *Sender, TCloseAction &action)
    {
        action = Dispatching ? caNone : caFree;
    }
public:
    TNotifyEvent OnRestartAction, OnManualAction, OnCompleteAction, OnRefreshState;
    void RefreshState() { if(OnRefreshState != NULL) OnRefreshState(this); }
    void SetWorkState(const UnicodeString &state, const UnicodeString &detail,
        const UnicodeString &trays, const UnicodeString &cell,
        const UnicodeString &sensors, const UnicodeString &records)
    {
        WorkState->Caption = state;
        Detail->Caption = detail;
        TrayInfo->Caption = trays;
        CellInfo->Caption = cell;
        Sensors->Caption = sensors;
        Records->Caption = records;
    }
    __fastcall TRecoveryPreviewForm(TComponent *owner, bool korean)
        : TForm(owner, 0), Korean(korean), Dispatching(false)
    {
        Name = "RecoveryPreviewForm";
        OnRestartAction = NULL;
        OnManualAction = NULL;
        OnCompleteAction = NULL;
        OnRefreshState = NULL;
        Caption = Text(L"\uc791\uc5c5 \uc911\ub2e8 / \ubcf5\uad6c \uc548\ub0b4", L"Work interrupted / recovery");
        BorderStyle = bsSingle;
        BorderIcons = TBorderIcons() << biSystemMenu;
        Position = poScreenCenter;
        ClientWidth = 820;
        ClientHeight = 650;
        Color = (TColor)0x00F5F5F5;
        Font->Name = "Malgun Gothic";
        Font->Size = 10;
        OnClose = ReleaseOnClose;

        TPanel *heading = new TPanel(this);
        heading->Parent = this;
        heading->SetBounds(0, 0, 820, 72);
        heading->BevelOuter = bvNone;
        heading->ParentBackground = false;
        heading->Color = (TColor)0x00444444;
        Label(heading, Text(L"\uc791\uc5c5 \uc911\ub2e8 / \ubcf5\uad6c \uc548\ub0b4", L"WORK INTERRUPTED / RECOVERY"),
            24, 10, 620, 30, 17, clWhite, true);
        Label(heading, Text(L"\uc2e4\uc81c \uc870\uc791 \ubc84\ud2bc\uc785\ub2c8\ub2e4. \uae30\uc874 \uad8c\ud55c \ubc0f \uc548\uc804 \uc778\ud130\ub77d\uc774 \uc801\uc6a9\ub429\ub2c8\ub2e4.",
            L"LIVE CONTROLS - Existing permissions and safety interlocks apply."),
            24, 44, 770, 22, 10, clWhite);
        WorkState = Label(this, L"--",
            24, 88, 770, 28, 13, (TColor)0x002A6080, true);
        Detail = Label(this, L"--",
            24, 123, 770, 24, 11, clWindowText);
        TrayInfo = Label(this, L"--", 24, 156, 775, 40, 10, clWindowText, true);
        CellInfo = Label(this, L"--", 24, 200, 770, 24, 10, clWindowText);
        Sensors = Label(this, L"--", 24, 229, 770, 24, 10, clWindowText);
        Records = Label(this, L"--", 24, 256, 770, 26, 10, clWindowText);
        Label(this, Text(L"\uc2e4\uc81c \uc140 \uc704\uce58\uc640 \uac78\ub9bc \uc5ec\ubd80\ub97c \ud655\uc778\ud55c \ud6c4 \uc870\uce58\ud558\uc138\uc694.",
            L"Confirm the actual cell location and check for a jam before recovery."),
            24, 289, 770, 28, 11, clWindowText, true);

        Choice(1, 330, Text(L"1. \uc7ac\uc2dc\uc791", L"1. Restart"),
            Text(L"\uc218\ub3d9 \uc774\ub3d9\uc774\ub098 \uc140 \uc774\ub3d9 \uc5c6\uc774 \uc6d0\uc778\uc744 \ud574\uacb0\ud55c \uacbd\uc6b0\r\n\uc7ac\uac1c \uc870\uac74 \ud655\uc778 \ud6c4 \uc911\ub2e8\ub41c \uc791\uc5c5\uc744 \uacc4\uc18d\ud569\ub2c8\ub2e4.",
                 L"Problem resolved without moving the cell or axes manually.\r\nContinue only after restart conditions are verified."));
        Choice(2, 396, Text(L"2. \uc218\ub3d9 \uc870\uce58", L"2. Manual correction"),
            Text(L"\uc218\ub3d9\ubaa8\ub4dc\uc5d0\uc11c \ubb38\uc81c\ub97c \ud574\uacb0\ud569\ub2c8\ub2e4.\r\n\uc870\uce58 \ud6c4 AUTO / Restart \ub610\ub294 \uc218\ub3d9 \uc0bd\uc785 \uc644\ub8cc\ub97c \uc120\ud0dd\ud569\ub2c8\ub2e4.",
                 L"Resolve the problem in MANUAL. Then select\r\nAUTO / Restart, or confirm manual insertion completion."));
        Choice(3, 462, Text(L"3. \uc218\ub3d9 \uc0bd\uc785 \uc644\ub8cc", L"3. Manual insert complete"),
            Text(L"\uc2e4\uc81c\ub85c Target\uc5d0 \uc0bd\uc785\uae4c\uc9c0 \uc644\ub8cc\ud55c \uacbd\uc6b0\uc5d0\ub9cc \uc0ac\uc6a9\ud569\ub2c8\ub2e4.\r\n\uc0bd\uc785 \ucc44\ub110 \ud655\uc778 \ud6c4 \uae30\ub85d \ubc18\uc601 \ubc0f FMS \ubcf4\uace0\ub97c \uc9c4\ud589\ud569\ub2c8\ub2e4.",
                 L"Only after the cell has actually been inserted into Target.\r\nConfirm the slot before updating records and reporting to FMS."));
        Selection = Label(this, Text(L"\uc7ac\uc2dc\uc791\uc740 \uae30\uc874 Restart\uc640 \uac19\uc2b5\ub2c8\ub2e4. AUTO \uc804\ud658\uc740 \uba54\uc778\ud654\uba74\uc5d0\uc11c \uc9c4\ud589\ud558\uc138\uc694.",
            L"Restart uses the existing handler. Select AUTO on the main screen first."),
            24, 540, 770, 51, 10, (TColor)0x00555555);
        TButton *close = new TButton(this);
        close->Parent = this;
        close->SetBounds(654, 601, 142, 34);
        close->Caption = Text(L"\ub2eb\uae30", L"Close");
        close->Cancel = true;
        close->OnClick = CloseClick;
        RefreshTimer = new TTimer(this);
        RefreshTimer->Interval = 500;
        RefreshTimer->OnTimer = RefreshTick;
    }
};

#endif
