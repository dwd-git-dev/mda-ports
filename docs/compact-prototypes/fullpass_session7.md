# Fullpass-log appendix - 2026-11-15 session 7 (loop re-entrance ghosts; violin-pizz not a defect)

## 2026-11-15 evening (session 7): "Second peak before 2 s" — one real ghost family found, fixed

User report: `mda-violin-pizz_midi60.wav` - "just before 2 seconds, the tuba sound has a second peak, not sure if it is digital noise, but the raw doesn't have this" (raw = the MIS source).

**Root cause family (in this session):** a voice held by the host (no note-off ever) decays below the machine's `kTailThreshold`, loops back to the beginning of the stored zone, and re-enters ff content at low but audible level if the stored zone does not fade to silence at its very end.

**mda-tuba:** fixed with `--end-fade 1.0` (`build_instrument_data.py` gained the flag). The stored zone's last 1 s of the ff blast now ramps to zero; the loop wraps inside silence. midi60 re-rendered. Verify verdict unchanged (4/6; 84/93 out of tune by design - source only reaches C4).

**mda-electric-bass and ** were found with the same re-entrance pattern by a scan of all 69 midi60 renders (post-decay rise > 12 dB after falling < -50 dBFS). Both rebuilt with `--end-fade 1.0`. Verify verdicts unchanged (electric-bass 2/6, 6/6 octave-fold / 5/6 in strict). Both re-rendered.

**mda-violin-pizz is NOT a defect.** The bump before 2 s is the source's own second hump: `mf_G3_0_m55.wav` (MIDI 60's source) decays to 0.040 at 0.8 s, then rises again to 0.061-0.139 at 1.0-1.2 s before dying - the plucked string's body resonance. The loop replays it faithfully. `mda-violin-pizz` was rebuilt with `--tail-trim -30` and produced **bit-identical output to the previous build** (841,833 bytes, verify 5/6 unchanged). If the user wants it gone, `--tail-trim -40` would shorten the audible tail further but would delete musical content - pending ear decision.

**Re-entrance scan of all 69 midi60 renders after the fixes:** clean. Only borderline flag left is mda-bells @0.13 s - measured, that is the source's own dynamic swell, not a loop ghost.

**Files:** `build_instrument_data.py` gained `--end-fade <seconds>` (applies an 8 ms... no: applies cos^2 ramp over the final N seconds of each stored zone; into-silence before planning).
**midi60-samples/** freshly re-rendered for tuba, electric-bass, violin-pizz, accordion (special), plus all others (all 69).
**release/**: 3 machines' samples+zones+engine refreshed; piano byte-identical intact.

**No ear pass has been run on these builds yet. This is a pitch/silence/discontinuity gate, not a listening test.**
