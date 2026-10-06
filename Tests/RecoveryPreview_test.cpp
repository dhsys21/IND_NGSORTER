#include <vcl.h>
#include <Vcl.Imaging.pngimage.hpp>
#include "RecoveryPreview.h"
#pragma hdrstop
#pragma argsused

static void Check(bool ok, const wchar_t *message)
{
    if(!ok) throw Exception(message);
}

class TActionProbe : public TObject
{
public:
    int restarts, manuals, completions;
    TActionProbe() : restarts(0), manuals(0), completions(0) {}
    void Verify(TObject *sender, int tag)
    {
        TButton *button = dynamic_cast<TButton*>(sender);
        Check(button != NULL && button->Tag == tag, L"Real non-null Sender required");
    }
    void __fastcall Restart(TObject *sender) { Verify(sender, 1); ++restarts; }
    void __fastcall Manual(TObject *sender) { Verify(sender, 2); ++manuals; }
    void __fastcall Complete(TObject *sender) { Verify(sender, 3); ++completions; }
};

static void Capture(TForm *form, const UnicodeString &file)
{
    Graphics::TBitmap *bitmap = new Graphics::TBitmap();
    TPngImage *png = new TPngImage();
    try {
        bitmap->SetSize(form->ClientWidth, form->ClientHeight);
        form->Update();
        HDC screen = GetDC(form->Handle);
        BitBlt(bitmap->Canvas->Handle, 0, 0, form->ClientWidth, form->ClientHeight,
            screen, 0, 0, SRCCOPY);
        ReleaseDC(form->Handle, screen);
        png->Assign(bitmap);
        png->SaveToFile(file);
    } __finally {
        delete png;
        delete bitmap;
    }
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
    TStringList *result = new TStringList();
    int code = 0;
    try {
        Application->Initialize();
        for(int language = 0; language < 2; ++language){
            TRecoveryPreviewForm *form = new TRecoveryPreviewForm(NULL, language == 0);
            TActionProbe *probe = new TActionProbe();
            form->OnRestartAction = probe->Restart;
            form->OnManualAction = probe->Manual;
            form->OnCompleteAction = probe->Complete;
            form->SetWorkState(L"MANUAL | Robot Pause=0 / Gripper Pause=0",
                L"Robot Seq=0 Step=0 / Gripper Seq=0 Step=0",
                L"SOURCE  - / CH0\r\nTARGET  - / CH0", L"No current work channel.",
                L"Sensors: communication unavailable", L"Work record: picked=- / inserted=-");
            try {
                form->Show();
                Application->ProcessMessages();
                Capture(form, language == 0 ? L"tmp/recovery-preview/ko.png" : L"tmp/recovery-preview/en.png");
                int choices = 0;
                for(int i = 0; i < form->ComponentCount; ++i){
                    TButton *button = dynamic_cast<TButton*>(form->Components[i]);
                    if(button == NULL || button->Tag == 0) continue;
                    button->Click();
                    ++choices;
                    bool shown = false;
                    for(int j = 0; j < form->ComponentCount; ++j){
                        TLabel *label = dynamic_cast<TLabel*>(form->Components[j]);
                        if(label != NULL && label->Caption.Pos(button->Caption) > 0) shown = true;
                    }
                    Check(shown, L"Selected action feedback missing");
                    Check(form->Visible, L"Preview action must not close the form");
                }
                Check(choices == 3, L"Expected three recovery choices");
                Check(probe->restarts == 1 && probe->manuals == 1 && probe->completions == 1,
                    L"Each button must invoke exactly its assigned existing handler");
                Capture(form, language == 0 ? L"tmp/recovery-preview/ko-selected.png" : L"tmp/recovery-preview/en-selected.png");
                result->Add(language == 0 ? L"PASS Korean / three delegated actions" : L"PASS English / three delegated actions");
            } __finally { delete form; delete probe; }
        }
    } catch(Exception &e) { result->Add(e.Message); code = 1; }
    result->SaveToFile(L"tmp/recovery-preview/result.txt");
    delete result;
    return code;
}
