#ifndef CHESS_BOARD_HPP
#define CHESS_BOARD_HPP


#include <string>
#include <utility>
#include <unordered_map>

#include "piece.hpp"
#include "square.hpp"
#include "set.hpp"
#include "zobrist.hpp"


namespace chess
{


/// Chess board.
///
/// Data structure that holds placement of pieces with efficient retrieval
/// of certain sets, such as which squares are occupied by which side.
/// In most cases, a chess position should be used instead since it holds more
/// information.
class board
{
public:
    /// Default board.
    ///
    /// Construct board with pieces in their initial positions.
    board();

    /// Arbitrary board.
    ///
    /// Construct board with the given piece placement. Takes a map from
    /// squares to piece and color and places the given pieces at the given
    /// square.
    ///
    /// \param pieces The piece placement.
    board(const std::unordered_map<square, std::pair<side, piece>>& pieces);

    /// Get side and piece at the given square.
    ///
    /// Returns the side and piece at the given square. Both side and piece
    /// can be none, in which case the square is empty.
    ///
    /// \param sq The square.
    /// \returns Pair of side and piece of given square.
    std::pair<side, piece> get(square sq) const;

    /// Set side and piece at the given square.
    ///
    /// Sets the side and piece of given square, and updates other internal
    /// state accordingly.
    /// \param sq The square.
    /// \param s New side of square.
    /// \param p New piece of square.
    void set(square sq, side s, piece p);

    /// Clear board.
    ///
    /// Sets the side and piece of all squares to none, and updates
    /// other internal state accordingly.
    void clear();

    /// Side set.
    ///
    /// Returns the set of all squares with pieces of the given side on them.
    ///
    /// \param s The side.
    /// \returns The set of squares.
    bitboard side_set(side s) const;

    /// Piece set.
    ///
    /// Returns the set of all squares with the given piece type on them.
    /// A side can optionally be specified to only get pieces of one side.
    ///
    /// \param p The piece.
    /// \param s The side.
    /// \returns The set of squares.
    bitboard piece_set(piece p, side s = side_none) const;

    /// Occupied set.
    ///
    /// Returns the set of all squares occupied by pieces of any side.
    ///
    /// \returns Occupied squares.
    bitboard occupied_set() const;

    /// Attack set.
    ///
    /// Returns the set of all squares attacked by a given side.
    ///
    /// \param s The side.
    /// \returns Squares attacked by side.
    bitboard attack_set(side s) const;

    /// Board hash.
    ///
    /// Returns the Zobrist hash of the board (piece placement).
    ///
    /// \returns Zobrist hash.
    std::size_t hash() const;

    /// Board to string.
    ///
    /// Returns (pretty) string representation of board, one rank per row.
    /// Starts at top left corner of board (square A8)
    ///
    /// \param coords Show coordinates.
    /// \returns Board string.
    std::string to_string(bool coords = true) const;

private:
    std::array<side, squares> square_sides;
    std::array<piece, squares> square_pieces;

    std::array<bitboard, sides> side_sets;
    std::array<bitboard, pieces> piece_sets;

    std::size_t zobrist_hash;
};


}


#endif