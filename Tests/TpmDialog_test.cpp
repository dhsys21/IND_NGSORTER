#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include <Vcl.Imaging.pngimage.hpp>
#include <fstream>
#include "../FormTpmLoss.h"
USEFORM("..\\FormTpmLoss.cpp", TpmLossForm);

class DialogDriver : public TObject {
public:
    TTimer *timer;
    int wanted;
    UnicodeString capture;
    bool clicked;
    DialogDriver() : wanted(-1), clicked(false) {
        timer = new TTimer(NULL);
        timer->Enabled = false;
        timer->Interval = 100;
        timer->OnTimer = Tick;
    }
    __fastcall ~DialogDriver() { delete timer; }
    void __fastcall Tick(TObject *) {
        timer->Enabled = false;
        if(!capture.IsEmpty()){
            Graphics::TBitmap *bitmap = new Graphics::TBitmap();
            TPngImage *png = new TPngImage();
            bitmap->SetSize(TpmLossForm->ClientWidth, TpmLossForm->ClientHeight);
            TpmLossForm->Repaint();
            HDC dc = GetDC(TpmLossForm->Handle);
            BitBlt(bitmap->Canvas->Handle, 0, 0, bitmap->Width, bitmap->Height, dc, 0, 0, SRCCOPY);
            ReleaseDC(TpmLossForm->Handle, dc);
            png->Assign(bitmap);
            png->SaveToFile(capture);
            delete png;
            delete bitmap;
        }
        if(wanted < 0){ clicked = true; TpmLossForm->btnCancel->Click(); return; }
        for(int i = 0; i < TpmLossForm->ComponentCount; ++i){
            TButton *b = dynamic_cast<TButton*>(TpmLossForm->Components[i]);
            if(b != NULL && b != TpmLossForm->btnCancel && b->Tag == wanted){
                if(!b->Showing || b->Left < 0 || b->Top < 0 ||
                    b->Left + b->Width > TpmLossForm->ClientWidth ||
                    b->Top + b->Height > TpmLossForm->ClientHeight){
                    TpmLossForm->ModalResult = mrCancel;
                    return;
                }
                clicked = true;
                b->Click();
                return;
            }
        }
        TpmLossForm->ModalResult = mrCancel;
    }
};

int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
    std::ofstream log("tmp/TPM20261001/dialog-test.log");
    try {
        Application->Initialize();
        Application->CreateForm(__classid(TTpmLossForm), &TpmLossForm);
        TStringList *labels = new TStringList();
        DialogDriver *driver = new DialogDriver();
        const char *languages[2] = {"Lang_En.ini", "Lang_Ko.ini"};
        for(int lang = 0; lang < 2; ++lang){
            labels->LoadFromFile(languages[lang], TEncoding::UTF8);
            for(int i = 0; i < TpmLossReasonCount; ++i){
                driver->wanted = i;
                driver->clicked = false;
                driver->capture = i == 0 ? UnicodeString(lang == 0 ?
                    "tmp/TPM20261001/tpm-en.png" : "tmp/TPM20261001/tpm-ko.png") : UnicodeString("");
                driver->timer->Enabled = true;
                if(TpmLossForm->SelectReason(labels) != i || !driver->clicked)
                    throw Exception("Reason button mapping failed");
                TPanel *code = dynamic_cast<TPanel*>(TpmLossForm->FindComponent(
                    "code" + UnicodeString(TpmLossReasons[i].code)));
                if(code == NULL || code->Caption != TpmLossReasons[i].code)
                    throw Exception("Code display mismatch");
            }
            driver->wanted = -1;
            driver->capture = "";
            driver->timer->Enabled = true;
            if(TpmLossForm->SelectReason(labels) != -1) throw Exception("Cancel reused old selection");
        }
        if(UnicodeString(TpmLossReasons[0].code) != "0300") throw Exception("Leading zero lost");
        log << "PASS: 20 selections (EN/KO), cancel/reopen, all code mappings, leading zero, native DFM render.\n";
        delete driver;
        delete labels;
        return 0;
    } catch(Exception &e) {
        log << "FAIL: " << AnsiString(e.Message).c_str() << "\n";
        return 1;
    }
}
