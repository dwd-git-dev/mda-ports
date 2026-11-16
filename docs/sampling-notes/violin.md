# violin — Violin

- Machine: `mda-violin` (801.1 KiB packed samples, done)
- Catalog row #3 - Articulation: arco (pizz see #3b)
- Source: Iowa MIS arco sweeps
- `machine_verify.py`: 5/6 in tune
- 2026-11-15 rework: none

## What it should sound like
Sustained note with breath/bow component — steady, stable, no beating unless the loop causes it.
- MIDI 36 is out of range (G3 root) — ignore the 36 probe.
- Pizzicato is a separate machine (mda-violin-pizz).
- G3 root = sampled from sul G; low register weak in source.

## How to sample it
1. Put the pristine source file(s) next to the machine (see docs/source-inventory.md for the library path).
2. Play the same phrase through source and machine, or one note per zone across the keyboard.
3. Listen for, in order: (a) attack transient matches source, (b) sustain timbre is stable and not hollow/fizzy, (c) tuning matches equal temperament at the anchor note, (d) the **end of the stored sample** and its loop wrap — there must be no click or level jump, (e) decay tail behaves like the source (or like the capped note-length if this machine was rebuilt with a 4 s cap).
4. If this machine was capped (see rework line above), end-of-note is a fade + seamless loop — compare against the source over the first ~4 s, not the whole take.
5. Narrow-register machines: ignore the 0–3/6 verdict; judge blend and integrity by ear, not probes.

## Known issues / flags
- 36 out of range; pizz source poor; ear 2026-11-15: OK (user pass, 2026-11-15)
