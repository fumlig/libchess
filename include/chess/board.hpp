#ifndef CHESS_BOARD_HPP
#define CHESS_BOARD_HPP


#include <iostream>
#include <cstdint>
#include <cstddef>
#include <utility>
#include <unordered_map>
#include <array>

#include "common.hpp"


namespace chess
{


class board
{
public:
    board():
    square_sides{},
    square_pieces{},
    side_masks{},
    piece_masks{},
    zobrist_hash{0}
    {
        square_sides.fill(side_none);
        square_pieces.fill(piece_none);
        side_masks.fill(bitboard_empty);
        piece_masks.fill(bitboard_empty);
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

    std::pair<side, piece> get(square sq) const
    {
        return {square_sides[sq], square_pieces[sq]};
    }

    void set(square sq, side s, piece p)
    {
        side s_prev = square_sides[sq];
        piece p_prev = square_pieces[sq];

        square_sides[sq] = s;
        square_pieces[sq] = p;

        if(s_prev != side_none && p_prev != piece_none)
        {
            side_masks[s_prev] = unset_square(side_masks[s_prev], sq);
            piece_masks[p_prev] = unset_square(piece_masks[p_prev], sq);
            zobrist_hash ^= pieces_zobrist_hash[sq][s_prev][p_prev];
        }

        if(s != side_none && p != piece_none)
        {
            side_masks[s] = set_square(side_masks[s], sq);
            piece_masks[p] = set_square(piece_masks[p], sq);
            zobrist_hash ^= pieces_zobrist_hash[sq][s][p];
        }
    }

    void clear()
    {
        for(int s = square_a1; s <= square_h8; s++)
        {
            square_sides[s] = side_none;
            square_pieces[s] = piece_none;
        }

        for(int p = piece_pawn; p <= piece_king; p++)
        {
            piece_masks[p] = 0;
        }
        
        side_masks[side_white] = 0;
        side_masks[side_black] = 0;

        zobrist_hash = 0;
    }


    inline bitboard side_mask(side s) const
    {
        return side_masks[s];
    }

    inline bitboard piece_mask(piece p) const
    {
        return piece_masks[p];
    }

    inline bitboard side_piece_mask(side s, piece p) const
    {
        return side_masks[s] & piece_masks[p];
    }

    inline bitboard empty_mask() const
    {
        return ~(side_masks[side_white] | side_masks[side_black]);
    }

    inline bitboard occupied_mask() const
    {
        return side_masks[side_white] | side_masks[side_black];
    }


    bitboard attack_mask(side s) const
    {
        bitboard pawns = side_piece_mask(s, piece_pawn);
        bitboard rooks = side_piece_mask(s, piece_rook);
        bitboard knights = side_piece_mask(s, piece_knight);
        bitboard bishops = side_piece_mask(s, piece_bishop);
        bitboard queens = side_piece_mask(s, piece_queen);
        bitboard kings = side_piece_mask(s, piece_king);

        bitboard attacks = 0;

        attacks |= bitboard_shift(pawns, static_cast<direction>(forwards(s) + direction_e));
        attacks |= bitboard_shift(pawns, static_cast<direction>(forwards(s) + direction_w));

        while(rooks)
        {
            square from = first_set_square(rooks);
            rooks = unset_square(rooks, from);
            attacks |= rook_attack_mask(from, occupied_mask());
        }

        while(knights)
        {
            square from = first_set_square(knights);
            knights = unset_square(knights, from);
            attacks |= knight_attack_mask(from);
        }

        while(bishops)
        {
            square from = first_set_square(bishops);
            bishops = unset_square(bishops, from);
            attacks |= bishop_attack_mask(from, occupied_mask());
        }

        while(queens)
        {
            square from = first_set_square(queens);
            queens = unset_square(queens, from);
            attacks |= (rook_attack_mask(from, occupied_mask()) | bishop_attack_mask(from, occupied_mask()));
        }

        while(kings)
        {
            square from = first_set_square(kings);
            kings = unset_square(kings, from);
            attacks |= king_attack_mask(from);
        }

        return attacks;
    }

    inline std::size_t hash() const
    {
        return zobrist_hash;
    }

private:
    std::array<side, squares> square_sides;
    std::array<piece, squares> square_pieces;

    std::array<bitboard, sides> side_masks;
    std::array<bitboard, pieces> piece_masks;

    std::size_t zobrist_hash;
};


}


#endif
