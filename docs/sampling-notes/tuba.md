# tuba - Tuba

- Machine: `mda-tuba` (210.2 KiB packed samples, done)
- Catalog row #13 - Articulation: sustained
- Source: Iowa MIS / VSCO-2-CE
- `machine_verify.py`: 4/6 in tune (84/93 out of range by design - source only reaches C4)
- 2026-11-15 session 5: engine spawn-boundary fix applied (octave-high playback eliminated; loop now wraps at correct rate); session 7: `--end-fade 1.0` applied (ff blast fades to silence, no re-entrance bump before 2 s)

## What it should sound like
Sustained note with breath/bow component - steady, stable, no beating unless the loop causes it.
- none flagged (the "second peak before 2 s" the user's ear flagged was the loop re-entering the ff body behind a nearly dead envelope; it is fixed; see `issues-log.md` session 7)

## How to sample it
1. Put the pristine source file(s) next to the machine (see docs/source-inventory.md for the library path).
2. Play the same phrase through source and machine, or one note per zone across the keyboard.
3. Listen for, in order: (a) attack transient matches source, (b) sustain timbre is stable and not hollow/fizzy, (c) tuning matches equal temperament at the anchor note, (d) the **end of the stored sample** and its loop wrap - there must be no click or level jump, (e) decay tail behaves like the source (or like the capped note-length if this machine was rebuilt with a 4 s cap).
4. If this machine was capped (see rework line above), end-of-note is a fade + seamless loop - compare against the source over the first ~4 s, not the whole take.
5. Narrow-register machines: ignore the 0-3/6 verdict; judge blend and integrity by ear, not probes.

## Known issues / flags
- ear 2026-11-15: DEFECT: digital bounce issue; rebuilt 2026-11-15 (STEPS 2/2/2 - factor-6 alias mirrors removed) - the "bounce" turned out to be the engine's spawn-boundary octave bug; fixed 2026-11-15, verified by user ear-pass pending
