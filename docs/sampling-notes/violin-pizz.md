# violin-pizz — Violin (pizz)

- Machine: `mda-violin-pizz` (822.1 KiB packed samples, done)
- Catalog row #3b - Articulation: pizzicato only
- Source: Iowa MIS arco sweeps
- `machine_verify.py`: 5/6 in tune
- 2026-11-15 session 5: engine spawn-boundary fix applied (octave-high playback eliminated)
- 2026-11-15 session 7: the "bump" before 2 s the user heard was measured to be the source's own second hump (mf_G3_0_m55.wav: decays to 0.040 at 0.8 s, rises again at 1.0-1.2 s - the plucked string's body resonance). Not a defect; kept bit-identical to the 5/6-verified build.
- 2026-11-15 rework: tail-trim + 4.0 s per-zone cap (raised-cosine fade); loop wraps at end of stored array; note sustains ~4 s then loops

## What it should sound like
Sustained note with breath/bow component — steady, stable, no beating unless the loop causes it.
- none flagged

## How to sample it
1. Put the pristine source file(s) next to the machine (see docs/source-inventory.md for the library path).
2. Play the same phrase through source and machine, or one note per zone across the keyboard.
3. Listen for, in order: (a) attack transient matches source, (b) sustain timbre is stable and not hollow/fizzy, (c) tuning matches equal temperament at the anchor note, (d) the **end of the stored sample** and its loop wrap — there must be no click or level jump, (e) decay tail behaves like the source (or like the capped note-length if this machine was rebuilt with a 4 s cap).
4. If this machine was capped (see rework line above), end-of-note is a fade + seamless loop — compare against the source over the first ~4 s, not the whole take.
5. Narrow-register machines: ignore the 0–3/6 verdict; judge blend and integrity by ear, not probes.

## Known issues / flags
- cmd-line pizzistry uses same maps; ; ear 2026-11-15: DEFECT: bounce, maybe
