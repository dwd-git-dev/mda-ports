#pragma once
#include <opxy/page_header.hpp>

namespace mdarecorderteno {
// Matches resources.display in definition.json: paths, points, nodes, texts, palette.
using Frame = opxy::draw::Frame<4, 64, 113, 18, 16>;
struct DisplayState {
    opxy::draw::PageHeader header;
    opxy::u32 phase, spiral_key, track, initialized, chord_serial;
    opxy::u32 previous[4];
    opxy::u32 chord_notes[4];
    opxy::u16 note_age[88];
    opxy::u16 release_age[88];
    opxy::u16 release_level[88];
    opxy::u16 envelope_tau[88];
};
static_assert(sizeof(DisplayState) == 776, "Display state size differs from definition.json");
}
