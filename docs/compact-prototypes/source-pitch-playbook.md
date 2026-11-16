# Source-pitch playbook: how a new instrument must behave

> Updated 2026-11-15 after the full rebuild. The pneumatic-rolls split now uses pitch tracking (strings) and equal-split-by-ladder (mallets) instead of amplitude slicing — see `compact-prototypes/split_sweep2.py` + `compact-prototypes/prep_gliss.py`. Everything else is current.

Every MDA sample machine is built the same way: a 22.05 kHz recording is
packed at 10 bits and played through the piano engine whose unison pitch
step (`delta = 0.5`) maps the recording to concert pitch **if** the recording
is in the expected pitch relation to the target. That relation has been the
source of almost every defect in this project. This document captures what
we learned the hard way (2026-11-15) so that new instruments can be built and
verified mechanically.

## The rule that makes a machine sound right

A zone in `mapping.json` has `root` and `high` (MIDI note range) and a
`file`. The engine maps MIDI `note` to zone with `note <= high` (after a small
hardness shift) and pitches the sample by `kStepQ16[note - root + 15]`.
At `note == root`, `delta = 32768 = 0.5`. The output sampling rate is
44.1 kHz, so the archived 22.05 kHz sample is played at **half speed** — one
octave lower than its stored pitch. That is the baseline:

> To emit concert pitch `C4` (261.63 Hz) at MIDI 60 with `root = 60`, the
> stored sample must contain ≈ 523 Hz.

MDA Piano's own sources satisfy this (source content for root=60 measures
~525 Hz). Anything measured by `pitch_hypotheses.py`, `audit_machine_data.py`
or listening that violates it will render sharp or flat against the piano.

**Concrete check:** after building, play MIDI 60 on the machine (e.g. with
`render_machine.py`) and YIN-measure the result — it must lie within
± 50 cents of ET's 261.63 Hz. If it does not, the source-normalization step
is wrong, not the mapping.

## The three pitch conventions we hit, by source

### 1. MDA Piano / `pristine-ref` (unchanged source)
Recorded at 22.05 kHz already. Pack with `factor=1` and ship. Render at
ET pitch. This is what `build_pristine_ref.py` does.

### 2. Iowa MIS / VSCO-2-CE / FreePats single notes
Recorded at 44.1 / 48 / 96 kHz at **concert pitch** (not written pitch),
verified by YIN against the filename on every library we could check.
Strategy: downsample to 22.05 kHz **without pitch shift**, then the engine
plays them an octave low at `delta=0.5`. That means you must record them
with their intended octave already in the file or pitch-shift up one octave
at build time.

What we did for flute / clarinet / oboe / / horn / trumpet /
trombone / tuba / saxophone /: build with the source as-is, and
the resulting machine happens to render at ET concert pitch because the
source jet-loaded at the intended rate. Verify with the ±50 c check above —
do not assume.

### 3. Multi-note sources — chromatic sweeps (strings, bass)
The Iowa MIS strings files (`SulG.D3B3.mono.aif` etc.) contain
**one chromatic run per file**, not one note. Our first attempt mapping
zone N to file N produced everything playing the same note (byte-identical
copies). Fix: silence-split each file and keep every note at its pitch.
Build zones file-by-file with names parsed from the filename.

`sweep_segments.py` does the silence-split; the resulting WAVs are named
`{dyn}_{NOTE}_{k}_m{midi}.wav` from the filename's note ladder
(e.g. `sulG.D3B3.mono.aif` -> D3..B3). Those files can be used directly as
normal single-note sources.

## Known mistakes (each cost us a build day)

| # | Mistake | Symptom | Fix |
|---|---|---|---|
| 1 | Source files assumed written pitch | everything transposed by its transposition | Measure first; MIS is sounding pitch |
| 2 | Chromatic sweeps kept as single samples | one note plays for the entire range; rest silent | Silence-split into per-note files |
| 3 | Duplicate files across zones | byte-identical `.wav`s in `mapping.json` | md5 the candidates before packaging |
| 4 | 24-bit AIF interpreted as 16-bit or as big-endian 24-bit | noise/low pitch | decode with correct 24-bit little-endian (ours works for `.aif`) |
| 5 | 96-kHz source declared as 44.1 kHz in AIF header | 13.5 semitones low (factor 44.1/96 exactly) | Re-rate at true rate |
| 6 | Writing `source_note` to match `root` limits | engine ignores it (unison maps from root only) | `source_note` is documentation; does not affect playback |
| 7 | `pitch_hypotheses.py` YIN octave errors on weak fundamentals | ±12 semitone outliers | Use harmonic-ladder (`ladder_check.py`) or ±50 c rule vs ET |
| 8 | Not evaluating one octave in isolation | wrong relation builds for the whole family | Build one instrument; switch; render; verify with `machine_verify.py`; then batch |
| 9 | Forgot engine is the same source of truth for *all* machines | think replace only the envelope or only samples | fix the common code path |
| 10 | Not every file at every rate is in tune | builds pass against "ff" only; pp/mf break differently | Verify across dynamics |
| 11 | Not normalizing to a known best sample when many
names match the same pitch (`pp` vs `mf` vs `ff`) | different dynamics sound different | Unify on one dynamic per instrument family (mf) |
| 12 | Gain staging differs from piano (~5 dB hot) | nothing audibly wrong, but louder than reference | Normalize RMS to the piano reference per build |
| 13 | Verifying without any verification | host tests pass but machine is wrong | Use `machine_verify.py`; pass = six probes in tune ±50 c |
| 14 | Hard-coding piano constants where an instrument needs overriding | update breaks all others | keep per-machine config and common engine separate |

## Things to check for every new instrument

1. **Run `label_scan.py` on the raw source set before mapping.** If files
   are multi-note, silence-split first.
2. **Check licensing and provenance** — every source either Iowa MIS,
   VSCO-2-CE, VCSL, or FreePats; record license, URL, and whether the file
   is sampled or synthesized. Note anything synthetic (`mdapiano` engine
   is fine synthetic, but sampled sources should not).
3. **Check in-tune across the library, not just at C4.** A library with
   bad low notes is still usable; you just have to say what the lower
   limit is.
4. **Compare bit depth and rate** before rebuilding — the engine assume
   22050 Hz samples; `SRD` in the builder must equal source rate
   (or be resampled correctly).
5. **Build one machine and verify it audibly before doing the rest.**
   Render C4 / C5 / A4 through `render_machine.py` and YIN the output.
6. **MD5-dedupe the source set before packaging.** Every zone assigned to a
   different MD5 unless it is truly the same note from two dynamics.
7. **If the build tests pass but the machine is wrong, that is information,
   not a bug to silence** — run the host tests anyway and capture the
   output.
8. **Document every mistake in `issues-log.md` with the root cause, not
   just the symptom.** Future-instrument maintainers will look there first.

## Fresh-agent recipe — build one instrument start to finish

Prerequisite tools in `compact-prototypes/`:
`label_scan.py`, `ladder_check.py`, `pitch_measure.py`, `pitch_wav.py`,
`sweep_segments.py`, `gen_mapping.py`, `build_instrument_data.py`,
`audit_machine_data.py`, `render_machine.py`, `verify_all.py`.

1. Get the source files (Iowa MIS / VSCO-2-CE / VCSL / FreePats). Record
   `source\<instrument>\README.md` with license-URL-file-mapping.
2. Inspect: `python label_scan.py <inst>`. If it reports
   "files without label" or multi-note files, stop — reduced to one-get-per
   source before mapping.
3. If the source files contain multiple notes each (chromatic sweeps), run
   `sweep_segments.py <inst>` to silence-split them into mono per-note WAVs,
   verify they renamed by filename, re-run `label_scan.py`.
4. Cross-check at least one file with `ladder_check.py` if YIN looks wrong.
   3 cent / minor third / +12 semitone false-positives are common on weak
   fundamentals. If ladder and label disagree, label wins.
5. Generate `pristine-samples\<inst>\mapping.json` with `gen_mapping.py`.
   Check the 15 zones map to sensible DIN `root`s (≥ 15 active files;
   MDA engine demands 15 entries).
6. `python build_instrument_data.py <inst>` - build.
7. `python machine_verify.py mda-<inst>` - run the full automated WAV/zone check.
   This is what the user will accept or reject without ears; it produces
   a verdict (PASS/WARN/FAIL) plus a per-zone report.
8. `python render_machine.py mda-<inst> 60 3` - quick render spot-check.
9. `python verify_all.py` - host tests should pass.
10. Add to `instrument-build-log.md` and `issues-log.md` if anything odd
    was found, with root cause.

Absolute rule: nothing is `done` until the automated pipeline passes.
Pitch audit and host tests are necessary, not sufficient.

| Library | Rate | Pitch | Notes |
|---|---|---|---|
| MDA Piano (upstream) | 22050 | 2x concert-style MDA convention | the reference every machine must match |
| Iowa MIS single-note | 44100 (16/24b) | concert | verified measurement (crotales, streamed stereo winds/brass) |
| Iowa MIS mono-DAT woodwinds/brass/mallets | 44100 (16/24b) | concert | **octave-ish chromatic sweeps** — pitch-track at build; mallets 96 kHz/24b (Marimba) needs resample |
| Iowa MIS xylophone/crotales | 44100 (24b) | concert | xylophone sweeps + gliss.up/down/roll exclusions; crotales = single notes, NO mf (pp/ff only) |
| Iowa MIS strings | 44100 | concert | chromatic sweeps — must be silence-split |
| VSCO-2-CE harp | 44100 | concert | 19 WAVs, audit result 4OK/2OCT — not ready for shipping until rebuilt and machine-verified |
| VCSL 1.2.2-RC | 44100/48000 (16/24b) | concert; **SFZ `pitch_keycenter` authoritative** | downloaded & mapped to candidates #29–72; 4249 files; folder map in `../source-inventory.md` |
| FreePats | 44100/48000 | concert; SFZ authoritative | bagpipe (French G, likely sharp of 440), ukulele, accordion (button), hang (D minor, NOT chromatic), electric organs (synthesized, not sampled), glass |
| jlearman.SteelDrum | 44100/16 FLAC | concert (C4–B5 chromatic) | The Unlicense; two SFZs (noxfade preferred, no velocity crossfades) |
| Karoryfer Cowsynth | 44100/16 | equal temperament (tuned voices) | CC0-1.0; synthesized medieval bagpipe — ET-safe reference for the bagpipe concept |

## The frame

1. **Source is the law.** Measure before you map; never trust filename.
2. **One instrument at a time.** Not twenty.
3. **Make the common mistake compiler-visible.** If a step is likely to be
   done wrong, write a check that turns it red, not a doc that says don't.
4. **Listening is delegated.** If you can verify by ear, do it - that is stronger evidence than FFT. Otherwise, use the automated WAV checker (`machine_verify.py`). It is not a substitute for ears; it is the next line of detection.
5. **Nothing is `done` until it passes the automated pipeline.** It checks silence, clipping, duplicates, pitch vs ET, discontinuities, silence ratio and ratio against ET for every zone - everything we can check without ears. The user decides what they accept; we do the checking.

