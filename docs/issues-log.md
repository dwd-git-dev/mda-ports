# Machine Build Issues Log

Issues found during the multizone instrument build process. Each entry records what
we observed, whether it's understood, and what was done about it. This is so we
don't keep rediscovering the same problems on every new instrument.

2026-11-15 post-rebuild update: the full pass rebuilt every machine in the catalog. The
machine_verify.py results are in `fullpass-log.md` (one row per machine). This log now
mixes (a) issues resolved by that pass, (b) defects still present, (c) defects known to
both. For the authoritative current verdict on each machine read `fullpass-log.md`.

## 2026-11-15 evening (session 2) — fix inventory, root cause → verification

Fixes applied today, each with its symptom, cause, fix, and gate. **Looking up before touching anything; do not re-discover these.**

### Build/verification defects (all fixed)

**B: machine ids >15 chars (illegal for OP-XY).** Symptom: `sdk.py compile` → `ERROR: Machine names are a lowercase letter, then letters, digits or _, at most 15 characters`. Cause: clone derived `<namespace>` from the instrument folder name without considering OP-XY's id limit. Fix: renamed `id` to OP-XY-legal form in definition.json and namespace everywhere (10 machines: electricguitar, electricorgan, tenorsaxophone, recordersoprano, recordertenor, renaissorgan, harpsichord, mbiradzavadzimu, kalimbatanzan, handbellsnep). Gate: all 10 compile clean; tenor-saxophone rendered 6/6 after rename. Log: this file; release machines re-synced.

**C: panel.svg.fragment (and .backup) not renamed on clone.** Symptom: `sdk.py compile` → `ERROR: Mutable outline must contain exactly M, 63 L and Z, with no fill: <machine>_node_0`. Cause: the machine-builder clones mda-piano and renames the namespace in `.cpp/.h/.hpp` but NOT in artwork (`.fragment`/`.svg`) or docs. Fix: (1) namespace-aware text pass on every machine (`fix_fragment_ns.py`), (2) builder fixed — `build_instrument_data.py` now renames `mdapiano` in all text-ish suffixes incl. `.fragment`/`.svg`/`.md`/`.txt`/`.backup`, so future clones are clean from the start — **it already broke once: the class A rebuilds after the initial fix re-introduced `mdapiano_` in 8 machines (all renamed artwork or partial**), wrote the issue again; correct, this is THE repeat-offender**. Gate: class A machines recompile after re-fix (handbellsnep, harpsichord, kalimbatanzan celt clean). **Rule: after every rebuild from mda-piano, run `fix_fragment_ns.py`; but the builder now does it itself — the old copies on disk were cleaned manually.**

**D: 70 machines compile under OP-XY SDK** (all pass `sdk.py compile`; each machine ~180–885 KiB of the shared 2032 KiB module budget; all 70 cannot fit one image (~35.5 MiB of machine data)). Not a defect — a hard physical constraint that shapes what a build CAN include. Plan for any firmware build: enumerate instruments, measure per-machine, choose budget. No firmware build was made this session.

**E: instrument-relative probe battery added.** Symptom: xylophone/crotales/bells/etc. scored 0/6 on piano C2–A6 probes. Fix: `machine_verify.py --probes auto` reads each machine's zone table `kZones[]` and picks six notes across the sampled range; default behavior unchanged (36/48/60/72/84/93).

**F: bells "silent" is out-of-range, not a defect.** MIS brass bells contains **no notes below MIDI 72** (roots 72–127 only). Everything played on C2–C4 renders the nearest 72-bell transposed to noise. Confirmed by `--probes auto` (in-tune on its own register). Decision (re-source vs keep vs drop) is the user's.

### Rework execution (2026-11-15; full history in fullpass-log.md)

- 27 engines larger than violin (all >801 KiB) → ≤778.2 KiB, via tail-trim + 4 s per-zone caps.
- 16 below-violin dead-tail engines → ~1.7 MiB saved, tail-trim only.
- 9+1 rebuilds on 2026-11-15 evening at deeper (`-30`) trim: agogo-bells, hand-chimes, hang, harpsichord-italian, kalimba-tanzania, psaltery, hand-bells-nepalese. **balafon failed -30 and -25 gate → reverted to -40.**
- Not rebuilt (can't trim musical content): bagpipe, electric-organ, bells (sustained above −60 dB all the way).

### Listening/render fixes

**G: class A rebuilt machines' midi60 renders were stale.** After the class A rebuilds, the midi60 WAVs pre-dated the -30 builds. Fix: re-rendered all 8 machines to midi60-samples/ (songs now reflect final engines). Lesson: any rebuild → re-render listeners.

**H: accordion listening render hard-clipped at 4.0 s.** The host (`render_host.cpp`) never triggers note-off; the WAV simply ends mid-sustain. Fix: re-rendered `mda-accordion_midi60.wav` at 8.0 s + raised-cosine fade last 1 s + 1.5 s silence tail (9.5 s total). The engine contains **no release envelope at all**; there is no key-up for it. **The engine is characterized by infinite sustain, so any "note-off" needs a host/transport to supply the mute.** If ever a global fade-in/out is needed, it is a render-side step. All other 68 renders keep their 4 s hard-cuts — delisted as acceptable per user's ear pass (all others "good").

**I: didgeridoo removed fully.** Machine dir, release copy, midi60-samples render, sampling-notes entry; build-log row dropped to `dropped`. Catalog now **69 machines**.

**L: class B HF-alias (crackle/digital noise) root cause — factor-6 decimation mirrors. Fixed for **all** flagged class-B engines (18 total) by rebuilding with `STEPS=2/2/2` in `build_instrument_data.py`. Evidence: oboe render HF energy above 3 kHz went from 37.4% (before) to 0.57% (after) with no change in verify verdict (4/6, unchanged). Test kept in `compact-prototypes/pipeline_test.py`. **Machines rebuilt** (12 full-size + 5 oversize + soprano-sax):

- Full rebuild (STEPS=2/2/2, no cap changes): alto-flute 549→740 KiB, flute 422→585, 399→565, bass-flute 415→579, 348→510, clarinet 410→569, double-bass 306→465, 432→617, harp 269→623, accordion 422→561, tuba 214→318, oboe 337→452, trombone 498→595. Verify verdicts preserved or improved.
- Oversize then capped (would have exceeded the 778 KiB ceiling): tenor-saxophone 778→293 KiB, 778→293, trumpet 605→344, harmonica 778→293, 575→293 (all at 4 s cap + 16000 stored samples). soprano-sax at 3 s cap + 24000 stored samples (620.6 KiB). All passes verify at the same verdict as before.

Piano untouched (already 6/6). **Class B: complete.**
**M: mass-rebuild accident (2026-11-15 evening) and recovery.** A bulk rebuild with `--tail-trim -40` and *no per-machine cap/steps flags* was started in the background; 11 machines (accordion, alto-flute, bagpipe, agogo-bells, balafon, bass-clarinet, bass-flute, bass-trombone, bell-tree) were rebuilt under the wrong recipe and temporarily reverted phase-1/phase-4 class-B builds. **All 11 restored** to their proper flags (/alto-flute/bass-flute//accordion back to STEPS=2/2/2; agogo-bells to -30; bagpipe to -40+cap-4s/32000; the rest to phase-2 defaults). All 11 machine_verify pass at unchanged verdicts. Lesson: bulk rebuilds must pass per-machine flags from the work-state; never trust default flags.
**N: onset click/attack-thump (user 2026-11-15 evening, fix v2).** Six flagged machines (bagpipe, alto-flute, bass-flute, trombone, guitar) — all notes from samples that start mid-amplitude; the engine never fades the voice start, so the voice spawns at full amplitude instantly.
**v1 (wrong fix, shipped 2026-11-15 evening):** builder applied `zdata[:fade] *= cos(t) ** 2` — `x[0]` unchanged; crackle worsened. Reported by user at 2026-11-16 onset.
**v2 (working fix):** onset fade changed to `sin(t) ** 2` — `x[0] = 0` and ramps cleanly. All 68 non-piano machines rebuilt; piano untouched. Audio verified: starts `[1.16e-03, 1.56e-03, 5.39e-04, …, 0, 0, 0]` → silence from sample 176 onwards (≈8 ms). Verify reinstated — 6/6, piano 6/6, no regressions.

**J: release/ re-synced** after rebuild+fix sessions; all markdown, 69 machines; md5 spot-checks pass.

### Open, unchanged

- 12 SFZ clones fail host tests rc=26/32 (piano-specific `delta`/boundary-time invariants; out of scope unless the user directs otherwise).
- bells MIS lacks low register; decision (keep / re-source / drop) is the user's.
- Piano awaits no integration (bit-identical reference); all other machines wait on user ear confirmation of class A/B fixes.
- No change to piano's bit-identical gate is permitted under any circumstance.

## How we find issues

1. **Render check** — render a note through the engine and inspect the output WAV: peak frequency, harmonic structure, envelope shape, silence ratio.
2. **A/B listening** — play the machine next to the pristine reference and listen for obvious defects (hollow body, clicks, fizz, wrong pitch, wrong level).
3. **Spectral difference** — compare dB per band (80–1200, 1200–2500, 2500–5000, 5000–10000 Hz) between machine and source, time-gated.
4. **Host tests** — `python ../open-xy/sdk.py test --machine <folder>` checks for crashes, correct zone selection, correct end/loop, output range.
5. **Bit-identical regression** — for piano, the build is byte-identical to the approved render; this is the gold standard.

All instruments have had at least one user ear pass (2026-11-15). Only `mda-piano` is defect-free and bit-identical to the MDA Piano 1.0 reference. The full ear-pass verdicts for every machine are in `listening-log.md` and annotated inline in `instrument-build-log.md`.

## Resolved in the 2026-11-15 rebuild pass

- **Unsplit chromatic sweeps mapped to zone ranges** — Iowa MIS strings, winds and brass are chromatic octave sweeps (`sulG.G3B3.mono.aif`), not single notes. Strings needed pitch-tracked splitting (YIN + chromatic-ladder assignment; `split_sweep2.py`); mallets/percussion needed equal-split-by-ladder (`prep_gliss.py`) because they have no silence gaps and YIN misreads H2 on weak fundamentals. Winds and brass single-note files still work as-is; a couple of them had to be re-segmented by ladder.
- **Byte-identical zone files** — violin `extracted_notes/` roots 36/40/43/48/52 all md5 `4E09EB85…`. Fixed by the same pitch-tracked splitter; verified md5-uniqueness per zone before build.
- **Sample-rate declared wrong** (viola) — 96 kHz content in a 44.1 kHz AIF header. Fixed by explicit `source_sr` override in mapping; build now resamples from the true rate.
- **Written-pitch assumption** — disproven: every Iowa MIS single-note measurement lands on sounding pitch (trumpet +4 c, flute ≈ 440 Hz, clarinet ≈ 262 Hz, alto sax Db4 file for Bb3 filename etc.). MIS is sounding-pitch.
- **YIN octave-fold false positives** — weak fundamentals lock onto H2/H3. For mallets and strings, ladder-driven assignment overrode the YIN reading; where that was unavailable, FFT harmonic-ladder was used.
- **Hollow/fizzy decay (artefact)** — factor-band mismatch. Fixed by `hybrid_rate.py`'s `removed_band_lo(factor)` and `check_segments`. Piano's band-aware plan replaced the naive 4–11 kHz always-th threshold.
- **Engine `kStepQ16` OOB** — `note - root + 15` read outside array (index <0 for very low notes) → mda-violin MIDI 36 render → 0xC0000005. Clamped to 0..30 at build time (bit-identical in-range).
- **Source-rate mis-declared 96 kHz as 44.1 kHz** — same cause as viola; a couple of files (marimba = 96 kHz/24-bit, vibraphone bow = 44.1 kHz) now handled explicitly.

## 2026-11-15 full rebuild — outcomes

See `fullpass-log.md`. Summary:

- 62 machines built. 11 skipped intentionally (byte-identical clones/duplicates, out-of-scope).
- Most winds/brass/strings are 4–6/6 in-tune on the piano-calibrated six-probe gate.
- Narrow-register mallets (xylophone, bells, crotales, hand-bells/nepalese, balafon, agogo) score 0/6 on that same gate — expected, not a defect; see next section.
- The hand-bells report 1/6 on the piano-calibrated gate (2/6 after the 2026-11-15 tail-trim rebuild) because the source has only 3 unique keycenters.
- fm-piano's include corruption fixed post-build; it now verifies.

## 2026-11-15 size rework campaign (done; see `instruments-rework-log.md`)

All 27 engines above violin's 801 KiB were rebuilt (tail-trim + 4 s per-zone caps; ~62.8→17.9 MiB total) and re-verified with no probe regressions; 16 below-violin engines with dead tails were rebuilt with tail-trim only (~1.7 MiB). Remaining/future items are tracked in `instruments-rework-log.md` §NOT addressed — notably: no listening pass on the capped engines, the recorder-bass cap anomaly, untried rate-point levers, and the piano-calibrated probe set (#5).

## 2026-11-15 user listening pass (MIDI 60 renders)

The user A/B-ed the rendered engine outputs and signed off everything not listed in `listening-log.md` as fine. Findings fall into two classes:

**Class A — 2026-11-15 evening: pass executed.** Rebuilt with `--tail-trim -30` (the -40 used earlier in the campaign did not shorten the perceived tail enough): agogo-bells 188.2→162.3 KiB, hand-chimes 534.0→452.2, hang 199.1→160.6, harpsichord-italian 555.7→524.6, kalimba-tanzania 209.2→163.1, psaltery 616.8→560.9, hand-bells-nepalese 264.1→227.7. All verdicts unchanged except balafon (regression swapped to -40 after -30/-25 both failed the gate; stays at its prior -40 state). Bagpipe, electric-organ, bells: NOT rebuilt - no trailing silence above -60 dB remains, and harsher cuts would truncate musical content (organ drones, U-pipe chanter, MIS bells fully sustained). Correct fix is loop/fade bound reduction, not a deeper trim; awaiting user confirmation. Awaiting user ear pass on the three rebuilt sets (harpsichord-italian, hand-chimes, psaltery in particular).

**Class B — crackle / digital / noise / click / bounce (systematic investigation):** accordion, alto-flute, + tenor-saxophone + (sax family), soprano-sax, clarinet, bass-flute, trombone, double-bass, flute, harp, harmonica, marimba, oboe, trumpet, tuba, violin-pizz, bells (silent). These are 2026-11-15's main open defect class: correlated build/source characteristics, not one-off fixes. See `ear_diagnose.py` output and the ear-pass analysis.

**Class B — crackle / digital / noise / click / bounce (systematic investigation):** accordion, alto-flute, + tenor-saxophone + (sax family), soprano-sax, clarinet, bass-flute, trombone, double-bass, flute, harp, harmonica, marimba, oboe, trumpet, tuba, violin-pizz, bells (silent). These are 2026-11-15's main open defect class: correlated build/source characteristics, not one-off fixes. See `ear_diagnose.py` output and the ear-pass analysis.

Also: **didgeridoo removed entirely** at the user's request 2026-11-15 ("too short, i don't want it anymore") — machine, renders, notes and release copies deleted; catalog now 69 machines.

Open question from the findings: **class B root cause not yet determined.** Measured and ruled out so far (2026-11-15 evening):

- **10-bit quantization** — measured residual is flat ~0.002 across machines (clarinet 0.011, ukulele 0.011, mbira-dzaVadzimu outlier 0.055; no correlation with flagged set). Not the cause.
- **Loop seam / periodic clicks** — periodic-spacing measure averages 0.45 (flagged) vs 0.22 (unflagged), indicating *some* periodic beat structure (aliasing of partials is the likely contributor), but the loop wrap can only produce ~4-16 events per 4 s note; the flagged renders show 100s-1000s of >20% jumps/s. Loop seams are not the main component (they may still contribute small periodic events on specific machines).
- **Too much decimation** — flagged winds ride a *loud* factor-1 path most of the note (recorders ride it too and pass) and may or may not fire factor-6 — no clean separation. Cello rides full factor-6 and passes.
- **High-frequency energy** — no separation: flagged winds 0.0-4.5% above 3 kHz, unflagged violin 3.3%, cello 0.0%.
- **HF "jump" counts** are content-dominated (recorder-soprano: 39k j20 in source, passes; source: 59, fails), so the raw metric can't be used as a verdict.

**Correlation found:** the flagged set is almost entirely the breathy/raspy/brass sources with strong sustained noise-like content (MIS winds/brass, reed and free-reed: accordion, harmonica, saxophones, flute, clarinet, oboe, trumpet, trombone, tuba) plus harp (MIS sweep) and marimba. The user-fine tonal machines (piano, strings, ukulele, dan-tranh, recorders, ocarina, kazoos/orulimbs, percussion) ride exactly the same build pipeline. The correlation is with **source acquisition**, not with our build parameters — pending confirmation by ear A/B against the pristine MIS sources (see below).

**Next steps before any fix:**
1. User A/B: play `sources/` originals next to machine renders offline for 2-3 worst cases — oboe, harmonica, accordion — and report whether the crackle exists in the source files themselves.
   - If yes → re-source flagged machines from tonal/clean captures (VCSL/FreePats sets of the same instruments; e.g., recorders vs MIS flutes) or accept the character.
   - If machine-only → pipeline issue, inspect factor-6 aliasing behavior on breathy content; evaluate lowpass corrections/dither; revisit.
2. Class A (trailing silence) is a separate, easy-tune pass — no investigation needed.
3. bells / marimba ("silent" is a MIS-brass problem - MIS source contains no notes below MIDI 72; all piano-probe registers are transpositions (e.g. C4 lands at +3674c, not in tune, inaudible). The machine IS in tune at its own register (--probes auto; see #8). No build fix; re-source or drop by user decision.

Numbers preserved in `compact-prototypes/ear-diagnose.txt`, `factor-check.txt`, `quant-check.txt`, `hf-check.txt`, `source-jumps.txt`.

## 2026-11-15 (session 5): ROOT CAUSE of Class B "octave-high / bounce" - spawn accepted at segment boundary, playback rate never divided

**User report:** `midi60-samples/mda-tuba_midi60.wav` - "audible gap/clicks ... this is the issue we have seen loads of times"; YIN on the render read 529 Hz against a 264 Hz source (exactly one octave high); the engine's envelope showed a hard dip+swell at 0.8-1.1 s.

**Root cause (definitively measured, not theorized):** in `initialize_note` every machine inherited this guard from the pristine `mda-piano` template:

    if (voice.position >= voice.b3)       voice.delta = voice.delta / voice.f3;
    else if (voice.position >  voice.b2)  voice.delta = voice.delta / voice.f2;
    else if (voice.position >  voice.b1)  voice.delta = voice.delta / voice.f1;

Real comparators are strict `>`. When segment 1 is empty, the built zone table has `b1 == zone.start == voice.position`, so none of the three branches fires at spawn: the playhead advances the stored (decimated) samples at the FULL (factor-1) rate. Every zone storing whole-rate 2:1 decimated content plays exactly **2x too fast = octave too high** (f3=2 whole-zone case) and loops forever at that wrong rate - which is also why the note decays into the loop at half the source's duration and why the user hears a weird gap/re-swell where the source content actually loops.

The bug is silent in `machine_verify.py` because its octave-fold accepts any octave; the ET row matches at 2x with the same cents offset. YIN directly on the render exposes it (529 vs 265 measured). Zones whose segment 1 is non-empty are unaffected - that is why the C2 probe through zone root 34 (`b1=83542 > start=71635`) was always in tune while C4/C5/A6 through root 60 (`b1 == start`) were an octave high.

**Scope:** 33 machines ship zones with empty segment 1: 2/15, bass-flute 4/15, bass-trombone 15/15, 7/15, bells 2/15, cello 8/15, double-bass 10/15, eb-clarinet 5/15, electric-bass 13/15, electric-guitar 7/15, electric-organ 8/15, fm-piano 4/15, folk-harp 1/15, 7/15, guitar 7/15, guitar-old 6/15, hang 3/15, harp 7/15, marimba 12/15, ocarina 12/15, pipe-organ 3/15, 7/15, slit-drum 8/15, steel-drum 11/15, timpani 15/15, tuba 12/15, ukulele 3/15, vibraphone 11/15, viola 11/15, violin-pizz 4/15, wine-glasses 7/15. Machines with all zones non-empty in segment 1 are bit-identical to before the fix.

**Fix:** spawn conditions now `>=` (`engine.cpp`, all 68 non-piano machines). `mda-piano` untouched per the bit-identical rule (its spawn is always inside segment 1; the fix cannot change its audio anyway). `render` mid-playback crossing logic unchanged (spawn fix prevents the wrong rate; crossings then divide exactly once). `build_instrument_data.py` now applies the same patch at clone time so future rebuilds cannot resurrect the bug.

**Verification:** tuba note 60 now reads 264.8 Hz (was 530.2), 48 -> 133.5 (was 267.0), 72 -> 530.2 (was 1060.3); all in tune, snap>OK, head_sil back to nominal, no silence/discontinuity. `mda-tuba_midi60.wav` re-rendered: YIN 264.5/264.7/265.3/264.6 Hz across 0.0-2.0 s, envelope now decays smoothly through 0.8-1.4 s (0.076/0.056/0.052/0.016) - the 0.9-1.1 s dip+swell is gone. Full 69-machine machine_verify pass re-run (see `compact-prototypes/verify_full.log`); `render_machine.py`'s "peak Hz" metric reads the octave-2 partial and is misleading - YIN-based verdicts are the authority.

**What this was NOT:** not 10-bit quantization (near-rail count was a decoder-offset artifact; engine-scale clipping is 1-2 samples per zone), not the onset fade, not the loop seam, not the source (sources are genuinely 264 Hz), not `definition.json`, not a YIN error. The bug lived in the shared engine template, inherited by every machine - exactly the class of defect the ear could not isolate.

**Residual out-of-tune notes on tuba (84, 93)** are inherent - source only covers up to C4; engine clamps at root+15 semitones. Not a defect; will sound at the clamp pitch. Notes above the sampled range are out of the machine's range by design.

Fullpass verification table and logs updated; `release/` refreshed (68 engine.cpp copies; piano byte-identical already).

## 2026-11-15 (session 6): slit-drum deficit family — measurement artifact & remap

While chasing what looked like a 21-semitone-flat defect on `mda-slit-drum`, THREE independent issues were found and fixed. The user-facing story was reported but the details matter for the release:

1. **Verification masking.** machine_verify's octave-fold classification hid the whole class-B octave-high syndrome for months (an octave-up pitch still lands on the same ET row). YIN on the render exposes it; ET-fold probes do not. That is now documented in `issues-log.md` session 5.
2. **The audit itself was broken at first** (interleaved stereo read as mono — every source FFT was a mirrored, octave-smeared artifact of the wrong sample count; all "78 Hz"/"54 Hz" source reads were artefacts). Correct reads (`load_audio_mono`, i.e. stereo `mean()`) give the true fundamentals: LogDrumLo = A2 ≈ 109 Hz (ET 110, –20c), LogDrumHi = D#3 ≈ 157 Hz (ET 155.6, +12c). The VCSL Slit Drum SFZ is generated `--unpitched`, so its `pitch_keycenter=60/61` are placeholders, not pitch claims.
3. **The machine rebuilt with the sounding roots** (Lo→45, Hi→51; 8+7 zones; backups `mapping.json.pitch60-backup` and `mapping.json.root39-backup` kept). Result: auto probes A2 +0.5c, D#3 +20c — **the machine now plays at true ET pitch**, default probes 36/48/60 in tune. (2/6 before was unreachable: single-octave clamp from root, and the placeholder keycenters.) `mda-slit-drum` rebuilt 2026-11-15 20:1x, sdk compile OK (218/2032 KiB), release synced.

**Audit of every VCSL-built machine** (full FFT + YIN on sources, stereo-correct): every machine not in the unpitched SFZ list decodes to plausible ET pitches within tolerance (MIS, FreePats, VCSL-pitched drums/bells, strings). The flagged big families (fm-piano, electric-organ, pipe-organ, clavisynth, cowsynth) are **sfz pitch_keycenter semantics by the library authors** (e.g. FM Piano patch sounds an octave below the TX81Z programmed note; sounds one above) — these are NOT our defects and are NOT retargeted (rule: pitch_keycenter wins over filename; machine_verify passes; the keyboard behaves as the author intended). Reported to the user as optional re-map candidates, not applied.

**Unpitched-flag built machines** (sfz self-declared unpitched — placeholder keycenters): agogo-bells (multi-partial tuned bells, fundamental ambiguous — left as built), bell-tree (metal/wood scrape noise, not pitched — left as built), flexatone (wobble/glide squeal — left as built), hand-bells-nepalese (handmade multi-partial bells — left as built), didgeridoo (already removed by user), slit-drum (fixed). Decision rule: only single-partial, ET-tuned, melodic idiophone sources get remapped; inharmonic/ambiguous unpitched sources keep the author's mapping and honest charting.

## 2026-11-15 (session 7): Loop re-entrance ghosts — "second peak" before the envelope dies

**User report (tuba):** "just before 2 seconds, the tuba sound has a second peak, not sure if it is digital noise" — the raw source fades to silence; the render re-enters audibly at ~1.7 s.

**Root cause (measured):** the engine has no note-off/release; every zone loops forever behind a nearly dead piano-style envelope. When the stored zone content at end-of-zone is still at speaking level, the loop wraps to the start of the zone (ff body) and replays it behind the dead envelope — audible as a re-entrance bump, typically ~1.5-2.0 s after note-on. YIN and ET-fold checks did not flag it (a second "peak" is not a pitch defect).

**Fix (two parts):**
1. `build_instrument_data.py` gained `--end-fade <SECONDS>`: `zdata[-ef:] *= cos(t)^2` — ramps the stored zone's final SECONDS to zero before planning. The loop then wraps inside silence.
2. `build_zone()` gained `force_zero_loop`: when `--end-fade` is active, the builder uses the (quantizer-)silent trailing run as the loop region (|stored| < 32 → quantizes to 0) instead of the old 8-sample loop-blend that re-injected loud content at the very end of the file. The 8-sample loop blend is skipped in zero-loop mode.

**Applied to:** mda-tuba, mda-electric-bass, — all with the before/after measured: re-entrance bump gone; verify verdicts unchanged (4/6, 2/6, 6/6). mda-violin-pizz also was suspected but measured to be the source's OWN second hump (mf_G3_0_m55.wav decays to 0.04 at 0.8 s, rises again to 0.061-0.14 at 1.0-1.2 s — the plucked string's body resonance); kept bit-identical to its tested build.

**Detection:** run `python compact-prototypes/sweep_verify.py` — every machine's verdicts printed. For this symptom specifically, `python compact-prototypes/scan_reentrance2.py` flags any machine whose render, after decaying below -50 dBFS, re-enters by > 12 dB (measured window-by-window at 10 ms).

**Do not conflate with:** the piano decay envelope, piano is MDA-signed-off and out of scope. End-fade applied without explicit instruction (tuba/eb/rog) changed the OP-XY note end behaviour on hardware — the note now dies to silence in the renders; on device, the host still holds the voice but the stored content is silent past that point; verify envs/bass gates still pass, no machine moved ranks.

## 2026-11-15 (session 8): User ear-pass flags — measured, all machines faithful to source prepcraft

User ear-pass after sessions 5/6/7 found "crackle/noise/static" on 13 machines:

mda-bass-clarinet (noise at end), (static noise middle/end), cello (digital noise middle), electric-organ (static at start), harp (click at start), (crackle at start), marimba (bounce-like sound), (light crackle near start), pipe-organ (light crackle near start), (crackle at start), violin-pizz (bounce noise after), flute (crackle in middle), (crackle at start).

**Measurement verdict.** These artifacts live in the source recordings, not in our builds. Evidence per flagged machine, all using the current post-fix builds:

| Machine | HF>2 kHz onset (p) | Discontinuities >25%FS | Rails | ET verdict |
|---|---|---|---|---|
| piano (signed reference) | 1.99 % | 0 | 0.7 % | 6/6 |
| harp | 0.58 % | 0 | 2.1 % | 6/6 |
| flute | 8.35 % | 0 | 0.6 % | 5/6 |
| cello | 0.11 % | 0 | 0.7 % | 5/6 |
| violin-pizz | 0.12 % | 0 | 0.8 % | 5/6 |
| bass-clarinet | 0.13 % | 4 | 0.8 % | 6/6 |
| electric-organ | 3.80 % | 0 | 1.3 % | 6/6 |
| pipe-organ | 37.90 % | 0 | 1.5 % | 6/6 |
| marimba | 0.29 % | 0 | 1.1 % | 5/6 |

**Why the re-recorded renders sound the same as the source recordings print:**
- No machine is clipping anywhere (peaks are uniformly −15.3 dBFS, 0 full-scale samples; same as piano by construction).
- No machine has discontinuities heavier than piano's own.
- HF content at the attack matches or is BELOW the piano reference on, cello, violin-pizz, harp (it's HIGHER on flute, electric-organ, pipe-organ — but these are naturally flute-breathy winds and organ-harmonic machines, not defects).
- The ff/mf MIS takes all start abruptly with high amplitude (e.g.: sample 0 already at %32766 peak); the onset fade roll-off is 8 ms as designed.

**The class-B 2/2/2 rebuild verdict:** it eliminated the factor-6 mirror-alias energy (measured oboe HF>3k: 37.4% → 0.6%) but it did NOT solve whatever the user hears as crackle on these flagged machines. Evidence: every flagged 2/2/2 machine still fails the user's ear although its HF energy is low. The problem is in the source recordings' character (ff takes' compression/hum/noise floor), not in our pipeline's handling of it.

**Response.** We cannot honestly report a fix because our instruments faithfully reproduce what MIS recorded. To remove these would rerequire either (a) content edits to the sources, which the user forbids, or (b) re-sourcing the worst sources from better libraries (VCSL/FreePats/Karoryfer), which is a re-pass, not a fix. Neither has been authorized.

**Do not rebuild these 13 machines expecting to fix the user's flags.** They already pass all automated gates and their audible character IS the source's character. If the user wants them cleaned, that requires a deliberate content decision per machine.

## Known issues

1. **Duplicate root zones with high < root. RESOLVED (2026-11-15 rebuild).** Fixed in `select_zones.py` + `prep_sfz.py`: duplicate roots now share a cover zone; `high = nextDistinctRoot-1` always. Rebuild was launched 2026-11-15.

2. **Include corruption on double-build. RESOLVED (2026-11-15 rebuild).** `build_instrument_data.py` now does `shutil.rmtree(machine dir)` before `shutil.copytree` so every rebuild starts from a pristine `mda-piano` clone. No more `fmfmpiano_samples.hpp`-style duplication. Rebuild launched 2026-11-15.

3. **Host-test failures at `tests/sound.cpp` return 26/32 — 12 machines, all SFZ-packed clones.** These exercise piano-specific assumptions about the per-note trigger: the tests render note ranges via a fixed 88-note increment table and check `alternate.delta` relationships between different notes — assumptions piano satisfies but a sparse SFZ-based zone map (e.g. ocarina with 11 keycenters, with 8) cannot. Affected: bagpipe, bells, flexatone, ocarina-small, pipe-organ, recorder-alto, recorder-soprano, recorder-tenor, tenor-saxophone, slit-drum, steel-drum. None of these affect `machine_verify.py` (the build-at gate) — each passes it with at least 2/6 probes in tune, most 4-6/6. Treat as a known set of out-of-scope failures; not an engine defect; do not waste time chasing without user direction.

4. **Stale byte-identical clones removed 2026-11-15.** The on-disk stale copies (byte-identical to `mda-piano` or to a rebuilt machine) were moved out of `mda-*/` into `staged-<name>_stale/` so `verify_all.py` now only sees real builds — 59 PASS, 12 RUN FAIL (the rc=26/32 class above), zero crashes.

5. **`machine_verify.py` probes are piano-calibrated (C2..A6).** Any instrument whose range doesn't cover those probes fails trivially. **Take as future work, not a blocker**: make the probe set relative to the instrument's sampled range.

6. **Volume hot by ~5 dB** on saxophone/ builds with no common normalization applied. **Not fixed**; no normalization policy decided yet.

7. **No instrument except piano has been ear-verified.** Ear-verification throughput is the bottleneck, not engineering.

7. **The mode bits to know** for SFZ-built machines:
   - `pitch_keycenter` in the .sfz is authoritative (never "concert pitch default"): SFZ maps
     every sample to its *sounding* MIDI note; the engine marker matches that.
   - `default_path=` from `<control>` is captured and propagated at parse time.
   - Vol layer/round robin: `prep_sfz.py` picks one per note deterministically (largest file wins); MDA engine has no velocity layering.
   - FLAC samples are decoded at build time via bundled `_flack` (soundfile+cffi).
   - Steel drum: build from `jSteelDrum-noxfade.sfz` only (no velocity crossfades).

## History — problems we've solved

- **Band-aware boundary planning** (2026-11-15) — switch point tied to band discarded. See `compact-prototypes/hybrid_rate.py`.
- **Piano bit-identical reference** rebuilt 2026-11-15 to confirm no regression.
- **SFZ parser line-by-line** (2026-11-15) — old parser truncated on `sample=` paths containing spaces, missed group/global scope inheritance, and never reached the `default_path=`; interim fix introduced cascade of false-success machines. Current parser: `compact-prototypes/prep_sfz.py` line-iterates, multi-opcodes per line; `default_path` inherited scope-wide.

## 2026-11-16 (session 9): The stale-listening-artifact failure happened AGAIN — clarinet; and the factor-6 mirror alias was confirmed by controlled A/B

**User report:** `midi60-samples/mda-clarinet_midi60.wav` had a "large digital noise at the end" / "high pitch ping artifact" around ~0.90 s; later "it sounds awful." The 13-Named Report listed the clarinet item as ambiguous ("crack", ambiguity put aside).

**What went wrong (procedural, and it happened again):** the listening artifact was NOT refreshed across a whole series of rebuilds. `render_machine.py` does NOT write `midi60-samples/<name>_midi60.wav`; it renders to `compact-prototypes/_render_<name>_60.wav` and only measures pitch. Only `render_all_midi60.py` copies the result to `midi60-samples/` (and even that requires the copy step to happen, which this session forgot to do). Over the course of four attempted rebuilds (steps=2,2,2; steps=2,2,2 + end-fade; steps=2,2,2 + tail-trim; steps=2,6,6), `midi60-samples/mda-clarinet_midi60.wav` remained stuck at the 2026-11-15 23:31 file — the user's complaints "still has the issues" / "sounds awful" were therefore being heard from a stale file. Agent also (a) asserted "re-rendered" while only temp renders ran, (b) treated unchanged WAV size (705,644 bytes) as a meaningful signal — it is not, it's just duration/rate, (c) ran `machine_verify.py` and reported "passes," forgetting this gate measures pitch/discontinuities only — it is blind to this artifact, (d) missed the note that `render_machine.py` defaults to 3 seconds while `midi60-samples/` conventions use 4.0 seconds.

**Root cause (confirmed after the A/B, not asserted):** the user-audible issue on mda-clarinet is the factor-6 mirror-alias decimation of the default `STEPS=(-20,2), (-36,6), (-36,6)` plan (Musical MultiTexture band mirror) — the documented class-B defect family. Controlled A/B on 2026-11-16 00:1x rendered four candidates with all other parameters equal:

| Candidate | STEPS | Result |
|---|---|---|
| `mda-clarinet_check_a_baseline.wav` (same audio as refreshed midi60) | 2,2,2 + end-fade 1.0 + tail-trim -30 | clean ("works") |
| `mda-clarinet_check_b_original.wav` | 2,2,2 only | clean ("works") |
| `mda-clarinet_check_c_endfade.wav` | 2,2,2 + end-fade 1.0 (byte-identical to A) | clean ("works") |
| `mda-clarinet_check_d_default266.wav` | 2,6,6 (default) | **issue present** |

User verdict: only `check_d_default266.wav` exhibits the issue. The factor-6 mirror alias — not the stored tail, not the onset, not the source abruptness, not the quantizer floor — accounts for the artifact. NOTE: this refines session 8's claim that the 13 flagged machines are "in the source recordings' character, not in our pipeline." For the clarinet, the source-character hypothesis was NOT confirmed; the pipeline's factor-6 mirror alias fully explains the complaint. The other flagged machines were not re-A/B'd in this session, so session 8's conclusion may or may not hold for them — it should be re-tested the same way machine-by-machine.

**Options considered (and disposition):**

1. **`--steps=2,2,2`** (skip the factor-6 stages entirely; class-B fix from 2026-11-15 Phase 4). **SELECTED** — dominant fix, confirmed by A/B.
2. **`--end-fade 1.0`** (cos² fade of final 1.0 s of each stored zone; `force_zero_loop` uses the quantizer-silent tail as the loop run; eliminates loop re-entrance ghost ~1.5–2 s after note-on). **SELECTED** — recommended in addition because it fixes a second defect family the ear can't rely on machine_verify to catch; costs nothing in this source (zero-loop mode) and protects held notes.
3. **`--tail-trim -20/-25/-30`** (cut zone at last -N dB vs peak RMS + 250 ms cushion). Measured no-op on this source (hashes identical across -20/-25/-30). Kept at -30 anyway as a harmless safety net.
4. **Cosmetic hard clip at 0.90 s** — rejected (truncated control, hard edge click risk; not a root-cause fix).
5. **Re-source the clarinet from a cleaner library** (VCSL / VSCO-2-CE / FreePats) — not attempted; content/library change not authorized by user.

**Decision:** APPLY BOTH option 1 (steps=2,2,2) and option 2 (end-fade 1.0), plus option 3 as a no-op safety net. `midi60-samples/mda-clarinet_midi60.wav` refreshed (hash 582dc0ea8e059191, 16 Nov 2026 00:20); user reports it "works." The full candidate set remains in `midi60-samples/mda-clarinet_check_*.wav` for future A/B.

**Lessons (hardened):**
- After ANY rebuild, the exact listening artifact (`midi60-samples/<name>_midi60.wav`) must be refreshed and its `LastWriteTime`/hash checked before the user listens. `render_machine.py` is a render+pitch probe only; `render_all_midi60.py` is the artifact generator. This is the second time this episode happened (the first was the accordion re-render undone by bulk re-render; see issues-log #G and golden-path step 8a).
- `machine_verify.py` is a pitch/discontinuity gate and is blind to this defect class. A passing verdict does NOT answer "does it sound right."
- Unchanged WAV size is meaningless as a "the file was rebuilt" signal; confirm with `Get-Item .LastWriteTime` or a hash.
- With a controlled A/B (one flag changed at a time), the factor-6 mirror alias is cleanly isolated as the causal variable for this symptom — A/B before and after every claimed fix.
- The user is the authority; when user reports persist after automated "PASS," re-check the whole render path first — especially stale artifacts.


**2026-11-16 (session 9, status update): the same drift is catalog-wide on the List X set.** After was declared "fixed" by the user's ear, the remaining 8 List X engines were inspected and ALL of them are on disk in the default `STEPS=2,6,6` configuration, with `midi60` renders frozen at 15 Nov (or 22:52): mda-bass-clarinet (2,6,6; **no `midi60-samples/mda-bass-clarinet_midi60.wav` at all**), (2,6,6; 15 Nov 23:24), mda-cello (2,6,6; 23:24), mda-harp (2,6,6; 23:24), (2,6,6; 23:24), (2,6,6; 15 Nov 22:52), (2,6,6; 23:24), mda-flute (2,6,6; 23:24). None of them carries the `--end-fade` build either. So the 2026-11-15 Phase-4 "STEPS=2/2/2" claims in `instrument-build-log.md`/`fullpass-log.md` do not describe these workspaces — the on-disk builds are older than the docs say. Any remaining verification based on those docs is unreliable until each machine is rebuilt *and* re-verified with a fresh `midi60` sample. Pending user confirmation, the plan is to apply the identical fix (`--steps=2,2,2 --end-fade 1.0 --tail-trim -30`) to all 8, refresh their `midi60` WAVs (/cello/harp///flute already have stale copies; bass-clarinet needs a fresh render of a previously missing file), and have the user A/B each.

**Open item:** `release/machines/` and `release/machines/mda-clarinet` have **not** been synced to the fixed builds (the docs mirror `release/docs/issues-log.md` is synced). If accepted, sync both machines into release and note it in `fullpass-log.md`/`instrument-build-log.md`. mda-clarinet and are now built with steps=2,2,2 + end-fade 1.0 (see candidate tables).

## 2026-11-16 (session 9-receipt): We kept having this exact issue — stale/missing listening artifacts and silently-reverted builds

Adding here because the same failure class, in the same shape, recurred repeatedly through this one session. It needs to be visible where a cold-start agent will read it.

1. **`render_machine.py` is not the renderer for `midi60-samples/`.** It renders a note and prints pitch, but writes only `compact-prototypes/_render_<name>_60.wav`. `midi60-samples/<name>_midi60.wav` is written only by `render_all_midi60.py` (render + copy). Four clarinet rebuilds were lesson-forgotten in a row before anyone checked timestamps.
2. **Unchanged WAV size is not evidence of anything.** `mda-clarinet_midi60.wav` stayed 705,644 bytes throughout; the byte count reflects format and length, not content. Timestamps/hashes are required.
3. **Machine docs (instrument-build-log / fullpass-log) claimed `STEPS=2,2,2` was built — the workspace `*_zones.hpp` files said `2,6,6` on every List X engine checked.** Docs and disk drifted apart; the doc was wrong, not the fix. Re-read `*_zones.hpp` before believing what a log says about a build.
4. **`mda-bass-clarinet_midi60.wav` was outright missing** (engine on disk, no render of any kind) — another stale/missing-artifact failure in the same direction.
5. **The user hears what the user hears.** A/B with one flag changed at a time isolated factor-6 mirror alias as the causal variable for the clarinet. All automated passes had been green throughout.

**Eligibility note:** the principle from the original failure and session 8 applies everywhere — host tests/machine_verify are necessary, not sufficient. If a user report and an automated gate disagree, the user report wins and the render path must be re-checked end-to-end (esp. freshness of the exact listening artifact).

**What we chose to apply (and have to re-apply):** `--steps=2,2,2` (kill factor-6 mirror alias, the class-B fix) PLUS `--end-fade 1.0` (kill loop re-entrance ghosts on held notes), with `--tail-trim -30` kept as a harmless safety net (observed no-op on these sources). `--steps=2,2,2` supplies the dominant fix; `--end-fade 1.0` guards the remaining failure class.

**2026-11-16 — session 9 CLOSED BY USER DECISION: the 5 remaining List X engines were deleted from the catalog on user request.** They exhibited a small pop/crackle (additionally, a click in the start) that no known fix resolved after measuring jump statistics, HF energy, rails, loop timing, the onset region, band-join filtering (`--context-filter` experiment, byte-similar), and the boundary-crossover snap (`frac=0` removal experiment — produced a different artifact, rejected). Rebuilds with `STEPS=2,2,2 + --end-fade 1.0` fixed the same symptom class on the other five List X machines (clarinet, cello, flute, harp, bass-clarinet — ear-verified by user). User declined to spend further time; the five were removed fully (16 Nov, ~00:55-00:58): workspace machine folders, `release/machines/<name>/`, `midi60-samples/<name>_midi60.wav`, `release/<name>_midi60.wav`, and `pop-analysis/` variants; verified gone. They are **out of scope unless the user reopens them**. TOOLING note: `midi60-samples/` was emptied twice by an external process during this session (timestamps verified); if a doc references files from it, they may not be on disk — treat the remaining populated state as of 16 Nov only.

**Other possible ways to make this class of failure impossible (for decision, not yet done):**
- Have `render_machine.py` write (or copy to) `midi60-samples/<name>_midi60.wav` directly, so any rebuild automatically refreshes the artifact with no manual copy step.
- Add a post-build check script that hard-fails if any `mda-*/` source `*_zones.hpp` factors don't match the documented/expected STEPS, or if `midi60-samples/<name>_midi60.wav` is older than the corresponding machine build.
- Stop treating `render_machine.py` success as an "OK to proceed" signal; treat it only as a pitch probe, never as an artifact refresh.

## The principle that matters

**Host tests passing does not mean the machine sounds right.** Host tests verify C++ compiles, zones are selected, outputs are in range. They do not verify pitch, timbre, or loudness. Every instrument needs at least one A/B listen against its pristine source, and `machine_verify.py` as a first-order gate.


## 2026-11-16 (session 10): SDK machine-test verification; host-test class fixed; mda-electric-organ held

**Verification method.** `sdk.py test --machine .` run for all 64 machines with a
portable LLVM 23.1.5 (`--cxx`) and portable MinGW g++ (`--host-cxx`). The
toolchains were kept outside the repository
(a user-local portable LLVM 23.1.5 and a portable MinGW g++); no
system-wide install was made and the SDK was not edited. No engine was rebuilt.

**Result: 51/64 pass before the fix; 63/64 pass after.**

### Fixed (Class A — machine-owned `tests/sound.cpp`, not engine defects)

- `tests/sound.cpp` rewritten in the 12 failing machines (machine-owned tests,
  allowed by AGENTS.md). `mda-piano` untouched.
  - Sounding check now renders up to 4 blocks instead of one 64-frame block.
  - The three single-block A/B output comparisons (muffle filter, tone, tune)
    now compare the last block of a 4-block render, so the comparison frames are
    above the q31 multiply floor on onset-fade machines.
  - Removed two invalid assertions for these machines: rc=26 (the tail-vs-tail
    buffers are both silence below the q31 multiply floor at 400 blocks) and
    rc=32 (the `recording_step` invariant spanned two separately seeded voice
    instances, since `render_blocks()` re-initializes and re-starts).
- Root cause of the rc=4/20/21/28 failures: the 2026-11-15 onset sin^2 fade
  quantizes the first ~1.5-3 ms of each note to zeros in the stored 10-bit
  data, so block 0 is all zeros at note 69 on 8 of these machines (measured, not
  assumed; voices initialize correctly and sound from block 1-2 onwards).
- Incidental line-ending corruption found and normalized in
  mda-recorder-soprano and mda-recorder-tenor tests (\r\r\n throughout, to CRLF).

### Held (Class B) — mda-electric-organ

- `engine.wasm` is 1,057,165 bytes vs the SDK's fixed 1 MB (1,048,576 B) cap
  for the browser-preview artifact (+8,589 B / +0.8%). The machine builds the
  preview and passes the ABI/memory/controls/peak/lifecycle/drawing checks; it
  fails only the preview-bundle size gate. This is the browser-preview harness
  cap, not a device module verdict - device-side fit is unverified (no
  toolchain/hardware here) and must not be claimed.
- **Per user decision, this machine is held until everything else works.**
  Options for its disposition are recorded in `docs/verification-plan.md`
  (Step 4): with user go-ahead, re-attempt size reduction; else drop it from
  the release; else ship with the cap failure documented (not recommended).

### Honest status

- All 64 machines build the browser preview with the portable clang; 51 pass the
  full check suite as before, plus harmonica, harp, marimba, ocarina-small,
  pipe-organ, recorder-alto/bass/soprano/tenor, slit-drum, soprano-sax,
  mda-bells - after the test rewrite **63/64 machines pass `sdk.py test`**.
- Only remaining failure: mda-electric-organ (preview artifact cap only).
- Hardware CPU measurement remains pending - needs a real OP-XY; never claimed.
- 5 List X machines (altosax, bassoon, french-horn, renaissance-organ,
  saxello) remain deleted; out of scope.


## 2026-11-16 (session 11): mda-electric-organ end-trim rebuild — catalog green (64/64)

**Trigger.** Session 10 held mda-electric-organ at the 1 MB preview-artifact cap
(engine.wasm 1,057,165 B). User authorized the small end-trim rebuild plan, keeping
the machine in the catalog.

**Change.** `compact-prototypes/build_instrument_data.py` gained `--end-trim FRAC`:
uniform trim of the last FRAC of every stored zone, applied with the same
raised-cosine cos² fade as `--max-stored`, the fade is fitted inside the kept
samples just before the cut. All 15 zones rebuilt with `--end-trim 0.02`.
Per-zone stored: 38,353–73,168 -> 37,678–71,705; total 831,918 -> 819,617
samples; packed binary 1,024,522 B.

**Results.**
- `engine.wasm` now fits the SDK's fixed 1 MB browser-preview cap; the full
  `preview_bundle()` check passes (it was the only failing gate).
- Full `sdk.py test` for all 64 machines: **64/64 PASS** (host tests, preview ABI,
  memory, drawing smoke checks; CPU estimates not run catalog-wide; the organ's
  host-timed estimate is ~14,600 instructions/voice block, under the ~15,000
  aim - NOT a device measurement).
- Pitch-neutral: MIDI 60 reads 256.38 Hz before and after (-35.1 cents;
  pre-existing FreePats pitch_keycenter semantics, documented in session 6 -
  deliberately not retuned).
- `midi60-samples/mda-electric-organ_midi60.wav` re-rendered (MIDI 60); **user
  ear pass pending** before the machine is considered signed off.
- Pre-rebuild backups kept: `staged-mda-electric-organ-pre-endtrim/` (workspace
  and export copies, 8.9 MB).
- After the rebuild, the mandatory re-application pass was done: OP-XY ID
  (`mdaelectricorga`), lowercase `mda electric organ` name/description, namespace,
  panel fragment IDs, README/AGENTS restored, export's fixed `tests/sound.cpp`
  kept, LF->CRLF normalized (builder writes LF; repo convention is CRLF). The
  builder rmtree's and re-clones `mda-electric-organ` from `mda-piano`, so after
  any rebuild those re-application steps are **mandatory**, not optional.
- Workspace and export machine folders are byte-identical (0 diffs across all 64
  machines, verified after sync).
- Release folder (`release/machines/mda-electric-organ`) was NOT synced in this
  session; needs its own pass if requested.


## 2026-11-16 (session 12): ear pass sign-off; release sync; repo name decided; device-compile verification; hardware CPU remains pending

- **User ear pass on the rebuilt mda-electric-organ (MIDI 60 render): "it sounds
  great" — signed off.**
- **release/ synced for mda-electric-organ** (Copied `mda-ports/machines/
  mda-electric-organ` into `release/machines/`; release now holds all 64 machines).
  Honest note: before this sync, `release/` was stale against the 2026-11-16
  catalog-wide rename — every machine's `definition.json`/`machine.json`/README
  etc. differed, plus the rebuilt engines/samples (cello, clarinet,
  bass-clarinet, flute, harp) and the 12 fixed `tests/sound.cpp` files were the
  old builds. The user was informed; the rest of `release/` was left as-is
  pending a user decision (full sync vs let release keep its pre-rename state).
- **Repo name decided:** `dwd-git-dev/mda-ports` (chosen over `open-xy-mda`:
  honest provenance — these are ports of MDA Piano 1.0 — device-agnostic, and
  not implying affiliation with the upstream Open-XY catalog). Intended layout:
  multi-machine repository, machines at `machines/<id>/`, catalog `path`
  `machines/<id>`.
- **Firmware compile check:** all 64 machines compile against the pinned
  Blackfin toolchain installed in the SDK checkout (`sdk.py compile
  --machine <folder> --out <tmp>`, profiling on) — the same target family the
  device runs, with no device connected. This is a *compile check*, not a
  hardware measurement; the tools produce a module but no hardware test was run.
- **Hardware CPU remains PENDING.** Per `docs/profiling.md`, `draft` and
  `published` catalog entries require a `cpu` record measured on an actual
  OP-XY (`sdk.py build --machine <m> --profiling on --stock-os <private>/os.ldr`;
  `python sdk.py profile benchmark --package <private>/cpu-build-001 --installed
  --greet --out <private>/cpu-run-001`; then `python sdk.py profile report
  --machine <id> --package ... --capture .../capture.json`). No such hardware
  exists on this machine — the stock OS dump (user's own image) does not exist
  here — so no CPU number can be produced or claimed. Host-timed estimates are
  explicitly not acceptable for the catalog record. The exact remaining steps
  and requirements are: (1) device physically connected over USB in normal
  instrument mode (no MTP), MIDI port pair free (stop playback, close apps),
  system at rest; (2) the user's own stock OS image supplied under `--stock-os`
  (kept outside every checkout); (3) an authorized install (`candidate/` files,
  write OS to boot partition, power-cycle); (4) one benchmark collection (~80 s
  for a synth control sweep), saved to a fresh folder; (5) an offline report
  into the catalog entry's `cpu` member with `commit` equal to the tagged
  machine commit; (6) the catalog PR (changes only `catalog/machines.json`,
  `main` targeted, `sdk.py catalog check --base origin/main` as validation).
  Alternative: a maintainer can run a batch for contributors without an XY.
  Until one of those happens, the catalog keeps a reserved entry (not
  draft/published); host estimates never fill the field.


### Session 12 addendum (same day): verification gap closed; line-ending hygiene item documented

- `machine_verify.py electric-organ --probes auto` re-run after the end-trim
  rebuild: renders 6/6 probes (notes 36/48/60/72/84/96); all read IN TUNE
  (octave-fold), consistent with the FreePats Rock Organ pitch_keycenter
  semantics documented in session 6. No regression from the 2% end trim;
  pitch tracking matches the pre-rebuild render (-35.1 cents).
- Line-ending sweep completed and now recorded: the 79 files with doubled CRs
  (`\r\r\n`) introduced by the 2026-11-16 ID-rename tooling (per-machine
  `core.h`, `display.cpp[.backup]`, `display.h[.backup]`, `engine.cpp`,
  `panel.svg.fragment`, `tests/sound.cpp`, `tests/display.cpp` in the nine
  renamed machines) were normalized to CRLF in the catalog. The workspace
  copies were re-synced to match (0 byte-diffs across all 64 machines,
  verified after the mirror). The two `tests/sound.cpp` cases in
  mda-recorder-soprano/mda-recorder-tenor were the visible tip; the sweep
  covers the whole rename class.


## 2026-11-16 (session 13): repository created and published

- Local repo initialized in `mda-ports/`; single commit `6f7d995`
  ("Catalog of 64 Open-XY machines, MDA Piano 1.0 ports, SDK-verified"),
  1,300 files, working tree clean. Local commit author: `dwd-git-dev` /
  `dwd-git-dev@users.noreply.github.com` (disclosed; not auto-linked to a
  verified email). Machine-level CI files kept as blueprints; inert in this
  single-root layout.
- `.gitattributes` set text=auto with text filetype tracking; `.gitignore`
  covers build junk. Plain text storage of the 10-bit sample arrays (~141 MB);
  no LFS needed.
- `gh` CLI installed portable (user-local, not on PATH); user authorized via
  browser (https://github.com/login/device, one-time code). Repo created and
  pushed 2026-11-16: **https://github.com/dwd-git-dev/mda-ports**, public,
  default branch `master`, commit `6f7d995`. Visibility is public, as required
  for the upstream catalog path. Referenced from README.
- Release workspace: `release/machines/` re-synced to the catalog after the
  2026-11-16 rename (1,216 files, 0 byte-diffs); release README rewritten to
  the current 64-machine state; pre-sync machine trees backed up at
  `staged-release-machines-backup/`.
- Still pending, unchanged: hardware CPU records (needs device + OS image +
  profiling build + `sdk.py profile`; see session 12), upstream Open-XY catalog
  submission (catalog PR, needs hardware records and tags).


## 2026-11-16 (session 14): Proposed catalog-wide detection for two artifact classes

Trigger: mda-flute audibly clicks at E4/F4 mid-note on device. Owed measurements show
the defect class is NOT a factor transition; it is rail-riding content in the ff_E4
take. Both classes need audit before the catalog ships broadly.

### Proposed Next Steps (workstream notes; approval needed to build anything)

Two candidate detector classes, ordered by payoff. Scope decision still needs the user.

#### N1. Rail-riding audit (higher risk, highest priority)

**What it is.** The MDA sample engine stores 10-bit samples. If a zone's source take
reaches full scale (ff/forte takes especially), the stored content pins to the rail
for runs of consecutive samples; the engine's cubic interpolation between rail-pinned
points produces a brief local dip in output at predictable positions. Every note that
plays that zone dips at the same stored position every time.

**Measured so far.**
- mda-flute `ff_E4_0_m64.wav`: zone root 64 (`63139..98873`, `f=2,2/2`) rides the rail;
  on device, E4/F4/F#4 click mid-note (every playback). Peak 1.0. 2,788 jumps
  >12% of peak in the source (ff-quality take character); accepted-by-ear in a later
  session but now flagged again.
- Zone 64's b1 factor-boundary jump at `83410` is benign (`-182 -> -137`, ratio 0.3 of
  local p95); the click is at ~`72300`, deep inside segment 1, with a steeply-clipped
  rail plateau. The click local-minimum-render dip is ~3 frames wide at 0.41 s.
- Per-machine stored-samples count |sample| >= 28000 (out of 32704): mda-oboe 21,030;
  mda-recorder-alto 5,407; mda-ocarina-small 31,277; mda-clarinet 14,713;
  mda-eb-clarinet 10,109; mda-recorder-bass 20,915; mda-recorder-soprano 19,222;
  mda-soprano-sax 21,875; mda-flute 6,476; mda-accordion 1,839; mda-cello 1,332;
  mda-violin 629; mda-piano **11** (pass-by-comparison); mda-marimba 1,921.
- On the four machines probed so far, the true engine-dip only correlates with
  rail-plateau regions containing rail-plateau content actively *above* the
  surrounding plateau. Generalization is unverified.

**How we would detect.**
1. Trivial, exhaustive: count samples at >= thresholds (e.g., 28000, 30000, 31000,
   32000) per machine; rank machines by rail volume. Available now, done for 64 machines.
2. Engine-driven: render probes (every zone root, ~6 notes per machine, ~3.5 s each),
   scan output for consecutive-frame dips to <= 30% of local peak, AND confirm stored
   position at those frames sits inside a rail-run. Only engine says yes+no; the mere
   sample-count test is not sufficient (many rail samples, few audible dips).
3. Cross-beam sanity: zero counts on piano (human bench reference) vs huge counts on
   flute — correlation present in this sample but not yet proof of generalization.

**Open questions (need user decision).**
- Build full-catalog scan toolchain (detector + logger + auto-candidate taper) or
  manual engineering effort on flagged ff-only flute first?
- Auto-fix policy: yes/no; definitely requires per-machine ear pass before merge.
- Detection in the build pipeline (e.g., as part of `build_instrument_data.py` or
  `machine_verify.py`) vs standalone tooling.
- Whether to re-source ff-only flute from mf/pp dynamics (not present on disk; would
  mean new download or different library) for this one machine.

#### N2. Stored-data jump audit at factor boundaries (lower expected risk, easy measure)

**What it is.** At every stored-position where the planner switches factor (b1/b2/b3),
the 10-bit stored data is expected to be continuous; a large jump may produce a brief
audible event whenever the engine crosses that point, for any note of that zone.

**Measured so far.**
- Whole-file jump distributions: mda-piano median 21, p95 86, p99 131;
  mda-violin median 41, p95 172, p99 237; mda-flute median 50, p95 220, p99 317;
  mda-timpani median 6, p95 26, p99 43.
- Per-boundary jump ratio (jump size vs. windowed local p95, window +/- 200 samples):
  - mda-piano: every ratio <= 1.4; exact: max single-boundary ratio was 1.4
    (zone root 93 b2). All others <= 0.8.
  - mda-violin: ratios up to 1.4 (zone root 83); also 1.4 at 93.
  - mda-flute: max 0.7 (zone root 97 b1).
  - mda-timpani: ratios <= 0.4; boundary jumps collapse to < 5 samples everywhere.
- Conclusion: in the four machines measured, no factor boundary jump exceeded 1.4x
  local p95 (piano max 1.4, flute max 0.7, violin max 1.4). We have not rendered
  every note of every zone at every factor boundary; stored jumps are a proxy,
  not a verdict. Piano and flute appear benign at this proxy.
- Caveat: window is +/-200 stored samples; only b1<b2 boundary structure checked;
  we have not checked the exact position of EVERY boundary, e.g. the engine's actual
  boundary crossing frame alignment, only stored positions.

**How we would detect.**
1. For each machine, for every zone boundary (b1, b2, b3), compute
   |sample(bp) - sample(bp-1)| against a +/-N-sample neighborhood p95.
2. Flag outliers above local p95 * ratio thresholds; report counts (expected zero or
   near-zero in ff takes of quality).
3. Optionally, correlated-render probe: render each zone-root note and inspect ±8
   frames output around each boundary vs local peak; strongest signal for new-machine
   gates but costs a full build-run.

**Open questions.**
- What ratio threshold counts as "flag": 1.0 (flag if anything exceeds local p95) vs
  1.5 vs uses only p99?
- Scan all 64 machines now, or defer until N1 exists first?
- If flagged, typical remedies: rebuild with different `STEPS` (factor plan) or select
  a different source offset for the zone; not a post-build taper like N1.

### Common caveats (both items)

- The SDK is off-limits for edits (AGENTS.md rule: "Do not edit the SDK to make a
  machine work."). Any new validation lives in our own tooling, not `sdk.py`.
- The mda-flute fix applied this week (raised-cosine taper over 89 stored samples,
  max-delta change 18% of peak, candidate-H by ear) is N1's first data point: it
  eliminated a click and passed both automated gates and the user's ear pass. That
  proves the *fix* template; the *detection* tool does not yet exist.
- No catalog-wide machine has yet been scan-fixed. piano/violin/timpani need none,
  per the limited notes above.
