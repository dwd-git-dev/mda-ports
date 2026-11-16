# bells — Glockenspiel

- Machine: `mda-bells` (778.2 KiB packed samples, adaptive-built)
- Catalog row #43 - Articulation: single (mallet)
- Source: Iowa MIS bells (brass)
- `machine_verify.py`: 1/6 in tune
- 2026-11-15 rework: tail-trim + 4.0 s per-zone cap (raised-cosine fade); loop wraps at end of stored array; note sustains ~4 s then loops

## What it should sound like
Struck idiophone — sharp onset, natural ring-down; pitch is the ring, not a held note.
- Iowa MIS brass bells. Narrow register. Probes fail by range, not quality.
- Listen for bell partials and strike transient — that is the whole instrument.

## How to sample it
1. Put the pristine source file(s) next to the machine (see docs/source-inventory.md for the library path).
2. Play the same phrase through source and machine, or one note per zone across the keyboard.
3. Listen for, in order: (a) attack transient matches source, (b) sustain timbre is stable and not hollow/fizzy, (c) tuning matches equal temperament at the anchor note, (d) the **end of the stored sample** and its loop wrap — there must be no click or level jump, (e) decay tail behaves like the source (or like the capped note-length if this machine was rebuilt with a 4 s cap).
4. If this machine was capped (see rework line above), end-of-note is a fade + seamless loop — compare against the source over the first ~4 s, not the whole take.
5. Narrow-register machines: ignore the 0–3/6 verdict; judge blend and integrity by ear, not probes.

## Known issues / flags
- (high register only); ; ear 2026-11-15: DEFECT: silent; too much space
- Narrow/limited register on piano-calibrated probes — see issues-log.md #5
