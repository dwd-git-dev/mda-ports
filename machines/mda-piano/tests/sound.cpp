// The machine's own sound checks, compiled for the host and run by
// python sdk.py test --machine <folder>.
#include "../engine.cpp"

namespace {
const opxy::u32 kNoteIncrements[88] = {
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
bool check(bool condition) { return condition; }
bool different(const opxy::i32 first[64], const opxy::i32 second[64]) {
    for (unsigned i = 0; i < opxy::block_frames; ++i)
        if (first[i] != second[i]) return true;
    return false;
}
const ::ZoneTable& zone_for(opxy::u32 note) {
    for (unsigned i = 0; i < 15; ++i)
        if (note <= kZones[i].high) return kZones[i];
    return kZones[14];
}
void render_note(mdapiano::Voice& voice, mdapiano::Shared& shared, opxy::u32 owner,
                 opxy::u32 increment, const opxy::i32 parameters[8], opxy::i32 output[64]) {
    opxy::u32 pitch[opxy::block_frames];
    for (unsigned i = 0; i < opxy::block_frames; ++i) pitch[i] = increment;
    mdapiano::initialize(voice, shared, owner);
    mdapiano::start(voice, shared, owner);
    mdapiano::render(voice, shared, owner, output, parameters, pitch);
}
void render_blocks(mdapiano::Voice& voice, mdapiano::Shared& shared, opxy::u32 owner,
                   opxy::u32 increment, const opxy::i32 parameters[8],
                   opxy::i32 output[64], unsigned blocks) {
    opxy::u32 pitch[opxy::block_frames];
    for (unsigned i = 0; i < opxy::block_frames; ++i) pitch[i] = increment;
    mdapiano::initialize(voice, shared, owner);
    mdapiano::start(voice, shared, owner);
    for (unsigned i = 0; i < blocks; ++i)
        mdapiano::render(voice, shared, owner, output, parameters, pitch);
}
}

int main() {
    static mdapiano::Voice voice;
    static mdapiano::Voice low;
    static mdapiano::Voice high;
    static mdapiano::Voice alternate;
    static mdapiano::Shared shared;
    opxy::i32 parameters[8] = {
        0x40000000, 0x40000000, 0x7fffffff, 0x40000000,
        0x40000000, 0x40000000, 0x40000000, 0x40000000
    };
    opxy::i32 output[opxy::block_frames];

    render_note(voice, shared, 1, 42852281u, parameters, output);
    if (!check(voice.note == 69 && voice.position >= zone_for(69).start && voice.position <= voice.end)) return 1;
    if (!check(voice.end == zone_for(69).end && voice.loop == zone_for(69).loop)) return 2;
    bool sounding = false;
    for (unsigned i = 0; i < opxy::block_frames; ++i) {
        sounding = sounding || output[i] != 0;
        if (!check(output[i] >= -1073741824 && output[i] <= 1073741824)) return 3;
    }
    if (!check(sounding)) return 4;

    render_note(low, shared, 2, 2678268u, parameters, output);
    if (!check(low.note == 21 && low.position >= zone_for(21).start && low.position <= low.end)) return 5;
    if (!check(low.end == zone_for(21).end && low.loop == zone_for(21).loop)) return 6;

    render_note(high, shared, 3, 407681904u, parameters, output);
    if (!check(high.note == 108 && high.position >= zone_for(108).start && high.position <= high.end)) return 7;
    if (!check(high.end == zone_for(108).end && high.loop == zone_for(108).loop)) return 8;

    for (opxy::u32 note = 21; note <= 108; ++note) {
        render_note(alternate, shared, 5, kNoteIncrements[note - 21], parameters, output);
        if (!check(alternate.note == note && alternate.position <= alternate.end)) return 14;
        if (!check(alternate.end == zone_for(note).end)) return 18;
    }

    render_note(alternate, shared, 6, kNoteIncrements[60 - 21], parameters, output);
    const opxy::u32 recording_step = alternate.delta;
    render_blocks(alternate, shared, 6, kNoteIncrements[60 - 21], parameters, output, 1200);
    if (!check(alternate.position <= alternate.end)) return 31;
    if (zone_for(60).b1 > zone_for(60).start)
        if (!check(alternate.delta <= recording_step)) return 32;

    opxy::i32 long_decay[8] = {0x40000000,0x40000000,0x7fffffff,0x40000000,0x40000000,0x40000000,0x40000000,0x40000000};
    opxy::i32 short_decay[8] = {0x40000000,0x40000000,0,0x40000000,0x40000000,0x40000000,0x40000000,0x40000000};
    render_note(alternate, shared, 4, 42852281u, short_decay, output);
    const opxy::i32 short_decay_coefficient = alternate.decay;
    render_note(alternate, shared, 4, 42852281u, long_decay, output);
    if (!check(alternate.decay > short_decay_coefficient)) return 10;

    opxy::i32 dark[8] = {0x40000000,0,0x7fffffff,0x40000000,0x40000000,0x40000000,0x40000000,0x40000000};
    render_note(alternate, shared, 4, 42852281u, dark, output);
    const opxy::i32 low_muffle_filter = alternate.filter_coefficient;
    opxy::i32 comparison[opxy::block_frames];
    for (unsigned i = 0; i < opxy::block_frames; ++i) comparison[i] = output[i];
    dark[1] = 0x7fffffff;
    render_note(alternate, shared, 4, 42852281u, dark, output);
    if (!check(alternate.filter_coefficient < low_muffle_filter)) return 11;
    if (!check(different(output, comparison))) return 20;

    opxy::i32 tone[8] = {0x40000000,0x40000000,0x7fffffff,0x40000000,0,0,0x40000000,0x40000000};
    render_note(alternate, shared, 4, 42852281u, tone, output);
    const opxy::u32 pitch_at_neutral_tone = alternate.delta;
    const opxy::i32 neutral_tone_filter = alternate.filter_coefficient;
    for (unsigned i = 0; i < opxy::block_frames; ++i) comparison[i] = output[i];
    tone[4] = 0x7fffffff;
    render_note(alternate, shared, 4, 42852281u, tone, output);
    if (!check(alternate.delta == pitch_at_neutral_tone)) return 16;
    if (!check(alternate.filter_coefficient > neutral_tone_filter)) return 17;
    if (!check(different(output, comparison))) return 21;

    opxy::i32 unrandomized[8] = {0x40000000,0x40000000,0x7fffffff,0x40000000,0x40000000,0,0x40000000,0x40000000};
    render_note(alternate, shared, 4, 42852281u, unrandomized, output);
    const opxy::u32 unrandomized_step = alternate.delta;
    for (unsigned i = 0; i < opxy::block_frames; ++i) comparison[i] = output[i];
    unrandomized[5] = 0x7fffffff;
    render_note(alternate, shared, 5, 42852281u, unrandomized, output);
    if (!check(alternate.delta != unrandomized_step)) return 19;
    if (!check(different(output, comparison))) return 25;

    opxy::i32 short_tail[8] = {0x40000000,0x40000000,0,0,0x40000000,0,0x40000000,0x40000000};
    opxy::i32 long_tail[8] = {0x40000000,0x40000000,0,0x7fffffff,0x40000000,0,0x40000000,0x40000000};
    render_blocks(alternate, shared, 4, 42852281u, short_tail, comparison, 400);
    const opxy::i32 short_tail_coefficient = alternate.tail;
    render_blocks(alternate, shared, 4, 42852281u, long_tail, output, 400);
    if (!check(alternate.tail > short_tail_coefficient)) return 22;
    if (!check(different(output, comparison))) return 26;

    opxy::i32 tune[8] = {0x40000000,0x40000000,0x7fffffff,0x40000000,0x40000000,0,0x40000000,0x40000000};
    render_note(alternate, shared, 4, 42852281u, tune, output);
    const opxy::u32 neutral_tune_step = alternate.delta;
    for (unsigned i = 0; i < opxy::block_frames; ++i) comparison[i] = output[i];
    tune[7] = 0x7fffffff;
    render_note(alternate, shared, 4, 42852281u, tune, output);
    if (!check(alternate.delta > neutral_tune_step)) return 27;
    if (!check(different(output, comparison))) return 28;

    opxy::i32 stretch[8] = {0x40000000,0x40000000,0x7fffffff,0x40000000,0x40000000,0,0x40000000,0x40000000};
    render_note(alternate, shared, 4, kNoteIncrements[96 - 21], stretch, output);
    const opxy::u32 neutral_stretch_step = alternate.delta;
    for (unsigned i = 0; i < opxy::block_frames; ++i) comparison[i] = output[i];
    stretch[6] = 0x7fffffff;
    render_note(alternate, shared, 4, kNoteIncrements[96 - 21], stretch, output);
    if (!check(alternate.delta > neutral_stretch_step)) return 29;

    opxy::i32 hard[8] = {0x7fffffff,0x40000000,0x7fffffff,0x40000000,0x40000000,0x40000000,0x40000000,0x40000000};
    render_note(alternate, shared, 4, 42852281u, hard, output);
    if (!check(alternate.position <= alternate.end)) return 13;

    return 0;
}
