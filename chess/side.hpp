#ifndef CHESS_SIDE_HPP
#define CHESS_SIDE_HPP


#include <string>


namespace chess
{


/// Sides in chess.
///
/// In some places, a none-side is useful (for example for empty board squares).
enum side
{
    side_white,
    side_black,
    side_none = -1,
};

/// Number of sides.
const int sides = 2;

/// Opponent of given side.
///
/// The opponent of white is black and vice versa.
///
/// \param s Side to get opponent of.
/// \returns Opponent of side.
constexpr side opponent(side s)
{
    return static_cast<side>(!s);
}


std::string to_string(side s);


}


#endif