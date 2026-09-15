# Emergency work recovery

The FormConfig checkbox below Target Tray Unload is opt-in (default OFF).
It is saved in MainSystemInfo.inf, RECOVERY / EMERGENCY_AUTO_RESTART.
The snapshot itself exists only in memory until the application exits.

## Operator flow

1. Enable the option and Apply before starting production.
2. EMG during AUTO captures the selected cell and pauses production.
3. Release EMG, close/lock doors and restore safety, Servo OPEN and ON.
4. Run HOME. Held-cell OPEN commands are blocked while recovery is enabled.
5. Confirm both original trays are present and centered, then select AUTO.
6. Press START/Restart explicitly. Selecting AUTO alone never moves the robot.

| Confirmed checkpoint | Restart action |
| --- | --- |
| Before pickup, empty and OPEN | Repeat the same Source channel move/pickup |
| CHUCK confirmed with cell | Save Source pickup, start Target move/insert |
| Target release acknowledged or insert committed | After HOME confirms empty/OPEN, save insert and use the existing CellTrackOut handshake |
| Ambiguous sensors/output transition, changed tray/cell, outstanding report | Block automatic recovery; resolve with manual completion/reset as appropriate |

No TrayLoad or ProcessStart is repeated for a valid recovery. Old accepted axis
targets are discarded after HOME. Completion reporting uses the existing
response/reset flow, and the recovery report path does not use FAT move-first.
A pending report from before EMG is not automatically replayed by this feature.

The snapshot is retired after successful recovery, successful manual completion,
RESET WORK or INIT SEQ. Removing either tray after capture invalidates recovery.
Changing CellId/LotId/NGCode/Grade/WorkFlag, slot occupancy or channel assignment
also blocks it. If manual OPEN is necessary, disable the option and use the
existing manual recovery workflow; simply removing a held cell does not count
as a completed transfer.

## Existing behavior with the option OFF

AUTO requires Servo OPEN/ON/HOME plus physical Gripper OPEN and cell-clear.
Removing a cell only satisfies this entry check: it does not update the Source,
Target or FMS records. START resumes FMS service and Restart releases retained
sequences. Servo OPEN can reset the gripper step to zero while retaining its
sequence/tool completion flags. Consequently a physically removed but already
committed pickup can reach the Target-move cell-detect interlock. RESET WORK
starts admission/TrayLoad again; it is not a manual insert confirmation.

## Verification

Tests/EmergencyWorkRecovery_test.cpp exercises checkpoint classification and all
512 Boolean combinations. Tests/EmergencyWorkRecovery_test.ps1 checks integration
gates and setting persistence. Full Win32 Debug build is required.

2026-09-11 verification: classifier and integration checks passed, as did the
existing Z profile/start/speed and PLC safety tests. Win32 Debug full compilation
and link passed (0 errors, 11 existing warnings), version label 006.
The command-line Rebuild removed NGSORTER.res; regenerate it using the
BuildVersionResource target with ForceRebuild=true, then run Build with
ForceRebuild=true. No project build settings were changed.

Hardware acceptance must cover EMG before CHUCK, after CHUCK, during Target move,
after OPEN acknowledgement, during HOME, a repeated EMG, a removed tray, a lost
cell, and an outstanding CellTrackOut. Confirm actual cell/tray records and FMS
reports; no hardware motion is executed by these offline tests.

## Version 007 recovery corrections

- HOME command/read errors and the motion watchdog cancel recovery HOME into
  Pause with no saved motion to replay. Correct the fault and press HOME again;
  each explicit HOME gets a fresh watchdog budget.
- While the enabled EMG snapshot is pending, the main tray timer preserves its
  admission steps. A temporary centering OFF no longer causes a Target barcode
  reread after restart; tray removal still invalidates the recovery record.
- When FMS Trouble overlaps EMG, START/Restart/popup Retry first acknowledges the
  FMS incident only, including in MANUAL. It does not start motion. Complete HOME,
  select AUTO, then press START/Restart again; all recovery gates still apply.
- Tests/EmergencyRecoveryFlow_review.ps1 now checks corrected behavior rather
  than expecting the defects. It uses extracted production bodies with offline
  doubles, not connected hardware.
- Version 007 verification: 11 flow checks and existing EMG integration/Z-axis
  regression checks passed. Full Win32 Debug compilation and link completed with
  0 errors and 11 existing warnings. Hardware recovery tests remain required.
