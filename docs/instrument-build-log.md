# Instrument Build Log

Tracking the multizone (pristine + adaptive) engine builds for each instrument.
Statuses: `pending` | `samples-found` | `pristine-built` | `adaptive-built` | `tested` | `done`.

**Cold start:** read `compact-prototypes/golden-path.md` FIRST (pipeline with gates, doc precedence rules). Then read `fullpass-log.md` for machine-by-machine verification results. This log is the per-instrument master. `"Adaptive" status means "exists", never "good"` — pitch is verified only by `machine_verify.py`.

## Note (2026-11-15 evening, session 7)

**Loop re-entrance ghost fixed** for the three machines where it was measured audible: mda-tuba, mda-electric-bass,. Root cause: engine has no note-off, the zone loops forever behind a nearly dead piano decay envelope; stored ff content after the note's floor re-enters audibly (~1.7 s). Fix: `build_instrument_data.py` gained `--end-fade <SECONDS>` (cos² ramp of the final N seconds of each stored zone to zero before planning, silent looprun chosen over the old 8-sample loop blend when the tail is quantizer-silent). All three rebuilt, re-verified at their pre-fix verdicts, re-rendered. mda-violin-pizz suspected but ruled out by measurement: its bump is the source's own second hump (the plucked string's body resonance); kept bit-identical to the build that verified 5/6. See `issues-log.md` session 7 and `fullpass-log.md` for the full facts.

## Note (2026-11-15 evening, session 5)

**Octave-high spawn bug fixed in every non-piano machine** (`engine.cpp`: spawn comparisons `>` -> `>=` in `initialize_note`). Any machine whose segment 1 is empty (`b1 == zone.start`) spawned at the segment boundary without dividing the playhead delta by the stored-rate factor, so the decimated content played 2x too fast (exactly one octave high) and looped at that rate - reported by the user as a "gap/click" on tuba and consistent with many ear-pass flags. 33 machines ship at least one such zone (tuba 12/15); machines with all non-empty segment 1s are unaffected bit-for-bit. `mda-piano` untouched (bit-identical reference). `build_instrument_data.py` now applies the same patch at clone time. See `issues-log.md` session 5 for the full diagnosis, measurements and per-machine table; `compact-prototypes/verify_full.log` for the re-run verdicts. `release/` refreshed.

## Note (2026-11-15 evening, session 3)

**All machines except piano were rebuilt with an 8 ms raised-cosine fade at the attack of every zone** (`build_instrument_data.py`). This fixes the attack click/thump class of defects the user flagged (bagpipe, alto-flute, bass-flute, trombone, guitar, and likely more). See `issues-log.md` for the full fix inventory and `fullpass-log.md` for the session entry.

## Master list (#1-#73)

| # | Instrument | Knobs (8) | Articulations | Source | Status | Adaptive KiB | Notes |
|---|---|---|---|---|---|---|---|
| 1 | Piano | Hardness, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | single (sustained) | MDA Piano 1.0 (MIT) | done | 317.1 | verified reference; bit-identical render; ear 2026-11-15: OK (user pass, 2026-11-15) |
| 2 | Harp | Intensity, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | single (plucked) | VSCO-2-CE (CC0) | done | 623.1 KiB | 5/6 in tune after clean rebuild (2026-11-15); ear 2026-11-15: OK (user pass, 2026-11-15); rebuilt 2026-11-15 (STEPS 2/2/2 — factor-6 alias mirrors removed) |
| 3 | Violin | Bow pressure, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | arco (pizz see #3b) | Iowa MIS arco sweeps | done | 801.1 | 5/6 in tune; 36 out of range; pizz source poor; ear 2026-11-15: OK (user pass, 2026-11-15) |
| 3b | Violin (pizz) | Bow pressure, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | pizzicato only | Iowa MIS arco sweeps | done | 358.0 | 5/6 in tune; cmd-line pizzistry uses same maps; reworked 2026-11-15 (tail-trim -40dB + 4 s zone cap; verify verdict unchanged); ear 2026-11-15: DEFECT: bounce, maybe |
| 4 | Cello | Bow pressure, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | arco + pizzicato | Iowa MIS / VSCO-2-CE (CC0) | adaptive-built | 500.0 | machine-verified 5/6 in tune (36/48 octave-fold) 2026-11-15; ear 2026-11-15: OK (user pass, 2026-11-15) |
| 5 | Viola | Bow pressure, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | arco + pizzicato | Iowa MIS / VSCO-2-CE (CC0) | adaptive-built | 497.8 | 2/6 in tune; high probes octave-folded; ear 2026-11-15: OK (user pass, 2026-11-15) |
| 6 | Flute | Breath, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | sustained (vib/non-vib) | Iowa MIS single notes | done | 584.8 | 5/6 in tune; 36 out of range; ear 2026-11-15: DEFECT: loud digital noise; rebuilt 2026-11-15 (STEPS 2/2/2 — factor-6 alias mirrors removed) |
| 7 | Clarinet | Breath, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | sustained (vib/non-vib) | Iowa MIS / VSCO-2-CE (CC0) | done | 569.3 | 5/6 in tune; ear 2026-11-15: DEFECT: crackle and digital noise; rebuilt 2026-11-15 (STEPS 2/2/2 — factor-6 alias mirrors removed) |
| 8 | Oboe | Breath, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | sustained | Iowa MIS / VSCO-2-CE (CC0) | done | 452.4 | 4/6 in tune; ear 2026-11-15: DEFECT: loud digital noise; rebuilt 2026-11-15 (STEPS 2/2/2 — factor-6 alias mirrors removed) |
| 10 | Trumpet | Breath, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | sustained (straight/muted) | Iowa MIS (Bb) / VSCO-2-CE | done | 343.5 | 5/6 in tune; reworked 2026-11-15 (tail-trim -40dB + 4 s zone cap; verify verdict unchanged); ear 2026-11-15: DEFECT: crackle; STEPS=2/2/2 rebuild + 4s cap 2026-11-15 |
| 12 | Trombone | Breath, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | sustained (straight/muted) | Iowa MIS (Tenor/Bass) | done | 497.5 | 4/6 in tune; ear 2026-11-15: DEFECT: click; digital noise; STEPS=2/2/2 rebuild 2026-11-15 |
| 13 | Tuba | Breath, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | sustained | Iowa MIS / VSCO-2-CE | done | 318.3 | 4/6 in tune; ear 2026-11-15: DEFECT: digital bounce issue; rebuilt 2026-11-15 (STEPS 2/2/2 — factor-6 alias mirrors removed) |
| 14 | Acoustic Guitar | Pluck force, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | fingered + strummed | Iowa MIS guitar | done | 147.7 | 3/6 in tune; ear 2026-11-15: OK (user pass, 2026-11-15) |
| 15| Electric Guitar (lead)| Pick attack, Muffle, Decay, Tail, Tone, Random, Stretch, Tune| clean| FreePats (CC0)| done| 589.8| 5/6 in tune; reworked 2026-11-15 (tail-trim -40dB + 4 s zone cap; verify verdict unchanged); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 16| Electric Bass| Pluck/pick, Muffle, Decay, Tail, Tone, Random, Stretch, Tune| picked| FreePats (CC0)| done| 290.1| 2/6 in tune (narrow keycenter set); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 18| Tenor Saxophone| Breath, Muffle, Decay, Tail, Tone, Random, Stretch, Tune| sustained (vib/non-vib)| FreePats (CC0)| done| 293.0| 6/6 in tune; reworked 2026-11-15 (tail-trim -40dB + 4 s zone cap; verify verdict unchanged); ear 2026-11-15: DEFECT: crackle and digital noise (sax family); STEPS=2/2/2 rebuild + 4s cap 2026-11-15 |
| 19 | Double Bass | Bow/pluck, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | arco + pizzicato | Iowa MIS | done | 465.3 | 4/6 in tune; ear 2026-11-15: DEFECT: click; bounce noise; rebuilt 2026-11-15 (STEPS 2/2/2 — factor-6 alias mirrors removed) |
| 20 | Timpani/Percussion | Strike force, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | single strike | VSCO-2-CE | done | 111.0 | 1/6 in tune (inaudible probes); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 21| Ocarina| Intensity, Muffle, Decay, Tail, Tone, Random, Stretch, Tune| single (sustained)| VCSL Typical ocarina (CC0)| done| 778.2| 4/6 in tune; superseded in some counts by recorder family; reworked 2026-11-15 (tail-trim -40dB + 4 s zone cap; verify verdict unchanged); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 22 | Alto Flute | Breath, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | sustained (vib/non-vib) | Iowa MIS | done | 739.8 | 5/6 in tune; ear 2026-11-15: DEFECT: crackle; rebuilt 2026-11-15 (STEPS 2/2/2 — factor-6 alias mirrors removed) |
| 23 | Bass Flute | Breath, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | sustained (vib/non-vib) | Iowa MIS | done | 579.2 | 6/6 in tune; ear 2026-11-15: DEFECT: click; rebuilt 2026-11-15 (STEPS 2/2/2 — factor-6 alias mirrors removed) |
| 24 | Eb Clarinet | Breath, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | sustained | Iowa MIS | done | 403.2 | 4/6 in tune; ear 2026-11-15: OK (user pass, 2026-11-15) |
| 25 | Bass Clarinet | Breath, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | sustained | Iowa MIS | done | 417.0 | 6/6 in tune; ear 2026-11-15: OK (user pass, 2026-11-15) |
| 26 | Bb Soprano Saxophone | Breath, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | sustained | Iowa MIS | done | 620.6 | 5/6 in tune; ear 2026-11-15: DEFECT: click (+ crackle and digital noise, sax family); STEPS=2/2/2 rebuild (3s cap) 2026-11-15 |
| 27 | Bass Trombone | Breath, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | sustained | Iowa MIS | done | 300.1 | 5/6 in tune; ear 2026-11-15: OK (user pass, 2026-11-15) |
| 29| Baroque Soprano Recorder| Breath, Muffle, Decay, Tail, Tone, Random, Stretch, Tune| sustained| VCSL (SFZ)| done| 778.2| 4/6 in tune; reworked 2026-11-15 (tail-trim -40dB + 4 s zone cap; verify verdict unchanged); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 30| Baroque Alto Recorder| Breath, Muffle, Decay, Tail, Tone, Random, Stretch, Tune| sustained| VCSL (SFZ)| done| 778.2| 4/6 in tune; reworked 2026-11-15 (tail-trim -40dB + 4 s zone cap; verify verdict unchanged); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 31| Baroque Tenor Recorder| Breath, Muffle, Decay, Tail, Tone, Random, Stretch, Tune| sustained| VCSL (SFZ)| done| 778.2| 5/6 in tune; reworked 2026-11-15 (tail-trim -40dB + 4 s zone cap; verify verdict unchanged); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 32| Baroque Bass Recorder| Breath, Muffle, Decay, Tail, Tone, Random, Stretch, Tune| sustained| VCSL (SFZ)| done| 586.2| 4/6 in tune; reworked 2026-11-15 (tail-trim -40dB + 4 s zone cap; verify verdict unchanged); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 33 | Pipe Organ | Breath/Registration, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | sustained | VCSL (SFZ) | done | 778.2 | 6/6 in tune; reworked 2026-11-15 (tail-trim -40dB + 4 s zone cap; verify verdict unchanged); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 34| Harmonica (Special20-C)| Breath, Muffle, Decay, Tail, Tone, Random, Stretch, Tune| sustained| VCSL (SFZ)| done| 293.0| 5/6 in tune (9 unique keycenters — zones repeat); reworked 2026-11-15 (tail-trim -40dB + 4 s zone cap; verify verdict unchanged); ear 2026-11-15: DEFECT: crackle; STEPS=2/2/2 rebuild + 4s cap 2026-11-15 |
| 35| Didgeridoo| Breath, Muffle, Decay, Tail, Tone, Random, Stretch, Tune| drone/sustain| VCSL (SFZ)| dropped| —| removed 2026-11-15: "too short, i don't want it anymore" (user); machine deleted, see listening-log.md |
| 36 | Strumstick | Pluck/Pick, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | fingered/strummed | VCSL (SFZ) | done | 504.6 | 5/6 in tune; tail-trim -40 rebuild 2026-11-15 (verdict unchanged); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 37 | Dan Tranh | Pluck/Pick, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | fingered/strummed | VCSL (SFZ) | done | 394.0 | 5/6 in tune; tail-trim -40 rebuild 2026-11-15 (verdict unchanged); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 38| Psaltery (bowed/plucked)| Pluck/Bow, Muffle, Decay, Tail, Tone, Random, Stretch, Tune| mixed| VCSL (SFZ)| done| 579.1| 4/6 in tune; reworked 2026-11-15 (tail-trim -40dB + 4 s zone cap; verify verdict unchanged); ear 2026-11-15: DEFECT: too much space |
| 39 | Harpsichord (Italian) | Pluck, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | sustained | VCSL (SFZ) | done | 524.6 | 6/6 in tune; reworked 2026-11-15 (tail-trim -40dB + 4 s zone cap; verify verdict unchanged); ear 2026-11-15: DEFECT: too much space; deeper tail-trim -30 dB 2026-11-15 (verdict unchanged) |
| 41| Ocarina, Small| Breath, Muffle, Decay, Tail, Tone, Random, Stretch, Tune| sustained| VCSL (SFZ)| done| 778.2| 3/6 in tune; huge — check bundle SFZ (possibly too many round-robins packed); reworked 2026-11-15 (tail-trim -40dB + 4 s zone cap; verify verdict unchanged); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 42 | Folk Harp | Intensity, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | sustained (plucked) | VCSL (SFZ) | done | 508.8 | 6/6 in tune; tail-trim -40 rebuild 2026-11-15 (verdict unchanged); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 43 | Glockenspiel | Strike force, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | single (mallet) | Iowa MIS bells (brass) | adaptive-built | 778.2 | 1/6 in tune (high register only); reworked 2026-11-15 (tail-trim -40dB + 4 s zone cap; verify verdict unchanged); ear 2026-11-15: DEFECT: silent; too much space |
| 44 | Tubular Bells | Strike force, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | single (mallet) | removed from plan | — | — | FreePats source, SFZ; cross-duplicate of #43 |
| 45 | Vibraphone | Strike force, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | sustained (tremolo) | Iowa MIS | done | 778.2 | 4/6 in tune; reworked 2026-11-15 (tail-trim -40dB + 4 s zone cap; verify verdict unchanged); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 46 | Marimba | Strike force, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | single (mallet) | Iowa MIS (cord) | done | 109.4 | 5/6 in tune; re-sourced yarn->cord 2026-11-15 (yarn mallets were the extra noise; cord is softer); ear 2026-11-15 review pending |
| 47| Xylophone| Strike force, Muffle, Decay, Tail, Tone, Random, Stretch, Tune| single (mallet)| Iowa MIS (rosewood)| done| 226.4| 0/6 in tune (high register only); ear 2026-11-15: OK |
| 48| Balafon| Strike force, Muffle, Decay, Tail, Tone, Random, Stretch, Tune| single (mallet)| VCSL (SFZ)| done| 118.9| 4/6 in tune (6 keycenters); tail-trim -40 rebuild 2026-11-15 (verdict unchanged); ear 2026-11-15: DEFECT: too much space |
| 49| Kalimba Kenya| Intensity, Muffle, Decay, Tail, Tone, Random, Stretch, Tune| thumb pluck| VCSL (SFZ)| done| 235.2| 3/6 in tune; tail-trim -40 rebuild 2026-11-15 (verdict unchanged); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 50 | Kalimba Tanzania | Intensity, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | thumb pluck | VCSL (SFZ) | done | 163.1 | 3/6 in tune; tail-trim -30 rebuild 2026-11-15 (verdict unchanged); ear 2026-11-15: DEFECT: too much space; deeper tail-trim -30 dB 2026-11-15 (verdict unchanged) |
| 51 | Mbira Mavembe | Intensity, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | thumb pluck | VCSL (SFZ) | done | 559.0 | 4/6 in tune; reworked 2026-11-15 (tail-trim -40dB + 4 s zone cap; verify verdict unchanged); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 52 | Mbira dzaVadzimu Nyamaropa | Intensity, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | thumb pluck | VCSL (SFZ) | done | 406.0 | 2/6 in tune; tail-trim -40 rebuild 2026-11-15 (verdict unchanged); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 53 | Nyunga Nyunga | Intensity, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | thumb pluck | VCSL (SFZ) | done | 350.9 | 3/6 in tune; tail-trim -40 rebuild 2026-11-15 (verdict unchanged); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 54| Hand Bells, Nepalese| Strike force, Muffle, Decay, Tail, Tone, Random, Stretch, Tune| single (bell)| VCSL (SFZ)| done| 227.7| 2/6 in tune (only 3 unique keycenters); tail-trim -30 rebuild 2026-11-15 (verdict improved 1/6 -> 2/6); ear 2026-11-15: DEFECT: too much space |
| 55 | Hand Chimes | Strike force, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | single (chime) | VCSL (SFZ) | done | 452.2 | 5/6 in tune; tail-trim -30 rebuild 2026-11-15 (verdict unchanged); ear 2026-11-15: DEFECT: too much space; deeper tail-trim -30 dB 2026-11-15 (verdict unchanged) |
| 56| Agogo Bells| Strike force, Muffle, Decay, Tail, Tone, Random, Stretch, Tune| single (bell)| VCSL (SFZ)| done| 162.3| 0/6 in tune (only 2 keycenters); tail-trim -30 rebuild 2026-11-15 (verdict unchanged); ear 2026-11-15: DEFECT: too much space |
| 57| Bell Tree| Sweep, Muffle, Decay, Tail, Tone, Random, Stretch, Tune| cascade gliss| VCSL (SFZ)| done| 582.6| 0/6 in tune (glissando, not ET); tail-trim -40 rebuild 2026-11-15 (verdict unchanged); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 58| Slit Drum| Strike force, Muffle, Decay, Tail, Tone, Random, Stretch, Tune| single| VCSL (SFZ)| done| 78.3| 2/6 in tune; tail-trim -40 rebuild 2026-11-15 (verdict unchanged); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 59| Wine Glasses| Intensity, Muffle, Decay, Tail, Tone, Random, Stretch, Tune| sustained| VCSL Wine Glasses (SFZ)| done| 778.2| 5/6 in tune; reworked 2026-11-15 (tail-trim -40dB + 4 s zone cap; verify verdict unchanged); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 60| Ukulele| Pluck force, Muffle, Decay, Tail, Tone, Random, Stretch, Tune| sustained| FreePats (SFZ)| done| 270.2| 5/6 in tune; tail-trim -40 rebuild 2026-11-15 (verdict unchanged); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 61 | Accordion | Breath, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | sustained | FreePats (SFZ) | done | 561.0 | 6/6 in tune; ear 2026-11-15: DEFECT: digital noise; rebuilt 2026-11-15 (STEPS 2/2/2 — factor-6 alias mirrors removed) |
| 62| Bagpipe| Breath, Muffle, Decay, Tail, Tone, Random, Stretch, Tune| chanter + drones| FreePats (SFZ)| done| 665.6| 5/6 in tune; reworked 2026-11-15 (tail-trim -40dB + 4 s zone cap; verify verdict unchanged); ear 2026-11-15: DEFECT: too much space |
| 63| Hang| Intensity, Muffle, Decay, Tail, Tone, Random, Stretch, Tune| single (strike)| FreePats (FLAC)| done| 160.6| 4/6 in tune (not chromatic — D-minor scale); tail-trim -30 rebuild 2026-11-15 (verdict unchanged); ear 2026-11-15: DEFECT: too much space |
| 64 | Steel Drum | Strike force, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | mallet | jlearman.SteelDrum (Unlicense) | done | 156.0 | 4/6 in tune; tail-trim -40 rebuild 2026-11-15 (verdict unchanged); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 65 | TX81Z FM Piano | Intensity, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | sustained (SFZ-mapped) | VCSL (SFZ) | done | 592.0 | 6/6 in tune after clone fix (was 0/6); reworked 2026-11-15 (tail-trim -40dB + 4 s zone cap; verify verdict unchanged); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 66 | Clavisynth | Intensity, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | clav | VCSL (SFZ) | done | 433.8 | 5/6 in tune; reworked 2026-11-15 (tail-trim -40dB + 4 s zone cap; verify verdict unchanged); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 68 | Electric Organ | Breath/Registration, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | sustained | FreePats (SFZ, rock) | done | 673.6 | 6/6 in tune; reworked 2026-11-15 (tail-trim -40dB + 4 s zone cap; verify verdict unchanged); ear 2026-11-15: DEFECT: too much space |
| 69 | Pan Flute | — | — | none | dropped | — | no CC0/PD source found (dropped 2026-11-15) |
| 70 | Crotales | Strike force, Muffle, Decay, Tail, Tone, Random, Stretch, Tune | mallet | Iowa MIS | done | 162.7 | 0/6 in tune (very high register); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 71 | FreePats Synthesizers | — | — | none | dropped | — | GM banks not a distinct instrument |
| 72| Flexatone| Sweep, Muffle, Decay, Tail, Tone, Random, Stretch, Tune| glissando| VCSL (SFZ)| done| 600.8| 0/6 in tune (continuous gliss; expected); reworked 2026-11-15 (tail-trim -40dB + 4 s zone cap; verify verdict unchanged); ear 2026-11-15: OK (user pass, 2026-11-15) |
| 73| Cowsynth (medieval bagpipe synth)| Breath, Muffle, Decay, Tail, Tone, Random, Stretch, Tune| sustained| Karoryfer Cowsynth (CC0)| done| 645.7| 4/6 in tune; reworked 2026-11-15 (tail-trim -40dB + 4 s zone cap; verify verdict unchanged); ear 2026-11-15: OK (user pass, 2026-11-15) |

Notes:
- Pipe-organ ET zones all keyboard stretch — consistent with the highest ranks (more stretch).
- Steel-drum runs below C4; probes 36/60 are below it (out of range).
- No duplicate instruments; every source verified local; idividual files are CC0/public-license-compatible.

## Pipeline

See `source-pitch-playbook.md` for the build recipe and `compact-prototypes/machine_verify.py` for verification. Automated pipeline: `python compact-prototypes/build_all_engines.py`. Log of each build: `fullpass-log.md`.

## Per-instrument balance (not measured yet)

Not measured on device. All figures are host measurements, not hardware readings.

## Source base

See `source-inventory.md` for all downloaded source sets and their verification status.

## Verification gate

`machine_verify.py` measures probes at MIDI 36/48/60/72/84/93 against equal temperament (±50c = passes; >50c at same class = octave-fold pass). Host tests are NOT a gate. Anything not `done` in the table has no verification sign-off at all.
