# steel-drum — Steel Drum

- Machine: `mda-steel-drum` (156.0 KiB packed samples, done)
- Catalog row #64 - Articulation: mallet
- Source: jlearman.SteelDrum (Unlicense)
- `machine_verify.py`: 4/6 in tune
- 2026-11-15 rework: dead (sub-40 dB) tail removed; pitch untouched; verdict unchanged

## What it should sound like
Struck idiophone — sharp onset, natural ring-down; pitch is the ring, not a held note.
- 2026-11-15 tail-trim: dead tail removed; tenor pan, bright mallet onset.

## How to sample it
1. Put the pristine source file(s) next to the machine (see docs/source-inventory.md for the library path).
2. Play the same phrase through source and machine, or one note per zone across the keyboard.
3. Listen for, in order: (a) attack transient matches source, (b) sustain timbre is stable and not hollow/fizzy, (c) tuning matches equal temperament at the anchor note, (d) the **end of the stored sample** and its loop wrap — there must be no click or level jump, (e) decay tail behaves like the source (or like the capped note-length if this machine was rebuilt with a 4 s cap).
4. If this machine was capped (see rework line above), end-of-note is a fade + seamless loop — compare against the source over the first ~4 s, not the whole take.
5. Narrow-register machines: ignore the 0–3/6 verdict; judge blend and integrity by ear, not probes.

## Known issues / flags
- ; ear 2026-11-15: OK (user pass, 2026-11-15)
