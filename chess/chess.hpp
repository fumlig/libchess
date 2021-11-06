#ifndef CHESS_HPP
#define CHESS_HPP


#include "board.hpp"
#include "direction.hpp"
#include "game.hpp"
#include "attack.hpp"
#include "move.hpp"
#include "piece.hpp"
#include "position.hpp"
#include "random.hpp"
#include "set.hpp"
#include "side.hpp"
#include "square.hpp"
#include "zobrist.hpp"


namespace chess
{ 


/// Initialize library.
///
/// Sets up internal state such as attack tables and Zobrist hash keys.
/// The default seed seems to work fine but there might exist one that is both
/// faster and yields better pseudorandom numbers.
void init(std::size_t seed = 2147483647ULL);


}


#endif