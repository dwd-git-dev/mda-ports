# harp — Harp

- Machine: `mda-harp` (623.1 KiB packed samples, done)
- Catalog row #2 - Articulation: single (plucked)
- Source: VSCO-2-CE (CC0)
- `machine_verify.py`: 5/6 in tune
- 2026-11-15 rework: none

## What it should sound like
Attack transient + decay toward silence — note dies naturally; do not expect sustain.
- Very long decay (10.5 s zones). Packed small because the planner drops to factor 6 quickly. Listen for the natural fade.

## How to sample it
1. Put the pristine source file(s) next to the machine (see docs/source-inventory.md for the library path).
2. Play the same phrase through source and machine, or one note per zone across the keyboard.
3. Listen for, in order: (a) attack transient matches source, (b) sustain timbre is stable and not hollow/fizzy, (c) tuning matches equal temperament at the anchor note, (d) the **end of the stored sample** and its loop wrap — there must be no click or level jump, (e) decay tail behaves like the source (or like the capped note-length if this machine was rebuilt with a 4 s cap).
4. If this machine was capped (see rework line above), end-of-note is a fade + seamless loop — compare against the source over the first ~4 s, not the whole take.
5. Narrow-register machines: ignore the 0–3/6 verdict; judge blend and integrity by ear, not probes.

## Known issues / flags
- after clean rebuild (2026-11-15); ear 2026-11-15: OK (user pass, 2026-11-15); rebuilt 2026-11-15 (STEPS 2/2/2 — factor-6 alias mirrors removed)
