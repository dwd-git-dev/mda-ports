# Digital Sound: how to test for it

What "digital sound" means in practice: the machine renders notes that don't match the pristine source. Not a crash or a compile error — a perceptual difference. These are the failure modes we've actually seen and measured, turned into checks you can run before any instrument is shipped.

## The four failure modes

### 1. Wrong pitch
The rendered note is at the wrong frequency. Detected by FFT peak of the rendered note vs the expected note.

**Rule:** The strongest partial must be within ±20 cents of the expected MIDI note frequency. A trumpet at C5 when expecting C4 is wrong.

### 2. Wrong level
The machine is significantly louder or quieter than the pristine reference. Detected by RMS comparison.

**Rule:** Rendered RMS within ±3 dB of pristine RMS for the same note and preset. A saxophone 5 dB hotter than piano is out of spec.

### 3. Spectral collapse (the "hollow" sound)
The multizone decimation removes bands that were still audible. Detected by time-gated band energy comparison.

**Rule:** No band (80–1200, 1200–2500, 2500–5000 Hz) may lose more than 3 dB vs pristine. More than 10 dB loss is the "hollow" sound we fixed for piano.

### 4. Silence / near-silence
The machine outputs mostly zeros. Detected by envelope inspection.

**Rule:** At least 40% of the rendered samples in the first 500 ms must exceed −60 dBFS. A note that decays to silence in 2 seconds and stays there is a bug or a design issue.

## Test procedure

For each machine, after building:

1. **Render 3 notes** across the range — low (e.g., C2), middle (C4), high (C6). Use `render_patch.cpp`.
2. **Peak frequency test** — FFT of first 500 ms. Compare peak to expected. Pass if within ±20 cents.
3. **Level test** — RMS of first 500 ms. Compare to pristine or to expected maximum. Pass if within ±3 dB.
4. **Band energy test** — compare 4 octave bands (80–1200, 1200–2500, 2500–5000, 5000–10000) against pristine. Pass if no band loses >3 dB.
5. **Envelope test** — draw RMS curve over time. Flag: silence starts before expected, or never rises above −40 dBFS.
6. **Discontinuity test** — count sample-to-sample jumps >50% of peak amplitude. Flag any >0.
7. **Loop test** — render 3 seconds, check for repeated transients where there should be a steady loop.

## What passes vs what doesn't

| Check | Pass | Fail |
|---|---|---|
| Peak frequency | Within ±20 cents | Outside ±50 cents |
| RMS level | Within ±3 dB of pristine | Outside ±5 dB |
| Band energy | No band loses >3 dB | Any band loses >5 dB |
| Envelope | Rises and decays naturally | Flat, always silent, or abrupt cut |
| Discontinuities | 0 | Any |
| Loop | Smooth loop, no repeating attack | Audible repeating transient |

## Automation

A script like `tools/test_machine.py <machine>` could:

1. Compile `render_patch.cpp` against the machine.
2. Render 3 notes.
3. Compute peak frequency, RMS, band energies, envelope, discontinuities.
4. Compare against pristine reference (if available) or against thresholds.
5. Print PASS/FAIL per check and an overall verdict.
6. Optionally write WAVs for listening.

This would catch the issues we found: trumpet octave-high, viola 2 Hz peak, alto sax quiet attack, etc. — before anyone listens to them.
