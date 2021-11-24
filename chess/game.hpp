#ifndef CHESS_GAME_HPP
#define CHESS_GAME_HPP


#include <vector>
#include <unordered_map>
#include <optional>
#include <cstdint>

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
    game(const position& p, const std::vector<move>& moves);

    void push(const chess::move& move);
    void pop();
    const position& top() const;
    const std::size_t size() const;
    const bool empty() const;

    const position& get_position() const;
    const std::vector<std::pair<move, undo>>& get_history() const;
    int get_repetitions(const std::optional<position>& position = std::nullopt);

    bool is_check() const;
    bool is_checkmate() const;
    bool is_stalemate() const;

    bool is_threefold_repetition() const;
    bool is_fivefold_repetition() const;

    bool is_fiftymove_rule() const;
    bool is_seventyfivemove_rule() const;

    bool is_insufficient_material() const;

    bool is_terminal() const;
    std::optional<float> get_score(side s = side_white) const;
    std::optional<int> get_value(side s = side_white) const;

private:
    position p;
    std::vector<std::pair<move, undo>> history;
    std::unordered_map<std::size_t, int> repetitions;
};


}


#endif