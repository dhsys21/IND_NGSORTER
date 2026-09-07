# Manual Cell Completion (codex-improve)

Version: 2026-09-07 002. Applies only to NGSORTER's improvement branch.

## Operator Procedure

1. Pause the failed transfer. Use the site's approved door/safety procedure before
   intervening. Finish inserting the selected Source cell into the Target tray.
2. Make the gripper empty and open, close/lock the doors and restore safety.
3. In either Eject Error or Insert Error, click **Manual work complete**.
4. Check Source Tray/channel, Target Tray and Cell ID. Enter the actual Target
   channel, check the physical-completion confirmation, then click **Confirm and report**.
5. Wait for CellTrackOut success and Response=0. On a report error, correct the
   FMS/connection/storage problem and use **Retry**. Do not transfer the cell again.
6. Click **Return to standby and resume**. The robot raises Z and returns to the
   taught standby position. After position, stop, origin and safety checks, automatic
   sorting continues with the next unprocessed Source NG cell.

The FMS status label above the detailed message remains visible throughout the
handshake. Amber distinguishes report preparation, Request OFF/reset wait, Request
ON/response wait and accepted-report reset wait. A stopped incomplete handshake is
red and preserves the error detail below it. Green completion appears only after
the response reset phase finishes; reset-timeout Retry still displays reset wait.

The ordinary Complete buttons retain their separate meanings: Eject complete
requires a cell held in the gripper; Insert complete requires an empty/open gripper.
They raise Z and continue the appropriate existing sequence without replaying the
interrupted XYZ move. A cell already placed by the operator should use the new
Manual work complete workflow so the actual Target channel can be confirmed.

## Data and Handshake

- Uses the production `TMesOpc::CELL_TRACK_OUT_REQUEST` overload with explicit
  Source/Target Tray IDs, source/target channels, Cell ID and CellUnloadComplete=true.
- Reads cell metadata from the approved Source snapshot, not deleted live FMS tags.
- Response 1 means success; 2 means failure. Other nonzero values are errors.
- Request OFF is drained from the asynchronous queue before a new Request ON.
  Transport-write completion is not treated as FMS acceptance: Response is checked separately.
- Success is retained while waiting for Response=0. Reset-timeout Retry does not
  resend an accepted cell. No Cycle Test bypass is used in this recovery transaction.
- Source becomes CELL_EXIST=false/PICK=N; the actual Target slot receives all
  retained cell metadata and becomes occupied. An unused old reservation is released.
  Other occupied/reserved slots cannot be overwritten. These maps feed the existing
  final cumulative TrackOutCellInformation report.
- A pending normal/FAT report blocks a new manual report. Normal automatic start,
  work reset, error-window movement and PLC tray unload are blocked while recovery is pending.
- The existing trays and process are retained. No new TrayLoad or ProcessStart is
  issued just to complete the manually handled cell. The transfer CSV records the actual Target channel.
- ManualCellOut tags/XML were not changed or deleted; vendor agreement remains pending.

## Persistence and Validation

`ManualCellCompletion.ini` beside the EXE records confirmed identity/phase before
map changes and before request transitions. It is atomically replaced with a flushed
temporary file. A storage failure blocks progress and permits a storage-only retry
after acceptance. Logs include the confirmed cell, tray/channel pair and errors.

After an uncontrolled application restart, a non-idle/corrupt journal blocks automatic
operation. This version does not automatically reconcile unknown FMS outcomes after
a process crash. An engineer must compare the physical trays, saved maps/journal and
FMS result before recovery. Do not delete the journal merely to clear the interlock.
The protocol has no transaction ID, so FMS must also handle a repeated identical
CellTrackOut after a lost response; exactly-once processing cannot be guaranteed by PC alone.

Validation: actual extracted production methods in `tmp/ManualCompletion/test_manual_completion.ps1`,
the eight existing regression runners, and a full Win32 Debug build. The default
Debug TDS was locked, so the verified executable is built under
`tmp/ManualCompletion/Win32Debug/` using a command-line FinalOutputDir override only.
No equipment or production application was launched. Field validation with the real
FMS and interlocked robot is still required before production use.
