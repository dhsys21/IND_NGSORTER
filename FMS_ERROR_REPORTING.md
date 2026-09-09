# NGSORTER FMS Error Reporting

Original error-reporting version: 2026-09-08 003. Applied to `codex-improve`; no change to `main`.

## FMS Trouble operator continuation (2026-09-09 005)

AUTO mode selection is allowed while `FmsStatus.Trouble.Status` is ON.
A new FMS Trouble incident still raises the alarm and pauses work, but an explicit
START / Retry / Main Restart acknowledges that incident even while Status stays ON.
START uses the normal physical Restart checks before releasing the paused sequence.
Repeated polling of the same acknowledged error cannot re-pause work or reset a
different FMS transaction's retry. A changed ErrorNo or a new ON after OFF requires
a fresh operator acknowledgement. Closing the popup alone never resumes work.

Acknowledgement does not write FMS Status/ErrorNo, clear the published equipment
alarm, fabricate FMS responses, or bypass physical interlocks. The 50307 code
remains until a valid connected FMS snapshot confirms Status=OFF. Missing data and
request/response failures retain their existing timeout/retry handling.

## Wire Contract

The three EQP-only tags under `F1NGS01.EquipmentStatus` are published together:

- `Trouble.ErrorNo`: UInt32 array containing the active unique codes, in occurrence order.
- `Trouble.ErrorLevel`: the first complete ErrorNo value; 0 when no errors remain. This is NOT severity 1/2/4 or the first digit of a code.
- `Status`: 4 while any registered error remains. Otherwise 8 for Pause, 2 for active work, 1 for Idle. Clearing an alarm does not automatically restart motion.

Example: `ErrorNo=[20026,50121]`, `ErrorLevel=20026`, `Status=4`.
After both recover: `ErrorNo=[]`, `ErrorLevel=0`, and the current normal Status.
Unchanged snapshots are not repeatedly queued. The Gateway lock covers all fields and the pending-queue commit.

Gateway configuration: `D:\OpcUaGateway_IND\CONFIG\NGSORTER.Config.xml`.
`ErrorNo` uses ValueRank=1 / ArrayDimensions=0 (variable length). ErrorLevel documentation now specifies the first code.
Reload/restart the Gateway to apply XML changes. The FMS consumer must use this revised contract, not the old fixed-20-array / severity convention.

## Compact codes (2026-09-09 006)

The outbound code format changed from six to three minimum payload digits.
For example, old `50000307` is now `50307`, and old `20000026` is now `20026`.
The FMS consumer must update its code lookup table at deployment. The UInt32 types,
array handling, alarm lifetimes and incoming FMS Trouble codes are unchanged.

## Code Encoding

Two decimal category digits plus a minimum three-digit decimal payload.
Values >= 1000 retain all payload digits (for example 40 + 1035 = 401035), not a fixed three-digit truncation:

| Category | Source | Example |
| --- | --- | --- |
| 10 | `sscGetAlarm`, SSC_ALARM_SYSTEM | 35h -> 10053 |
| 20 | `sscGetAlarm`, SSC_ALARM_SERVO, all configured axes | 1Ah -> 20026 |
| 30 | `sscGetAlarm`, SSC_ALARM_OPERATION, all configured axes | 1Ah -> 30026 |
| 40 | `sscGetSystemStatusCode`, E***; remove E, convert remaining hex digits | E40Bh -> 401035 |
| 50 | Local equipment/sequence/FMS errors | Door #1 -> 50121 |

Native alarm values returned by the API are already integers. Do not interpret their display strings as decimal.
E*** system errors are system STATUS codes, separate from system alarms. See the [Mitsubishi API manual](https://www.mitsubishielectric.com/dl/fa/document/manual/ssc/ib0300225/ib0300225engl.pdf).
SSC function-call return errors remain in the existing WriteLog log; they are not reclassified as E*** system errors.

All entries in the existing alarm lists are mapped in [FMS_SERVO_ERROR_CODES.md](FMS_SERVO_ERROR_CODES.md).
Subcodes (for example 16.3) and affected axis details remain in local servo diagnostics. The FMS code identifies the main alarm (16h).
The same main alarm on multiple axes is reported once, but remains active until every corresponding axis has recovered.
Only a successful read of that same alarm source can replace/clear it. Disconnects and failed reads preserve the last confirmed alarm.

## Local Codes

| ErrorNo | Condition | Clear Condition |
| --- | --- | --- |
| 50121 | Door #1 open by existing IsSafetyDoorOpen logic | Fresh CC-Link input confirms clear |
| 50122 | Door #2 open | Fresh CC-Link input confirms clear |
| 50123 | Emergency stop active | Fresh input confirms released |
| 50124 | Keylock inactive during AUTO | Fresh input confirms keylock active |
| 50125 | Board status / alarm read failure | Successful read of every affected source |
| 50126 | X0023 BUFFER active | Fresh input confirms inactive |
| 50151 | CC-Link lost after a healthy connection | CC-Link ready again |
| 50152 | PLC status stale/lost after healthy communication | Fresh PLC status |
| 50153 | PLC ERROR (D10102) | Fresh PLC ERROR OFF |
| 50181 | Source barcode failure | Successful rescan, forced tray-out acknowledgement or discarded TrayLoad |
| 50182 | Target barcode failure | Same rule for target |
| 50201 | Eject recovery alarm | Pickup completes (step 9), manual completion is accepted, or guarded Init Work discards old work |
| 50202 | Insert recovery alarm | Insert completes (step 11), manual completion is accepted, or guarded Init Work discards old work |
| 50203 | MotionFault stop | Valid Restart or cancellation of the failed motion sequence |
| 50204 | Common sequence error dialog | Explicit acknowledgement or successful Restart |
| 50205 | General robot/interlock alarm dialog | Dialog acknowledged; native/physical faults remain independent |
| 50206 | NG count limit alarm | Operator chooses continue or tray out |
| 50207 | Load-factor alarm | Monitored loads return within the configured limit |
| 50209 | Manual-completion recovery failure | Report/storage completes or standby recovery completes |
| 50301 | Source TrayLoad failure / timeout / invalid data | Complete handshake or explicitly discarded load |
| 50302 | Target TrayLoad failure / timeout / invalid data | Same rule for target |
| 50303 | ProcessStart failure / timeout | Full response-reset handshake completes or guarded Init Work cancels the old request |
| 50304 | CellTrackOut failure / timeout | Full response-reset handshake completes, including accepted manual recovery |
| 50305 | ProcessEnd failure / timeout | Full response-reset handshake completes |
| 50306 | TrayUnload failure / timeout | Full response-reset handshake completes |
| 50307 | FMS-side Trouble.Status=true | Connected valid snapshot confirms false (Pause still requires acknowledgement) |
| 50308 | Manual Source TrayLoad failure | Manual handshake including response reset completes, or explicit AUTO entry discards the manual session |
| 50309 | Manual Target TrayLoad failure | Manual handshake including response reset completes, or explicit AUTO entry discards the manual session |

FMS failure kinds (timeout, rejected response, invalid data) share a per-transaction code; detailed reason, response value and phase remain in the FMS alarm window/log.
FMS Close and Retry do not clear the registered failure. Eject/Insert Retry and popup Hide likewise do not imply successful recovery.
Existing safety actions, input polarity, motion admission and restart interlocks are not changed by this reporting layer.
Unused door #3 is not added. CP/MS contacts without a verified fault polarity are not newly interpreted as faults.
Smoke/temperature information continues using the separate five EnvStatus tags.

## Verification

- Full C++Builder 10.1 Win32 Debug rebuild: 0 errors, 11 pre-existing warnings.
- `Tests/FmsEquipmentErrors_test.cpp`: encoding, UInt32 bounds, duplicate owners, order, clearing, variable arrays and handshake-step mapping.
- Catalog exporter verifies the layout of all four existing English alarm lists.
- Actual servo/CC-Link fault injection and live FMS receipt must be verified at the machine. Do not simulate a safety fault by bypassing a protective circuit.

Recommended acceptance cases: two axes with the same alarm; servo plus door alarm; failed read with an existing alarm; barcode rescan; FMS timeout -> Close -> Retry -> response 1 -> response 0; final alarm clear while still paused.
