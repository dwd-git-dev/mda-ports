# mda-ports

64 sampled instrument machines, derived from the MDA Piano 1.0 skeleton
(Paul Kellett / MDA) — not original work. The instruments are ports of the
upstream MDA sound library to a small, embedded-friendly sample engine.

License notes: per-machine `LICENSE` / `MDA-LICENSE.txt` files ship inside each
instrument folder. The ports are MIT-style, as is the derivative work.

Provenance: all sample sources are CC0 / public domain / MIT-compatible —
Iowa MIS, VSCO-2-CE, VCSL, FreePats, jlearman.SteelDrum — audit trail in
`docs/source-inventory.md`. The display names on the device are lowercase
`<brand> <instrument>`: `mda piano`, `mda accordion`, `mda violin pizz`, etc.

Device targets: **Open-XY** (currently). Other targets/ports are possible later —
the repo identity is not tied to one device; it is just the shared machine code.
`Open-XY` machine IDs must be unique per instrument and follow the OP-XY id rules.

Structure:

| Path | Contents |
|---|---|
| `machines/<name>/` | One folder per machine: `engine.cpp`, `core.h`, `display.*`, `definition.json`, `machine.json`, `<name>_samples.hpp`, `<name>_zones.hpp`, licenses, tests. |
| `docs/` | Build and verification notes: `docs/fullpass-log.md`, `docs/instrument-build-log.md`, `docs/issues-log.md`, `docs/instruments-rework-log.md`, `docs/listening-log.md`, `docs/source-inventory.md`, `docs/sampling-notes/` — per-instrument notes. |

**Note on documentation:** the logs describe the full build/verify history. The
verification tooling referenced there is **not bundled in this repository**;
treat verification claims as historical records, not regenerateable procedures.

Catalog (64 machines): see `machines/`.

Note on the per-machine files: each machine folder carries a `tests/`,
`display.*.backup` and a `.github/workflows/machine.yml`, carried from the
pristine layout they were built from. The nested workflows are blueprints for a
machine's own repository; they are inert in a multi-machine layout and are kept
only as templates (they are not running CI here).

Status (2026-11-16): all 64 machines build the browser preview and pass the full
SDK checks (preview ABI + host tests) — **64/64**. All 64 also compile against
the pinned Blackfin tools (device-target compile check, no device connected).
The mda-electric-organ rebuild (2% uniform end trim, pitch-neutral) resolved the
one remaining host-visible gate; it has the listener's sign-off ("sounds great").
`release/` in the build workspace is synced for the organ; note it was stale
against the 2026-11-16 catalog-wide rename before that sync. Hardware CPU
measurements are **pending** — no OP-XY hardware was available; the host-timed
estimates and the device-compile checks above are not device measurements. See
`docs/issues-log.md` (sessions 10–12) and `docs/verification-plan.md`.

## Repository

Public repository: https://github.com/dwd-git-dev/mda-ports (created 2026-11-16).
Multi-machine repository layout: each machine is `machines/<id>/`, suitable for
catalog entries with `path: machines/<id>` (see the Open-XY
[catalog guide](https://github.com/etcetc000/open-xy/blob/staging/docs/catalog.md)).
