# NGSORTER FMS Error Reporting

Version: 2026-09-08 003. Applied to `codex-improve`; no change to `main`.

## Wire Contract

The three EQP-only tags under `F1NGS01.EquipmentStatus` are published together:

- `Trouble.ErrorNo`: UInt32 array containing the active unique codes, in occurrence order.
- `Trouble.ErrorLevel`: the first complete ErrorNo value; 0 when no errors remain. This is NOT severity 1/2/4 or the first digit of a code.
- `Status`: 4 while any registered error remains. Otherwise 8 for Pause, 2 for active work, 1 for Idle. Clearing an alarm does not automatically restart motion.

Example: `ErrorNo=[20000026,50000121]`, `ErrorLevel=20000026`, `Status=4`.
After both recover: `ErrorNo=[]`, `ErrorLevel=0`, and the current normal Status.
Unchanged snapshots are not repeatedly queued. The Gateway lock covers all fields and the pending-queue commit.

Gateway configuration: `D:\OpcUaGateway_IND\CONFIG\NGSORTER.Config.xml`.
`ErrorNo` uses ValueRank=1 / ArrayDimensions=0 (variable length). ErrorLevel documentation now specifies the first code.
Reload/restart the Gateway to apply XML changes. The FMS consumer must use this revised contract, not the old fixed-20-array / severity convention.

## Code Encoding

Two decimal category digits plus a minimum six-digit decimal payload:

| Category | Source | Example |
| --- | --- | --- |
| 10 | `sscGetAlarm`, SSC_ALARM_SYSTEM | 35h -> 10000053 |
| 20 | `sscGetAlarm`, SSC_ALARM_SERVO, all configured axes | 1Ah -> 20000026 |
| 30 | `sscGetAlarm`, SSC_ALARM_OPERATION, all configured axes | 1Ah -> 30000026 |
| 40 | `sscGetSystemStatusCode`, E***; remove E, convert remaining hex digits | E40Bh -> 40001035 |
| 50 | Local equipment/sequence/FMS errors | Door #1 -> 50000121 |

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
| 50000121 | Door #1 open by existing IsSafetyDoorOpen logic | Fresh CC-Link input confirms clear |
| 50000122 | Door #2 open | Fresh CC-Link input confirms clear |
| 50000123 | Emergency stop active | Fresh input confirms released |
| 50000124 | Keylock inactive during AUTO | Fresh input confirms keylock active |
| 50000125 | Board status / alarm read failure | Successful read of every affected source |
| 50000126 | X0023 BUFFER active | Fresh input confirms inactive |
| 50000151 | CC-Link lost after a healthy connection | CC-Link ready again |
| 50000152 | PLC status stale/lost after healthy communication | Fresh PLC status |
| 50000153 | PLC ERROR (D10102) | Fresh PLC ERROR OFF |
| 50000181 | Source barcode failure | Successful rescan, forced tray-out acknowledgement or discarded TrayLoad |
| 50000182 | Target barcode failure | Same rule for target |
| 50000201 | Eject recovery alarm | Pickup completes (step 9), manual completion is accepted, or guarded Init Work discards old work |
| 50000202 | Insert recovery alarm | Insert completes (step 11), manual completion is accepted, or guarded Init Work discards old work |
| 50000203 | MotionFault stop | Valid Restart or cancellation of the failed motion sequence |
| 50000204 | Common sequence error dialog | Explicit acknowledgement or successful Restart |
| 50000205 | General robot/interlock alarm dialog | Dialog acknowledged; native/physical faults remain independent |
| 50000206 | NG count limit alarm | Operator chooses continue or tray out |
| 50000207 | Load-factor alarm | Monitored loads return within the configured limit |
| 50000209 | Manual-completion recovery failure | Report/storage completes or standby recovery completes |
| 50000301 | Source TrayLoad failure / timeout / invalid data | Complete handshake or explicitly discarded load |
| 50000302 | Target TrayLoad failure / timeout / invalid data | Same rule for target |
| 50000303 | ProcessStart failure / timeout | Full response-reset handshake completes or guarded Init Work cancels the old request |
| 50000304 | CellTrackOut failure / timeout | Full response-reset handshake completes, including accepted manual recovery |
| 50000305 | ProcessEnd failure / timeout | Full response-reset handshake completes |
| 50000306 | TrayUnload failure / timeout | Full response-reset handshake completes |
| 50000307 | FMS-side Trouble.Status=true | Connected valid snapshot confirms false (Pause still requires acknowledgement) |
| 50000308 | Manual Source TrayLoad failure | Manual handshake including response reset completes |
| 50000309 | Manual Target TrayLoad failure | Manual handshake including response reset completes |

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
