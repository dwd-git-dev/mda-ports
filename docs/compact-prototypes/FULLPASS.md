# The Full Instrument Pass — fresh-agent instructions

Read this BEFORE running anything. It is written so that an agent with no
prior context can execute the pipeline correctly. Everything below lives
under `workspace\`.

## What this is

Every instrument in the catalog is built from raw sample libraries into an
`mda-*` machine (MDA clone built from `mda-piano`, engine patched by the
builder), then verified mechanically with `machine_verify.py`. This folder is
the authoritative record of that process; `fullpass-log.md` is the ledger.

## The entry points

```
python compact-prototypes/build_all_engines.py            # all machines (background)
python compact-prototypes/build_all_engines.py --list     # print the plan
python compact-prototypes/build_all_engines.py --only flute
python compact-prototypes/build_all_engines.py --only recorder-alto
```

Dependent steps you should NOT need to call directly (build_all_engines does):

1. `prep_sweeps.py`      — pitch-track and silence-split sweep/single-note files
                           (Iowa MIS mono-DAT convention: octave-ish chromatic sweeps,
                           e.g. `AltoFlute.ff.C4B4.aiff`). Never map by filename alone.
2. `select_zones.py`     — pick 15 ET zones (mf > ff > pp preference, md5-unique,
                           evenly spread over the sampled range, audit ±45c).
3. `prep_sfz.py`         — resolve SFZ patches to concrete samples per note
                           (`pitch_keycenter` is authoritative, decode FLAC via
                           `compact-prototypes/_flack` (soundfile)).
4. `build_instrument_data.py` — packs 10-bit cubic + adaptive segments, patches
                           engine.cpp (zone OOB clamp), clones `mda-piano`.
5. `machine_verify.py`   — the only automated quality gate: six probes
                           (MIDI 36/48/60/72/84/93), pitch vs ET A4=440 (±50c),
                           silence/clipping/discontinuity checks.

## Ground rules (do not violate)

- **`sources/` is read-only.** Builds read it, never write. Work products land
  in `pristine-samples/<name>-work/` (mapping.json) and `pristine-samples/<name>/`
  (build output dirs used by the packer).
- **SFZ `pitch_keycenter` wins over filenames** — always. If a pitch audit and an
  SFZ disagree, trust the SFZ.
- **Host tests do not imply a good machine.** Every build ends at `machine_verify.py`;
  record the verdict. A machine is not "done" until all in-range probes pass.
- **No crossfades.** Steel drum must build from `jSteelDrum-noxfade.sfz`; velocity
  layers are collapsed to a single layer by `prep_sfz.py`.
- **Pan Flute has no CC0/public-domain source** (FreePats has none; VCSL/MIS/VSCO/
  Karoryfer don't either). It was dropped; do not add a nonfree source.
- **Scores are transpositions you ignore**: MIS, VCSL, FreePats all store
  *sounding* pitch. Map by measurement, never by printed clef.
- **Transposition traps:** alto flute = P4 below, bass flute = 8ve below,
  Eb clarinet = m3 above, bass clarinet = M9 below, soprano sax = M2 below,
  alto sax = M6 below, glockenspiel/crotales = 2 octaves above written.
- **VCSL string/harp/organ patches exist for other instruments** — check
  `source-inventory.md` before adding a duplicate of something we already have.

## What "done" looks like per machine

- `machine_verify.py` → all probes within range show ±50c ET; no silence,
  clipping or discontinuity flags; six probes rendered.
- `fullpass-log.md` row appended (build_all_engines does this automatically).
- Not acceptable: "host tests pass" alone, "it sounds okay to me", unaudited
  zone maps, or files silence-split without pitch tracking on sweep sources.

## Known machine-specific traps

- mda-violin MIDI 36: unbuildable (violin bottoms at G3=55); remains out of
  tune forever, expected.
- mda-timpani: only 5 VSCO single notes exist; machine will be severely limited
  but should be ET-correct if samples are.
- mda-flexatone: glissando idiophone, verify-fail expected.
- mda-marimba: 96 kHz / 24-bit sweeps; must resample at build (builder handles).
- mda-crotales: pp/ff only (no mf); reuse ff if mf missing.
- mda-hang: D-minor scale, not chromatic; our engine maps what it has; build
  is valid, coverage is intentionally limited.
- MIS sets with `roll.*` / `gliss.*` / `ambient*` files are excluded from
  builds (not single notes).
- Bell sets are sweeps in MIS (`bells.brass.zip`); marimba/vibraphone/
  xylophone sweeps need pitch tracking.
- MIS `MIDI`–`MIDI-chk` sets are 24 bit; rate comes from the AIF header.
  Do not re-tag rates — a mislabeled 96k file costs 13.5 semitones.

## Procedure for a failed machine

1. Read the exact `machine_verify.py` output (which probe, sign of error).
2. Wrong octave ±1200c → octave-folded zone (prep bug) — check the
   largest-|cents| files in the split report; swap the zone file.
3. Constant small offset (< 50c) → document; MIS instruments are intrinsically
   ~10-20c sharp/flat; acceptable if within tolerance.
4. Silence/gaps → file selection picked a transition/noise file; re-audit
   the chosen zone, pick the next-best file for that note.
5. Wrong pitch widely across ALL probes → the whole mapping is off; re-run
   prep_sweeps.py on that set; if it persists, compare against
   `source-pitch-playbook.md` conventions.
6. Re-build and re-verify before declaring the machine done. Record each
   defect + fix in `issues-log.md` with the root cause.

## The bar for quality

**The user cannot ear-check (only piano).** The pipeline is the gate: build →
`machine_verify.py` → record verdict → log issues. Do not ship a machine that
fails probes without logging why.
