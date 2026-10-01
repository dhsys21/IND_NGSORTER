#include <vcl.h>
#pragma hdrstop
#include "FormTpmLoss.h"
#pragma package(smart_init)
#pragma resource "*.dfm"
TTpmLossForm *TpmLossForm;

static UnicodeString TpmText(TStrings *labels, const char *key,
    const UnicodeString &fallback)
{
    if(labels != NULL){
        int pos = labels->IndexOfName(key);
        if(pos >= 0 && !labels->ValueFromIndex[pos].IsEmpty())
            return labels->ValueFromIndex[pos];
    }
    return fallback;
}

__fastcall TTpmLossForm::TTpmLossForm(TComponent *Owner)
    : TForm(Owner), FSelectedReason(-1)
{
}

void __fastcall TTpmLossForm::ApplyLanguage(TStrings *labels)
{
    // TPM LOSS: real DFM controls remain editable in the form designer.
    hdrDescription->Caption = TpmText(labels, "TPM_DESCRIPTION", "Description");
    hdrCode->Caption = TpmText(labels, "TPM_CODE", "Code");
    hdrChoice->Caption = TpmText(labels, "TPM_CHOICE", "Choice");
    for(int i = 0; i < TpmLossReasonCount; ++i){
        UnicodeString code = TpmLossReasons[i].code;
        TPanel *index = dynamic_cast<TPanel*>(FindComponent("idx" + code));
        TPanel *description = dynamic_cast<TPanel*>(FindComponent("desc" + code));
        TButton *button = dynamic_cast<TButton*>(FindComponent("btnSelect" + code));
        if(index != NULL) index->Caption = TpmText(labels,
            TpmLossReasons[i].indexKey, index->Caption);
        if(description != NULL) description->Caption = TpmText(labels,
            TpmLossReasons[i].descriptionKey, description->Caption);
        if(button != NULL){
            button->Tag = i;
            button->Caption = TpmText(labels, "TPM_SELECT", "Select");
        }
    }
    btnCancel->Caption = TpmText(labels, "TPM_CANCEL", "Cancel");
    lblStatus->Caption = TpmText(labels, "TPM_PAUSED",
        "Paused. Select a reason to enter Manual mode.");
}

int __fastcall TTpmLossForm::SelectReason(TStrings *labels)
{
    // TPM LOSS: opening resets the selection; Cancel cannot reuse an old code.
    FSelectedReason = -1;
    ModalResult = mrNone;
    ApplyLanguage(labels);
    ActiveControl = btnSelect0300;
    return ShowModal() == mrOk ? FSelectedReason : -1;
}

void __fastcall TTpmLossForm::SelectReason(int index)
{
    if(index < 0 || index >= TpmLossReasonCount) return;
    FSelectedReason = index;
    ModalResult = mrOk;
}

// TPM LOSS: published handlers support designer double-click navigation.
void __fastcall TTpmLossForm::btnSelect0300Click(TObject *Sender) { SelectReason(0); }
void __fastcall TTpmLossForm::btnSelect1200Click(TObject *Sender) { SelectReason(1); }
void __fastcall TTpmLossForm::btnSelect1510Click(TObject *Sender) { SelectReason(2); }
void __fastcall TTpmLossForm::btnSelect1520Click(TObject *Sender) { SelectReason(3); }
void __fastcall TTpmLossForm::btnSelect1720Click(TObject *Sender) { SelectReason(4); }
void __fastcall TTpmLossForm::btnSelect1730Click(TObject *Sender) { SelectReason(5); }
void __fastcall TTpmLossForm::btnSelect1400Click(TObject *Sender) { SelectReason(6); }
void __fastcall TTpmLossForm::btnSelect4110Click(TObject *Sender) { SelectReason(7); }
void __fastcall TTpmLossForm::btnSelect3500Click(TObject *Sender) { SelectReason(8); }
void __fastcall TTpmLossForm::btnSelect5100Click(TObject *Sender) { SelectReason(9); }
