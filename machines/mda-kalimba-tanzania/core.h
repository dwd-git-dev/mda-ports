#pragma once
#include <opxy/audio.hpp>

namespace mdakalimbatanza {
struct Voice {
    opxy::u32 position, fraction, delta, end, loop, note, initialized, random_seed;
    opxy::i32 envelope, decay, tail, filter, previous, filter_coefficient, gain;
    // Up to 4 time segments: [start,b1)@1, [b1,b2)@f1, [b2,b3)@f2, [b3,end]@f3.
    opxy::u32 b1, f1, b2, f2, b3, f3;
};
struct Shared { opxy::u32 random_seed; };
}