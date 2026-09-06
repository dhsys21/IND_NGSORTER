# Production Hardening - 2026-09-06 004

Latest update: `2026-09-06 005`, temporary FAT move-first overlap restored.

Branch: `codex-improve` only. Base: `13c20dd9f3eff85651e8ca71d3d90b03e40284b3`.
`main` is not changed. Original findings remain in `REVIEW_20260906.md`.

## Implementation

1. Pause sends DriveStop to all three axes, retains the full sequence step and accepted absolute targets, and reissues interrupted point moves only after explicit Restart and fresh stop/safety/centering checks. Interrupted HOME/JOG is not resumed automatically: request HOME/JOG again.
2. Failed point writes never call AutoStart. Failed starts/partial X/Y commands stop all axes and Pause. Callers return before advancing the step.
3. X0023 BUFFER ON stops robot and gripper sequences. Existing active-high BUFFER polarity is retained.
4. CC-Link board/link/read/received-size failures invalidate the input snapshot and stop active work. Signal recovery alone cannot resume work.
5. Robot timer owns Y003D. A fresh BYPASS ON edge enables it; completing the existing KEYLOCK set sequence clears it. Main's display timer no longer reasserts it. Unlock requires fresh all-axis stop confirmation, including the delayed door-output change. X0026/27 polarity is unchanged.
6. Approved Source TrackIn is copied before TrayLoad Request OFF. CellTrackOut uses this immutable TrayId/CellNo snapshot, including WorkFlag, after FMS deletes its tags. Identity mismatches stop reporting rather than silently rewriting CellId.
7. Normal mode waits for CellTrackOut and local result save before the next pickup. Revision 005 restores the temporary FAT Maximum speed option: next Source move is requested first, then the completed cell is saved/reported while that pickup runs. The gripper waits for the previous report before advancing to the next insert or tray exchange, so the single report slot cannot be replaced. Failed save/request retains the completed assignment and pauses for explicit recovery. This option does not disable physical interlocks.
8. Flush publishes staged PC tags atomically into pending memory. Pending values are removed only after successful socket write and only if no newer value replaced them. Failed writes retain the pending state. Reconnect resends locally authored EQP values.
9. Only Indy OnExecute writes TCP. It polls at 100 ms; Windows SO_SNDTIMEO is 2000 ms. No IOHandler WriteTimeout is used. Shutdown has a bounded 500 ms final-state drain. Existing worker-to-main log dispatch is retained.
10. Manual-to-Auto preserves accepted Response=0-wait phases. Only requests without an accepted result are retried; timeout clocks restart on mode return.
11. Barcode processing requires CR/LF and an active scan. Unterminated tails remain buffered. Timeout closes the old socket so late data cannot be attributed to a new scan. Buffer size is bounded; simultaneous triggers are rejected.
12. FMS EnvStatus uses one data lock for all five fields and one published batch. Existing 0.05 C / Boolean change detection remains. Local EQP state survives reconnect.
13. PLC parsing uses a bounded binary accumulator. Length is in bytes, including end code. Seven-word reads require 25 bytes; partial frames do not complete a request. Coalesced frames are retained and error/malformed frames close the connection.
14. Robot and gripper progress watchdogs stop active waits after 120 seconds without a step change. Pause resets the watchdog; FMS waits and target exchange keep their existing timeouts/operator-wait behavior. Existing shorter sensor timeouts remain.
15. Source and Target reject repeated nonzero CellNo before TrayLoad approval. Zero-filled unused records remain permitted. Production does not use the live-tag test fallback.
16. Tray INI saves use a validated, flushed temporary file, atomic replacement and `.bak`. Save failures return false, retain physical completion, and Pause at the commit step. Restart retries pending saves before motion. CSV rows use whole-file replacement; failed saves retain timing data. `TRANSFER_RECOVERY` records source/target IDs, cell numbers, pickup/insert completion and FMS acceptance separately.

## Verification

- Full C++Builder 10.1 Update 2 Win32 Debug build: 0 errors, 11 existing warnings (no equipment program launched).
- Existing FMS current-response, alarm recovery, process-order, deferred-tray and target-exchange regressions.
- Door/PLC admission harness updated for the current TrayLoad-before-centering contract.
- `tmp/ProductionHardening/test_hardening.ps1`: actual production function bodies with fake SSC calls; split/coalesced MC frames; send-ACK preservation; failed-save retention; static wiring checks.
- All seven regression runners above plus `tmp/ProductionHardening/test_maximum_speed.ps1` passed (eight total). The original motion-reproduction runner now delegates to the corrected regression.
- CP949 source files remain CP949. English additions are ASCII.
- Revision 005: Maximum speed also starts the Source request in the same scan when Optimize sequence delay is OFF. Immediate X/Y requires a freshly read Z=0. Added a fake-FMS/disk regression for post-move report retention and mode-change recovery.

## Required Commissioning

These are software changes, not validation of the safety circuit. Do not bypass safety contacts to test them.

- At guarded low speed: Pause during X/Y and Z movement, verify actual deceleration/stop and explicit Restart to the original destination. Verify HOME requires a new command after interruption.
- Verify door outputs remain locked until all axes have actually stopped. Confirm X0023 polarity and stop behavior against the wiring and mechanical buffer arrangement.
- Disconnect CC-Link/PLC during guarded motion; restore communication and verify no automatic restart. Check centering loss while paused blocks Restart before any new command.
- Delay/disconnect Gateway during each handshake phase, including longer than 30 seconds. Verify UI responsiveness, pending reports, reconnect state and EnvStatus five-field coherence with the real Gateway.
- Test disk-full/access-denied and process interruption at pickup, insert and FMS acceptance. Inspect `.pending`, `.bak` and `TRANSFER_RECOVERY` against physical tray contents before restarting production. These files aid reconciliation; they do not prove physical state after an uncontrolled power loss or provide automatic crash recovery.
- Check the conservative 120-second watchdog against the longest legitimate HOME/travel operation. Adjust only after measuring the machine's worst-case time.
- Windows socket write success is transport delivery, not FMS acceptance. Application four-phase responses remain mandatory; there is no exactly-once guarantee from TCP alone.
- FAT Maximum speed intentionally allows pickup before the previous cell is durably saved or acknowledged. Remove this temporary option after acceptance; use normal mode for production. Measure overlap, delayed responses, disk failure and full/final-tray boundaries on the guarded machine before the speed demonstration.
