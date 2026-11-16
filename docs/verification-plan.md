# Addressing the 13 SDK-test failures — plan (2026-11-16)

Status when proposed: **51 of 64 machines pass** `sdk.py test` (preview ABI + host
tests), 13 fail. All measurements below were taken today with a portable LLVM
23.1.5 (`--cxx`) and a portable MinGW g++ (`--host-cxx`); portable toolchains live outside this repo and can be deleted when no longer needed.
`tests/sound.cpp` was also compiled and run directly to read its exit codes.

Two failure classes, unrelated to each other:

- **Class A — 12 machines, `tests/sound.cpp` host-test assertion failure.**
  The test is machine-owned, not SDK-owned. It fails in three distinct
  assertions, all piano-specific assumptions (this class is the known
  `issues-log #3`; the deeper root-cause measurement is below).
- **Class B — 1 machine, `mda-electric-organ`, preview artifact too large.**
  The machine works (it builds the preview and passes the ABI smoke checks);
  only the SDK's fixed 1 MB `engine.wasm` cap for the browser preview fails
  (1,057,165 B vs 1,048,576 B, +8,589 B / +0.8 %). Per user direction this
  machine is to be **held until everything else works**.

## Class A — measured root causes (not guesses)

| Exit code | Machines | Failing assertion | Cause (measured 2026-11-16) |
|---|---|---|---|
| rc=4 | mda-harp, mda-pipe-organ, mda-recorder-alto, mda-recorder-bass, mda-recorder-soprano, mda-recorder-tenor, mda-ocarina-small, mda-soprano-sax (8) + 1 more | `sounding` check on the single 64-frame render | These machines (rebuilt 2026-11-15 with the onset sin² fade) emit **exact zeros in block 0** at note 69. Voices initialize correctly (valid `position`/`end`/`delta`/`envelope`), and signal is present from block 1–2 onwards (verified over 400 blocks). The test renders one block and requires nonzero output. Not an engine defect; not out-of-range. |
| rc=26 | mda-harmonica, mda-marimba (2) | `different(output, comparison)` after 400 blocks | The short-tail and long-tail renders have **both decayed below the q31 multiply floor** by block 400; both outputs are silence, so the buffers are identical. The envelope for short-tail is below the threshold that selects `tail`, but the multiplied result rounds to zero before it can differ. Not an engine defect. |
| rc=32 | mda-bells (1) | `alternate.delta <= recording_step` | `render_blocks()` **re-initializes and re-starts the voice** (new LCG state, new random detune) before the 2500-block render, so the recorded delta belongs to a *different voice instance* than the one measured. The "delta never increases" invariant is only meaningful within a single voice/seed. Not an engine defect. |

Notes on the 9th rc=4 machine: alto/soprano recorders needed 2 blocks before
nonzero output; the other six needed 1. All are the same class.

What these fixes are **not**: they do not touch `mda-piano`, they do not rebuild
any engine, they do not edit the SDK, sources stay read-only, and no content or
sample data changes. Only `tests/sound.cpp` of the 12 machines changes, which is
the machine's own test (AGENTS.md: "extend `tests/` for the machine's promised
sound and behavior").

## Plan

### Step 1 — Rewrite the failing assertions in the 12 machines' `tests/sound.cpp`  (DONE, 2026-11-16)

Three localized changes, applied uniformly to the 12 machines (plus a fourth
learned during bring-up: the three single-block A/B output comparisons —
muffle filter, tone, tune — now compare the last block of a 4-block render,
i.e. block 3, which is above the onset-fade quantization floor). Everything
else in the tests stays byte-identical to the piano reference.

1. **Sounding check (rc=4 → fixed):** render up to 4 blocks (256 frames) and
   require *any* nonzero output instead of exactly one block. The piano gate
   would still pass; machines with the onset fade produce their first audible
   frame in block 1–2.
2. **Tail check (rc=26 → fixed):** compare the short-tail/long-tail outputs at
   a block count where the envelope is still above the multiply floor, or drop
   the output A/B comparison and keep only the zone-independent coefficient
   invariant (`alternate.tail > short_tail_coefficient`, the rc=22 path), which
   is valid for every machine. Decide per machine by measuring the block at
   which the outputs first differ (probe at 50/100/150 blocks); the conservative
   option is the coefficient-only assert.
3. **Delta check (rc=32 → fixed):** drop the `zone_for(...)` guard and the
   `alternate.delta <= recording_step` assertion (the invariants conflate two
   different voice instances), or restate it so it compares within one
   initialize→render sequence only. Keep the `position <= end` checks, which
   are valid and pass.

### Step 2 — Gate  (DONE, 2026-11-16)

- ✅ Re-ran `sdk.py test --machine .` on each of the 12 machines; all pass rc=0.
- ✅ Re-ran all 64 machines: **63/64 pass**. Only `mda-electric-organ` fails
  its documented preview-cap check. (Superseded by Steps 3–4: after the organ
  rebuild, **64/64 pass**. `mda-piano` untouched.)

### Step 3 — `mda-electric-organ` — HOLD → RESOLVED  (2026-11-16)

- Held per user direction, then resolved via the Step 4 path the user approved:
  uniform 2% end trim with raised-cosine fade (new builder flag `--end-trim`),
  full rebuild, pitch-neutral (-35.1 cents before/after), machine_verify 6/6
  IN TUNE (octave-fold; FreePats semantics), full SDK check now passes for
  **all 64 machines**. **Ear pass signed off by user (2026-11-16).**
  Details: `docs/issues-log.md` session 11.
- The machine passed the browser-preview 1 MB cap and the full check suite
  (63/64 -> 64/64). Device-side module fit remains unverified (no toolchain or
  device available here) and is not claimed.

### Step 4 — `mda-electric-organ` decision (DONE, 2026-11-16)

Executed: option 1, size reduction, under user authorization — uniform 2% end
trim (all 15 zones) with raised-cosine fade; `--end-trim` flag added to the
builder. Result: 1,057,165 B -> ~1.03 MB wasm; **64/64 pass**. Ear pass signed off by user.
Options 2 (drop) and 3 (ship with documented failure) were rejected by execution.

### Step 5 - Close-out  (DONE, 2026-11-16)

- ✅ `docs/issues-log.md` (sessions 10 and 11), `README.md` status updated.
- ✅ Final full-run: **64/64 pass**.
- ⬜ Create the GitHub repo and push — only when the user asks.

## Explicitly out of scope / unchanged

- `mda-piano` bit-identical gate: untouched, non-negotiable.
- The 5 deleted List X machines (altosax, bassoon, french-horn,
  renaissance-organ, saxello): out of scope permanently unless the user reopens.
- Hardware/device CPU measurement: still not possible here; remains pending.
- Repo naming: **decided 2026-11-16 — `dwd-git-dev/mda-ports`** (see `README.md`).
- Repo creation itself: pending explicit user instruction (`.github/workflows`-free,
  no push until authorized).
- Note: one engine WAS rebuilt in this plan — mda-electric-organ (Steps 3–4,
  2% uniform end trim, pitch-neutral, machine_verify green, ear pass signed off by user). No
  other machine was rebuilt.

## Risks

- A rewritten sounding check that looks too weak could mask a real silent
  defect — mitigated by keeping the check at nonzero output (ear can still
  verify) and by not touching any other assertion.
- The measured 1-block silence at block 0 is an onset-fade artifact; 4-block
  render tolerates it without endorsing permanent silence. If a machine turns
  out to be silent beyond block 4, that is a real finding and must be reported,
  not silently loosened.
- The tail-check weakening removes one audible-difference coverage point; the
  coefficient invariant (rc=22) retains coverage of the parameter itself.
- Any change more than the three localized edits per `sound.cpp` is out of plan
  and needs to be flagged, not smuggled in.

## Evidence (today's probes, in this workspace)

- `test_all64.py` — full 64-machine run; list of PASS/FAIL above.
- `soundcheck.py` — direct compile+run of `tests/sound.cpp` per failing
  machine (rc 4/26/32).
- `peakcheck.py` / `peakcheck2.py` — note-69 render state and block peaks
  (voices initialize; block 0 is zero; sound present from block 1–2).
- `peakcheck3.py` — 400-block renders: every rc=4 machine sounds loudly
  (peaks ≈ kOutputGain) from block 1 onwards.
- `peakcheck4.py` — short/long-tail A/B at 400 and 2500 blocks: harmonica and
  marimba output-identify below the multiply floor (env_s ≈ 254,839 « 2^30);
  bells/slit-drum differ.
