#include <vector>
#include <stack>
#include <unordered_map>
#include <optional>
#include <sstream>

#include "board.hpp"
#include "position.hpp"
#include "game.hpp"

namespace chess
{

game::game():
p(),
history(),
repetitions(),
moves{std::nullopt},
terminal{std::nullopt}
{
    repetitions[p.hash()]++;
}

game::game(const position& p, const std::vector<move>& moves):
p{p},
history(),
repetitions(),
moves{std::nullopt},
terminal{std::nullopt}
{
    repetitions[p.hash()]++;

    for(const move& move: moves)
    {
        push(move);
    }
}

void game::push(const chess::move& move)
{
    undo undo = p.make_move(move);
    history.emplace_back(move, undo);
    repetitions[p.hash()]++;
    p.repetitions = repetitions[p.hash()];
    moves = std::nullopt;
    terminal = std::nullopt;
}

void game::pop()
{
    repetitions[p.hash()]--;
    auto [move, undo] = history.back();
    p.undo_move(move, undo);
    history.pop_back();
    p.repetitions = repetitions[p.hash()];
    moves = std::nullopt;
    terminal = std::nullopt;
}

const position& game::top() const
{
    return p;
}

const std::size_t game::size() const
{
    return history.size();
}

const bool game::empty() const
{
    return history.empty();
}


const std::vector<move>& game::get_moves() const
{
    if(!moves)
    {
        moves = p.moves();
    }

    return *moves;
}

bool game::is_terminal() const
{
    if(!terminal)
    {
        terminal = p.is_terminal();
    }

    return *terminal;
}


int game::get_repetitions(const std::optional<position>& position)
{
    if(!position)
    {
        return repetitions[p.hash()];
    }
    else
    {
        return repetitions[(*position).hash()];
    }
}

const position& game::get_position() const
{
    return p;
}

const std::vector<std::pair<move, undo>>& game::get_history() const
{
    return history;
}

std::optional<float> game::get_score(side s) const
{
    if(is_terminal())
    {
        if(get_moves().empty() && p.is_check())
        {
            return p.get_turn() == opponent(s) ? 1.0f : 0.0f;
        }
        else
        {
            return 0.5f;
        }
    }

    return std::nullopt;
}

std::optional<int> game::get_value(side s) const
{
    if(is_terminal())
    {
        if(get_moves().empty() && p.is_check())
        {
            return p.get_turn() == opponent(s) ? 1 : -1;
        }
        else
        {
            return 0;
        }
    }

    return std::nullopt;
}


std::string game::to_string() const
{
    std::ostringstream out;
    out << p.to_string() << '\n' << "history: ";
    for(auto [m, _]: history)
    {
        out << m.to_lan() << ' ';
    }
    out << '\n';
    return out.str();
}


}
