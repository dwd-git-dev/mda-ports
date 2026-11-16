# Sampling Notes — how to audition every Open-XY machine

One markdown note per machine in this folder (`<machine>.md`, 69 files). Each tells you what the instrument should sound like, its source and license, its automated verification result, what was changed in the 2026-11-15 rework, the user's ear review, and exactly what to listen for.

**Fresh from the ears:** on 2026-11-15 the user A/B-tested the MIDI-60 renders. Machines with defects are marked `DEFECT: ...` in each note; everything else was passed by the user. Class A ("too much space", trailing silence) and class B (crackle/digital/click/noise) are split in `listening-log.md` and `issues-log.md`. Didgeridoo was removed entirely at user request.

## The protocol (per machine, ~2 minutes)

1. Load the pristine source next to the machine (path in each note; full inventory in `docs/source-inventory.md`).
2. Play the same phrase through source and machine — or one note per zone across the keyboard.
3. Judge, in order:
   a. **Attack** — does the onset match the source (pick bite / reed chirp / mallet thump)?
   b. **Sustain** — is the held part stable, not hollow/fizzy, no beating from a bad loop?
   c. **Tuning** — anchor notes vs equal temperament. Narrow-register machines may not cover the probes — judge by ear.
   d. **End of sample / loop wrap** — at the end of the stored sample the loop wraps; there must be no click, step or level jump. Capped machines (see below) sustain ~4 s then loop seamlessly — compare against the source's first ~4 s.
   e. **Decay** — tail behaves like the source, or like the known 4 s cap.
4. Write your verdict into the machine's note (or `fullpass-log.md`) and keep going. Flag anything you sign off as `ear-verified`.

## Suggested order (highest risk first)

1. **The 18 class-B engines rebuilt to remove the factor-6 mirror**: alto-flute, flute, bass-flute, clarinet, double-bass, harp, accordion, tuba, oboe, tenor-saxophone, trumpet, harmonica, soprano-sax. All rebuilt with STEPS=2/2/2 (the five biggest capped at 4 s, and soprano-sax capped at 3 s to preserve sustain inside the ceiling). The mirror alias is gone; verdicts unchanged; ear pass pending.
2. **Class A - "too much space"**: see the 11 flagged machines. Seven were rebuilt at `--tail-trim -30` (deep cut). Bagpipe, electric-organ, bells — untouched (no trailing silence above -60 dB available to trim).
3. **Narrow-register machines** (verdicts of 0-3/6 are probes, not necessarily defects): bells, flexatone, ocarina-small, agogo-bells, bell-tree, hand-bells-nepalese, xylophone, crotales, timpani, slit-drum, balafon, marimba, kalimba-*/mbira-*/nyunga-nyunga.
4. **Everything else** — mostly 4-6/6 probes and plausible; spot-check a few per family (winds, brass, strings, organs).

## Special categories

- **Capped (4 s + loop)** — all machines rebuilt 2026-11-15 with `--cap-long 4.0`. Note truncates with a raised-cosine fade; loop wrap at end of stored array. The `<machine>.md` says so in its rework line.
- **2/2/2 (no factor-6)** — all 18 class-B winds/strings/harmonics: no factor-6 decimation anywhere; the build no longer applies cubic interpolation to a decimated ~1.7 kHz slice, which is what produced audible mirror noise.
- **Glissandi** — flexatone, bell-tree: no static pitch to check; listen to the sweep.
- **Repeated zones (sparse keycenters)** — agogo-bells (2), hand-bells-nepalese (3), ocarina-small (~10), electric-bass (narrow set), harmonica (9): zones cover wide ranges; watch for timbre steps at boundaries.
- **Synthetic organs/keys** — electric-organ (FreePats Rock emulation), fm-piano/clavisynth (TX81Z), cowsynth (Karoryfer): synthetic by design, no acoustic attack.
- **Piano** — `mda-piano.md`: reference. Never rebuild. Sign-off exists; use it to calibrate your judgment of the others.

## Verdict table

| Machine | KiB | machine_verify | 2026-11-15 rework | Instrument |
|---|---|---|---|---|
| [`mda-accordion`](accordion.md) | 561.0 | 6/6 in tune | rebuilt 2/2/2 | Accordion |
| [`mda-agogo-bells`](agogo-bells.md) | 162.3 | 0/6 in tune | tail-trim | Agogo Bells |
| [`mda-alto-flute`](alto-flute.md) | 739.8 | 5/6 in tune | rebuilt 2/2/2 | Alto Flute |
| [`mda-bagpipe`](bagpipe.md) | 665.6 | 5/6 in tune | cap 4s | Bagpipe |
| [`mda-balafon`](balafon.md) | 118.9 | 4/6 in tune | tail-trim | Balafon |
| [`mda-bass-clarinet`](bass-clarinet.md) | 417.0 | 6/6 in tune |  | Bass Clarinet |
| [`mda-bass-flute`](bass-flute.md) | 579.2 | 6/6 in tune | rebuilt 2/2/2 | Bass Flute |
| [`mda-bass-trombone`](bass-trombone.md) | 300.1 | 5/6 in tune |  | Bass Trombone |
| [`mda-bell-tree`](bell-tree.md) | 582.6 | 0/6 in tune | tail-trim | Bell Tree |
| [`mda-bells`](bells.md) | 778.2 | 1/6 in tune | cap 4s | Glockenspiel |
| [`mda-cello`](cello.md) | 500.0 | 5/6 in tune |  | Cello |
| [`mda-clarinet`](clarinet.md) | 569.3 | 5/6 in tune | rebuilt 2/2/2 | Clarinet |
| [`mda-clavisynth`](clavisynth.md) | 433.8 | 5/6 in tune | cap 4s | Clavisynth |
| [`mda-cowsynth`](cowsynth.md) | 645.7 | 4/6 in tune | cap 4s | Cowsynth (medieval bagpipe synth) |
| [`mda-crotales`](crotales.md) | 162.7 | 0/6 in tune |  | Crotales |
| [`mda-dan-tranh`](dan-tranh.md) | 394.0 | 5/6 in tune | tail-trim | Dan Tranh |
| [`mda-double-bass`](double-bass.md) | 465.3 | 4/6 in tune | rebuilt 2/2/2 | Double Bass |
| [`mda-eb-clarinet`](eb-clarinet.md) | 403.2 | 4/6 in tune |  | Eb Clarinet |
| [`mda-electric-bass`](electric-bass.md) | 290.1 | 2/6 in tune |  | Electric Bass |
| [`mda-electric-guitar`](electric-guitar.md) | 589.8 | 5/6 in tune | cap 4s | Electric Guitar |
| [`mda-electric-organ`](electric-organ.md) | 673.6 | 6/6 in tune | cap 4s | Electric Organ |
| [`mda-flexatone`](flexatone.md) | 600.8 | 0/6 in tune | cap 4s | Flexatone |
| [`mda-flute`](flute.md) | 584.8 | 5/6 in tune | rebuilt 2/2/2 | Flute |
| [`mda-fm-piano`](fm-piano.md) | 592.0 | 6/6 in tune | cap 4s | TX81Z FM Piano |
| [`mda-folk-harp`](folk-harp.md) | 508.8 | 6/6 in tune | tail-trim | Folk Harp |
| [`mda-guitar`](guitar.md) | 147.7 | 3/6 in tune |  | Acoustic Guitar |
| [`mda-hand-bells-nepalese`](hand-bells-nepalese.md) | 227.7 | 2/6 in tune | tail-trim | Hand Bells, Nepalese |
| [`mda-hand-chimes`](hand-chimes.md) | 452.2 | 5/6 in tune | tail-trim | Hand Chimes |
| [`mda-hang`](hang.md) | 160.6 | 4/6 in tune | tail-trim | Hang |
| [`mda-harmonica`](harmonica.md) | 293.0 | 5/6 in tune | rebuilt 2/2/2 + cap 4s | Harmonica (Special20-C) |
| [`mda-harp`](harp.md) | 623.1 | 6/6 in tune | rebuilt 2/2/2 | Harp |
| [`mda-harpsichord-italian`](harpsichord-italian.md) | 524.6 | 6/6 in tune | tail-trim | Harpsichord (Italian) |
| [`mda-kalimba-kenya`](kalimba-kenya.md) | 235.2 | 3/6 in tune | tail-trim | Kalimba Kenya |
| [`mda-kalimba-tanzania`](kalimba-tanzania.md) | 163.1 | 3/6 in tune | tail-trim | Kalimba Tanzania |
| [`mda-marimba`](marimba.md) | 109.4 | 5/6 in tune | re-sourced (yarn→cord) | Marimba |
| [`mda-mbira-dzavadzimu`](mbira-dzavadzimu.md) | 406.0 | 2/6 in tune | tail-trim | Mbira dzaVadzimu Nyamaropa |
| [`mda-mbira-mavembe`](mbira-mavembe.md) | 559.0 | 4/6 in tune | cap 4s | Mbira Mavembe |
| [`mda-nyunga-nyunga`](nyunga-nyunga.md) | 350.9 | 3/6 in tune | tail-trim | Nyunga Nyunga |
| [`mda-oboe`](oboe.md) | 452.4 | 4/6 in tune | rebuilt 2/2/2 | Oboe |
| [`mda-ocarina`](ocarina.md) | 778.2 | 4/6 in tune | cap 4s | Ocarina, Typical |
| [`mda-ocarina-small`](ocarina-small.md) | 778.2 | 3/6 in tune | cap 4s | Ocarina, Small |
| [`mda-piano`](piano.md) | 317.1 | bit-identical | reference | Piano |
| [`mda-pipe-organ`](pipe-organ.md) | 778.2 | 6/6 in tune | cap 4s | Pipe Organ |
| [`mda-psaltery`](psaltery.md) | 579.1 | 4/6 in tune | tail-trim | Psaltery (bowed/plucked) |
| [`mda-recorder-alto`](recorder-alto.md) | 778.2 | 4/6 in tune | cap 4s | Baroque Alto Recorder |
| [`mda-recorder-bass`](recorder-bass.md) | 586.2 | 4/6 in tune | cap 4s | Baroque Bass Recorder |
| [`mda-recorder-soprano`](recorder-soprano.md) | 778.2 | 4/6 in tune | cap 4s | Baroque Soprano Recorder |
| [`mda-recorder-tenor`](recorder-tenor.md) | 778.2 | 5/6 in tune | cap 4s | Baroque Tenor Recorder |
| [`mda-slit-drum`](slit-drum.md) | 78.3 | 2/6 in tune | tail-trim | Slit Drum |
| [`mda-soprano-sax`](soprano-sax.md) | 620.6 | 5/6 in tune | rebuilt 2/2/2 + 3s cap | Bb Soprano Saxophone |
| [`mda-soprano-sax`](soprano-sax.md) | 620.6 | 5/6 in tune | rebuilt 2/2/2 + 3s cap | Tenor Saxophone |
| [`mda-steel-drum`](steel-drum.md) | 156.0 | 4/6 in tune | tail-trim | Steel Drum |
| [`mda-strumstick`](strumstick.md) | 504.6 | 5/6 in tune | tail-trim | Strumstick |
| [`mda-tenor-saxophone`](tenor-saxophone.md) | 293.0 | 6/6 in tune | rebuilt 2/2/2 + cap 4s | Tenor Saxophone |
| [`mda-timpani`](timpani.md) | 111.0 | 1/6 in tune |  | Timpani/Percussion |
| [`mda-trombone`](trombone.md) | 497.5 | 4/6 in tune | rebuilt 2/2/2 | Trombone |
| [`mda-trumpet`](trumpet.md) | 343.5 | 5/6 in tune | rebuilt 2/2/2 + cap 4s | Trumpet |
| [`mda-tuba`](tuba.md) | 318.3 | 4/6 in tune | rebuilt 2/2/2 | Tuba |
| [`mda-ukulele`](ukulele.md) | 270.2 | 5/6 in tune | tail-trim | Ukulele |
| [`mda-vibraphone`](vibraphone.md) | 778.2 | 4/6 in tune | cap 4s | Vibraphone |
| [`mda-viola`](viola.md) | 497.8 | 2/6 in tune |  | Viola |
| [`mda-violin`](violin.md) | 801.1 | 5/6 in tune | size anchor | Violin |
| [`mda-violin-pizz`](violin-pizz.md) | 358.0 | 5/6 in tune | cap 4s | Violin (pizz) |
| [`mda-wine-glasses`](wine-glasses.md) | 778.2 | 5/6 in tune | cap 4s | Wine Glasses |
| [`mda-xylophone`](xylophone.md) | 226.4 | 0/6 in tune |  | Xylophone |

## If you find a problem

Log it in `docs/issues-log.md` format (symptom → reproduction → diagnosis → fix), rebuild from pristine via `compact-prototypes/build_all_engines.py --only <name> [--tail-trim|--cap-long ...]`, re-run `machine_verify.py`, and note the outcome in `docs/fullpass-log.md`.

## What these notes are not

They are listening guidance, not proof. Nothing in this folder replaces your ears.

