#include <vcl.h>
#pragma hdrstop
#include "FormBase.h"
#include "ProductionHistory.h"
#pragma package(smart_init)

void __fastcall TMainForm::BeginProductionSourceCycle()
{
    // Fresh SOURCE absence arms this in senTimerTimer; mode/FMS retry never does.
    if(!productionSourceArmed) return;
    productionSourceArmed = false;
    ProductionHistory().BeginCycle();
    productionSourceCaptured = false;
    productionTestCycle = false;
    productionSourceCells = productionSourceNg = 0;
    productionSourceTrayId = "";
}

void __fastcall TMainForm::CaptureProductionSource()
{
    // Freeze incoming quantities before pickup clears CELL_EXIST/PICK.
    // Manual viewing and re-reading a partly sorted tray must not change counts.
    if(productionSourceArmed || productionSourceCaptured) return;
    productionSourceTrayId = pTrayid_source->Caption.Trim();
    if(productionSourceTrayId.IsEmpty()) return;
    for(int i = 0; i < tray_source.SLOT_COUNT && i < 96; ++i){
        if(!tray_source.CELL_EXIST[i]) continue;
        ++productionSourceCells;
        if(SameText(tray_source.RANK[i], "NG") || tray_source.PICK[i] == "Y")
            ++productionSourceNg;
    }
    productionSourceCaptured = true;
    productionTestCycle = cbCycle->Checked || chkBypass->Checked || BaseForm->config.useFatTestBarcodes;
}

void __fastcall TMainForm::CompleteProductionSource()
{
    if(!productionSourceCaptured || productionSourceTrayId != pTrayid_source->Caption.Trim()){
        memoMainLineAdd("[PRODUCTION] Source snapshot missing/mismatched; no estimated counts recorded.");
        return;
    }
    bool test = productionTestCycle || cbCycle->Checked || chkBypass->Checked ||
        BaseForm->config.useFatTestBarcodes;
    if(!ProductionHistory().CompleteCycle(productionSourceTrayId, productionSourceCells,
        test, Now(), productionSourceCells - productionSourceNg, productionSourceNg))
        memoMainLineAdd("[PRODUCTION] Save pending retry: " + AnsiString(ProductionHistory().Error()));
}
