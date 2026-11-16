# electric-bass — Electric Bass

- Machine: `mda-electric-bass` (274.4 KiB packed samples, done)
- Catalog row #16 - Articulation: picked
- Source: FreePats (CC0)
- `machine_verify.py`: 2/6 in tune
- 2026-11-15 session 5: engine spawn-boundary fix applied (octave-high playback eliminated)
- 2026-11-15 session 7: `--end-fade 1.0` applied (the loop-re-entrance ghost that made the note jump back to -31 dB at 0.74 s is gone; the note now dies to silence instead)

## What it should sound like
Attack transient + decay toward silence — note dies naturally; do not expect sustain.
- Picked finger-style bass; narrow keycenter set (zones repeat); watch zone-to-zone timbre step.

## How to sample it
1. Put the pristine source file(s) next to the machine (see docs/source-inventory.md for the library path).
2. Play the same phrase through source and machine, or one note per zone across the keyboard.
3. Listen for, in order: (a) attack transient matches source, (b) sustain timbre is stable and not hollow/fizzy, (c) tuning matches equal temperament at the anchor note, (d) the **end of the stored sample** and its loop wrap — there must be no click or level jump, (e) decay tail behaves like the source (or like the capped note-length if this machine was rebuilt with a 4 s cap).
4. If this machine was capped (see rework line above), end-of-note is a fade + seamless loop — compare against the source over the first ~4 s, not the whole take.
5. Narrow-register machines: ignore the 0–3/6 verdict; judge blend and integrity by ear, not probes.

## Known issues / flags
- core click at loop wrap GONE (session 7 end-fade); ear 2026-11-15: DEFECT: bounce, maybe - that's the loop re-entrance, now fixed
- narrow keycenter set; ear 2026-11-15: OK (user pass, 2026-11-15)
