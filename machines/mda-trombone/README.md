# mda trombone

An Open-XY machine, ID `mdatrombone`. Machine code and tests live at this
repository's root.

## Current implementation

The display maps all 88 standard piano notes (MIDI 21–108, A0–C8) onto a
large grayscale spiral. Each octave makes a full turn, keeping every C at the
top of a loop; the spiral darkens at each C, ending in black at C8.
The spiral stays smooth as controls move; note markers shrink and dim
from note-on until they disappear, even while a key remains held, using a
shortened version of the MDA Piano Decay mapping; after note-off, any remaining
glow follows its Tail mapping. Pressing a close group of three to ten notes
draws a `#780606` outline between them; each segment narrows with its endpoint
markers' Decay and Tail envelopes until it is hidden, and widely spread
groups do not draw an outline. Separately, the Roman-numeral display recognizes
only five four-chord progressions: I–V–vi–IV, vi–IV–I–V, I–vi–IV–V,
I–IV–V–I and I–V–IV–I. The first chord is hidden; while a sequence is a valid
prefix, the latest numeral appears on both sides, and a completed progression
replaces it with the full sequence. Chords must arrive within one second of
each other; an unmatched chord can begin a new sequence. The numeral fades with
the current chord notes by clearing when they fade below visibility, without
changing size. Numerals use a fixed, larger 54 px light-gray type size.
Completed progressions appear inside simple eye outlines with eyelashes on both
sides of the spiral.
Hardness, Muffle, Decay, Tail, Tone, Random, Stretch and Tune also shape the
drawing.

The synth plays MDA's 15 multisample zones across the 88-note A0–C8 range,
with sample looping, the Hardness zone shift, Muffle filtering, Decay and Tail
envelopes, Tone, Random per-note detuning, Stretch tuning and fine Tune. The
sample data is MDA Piano 1.0 (upstream commit `8c8af6f`), taken unmodified and
packed as signed 12-bit PCM in [piano_samples.hpp](piano_samples.hpp), with
upstream's own keygroup table and loop points. The samples are covered by
[MDA-LICENSE.txt](MDA-LICENSE.txt).

The data is 22.05 kHz material: upstream advances it at 22,050 samples per
output second while the output runs at 44.1 kHz, so one data sample covers two
output samples. Each zone is a recording of one note, kept at full length -
its natural attack and decay, followed by the steady-state loop upstream loops
against.

**Each zone is stored in two parts: the recording, then its tail at a lower
rate.** The strike is where the brightness lives - the 4-11 kHz band sits 17 to
31 dB down through the first 100 ms of every note - so the recording is kept
whole up to the point where that band has died for good, and only what comes
after is low-passed and decimated. The engine plays the two parts at two
different rates, so the pitch does not move and the attack is untouched: the
first 100 ms of every note is bit for bit the original.

How far each zone's tail is cut is measured, not guessed: the machine is built
and rendered, and a zone's factor is lowered until that zone's own output is
within 1.5 dB of the original machine's. The result is 461 KiB of data
instead of 859 KiB, and the machine at 497 KiB instead of 894 KiB, with the
attack identical and the tail within 0.7 dB on average.

Two shorter routes were tried and rejected. Storing each zone at its own
lower rate from the start removes the 4-11 kHz band from the strike as well,
so the note goes dull rather than losing a transient. Replacing the sustain
with a model of the partials - a recorded attack, then eight modelled partials
- gets to 30 KiB of data, but the modelled sustain rings and does not sound
like this piano; it is kept as a prototype rather than the machine.

The samples are close to incompressible: their second differences carry about
9.8 bits each, so a lossless coder would land near 700 KiB. What is left
below that has to give up something audible.

### Audio controls

| Control | Current behavior |
|---|---|
| Hardness | Shifts the played key across adjacent MDA multisample zones. |
| Muffle | Increasing it lowers the sample low-pass cutoff. |
| Decay | Changes the MDA-style per-voice decay coefficient. |
| Tail | Switches to the MDA release rate once the internal envelope reaches half scale, lengthening or shortening the ring-out. The highest notes (94 and above) keep their Decay rate, as in MDA. |
| Tone | Changes filter brightness without detuning the played note. |
| Random | Adds deterministic per-note sample detuning. |
| Stretch | Applies MDA stretch tuning, sharpening treble notes by up to half a semitone at C8. |
| Tune | Fine-tunes the machine by up to half a semitone either way. |

The OP-XY's own voice layer supplies per-note velocity, the amp envelope
(including note-off release), stereo width and volume after this machine's
output. Those are not duplicated here. `Tail` approximates MDA's release rate
inside the machine's own envelope because the synth callback has no note-off
event; the stock ADSR remains the real key-release envelope.

## Develop

Keep the [SDK](https://github.com/etcetc000/open-xy) beside this repository as
`../open-xy`, on `staging`; adjust the commands if yours is elsewhere. The
[first-machine guide](https://github.com/etcetc000/open-xy/blob/staging/docs/first-machine.md)
covers prerequisites. Read the
[core machines](https://github.com/etcetc000/open-xy-core-machines) as worked
examples and the [machine API](https://github.com/etcetc000/open-xy/blob/staging/docs/api.md)
for the supported interfaces. Follow the
[UX conventions](https://github.com/etcetc000/open-xy/blob/staging/docs/ux.md);
Macro Osc is the two-page reference. Synth starters with five to eight controls
use the [shared header](https://github.com/etcetc000/open-xy/blob/staging/docs/display.md#two-page-header):
keep its SVG block and update the labels/value captions with your controls.
Columns without alternates repeat the same saved parameter on both rows.
Agent instructions are in [AGENTS.md](AGENTS.md).

Build and audition the synth in the browser:

```sh
python ../open-xy/sdk.py preview --machine . --out ../previews/mdatrombone-001
python ../open-xy/tools/serve_harness.py --preview ../previews/mdatrombone-001
```

```sh
python ../open-xy/sdk.py test --machine . --cxx clang++ --host-cxx g++
```

Extend `tests/` as the machine's behavior grows. The starter is a minimal
implementation, not a finished instrument. Keep generated output outside this
repository. The generated workflow checks source, sound tests and synth previews
on Linux, and compiles the native module on Windows; it never uses a device.
Its `OPEN_XY_REF` starts at `staging`; pin a compatible SDK commit for a release.

## Hardware and publication

Follow the SDK's [firmware build guide](https://github.com/etcetc000/open-xy/blob/staging/docs/build.md)
and [hardware profiling workflow](https://github.com/etcetc000/open-xy/blob/staging/docs/profiling.md).
Keep firmware and stock content outside every checkout. Commit the machine,
build with profiling enabled, install when authorized and collect a run
(see its "Collect a run" section).
The host estimate printed by `test` cannot substitute for those measurements.

For the builder, follow [catalog submission](https://github.com/etcetc000/open-xy/blob/staging/docs/catalog.md#submit-a-machine).
Every new or changed draft/published entry needs a hardware CPU record tied to
its pinned commit. Hardware validation may be performed by a maintainer if you
do not own an XY. Publication remains pending until that evidence is available.

The generated starter is covered by [LICENSE](LICENSE). Preserve notices for
code you reuse and include the licences of any dependencies or ports.
