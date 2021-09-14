#ifndef CHESS_DIRECTION_HPP
#define CHESS_DIRECTION_HPP


#include "board.hpp"


namespace chess
{


#define DIRECTIONS 16

enum direction
{
    DIRECTION_NONE  = 0,
    DIRECTION_N     = +8,
    DIRECTION_NE    = +9,
    DIRECTION_E     = +1,
    DIRECTION_SE    = -7,
    DIRECTION_S     = -8,
    DIRECTION_SW    = -9,
    DIRECTION_W     = -1,
    DIRECTION_NW    = +7,
    DIRECTION_NNE   = +17,
    DIRECTION_ENE   = +10,
    DIRECTION_ESE   = -6,
    DIRECTION_SSE   = -15,
    DIRECTION_SSW   = -17,
    DIRECTION_WSW   = -10,
    DIRECTION_WNW   = +6,
    DIRECTION_NNW   = +15
};

direction direction_opposite(direction d)
{
    return static_cast<direction>((d + 8) % 16);
}

direction direction_forward(side s)
{
    switch(s)
    {
    case SIDE_WHITE:
        return DIRECTION_N;
    case SIDE_BLACK:
        return DIRECTION_S;
    case SIDE_NONE:
    default:
        return DIRECTION_NONE;
    }
}


}


#endif
