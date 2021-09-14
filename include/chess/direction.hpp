#ifndef DIRECTION_H
#define DIRECTION_H


#include <stdio.h>

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

enum direction direction_opposite(enum direction d);
enum direction direction_forward(enum side s);

void direction_print(enum direction d);


enum direction direction_opposite(enum direction d)
{
    return static_cast<direction>((d + 8) % 16);
}

enum direction direction_forward(enum side s)
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


void direction_print(enum direction d)
{
    switch(d)
    {
    case DIRECTION_N:   printf("n");    break;
    case DIRECTION_NNE: printf("nne");  break;
    case DIRECTION_NE:  printf("ne");   break;
    case DIRECTION_ENE: printf("ene");  break;
    case DIRECTION_E:   printf("e");    break;
    case DIRECTION_ESE: printf("ese");  break;
    case DIRECTION_SE:  printf("se");   break;
    case DIRECTION_SSE: printf("sse");  break;
    case DIRECTION_S:   printf("s");    break;
    case DIRECTION_SSW: printf("ssw");  break;
    case DIRECTION_SW:  printf("sw");   break;
    case DIRECTION_WSW: printf("wsw");  break;
    case DIRECTION_W:   printf("w");    break;
    case DIRECTION_WNW: printf("wnw");  break;
    case DIRECTION_NW:  printf("nw");   break;
    case DIRECTION_NNW: printf("nnw");  break;
    default: break;
    }
}


}


#endif
