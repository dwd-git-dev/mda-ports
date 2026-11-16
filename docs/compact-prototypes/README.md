# Twelve ways to store the same piano

> **Historical project log** (Dec 13 — 2026-11-12, condensed 2026-11-15).
> This file documents the compression/modelling research that led to the
> current adaptive engine. It is **not** updated to the post-rebuild state
> of the catalog; for the current build/verify status read `instrument-build-log.md`.
> Kept for the provenance of the piano path. No decisions documented here remain binding.

A prototype sweep over compression and modelling approaches for MDA Piano,
run outside the machine so nothing in `mda-piano` depends on it.

Every method starts from the same pristine samples
(`mda-upstream/src/mdaPianoData.h`), encodes them, decodes them again and
renders the same six notes (C2, C3, C4, C5, C6, A6), so the byte counts and
the sounds are directly comparable. The listening board is at
`http://127.0.0.1:8919/`, section 4.

## How they are judged

The first pass compared only two derived things: the **partial levels over
time** (16 partials, 28 time points, a 93 ms window) and the **loudness
curve**. Both are computed after matching the peak.

That pass flattered the resynthesis methods, because it only looked at the
partials they model. A second pass (`rescore.py`) adds measures that do look
at the whole signal:

* **full band** - the distance of the whole short-time spectrum, band-limited
  to 40 dB below each frame's peak, so it compares what carries the sound;
* **attack** - the same over the first 100 ms;
* **floor** - the noise floor relative to the original's;
* **waveform SNR** - sample-wise, meaningful only where the phase survives;
* **roughness** - the largest sample step against the signal's own.

## Results

| Method | Data | Partials | Full band | Attack | Floor |
|--------|------|----------|-----------|--------|-------|
| group synthesis | 6.6 KiB | 2.4 dB | 3.9 dB | 9.5 dB | -28 dB |
| polynomial | 1.9 KiB | 2.1 dB | 4.1 dB | 9.4 dB | -27 dB |
| interpolation | 3.6 KiB | 2.7 dB | 4.2 dB | 9.2 dB | -28 dB |
| PCA | 3.4 KiB | 2.9 dB | 4.4 dB | 9.8 dB | -27 dB |
| delta between notes | 6.6 KiB | 9.0 dB | 5.6 dB | 11.1 dB | -27 dB |
| modal (Fourier) | 0.8 KiB | 2.8 dB | 6.1 dB | 9.5 dB | -31 dB |
| shared decay curves | 0.8 KiB | 2.7 dB | 6.2 dB | 9.5 dB | -31 dB |
| psychoacoustic | 0.3 KiB | 6.9 dB | 6.7 dB | 9.6 dB | -32 dB |
| wavelet | 88.6 KiB | 2.5 dB | 7.2 dB | 4.5 dB | +45 dB |
| low-rank (waveform) | 512 KiB | 38.6 dB | 7.5 dB | 5.1 dB | -56 dB |
| physical model | 0.03 KiB | 29.5 dB | 20.0 dB | 12.5 dB | -9 dB |
| one factor | 0.5 KiB | 10.2 dB | 22.3 dB | 10.8 dB | -6 dB |

For reference: the original samples are 1,145 KiB at 16 bit and 859 KiB
packed at 12 bit. The machine's non-sample code is 35 KiB.

## What the numbers say

* **The modelling methods are the compact ones.** Polynomial, modal and
  shared-decay describe the whole piano in under 2 KiB. The full-band
  distance puts them at 4-6 dB, roughly double what the partial-level metric
  said: they reproduce the partials and the decay, and miss the broadband
  part of the recording.
* **The attack is where the modelling methods lose.** About 9-11 dB over the
  first 100 ms: the recorded hammer transient against a generated strike.
  The coding methods keep the attack (wavelet 4.5 dB, low-rank 5.1 dB)
  because they keep the audio itself.
* **They also lose the noise floor.** Their floor sits 27-31 dB below the
  recording's: cleaner, but without the room and the key noise. The wavelet
  goes the other way, its discarded coefficients becoming a floor 45 dB above
  the original's, which is why its full-band distance is poor even though its
  partials match well.
* **Waveform SNR is around 0 dB for the resynthesis methods** - they are
  different waveforms, not slightly different ones, so a sample-wise
  comparison does not apply to them. It does apply to the coding methods:
  the wavelet keeps a 6 dB waveform SNR, the rank-3 factorisation loses it.
* **PCA is not the lever it looks like.** The singular values of the note
  matrix fall to 0.28, 0.15, 0.10, 0.10, 0.095 and then stay flat: fifteen
  piano notes are not low-rank.
* **Delta encoding between notes does not pay here.** The differences
  between neighbouring zones are not small enough to fit in fewer bits than
  the absolute values.
* **Psychoacoustic masking is the sharpest tool.** Everything below the
  masking thresholds leaves 0.3 KiB - the quiet partials really are
  inaudible - at 6.7 dB of full-band error.
* **Waveform-domain methods lose.** A rank-3 factorisation of the waveforms
  costs 512 KiB: notes at different pitches share no waveform basis.
  Wavelet coding works properly but needs 88.6 KiB to reach 7.2 dB, because
  a piano's transient spreads its energy over many coefficients.
* **The physical model is a different instrument.** A Karplus-Strong string
  is 30 bytes and its brightness falls on its own the way a piano's does,
  but its loudness curve is not MDA's curve, and these samples have a weak
  or missing fundamental that a string model always has.

## The residual: playing the model and a sample together

`hybrid.py` takes the polynomial model, subtracts it from the original, and
plays the two back together. The residual is everything the model does not
explain: the hammer transient, the key noise, the beating between strings.

Adding the whole residual back reproduces the original exactly (0.00 dB on
every measure), so the arithmetic is sound. The interesting part is what the
residual turns out to be:

* **It is not just the attack.** Only 10.4% of its energy is in the first
  10 ms; the rest is spread through the note at 3-5% per 20 ms window.
* **It is uncorrelated between notes** (mean correlation +0.002, max +0.217),
  so one shared residual cannot serve the whole keyboard.
* **A richer model does not shrink it.** Moving from a cubic to a quintic to a
  seventh degree per partial leaves the residual at -4.5 dB of the original's
  energy. The residual is phase drift and broadband content, not envelope
  error, so more coefficients do not help.
* **Noise is not a substitute for it.** Synthesising the residual as shaped
  noise scores 12.6 dB, far worse than leaving it out (4.1 dB).

What does work is keeping a short head of each zone's own residual:

| Variant | Residual data | Partials | Full band | Attack |
|---------|---------------|----------|-----------|--------|
| polynomial alone | 0 | 2.1 dB | 4.1 dB | 9.4 dB |
| + 20 ms per zone | 19.4 KiB | 2.3 dB | 3.6 dB | 4.2 dB |
| + 60 ms per zone | 58.1 KiB | 2.7 dB | 4.3 dB | 3.6 dB |
| + 120 ms per zone | 116.3 KiB | 1.9 dB | 4.6 dB | 0.3 dB |
| one shared residual | 4.3 KiB | 4.8 dB | 5.7 dB | 6.9 dB |
| synthesised noise | 0.1 KiB | 12.1 dB | 12.6 dB | 12.7 dB |

The 20 ms version is the sweet spot: 19.4 KiB of residual buys a 5 dB better
attack and a slightly better overall match than the model alone. The head is
counted at 44.1 kHz and 12 bit; at the machine's 22.05 kHz data rate it would
halve to 9.7 KiB, but the top octave of the hammer would be gone. Longer heads
restore the attack further but the step where the residual is dropped starts
to cost more than it gains.

## Shared dictionary plus residuals

`dictionary.py` cuts every zone into 512-sample segments, clusters the segments
that look alike across all fifteen zones into one shared dictionary, and
stores each segment as a few dictionary entries with their gains plus the
residual in a DCT basis. The engine's side is a handful of multiply-adds per
segment, then one lookup, one multiply and one add per sample - close to what
the sample player costs now.

**The material repeats strongly.** A dictionary of only 16 entries, 8 KiB in
total, predicts the segments to within 1.94 dB:

| Entries | Falls short by | Dictionary cost |
|---------|----------------|-----------------|
| 16 | 1.94 dB | 8 KiB |
| 32 | 3.05 dB | 16 KiB |
| 64 | 5.04 dB | 32 KiB |
| 128 | 6.83 dB | 64 KiB |
| 256 | 9.44 dB | 128 KiB |

**With a sparse combination and a coded residual:**

| Entries | Terms | Data | Partials | Full band | Attack |
|---------|-------|------|----------|-----------|--------|
| 64 | 2 | 118 KiB | 5.7 dB | 4.9 dB | 5.2 dB |
| 64 | 4 | 149 KiB | 4.6 dB | 4.5 dB | 4.8 dB |
| 128 | 2 | 152 KiB | 3.0 dB | 3.9 dB | 3.9 dB |
| 128 | 4 | 186 KiB | 2.1 dB | 3.3 dB | 3.4 dB |

The original packed at 12 bit is 859 KiB at 0 dB, so the dictionary codec is
4.6 times smaller for a 3.3 dB error, and its decoder is cheap.

**The mathematical extension does not pay.** Coding the residual in its own
eigenbasis - the KLT, the eigenvectors of the residual covariance - gives
3.24 dB where the DCT gives 3.26 dB, and the basis itself costs 8 to 32 KiB.
The residual is noise-like, and no linear transform turns noise into something
compressible.

What would still move this number: entropy coding the residual coefficients
(they are currently raw fixed-length 8-bit and heavily peaked around zero),
perceptual weighting to drop the masked ones, and a second-level dictionary
over the per-segment coefficient vectors.

## How low can the samples go? The lossless baseline

`pristine_zip.py` writes every zone out as a WAV and compresses the set with
everything available, and then computes the floor a lossless coder could
reach.

The set is **15 zones**, not 18: that is MDA's own keygroup table, and it
covers all 88 notes.

| Form | Size | Of the WAVs | Error |
|------|------|-------------|-------|
| 16-bit WAV at 22,050 Hz | 1,145.8 KiB | 100% | none |
| **zip (deflate 9)** | **1,095.2 KiB** | **95.6%** | none |
| gzip | 1,095.0 KiB | 95.6% | none |
| xz | 1,043.3 KiB | 91.1% | none |
| bzip2 | 985.7 KiB | 86.0% | none |
| the machine's 12-bit pack | 858.9 KiB | 74.9% | inaudible, about 72 dB SNR |
| the 12-bit pack, gzipped | 816.2 KiB | 71.2% | inaudible |
| Rice-coded second differences | 703.2 KiB | 61.4% | none |
| order-2 entropy, whole set | 744.6 KiB | 65.0% | none |

The zip is at `workspace/piano-experiments/pristine_zones.zip` and the WAVs
beside it.

**The headline: zipping barely helps.** DEFLATE gets 4% off audio samples, and
the best general-purpose compressor here gets 14%. The order-2 entropy of the
material is about 9.8 bits per sample, so even an ideal lossless coder -
which is roughly what FLAC is - lands at 703 to 745 KiB. Nothing lossless
takes this piano below about 700 KiB, and the machine's 12-bit pack at 859 KiB
is already within 20% of that floor.

Everything below roughly 700 KiB is therefore lossy by definition, and that is
where the modelling work lives:

| Route | Size | Error |
|-------|------|-------|
| pristine 16-bit | 1,146 KiB | 0 dB |
| 12-bit pack (in the machine now) | 859 KiB | inaudible |
| lossless floor | ~703 KiB | 0 dB |
| dictionary codec | 186 KiB | 3.3 dB |
| model plus residual | 21 KiB | 3.6 dB |
| polynomial model | 1.9 KiB | 4.1 dB |

## Fewer zones, better interpolation

`zonereduction.py` drops zones and fills the gaps. Measured on the gap notes
only, since the notes a kept zone covers need no interpolation. The reference
is what the machine does today with all fifteen zones.

| Zones kept | Data | Method | Full band | Attack |
|-------------|------|--------|-----------|--------|
| 15 | 859 KiB | resampling (today) | 0.00 dB | 0.00 dB |
| 8 | 441 KiB | resampling | 3.83 dB | 2.84 dB |
| 8 | 441 KiB | morph, harmonic index | 6.24 dB | 11.52 dB |
| 8 | 441 KiB | morph, absolute frequency | 6.77 dB | 12.02 dB |
| 5 | 302 KiB | resampling | 7.52 dB | 3.58 dB |
| 5 | 302 KiB | morph, harmonic index | 6.88 dB | 10.74 dB |
| 4 | 219 KiB | resampling | 8.35 dB | 3.85 dB |
| 4 | 219 KiB | morph, harmonic index | 7.25 dB | 10.97 dB |

Two things stand out. **Interpolating by harmonic index beats interpolating by
absolute frequency**, so a piano's timbre morph follows the partial number, not
the frequency. And **the morph only wins once the gaps are wide**: at eight
zones plain resampling is still ahead, at five and four the morph is better,
because a treble zone pulled down three semitones is simply the wrong note.

**The ceiling.** A variant that carries the neighbour's recording and corrects
only its spectrum and decay was built and measured against an oracle: the same
correction, but driven by the *true* envelope of the note being played, which
the machine does not have. The oracle reaches 0.09 dB with all fifteen zones,
so the correction machinery itself is sound. But at eight zones the oracle is
5.80 dB, *worse* than plain resampling at 3.83 dB, and at five zones it is
13.05 dB. A different note's recording simply does not contain what is missing,
and no amount of clever correction recovers it.

That is the answer to "half the zones with better interpolation":

| Eight zones kept | Data | Full band on the gap notes | Attack |
|------------------|------|----------------------------|--------|
| resampling (today's method) | 441 KiB | **3.83 dB** | 2.84 dB |
| morphed envelope | 441 KiB | 6.24 dB | 11.52 dB |
| recording for the attack, morphed sustain | 441 KiB | 5.35 dB | 4.73 dB |
| envelope correction with the true envelope (oracle) | 441 KiB | 5.80 dB | 4.41 dB |

So the honest answer is that resampling is hard to beat, and the headroom for
better interpolation in that direction is negative. The route that does beat it
is the transform codec below, which needs no zone structure at all.

## What the CPU measurement settled

`cpu_cost.cpp` times each candidate decoder on the host against the SDK's own
reference loop (590 Blackfin instructions per 64-sample block), so the numbers
are comparable with the machines in docs/api.md. The budget is 15,000 per
voice block.

| Route | Instructions a voice block |
|-------|---------------------------|
| the sample player, bare | 291 |
| the dictionary codec's decoder, bare | the same shape, so the same |
| the additive model, 4 partials | 4,496 |
| the additive model, 8 partials | 8,695 |
| the additive model, 12 partials | 13,114 |
| the additive model, 16 partials | 17,367 |
| the MDCT decode, n=32 | 27,297 |
| the MDCT decode, n=256 | 41,145 |

Two conclusions. **The MDCT codec is dead on this hardware** - even the
smallest transform is twice the budget, and amortising the decode over a whole
zone does not rescue it, because decoding a zone costs about eighty times
playing it. **The additive model fits at eight to twelve partials**, and the
psychoacoustic analysis independently says eight to twelve partials carry the
sound. That convergence is what makes the model route worth pursuing.

## The additive model, done properly

`additive_v2.py` adds the three things the first version left out.

* **Inharmonicity.** The partials sit at `m f0 sqrt(1 + B m^2)`. The fitted
  stiffness is 3.9e-5 in the bass and near zero in the treble, which is what
  the physics says. It moves the error from 4.10 to 4.07 dB.
* **Real phases.** Each partial keeps its measured phase. The waveform
  correlation stays around zero, because a fraction of a dB of phase error on
  one partial decorrelates a sum of partials - the spectral distance is the
  meaningful measure, not the waveform.
* **The noise floor.** The recording's own floor, measured from its quietest
  stretch and resynthesised with that shape. Worth 0.07 dB.

So the feature targets turned out to matter much less than the attack.

## The hybrid: a real attack, a modelled sustain

`hybrid_model.py`. The CPU measurement said the additive model fits the
machine; the error measurement said the model's problem is the attack (9.6 dB)
and not the sustain. So keep a short slice of each recording for the attack
and model everything after it.

| Real attack kept | Data | Full band | Attack error |
|------------------|------|-----------|--------------|
| none (the model alone) | 1.8 KiB | 4.07 dB | 9.76 dB |
| 20 ms | 9.7 KiB | 3.12 dB | 3.29 dB |
| 40 ms | 19.4 KiB | 3.00 dB | 2.90 dB |
| 80 ms | 38.8 KiB | 2.94 dB | 1.47 dB |
| 160 ms | 77.5 KiB | 2.43 dB | 0.02 dB |

**And with fewer zones, `hybrid_zones.py`:**

| Zones kept | Data | Full band | Attack error |
|------------|------|-----------|--------------|
| 15 | 21.2 KiB | 3.00 dB | 2.90 dB |
| 8 | 11.3 KiB | **2.94 dB** | 3.38 dB |
| 5 | 7.1 KiB | 3.06 dB | 3.27 dB |

Halving the zones costs nothing, because a gap note borrows the
neighbouring zone's attack, resampled to its own pitch, and takes its sustain
from the blended envelopes of the two neighbours.

**A shared basis across the zones does not help the model.** The singular
values of the model's curve matrix are 1, 0.37, 0.32, 0.29, 0.25, 0.22, 0.19 -
still a slow decay, so eight components leave a 16 dB error in the partial
levels and cost 1.1 KiB. The direct per-note model is 1.8 KiB with no
approximation error, so it wins. This is the opposite of the earlier PCA
result on the raw trajectories: the model's representation is already compact
and structured, so sharing it has nothing left to exploit.

Together: **7 to 21 KiB for the whole piano at about 3 dB**, with a per-voice
cost of 13,000 instructions, inside the machine's budget.

## The multi-rate switch: watch the band you remove

`hybrid_rate.py`. The multi-rate piano keeps a real attack and then lowers the
data rate as the note decays, in four segments: full rate, then 1/3, 1/4 and
1/8. This is the largest size win in the project - 859 KiB down to 286 KiB at
10 bits - and it produced the worst artifact in the project.

The audible fault was a hollow, fizzy decay: the body of the note dropped away
and a thin top appeared on top of it. Measured on the Concert preset, C4,
0.7-1.2 s, against the full-rate recording:

| Build | 1.2-2.5 kHz | 5-10 kHz |
|-------|-------------|----------|
| full rate (reference) | 94.2 dB | 62.7 dB |
| multi-rate, old planner | **83.2 dB (-11.0)** | **70.1 dB (+7.4)** |
| multi-rate, fixed planner | 94.0 dB (-0.3) | 66.9 dB (+4.2) |

**The cause was the switch point, not interpolation or bit depth.** Controlled
runs, changing one thing at a time:

| Change | Effect on the tail error |
|--------|--------------------------|
| linear -> cubic interpolation | 0.1 dB |
| 12 -> 10 bit | 0.0 dB |
| move the switch later | **4.8 to 10.7 dB** |

The planner measured a fixed 4-11 kHz band before every switch. That is
roughly the band a factor-2 segment removes (5.4-11 kHz), but not the band a
larger factor removes:

| Factor | Stored up to | Band the switch discards | What the planner measured |
|--------|--------------|--------------------------|---------------------------|
| 2 | 5.37 kHz | 5.4-11 kHz | 4-11 kHz |
| 3 | 3.53 kHz | 3.5-11 kHz | 4-11 kHz |
| 4 | 2.61 kHz | 2.6-11 kHz | 4-11 kHz |
| 6 | 1.70 kHz | **1.7-11 kHz** | 4-11 kHz |
| 8 | 1.24 kHz | **1.2-11 kHz** | 4-11 kHz |

Waiting for 4-11 kHz to fade and then dropping to 1/6 throws away 1.7-4 kHz
while it is still loud - on C4 at 0.56 s, with most of the audible decay still
ahead. That is the 11 dB hole. A later factor-8 switch does the same thing
again, one rate step lower, and adds the interpolation images heard as fizz.

**The rule, in `hybrid_rate.py`:** measure the band the new factor actually
removes, not a fixed band.

```python
def removed_band_lo(factor):
    return min(4000.0, cutoff_for_factor(factor))

def sustained_boundary(x, threshold_db, limit, factor):
    levels = high_band_level(x, lo=removed_band_lo(factor))
    ...
```

The 4 kHz floor matters. Factor 2 removes only 5.4-11 kHz, and measuring from
5.4 kHz would switch C4 at 0.06 s instead of 0.28 s, dulling the attack's top
octave. Never measure a narrower band than 4-11 kHz.

`plan_segments` uses that boundary for every switch, so a factor-8 segment
waits until 1.2-11 kHz is below threshold. `check_segments` re-derives the safe
boundaries and rejects a hand-written table that switches earlier - it is there
because the failure was silent in the data and only audible in the result:

```
segment 2 switches to factor 3 at 0.280 s, but the band it
removes is above its threshold until 0.300 s
```

The corrected planner, with the original thresholds, moves the C4 switches
from 0.28/0.40/0.56 s to 0.30/1.54/1.54 s and the G4 switches from
0.20/0.32/0.54 s to 0.20/0.84/1.04 s. It costs nothing in size: 286.2 KiB at
10 bits versus 288.9 KiB for the hand-tuned build, and 343.4 versus 346.7 KiB
at 12 bits. The tail now matches full rate within 1 dB from 1.2 to 5 kHz.

**For any other instrument using this scheme:** the threshold and the band must
be chosen per factor, and the band is the one being discarded. If a build
sounds hollow or fizzy in the decay, measure the removed band before touching
interpolation, dither or bit depth - those were all within 0.1 dB here.

## Would a frequency-weighted coder save more?

`bandwidth.py`. The material has almost nothing above 5 kHz: the level at 8 kHz
is -83 to -95 dB below the note's peak, and the audible limit per zone falls
between 6.5 and 10.3 kHz.

| Data rate | Bits | Size | Of the current machine | Error |
|-----------|------|------|-------------------------|-------|
| 22,050 | 12 | 859 KiB | 100% | 0.01 dB |
| 22,050 | 10 | 716 KiB | 83% | 0.02 dB |
| 22,050 | 8 | 573 KiB | 67% | 0.09 dB |
| 16,000 | 12 | 623 KiB | 73% | 0.33 dB |
| 16,000 | 8 | 416 KiB | 48% | 0.35 dB |
| 11,025 | 12 | 429 KiB | 50% | 0.58 dB |
| 11,025 | 8 | 286 KiB | 33% | 0.59 dB |
| 8,000 | 12 | 312 KiB | 36% | 1.14 dB |
| 8,000 | 8 | 208 KiB | 24% | 1.15 dB |

**The rate is the lever, not the bits.** Halving the data rate costs 0.58 dB;
halving the bit depth costs 0.02 dB. And the threshold-weighted distance is the
same as the flat one, so what the rate cut removes really is below the
threshold of hearing rather than merely different.

The rate change is an engine change, not a sound change: the engine plays the
data at the data's own rate, so a lower rate only means a different entry in
the step table.

## The CPU question

Every modelling method needs the partials synthesised at run time: one sine
and one envelope evaluation per partial per voice-sample. At 16 partials and
8 voices that is far beyond the machine's budget, which the host estimate
puts at about 8,000 Blackfin instructions per voice block. The same fits can
be baked into one wavetable per zone, but then the size is the size of the
tables, not the size of the model.

## Files

* `piano_core.py` - feature extraction, the shared renderer, the metrics
* `methods.py` - the twelve methods
* `hybrid_rate.py` - the multi-rate planner and the removed-band rule
* `build_instrument.py` - builds a machine's data from a zone mapping
* `run_all.py` - runs everything, scores it, writes the board audio
* `rescore.py` - the second scoring pass, over the whole signal
* `write_board.py` - writes the board section
* `out/results.json`, `out/rescore.json` - the tables above

Run with `& ..\open-xy\.venv\Scripts\python.exe run_all.py`.
