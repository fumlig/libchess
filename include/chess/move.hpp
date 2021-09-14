#ifndef CHESS_MOVE_HPP
#define CHESS_MOVE_HPP


#include <iostream>

#include "move.hpp"
#include "board.hpp"
#include "bitboard.hpp"


namespace chess
{


#define MOVE_UCI_LENGTH 6 // "d2d4", "a7a8q"


struct position;


struct move
{
    square from;
    square to;
    piece promote;
};

struct undo
{
    // irreversible information of a move
    piece capture;
    square en_passant;
    bool kingside_castle[sides];
    bool queenside_castle[sides];
};


void move_init(move* m, square from, square to, piece promote);


int move_list_piecewise(square from, bitboard tos, piece promote, move* moves);
int move_list_setwise(bitboard froms, bitboard tos, piece promote, move* moves);



void move_init(move* m, square from, square to, piece promote)
{
    m->from = from;
    m->to = to;
    m->promote = promote;
}


int move_list_piecewise(square from, bitboard tos, piece promote, move* moves)
{
    int m = 0;
    while(tos)
    {
        square to = bitboard_lsb(tos);
        tos = bitboard_reset(tos, to);
        move_init(&moves[m++], from, to, promote);
    }
    return m;
}

int move_list_setwise(bitboard froms, bitboard tos, piece promote, move* moves)
{
    int m = 0;
    while(froms && tos)
    {
        square from = bitboard_lsb(froms);
        square to = bitboard_lsb(tos);
        froms = bitboard_reset(froms, from);
        tos = bitboard_reset(tos, to);
        move_init(&moves[m++], from, to, promote);
    }
    return m;
}



}


#endif
