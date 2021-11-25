#ifndef CHESS_POSITION_HPP
#define CHESS_POSITION_HPP


#include <string>
#include <sstream>
#include <optional>

#include "side.hpp"
#include "square.hpp"
#include "board.hpp"
#include "move.hpp"
#include "set.hpp"


namespace chess
{


/// Chess position.
///
/// Contains information about a chess position including piece placement,
/// whose turn it is, castling avalailability, en passant target square...
class position
{
public:
    /// Start position.
    ///
    /// Constructs initial position.
    position();

    /// Construct position.
    ///
    /// Construct a position froo the given information. It is assumed that the
    /// position is valid.
    ///
    /// \param n The board (pieces).
    /// \param turn The side whose turn it is.
    /// \param white_kingside_castle White kingside castling availability.
    /// \param white_queenside_castle White queenside castling availability.
    /// \param black_kingside_castle Black kingside castling availability.
    /// \param black_queenside_castle Black queenside castling availability.
    /// \param en_passant En passant target square (if last move was a double pawn push).
    /// \param halfmove_clock Number of silent halfmoves (for 50 move rule).
    /// \param fullmove_number Number of fullmoves made.
    position
    (
        const board& b, 
        side turn = side_white,
        bool white_kingside_castle = true,
        bool white_queenside_castle = true,
        bool black_kingside_castle = true,
        bool black_queenside_castle = true,
        square en_passant = square_none,
        int halfmove_clock = 0,
        int fullmove_number = 1
    );

    /// Fen for starting position.
	static const inline std::string_view fen_start = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	
    /// Fen for empty position.
    static const inline std::string_view fen_empty = "8/8/8/8/8/8/8/8 w - - 0 1";

    /// Create position from Forsyth-Edwards Notation (FEN).
    ///
    /// Parse FEN string and returns the position it encodes.
    ///
    /// \param fen FEN string. Defaults to starting position.
    /// \returns Position encoded by FEN.
    /// \throws Invalid argument if FEN seems to be invalid.
    /// \note To get the initial position, the default position constructor can be used.
	static position from_fen(std::string_view fen);

    /// Convert position to Forsyth-Edwards Notation (FEN).
    ///
    /// Serialize position to FEN string.
    ///
    /// \returns FEN encoding position.
	std::string to_fen() const;

    /// Make move.
    ///
    /// Make move on given position by updating internal state.
    ///
    /// \param m The move.
    /// \returns Undo data.
    undo make_move(const move& m);

    /// Undo move.
    ///
    /// Undo move on given position by updating internal state. The undo data
    /// used should be the one returned when making the move.
    ///
    /// \param m The move.
    /// \param u Undo data.
    void undo_move(const move& m, const undo& u);

    /// Copy move.
    ///
    /// Make move on given position by copying state and updating the copy.
    /// This is a little more expensive than making a move by updating internal
    /// state, but when a lot of move undos are involved, copying seems to
    /// perform similarly (as undoing is not necessary).
    ///
    /// \param m The move.
    /// \returns Position with the move made.
    position copy_move(const move& m) const;

    /// Legal moves.
    ///
    /// Returns list of legal moves in position.
    ///
    /// \returns List of legal moves.
    std::vector<move> moves() const;

    /// Position board.
    ///
    /// Returns the piece placement of the position.
    ///
    /// \returns The board.
    const board& get_board() const;

    /// Fullmove number.
    ///
    /// Returns the number of fullmoves since initial position.
    ///
    /// \returns The fullmove number.
    int get_fullmove() const;

    /// Halfmove number.
    ///
    /// Returns the number of halfmove (often referred to as ply)
    /// since initial position.
    ///
    /// \returns The halfmove number.
    int get_halfmove() const;

    /// Get current turn
    ///
    /// Returns the side of the player whose turn it is
    ///
    /// \returns Current turn.
    side get_turn() const;

    /// Get halfmove clock.
    ///
    /// Returns the number of halfmoves since last pawn push or capture.
    ///
    /// \returns Halfmove clock.
    int get_halfmove_clock() const;

    // Should only be used on positions managed by game class.
    int get_repetititons() const;

    bool can_castle_kingside(side s) const;
    bool can_castle_queenside(side s) const;

    /// Position hash.
    ///
    /// Returns the Zobrist hash of the position.
    ///
    /// \returns The hash.
    std::size_t hash() const;

    /// Board to string.
    ///
    /// Returns (pretty) string representation of board, one rank per row.
    /// Starts at top left corner of board (square A8)
    ///
    /// \param coords Show coordinates.
    /// \returns Board string.
    std::string to_string(bool coords = true) const;

    /// Check flag.
    ///
    /// Returns whether the side whose turn it is is checked.
    ///
    /// \returns Check flag.
    bool is_check() const;

    /// Checkmate flag.
    ///
    /// Returns whether the side whose turn it is is checkmated.
    ///
    /// \returns Checkmate flag.
    bool is_checkmate() const;

    /// Stalemate flag.
    ///
    /// Returns whether the side whose turn it is is stalemated.
    ///
    /// \returns Stalemate flag.
    bool is_stalemate() const;

    bool is_threefold_repetition() const;
    bool is_fivefold_repetition() const;

    bool is_fiftymove_rule() const;
    bool is_seventyfivemove_rule() const;

    bool is_insufficient_material() const;

    bool is_draw() const;
    bool is_terminal() const;

private:
    void piecewise_moves(square from, bitboard tos, piece promote, std::vector<move>& moves) const;
    void setwise_moves(bitboard froms, bitboard tos, piece promote, std::vector<move>& moves) const;

    board b;
    side turn;
    std::array<bool, sides> kingside_castle;
    std::array<bool, sides> queenside_castle;
    square en_passant;
    int halfmove_clock;
    int fullmove_number;
    std::size_t zobrist_hash;
    int repetitions;

    friend class game;
};


}


#endif