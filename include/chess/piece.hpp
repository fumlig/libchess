#ifndef PIECE_H
#define PIECE_H


#include <stdlib.h>

#include "side.hpp"


namespace chess
{


#define PIECES  6


enum piece
{
    PIECE_NONE      = -1,
    PIECE_PAWN      = 0,
    PIECE_ROOK      = 1,
    PIECE_KNIGHT    = 2,
    PIECE_BISHOP    = 3,
    PIECE_QUEEN     = 4,
    PIECE_KING      = 5,
};


piece piece_from_san(char san, side* s)
{
    if(s != NULL) *s = san < 'a' ? SIDE_WHITE : SIDE_BLACK;
    switch(san)
    {
    case 'P':
    case 'p':
        return PIECE_PAWN;
    case 'R':
    case 'r':
        return PIECE_ROOK;
    case 'N':
    case 'n':
        return PIECE_KNIGHT;
    case 'B':
    case 'b':
        return PIECE_BISHOP;
    case 'Q':
    case 'q':
        return PIECE_QUEEN;
    case 'K':
    case 'k':
        return PIECE_KING;
    default:
        return PIECE_NONE;
    }
}

char piece_to_san(piece p, side s)
{
    if(p == PIECE_NONE) return '.';

    static const char san_white[] = {'P', 'R', 'N', 'B', 'Q', 'K'};
    static const char san_black[] = {'p', 'r', 'n', 'b', 'q', 'k'};
    
    switch(s)
    {
    case SIDE_WHITE:
    case SIDE_NONE:
    default:
        return san_white[p];
    case SIDE_BLACK:
        return san_black[p];
    }
}

const char* piece_to_fan(piece p, side s)
{
    if(p == PIECE_NONE) return ".";

    static const char* san_white[] = {"♟", "♜", "♞", "♝", "♛", "♚"};
    static const char* san_black[] = {"♙", "♖", "♘", "♗", "♕", "♔"};
    
    switch(s)
    {
    case SIDE_WHITE:
    case SIDE_NONE:
    default:
        return san_white[p];
    case SIDE_BLACK:
        return san_black[p];
    }
}


}


#endif
