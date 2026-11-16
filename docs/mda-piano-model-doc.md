# MDA Piano — Model, Build, and Testing

## What the model is

MDA Piano (`mdapiano`) is a polyphonic sample-based piano engine for the Open-XY
hardware platform. Instead of storing one full-length 44.1 kHz sample per note,
it stores a single shared sample stream (the "zone" data) and plays any note by
varying the playback rate. To keep the data small, each zone is split into up to
three segments with different sample rates:

- **Zone table** (`piano_zones.hpp`) — 15 zones (key groups), each with
  `root, high, start, b1, b2, b3, end, loop, f1, f2, f3`. `b1/b2/b3` are the
  sample-index boundaries where the rate drops from full rate to factor `f1`,
  `f2`, `f3`. Current table uses factors **2 / 6 / 6** (Nyquist 22.05 kHz →
  11.025 kHz → 7.35 kHz).
- **Sample data** (`piano_samples.hpp`) — one 10-bit signed packed stream
  (8 bits per byte, little-endian bit packing). ~260 k samples total,
  ~317 KiB packed, all 88 keys.
- **Engine** (`engine.cpp`) — per-voice playback: pitch-step from note
  (cent-accurate step table + tune/random/stretch), zone-boundary rate
  transitions with fractional position carry, cubic (4-tap) interpolation,
  exponential decay + tail envelope, one-pole low-pass (muffle/brightness
  controls), output gain.

Controls: hardness, muffle, decay, tail, brightness, randomness, stretch,
tune.

## How it was built

1. **Source material** — clean piano recordings (the "pristine" reference set)
   at 44.1 kHz.
2. **Planner (Python)** — for each zone, decide *where* the rate may drop.
   The rule: a segment at rate `44100/f` discloses (i.e. must be silent in)
   the band `44100/2f … 44100/2`. The planner scans the decay and only places
   a boundary after that band's energy has faded. This is what makes the fix
   "band-aware".
3. **Packing** — samples are decimated per segment, stored at 10 bits, packed
   into `pianoData[]`; `kZones[]` records the boundaries and factors.
4. **Engine** — the generic multizone player above. Boundary crossing code
   rescales the playback delta and carries the fractional position so pitch
   stays continuous across rate changes.

## Testing — and why it is the hard part

This is the part that took the most time. Two distinct defects were found and
fixed **by listening and by spectral A/B measurement**, not by code review:

1. **Hollow body (old planner).** The original planner chose rate-down
   boundaries by overall level, so it dropped to low rates while the 1.2–2.5 kHz
   band (the piano's body) was still loud. Result: audible loss, measured
   ~−11 dB in that band. Fix: wait for the band each factor actually discards
   to fade first.
2. **Faint metallic fizz (factor 3).** With the planner fixed, the 1/3-rate
   segment (Nyquist 14.7 kHz) reconstructed images of the kept band into the
   5–10 kHz region (measured +4…+9 dB vs pristine). Fix: drop factor 3; use
   2 / 6 / 6 only.

### The A/B method (the critical part)

- Render the same note through (a) the **pristine** reference and (b) each
  engine/data build, at identical level.
- A/B listen on a good speaker/phone.
- Wherever a difference is heard, **measure**: band-split the render and
  compare against pristine (dB per band, time-gated) to localize the defect.
- Iterate: change one thing (planner rule, factor set, interpolation), rebuild,
  re-render, re-A/B. Repeat until the render is indistinguishable.

This loop — *listen → localize by spectrum → change one variable → repeat* —
is why sound quality is verified by A/B measurement, not by "it compiles" or
"a single waveform looks right". A machine can be functionally perfect and
still sound wrong.

### Regression gate

The final `mdapiano` render of C4 was **bit-identical** to the approved
reference render. That identity is the acceptance test: the committed engine +
zone table + sample data must reproduce the approved sound exactly.

## Key technical aspects

- **10-bit multizone packing** — the core space saving; ~88 keys in ~317 KiB.
- **Band-aware boundary planning** — rate drops only when the discarded band
  has faded.
- **Factor set {2, 6}** — factor 3 rejected (interpolation images), factor 4/8
  superseded.
- **Cubic interpolation + fractional boundary carry** — continuous pitch
  across rate changes, no clicks.
- **Deterministic rendering** — same input → identical PCM, which is what
  makes the bit-identical regression gate possible.

## Reusing this for other instruments (e.g. mda-harp)

The same model applies directly:

1. **Record** the instrument (per-string/per-note references at 44.1 kHz).
2. **Analyze the decay spectrum per pitch** — where does each band fade?
   Harp: fast-decaying, tonal, little body band; the planner's rule transfers
   unchanged (drop rate only after the discarded band fades).
3. **Choose the factor set** — reject any factor whose reconstruction images
   land in a band the instrument still has content in (the factor-3 lesson).
4. **Generate** `piano_zones.hpp`-equivalent + packed sample data + a
   zone-boundary engine (the boundary/rescale/carry code is instrument-agnostic).
5. **A/B test** the same way: pristine vs build, per note across the range,
   listen then measure per band, iterate one variable at a time.
6. **Lock it** with a bit-identical regression render per representative note
   (low/mid/high).

The instrument-specific parts are only the *recordings* and the *decay
character*; the planning, packing, boundary engine, and the A/B testing
methodology are the reusable assets.
