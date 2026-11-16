#include "core.h"

#include "cello_samples.hpp"

#include "cello_zones.hpp"

#include <opxy/q31.hpp>



namespace mdacello {

namespace {

using opxy::i32;

using opxy::i64;

using opxy::u32;



static_assert(sizeof(Voice) == 84, "Update definition.json voiceBytes");

static_assert(sizeof(Shared) == 4, "Update definition.json sharedBytes");



const u32 kStepQ16[31] = {

    13777u, 14596u, 15464u, 16384u, 17358u, 18390u, 19484u, 20643u,

    21870u, 23170u, 24548u, 26008u, 27554u, 29193u, 30929u, 32768u,

    34716u, 36781u, 38968u, 41285u, 43740u, 46341u, 49097u, 52016u,

    55109u, 58386u, 61858u, 65536u, 69433u, 73562u, 77936u,

};

const u32 kNoteIncrements[88] = {

    2678268u, 2837526u, 3006254u, 3185015u, 3374406u, 3575058u, 3787642u, 4012867u,

    4251485u, 4504291u, 4772130u, 5055896u, 5356535u, 5675051u, 6012507u, 6370030u,

    6748811u, 7150117u, 7575285u, 8025735u, 8502970u, 9008582u, 9544261u, 10111792u,

    10713070u, 11350103u, 12025015u, 12740059u, 13497623u, 14300233u, 15150569u, 16051469u,

    17005939u, 18017165u, 19088521u, 20223584u, 21426141u, 22700205u, 24050030u, 25480119u,

    26995246u, 28600467u, 30301139u, 32102938u, 34011878u, 36034330u, 38177043u, 40447168u,

    42852281u, 45400411u, 48100060u, 50960238u, 53990491u, 57200933u, 60602278u, 64205876u,

    68023757u, 72068660u, 76354085u, 80894335u, 85704563u, 90800821u, 96200119u, 101920476u,

    107980983u, 114401866u, 121204555u, 128411753u, 136047513u, 144137319u, 152708170u, 161788671u,

    171409126u, 181601643u, 192400238u, 203840952u, 215961966u, 228803732u, 242409110u, 256823506u,

    272095026u, 288274639u, 305416341u, 323577341u, 342818251u, 363203285u, 384800477u, 407681904u

};

const u32 kExpQ16[21] = {

    8869, 11388, 14623, 18776, 24109, 30957, 39750, 51039, 65536, 84150,

    108051, 138740, 178145, 228743, 293712, 377134, 484249, 621788, 798392,

    1025156, 1316326

};

const i32 kTailThreshold = 0x40000000;

const i32 kOutputGain = 184685363;



u32 control(i32 value) {

    if (value <= 0) return 0;

    const u32 result = static_cast<u32>(value) >> 21;

    return result > 1023 ? 1023 : result;

}



i32 cello_sample(u32 index) {

    const u32 bit = index * 10u;

    const u32 byte = bit >> 3;

    const u32 shift = bit & 7u;

    u32 value = cello_data[byte] | (static_cast<u32>(cello_data[byte + 1]) << 8);

    if (shift + 10u > 16u) value |= static_cast<u32>(cello_data[byte + 2]) << 16;

    value = (value >> shift) & 0x3ffu;

    const i32 signed_value = value & 0x200u

        ? static_cast<i32>(value) - 1024 : static_cast<i32>(value);

    return signed_value * 64;

}



u32 note_from_pitch(u32 increment) {

    u32 low = 0, high = 87;

    if (increment <= kNoteIncrements[0]) return 21;

    if (increment >= kNoteIncrements[87]) return 108;

    while (low < high) {

        const u32 middle = (low + high) / 2;

        if (increment > kNoteIncrements[middle]) low = middle + 1;

        else high = middle;

    }

    if (increment - kNoteIncrements[low - 1] <= kNoteIncrements[low] - increment) --low;

    return low + 21;

}



u32 exp_q16(i32 exponent) {

    const i32 minimum = -2 * 65536;

    const i32 maximum = 3 * 65536;

    if (exponent <= minimum) return kExpQ16[0];

    if (exponent >= maximum) return kExpQ16[20];

    const u32 offset = static_cast<u32>(exponent - minimum);

    const u32 index = offset / 16384u;

    const u32 remainder = offset % 16384u;

    const i32 difference = static_cast<i32>(kExpQ16[index + 1]) -

                           static_cast<i32>(kExpQ16[index]);

    return kExpQ16[index] + (difference * static_cast<i32>(remainder)) / 16384;

}



u32 decay_coefficient(u32 note, u32 decay) {

    if (note < 44) note = 44;

    i32 decay_level = static_cast<i32>(decay * 2);

    if (decay_level < 1024) decay_level += 256 - static_cast<i32>(decay / 2);

    const i32 exponent = -39322 + static_cast<i32>(note * 2163u) - decay_level * 64;

    const u32 rate = static_cast<u32>(static_cast<i64>(exp_q16(exponent)) * 32768 / 44100);

    return rate >= 0x7fffffffu ? 0 : 0x7fffffffu - rate;

}



u32 tail_coefficient(u32 note, u32 tail) {

    const i32 exponent = 131072 + static_cast<i32>(note * 1114u) -

                         static_cast<i32>(tail * 128u);

    const u32 rate = static_cast<u32>(static_cast<i64>(exp_q16(exponent)) * 32768 / 44100);

    return rate >= 0x7fffffffu ? 0 : 0x7fffffffu - rate;

}



u32 pitch_step(u32 note, u32 root, u32 randomness, u32 stretch, u32 tune, u32 seed) {

    const i32 step_index = static_cast<i32>(note) - static_cast<i32>(root) + 15;
    const i32 step_clamped = step_index < 0 ? 0 : (step_index > 30 ? 30 : step_index);
    const i32 base = static_cast<i32>(kStepQ16[step_clamped]);

    const i32 random_semitones = static_cast<i32>(

        static_cast<i32>(seed >> 16) - 32768) * randomness * randomness /

        (1024 * 1024);

    const i64 adjustment = static_cast<i64>(base) * random_semitones * 3896 /

                           (static_cast<i64>(1) << 32);

    i32 factor = 65536 + (static_cast<i32>(tune) - 512) * 3696 / 1000;

    if (note > 60) {

        const i32 distance = static_cast<i32>(note) - 60;

        factor += static_cast<i32>(

            static_cast<i64>(static_cast<i32>(stretch) - 512) *

            distance * distance * 1604 / 1000000);

    }

    const i64 step = (static_cast<i64>(base) + adjustment) * factor / 65536;

    return step > 0 ? static_cast<u32>(step) : 1;

}



void initialize_note(Voice& voice, opxy::Parameters parameters, u32 pitch_increment) {

    const u32 note = note_from_pitch(pitch_increment);

    const u32 hardness = control(parameters[0]);

    const i32 zone_shift = static_cast<i32>(hardness * 12 / 1024) - 6;

    u32 group = 0;

    while (group < 14 && static_cast<i32>(note) >

           static_cast<i32>(kZones[group].high) + zone_shift) ++group;

    const ZoneTable& key = kZones[group];



    voice.note = note;

    voice.position = key.start;

    voice.fraction = 0;

    voice.delta = pitch_step(note, key.root, control(parameters[5]),

                             control(parameters[6]), control(parameters[7]),

                             voice.random_seed);

    voice.b1 = key.b1; voice.f1 = key.f1;

    voice.b2 = key.b2; voice.f2 = key.f2;

    voice.b3 = key.b3; voice.f3 = key.f3;

    voice.end = key.end;

    voice.loop = key.loop;

    if (voice.position >= voice.b3) voice.delta = voice.delta / voice.f3;

    else if (voice.position >= voice.b2) voice.delta = voice.delta / voice.f2;

    else if (voice.position >= voice.b1) voice.delta = voice.delta / voice.f1;

    voice.envelope = 0x7fffffff;

    voice.decay = static_cast<i32>(decay_coefficient(note, control(parameters[2])));

    voice.tail = note >= 94

        ? voice.decay

        : static_cast<i32>(tail_coefficient(note, control(parameters[3])));

    const u32 muffle = control(parameters[1]);

    i32 cutoff = 210 - static_cast<i32>(muffle * muffle * 160 / (1024 * 1024));

    cutoff += (static_cast<i32>(control(parameters[4])) - 512) * 120 / 1024;

    const i32 minimum = 55 + static_cast<i32>(note) / 4;

    if (cutoff < minimum) cutoff = minimum;

    if (cutoff > 210) cutoff = 210;

    voice.filter_coefficient = static_cast<i32>(

        static_cast<i64>(cutoff) * cutoff * 0x7fffffff / 44100);

    voice.filter = 0;

    voice.previous = 0;

    voice.gain = kOutputGain;

    voice.initialized = 1;

}

}



inline void initialize(Voice& voice, Shared&, u32) { voice.initialized = 0; }

inline void start(Voice& voice, Shared& shared, u32 owner) {

    voice.initialized = 0;

    shared.random_seed = shared.random_seed * 1664525u + 1013904223u + owner;

    voice.random_seed = shared.random_seed;

}

inline void destroy(Voice&, Shared&, u32) {}



inline void render(Voice& voice, Shared&, u32, opxy::Output output,

                   opxy::Parameters parameters, opxy::Pitch pitch) {

    if (!voice.initialized) initialize_note(voice, parameters, pitch[0]);

    for (unsigned i = 0; i < opxy::block_frames; ++i) {

        const u32 old_pos = voice.position;

        u32 phase = voice.fraction + voice.delta;

        u32 pos = voice.position + (phase >> 16);

        u32 frac = phase & 0xffffu;

        if (old_pos < voice.b1 && pos >= voice.b1 && voice.f1 > 0) {
            pos = voice.b1;
            frac = 0;
            voice.delta = voice.delta / voice.f1;
        }

        if (old_pos < voice.b2 && pos >= voice.b2 && voice.b2 > voice.b1 && voice.f2 > 0) {
            pos = voice.b2;
            frac = 0;
            voice.delta = (u32)(((unsigned long long)voice.delta * voice.f1) / voice.f2);
        }

        if (old_pos < voice.b3 && pos >= voice.b3 && voice.b3 > voice.b2 && voice.f3 > 0) {
            pos = voice.b3;
            frac = 0;
            voice.delta = (u32)(((unsigned long long)voice.delta * voice.f2) / voice.f3);
        }

        voice.position = pos;

        voice.fraction = frac;

        if (voice.position > voice.end) voice.position -= voice.loop;



        const i32 p0 = cello_sample(voice.position > 0 ? voice.position - 1 : 0);

        const i32 p1 = cello_sample(voice.position);

        const u32 n1 = voice.position < voice.end ? voice.position + 1 : voice.position;

        const i32 p2 = cello_sample(n1);

        const u32 n2 = n1 < voice.end ? n1 + 1 : n1;

        const i32 p3 = cello_sample(n2);

        const i64 t = static_cast<i64>(voice.fraction >> 1);

        i64 y = static_cast<i64>(-p0) + 3 * p1 - 3 * p2 + p3;

        y = (y * t) / 32768 + (2 * static_cast<i64>(p0) - 5 * p1 + 4 * p2 - p3);

        y = (y * t) / 32768 + (static_cast<i64>(p2) - p0);

        y = (y * t) / 32768 + 2 * static_cast<i64>(p1);

        const i32 sample = static_cast<i32>(y / 2);

        const i32 input = sample * 65535;

        const i32 excited = opxy::q31::multiply(input, voice.envelope);

        const i32 coefficient = voice.envelope > kTailThreshold

            ? voice.decay : voice.tail;

        voice.envelope = opxy::q31::multiply(voice.envelope, coefficient);

        i32 difference = opxy::q31::add(excited, voice.previous);

        const i32 inverse_filter = voice.filter == (-0x7fffffff - 1)

            ? 0x7fffffff : -voice.filter;

        difference = opxy::q31::add(difference, inverse_filter);

        const i32 filtered = opxy::q31::add(

            voice.filter, opxy::q31::multiply(voice.filter_coefficient, difference));

        voice.previous = excited;

        voice.filter = filtered;

        output[i] = opxy::q31::multiply(filtered, voice.gain);

    }

}

}

