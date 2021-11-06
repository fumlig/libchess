#ifndef CHESS_PIECE_HPP
#define CHESS_PIECE_HPP


#include <utility>
#include <string>
#include <stdexcept>

#include "side.hpp"


namespace chess
{



/// Pieces in chess.
///
/// In some places, a none-piece is useful (for example for empty board squares).
enum piece
{
    piece_pawn,
    piece_rook,
    piece_knight,
    piece_bishop,
    piece_queen,
    piece_king,
    piece_none = -1,
};

/// Number of pieces.
const int pieces = 6;

/// Piece and side from Short Algebraic Notation (SAN).
///
/// Pawn is 'P', rook is 'R', knight is 'N', bishop is 'B', queen is 'Q'
/// and king is 'K'. An uppercase letter indicates that it is a white piece 
/// and a lowercase indicates letter that it is a black piece.
///
/// \param san SAN of for piece.
/// \returns Side and piece type
/// \throws Invalid argument if SAN does not denote a piece.
std::pair<side, piece> piece_from_san(char san);


/// Piece and side to Short Algebraic Notation (SAN).
///
/// Pawn is 'P', rook is 'R', knight is 'N', bishop is 'B', queen is 'Q'
/// and king is 'K'. An uppercase letter indicates that it is a white piece 
/// and a lowercase indicates letter that it is a black piece.
///
/// \param s The side of the piece.
/// \param p The type of the piece.
/// \returns SAN for piece of given side.
char piece_to_san(side s, piece p);


}


#endif