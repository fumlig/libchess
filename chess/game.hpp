#ifndef CHESS_GAME_HPP
#define CHESS_GAME_HPP


#include <vector>
#include <stack>
#include <unordered_map>
#include <optional>


#include "board.hpp"
#include "position.hpp"


namespace chess
{


/// Chess game.
///
/// Includes position and move history.
class game
{
public:
    game();
    game(position&& p, const std::vector<move>& moves);

    void push_move(const chess::move& move);
    void pop_move();

    int get_repetitions(const std::optional<position>& position = std::nullopt);

    const position& get_position() const;

    bool is_check() const;
    bool is_checkmate() const;
    bool is_stalemate() const;

    bool is_threefold_repetition() const;
    bool is_fivefold_repetition() const;

    bool is_fiftymove_rule() const;
    bool is_seventyfivemove_rule() const;

    bool is_insufficient_material() const;

private:
    position p;
    std::stack<std::pair<move, undo>> history;
    std::unordered_map<std::size_t, int> repetitions;
};


}


#endif