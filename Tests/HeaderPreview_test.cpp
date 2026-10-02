#include <vcl.h>
#pragma hdrstop
#include <Vcl.Imaging.jpeg.hpp>
#include <Vcl.Imaging.pngimage.hpp>
#include "AdvSmoothButton.hpp"
#include "AdvSmoothPanel.hpp"
#include <memory>
#include <stdio.h>
int WINAPI wWinMain(HINSTANCE,HINSTANCE,LPWSTR,int) {
    FILE *log=fopen("tmp/Access20261001/header-result.txt","w");
    try {
        Application->Initialize();
        RegisterClass(__classid(TAdvSmoothButton));
        RegisterClass(__classid(TAdvSmoothPanel));
        RegisterClass(__classid(TImage));
        RegisterClass(__classid(TLabel));
        RegisterClass(__classid(TPanel));
        RegisterClass(__classid(TButton));
        RegisterClass(__classid(TRadioButton));
        std::auto_ptr<TForm> form(new TForm(NULL,0));
        std::auto_ptr<TMemoryStream> text(new TMemoryStream());
        std::auto_ptr<TMemoryStream> binary(new TMemoryStream());
        text->LoadFromFile("tmp/Access20261001/header.dfm");
        ObjectTextToBinary(text.get(),binary.get());
        binary->Position=0;
        binary->ReadComponent(form.get());
        const char *buttons[]={"btnProduction","btnUser","Button1","AdvSmoothButton4"};
        for(int i=0;i<4;++i){
            TControl *button=dynamic_cast<TControl*>(form->FindComponent(buttons[i]));
            if(!button || button->Width!=90 || button->Left!=1266+94*i)
                throw Exception("Header button width/order/gap mismatch");
        }
        TControl *status=dynamic_cast<TControl*>(form->FindComponent("pbcr2"));
        TControl *user=dynamic_cast<TControl*>(form->FindComponent("lblAccessUser"));
        if(!status || form->ClientWidth-status->Left-status->Width!=36 ||
            !user || user->Left+user->Width>1044)
            throw Exception("Header status/user placement mismatch");
        const char *safety[]={"btnBypassOn","btnSafetyReset","btnKeyUnLock","btnKeyLock"};
        for(int i=0;i<4;++i){
            TControl *button=dynamic_cast<TControl*>(form->FindComponent(safety[i]));
            if(!button || button->Left!=1044+111*(i%2) || button->Width!=107)
                throw Exception("Safety/keylock group placement mismatch");
        }
        form->Show();
        form->Update();
        form->ClientWidth=2000;
        if(form->ClientWidth-status->Left-status->Width!=36)
            throw Exception("Connection indicators lost right anchor");
        form->ClientWidth=1920;
        std::auto_ptr<Graphics::TBitmap> bitmap(form->GetFormImage());
        std::auto_ptr<TPngImage> png(new TPngImage());
        png->Assign(bitmap.get());
        png->SaveToFile("tmp/Access20261001/header.png");
        fprintf(log,"PASS: isolated header DFM loaded and rendered without machine modules.\n");
        fclose(log);return 0;
    }catch(Exception &e){fprintf(log,"FAIL: %s\n",AnsiString(e.Message).c_str());fclose(log);return 1;}
}
