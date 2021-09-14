#ifndef CHESS_BOARD_HPP
#define CHESS_BOARD_HPP


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <uchar.h>

#include "random.hpp"
#include "square.hpp"
#include "piece.hpp"
#include "move.hpp"
#include "bitboard.hpp"


namespace chess
{


typedef uint64_t bitboard;
struct move;



void board_init(random* r);


struct board
{
    side sides[SQUARES];
    piece pieces[SQUARES];

    bitboard side_bb[SIDES];
    bitboard piece_bb[PIECES];

    uint64_t hash;
};


void board_clear(struct board* b);

piece board_get(const board* b, square sq, side* s);
void board_set(board* b, square sq, side s, piece p);


bitboard board_empty(const board* b);
bitboard board_occupied(const board* b);
bitboard board_side(const board* b, side s);
bitboard board_piece(const board* b, piece p);
bitboard board_side_piece(const board* b, side s, piece p);
bitboard board_attacks(const board* b, side s);


uint64_t board_hash(const board* b);
void board_copy(const board* src, board* dst);
void board_print(const board* b, bool figurine, bool coordinates);



static uint64_t board_hash_keys[SQUARES][SIDES][PIECES];


void board_init(random* r)
{
    for(int i = SQUARE_A1; i <= SQUARE_H8; i++)
    {
        for(int j = PIECE_PAWN; j <= PIECE_KING; j++)
        {
            square sq = static_cast<square>(i);
            piece p = static_cast<piece>(j);

            board_hash_keys[sq][SIDE_WHITE][p] = random_generate(r);
            board_hash_keys[sq][SIDE_BLACK][p] = random_generate(r);
        }
    }
}


void board_clear(board* b)
{
    for(int s = SQUARE_A1; s <= SQUARE_H8; s++)
    {
        b->sides[s] = SIDE_NONE;
        b->pieces[s] = PIECE_NONE;
    }

    for(int p = PIECE_PAWN; p <= PIECE_KING; p++)
    {
        b->piece_bb[p] = 0;
    }
    
    b->side_bb[SIDE_WHITE] = 0;
    b->side_bb[SIDE_BLACK] = 0;

    b->hash = 0;
}

piece board_get(const board* b, square sq, side* s)
{
    if(s != NULL) *s = b->sides[sq];
    return b->pieces[sq];
}

void board_set(board* b, square sq, side s, piece p)
{
    side s_prev = b->sides[sq];
    piece p_prev = b->pieces[sq];

    b->sides[sq] = s;
    b->pieces[sq] = p;

//    if(s_prev != SIDE_NONE) b->side_bb[s_prev] = bitboard_reset(b->side_bb[s_prev], sq);
//    if(p_prev != PIECE_NONE) b->piece_bb[p_prev] = bitboard_reset(b->piece_bb[p_prev], sq);

    if(s_prev != SIDE_NONE && p_prev != PIECE_NONE)
    {
        b->side_bb[s_prev] = bitboard_reset(b->side_bb[s_prev], sq);
        b->piece_bb[p_prev] = bitboard_reset(b->piece_bb[p_prev], sq);
        b->hash ^= board_hash_keys[sq][s_prev][p_prev];
    }

//    if(s != SIDE_NONE) b->side_bb[s] = bitboard_set(b->side_bb[s], sq);
//    if(p != PIECE_NONE) b->piece_bb[p] = bitboard_set(b->piece_bb[p], sq);

    if(s != SIDE_NONE && p != PIECE_NONE)
    {
        b->side_bb[s] = bitboard_set(b->side_bb[s], sq);
        b->piece_bb[p] = bitboard_set(b->piece_bb[p], sq);
        b->hash ^= board_hash_keys[sq][s][p];
    }
}

bitboard board_empty(const board* b)
{
    return ~(b->side_bb[SIDE_WHITE] | b->side_bb[SIDE_BLACK]);
}

bitboard board_occupied(const board* b)
{
    return b->side_bb[SIDE_WHITE] | b->side_bb[SIDE_BLACK];
}

bitboard board_side(const board* b, side s)
{
    return b->side_bb[s];
}

bitboard board_piece(const board* b, piece p)
{
    return b->piece_bb[p];
}

bitboard board_side_piece(const board* b, side s, piece p)
{
    return b->side_bb[s] & b->piece_bb[p];
}

bitboard board_attacks(const board* b, side s)
{
    bitboard pawns = board_side_piece(b, s, PIECE_PAWN);
    bitboard rooks = board_side_piece(b, s, PIECE_ROOK);
    bitboard knights = board_side_piece(b, s, PIECE_KNIGHT);
    bitboard bishops = board_side_piece(b, s, PIECE_BISHOP);
    bitboard queens = board_side_piece(b, s, PIECE_QUEEN);
    bitboard kings = board_side_piece(b, s, PIECE_KING);

    bitboard attacks = 0;

    attacks |= bitboard_shift(pawns, static_cast<direction>(direction_forward(s) + DIRECTION_E));
    attacks |= bitboard_shift(pawns, static_cast<direction>(direction_forward(s) + DIRECTION_W));

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


uint64_t board_hash(const board* b)
{
    return b->hash;
}

void board_copy(const board* src, board* dst)
{
    for(int i = SQUARE_A1; i <= SQUARE_H8; i++)
    {
        const square sq = static_cast<square>(i);

        dst->sides[sq] = src->sides[sq];
        dst->pieces[sq] = src->pieces[sq];
    }

    dst->side_bb[SIDE_WHITE] = src->side_bb[SIDE_WHITE];
    dst->side_bb[SIDE_BLACK] = src->side_bb[SIDE_BLACK];

    dst->piece_bb[PIECE_PAWN]   = src->piece_bb[PIECE_PAWN];
    dst->piece_bb[PIECE_ROOK]   = src->piece_bb[PIECE_ROOK];
    dst->piece_bb[PIECE_KNIGHT] = src->piece_bb[PIECE_KNIGHT];
    dst->piece_bb[PIECE_BISHOP] = src->piece_bb[PIECE_BISHOP];
    dst->piece_bb[PIECE_QUEEN]  = src->piece_bb[PIECE_QUEEN];
    dst->piece_bb[PIECE_KING]   = src->piece_bb[PIECE_KING];

    dst->hash = src->hash;
}

void board_print(const board* b, bool figurine, bool coordinates)
{
    for(int r = RANK_8; r >= RANK_1; r--)
    {
        for(int f = FILE_A; f <= FILE_H; f++)
        {
            square sq = square_from_file_rank(static_cast<file>(f), static_cast<rank>(r));
            side s;
            piece p = board_get(b, sq, &s);

            if(figurine) printf("%s", piece_to_fan(p, s));
            else printf("%c", piece_to_san(p, s));
        }
        if(coordinates)
        {
            putchar(' ');
            putchar(rank_to_char(static_cast<rank>(r)));
        }
        putchar('\n');
    }
    if(coordinates)
    {
        for(int f = FILE_A; f <= FILE_H; f++)
        {
            putchar(file_to_char(static_cast<file>(f)));
        }
        putchar('\n');
    }
}


}


#endif
