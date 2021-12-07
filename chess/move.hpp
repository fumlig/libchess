#ifndef CHESS_MOVE_HPP
#define CHESS_MOVE_HPP


#include <array>
#include <string>

#include "piece.hpp"
#include "square.hpp"


namespace chess
{


/// Chess move.
///
/// Contains all information needed to make a move.
struct move
{
	/// Null-move.
    ///
    /// Constructs move which does nothing. Should probably not be used.
	move();

    /// Move constructor.
    ///
    /// Constructs move from given square to given square. If the move is a
    /// pawn push or attack to the final rank for the side making the move,
    /// a promotion piece should be specified.
    ///
    /// \param from Source square.
    /// \param to Destination square.
    /// \param promote Promotion piece.
	move(square from, square to, piece promote = piece_queen);

	// todo: implement
    //static move from_san(std::string_view san, const position& p);

    /// Move from Long Algebraic Notation (LAN).
    ///
    /// Create move from LAN. "a2a4" is the move from square A2 to A4.
    /// "h7h8q" is a promotion to a queen on the H file.
    ///
    /// \param lan LAN string.
    /// \returns Move corresponding to LAN.
    /// \throws Invalid argument if LAN does not denote a move.
	static move from_lan(std::string_view lan);

    /// Move to Long Algebraic Notation (LAN).
    ///
    /// Covnert move to LAN. "a2a4" is the move from square A2 to A4.
    /// "h8h1q" is a promotion to a queen on the H file.
    ///
    /// \returns LAN of move.
	std::string to_lan() const;

    bool is_null() const;

    //auto operator<=>(const move&) const = default;

    square from;
    square to;
    piece promote;
};

/// Chess move undo.
///
/// Contains all information needed to undo a move on a given position. This
/// is the information that can not be deduced from the position state and move
/// information alone.
struct undo
{
    piece capture;
    square en_passant;
    std::array<bool, sides> kingside_castle;
    std::array<bool, sides> queenside_castle;
    int halfmove_clock;
};


}


#endif