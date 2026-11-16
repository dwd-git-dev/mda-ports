# hand-bells-nepalese — Hand Bells, Nepalese

- Machine: `mda-hand-bells-nepalese` (227.7 KiB packed samples, done)
- Catalog row #54 - Articulation: single (bell)
- Source: VCSL (SFZ)
- `machine_verify.py`: 2/6 in tune
- 2026-11-15 rework: none

## What it should sound like
Sustained note with breath/bow component — steady, stable, no beating unless the loop causes it.
- 2026-11-15 tail-trim: verdict improved 1/6 -> 2/6; 3 keycenters; listen across the 3 zones for timbre step.

## How to sample it
1. Put the pristine source file(s) next to the machine (see docs/source-inventory.md for the library path).
2. Play the same phrase through source and machine, or one note per zone across the keyboard.
3. Listen for, in order: (a) attack transient matches source, (b) sustain timbre is stable and not hollow/fizzy, (c) tuning matches equal temperament at the anchor note, (d) the **end of the stored sample** and its loop wrap — there must be no click or level jump, (e) decay tail behaves like the source (or like the capped note-length if this machine was rebuilt with a 4 s cap).
4. If this machine was capped (see rework line above), end-of-note is a fade + seamless loop — compare against the source over the first ~4 s, not the whole take.
5. Narrow-register machines: ignore the 0–3/6 verdict; judge blend and integrity by ear, not probes.

## Known issues / flags
- (only 3 unique keycenters); tail-trim -30 rebuild 2026-11-15 (verdict improved 1/6 -> 2/6); ear 2026-11-15: DEFECT: too much space
- Narrow/limited register on piano-calibrated probes — see issues-log.md #5
- Narrow/limited register on piano-calibrated probes — see issues-log.md #5
