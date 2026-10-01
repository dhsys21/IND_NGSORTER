#ifndef FormTpmLossH
#define FormTpmLossH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "TpmLoss.h"

class TTpmLossForm : public TForm
{
__published:
    TPanel *pnlTitle;
    TLabel *lblStatus;
    TButton *btnCancel;
    TPanel *hdrState;
    TPanel *hdrIndex;
    TPanel *hdrDescription;
    TPanel *hdrCode;
    TPanel *hdrChoice;
    TPanel *pnlDown;
    TPanel *idx0300;
    TPanel *desc0300;
    TPanel *code0300;
    TButton *btnSelect0300;
    TPanel *idx1200;
    TPanel *desc1200;
    TPanel *code1200;
    TButton *btnSelect1200;
    TPanel *idx1510;
    TPanel *desc1510;
    TPanel *code1510;
    TButton *btnSelect1510;
    TPanel *idx1520;
    TPanel *desc1520;
    TPanel *code1520;
    TButton *btnSelect1520;
    TPanel *idx1720;
    TPanel *desc1720;
    TPanel *code1720;
    TButton *btnSelect1720;
    TPanel *idx1730;
    TPanel *desc1730;
    TPanel *code1730;
    TButton *btnSelect1730;
    TPanel *idx1400;
    TPanel *desc1400;
    TPanel *code1400;
    TButton *btnSelect1400;
    TPanel *idx4110;
    TPanel *desc4110;
    TPanel *code4110;
    TButton *btnSelect4110;
    TPanel *idx3500;
    TPanel *desc3500;
    TPanel *code3500;
    TButton *btnSelect3500;
    TPanel *idx5100;
    TPanel *desc5100;
    TPanel *code5100;
    TButton *btnSelect5100;
    void __fastcall btnSelect0300Click(TObject *Sender);
    void __fastcall btnSelect1200Click(TObject *Sender);
    void __fastcall btnSelect1510Click(TObject *Sender);
    void __fastcall btnSelect1520Click(TObject *Sender);
    void __fastcall btnSelect1720Click(TObject *Sender);
    void __fastcall btnSelect1730Click(TObject *Sender);
    void __fastcall btnSelect1400Click(TObject *Sender);
    void __fastcall btnSelect4110Click(TObject *Sender);
    void __fastcall btnSelect3500Click(TObject *Sender);
    void __fastcall btnSelect5100Click(TObject *Sender);
private:
    int FSelectedReason;
    void __fastcall SelectReason(int Index);
public:
    __fastcall TTpmLossForm(TComponent *Owner);
    void __fastcall ApplyLanguage(TStrings *Strings);
    int __fastcall SelectReason(TStrings *Strings);
};

extern PACKAGE TTpmLossForm *TpmLossForm;
#endif
