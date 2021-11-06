#include <bit>
#include <cstdint>
#include <vector>

#include "direction.hpp"
#include "random.hpp"
#include "square.hpp"
#include "set.hpp"


namespace chess
{

// todo: extern magic files


std::vector<square> set_elements(bitboard bb)
{
    std::vector<square> elements;

    while(bb)
    {
        square sq = set_first(bb);
        elements.push_back(sq);
        bb = set_erase(bb, sq);
    }

    return elements;
}

bitboard set_shift(bitboard bb, direction d)
{
    if(d > 0)
    {
        bb <<= d;
    }
    else
    {
        bb >>= -d;
    }

    bitboard trim = empty_set;

    switch(d)
    {
    case direction_e:
    case direction_ne:
    case direction_se:
    case direction_nne:
    case direction_sse:
        trim = file_set(file_a);
        break;
    case direction_w:
    case direction_nw:
    case direction_sw:
    case direction_nnw:
    case direction_ssw:
        trim = file_set(file_h);
        break;
    case direction_ene:
    case direction_ese:
        trim = file_set(file_a) | file_set(file_b);
        break;
    case direction_wnw:
    case direction_wsw:
        trim = file_set(file_g) | file_set(file_h);
        break;
    default:
        break;
    }

    return bb & ~trim;
}


bitboard set_ray(bitboard bb, direction d, bitboard occupied)
{
    bitboard shift = bb;
    bitboard ray = 0;
    while(shift != 0 && !(shift & occupied))
    {
        shift = set_shift(shift, d);
        ray |= shift;
    }
    return ray;
}

}
