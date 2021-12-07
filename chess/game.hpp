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
/// Includes position and move history. Caches legal moves.
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

    const std::vector<move>& get_moves() const;
    bool is_terminal() const;

    const position& get_position() const;
    const std::vector<std::pair<move, undo>>& get_history() const;
    int get_repetitions(const std::optional<position>& position = std::nullopt);

    std::optional<float> get_score(side s = side_white) const;
    std::optional<int> get_value(side s = side_white) const;

    std::string to_string() const;

private:
    position p;
    std::vector<std::pair<move, undo>> history;
    std::unordered_map<std::size_t, int> repetitions;
    mutable std::optional<std::vector<move>> moves;
    mutable std::optional<bool> terminal;
};


}


#endif