# Source inventory — raw libraries for the full instrument pass

Generated 2026-11-15 by `compact-prototypes/inventory_sources.py` from `workspace\sources\`.
Lists **downloaded, unmodified** bytes only; builds read these but never write here.
Naming: source URLs in `compact-prototypes/download_sources.py`. Case conventions vary by library — do not rename files in this tree.

## Global summary

| Set | Audio files | Size | Format | Rate | Bits | Ch | Single notes | Sweeps | Special |
|---|---|---|---|---|---|---|---|---|---|
| freepats/accordion | 34 | 13 MB | WAV:34 | 44100:34 | 16:34 | 2:34 | 34 | 0 | 0 |
| freepats/bagpipe | 26 | 37 MB | WAV:26 | 44100:26 | 24:26 | 1:26 | 26 | 0 | 0 |
| freepats/electric-organ-drawbar | 16 | 12 MB | WAV:16 | 44100:16 | 16:16 | 1:16 | 16 | 0 | 0 |
| freepats/electric-organ-rock | 32 | 25 MB | WAV:32 | 44100:32 | 16:32 | 1:32 | 0 | 0 | 32 |
| freepats/glass | 152 | 61 MB | WAV:152 | 48000:152 | 24:152 | 1:152 | 147 | 5 | 0 |
| freepats/hang | 52 | 50 MB | FLAC:51, ?:1 | 44100:51, ?:1 | 24:51, ?:1 | 2:51, ?:1 | 52 | 0 | 0 |
| freepats/ukulele | 15 | 7 MB | WAV:15 | 48000:15 | 24:15 | 1:15 | 13 | 0 | 2 |
| iowa-mis/alto_flute | 11 | 58 MB | AIFF:11 | 44100:11 | 16:11 | 1:11 | 0 | 11 | 0 |
| iowa-mis/bass_clarinet | 12 | 87 MB | AIFF:12 | 44100:12 | 16:12 | 1:12 | 0 | 12 | 0 |
| iowa-mis/bass_flute | 10 | 68 MB | AIFF:10 | 44100:10 | 16:10 | 1:10 | 0 | 10 | 0 |
| iowa-mis/bass_trombone | 12 | 68 MB | AIFF:12 | 44100:12 | 16:12 | 1:12 | 0 | 12 | 0 |
| iowa-mis/bells | 12 | 582 MB | AIFF:12 | 44100:12 | 24:12 | 2:12 | 0 | 12 | 0 |
| iowa-mis/crotales | 50 | 118 MB | AIFF:50 | 44100:50 | 24:50 | 2:48, 1:2 | 50 | 0 | 0 |
| iowa-mis/eb_clarinet | 14 | 46 MB | AIFF:14 | 44100:14 | 16:14 | 1:14 | 1 | 12 | 1 |
| iowa-mis/marimba | 84 | 2769 MB | AIFF:84 | 96000:84 | 24:84 | 2:84 | 14 | 70 | 0 |
| iowa-mis/soprano_sax_novib | 12 | 54 MB | AIFF:12 | 44100:12 | 16:12 | 1:12 | 0 | 12 | 0 |
| iowa-mis/soprano_sax_vib | 12 | 56 MB | AIFF:12 | 44100:12 | 16:12 | 1:12 | 0 | 12 | 0 |
| iowa-mis/vibraphone | 40 | 1849 MB | AIFF:40 | 44100:40 | 24:40 | 2:40 | 0 | 40 | 0 |
| iowa-mis/xylophone | 62 | 366 MB | AIFF:62 | 44100:62 | 24:62 | 2:62 | 12 | 48 | 2 |
| sfzinstruments/cowsynth | 40 | 28 MB | WAV:40 | 44100:40 | 16:40 | 1:40 | 35 | 0 | 5 |
| sfzinstruments/steel-drum | 368 | 79 MB | FLAC:368 | 44100:368 | 16:368 | 2:368 | 368 | 0 | 0 |
| vcsl/vcsl | 4249 | 9736 MB | WAV:4231, ?:18 | 44100:3773, 48000:458, ?:18 | 24:2729, 16:1502, ?:18 | 2:3697, 1:534, ?:18 | 3066 | 23 | 1160 |

## VCSL folder map (candidate # -> path under VCSL-1.2.2 tree)

VCSL keeps sample folders per instrument AND patch files beside them (`Aerophones/<family>/<name> - <Articulation>.sfz`).
| # | Instrument | Folder | Samples | Patch SFZ files |
|---|---|---|---|---|
| 29 | Baroque Soprano Recorder | `Aerophones/Edge-blown Aerophones/Baroque Soprano Recorder` | 25 | 3 |
| 30 | Baroque Alto Recorder | `Aerophones/Edge-blown Aerophones/Baroque Alto Recorder` | 36 | 4 |
| 31 | Baroque Tenor Recorder | `Aerophones/Edge-blown Aerophones/Baroque Tenor Recorder` | 39 | 4 |
| 32 | Baroque Bass Recorder | `Aerophones/Edge-blown Aerophones/Baroque Bass Recorder` | 41 | 4 |
| 33 | Pipe Organ | `Aerophones/Edge-blown Aerophones/Pipe Organ` | 65 | 5 |
| 34 | Harmonica (Hohner Special20-C) | `Aerophones/Free Aerophones/Harmonica-Hohner-Special20-C` | 58 | 4 |
| 35 | Didgeridoo | `Aerophones/Lip Aerophones/Didgeridoo` | 12 | 1 | (machine REMOVED 2026-11-15 at user request)
| 36 | Strumstick | `Chordophones/Composite Chordophones/Strumstick` | 57 | 1 |
| 37 | Dan Tranh | `Chordophones/Zithers/Dan Tranh` | 115 | 6 |
| 38 | Psaltery | `Chordophones/Zithers/Psaltery, Bowed and Plucked` | 33 | 4 |
| 39 | Harpsichord (Italian) | `Chordophones/Zithers/Harpsichord, Italian` | 63 | 1 |
| 41 | Ocarina, Small | `Aerophones/Edge-blown Aerophones/Ocarina, Small` | 23 | 3 |
| 42 | Folk Harp | `Chordophones/Composite Chordophones/Folk Harp` | 58 | 1 |
| 43 | Glockenspiel | `Idiophones/Struck Idiophones/Glockenspiel` | 18 | 1 |
| 44 | Tubular Bells | `Idiophones/Struck Idiophones/Tubular Bells 1` | 18 | 1 |
| 45 | Vibraphone | `Idiophones/Struck Idiophones/Vibraphone` | 50 | 4 |
| 46 | Marimba | `Idiophones/Struck Idiophones/Marimba` | 30 | 1 |
| 47 | Xylophone | `Idiophones/Struck Idiophones/Xylophone` | 48 | 4 |
| 48 | Balafon | `Idiophones/Struck Idiophones/Balafon` | 54 | 4 |
| 49 | Kalimba Kenya | `Idiophones/Plucked Idiophones/Kalimba, Kenya` | 15 | 1 |
| 50 | Kalimba Tanzania | `Idiophones/Plucked Idiophones/Kalimba, Tanzania` | 27 | 1 |
| 51 | Mbira Mavembe (Low G) | `Idiophones/Plucked Idiophones/Mbira Mavembe (Gandanga), Zimbabwe, Low G` | 24 | 1 |
| 52 | Mbira dzaVadzimu Nyamaropa (Low B) | `Idiophones/Plucked Idiophones/Mbira dzaVadzimu Nyamaropa, Zimbabwe, Low B` | 24 | 1 |
| 53 | Nyunga Nyunga (Low F) | `Idiophones/Plucked Idiophones/Nyunga Nyunga, Mozambique, Low F` | 19 | 1 |
| 54 | Hand Bells, Nepalese | `Idiophones/Struck Idiophones/Hand Bells, Nepalese` | 3 | 1 |
| 55 | Hand Chimes | `Idiophones/Struck Idiophones/Hand Chimes` | 19 | 1 |
| 56 | Agogo Bells | `Idiophones/Struck Idiophones/Agogo Bells` | 15 | 1 |
| 57 | Bell Tree | `Idiophones/Struck Idiophones/Bell Tree` | 11 | 4 |
| 58 | Slit Drum | `Idiophones/Struck Idiophones/Slit Drum` | 6 | 1 |
| 59 | Wine Glasses | `Idiophones/Friction Idiophones/Wine Glasses` | 16 | 3 |
| 65 | TX81Z FM Piano | `Electrophones/TX81Z/FM Piano` | 66 | 1 |
| 66 | Clavisynth | `Electrophones/TX81Z/Clavisynth` | 57 | 1 |
| 72 | Flexatone | `Idiophones/Struck Idiophones/Flexatone` | 8 | 1 |

## Per-set details

### freepats/accordion — FreePats (CC0)

- Note: Button accordion (Hornby) — not a piano accordion
- Audio files: 34; total bytes 13 MB; total audio duration 39s
- Formats: WAV:34; rates: 44100:34; bits: 16:34; channels: 2:34
- Classification: **34 single-note, 0 range-sweep, 0 special/other**
- Token vocabulary (articulations/dynamics): button×34, accordion×34, hn×34, rel×17, a4×2, a5×2, b3×2, b4×2, b5×2, c5×2, c6×2, d4×2
- SFZ files present: 1
- Build guidance: Button accordion (Hornby HN), not a piano accordion; single-row chromatic layout; SFZ included. Reed detune is inherent to free-reed (beating); ET content still passes machine_verify if measured within tolerance.

### freepats/bagpipe — FreePats (CC0)

- Note: Real French-style pipe (G), recorded 2020 by Gilles Sadowski
- Audio files: 26; total bytes 37 MB; total audio duration 146s
- Formats: WAV:26; rates: 44100:26; bits: 24:26; channels: 1:26
- Classification: **26 single-note, 0 range-sweep, 0 special/other**
- Token vocabulary (articulations/dynamics): 31×13, 32×11, a#4×2, a4×2, b4×2, c#5×2, c5×2, d#5×2, d5×2, drone×2, e5×2, f#5×2
- SFZ files present: 1
- Build guidance: Real G-chanter pipe. Recorded sharp of A4=440 (typical for live pipe); either pitch-correct at build or accept drift and re-verify with machine_verify.py. 2 drones (G2/G3); chanter F4-G6; SFZ included (use SFZ over SF2). Chanter+drones: drones sound continuously — candidate for a dedicated drone knob/base layer, not a plain MDA zone.

### freepats/electric-organ-drawbar — FreePats (CC0)

- Note: Drawbar emulation (synthesis; Hammond-like)
- Audio files: 16; total bytes 12 MB; total audio duration 74s
- Formats: WAV:16; rates: 44100:16; bits: 16:16; channels: 1:16
- Classification: **16 single-note, 0 range-sweep, 0 special/other**
- Token vocabulary (articulations/dynamics): c2×1, c3×1, c4×1, c5×1, c6×1, c7×1, e2×1, e3×1, e4×1, e5×1, e6×1, g#2×1
- SFZ files present: 1
- Build guidance: Drawbar organ emulation (synthesized, Hammond-like) — strong alternative electric-organ character.

### freepats/electric-organ-rock — FreePats (CC0)

- Note: Rock organ emulation (synthesis, not sampled)
- Audio files: 32; total bytes 25 MB; total audio duration 154s
- Formats: WAV:32; rates: 44100:32; bits: 16:32; channels: 1:32
- Classification: **0 single-note, 0 range-sweep, 32 special/other**
- Token vocabulary (articulations/dynamics): c2l×1, c2r×1, c3l×1, c3r×1, c4l×1, c4r×1, c5l×1, c5r×1, c6l×1, c6r×1, c7l×1, c7r×1
- SFZ files present: 1
- Build guidance: Rock organ emulation (synthesized, not sampled); cheap, ET-safe, sounds like a rock combo organ.

### freepats/glass — FreePats (CC0)

- Note: Water glasses, chromatic A4-G6
- Audio files: 152; total bytes 61 MB; total audio duration 279s
- Formats: WAV:152; rates: 48000:152; bits: 24:152; channels: 1:152
- Classification: **147 single-note, 5 range-sweep, 0 special/other**
- Example sweep files: A#4-B4_01.wav, A#4-B4_02.wav, A#4-B4_03.wav, A#4-B4_04.wav, A#4-B4_05.wav
- Token vocabulary (articulations/dynamics): 01×20, 02×20, 03×20, 04×20, 05×20, 06×19, 07×16, 08×12, b4×11, e5×9, f5×9, f6×9
- SFZ files present: 1
- Build guidance: Water glasses tuned chromatically A4-G6; round-robin layers; CC0; direct alternative for #59 if VCSL wine glasses disappoint.

### freepats/hang — FreePats (CC0)

- Note: Hang in D minor — scale-limited, not chromatic
- Audio files: 52; total bytes 50 MB; total audio duration 150s
- Formats: FLAC:51, ?:1; rates: 44100:51, ?:1; bits: 24:51, ?:1; channels: 2:51, ?:1
- Classification: **52 single-note, 0 range-sweep, 0 special/other**
- Token vocabulary (articulations/dynamics): 01×10, 02×9, 03×9, 04×9, 05×9, d4×7, a3×6, a4×6, e4×6, f4×6, g4×6, 06×5
- SFZ files present: 2
- Build guidance: D minor hang — NOTE-LIMITED to the D minor scale (D4..A4ish), NOT chromatic. Keep only as a limited-scale build or drop; decide before build.

### freepats/ukulele — FreePats (CC0)

- Note: Soprano ukulele
- Audio files: 15; total bytes 7 MB; total audio duration 28s
- Formats: WAV:15; rates: 48000:15; bits: 24:15; channels: 1:15
- Classification: **13 single-note, 0 range-sweep, 2 special/other**
- Token vocabulary (articulations/dynamics): a4×1, a5×1, b4×1, body×1, tap×1, c#5×1, c4×1, c6×1, chuck×1, d#5×1, d4×1, e4×1
- SFZ files present: 1
- Build guidance: Soprano ukulele; concert pitch; SFZ mapping included.

### iowa-mis/alto_flute — Iowa MIS (unrestricted)

- Note: Alto flute; sounds P4 below written
- Audio files: 11; total bytes 58 MB; total audio duration 406s
- Formats: AIFF:11; rates: 44100:11; bits: 16:11; channels: 1:11
- Classification: **0 single-note, 11 range-sweep, 0 special/other**
- Example sweep files: AltoFlute.ff.C4B4.aiff, AltoFlute.ff.C5B5.aiff, AltoFlute.ff.C6G6.aiff, AltoFlute.ff.G3B3.aiff, AltoFlute.mf.C4B4.aiff, AltoFlute.mf.C5B5.aiff, AltoFlute.mf.C6G6.aiff, AltoFlute.mf.G3B3.aiff
- Token vocabulary (articulations/dynamics): altoflute×11, ff×4, mf×4, c4b4×3, g3b3×3, pp×3, c5b5×2, c6g6×2, c5g5×1
- Build guidance: Sounds P4 below written (written G4 = C4). Mono DAT files are octave-ish chromatic sweeps (e.g. .C4B4.); no single-note files in this archive. Treat as pitch-tracking sources at build. pp/mf/ff dynamics present.

### iowa-mis/bass_clarinet — Iowa MIS (unrestricted)

- Note: Bass clarinet; sounds M9 below written
- Audio files: 12; total bytes 87 MB; total audio duration 702s
- Formats: AIFF:12; rates: 44100:12; bits: 16:12; channels: 1:12
- Classification: **0 single-note, 12 range-sweep, 0 special/other**
- Example sweep files: BassClarinet.ff.C3B3.aiff, BassClarinet.ff.C4B4.aiff, BassClarinet.ff.C5Bb5.aiff, BassClarinet.ff.Db2B2.aiff, BassClarinet.mf.B2Bb3.aiff, BassClarinet.mf.B3Bb4.aiff, BassClarinet.mf.B4Bb5.aiff, BassClarinet.mf.Db2Bb2.aiff
- Token vocabulary (articulations/dynamics): bassclarinet×12, ff×4, mf×4, pp×4, c4b4×2, c5bb5×2, c3b3×1, db2b2×1, b2bb3×1, b3bb4×1, b4bb5×1, db2bb2×1
- Build guidance: Sounds M9 below written (written C4 = Bb3). Sweep convention.

### iowa-mis/bass_flute — Iowa MIS (unrestricted)

- Note: Bass flute; sounds octave below written
- Audio files: 10; total bytes 68 MB; total audio duration 495s
- Formats: AIFF:10; rates: 44100:10; bits: 16:10; channels: 1:10
- Classification: **0 single-note, 10 range-sweep, 0 special/other**
- Example sweep files: BassFlute.ff.C3B3.aiff, BassFlute.ff.C4B4.aiff, BassFlute.ff.C5B5.aiff, BassFlute.ff.C6Db6.aiff, BassFlute.mf.C3B3.aiff, BassFlute.mf.C4B4.aiff, BassFlute.mf.C5Bb5.aiff, BassFlute.pp.C3B3.aiff
- Token vocabulary (articulations/dynamics): bassflute×10, ff×4, c3b3×3, c4b4×3, mf×3, pp×3, c5b5×1, c6db6×1, c5bb5×1, c5e5×1
- Build guidance: Sounds octave below written. Same sweep convention as alto flute.

### iowa-mis/bass_trombone — Iowa MIS (unrestricted)

- Note: Bass trombone; sounds octave below written
- Audio files: 12; total bytes 68 MB; total audio duration 603s
- Formats: AIFF:12; rates: 44100:12; bits: 16:12; channels: 1:12
- Classification: **0 single-note, 12 range-sweep, 0 special/other**
- Example sweep files: BassTrombone.ff. Db1B1.aiff, BassTrombone.ff.C2B2.aiff, BassTrombone.ff.C3B3.aiff, BassTrombone.ff.C4F4.aiff, BassTrombone.mf. Db1B1.aiff, BassTrombone.mf.C2B2.aiff, BassTrombone.mf.C3B3.aiff, BassTrombone.mf.C4G4.aiff
- Token vocabulary (articulations/dynamics): basstrombone×12, ff×4, mf×4, pp×4, c2b2×3, c3b3×3, db1b1×2, c4f4×1, c4g4×1, c1b1×1, c4bb4×1
- Build guidance: Sounds octave below written. Sweep convention.

### iowa-mis/bells — Iowa MIS (unrestricted)

- Note: Orchestral bells (glockenspiel) — brass and plastic
- Audio files: 12; total bytes 582 MB; total audio duration 1362s
- Formats: AIFF:12; rates: 44100:12; bits: 24:12; channels: 2:12
- Classification: **0 single-note, 12 range-sweep, 0 special/other**
- Example sweep files: bells.brass.C5B5.aif, bells.brass.C6B6.aif, bells.brass.C7B7.aif, bells.brass.C8E8.aif, bells.plastic.ff.C5B5.aif, bells.plastic.ff.C6B6.aif, bells.plastic.ff.C7B7.aif, bells.plastic.ff.C8E8.aif
- Token vocabulary (articulations/dynamics): bells×12, plastic×8, brass×4, ff×4, pp×4, c5b5×3, c6b6×3, c7b7×3, c8e8×3
- Build guidance: Brass bells = orchestral glockenspiel character (recommended); plastic = toy-bell character. Two mallet regimes; ff/pp dynamics for plastic, brass zip contents: see extracted names.

### iowa-mis/crotales — Iowa MIS (unrestricted)

- Note: Crotales, pp/ff only (no mf), C6-C8
- Audio files: 50; total bytes 118 MB; total audio duration 473s
- Formats: AIFF:50; rates: 44100:50; bits: 24:50; channels: 2:48, 1:2
- Classification: **50 single-note, 0 range-sweep, 0 special/other**
- Token vocabulary (articulations/dynamics): crotale×50, ff×25, pp×25, a6×2, a7×2, ab6×2, ab7×2, b6×2, b7×2, bb6×2, bb7×2, c6×2
- Build guidance: Crotales C6-C8, PP/FF only — no mf anywhere; normalize gain at build. No sweeps; single notes per file. 2 octaves above written.

### iowa-mis/eb_clarinet — Iowa MIS (unrestricted)

- Note: Eb clarinet; sounds m3 above written
- Audio files: 14; total bytes 46 MB; total audio duration 330s
- Formats: AIFF:14; rates: 44100:14; bits: 16:14; channels: 1:14
- Classification: **1 single-note, 12 range-sweep, 1 special/other**
- Example sweep files: EbClar.ff.C4B4.aiff, EbClar.ff.C5B5.aiff, EbClar.ff.C6A6.aiff, EbClar.ff.G3B3.aiff, EbClar.mf.C4B4.aiff, EbClar.mf.C5B5.aiff, EbClar.mf.C6B6.aiff, EbClar.mf.G3B3.aiff
- Token vocabulary (articulations/dynamics): ebclar×14, mf×5, ff×4, pp×4, c4b4×3, c5b5×3, g3b3×3, ambient×1, silence×1, c6a6×1, c6b6×1, c7×1
- Build guidance: Sounds m3 above written (written C4 = Eb4). Sweep convention.

### iowa-mis/marimba — Iowa MIS (unrestricted)

- Note: Marimba — yarn/cord/rubber/deadstroke/roll mallets
- Audio files: 84; total bytes 2769 MB; total audio duration 3581s
- Formats: AIFF:84; rates: 96000:84; bits: 24:84; channels: 2:84
- Classification: **14 single-note, 70 range-sweep, 0 special/other**
- Example sweep files: Marimba.cord.ff.C2B2.aif, Marimba.cord.ff.C3B3.aif, Marimba.cord.ff.C4B4.aif, Marimba.cord.ff.C5B5.aif, Marimba.cord.ff.C6B6.aif, Marimba.cord.mf.C2B2.aif, Marimba.cord.mf.C3B3.aif, Marimba.cord.mf.C4B4.aif
- Token vocabulary (articulations/dynamics): marimba×84, ff×30, pp×30, mf×24, cord×18, roll×18, rubber×18, yarn×18, c2b2×14, c3b3×14, c4b4×14, c5b5×14
- Build guidance: MARK: 96 kHz / 24-bit / stereo sweeps (resample at build). MALLET MAP: yarn.* = soft yarn mallets (primary, use .mf. as baseline); cord.* = cord mallets; rubber.* = rubber; deadstroke.* = dead strokes; roll.* = tremolo (not ET-mappable as single hits). All C2-C7. Primary build: yarn.mf sweeps.

### iowa-mis/soprano_sax_novib — Iowa MIS (unrestricted)

- Note: Bb soprano sax, non-vibrato set
- Audio files: 12; total bytes 54 MB; total audio duration 375s
- Formats: AIFF:12; rates: 44100:12; bits: 16:12; channels: 1:12
- Classification: **0 single-note, 12 range-sweep, 0 special/other**
- Example sweep files: SopSax.NoVib.ff.Ab3B3.aiff, SopSax.NoVib.ff.C4B4.aiff, SopSax.NoVib.ff.C5B5.aiff, SopSax.NoVib.ff.C6Eb6.aiff, SopSax.NoVib.mf.Ab3B3.aiff, SopSax.NoVib.mf.C4B4.aiff, SopSax.NoVib.mf.C5B5.aiff, SopSax.NoVib.mf.C6Eb6.aiff
- Token vocabulary (articulations/dynamics): sopsax×12, novib×12, ff×4, mf×4, pp×4, ab3b3×3, c4b4×3, c5b5×3, c6eb6×3
- Build guidance: Bb soprano sax non-vibrato; sounds M2 below written. NoVib zip only (vibrato in sibling set).

### iowa-mis/soprano_sax_vib — Iowa MIS (unrestricted)

- Note: Bb soprano sax, vibrato set
- Audio files: 12; total bytes 56 MB; total audio duration 387s
- Formats: AIFF:12; rates: 44100:12; bits: 16:12; channels: 1:12
- Classification: **0 single-note, 12 range-sweep, 0 special/other**
- Example sweep files: SopSax.Vib.ff.Ab3B3.aiff, SopSax.Vib.ff.C4B4.aiff, SopSax.Vib.ff.C5B5.aiff, SopSax.Vib.ff.C6Eb6.aiff, SopSax.Vib.mf.Ab3B3.aiff, SopSax.Vib.mf.C4B4.aiff, SopSax.Vib.mf.C5B5.aiff, SopSax.Vib.mf.C6Eb6.aiff
- Token vocabulary (articulations/dynamics): sopsax×12, vib×12, ff×4, mf×4, pp×4, ab3b3×3, c4b4×3, c5b5×3, c6eb6×3
- Build guidance: Bb soprano sax with vibrato; sounds M2 below written. Vib sibling of novib set.

### iowa-mis/vibraphone — Iowa MIS (unrestricted)

- Note: Vibraphone — sustain/dampen/shortsustain/bow
- Audio files: 40; total bytes 1849 MB; total audio duration 4150s
- Formats: AIFF:40; rates: 44100:40; bits: 24:40; channels: 2:40
- Classification: **0 single-note, 40 range-sweep, 0 special/other**
- Example sweep files: Vibraphone.bow.C3B3.aif, Vibraphone.bow.C4B4.aif, Vibraphone.bow.C5B5.aif, Vibraphone.bow.C6F6.aif, Vibraphone.dampen.ff.C3B3.aif, Vibraphone.dampen.ff.C4B4.aif, Vibraphone.dampen.ff.C5B5.aif, Vibraphone.dampen.ff.C6F6.aif
- Token vocabulary (articulations/dynamics): vibraphone×40, dampen×12, ff×12, mf×12, pp×12, shortsustain×12, sustain×12, c3b3×10, c4b4×10, c5b5×10, c6f6×10, bow×4
- Build guidance: SUSTAIN = standard ringing bars (primary: sustain.mf); dampen.* = damped/staccato; shortsustain.* = short sustain; bow.* = BOWED vibraphone (arco, very long tones) — separate articulation, not ET single notes in chromatic sense? (check); pp/mf/ff.

### iowa-mis/xylophone — Iowa MIS (unrestricted)

- Note: Xylophone — roll + single strokes; glissandi
- Audio files: 62; total bytes 366 MB; total audio duration 1452s
- Formats: AIFF:62; rates: 44100:62; bits: 24:62; channels: 2:62
- Classification: **12 single-note, 48 range-sweep, 2 special/other**
- Example sweep files: xylophone.hardrubber.ff.C5B5.aif, xylophone.hardrubber.ff.C6B6.aif, xylophone.hardrubber.ff.C7B7.aif, xylophone.hardrubber.ff.F4B4.aif, xylophone.hardrubber.mf.C5B5.aif, xylophone.hardrubber.mf.C6B6.aif, xylophone.hardrubber.mf.C7B7.aif, xylophone.hardrubber.mf.F4B4.aif
- Token vocabulary (articulations/dynamics): xylophone×62, hardrubber×30, roll×30, rosewood×30, ff×20, mf×20, pp×20, c5b5×12, c6b6×12, c7b7×12, c8×12, f4b4×12
- Build guidance: No zips on the page; individual aiff sweeps. roll.* = tremolo rolls (not single notes); gliss.up/down = glissandi (not usable); plain .aif = chromatic sweeps (C5B5 etc.); PRIMARY: rosewood.mf single-stroke sweep files.

### sfzinstruments/cowsynth — Karoryfer Cowsynth (CC0-1.0)

- Note: Medieval bagpipe synth, ET-tuned voices
- Audio files: 40; total bytes 28 MB; total audio duration 166s
- Formats: WAV:40; rates: 44100:40; bits: 16:40; channels: 1:40
- Classification: **35 single-note, 0 range-sweep, 5 special/other**
- Token vocabulary (articulations/dynamics): fake×16, cchirp×5, trill×3, g5×3, a2×2, a3×2, a5×2, a6×2, c4×2, c7×2, d4×2, d7×2
- SFZ files present: 17
- Build guidance: Medieval bagpipe rendered as ET oscillator voices; voices: organ/pad/pluck, drones G/A. Synthetic, ET-safe, useful to sanity-check the bagpipe concept through the pipeline.

### sfzinstruments/steel-drum — sfzinstruments/jlearman.SteelDrum (The Unlicense)

- Note: C-tenor pan, C4-B5, 5 vel layers
- Audio files: 368; total bytes 79 MB; total audio duration 723s
- Formats: FLAC:368; rates: 44100:368; bits: 16:368; channels: 2:368
- Classification: **368 single-note, 0 range-sweep, 0 special/other**
- Token vocabulary (articulations/dynamics): steeldrum×203, 1×180, 2×180, jsdb×165, 3×126, 4×100, 3a×42, 065×21, f4×21, 061×18, db4×18, 069×18
- SFZ files present: 2
- Build guidance: C-tenor pan, samples C4-B5 only. Two SFZs: with and without velocity crossfades. Our no-crossfade rule: use jSteelDrum-noxfade.sfz and/or a single velocity layer. Unlooped stereo 16/44.1.

### vcsl/vcsl — VCSL (CC0)

- Note: VCSL 1.2.2-RC, full library
- Audio files: 4249; total bytes 9736 MB; total audio duration 27875s
- Formats: WAV:4231, ?:18; rates: 44100:3773, 48000:458, ?:18; bits: 24:2729, 16:1502, ?:18; channels: 2:3697, 1:534, ?:18
- Classification: **3066 single-note, 23 range-sweep, 1160 special/other**
- Example sweep files: KSHarp_A2_f1.wav, KSHarp_A4_f1.wav, KSHarp_A6_f1.wav, KSHarp_B1_f1.wav, KSHarp_B3_f1.wav, KSHarp_B5_f1.wav, KSHarp_B6_f1.wav, KSHarp_C3_f2.wav
- Token vocabulary (articulations/dynamics): rr1×2414, main×1039, sus×627, rel×574, rr2×518, vl2×425, hit×413, player×371, mid×371, close×353, jhpiano×351, v2×350
- SFZ files present: 183
- Build guidance: VCSL 1.2.2-RC full library (~4k samples). Per-folder SFZ with pitch_keycenter = authoritative ET note; some folders lack SFZ (then measure). Track down: buildable 1-2 voices per folder; many are multipart (full VCSL Pipe Organ etc.) — pick one rank/canvas.

## Sweep-flagged sets (multi-note files, need pitch tracking at build)

- Everything named `*.VIP*` sweeps on the 2012 MIS pages and every file whose basename carries TWO note tokens (e.g. `AltoFlute.ff.C4B4.aiff`, `Marimba.yarn.mf.C2B2.aif`, `xylophone.rosewood.mf.C5B5.aif`).
- Iowa MIS strings are sweeps (documented earlier); mallets follow the same convention: one file per octave-ish range per mallet/dynamic.
- Iowa MIS winds/brass: mono DAT files are octave-ish sweeps (e.g. `AltoFlute.pp.G3B3.aiff`); the `.stereo.zip` archives contain single-note files (e.g. `Flute.nonvib.ff.A4.stereo.aif` in pristine-samples).
- VCSL/FreePats/sfzinstruments: per-note samples with SFZ mapping; pitch via `pitch_keycenter`.
