#ifndef CHESS_ZOBRIST_HPP
#define CHESS_ZOBRIST_HPP


#include <array>
#include <cstdint>

#include "side.hpp"
#include "piece.hpp"
#include "square.hpp"
#include "random.hpp"


namespace chess
{


std::size_t zobrist_piece_key(square sq, side s, piece p);
std::size_t zobrist_kingside_castle_key(side s);
std::size_t zobrist_queenside_castle_key(side s);
std::size_t zobrist_en_passant_key(file f);
std::size_t zobrist_side_key();


void zobrist_init(random& rng);


}


#endif