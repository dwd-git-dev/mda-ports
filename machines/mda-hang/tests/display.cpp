#include "../display.cpp"

namespace {
using opxy::draw::unit;

mdahang::DisplayState state;
mdahang::Frame frame;
opxy::draw::Input input;

bool check(bool condition) {
    return condition;
}

void set_pitch(opxy::draw::Input& input, opxy::u32 note) {
    input.held_pitches[note / 32] |= 1u << (note % 32);
    ++input.held_notes;
}
}

int main() {
    input.track = 2;
    input.elapsed = 50;
    input.held_notes = 0;
    input.slots[0] = 0x40000000;
    input.slots[1] = 0x60000000;
    input.slots[2] = 0x40000000;
    input.slots[3] = 0x40000000;
    input.slots[4] = 0x40000000;
    input.slots[5] = 0;
    input.slots[6] = 0x40000000;
    input.slots[7] = 0x40000000;
    mdahang::display_reset(state);
    mdahang::display(state, input, frame);
    if (!check(opxy::draw::valid(frame))) return 1;
    if (!check(frame.paths[0].points[0].x < 3 * unit)) return 54;
    if (!check(frame.paths[0].points[63].x == 0 &&
               frame.paths[0].points[63].y == 0)) return 55;
    if (!check(frame.nodes[100].x == opxy::draw::hidden_x * unit)) return 22;
    for (opxy::u32 segment = 0; segment < 4; ++segment)
        if (!check(frame.nodes[segment].x == opxy::draw::hidden_x * unit &&
                   frame.nodes[segment].paint == opxy::draw::keep)) return 49;
    for (opxy::u32 segment = 0; segment < 4; ++segment) {
        for (opxy::u32 i = 0; i < 31; ++i) {
            const opxy::draw::Point& outer_a = frame.paths[segment].points[i];
            const opxy::draw::Point& outer_b = frame.paths[segment].points[i + 1];
            const opxy::draw::Point& inner_a = frame.paths[segment].points[63 - i];
            const opxy::draw::Point& inner_b = frame.paths[segment].points[62 - i];
            const opxy::i32 outer_dx = outer_b.x - outer_a.x;
            const opxy::i32 outer_dy = outer_b.y - outer_a.y;
            const opxy::i32 inner_dx = inner_b.x - inner_a.x;
            const opxy::i32 inner_dy = inner_b.y - inner_a.y;
            const opxy::i32 outer_screen_dx = outer_dx * mdahang::kSpiralScale / unit;
            const opxy::i32 outer_screen_dy = outer_dy * mdahang::kSpiralScale / unit;
            const opxy::i32 inner_screen_dx = inner_dx * mdahang::kSpiralScale / unit;
            const opxy::i32 inner_screen_dy = inner_dy * mdahang::kSpiralScale / unit;
            if (!check(outer_screen_dx * outer_screen_dx +
                       outer_screen_dy * outer_screen_dy < 625 * unit * unit)) return 47;
            if (!check(inner_screen_dx * inner_screen_dx +
                       inner_screen_dy * inner_screen_dy < 625 * unit * unit)) return 48;
        }
    }
    for (opxy::u32 segment = 0; segment < 3; ++segment) {
        const opxy::draw::Point& outer_end = frame.paths[segment].points[31];
        const opxy::draw::Point& outer_start = frame.paths[segment + 1].points[0];
        const opxy::draw::Point& inner_end = frame.paths[segment].points[32];
        const opxy::draw::Point& inner_start = frame.paths[segment + 1].points[63];
        if (!check(outer_end.x == outer_start.x && outer_end.y == outer_start.y)) return 35;
        if (!check(inner_end.x == inner_start.x && inner_end.y == inner_start.y)) return 36;
    }

    input.held_pitches[0] = 1u << 24;
    input.held_notes = 1;
    input.note_serial = 1;
    mdahang::display(state, input, frame);
    if (!check(opxy::draw::valid(frame))) return 2;
    if (!check(frame.nodes[12 + 24 - 21].x == 240 * unit)) return 3;
    if (!check(frame.nodes[12 + 24 - 21].scale >= 640)) return 52;

    input.held_pitches[0] |= 1u << 25;
    input.held_notes = 2;
    mdahang::display(state, input, frame);
    if (!check(frame.nodes[100].x == opxy::draw::hidden_x * unit)) return 23;

    input.held_pitches[0] = 0;
    input.held_notes = 0;
    for (opxy::u32 note = 24; note <= 108; note += 12) set_pitch(input, note);
    input.note_serial = 2;
    mdahang::display(state, input, frame);
    if (!check(opxy::draw::valid(frame))) return 4;
    for (opxy::u32 note = 24; note <= 108; note += 12) {
        const opxy::draw::Node& node = frame.nodes[12 + note - 21];
        if (!check(node.x != opxy::draw::hidden_x * unit)) return 5;
    }
    for (opxy::u32 note = 24; note <= 108; note += 12) {
        const opxy::draw::Node& c_node = frame.nodes[12 + note - 21];
        if (!check(c_node.x == 240 * unit)) return 6;
        if (!check(c_node.y < 129 * unit)) return 53;
    }
    if (!check(frame.nodes[100].x != opxy::draw::hidden_x * unit)) return 7;
    if (!check(frame.nodes[110].x == opxy::draw::hidden_x * unit)) return 8;

    for (opxy::u32 word = 0; word < 4; ++word) input.held_pitches[word] = 0;
    input.held_notes = 0;
    for (opxy::u32 note = 21; note <= 108; ++note) set_pitch(input, note);
    input.note_serial = 3;
    mdahang::display(state, input, frame);
    if (!check(opxy::draw::valid(frame))) return 9;
    for (opxy::u32 i = 0; i < 88; ++i)
        if (!check(frame.nodes[12 + i].x != opxy::draw::hidden_x * unit)) return 10;
    input.slots[0] = 0x7fffffff;
    input.slots[5] = 0x7fffffff;
    mdahang::display(state, input, frame);
    if (!check(opxy::draw::valid(frame))) return 24;

    input.held_pitches[0] = 0;
    input.held_pitches[1] = 0;
    input.held_pitches[2] = 1u << (65 % 32);
    input.held_pitches[3] = 0;
    input.held_notes = 1;
    input.note_serial = 4;
    input.slots[3] = 0;
    mdahang::display(state, input, frame);
    input.held_pitches[0] = 0;
    input.held_pitches[2] = 0;
    input.held_notes = 0;
    input.note_off_serial = 1;
    mdahang::display(state, input, frame);
    if (!check(frame.nodes[12 + 65 - 21].x != opxy::draw::hidden_x * unit)) return 11;
    input.elapsed = 102;
    for (opxy::u32 i = 0; i < 13; ++i) mdahang::display(state, input, frame);
    if (!check(frame.nodes[12 + 65 - 21].x == opxy::draw::hidden_x * unit)) return 12;
    if (!check(opxy::draw::valid(frame))) return 13;

    for (opxy::u32 word = 0; word < 4; ++word) input.held_pitches[word] = 0;
    for (opxy::u32 slot = 0; slot < 8; ++slot) input.slots[slot] = 0;
    input.held_notes = 0;
    input.reduced_motion = 1;
    set_pitch(input, 60);
    input.held_notes = 1;
    input.elapsed = 0;
    mdahang::display_reset(state);
    mdahang::display(state, input, frame);
    const opxy::i32 fast_decay_start = frame.nodes[12 + 60 - 21].scale;
    const opxy::u32 bright_decay_start = frame.nodes[12 + 60 - 21].shade;
    input.elapsed = 102;
    mdahang::display(state, input, frame);
    const opxy::i32 fast_decay_scale = frame.nodes[12 + 60 - 21].scale;
    for (opxy::u32 i = 0; i < 8 &&
         frame.nodes[12 + 60 - 21].shade >= bright_decay_start; ++i)
        mdahang::display(state, input, frame);
    if (!check(fast_decay_scale < fast_decay_start)) return 25;
    if (!check(frame.nodes[12 + 60 - 21].shade < bright_decay_start)) return 30;
    for (opxy::u32 i = 0; i < 15; ++i) mdahang::display(state, input, frame);
    if (!check(frame.nodes[12 + 60 - 21].x == opxy::draw::hidden_x * unit)) return 31;
    if (!check(input.held_pitches[60 / 32] & (1u << (60 % 32))) ||
        !check(input.held_notes == 1)) return 32;

    input.slots[2] = 0x7fffffff;
    mdahang::display_reset(state);
    input.elapsed = 0;
    mdahang::display(state, input, frame);
    for (opxy::u32 i = 0; i < 1; ++i) {
        input.elapsed = 102;
        mdahang::display(state, input, frame);
    }
    if (!check(frame.nodes[12 + 60 - 21].scale > fast_decay_scale)) return 26;
    for (opxy::u32 i = 0; i < 30; ++i) mdahang::display(state, input, frame);
    if (!check(frame.nodes[12 + 60 - 21].x == opxy::draw::hidden_x * unit)) return 33;

    for (opxy::u32 word = 0; word < 4; ++word) input.held_pitches[word] = 0;
    input.held_notes = 0;
    input.slots[3] = 0;
    input.elapsed = 0;
    mdahang::display_reset(state);
    mdahang::display(state, input, frame);
    set_pitch(input, 60);
    input.held_notes = 1;
    mdahang::display(state, input, frame);
    input.held_pitches[60 / 32] &= ~(1u << (60 % 32));
    input.held_notes = 0;
    mdahang::display(state, input, frame);
    for (opxy::u32 i = 0; i < 5; ++i) {
        input.elapsed = 102;
        mdahang::display(state, input, frame);
    }
    if (!check(frame.nodes[12 + 60 - 21].x == opxy::draw::hidden_x * unit)) return 27;

    input.slots[3] = 0x7fffffff;
    input.elapsed = 0;
    mdahang::display_reset(state);
    mdahang::display(state, input, frame);
    set_pitch(input, 60);
    input.held_notes = 1;
    mdahang::display(state, input, frame);
    input.held_pitches[60 / 32] &= ~(1u << (60 % 32));
    input.held_notes = 0;
    mdahang::display(state, input, frame);
    for (opxy::u32 i = 0; i < 5; ++i) {
        input.elapsed = 102;
        mdahang::display(state, input, frame);
    }
    if (!check(frame.nodes[12 + 60 - 21].x != opxy::draw::hidden_x * unit)) return 28;
    for (opxy::u32 i = 0; i < 20; ++i) mdahang::display(state, input, frame);
    if (!check(frame.nodes[12 + 60 - 21].x == opxy::draw::hidden_x * unit)) return 38;
    if (!check(opxy::draw::valid(frame))) return 29;

    mdahang::display_reset(state);
    for (opxy::u32 word = 0; word < 4; ++word) input.held_pitches[word] = 0;
    for (opxy::u32 slot = 0; slot < 8; ++slot) input.slots[slot] = 0;
    input.held_notes = 0;
    input.note_off_serial = 0;
    input.note_serial = 10;
    input.reduced_motion = 1;
    input.elapsed = 0;
    set_pitch(input, 27);
    mdahang::display(state, input, frame);
    const opxy::i32 base_x = frame.nodes[12 + 27 - 21].x;
    const opxy::i32 base_y = frame.nodes[12 + 27 - 21].y;
    const opxy::i32 base_scale = frame.nodes[12 + 27 - 21].scale;
    const opxy::u32 base_shade = frame.nodes[12 + 27 - 21].shade;
    const opxy::draw::Point base_line = frame.paths[0].points[3];

    input.slots[0] = 0x7fffffff;
    mdahang::display(state, input, frame);
    if (!check(frame.paths[0].points[3].x == base_line.x &&
               frame.paths[0].points[3].y == base_line.y)) return 14;
    if (!check(frame.nodes[12 + 27 - 21].x != base_x ||
               frame.nodes[12 + 27 - 21].y != base_y)) return 37;
    input.slots[0] = 0;
    input.slots[1] = 0x7fffffff;
    mdahang::display(state, input, frame);
    if (!check(frame.nodes[12 + 27 - 21].shade != base_shade)) return 15;
    input.slots[1] = 0;
    input.slots[2] = 0x7fffffff;
    mdahang::display(state, input, frame);
    if (!check(frame.nodes[12 + 27 - 21].scale != base_scale)) return 16;
    input.slots[2] = 0;
    input.slots[4] = 0x7fffffff;
    mdahang::display(state, input, frame);
    if (!check(frame.nodes[12 + 27 - 21].scale != base_scale)) return 17;
    input.slots[4] = 0;
    input.slots[5] = 0x7fffffff;
    mdahang::display(state, input, frame);
    if (!check(frame.nodes[12 + 27 - 21].x != base_x ||
               frame.nodes[12 + 27 - 21].y != base_y)) return 18;
    input.slots[5] = 0;
    input.slots[6] = 0x7fffffff;
    mdahang::display(state, input, frame);
    if (!check(frame.nodes[12 + 27 - 21].shade != base_shade)) return 19;
    input.slots[6] = 0;
    input.slots[7] = 0x7fffffff;
    mdahang::display(state, input, frame);
    if (!check(frame.nodes[12 + 27 - 21].shade != base_shade)) return 20;

    input.slots[7] = 0;
    input.slots[3] = 0;
    input.held_pitches[0] = 0;
    input.held_notes = 0;
    input.note_off_serial = 1;
    mdahang::display(state, input, frame);
    input.slots[3] = 0x7fffffff;
    input.elapsed = 102;
    mdahang::display(state, input, frame);
    if (!check(frame.nodes[12 + 27 - 21].x != opxy::draw::hidden_x * unit)) return 21;

    mdahang::display_reset(state);
    for (opxy::u32 word = 0; word < 4; ++word) input.held_pitches[word] = 0;
    for (opxy::u32 slot = 0; slot < 8; ++slot) input.slots[slot] = 0;
    input.held_notes = 0;
    input.note_serial = 100;
    input.elapsed = 0;
    input.reduced_motion = 1;
    mdahang::display(state, input, frame);
    const opxy::u32 close_notes[3] = {60, 64, 67};
    for (opxy::u32 i = 0; i < 3; ++i) {
        set_pitch(input, close_notes[i]);
        ++input.note_serial;
        mdahang::display(state, input, frame);
    }
    for (opxy::u32 i = 0; i < 3; ++i) {
        if (!check(frame.nodes[100 + i].x != opxy::draw::hidden_x * unit)) return 39;
        if (!check(frame.nodes[100 + i].shade == 15)) return 40;
    }
    for (opxy::u32 i = 3; i < 10; ++i)
        if (!check(frame.nodes[100 + i].x == opxy::draw::hidden_x * unit)) return 41;
    input.elapsed = 102;
    mdahang::display(state, input, frame);
    if (!check(frame.nodes[100].x != opxy::draw::hidden_x * unit)) return 42;
    if (!check(frame.nodes[100].shade < 15 && frame.nodes[100].shade >= 11)) return 45;
    for (opxy::u32 i = 0; i < 8; ++i) {
        input.elapsed = 102;
        mdahang::display(state, input, frame);
    }
    for (opxy::u32 i = 0; i < 10; ++i) {
        if (frame.nodes[12 + close_notes[i % 3] - 21].x == opxy::draw::hidden_x * unit) break;
        input.elapsed = 102;
        mdahang::display(state, input, frame);
    }
    for (opxy::u32 i = 0; i < 3; ++i)
        if (!check(frame.nodes[12 + close_notes[i] - 21].x == opxy::draw::hidden_x * unit)) return 46;
    for (opxy::u32 i = 0; i < 3; ++i)
        if (!check(frame.nodes[100 + i].x == opxy::draw::hidden_x * unit)) return 42;

    mdahang::display_reset(state);
    for (opxy::u32 word = 0; word < 4; ++word) input.held_pitches[word] = 0;
    input.held_notes = 0;
    input.note_serial = 200;
    input.elapsed = 0;
    mdahang::display(state, input, frame);
    const opxy::u32 distant_notes[3] = {96, 102, 108};
    for (opxy::u32 i = 0; i < 3; ++i) {
        set_pitch(input, distant_notes[i]);
        ++input.note_serial;
        mdahang::display(state, input, frame);
    }
    for (opxy::u32 i = 0; i < 10; ++i)
        if (!check(frame.nodes[100 + i].x == opxy::draw::hidden_x * unit)) return 43;
    if (!check(opxy::draw::valid(frame))) return 44;

    if (!check(opxy::draw::valid(frame))) return 44;
    // Numeral/progression/eyes legacy feature removed.
    return 0;
}
