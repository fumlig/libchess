#ifndef CHESS_DIRECTION_HPP
#define CHESS_DIRECTION_HPP


#include "side.hpp"


namespace chess
{


/// Directions of piece movement.
///
/// Kings move north, east, south and west. Knights move north-northeast,
/// east-northeast, etc.
enum direction
{
    direction_n     = 8,
    direction_e     = 1,
    direction_s     = -8,
    direction_w     = -1,
    
	direction_ne    = direction_n + direction_e,
    direction_se    = direction_s + direction_e,
    direction_sw    = direction_s + direction_w,
    direction_nw    = direction_n + direction_w,
    
	direction_nne   = direction_n + direction_n + direction_e,
    direction_ene   = direction_e + direction_n + direction_e,
    direction_ese   = direction_e + direction_s + direction_e,
    direction_sse   = direction_s + direction_s + direction_e,
    direction_ssw   = direction_s + direction_s + direction_w,
    direction_wsw   = direction_w + direction_s + direction_w,
    direction_wnw   = direction_w + direction_n + direction_w,
    direction_nnw   = direction_n + direction_n + direction_w,

    direction_none  = 0,
};

/// Number of directions.
const int directions = 16;

/// Opposite of given direction.
///
/// For example, the oppsoite of north is south.
///
/// \param d Direction to get opposite of.
/// \returns Opposite direction. 
direction opposite(direction d);

/// Forward direction for given side.
///
/// This assumes the board is seen from white's perspective.
///
/// \param s The side.
/// \returns Forwards for given side.
direction forwards(side s);


}


#endif