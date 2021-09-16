#ifndef CHESS_BOARD_HPP
#define CHESS_BOARD_HPP


#include <iostream>
#include <cstdint>
#include <cstddef>
#include <utility>
#include <unordered_map>
#include <array>

#include "types.hpp"
#include "random.hpp"
#include "move.hpp"
#include "bitboard.hpp"


namespace chess
{


using bitboard = std::uint64_t;
struct move;



static std::size_t board_hash_keys[squares][sides][pieces];


void board_init(random* r)
{
    for(int i = square_a1; i <= square_h8; i++)
    {
        for(int j = piece_pawn; j <= piece_king; j++)
        {
            square sq = static_cast<square>(i);
            piece p = static_cast<piece>(j);

            board_hash_keys[sq][side_white][p] = random_generate(r);
            board_hash_keys[sq][side_black][p] = random_generate(r);
        }
    }
}


class board
{
public:
    board():
    square_sides{},
    square_pieces{},
    side_masks{},
    piece_masks{},
    hash{0}
    {
        square_sides.fill(side_none);
        square_pieces.fill(piece_none);
        side_masks.fill(empty_mask);
        piece_masks.fill(empty_mask);
    }

    board(const std::unordered_map<square, std::pair<side, piece>>& pieces):
    board()
    {
        for(auto [sq, sp]: pieces)
        {
            auto [s, p] = sp;
            set(sq, s, p);
        }
    }

    void set(square sq, side s, piece p)
    {
        side s_prev = square_sides[sq];
        piece p_prev = square_pieces[sq];

        square_sides[sq] = s;
        square_pieces[sq] = p;

        if(s_prev != side_none && p_prev != piece_none)
        {
            side_masks[s_prev] = bitboard_reset(side_masks[s_prev], sq);
            piece_masks[p_prev] = bitboard_reset(piece_masks[p_prev], sq);
            hash ^= board_hash_keys[sq][s_prev][p_prev];
        }

        if(s != side_none && p != piece_none)
        {
            side_masks[s] = bitboard_set(side_masks[s], sq);
            piece_masks[p] = bitboard_set(piece_masks[p], sq);
            hash ^= board_hash_keys[sq][s][p];
        }
    }

    std::array<side, squares> square_sides;
    std::array<piece, squares> square_pieces;

    std::array<bitboard, sides> side_masks;
    std::array<bitboard, pieces> piece_masks;

    std::size_t hash{0};
};


void board_clear(board* b)
{
    for(int s = square_a1; s <= square_h8; s++)
    {
        b->square_sides[s] = side_none;
        b->square_pieces[s] = piece_none;
    }

    for(int p = piece_pawn; p <= piece_king; p++)
    {
        b->piece_masks[p] = 0;
    }
    
    b->side_masks[side_white] = 0;
    b->side_masks[side_black] = 0;

    b->hash = 0;
}

piece board_get(const board* b, square sq, side* s)
{
    if(s != NULL) *s = b->square_sides[sq];
    return b->square_pieces[sq];
}

void board_set(board* b, square sq, side s, piece p)
{
    side s_prev = b->square_sides[sq];
    piece p_prev = b->square_pieces[sq];

    b->square_sides[sq] = s;
    b->square_pieces[sq] = p;

    if(s_prev != side_none && p_prev != piece_none)
    {
        b->side_masks[s_prev] = bitboard_reset(b->side_masks[s_prev], sq);
        b->piece_masks[p_prev] = bitboard_reset(b->piece_masks[p_prev], sq);
        b->hash ^= board_hash_keys[sq][s_prev][p_prev];
    }

    if(s != side_none && p != piece_none)
    {
        b->side_masks[s] = bitboard_set(b->side_masks[s], sq);
        b->piece_masks[p] = bitboard_set(b->piece_masks[p], sq);
        b->hash ^= board_hash_keys[sq][s][p];
    }
}

bitboard board_empty(const board* b)
{
    return ~(b->side_masks[side_white] | b->side_masks[side_black]);
}

bitboard board_occupied(const board* b)
{
    return b->side_masks[side_white] | b->side_masks[side_black];
}

bitboard board_side(const board* b, side s)
{
    return b->side_masks[s];
}

bitboard board_piece(const board* b, piece p)
{
    return b->piece_masks[p];
}

bitboard board_side_piece(const board* b, side s, piece p)
{
    return b->side_masks[s] & b->piece_masks[p];
}

bitboard board_attacks(const board* b, side s)
{
    bitboard pawns = board_side_piece(b, s, piece_pawn);
    bitboard rooks = board_side_piece(b, s, piece_rook);
    bitboard knights = board_side_piece(b, s, piece_knight);
    bitboard bishops = board_side_piece(b, s, piece_bishop);
    bitboard queens = board_side_piece(b, s, piece_queen);
    bitboard kings = board_side_piece(b, s, piece_king);

    bitboard attacks = 0;

    attacks |= bitboard_shift(pawns, static_cast<direction>(direction_forward(s) + direction_e));
    attacks |= bitboard_shift(pawns, static_cast<direction>(direction_forward(s) + direction_w));

    while(rooks)
    {
        square from = bitboard_ls1b(rooks);
        rooks = bitboard_reset(rooks, from);
        attacks |= bitboard_rook_attacks(from, board_occupied(b));
    }

    while(knights)
    {
        square from = bitboard_ls1b(knights);
        knights = bitboard_reset(knights, from);
        attacks |= bitboard_knight_attacks(from);
    }

    while(bishops)
    {
        square from = bitboard_ls1b(bishops);
        bishops = bitboard_reset(bishops, from);
        attacks |= bitboard_bishop_attacks(from, board_occupied(b));
    }

    while(queens)
    {
        square from = bitboard_ls1b(queens);
        queens = bitboard_reset(queens, from);
        attacks |= (bitboard_rook_attacks(from, board_occupied(b)) | bitboard_bishop_attacks(from, board_occupied(b)));
    }

    while(kings)
    {
        square from = bitboard_ls1b(kings);
        kings = bitboard_reset(kings, from);
        attacks |= bitboard_king_attacks(from);
    }

    return attacks;
}


std::size_t board_hash(const board* b)
{
    return b->hash;
}


}


#endif
