# Size Rework Campaign — 2026-11-15 (EXECUTED)

Tracks the phased reduction of the Open-XY catalog against the mda-violin size anchor (~801 KiB). Piano (~317 KiB, bit-identical reference) is never touched.

**Phase 1** — the 27 engines **larger than violin**. All now ≤ 778.2 KiB. 62,789 → 17,854 KiB (−71.6%).
**Phase 2** — the **below-violin easy wins** (tail-trim only). 6,745 → 5,000 KiB (−1,745 KiB).
**Phase 3** — 7 machines rebuilt with `--tail-trim -30` (agogo-bells 162.3 KiB, hand-chimes 452.2, hang 160.6, harpsichord-italian 524.6, kalimba-tanzania 163.1, psaltery 579.1, hand-bells-nepalese 227.7); balafon failed -30/-25 and reverted to -40.
**Phase 4** — all 18 class-B engines rebuilt with STEPS=2/2/2 (no factor-6 decimation; the mirror alias removed — oboe HF energy 37% → 0.6%). The five largest plus soprano-sax were capped tighter to fit inside the 778 KiB ceiling. marimba re-sourced yarn → cord mallets to fix the flagged noise.
**Phase 5** (2026-11-15 evening) — all 68 non-piano machines rebuilt with 8 ms raised-cosine onset fade (in `build_instrument_data.py`), fixing the attack-thump defect the user flagged across MIS/FreePats-sourced machines.

**Combined (Phase 1+2 only)**: 46,680 KiB (~45.6 MiB) saved across both passes.
**Phase 4** (2026-11-15, class B crackle/click/digital/bounce): 12 engines rebuilt with STEPS=2/2/2 - no factor-6 decimation, the mirror alias removed; measured 2-10x less HF energy in renders; the 5 candidates that blew past the 778 KiB ceiling were rebuilt with a tight 4 s cap at 16000 stored samples each (~293 KiB), which also fixed the crackle for free. Five candidates (tenor-saxophone, trumpet, harmonica) blew past the 778 KiB ceiling and were reverted to 2/6/6 + cap. marimba re-sourced yarn -> cord mallets. See issues-log.md for HF-alias testing details.

**Phase 3** (evening, class A "too much space", 2026-11-15): 7 machines rebuilt with `--tail-trim -30` — agogo-bells 162.3 KiB, hand-chimes 452.2, hang 160.6, harpsichord-italian 524.6, kalimba-tanzania 163.1, psaltery 560.9, hand-bells-nepalese 227.7 (verdicts unchanged); balafon failed -30/-25 and was reverted to its prior -40 build. bagpipe/electric-organ/bells survive untouched (no trailing silence; see `issues-log.md` and `fullpass-log.md`).
Every rebuilt engine re-passed `machine_verify.py`; **no probe that was in tune went out of tune**. Full per-pass records in `fullpass-log.md`, sizes in `instrument-build-log.md`.

## Why these machines were big (root cause, measured)

Piano stays small because its high band degrades into the factor-6 stage within ~1 s and each zone stores ~17k samples. Sustained winds/organs/mallets never fall below −20 dB in 4–11 kHz, so the 2/6/6 cascade never fires early. Size came from **duration of the sustained region**, never from bit depth, sample rate, zone count or packing — all are already at the piano floor. Source files are concert-pitch 22.05 kHz mono (any >16-bit source was downsampled); nothing there to cut.

Method tools (in `compact-prototypes/`): `zone_profile.py` (per-zone source seconds), `tail_profile.py` (last moment each zone's 93 ms windowed RMS is above −40 dB vs zone peak), `dryrun_trim.py` (exact packed-byte arithmetic without writing files; validated against known builds to 0.1 KiB), plus the builder's `--tail-trim` / `--cap-long` flags.

## Phase 1 — the 27 engines larger than mda-violin

**Rule: applied per engine, never as a blanket setting.** Zones under the limit are stored untouched — “don't truncate a fine sample”.

1. **Measure first.** All 27: zones averaged 5–26 s, all > 2 s; only 4 had dead tails (harpsichord-italian 41%, electric-guitar 35%, mbira-mavembe 32%, psaltery 22%). No filtering could fix the rest — the size *is* real signal.
2. **Pass A — `--tail-trim -40`** (cut zone at last −40 dB point + 250 ms, 200 ms raised-cosine fade). No-op by construction on 23 engines. Wins: harpsichord-italian 1248→937, electric-guitar 1956→1461, psaltery 815→699, mbira-mavembe 828→690, ocarina 1295→1214, pipe-organ 3887→3661, fm-piano 990→961, clavisynth 1106→1038, ~1498→1486.
3. **Pass B — `--cap-long 4.0 32000`** — only zones longer than 4.0 s capped (kept: full attack + ~1 s+ of tail, loop intact at end of stored array). Result: every engine ≤ ceiling (range 358.0–673.6 KiB).
4. **Pass B2 — `--cap-long 4.0 42500`** — the 12 engines whose 15 zones *all* needed the cap sat at exactly 585.9 KiB; cap raised to 42,500 → all land at exactly 778.2 KiB (≈ +0.4 s of pre-loop tail per zone, still under ceiling). Those 12: ocarina-small, wine-glasses, recorder-alto, recorder-soprano, vibraphone, pipe-organ, recorder-tenor, bells, harmonica, tenor-saxophone, ocarina.
5. **Pass C/D (3.0 s / 2.5 s)** — never needed. Ceiling reached after B/B2.

Guardrails kept: piano never rebuilt; violin not re-cut; no bit-depth/rate/zone/factor changes; fades raised-cosine; loop wrap at end of stored array → seamless loop; no source files edited.

## Phase 2 — easy wins below violin

Same measurement applied to the ≤801 KiB catalog found 16 engines with dead tails (plucked/struck idiophones and chordophones — the note decays, then silence is recorded). Scope limit: **tail-trim only** (zero audible risk), no caps. Verdicts: all unchanged, except hand-bells-nepalese **improved 1/6 → 2/6**.

folk-harp 774.8→508.8 · bell-tree 751.5→582.6 · hand-chimes 687.0→534.0 · strumstick 657.1→504.6 · kalimba-kenya 431.7→235.2 · dan-tranh 544.1→394.0 · mbira-dzavadzimu 538.7→406.0 · nyunga-nyunga 453.9→350.9 · kalimba-tanzania 288.9→209.2 · hang 285.4→199.1 · steel-drum 230.3→156.0 · agogo-bells 234.9→188.2 · hand-bells-nepalese 317.2→264.1 · balafon 159.2→118.9 · slit-drum 110.8→78.3 · ukulele 279.7→270.2 (7.3%, included as same risk class)

## Verification gate (both phases)

`python compact-prototypes/machine_verify.py <engine>` after every rebuild. Pass = identical verdict to the pre-rework build (the six probes are piano-calibrated C2–A6; low probes out of range are pre-existing mapping gaps, not regressions). Narrow-register engines (bells, flexatone, ocarina-small, agogo-bells, bell-tree) score 0-3/6 pre-existing - see `issues-log.md` #5.

## NOT addressed — future work

### Above violin (the 27 — all now at/near the ceiling)

1. **No listening pass yet.** Every capped engine lost its natural decay after ~4 s + loop. `machine_verify` cannot hear this; the wrap is loop-seamless but the *note length* changed. User A/B end-of-note vs source, at least for ocarina-small, wine-glasses, vibraphone.
2. **recorder-bass anomaly** — its ~480,000 stored samples ≈ all 15 zones at the 32,000 cap (unlike the other mixed engines, whose sub-4 s zones added extra). It could join the B2 group (cap 42500 ⇒ ≈ 778 KiB) for ~+0.4 s of pre-loop tail per zone for free. Verify-in-tune gate applies.
3. **Sustained engines are pinned to “4 s + loop”.** Untried levers (need A/B listening, do not apply blindly): earlier factor-2 boundary (raise the −20 dB hold threshold), a factor-4 intermediate cascade stage, `find_loop` capped at ~0.6–0.8 s for fast-decay notes.
4. **`find_loop` sizes loops up to 1.5 s regardless of decay time** — short-decay notes may carry a redundant loop region at factor 6. Small gains, nonzero risk (loop seam clicks).
5. **Mixed engines cannot be raised to 42500** — doing so would exceed the ceiling (e.g., electric-organ ≈ +197 KiB ⇒ >800). Don't touch.
6. **Piano-calibrated probe set mis-measures narrow-register engines** — instrument-relative probes are tracked as future work (`issues-log.md` #5).
7. **Instrument-relative loop tuning / per-engine decay-matched caps** — folk-harp-style analysis shows loop could be shortened for struck/plucked sources; not attempted.

### Below violin (≤801 KiB)

1. **No easy win left.** Every remaining engine measures 0.0% tail-save (fully sustained) — the 16 in Phase 2 plus ukulele were the last ones.
2. **Cap-lever candidates exist but are not “easy”** — viola (4.4 s zones, 497.8 KiB), electric-bass (4.6 s, 290.1), eb-clarinet (4.3 s, 403.2), marimba (4.0 s, 503.9). The Phase 1 4 s-cap approach would apply; it is a taste decision (real content removed), not a free win.
3. **mda-violin itself (801.1)** — sustained, 0% tail-save, at the ceiling; any further shrink needs the cap lever (off-limits so far).
4. **harp (268.8 KiB, 10.5 s zones)** — already factor-6-limited; trimming gains 0%; not worth touching.
5. **Acoustic-guitar (mda-guitar, 147.7 KiB, 3/6)** — verify-low-pass; ear check still pending (pre-existing).
6. **cello/viola weak low probes** — likely weak-fundamental detection, not mapping; investigate only with an ear or a better detector.

### Both groups / catalog-wide

1. Only `mda-piano` is ear-verified; every other machine is automated-gate-checked at best.
2. 12 SFZ clones fail host tests rc=26/32 (piano-specific assumptions in `tests/sound.cpp`); not an engine defect; out of scope unless the user directs otherwise.
3. `machine_verify.py` is necessary, not sufficient — it cannot hear loop seams, truncation, timbre or loudness. `issues-log.md` #3, #5, #7.
4. All numbers are host measurements; hardware performance is unmeasured.
5. Stale artifacts (e.g., `mda-guitar-old`, `mda-compare`, `mda-upstream` dirs) are not part of the catalog; leave them out of any size accounting.

## 2026-11-15 evening (session 2 continued): marimba re-sourced (yarn -> cord)

User ear pass flagged marimba "extra noise". The yarn mallet is the fuzziest of the four on-disk options. Rebuilt from `marimba.cord.*`. Build now 109.4 KiB (was 503.9), verify 5/6 (was 3/6). The old yarn build is staged at `staged-marimba-yarn/` for revert. Re-verify at 5/6 then re-render the MIDI-60 sample.
