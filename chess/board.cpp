#include <sstream>
#include <string>
#include <utility>
#include <unordered_map>

#include "piece.hpp"
#include "square.hpp"
#include "set.hpp"
#include "zobrist.hpp"
#include "board.hpp"
#include "attack.hpp"


namespace chess
{


board::board():
board
(
    {
        {square_a8, {side_black, piece_rook}},
        {square_b8, {side_black, piece_knight}},
        {square_c8, {side_black, piece_bishop}},
        {square_d8, {side_black, piece_queen}},
        {square_e8, {side_black, piece_king}},
        {square_f8, {side_black, piece_bishop}},
        {square_g8, {side_black, piece_knight}},
        {square_h8, {side_black, piece_rook}},

        {square_a7, {side_black, piece_pawn}},
        {square_b7, {side_black, piece_pawn}},
        {square_c7, {side_black, piece_pawn}},
        {square_d7, {side_black, piece_pawn}},
        {square_e7, {side_black, piece_pawn}},
        {square_f7, {side_black, piece_pawn}},
        {square_g7, {side_black, piece_pawn}},
        {square_h7, {side_black, piece_pawn}},

        {square_a2, {side_white, piece_pawn}},
        {square_b2, {side_white, piece_pawn}},
        {square_c2, {side_white, piece_pawn}},
        {square_d2, {side_white, piece_pawn}},
        {square_e2, {side_white, piece_pawn}},
        {square_f2, {side_white, piece_pawn}},
        {square_g2, {side_white, piece_pawn}},
        {square_h2, {side_white, piece_pawn}},

        {square_a1, {side_white, piece_rook}},
        {square_b1, {side_white, piece_knight}},
        {square_c1, {side_white, piece_bishop}},
        {square_d1, {side_white, piece_queen}},
        {square_e1, {side_white, piece_king}},
        {square_f1, {side_white, piece_bishop}},
        {square_g1, {side_white, piece_knight}},
        {square_h1, {side_white, piece_rook}},
    }
)
{
}

board::board(const std::unordered_map<square, std::pair<side, piece>>& pieces):
square_sides{},
square_pieces{},
side_sets{},
piece_sets{},
zobrist_hash{0}
{
    square_sides.fill(side_none);
    square_pieces.fill(piece_none);
    side_sets.fill(empty_set);
    piece_sets.fill(empty_set);

    for(auto [sq, sp]: pieces)
    {
        auto [s, p] = sp;
        set(sq, s, p);
    }
}

std::pair<side, piece> board::get(square sq) const
{
    return {square_sides[sq], square_pieces[sq]};
}

void board::set(square sq, side s, piece p)
{
    side s_prev = square_sides[sq];
    piece p_prev = square_pieces[sq];

    square_sides[sq] = s;
    square_pieces[sq] = p;

    if(s_prev != side_none && p_prev != piece_none)
    {
        side_sets[s_prev] = set_erase(side_sets[s_prev], sq);
        piece_sets[p_prev] = set_erase(piece_sets[p_prev], sq);
        zobrist_hash ^= zobrist_piece_key(sq, s_prev, p_prev);
    }

    if(s != side_none && p != piece_none)
    {
        side_sets[s] = set_insert(side_sets[s], sq);
        piece_sets[p] = set_insert(piece_sets[p], sq);
        zobrist_hash ^= zobrist_piece_key(sq, s, p);
    }
}

void board::clear()
{
    for(int s = square_a1; s <= square_h8; s++)
    {
        square_sides[s] = side_none;
        square_pieces[s] = piece_none;
    }

    for(int p = piece_pawn; p <= piece_king; p++)
    {
        piece_sets[p] = 0;
    }
    
    side_sets[side_white] = 0;
    side_sets[side_black] = 0;

    zobrist_hash = 0;
}

bitboard board::side_set(side s) const
{
    return side_sets[s];
}

bitboard board::piece_set(piece p, side s) const
{
    return piece_sets[p] & (s != side_none ? side_sets[s] : universal_set);
}

bitboard board::occupied_set() const
{
    return side_sets[side_white] | side_sets[side_black];
}

bitboard board::attack_set(side s) const
{
    bitboard pawns = piece_set(piece_pawn, s);
    bitboard rooks = piece_set(piece_rook, s);
    bitboard knights = piece_set(piece_knight, s);
    bitboard bishops = piece_set(piece_bishop, s);
    bitboard queens = piece_set(piece_queen, s);
    bitboard kings = piece_set(piece_king, s);

    bitboard attacks = 0;

    attacks |= set_shift(pawns, static_cast<direction>(forwards(s) + direction_e));
    attacks |= set_shift(pawns, static_cast<direction>(forwards(s) + direction_w));

    while(rooks)
    {
        square from = set_first(rooks);
        rooks = set_erase(rooks, from);
        attacks |= rook_attack_set(from, occupied_set());
    }

    while(knights)
    {
        square from = set_first(knights);
        knights = set_erase(knights, from);
        attacks |= knight_attack_set(from);
    }

    while(bishops)
    {
        square from = set_first(bishops);
        bishops = set_erase(bishops, from);
        attacks |= bishop_attack_set(from, occupied_set());
    }

    while(queens)
    {
        square from = set_first(queens);
        queens = set_erase(queens, from);
        attacks |= (rook_attack_set(from, occupied_set()) | bishop_attack_set(from, occupied_set()));
    }

    while(kings)
    {
        square from = set_first(kings);
        kings = set_erase(kings, from);
        attacks |= king_attack_set(from);
    }

    return attacks;
}

std::size_t board::hash() const
{
    return zobrist_hash;
}

std::string board::to_string(bool coords) const
{
    std::ostringstream stream;

    for(int i = rank_8; i >= rank_1; i--)
    {
        rank r = static_cast<rank>(i);

        if(coords)
        {
            stream << rank_to_san(r);
        }

        for(int j = file_a; j <= file_h; j++)
        {
            file f = static_cast<file>(j);

            square sq = cat_coords(f, r);
            auto [s, p] = get(sq);

            if(p == piece_none)
            {
                stream << '.';
            }
            else
            {
                stream << piece_to_san(s, p);
            }
        }

        stream << '\n';
    }

    if(coords)
    {
        stream << ' ';
        for(int j = file_a; j <= file_h; j++)
        {
            file f = static_cast<file>(j);
            stream << file_to_san(f);
        }
    }

    return stream.str();
}


}