#ifndef CHESS_DIRECTION_HPP
#define CHESS_DIRECTION_HPP


#include "board.hpp"


namespace chess
{


const int directions = 16;

enum direction
{
    direction_none  = 0,
    direction_n     = +8,
    direction_ne    = +9,
    direction_e     = +1,
    direction_se    = -7,
    direction_s     = -8,
    direction_sw    = -9,
    direction_w     = -1,
    direction_nw    = +7,
    direction_nne   = +17,
    direction_ene   = +10,
    direction_ese   = -6,
    direction_sse   = -15,
    direction_ssw   = -17,
    direction_wsw   = -10,
    direction_wnw   = +6,
    direction_nnw   = +15
};

direction direction_opposite(direction d)
{
    return static_cast<direction>((d + 8) % 16);
}

direction direction_forward(side s)
{
    switch(s)
    {
    case side_white:
        return direction_n;
    case side_black:
        return direction_s;
    case side_none:
    default:
        return direction_none;
    }
}


}


#endif
