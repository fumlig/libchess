#ifndef CHESS_PIECE_HPP
#define CHESS_PIECE_HPP


#include "side.hpp"


namespace chess
{


const int pieces = 6;


enum piece
{
    piece_none      = -1,
    piece_pawn      = 0,
    piece_rook      = 1,
    piece_knight    = 2,
    piece_bishop    = 3,
    piece_queen     = 4,
    piece_king      = 5,
};


piece piece_from_san(char san, side* s)
{
    if(s != NULL) *s = san < 'a' ? side_white : side_black;
    switch(san)
    {
    case 'P':
    case 'p':
        return piece_pawn;
    case 'R':
    case 'r':
        return piece_rook;
    case 'N':
    case 'n':
        return piece_knight;
    case 'B':
    case 'b':
        return piece_bishop;
    case 'Q':
    case 'q':
        return piece_queen;
    case 'K':
    case 'k':
        return piece_king;
    default:
        return piece_none;
    }
}

char piece_to_san(piece p, side s)
{
    if(p == piece_none) return '.';

    static const char san_white[] = {'P', 'R', 'N', 'B', 'Q', 'K'};
    static const char san_black[] = {'p', 'r', 'n', 'b', 'q', 'k'};
    
    switch(s)
    {
    case side_white:
        return san_white[p];
    case side_black:
    case side_none:
    default:
        return san_black[p];
    }
}


}


#endif
