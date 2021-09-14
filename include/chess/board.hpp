#ifndef CHESS_BOARD_HPP
#define CHESS_BOARD_HPP


#include <iostream>
#include <cstdint>
#include <cstddef>

#include "piece.hpp"
#include "random.hpp"
#include "square.hpp"
#include "move.hpp"
#include "bitboard.hpp"


namespace chess
{


using bitboard = std::uint64_t;
struct move;



void board_init(random* r);


class board
{
public:
    side square_sides[squares];
    piece square_pieces[squares];

    bitboard side_masks[sides];
    bitboard piece_masks[pieces];

    std::size_t hash;
};


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
        square from = bitboard_lsb(rooks);
        rooks = bitboard_reset(rooks, from);
        attacks |= bitboard_rook_attacks(from, board_occupied(b));
    }

    while(knights)
    {
        square from = bitboard_lsb(knights);
        knights = bitboard_reset(knights, from);
        attacks |= bitboard_knight_attacks(from);
    }

    while(bishops)
    {
        square from = bitboard_lsb(bishops);
        bishops = bitboard_reset(bishops, from);
        attacks |= bitboard_bishop_attacks(from, board_occupied(b));
    }

    while(queens)
    {
        square from = bitboard_lsb(queens);
        queens = bitboard_reset(queens, from);
        attacks |= (bitboard_rook_attacks(from, board_occupied(b)) | bitboard_bishop_attacks(from, board_occupied(b)));
    }

    while(kings)
    {
        square from = bitboard_lsb(kings);
        kings = bitboard_reset(kings, from);
        attacks |= bitboard_king_attacks(from);
    }

    return attacks;
}


std::size_t board_hash(const board* b)
{
    return b->hash;
}

void board_copy(const board* src, board* dst)
{
    for(int i = square_a1; i <= square_h8; i++)
    {
        const square sq = static_cast<square>(i);

        dst->square_sides[sq] = src->square_sides[sq];
        dst->square_pieces[sq] = src->square_pieces[sq];
    }

    dst->side_masks[side_white] = src->side_masks[side_white];
    dst->side_masks[side_black] = src->side_masks[side_black];

    dst->piece_masks[piece_pawn]   = src->piece_masks[piece_pawn];
    dst->piece_masks[piece_rook]   = src->piece_masks[piece_rook];
    dst->piece_masks[piece_knight] = src->piece_masks[piece_knight];
    dst->piece_masks[piece_bishop] = src->piece_masks[piece_bishop];
    dst->piece_masks[piece_queen]  = src->piece_masks[piece_queen];
    dst->piece_masks[piece_king]   = src->piece_masks[piece_king];

    dst->hash = src->hash;
}


}


#endif
