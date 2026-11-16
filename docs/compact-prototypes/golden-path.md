# Golden Path — building an MDA instrument correctly

Read this file first, then `source-pitch-playbook.md` for the pitch details, then `instrument-build-log.md` (master log), `issues-log.md` (defect classes & investigations), then `digital-sound-test.md` (render/FFT harness). This file exists because a cold-start agent can produce broken machines that pass host tests - host tests do **not** check pitch, timbre or loudness. Build → test → **listen**, or it isn't done.

## 0. Status banner (2026-11-15, post ear-pass + spawn-fix session)
Only `mda-piano` is signed off defect-free. **On 2026-11-15 every machine in the catalog received a user ear pass** (`listening-log.md`). The systematic ear-pass investigation cold-start rebuild and spawn fix (session 5) and loop-reentrance fix (session 7) landed on 2026-11-15: whole-engine bug affected 33 machines (octave-high forever; see defect #13) and ~4 machines had loop re-entrance ghosts (defect #14). Rebuilt machines are re-verified at their prior verdicts. **But no rebuilt machine has been ear-verified yet.** Didgeridoo was removed entirely at user request. Everything without a comment in `listening-log.md` is signed off by the user as fine. Narrow-register idiophones scoring 0-3/6 remain measurement-tool artefacts. Catalog: 69 machines.
**Current "done" bar (reminder):** `machine_verify.py` (6 probes vs ET, octave-fold; `--probes auto` for narrow/special-register instruments) + `sweep_verify.py` (all 69) + freshly re-rendered `midi60-samples/` + (user sign-off). A build that passes only `sdk.py test` is not done.

## 1. The pipeline, with gates
Each step has a measurable pass/fail gate. A step without a gate is skipped at your peril.

0a. **Sources on disk** — the full pass's raw libraries are downloaded, extracted and cataloged under `workspace\sources\` (see `source-inventory.md` for contents, the VCSL folder map, sweep classification, and `sources/<lib>/<set>/README.md` + VCSL per-folder READMEs for per-instrument provenance and measured features). Do not rename or modify files there; builds read from it, never write. Iowa MIS mono-DAT sets (flutes, clarinets, saxes, bass trombone, all mallets except crotales) are octave-ish chromatic sweeps and must go through the pitch-tracking path — never zone-mapped by filename.
0. **Source pitch audit** — `pitch_hypotheses.py <instrument>` for single-note files; `sweep_segments.py` for chromatic sweeps (silence-splits at 20 ms RMS, YIN per segment, filename ladder match). Gate: every file within ±50 cents of its label; any ±1200/2400-cent flag is a YIN false octave (re-check by FFT harmonic ladder, don't trust). Any constant whole-library offset → content defect (viola case: see below) - do not build until resolved.
1. **Zone plan** — one file per zone; `source_note` = the measured pitch of the file, never a filename assumption; hash every file used per zone (md5), duplicates = bug.
2. **Resample** to engine rate (22.05 kHz mono in current builds), find loop, trim leading silence.
3. **Adaptive plan** — factors 2 and 6 only. `hybrid_rate.py`: `removed_band_lo = min(4000, cutoff_for_factor(factor))`; `check_segments` must pass. Budget: **≤801 KiB ceiling (mda-violin)** — piano ≈ 317 KiB for short notes; ~778 KiB for long sustained winds/organs via 4 s per-zone caps (see `instruments-rework-log.md`). Trim dead tails with `--tail-trim`; cap long zones with `--cap-long`; never cut rate/bit-depth/zone count to hit budget.
4. **10-bit pack.**
5. **Build machine data + engine** — see `new-engine-playbook.md`.
6. **Host tests pass** — necessary, unlocks nothing.
7. **Render audit** — render C4/E4/G4/C5 via the `digital-sound-test.md` harness. Gate: FFT peak within ±50 cents of equal temperament (percussion: exempt - check onset/gap instead); silence/discontinuity checks clean. For a full re-verification, run `python compact-prototypes/sweep_verify.py` (every built machine), not just the one you changed — any engine change can move octave-fold behavior by several semitones (see issues-log session 5).
8. **User A/B listen vs pristine** — the only human sign-off; the user must listen, then the machine is marked verified. **Without step 8 the machine is not done, whatever the host tests say.**
8a. **Every rebuild needs a fresh midi60 render.** `midi60-samples/` is the listening artifact: after ANY rebuild, re-render all 69 machines (`compact-prototypes/render_all_midi60.py`). Rendering one and forgetting the rest is how stale WAVs linger (the accordion was once undone by an earlier bulk re-render).
9. **Size + device flash** — performance measured on device, host numbers are not hardware results.

## 2. Defect classes actually hit, and their regression gates
| # | Defect | Symptom | Root cause (measured) | Fix | Gate |
|---|---|---|---|---|---|
| 1 | Unsplit chromatic sweep mapped to whole zone range | every note plays the sweep from note-on | Iowa MIS strings are one file per semitone-range, not per note | silence-split; per-note files (strings), equal-split-by-ladder (mallets) | per-zone FFT |
| 2 | Byte-identical zone files | all zones play one sample | violin `extracted_notes/` roots 36/40/43/48/52 all md5 `4E09EB85…`; percussion set under plateau-YIN produced near-duplicates | regenerate splits; md5 over zone list | md5 uniqueness over zone list |
| 3 | Sample-rate declared wrong | everything ≈ −13.5 semitones | viola: 96 kHz content in 44.1 kHz AIF; content = label × 0.459375 exactly | resample/re-rate from true rate | render FFT ±50 c |
| 4 | Written-vs-sounding pitch misbelief | every transposing instrument shifted by its transposition | **disproven 2026-11-15**: all Iowa MIS single-note sources measure as *sounding* pitch (flute 440.9, clarinet 261.9 for "C4", alto sax 219.9 for "A3", trumpet +4 c …) | sounding pitch (MIDI) mapping | audit step 0 |
| 6 | Band wider than new factor removes | hollow/fizzy decay | switch point chosen by fixed 4–11 kHz band | `removed_band_lo`, `check_segments` (step 3) | band-aware plan |
| 7 | Factor 3 segment | 5–10 kHz mirror fizz | Nyquist 14.7 kHz images | factors 2/6 only | plan review |
| 8 | Host tests pass, music wrong | all fine on paper, sounds broken | tests don't check pitch/loudness | steps 7–8 | ear |
| 9 | Engine pitch-step index out of bounds | machine crashes / 0xC0000005 when note > root+15 or < root-15 | `note - root + 15` index outside the 31-slot array | clamp step index 0..30 at build time (done 2026-11-15) | machine_verify renders six probes clean |
| 10 | SFZ zone high<root | `note_from_pitch` fails for notes between duplicate roots; engine misbinds | `select_zones.py`/`prep_sfz.py` duplicate roots for low-unique-count sources, encoding `high=root-1` | cap zones at unique keycenter count (rebuild launched 2026-11-15); duplicate roots share one cover zone, `high=nextDistinctRoot-1` | host tests pass; six probes clean |
| 11 | Host tests fail rc=26/32 on SFZ clones | host test execution fails at return 26/32 | SFZ-packed zone maps don't satisfy piano-specific 88-note + stretch invariants the `tests/sound.cpp` assumes (and the stale duplicate ones segfaulted) | clones now always rebuilt from pristine `mda-piano`; see issues-log §"Known issues" #3 | host tests pass (modulo `machine_verify.py`) |
| 12 | Gain staging mismatch | machines ±5 dB | no per-machine normalization | normalize or document target | loudness report |
| 13 | Spawn boundary evaluates spawn at segment boundary (empty segment 1) | machine plays an octave high permanently; jumps loop wrong rate forever; machine_verify hides via octave-fold | `voice.position > voice.bN` uses strict `>` when spawning voice at position == b1 (empty segment 1); the division by the stored-rate factor never happens | spawn conditions `>=` in all 68 non-piano engines; clone-time patch in builder; piano untouched (b1 > start always) | run the FULL sweep after any engine touch; YIN on render, not just ET |
| 14 | Loop re-entrance ghost | note decays to floor; loop wraps to loud body and replays it at low-but-audible level — a "second peak" a second before you expect one | the engine has no note-off/release; zone loops forever; stored body replays forever behind a nearly dead envelope | `--end-fade SECONDS` builder flag: ramps stored zone's tail to zero; builder loops the quantizer-silent run | `scan_reentrance2.py` over `midi60-samples/` (no re-entrance after note's peak decays below -50 dBFS) |

## 3. Definition of done (copy into the log per machine)
- [ ] Step 7 pass (FFT ±50 c, silence/continuity checks) - raw output attached.
- [ ] Step 8 user sign-off recorded verbatim.
- [ ] Log entry: source, sizes, render frequencies, defects found+fixed, sign-off date/user.
- [ ] `instrument-build-log.md` status flipped to `done`. No `done` without step 8.

## 4. Doc precedence when things conflict
1. Measurement (FFT/YIN/hash) beats convention.
2. SFZ `pitch_keycenter` (VCSL/FreePats/jlearman) beats filename.
3. Human listening (the user) beats FFT for timbre/musicality.
4. Host tests never speak to quality.
5. Newer markdown beats older markdown — when in doubt, delete the stale claim rather than argue with it (update at the time of discovery, don't append corrections forever).
6. If two documents contradict and measurement can settle it, measure; if the user's ear can settle it, ask.
7. `fullpass-log.md` is authoritative for whether a machine is done (automated sign-off); `instrument-build-log.md` is the human-readable catalog/summary.

## 5. File map
- `new-engine-playbook.md` - build details (engine skeleton, knob design, worked harp example). Steps 5–6 here.
- `source-inventory.md` (workspace root) - measured catalog of all downloaded raw libraries for the full pass: formats, rates/bits/channels, single-note vs sweep classification, VCSL candidate→folder map, per-set build guidance; plus `sources/<lib>/<set>/README.md`.
- `../sources/` - raw downloaded libraries (read-only for builds).
- `instrument-build-log.md` - per-instrument facts and status (README table + source audit section).
- `issues-log.md` - defect catalogue & investigations with dates.
- `digital-sound-test.md` - the render/FFT harness contract.
- `README.md` - adaptive-build quick reference (planner, listen sets).
- `compact-prototypes/pitch_measure.py`, `pitch_hypotheses.py`, `sweep_segments.py`, `pitch_wavdir.py` - measurement tools.
- `compact-prototypes/sweep_verify.py` - batch-run `machine_verify.py` over every built machine and write the combined log (`compact-prototypes/verify_full.log`). Run this after ANY engine/data change.
- `compact-prototypes/render_all_midi60.py` - re-render all 69 machines' MIDI-60 C4 WAVs into `midi60-samples/` (accordion special-cased).
- `compact-prototypes/scan_reentrance2.py` - flag machines whose render re-enters > 12 dB after decaying below -50 dBFS (the tuba ghost-loop symptom; see issues-log session 7).
- `compact-prototypes/level_audit.py`, `clip_detail.py` - peak/RMS/clip audit of stored data vs piano reference.
- `compact-prototypes/pitch-meas-*.txt` - raw per-instrument measurement dumps (run these to re-audit after any source change).

## 6. Not proven yet (as of 2026-11-15, after full rebuild pass)
- `mda-piano` still the only ear-verified machine; nothing since Piano 1.0 has been ear-verified.
- A large cohort built this pass is *automated-verification-checked* (`machine_verify.py`) but NOT ear-confirmed.
- Narrow-register machines (crotales, bells, xylophone, flexatone, kalimba/mbira, agogo-bells, hand-bells-nepalese, slit-drum, bell-tree, balafon) score 0/6 on the piano-calibrated probes — that is spurious failed range, not defect. The engine's `note - root + 15` is also now clamped against OOB (fix landed in the 2026-11-15 builder); previously it crashed the renderer at very low out-of-range probes.
- `mda-violin` (arco) is rebuilt and verifies 5/6; MIDI 36 out of range for a G3-based instrument. `mda-viola` still octave-folds toward the band limit; `mda-cello` and `mda-viola` both have poor in-tune low probes — likely low-register reed-formant/weak-fundamental detection, not mapping defect.
- Engine `root` vs `source_note` issue is moot: every rebuild in this pass routes `root` to the file's *measured* sounding pitch (SFZ `pitch_keycenter` or YIN). New builds are correct; older ones (piano/harp) were not rebuilt for engine-consistency (piano is the bit-identical reference; do not touch).
- Every machine but `mda-piano` and the 2026-11-15 flagged cohort has user A/B sign-off (2026-11-15). Flagged machines: `listening-log.md`. **Do not ship the flagged machines without a fix.**
- The 43 machines rebuilt in the 2026-11-15 size campaign (27 >violin with caps, 16 ≤violin with tail-trim) re-passed `machine_verify.py` with no regressions, but none has an ear pass — especially the capped engines (4 s + loop note length, not full natural decay).
- Device (hardware) performance: never measured; all numbers are host.

## 7. Fresh-agent recipe
1. Read this file → `instrument-build-log.md` → `fullpass-log.md` → `issues-log.md`.
2. Audit sources before mapping (step 0). Keep `pitch-meas-<inst>.txt` per source in the repo.
3. Build one instrument, run steps 0a–8, mark it, then the next. Never batch without auditing between machines — the 2026-11-15 pass rebuilt the whole catalogue and cleaned up the defects afterwards; the cost of skipping the audit gate was 45 machines needing re-inspection. **After any rebuild of any machine, re-run `sweep_verify.py` (all 69) and re-render `midi60-samples/`** — the 2026-11-15 spawn-fix session proved a one-line engine change can move an octave-fold pitch by 12 semitones; release and listening material both go stale.
4. Write findings into `instrument-build-log.md` same day; keep `fullpass-log.md` current; let `issues-log.md` carry the defects found and fixed.
