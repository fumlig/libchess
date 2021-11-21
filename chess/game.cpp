#include <vector>
#include <stack>
#include <unordered_map>
#include <optional>

#include "board.hpp"
#include "position.hpp"
#include "game.hpp"

namespace chess
{

game::game():
p(),
history(),
repetitions()
{
    repetitions[p.hash()]++;
}

game::game(const position& p, const std::vector<move>& moves):
p{p},
history{},
repetitions{}
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
    history.emplace(move, undo);
    repetitions[p.hash()]++;
}

void game::pop()
{
    repetitions[p.hash()]--;
    auto [move, undo] = history.top();
    history.pop();
    p.undo_move(move, undo);
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

bool game::is_check() const
{
    return p.is_check();
}

bool game::is_checkmate() const
{
    return p.is_checkmate();
}

bool game::is_stalemate() const
{
    return p.is_stalemate();
}

bool game::is_threefold_repetition() const
{
    return repetitions.at(p.hash()) >= 3;
}

bool game::is_fivefold_repetition() const
{
    return repetitions.at(p.hash()) >= 5;
}

bool game::is_fiftymove_rule() const
{
    return p.get_halfmove_clock() >= 100;
}

bool game::is_seventyfivemove_rule() const
{
    return p.get_halfmove_clock() >= 150;
}

bool game::is_insufficient_material() const
{
    int pawns = set_cardinality(p.get_board().piece_set(piece_pawn));
    int rooks = set_cardinality(p.get_board().piece_set(piece_rook));
    int knights = set_cardinality(p.get_board().piece_set(piece_knight));
    int bishops = set_cardinality(p.get_board().piece_set(piece_bishop));
    int queens = set_cardinality(p.get_board().piece_set(piece_queen));

    if(pawns > 0 || rooks > 0 || queens > 0)
    {
        return false;
    }

    if((knights == 0 && bishops <= 1) || (knights <= 1 && bishops == 0))
    {
        return true;
    }

    if(bishops == 2)
    {
        bitboard white_bishop_set = p.get_board().piece_set(piece_bishop, side_white);
        bitboard black_bishop_set = p.get_board().piece_set(piece_bishop, side_black);

        if(!white_bishop_set || !white_bishop_set)
        {
            return false;
        }

        side white_bishop_color = color_of(set_first(white_bishop_set));
        side black_bishop_color = color_of(set_first(black_bishop_set));

        if(white_bishop_color == black_bishop_color)
        {
            return true;
        }
    }

    return false;
}

bool game::is_terminal() const
{
    return is_checkmate() || is_stalemate() || is_threefold_repetition() || is_fiftymove_rule() || is_insufficient_material();
}

std::optional<float> game::get_score(side s) const
{
    if(is_terminal())
    {
        if(is_checkmate())
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
        if(is_checkmate())
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


}
