#ifndef CHESS_MOVE_HPP
#define CHESS_MOVE_HPP


#include <iostream>
#include <array>
#include <vector>

#include "move.hpp"
#include "board.hpp"
#include "bitboard.hpp"


namespace chess
{


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
    std::array<bool, sides> kingside_castle;
    std::array<bool, sides> queenside_castle;
};

}


#endif
