#ifndef TpmLossH
#define TpmLossH
#include <System.hpp>

// TPM LOSS: customer codes remain strings so the leading zero in 0300 is retained.
struct TTpmLossReason {
    const char *code;
    const char *indexKey;
    const char *descriptionKey;
};
static const int TpmLossReasonCount = 10;
static const TTpmLossReason TpmLossReasons[TpmLossReasonCount] = {
    {"0300", "TPM_BM", "TPM_DESC_0300"},
    {"1200", "TPM_PM", "TPM_DESC_1200"},
    {"1510", "TPM_PM", "TPM_DESC_1510"},
    {"1520", "TPM_PM", "TPM_DESC_1520"},
    {"1720", "TPM_CM", "TPM_DESC_1720"},
    {"1730", "TPM_CM", "TPM_DESC_1730"},
    {"1400", "TPM_CM", "TPM_DESC_1400"},
    {"4110", "TPM_MINOR_STOP", "TPM_DESC_4110"},
    {"3500", "TPM_MATERIAL", "TPM_DESC_3500"},
    {"5100", "TPM_MATERIAL", "TPM_DESC_5100"}
};

// TPM LOSS: captured on manual click, committed only on Select. Not an FMS queue.
// The last confirmed record survives dialog cancellation, but not application exit.
struct TTpmLossRecord {
    bool valid;
    UnicodeString code;
    UnicodeString unit, index, description;
    TDateTime requestedAt, selectedAt;
    int previousMode, robotSequence, robotStep, gripperSequence, gripperStep;
    int sourceStep, targetStep;
    UnicodeString sourceTrayId, targetTrayId, sourceChannel, targetChannel;
    TTpmLossRecord() : valid(false), requestedAt(0), selectedAt(0), previousMode(0),
        robotSequence(0), robotStep(0), gripperSequence(0), gripperStep(0),
        sourceStep(0), targetStep(0) {}
};
#endif
