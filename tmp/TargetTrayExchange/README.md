# Target Tray Exchange (2026-09-06)

## Behavior

- FormConfig > Target Tray Unload accepts 0 through 96. SAVE persists
  `[TARGET_TRAY] UNLOAD_CELL_COUNT` in the existing `MainSystemInfo.inf`.
- 0 preserves full-tray-only automatic exchange. Existing settings default to 0.
- A nonzero limit is checked only when the current Source tray has no further NG
  cells. For example, reaching 10 midway through a Source tray does not stop it.
  The accumulated occupied Target count at Source completion is compared using
  `>=`. Existing Target cells count; uncommitted `PICK=R` reservations do not.
- At the limit, a notice and **Unload Target Tray** button appear below the
  equipment log. The next Source barcode/process and payload reset are held.
  Source ProcessEnd and Source Tray Out retain their existing sequence.
- The button is enabled after reports complete and the equipment is safely idle.
  Remain in AUTO; this is an operator confirmation, not a Pause/Restart operation.
- The PLC Target Tray Out output is issued only after the existing FMS
  TrayUnload handshake and a nonblocking 3-second delay with a final safety check.
- Actual full trays still exchange automatically during a Source process.
  Completion preserves that Source's remaining cells and ProcessStart state.
- Target In and Centering must both be observed OFF before a replacement ON is
  accepted. The replacement barcode is read again and the existing FMS/local tray
  validation and four-phase TrayLoad complete before work can resume.
- A replacement without a free slot is exchanged again; it is never released for
  insertion. A partially filled replacement with free slots is usable, with the
  configured limit checked again at the next Source completion.
- Manual, Pause, FMS errors, or missing communication retain the exchange state.
  Work initialization is blocked during exchange so reports are not discarded.

## Safety Conditions

Target unload requires no cell operation/reservation or pending FMS report,
fresh PLC Target In/Centering, CC-Link ready, safety ready, an empty/open gripper,
no buffer alarm, all servo axes ready/homed/in-position/not operating, and live
Z command position 0. A failed SSC read blocks unloading. These checks are not
bypassed by Cycle/FAT options. They supplement, not replace, hardware safety.

## Offline Verification

Run from the repository root:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tmp/TargetTrayExchange/test_exchange.ps1
```

The test extracts the production function bodies and compiles them using bcc32
with fake PLC, SSC, FMS, barcode and UI boundaries. It does not run NGSORTER or
connect to any equipment. Coverage includes threshold boundaries, reservations,
late reports, automatic full exchange, no duplicate unload, the delay and live
safety recheck, departure-before-arrival, Source preservation, Source-absent
replacement, Pause/Manual/FMS recovery, full replacement rejection and barcode
retention after Gateway loss. It also checks INI wiring, DFM bindings and all
three language keys.

Existing FmsCurrentResponseFix, FmsAlarmPauseClose, ProcessOrderFix and TrayWaitFix
regressions were run. Win32 Debug full Build passed with 0 errors and the 11
existing warnings. CP949 source encodings and language-file BOMs were preserved.
The production application was not started; actual PLC timing, servo positions
and visual behavior still require field verification.

## Field Acceptance

1. Set 10; use a Source tray that raises Target occupancy from 9 to more than 10.
   All remaining NG cells in that Source must finish before the notice appears.
2. Verify no next Source processing before the button and valid replacement.
   Click the button once, check FMS success/reset, then PLC unload and new scan.
3. Set 0 and start near 96 occupied cells. Confirm mid-Source automatic exchange
   and continuation at the next unprocessed Source cell without ProcessStart again.
4. Try Pause and FMS timeout/failure during exchange. Recover using the existing
   Retry/Restart procedure; no early PLC output or duplicate cell movement.
5. Verify disconnected PLC/CC-Link, an occupied gripper, motion or Z not up keeps
   unload disabled. Perform fault checks only under the site's safe test procedure.
6. Verify English/Korean/Hindi captions, setting persistence after restart, and
   that the 96-cell maps remain fully visible when the notice is shown.
