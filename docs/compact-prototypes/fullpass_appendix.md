# Fullpass-log appendix - 2026-11-15 session 5+6 (octave-high spawn bug, slit-drum)

## 2026-11-15 evening (session 5 + 6): ROOT CAUSE of the clicks - spawn at a segment boundary never divides the rate

User bug report: tuba render "audible gap/clicks at 0.9-1.1 s" - YIN on the render read **529 Hz against a 264 Hz source** (exactly one octave high), and the apparent dip was the loop point no longer aligned with the source's own dip.

**Root cause (measured):** in `initialize_note` (all machines, cloned from piano template):

    if (voice.position >= voice.b3) voice.delta = voice.delta / voice.f3;
    else if (voice.position >  voice.b2) voice.delta = voice.delta / voice.f2;
    else if (voice.position >  voice.b1) voice.delta = voice.delta / voice.f1;

The two strict `>` never fire when spawn happens exactly at a segment boundary (segment 1 empty -> `b1 == zone.start == voice.position`). The voice advances the stored (decimated) samples at full rate -> 2x the source pitch, loops at the wrong rate forever. piano safe (spawn is always inside segment 1). **octave-fold in machine_verify hid it everywhere** (an octave-up pitch lands on the same ET row).

**Scope:** 33 machines ship at least one zone with empty segment 1 (2, bass-flute 4, bass-trombone 15, 7, bells 2, cello 8, double-bass 10, eb-clarinet 5, electric-bass 13, electric-guitar 7, electric-organ 8, fm-piano 4, folk-harp 1, 7, guitar 7, guitar-old 6, hang 3, harp 7, marimba 12, ocarina 12, pipe-organ 3, 7, slit-drum 8, steel-drum 11, timpani 15, tuba 12, ukulele 3, vibraphone 11, viola 11, violin-pizz 4, wine-glasses 7).

**Fix:** spawn comparisons `>` -> `>=` in `engine.cpp` for all 68 non-piano machines (piano untouched, bit-identical rule; its b1 > start everywhere, so identical audio either way). `build_instrument_data.py` now applies the same patch at clone time. `render()` mid-playback divide unchanged (spawn fix divides exactly once; crossings then behave).

**Verification (full 69-machine re-run, `compact-prototypes/verify_full.log`):**

- tuba: 48/60/72 now 133.5/264.8/530.2 Hz, all in tune (was 267/530/1060); probes 84/93 out of tune BY DESIGN (source only reaches C4; engine clamps at root+15 semis). midi60 re-rendered (YIN 264.5/264.7/265.3/264.6 Hz).
- harp 5/6 -> 6/6; steel-drum 4/6 -> 5/6; all others unchanged (incl. all machines with non-empty segment 1 everywhere - bit-identical audio).
- slit-drum (below): 2/6 -> 1/6 -> REMAPPED -> 3/6.
- no new snap>BAD, no silence/discontinuity flags; all snap>WARN counts pre-existing.

**slit-drum deficit family (session 6):** VCSL Slit Drum SFZ is generated `--unpitched` (pitch_keycenter = placeholder, NOT an ET claim). Source WAV files are STEREO; naive mono reads interleave L,R and mirror everything to half true frequency - all "78/54 Hz" source reads were artefacts. Correct stereo->mono reads: LogDrumLo ~ 109 Hz (ET A2 = 110, in tune), LogDrumHi ~ 157 Hz (ET D#3 = 155.6, in tune). Mapping retargeted to sounding ET roots (Lo -> 45, Hi -> 51) and rebuilt: auto probes A2 +0.5c, D#3 +20c - in tune at its own pitches; default probes 36/48/60 in tune (72+ clamps - unsupported register). Backup mappings: `pristine-samples/slit-drum/mapping.json.pitch60-backup` (original), `mapping.json.root39-backup` (first re-map attempt; abandoned).

**midi60-samples/ re-rendered for all 69 machines** with the fixed engines (69/69 OK; accordion rebuilt at 8.0 s + cosine fade last 1 s + 1.5 s silence tail = 9.5 s, same as ear-passed). ~672 KB each, 47.4 MiB total.

**release/ refreshed:** 68 engine.cpp (spawn fix), slit-drum full machine (remap; sdk compile 218/2032 KiB). piano byte-identical in both places. docs mirrored.

**What this means for ear class B:** the single dominant mechanism behind the "crackle/bounce/digital" flags on the flagged winds/brass/harmonica family - the octave-plus-wrong-rate playback - is gone everywhere. Class B cannot be declared closed from this alone (some machines share the class for source reasons, e.g. MIS breath-noise), but the structural engine bug is fixed; a fresh ear-pass on midi60-samples/, family by family, is the next gate. Tuba specifically should now match the user's own source reference.

WARNING: no instrument except piano has an ear pass on THESE builds yet - the render checks pitch, silence and discontinuities only.
