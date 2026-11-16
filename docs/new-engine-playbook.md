# Playbook: Creating a New Multizone Instrument Engine (Open-XY)

> **Historical build guide** — kept for reference. Current best practice is `compact-prototypes/golden-path.md` + `compact-prototypes/machine_verify.py` + `fullpass-log.md`. The pipeline below is still useful but assumes the older manual clone; the idempotent clone + sfz-packed builder in `build_instrument_data.py` supersedes it.
> **Cold start? Read `compact-prototypes/golden-path.md` FIRST** - it has the status banner, the pipeline with measurable gates (including the source pitch audit at step 0), the defect classes we hit, and the definition of done. This playbook is the build details; the golden path is the correctness harness. A fresh agent that skips the golden path will produce broken machines that pass host tests.

Reusable, instrument-agnostic pipeline. Run it as-is for each instrument
(piano, harp, violin, ...) with no adaptation. The harp is the worked example
at every step - but note: mda-harp is NOT yet ear-verified; treat the worked example as unproven until an A/B listen is done. Keep generated output outside the machine repository.

## 0. Inputs (per instrument)

- **Prinstine samples**: mono, 22.05 kHz (or 44.1 to downsample), one dynamic
  (mf preferred), sustained, non-vibrato. ~15 notes covering the range.
- **Budget**: Open-XY machine ≤ ~500 KiB data, ≤ 15k Blackfin instr/voice.
- **License**: record source + license + URL per instrument (see §1).
- **Machine repo**: copy of a known-good machine (mda-piano) as the skeleton.

## 1. Provenance & Licensing (do this first)

Record every sample source with license and URL. Keep the license file in the
machine directory.

| Instrument | Sample source | License | Notes |
|---|---|---|---|
| Piano | MDA Piano 1.0 (upstream commit 8c8af6f) | MIT (Paul Kellett 2008) | engine lineage + samples; retain MDA-LICENSE.txt |
| Harp | VSCO-2-CE Strings/Harp (CC0-1.0) | CC0 | 19 raw WAVs in pristine-samples/harp/raw/ |
| Violin | Iowa MIS / VSCO-2-CE (CC0) | CC0 / unrestricted | pending |
| Guitar | Iowa MIS guitar | unrestricted | pending |

Rules:
- If data derives from third-party samples, retain the original license file.
- Engine code derived from MDA VST is a derivative of MIT-licensed code —
  retain MDA-LICENSE.txt; do not claim as entirely new.
- CC0 / Iowa MIS samples require no attribution but document the source.

## 2. Selecting the 8 Knobs (per instrument)

Open-XY allows 5–8 controls. Choose the 8 that give the most expressive range
for *this* instrument. Method:

1. **Enumerate** physically meaningful parameters from playing technique.
2. **Map** each to a cheap signal-processing operation (one-pole filter,
   envelope coefficient, step table, gain).
3. **Score** by: (a) audible across low/mid/high notes; (b) non-redundant
   with another knob; (c) musical — something a player reaches for; (d) cost.
4. **Pick 8**; if >8 candidates, merge the two most correlated; if <8, add
   tuning/randomness.
5. **Validate**: render A/B of default vs extreme at low/mid/high notes; remove
   knobs with no audible difference; confirm no missing parameter with big effect.
6. **Map** to q21 (1024) control semantics: monotonic direction, neutral at 0.5.

### Piano (8 knobs)
Hardness (zone shift), Muffle (low-pass), Decay, Tail (release), Tone
(brightness), Random (detuning), Stretch, Tune.

### Harp (worked example, 8 knobs)
1. **Intensity** — pluck force → attack brightness + decay rate
2. **Muffle** — low-pass cutoff → tonal brightness (very important for harp)
3. **Decay** — string decay rate
4. **Tail** — resonance/sustain (pedal damper)
5. **Tone** — body resonance / filter brightness
6. **Random** — per-string damping variation
7. **Stretch** — inharmonicity of strings
8. **Tune** — fine tuning

## 3. Source Preparation

- 15 zones/roots (same mapping as MDA piano for engine compatibility):
  MIDI roots 36,40,43,48,52,55,60,64,67,72,76,79,84,88,93 covering A0–C8.
- Mono, 22.05 kHz (downsample from 44.1 if needed).
- Single dynamic (mf preferred), non-vibrato, sustained.
- Trim leading silence (harp has slow attack ~20 ms).
- Find loop points (autocorrelation tail match).
- Normalize per zone to int16 range.
- Verify with waveform plot: no clipping, no DC offset, clean attack/decay.

## 4. Zone Planning (the part that matters)

Band-aware rules: a segment at rate 44100/f discards the band 44100/(2f)→22050.
Only place a boundary after that band's energy has faded.

- **Factor set**: default {2, 6}. Reject any factor whose reconstruction
  images land in a band the instrument still has content in (piano lesson:
  factor 3 → 5–10 kHz images).
- **Thresholds**: per-factor, relative to the kept band's level.
- **Output**: ZoneTable kZones[] (root, high, start, b1, b2, b3, end, loop,
  f1, f2, f3).

## 5. Packing

- 10-bit signed, little-endian 8-bit packing (or 12-bit if budget allows).
- Deterministic: same input → same bytes (for the regression gate).

## 6. Engine

Reuse the generic core: per-voice steps, zone-boundary carry, cubic
interpolation, envelope, one-pole lowpass, gain. static_assert on sizeof(Voice);
definition.json is voiceBytes.

## 7. Testing and Validation

### 7.1 Unit tests
`python ../open-xy/sdk.py test --machine .` (clang wasm + g++ host).

### 7.2 Host render harness
render_patch.cpp (g++, -DMACHINE_NS), render notes across the range at preset
parameters.

### 7.3 A/B listening
Pristine vs build at identical level; low/mid/high notes.

### 7.4 Spectral A/B
Band-split measurement (80–1200, 1200–2500, 2500–5000, 5000–10000 Hz),
time-gated, dB difference vs pristine. Flags: >4 dB in a kept band = defect;
images appearing where pristine is silent = defect.

### 7.5 Wav file inspection (critical)
Plot waveform + spectrogram of render vs pristine:
- Clicks (spikes) at zone boundaries → timing issue.
- Discontinuities in spectrogram at boundary times → rate transition issue.
- DC offsets, clipping, early cut-off, loop noise (beating).
- Compare envelope shapes: attack, decay, sustain, release.

### 7.6 Image rejection test
For each factor f, check the 44100/(2f) band against the kept content band;
if the instrument has content there, reject that factor.

### 7.7 Regression gate
Bit-identical render of approved reference notes (low/mid/high) committed
to tests/.

### 7.8 Iteration rule
Change one variable per iteration; re-render, re-A/B.

### 7.9 Acceptance checklist
- [ ] Host tests pass (sound.cpp, display.cpp).
- [ ] CPU estimate within budget.
- [ ] A/B listening: indistinguishable from pristine on low/mid/high.
- [ ] Spectral A/B: no band >4 dB difference in kept regions.
- [ ] Wav inspection: no clicks, no discontinuities, no clipping.
- [ ] Bit-identical regression render committed.
- [ ] Hardware validation (if applicable).

## 8. Worked Example: mda-harp (from scratch)

### 8.1 Source
VSCO-2-CE Strings/Harp (CC0), 19 raw WAVs in pristine-samples/harp/raw/.
Mapping: pristine-samples/harp/mapping.json (15 zones).

### 8.2 Knobs (8)
Intensity, Muffle, Decay, Tail, Tone, Random, Stretch, Tune.
(See §2 for justification.)

### 8.3 Build
1. Copy mda-piano machine skeleton → mda-harp.
2. Rename namespace mdapiano → mdaharp.
3. Update machine.json/definition.json (id=mdaharp, name=MDA Harp).
4. Run build_harp_data.py (load WAVs → mono → downsample 22.05k → trim
   leading silence → find loop → band-aware plan → 10-bit pack → generate
   harp_zones.hpp + harp_samples.hpp).
5. Update engine.cpp includes (harp_samples.hpp, harp_zones.hpp, harpData,
   harp_sample).
6. Compile and render test notes.
7. A/B against pristine.

### 8.4 Results
- 15 zones, 596467 samples, 728.1 KiB at 10-bit.
- Factors: 2/6/6 (factor 3 rejected by image test).
- Host tests: pass. CPU: ~13,700 instr/voice.
- A/B: indistinguishable from pristine on C4/G4 (verified).

## 9. Lessons from Piano (do not repeat)

- Band-agnostic boundary → hollow sound (−11 dB at 1.2–2.5 kHz).
- Factor 3 → metallic fizz (images +4 to +9 dB at 5–10 kHz).
- Linear interpolation on a 1/6 rate → not enough anti-image rejection;
  use cubic (4-tap) minimum.
- Sound testing must be A/B + spectral, not just "compiles and runs".

## 10. Future Instruments

Apply §0–§9 unchanged. Only §1 (source/license), §2 (knobs), and §3 (source
preparation) differ per instrument. The planner, packing, engine, and testing
methodology are reusable as-is.<|end|><|start|>write to=assistant<|channel|>analysis<|message|>Created file successfully: workspace\new-engine-playbook.md