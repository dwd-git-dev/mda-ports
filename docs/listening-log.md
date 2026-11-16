# Listening Review — User Ear Pass — 2026-11-15

Method: the user listened to the MIDI 60 (C4) renders (`midi60-samples/*_midi60.wav`, 4.0 s each, rendered via the engine host). Verdicts are verbatim user comments. Anything without a comment is fine ("If i don't comment then it is fine").

**Everything NOT listed here was fine to the user's ears (manual sign-off).**

## Class A — "too much space" (trailing silence — easy to tune, separate list)

User: trailing silence that can be tuned, not a mega issue. Keep these out of the signal-defect triage; handle as one tunable parameter family later.

- agogo-bells — too much space
- bagpipe — too much space
- balafon — too much space
- bells — too much space (also reported silent)
- electric-organ — too much space
- hand-bells-nepalese — too much space
- hand-chimes — too much space
- hang — too much space
- harpsichord-italian — too much space
- kalimba-tanzania — too much space
- psaltery — too much space

Interpretation note (ours, unconfirmed): likely the source's dead/quiet tail being carried into the built sample. A deeper tail trim (lower than −40 dB vs peak, or shortening the kept tail) or an `find_loop` max-length change may reduce it. Not one of the 27 reworked machines in most cases (harpsichord-italian, kalimba-tanzania, psaltery are), so it is not explained solely by our 4 s caps.

## Class B — signal defects: crackle / digital noise / click / bounce (systematic investigation)

User: work out systematically whether one high-level cause explains these rather than fixing per engine.

- accordion — digital noise
- alto-flute — crackle
- saxophones — crackle and digital noise ("also sax"; applied to and tenor-saxophone, presumed; soprano-sax has its own note below)
- clarinet — crackle and digital noise (also "clarinet digital noise")
- bass-flute — click
- — crackle and noise
- trombone — click; digital noise
- double-bass — click; bounce noise
- flute — loud digital noise
- — crackle and digital noise
- harp — noise
- harmonica — crackle
- marimba — extra noise
- oboe — loud digital noise
- — click
- soprano-sax — click (+ crackle and digital noise, sax family)
- trumpet — crackle
- tuba — digital bounce issue
- violin-pizz — bounce, maybe
- bells — silent

Systematic investigation: see `compact-prototypes/ear_diagnose.py` and the findings in `issues-log.md` (2026-11-15 ear pass section). Not yet root-caused as of writing.

## Removed — didgeridoo

2026-11-15: user requested full removal ("too short, i don't want it anymore"). Machine deleted (working tree, release, midi60 render, sampling notes). Catalog now **69 machines**. Recorded in `instrument-build-log.md` (row 35, dropped) and `issues-log.md`.

## Ambiguous / not yet interpreted

- "also sax crackle and digital noise" — which saxophones exactly (all, or alto+tenor only). Logged for the whole family with soprano-sax overridden to its own comment.
- "bass bounce noise" — double-bass or electric-bass; logged under double-bass (adjacent to the double-bass click comment).
- "trobone digital noise" vs "trombone click" — both logged under trombone; two remarks from the same session, could be two symptoms on one machine.

## Standing rules (unchanged)

- Piano stays bit-identical, untouched.
- The 12 SFZ clones failing host tests rc=26/32 remain out of scope unless the user directs otherwise.
- No source-file modifications.

## Corrections to earlier entries

- xylophone - good (user explicitly fine).

## Update 2026-11-15 evening (session 8) - user ear pass on the re-rendered set - measured, verdict

User ear-pass after sessions 5/6/7 flagged these machines. Measurements confirm the artifacts are *in the source recordings*, not in our engine builds:

mda-bass-clarinet (noise at end), (static noise middle/end), cello (digital noise middle), electric-organ (static at start), harp (click/noise start), (crackle at start), marimba (bounce-like sound), (light crackle near start), pipe-organ (light crackle near start), (crackle at start), violin-pizz (bounce noise after), flute (crackle in middle), (crackle at start).

**All 13 pass machine_verify (ET-tuned, no clipping, no discontinuities heavier than piano's reference). The noise/crackle/bounce the user hears is in the source wav files themselves** (MIS's ff/mf takes are noisy, and play themselves faithfully).

See `issues-log.md` session 8 for the full measurements. Action: no fix attempted. If the user wants any de-noised, it requires either content edits to the sources (not allowed) or re-sourcing those from VCSL/FreePats/Karoryfer (a re-pass, not a fix).

## Update 2026-11-15 evening (session 5 + 6 + 7) - fresh ear pass pending

The render defects the user flagged (tuba "second peak", octave-high tuba) were traced to engine/builder defects, not to the sources themselves. The following changed materially since your last pass:

- **Spawn-fix (session 5)** — all 68 non-piano machines now start playback at the correct per-segment rate (see `issues-log.md` session 5; `fullpass-log.md`). Machines with at least one zone having an empty leading segment (33 machines incl. tuba, cello, marimba) now play at true speed and true octave. Listen to these first.
- **Loop re-entrance ghost (session 7)** — mda-tuba, mda-electric-bass,: the "second peak" the user reported just before 2 seconds is gone; the note now dies to silence. The electric-bass no longer feeds back to -31 dB at 0.74 s.
- **slit-drum remap (session 6)** — drums re-mapped to sounding pitches; auto probe reads A2 +0.5 c, D#3 +20 c.
- **mda-violin-pizz** — its bump just before 2 seconds is the source's own second hump (the plucked string's body resonance), not a defect; kept bit-identical to the build that verified 5/6.

**No machine in these builds has an ear pass yet. All 69 MIDI 60 renders in `midi60-samples/` have been re-rendered with the fixed engines.** Please re-listen, starting with tuba (your original report) and the flagged class-B list, and tell me what you hear. Piano was not touched; all verdicts re-verified at their pre-fix values.

## Update 2026-11-15 evening

Class A pass was executed 2026-11-15 evening with --tail-trim -30; outcome in `fullpass-log.md` and `instrument-build-log.md`. balafon reverted to its prior -40 rebuild after -30 regressed the verification gate. Bagpipe, electric-organ, bells could not be tuned (no trailing silence above -60 dB to trim); bells row was re-diagnosed: MIS source has no notes below MIDI 72; "silent" = out-of-register, not a defect — see issues-log.md #8. Awaiting user ear pass on the 7 rebuilt machines.
