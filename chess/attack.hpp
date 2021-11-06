#ifndef CHESS_ATTACK_HPP
#define CHESS_ATTACK_HPP


#include "set.hpp"


namespace chess
{


void attack_init(random& rng);


/// Set of all east pawn attacks.
///
/// Given a set of pawns of a given side, returns the set of squares attacked 
/// in the east direction by those pawns.
///
/// \param bb Set of pawns.
/// \param s Side of pawns.
/// \returns Attacked squares.
bitboard pawn_east_attack_set(bitboard bb, side s);


/// Set of all west pawn attacks.
///
/// Given a set of pawns of a given side, returns the set of squares attacked 
/// in the west direction by those pawns.
///
/// \param bb Set of pawns.
/// \param s Side of pawns.
/// \returns Attacked squares.
bitboard pawn_west_attack_set(bitboard bb, side s);


/// Set of all rook attacks.
///
/// Given a rook position and occupied squares, returns the set of squares 
/// attacked by the rook.
///
/// \param sq Position of rook.
/// \param occupied Set of occupied squares.
/// \returns Attacked squares.
bitboard rook_attack_set(square sq, bitboard occupied);


/// Set of all knight attacks.
///
/// Given a knight, returns the set of squares attacked by the knight.
///
/// \param sq Position of knight.
/// \returns Attacked squares.
bitboard knight_attack_set(square sq);


/// Set of all bishop attacks.
///
/// Given a bishop position and occupied squares, returns the set of squares 
/// attacked by the bishop.
///
/// \param sq Position of bishop.
/// \param occupied Set of occupied squares.
/// \returns Attacked squares.
bitboard bishop_attack_set(square sq, bitboard occupied);


/// Set of all queen attacks.
///
/// Given a queen position and occupied squares, returns the set of squares 
/// attacked by the queen.
///
/// \param sq Position of queen.
/// \param occupied Set of occupied squares.
/// \returns Attacked squares.
bitboard queen_attack_set(square sq, bitboard occupied);


/// Set of all king attacks.
///
/// Given the position of a king, returns the set of squares attacked by the king.
///
/// \param sq Position of king.
/// \returns Attacked squares.
bitboard king_attack_set(square sq);


}


#endif