#include "display.h"

namespace mdaviola {
namespace {
using namespace opxy::draw;

const opxy::u32 kFirstNote = 21;
const opxy::u32 kLastNote = 108;
const opxy::u32 kNoteCount = kLastNote - kFirstNote + 1;
const opxy::u32 kFirstHeaderNode = 4;
const opxy::u32 kFirstNoteNode = 12;
const opxy::u32 kFirstChordNode = 100;
const opxy::u32 kChordNodeCapacity = 10;
const opxy::i32 kCenterX = 240;
const opxy::i32 kCenterY = 129;
const opxy::i32 kSpiralInner = 0;
const opxy::i32 kSpiralOuter = 52 * unit;
const opxy::i32 kSpiralScale = unit * 5 / 3;
const opxy::i32 kSpiralRibbon = unit / 8;
const opxy::i32 kSpiralAnglePerSemitone = 300;
const opxy::u32 kSpiralStart[4] = {0, 28, 50, 68};
const opxy::u32 kSpiralSpan[4] = {28, 22, 18, 16};
const opxy::u32 kInactiveAge = 0xffffu;
const opxy::u32 kAudibleThreshold = 4;
const opxy::u32 kVisibleThreshold = 128;
const opxy::u32 kChordFadeStart = 11;
const opxy::u32 kChordFadeLevels = 5;
const opxy::i32 kChordMaximumDistance = 90 * unit;
const opxy::i32 kSilenceExponent = 9431;
const opxy::u32 kExpTableSize = 46;
// Q15 e^x samples in 0.25 steps, spanning the MDA envelope exponents.
const opxy::u32 kExpTable[46] = {
    1, 2, 2, 3, 4, 5, 7, 9, 11, 14, 18, 23, 30, 38, 49, 63,
    81, 104, 134, 172, 221, 283, 364, 467, 600, 771, 990, 1271,
    1631, 2095, 2690, 3454, 4435, 5694, 7312, 9388, 12055, 15479,
    19875, 25520, 32768, 42075, 54025, 69370, 89073, 114372
};

opxy::i32 fraction(opxy::i32 value) {
    return value < 0 ? 0 : static_cast<opxy::i32>((static_cast<opxy::u32>(value) >> 16) * 1024 / 32767);
}

opxy::u32 pitch_held(const Input& input, opxy::u32 note) {
    return (input.held_pitches[note / 32] >> (note % 32)) & 1u;
}

opxy::u32 exp_q15(opxy::i32 exponent) {
    const opxy::i32 minimum = -10 * unit;
    const opxy::i32 maximum = 5 * unit / 4;
    if (exponent <= minimum) return kExpTable[0];
    if (exponent >= maximum) return kExpTable[kExpTableSize - 1];
    const opxy::u32 offset = static_cast<opxy::u32>(exponent - minimum);
    const opxy::u32 index = offset / 256u;
    const opxy::u32 remainder = offset % 256u;
    const opxy::i32 difference = static_cast<opxy::i32>(kExpTable[index + 1]) -
                                 static_cast<opxy::i32>(kExpTable[index]);
    return static_cast<opxy::u32>(static_cast<opxy::i32>(kExpTable[index]) +
        difference * static_cast<opxy::i32>(remainder) / 256);
}

opxy::u32 decay_tau(opxy::u32 note, opxy::i32 decay) {
    if (note < 44) note = 44;
    const opxy::i32 decay_level = decay < 512 ? decay * 3 / 2 + 256 : decay * 2;
    // Keep visual markers shorter-lived than MDA's audio envelope.
    const opxy::i32 exponent = -614 + static_cast<opxy::i32>(note * 3379u / 100u) -
                               decay_level;
    return exp_q15(-exponent) * unit / 32768u / 4u;
}

opxy::u32 release_tau(opxy::u32 note, opxy::i32 release) {
    const opxy::i32 exponent = 2048 + static_cast<opxy::i32>(note * 1741u / 100u) -
                               release * 2;
    return exp_q15(-exponent) * unit / 32768u;
}

opxy::u32 envelope(opxy::u32 age, opxy::u32 tau) {
    if (!tau || age * unit / tau >= static_cast<opxy::u32>(kSilenceExponent)) return 0;
    const opxy::i32 exponent = -static_cast<opxy::i32>(age * unit / tau);
    return exp_q15(exponent);
}

opxy::u32 note_level(const DisplayState& state, opxy::u32 index, bool held) {
    if (held) return envelope(state.note_age[index], state.envelope_tau[index]);
    if (state.release_age[index] == kInactiveAge) return 0;
    if (state.release_level[index]) {
        const opxy::u32 release_level = envelope(state.release_age[index],
                                                 state.envelope_tau[index]);
        return state.release_level[index] * release_level / 32768u;
    }
    if (state.note_age[index] != kInactiveAge)
        return envelope(state.note_age[index] + state.release_age[index],
                        state.envelope_tau[index]);
    return 0;
}

opxy::u32 gray_shade(opxy::u32 shade) {
    return 1u + ((shade - 1u) * 9u + 7u) / 14u;
}

opxy::u16 advance_age(opxy::u16 age, opxy::i32 elapsed) {
    const opxy::u32 advanced = age + static_cast<opxy::u32>(elapsed);
    return static_cast<opxy::u16>(advanced >= kInactiveAge ? kInactiveAge - 1 : advanced);
}

Point polar(opxy::i32 radius, opxy::i32 angle) {
    Point point = {floor_q10(sine(angle) * radius), -floor_q10(sine(angle + 900) * radius)};
    return point;
}

Point note_position(opxy::u32 note, opxy::i32 hardness, opxy::i32 randomness, opxy::u32 phase) {
    const opxy::i32 delta = static_cast<opxy::i32>(note) - 24;
    const opxy::i32 pitch_class = (delta % 12 + 12) % 12;
    const opxy::i32 angular_noise = pitch_class
        ? floor_q10(sine(static_cast<opxy::i32>(note * 173u + phase)) * randomness) * 24 / 1024
        : 0;
    const opxy::i32 radial_noise = floor_q10(
        sine(static_cast<opxy::i32>(note * 211u + phase * 2u)) * hardness * 2);
    opxy::i32 radius = kSpiralInner + delta * (kSpiralOuter - kSpiralInner) / 84 + radial_noise;
    if (radius < unit) radius = unit;
    Point position = polar(radius, delta * kSpiralAnglePerSemitone + angular_noise);
    position.x = floor_q10(position.x * kSpiralScale);
    position.y = floor_q10(position.y * kSpiralScale);
    return position;
}

void build_spiral(DisplayState& state, Frame& frame) {
    for (opxy::u32 segment = 0; segment < 4; ++segment) {
        Frame::path_type& path = frame.paths[segment];
        for (opxy::u32 i = 0; i < 32; ++i) {
            const opxy::i32 semitone = static_cast<opxy::i32>(kSpiralStart[segment] * unit) +
                static_cast<opxy::i32>(
                    static_cast<opxy::i64>(i) * kSpiralSpan[segment] * unit / 31u);
            const opxy::i32 angle = semitone * kSpiralAnglePerSemitone / unit;
            const opxy::i32 radius = kSpiralInner + kSpiralRibbon +
                static_cast<opxy::i32>(
                    static_cast<opxy::i64>(kSpiralOuter - kSpiralInner - 2 * kSpiralRibbon) *
                    semitone / (84 * unit));
            const Point outer = polar(radius + kSpiralRibbon, angle);
            const Point inner = polar(radius - kSpiralRibbon, angle);
            path.points[i] = outer;
            path.points[63 - i] = inner;
        }
        hide(frame, segment);
    }
    state.spiral_key = state.spiral_key % 0x7ffffffeu + 1;
    for (opxy::u32 segment = 0; segment < 4; ++segment)
        frame.paths[segment].key = state.spiral_key;
}

opxy::u32 integer_sqrt(opxy::i64 value) {
    opxy::u32 low = 0;
    opxy::u32 high = static_cast<opxy::u32>(kChordMaximumDistance) + 1;
    while (low + 1 < high) {
        const opxy::u32 middle = low + (high - low) / 2;
        if (static_cast<opxy::i64>(middle) * middle <= value) low = middle;
        else high = middle;
    }
    return low;
}

void hide_chord(Frame& frame) {
    for (opxy::u32 i = 0; i < kChordNodeCapacity; ++i) hide(frame, kFirstChordNode + i);
    hide(frame, 112);
}

void build_chord(const DisplayState& state, const Input& input, Frame& frame,
                 opxy::i32 hardness, opxy::i32 randomness) {
    Point points[kChordNodeCapacity];
    opxy::u32 notes[kChordNodeCapacity];
    opxy::u32 count = 0;
    for (opxy::u32 note = kFirstNote; note <= kLastNote; ++note) {
        if (!(state.chord_notes[note / 32] & (1u << (note % 32)))) continue;
        if (count == kChordNodeCapacity) {
            hide_chord(frame);
            return;
        }
        notes[count] = note;
        points[count++] = note_position(note, hardness, randomness, state.phase);
    }
    if (count < 3) {
        hide_chord(frame);
        return;
    }

    const opxy::i64 maximum_distance_squared =
        static_cast<opxy::i64>(kChordMaximumDistance) * kChordMaximumDistance;
    for (opxy::u32 i = 0; i < count; ++i) {
        for (opxy::u32 j = i + 1; j < count; ++j) {
            const opxy::i64 dx = points[j].x - points[i].x;
            const opxy::i64 dy = points[j].y - points[i].y;
            if (dx * dx + dy * dy > maximum_distance_squared) {
                hide_chord(frame);
                return;
            }
        }
    }

    for (opxy::u32 i = 0; i < count; ++i) {
        const Point& from = points[i];
        const Point& to = points[(i + 1) % count];
        const opxy::i32 dx = to.x - from.x;
        const opxy::i32 dy = to.y - from.y;
        const opxy::i64 distance_squared =
            static_cast<opxy::i64>(dx) * dx + static_cast<opxy::i64>(dy) * dy;
        const opxy::u32 distance = integer_sqrt(distance_squared);
        if (!distance) {
            hide_chord(frame);
            return;
        }
        const opxy::u32 node = kFirstChordNode + i;
        const opxy::u32 from_index = notes[i] - kFirstNote;
        const opxy::u32 to_index = notes[(i + 1) % count] - kFirstNote;
        const opxy::u32 from_level = note_level(
            state, from_index, pitch_held(input, notes[i]) != 0);
        const opxy::u32 to_level = note_level(
            state, to_index, pitch_held(input, notes[(i + 1) % count]) != 0);
        const opxy::u32 level = from_level < to_level ? from_level : to_level;
        if (level < kVisibleThreshold) {
            hide(frame, node);
            continue;
        }
        const opxy::i32 scale = static_cast<opxy::i32>(distance / 64u);
        const opxy::u32 shade = kChordFadeStart +
            level * (kChordFadeLevels - 1u) / 32768u;
        place(frame, node, kCenterX * unit + (from.x + to.x) / 2,
              kCenterY * unit + (from.y + to.y) / 2,
              clamp(scale, 1, 2 * unit), shade, stroke);
        const Turn direction = {
            static_cast<opxy::i32>(static_cast<opxy::i64>(dx) * unit / distance),
            static_cast<opxy::i32>(static_cast<opxy::i64>(dy) * unit / distance)
        };
        turn(frame, node, direction);
    }
    for (opxy::u32 i = count; i < kChordNodeCapacity; ++i) hide(frame, kFirstChordNode + i);
    hide(frame, 112);
}
}

inline void display_reset(DisplayState& state) {
    state.header.reset();
    state.phase = 0;
    state.spiral_key = 1;
    state.track = 0;
    state.initialized = 0;
    state.chord_serial = 0;
    for (opxy::u32 i = 0; i < 4; ++i) {
        state.previous[i] = 0;
        state.chord_notes[i] = 0;
    }
    for (opxy::u32 i = 0; i < kNoteCount; ++i) {
        state.note_age[i] = kInactiveAge;
        state.release_age[i] = kInactiveAge;
        state.release_level[i] = 0;
        state.envelope_tau[i] = 0;
    }
}

inline void display(DisplayState& state, const Input& input, Frame& frame) {
    const char* const titles[8] = {
        "Hardness", "Muffle", "Decay", "Tail", "Tone", "Random", "Stretch", "Tune"
    };
    char values[8][header_capacity];
    for (opxy::u32 i = 0; i < 8; ++i)
        header_amount(values[i], header_capacity, input.slots[i]);
    page_header(state.header, input, frame, titles, values, kFirstHeaderNode);

    const bool track_changed = !state.initialized || state.track != input.track;
    const opxy::i32 elapsed = clamp(input.elapsed, 0, 102);
    const bool pitches_available = input.held_notes <= 128;
    const opxy::i32 hardness = fraction(input.slots[0]);
    const opxy::i32 muffle = fraction(input.slots[1]);
    const opxy::i32 decay = fraction(input.slots[2]);
    const opxy::i32 tail = fraction(input.slots[3]);
    const opxy::i32 tone = fraction(input.slots[4]);
    const opxy::i32 randomness = fraction(input.slots[5]);
    const opxy::i32 stretch = fraction(input.slots[6]);
    const opxy::i32 tune = fraction(input.slots[7]);

    if (track_changed) {
        for (opxy::u32 i = 0; i < 4; ++i) state.previous[i] = 0;
        for (opxy::u32 i = 0; i < 4; ++i) state.chord_notes[i] = 0;
        state.chord_serial = input.note_serial;
        for (opxy::u32 i = 0; i < kNoteCount; ++i) {
            state.note_age[i] = kInactiveAge;
            state.release_age[i] = kInactiveAge;
            state.release_level[i] = 0;
            state.envelope_tau[i] = 0;
        }
        state.phase = 0;
    }
    if (!pitches_available) {
        for (opxy::u32 i = 0; i < 4; ++i) state.previous[i] = 0;
        for (opxy::u32 i = 0; i < 4; ++i) state.chord_notes[i] = 0;
        state.chord_serial = input.note_serial;
        for (opxy::u32 i = 0; i < kNoteCount; ++i) {
            state.note_age[i] = kInactiveAge;
            state.release_age[i] = kInactiveAge;
            state.release_level[i] = 0;
            state.envelope_tau[i] = 0;
        }
    }
    if (pitches_available && !track_changed && input.note_serial != state.chord_serial) {
        state.chord_serial = input.note_serial;
        if (input.held_notes >= 3) {
            for (opxy::u32 i = 0; i < 4; ++i) state.chord_notes[i] = input.held_pitches[i];
        }
    }
    if (!input.reduced_motion && pitches_available && input.held_notes)
        state.phase = (state.phase + static_cast<opxy::u32>(elapsed)) % 3600u;

    build_spiral(state, frame);
    // Keep legacy numeral/eyes slots inert.
    hide(frame, 110);
    hide(frame, 111);
    opxy::draw::text(frame, 16, 48 * unit, 140 * unit, 2 * unit, "");
    opxy::draw::text(frame, 17, 432 * unit, 140 * unit, 2 * unit, "");

    const opxy::i32 pan_radius = floor_q10(kSpiralOuter * kSpiralScale);
    for (opxy::u32 note = kFirstNote; note <= kLastNote; ++note) {
        const opxy::u32 index = note - kFirstNote;
        const opxy::u32 mask = 1u << (note % 32);
        const bool held = pitches_available && pitch_held(input, note);
        const bool was_held = !track_changed && (state.previous[note / 32] & mask);
        if (held) {
            if (!was_held) {
                state.note_age[index] = 0;
                state.release_age[index] = kInactiveAge;
                state.release_level[index] = 0;
                state.envelope_tau[index] = static_cast<opxy::u16>(decay_tau(note, decay));
            } else {
                state.note_age[index] = advance_age(state.note_age[index], elapsed);
            }
        } else if (was_held) {
            const opxy::u16 age = advance_age(state.note_age[index], elapsed);
            const opxy::u32 level = envelope(age, state.envelope_tau[index]);
            state.note_age[index] = note >= 94 ? age : kInactiveAge;
            state.release_age[index] = level >= kAudibleThreshold ? 0 : kInactiveAge;
            state.release_level[index] = note >= 94 ? 0 :
                static_cast<opxy::u16>(level);
            if (note < 94 && level >= kAudibleThreshold)
                state.envelope_tau[index] = static_cast<opxy::u16>(release_tau(note, tail));
        } else if (state.release_age[index] != 0xffffu) {
            state.release_age[index] = advance_age(state.release_age[index], elapsed);
        }

        if (held) state.previous[note / 32] |= mask;
        else state.previous[note / 32] &= ~mask;

        const opxy::u32 level = note_level(state, index, held);
        if (level < kVisibleThreshold) {
            if (!held) {
                state.note_age[index] = kInactiveAge;
                state.release_age[index] = kInactiveAge;
                state.release_level[index] = 0;
                state.envelope_tau[index] = 0;
            }
            hide(frame, kFirstNoteNode + index);
            continue;
        }
        const Point position = note_position(note, hardness, randomness, state.phase);
        const opxy::i32 intensity = static_cast<opxy::i32>(level * unit / 32768u);
        const opxy::i32 pan = clamp(position.x < 0 ? -position.x * 1024 / pan_radius
                                                    : position.x * 1024 / pan_radius, 0, 1024);
        const opxy::i32 size = 640 + decay / 4 + floor_q10(tone * pan) / 4;
        const opxy::i32 visible_size = floor_q10(size * intensity);
        const opxy::i32 base_shade = clamp(
            2 + tune * 10 / 1024 - muffle * 4 / 1024 + stretch * 2 / 1024, 1, 15);
        const opxy::u32 shade = gray_shade(static_cast<opxy::u32>(
            clamp(1 + (base_shade - 1) * static_cast<opxy::i32>(level) / 32768, 1, 15)));
        place(frame, kFirstNoteNode + index, kCenterX * unit + position.x,
              kCenterY * unit + position.y, clamp(visible_size, 1, 2 * unit), shade, fill_and_stroke);
    }
    state.track = input.track;
    state.initialized = 1;

    build_chord(state, input, frame, hardness, randomness);
}
}
