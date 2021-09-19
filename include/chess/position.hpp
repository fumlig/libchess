#ifndef CHESS_POSITION_HPP
#define CHESS_POSITION_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <cstdint>
#include <array>
#include <vector>

#include "common.hpp"
#include "board.hpp"


namespace chess
{


class position
{
public:
    position():
    pieces(),
    turn{side_white},
    kingside_castle{true, true},
    queenside_castle{true, true},
    en_passant{square_none},
    halfmove_clock{0},
    fullmove_number{1},
    zobrist_hash{0}
    {}

    position
    (
        board&& pieces, 
        side turn = side_white,
        bool white_kingside_castle = true,
        bool white_queenside_castle = true,
        bool black_kingside_castle = true,
        bool black_queenside_castle = true,
        square en_passant = square_none,
        int halfmove_clock = 0,
        int fullmove_number = 1
    ):
    pieces(pieces),
    turn{turn},
    kingside_castle{white_kingside_castle, black_kingside_castle},
    queenside_castle{white_queenside_castle, black_queenside_castle},
    en_passant{en_passant},
    halfmove_clock{halfmove_clock},
    fullmove_number{fullmove_number},
    zobrist_hash{0}
    {
        if(turn == side_black)              zobrist_hash ^= side_zobrist_hash;
        if(kingside_castle[side_white])     zobrist_hash ^= kingside_castle_zobrist_hash[side_white];
        if(queenside_castle[side_white])    zobrist_hash ^= queenside_castle_zobrist_hash[side_white];
        if(kingside_castle[side_black])     zobrist_hash ^= kingside_castle_zobrist_hash[side_black];
        if(queenside_castle[side_black])    zobrist_hash ^= queenside_castle_zobrist_hash[side_black];
        if(en_passant != square_none)       zobrist_hash ^= en_passant_zobrist_hash[file_of(en_passant)];
    }


    undo make_move(const move& m)
    {
        undo u{pieces.get(m.to).second, en_passant, kingside_castle, queenside_castle};
        auto [side, piece] = pieces.get(m.from);
        square ep = en_passant;

        pieces.set(m.from, side_none, piece_none);

        if(m.promote != piece_none) pieces.set(m.to, side, m.promote);
        else pieces.set(m.to, side, piece);

        en_passant = square_none;
        if(ep != square_none) zobrist_hash ^= en_passant_zobrist_hash[file_of(ep)];

        if(piece == piece_pawn)
        {
            if(rank_of(m.from) == side_rank(side, rank_2) && rank_of(m.to) == side_rank(side, rank_4))
            {
                // double push
                en_passant = cat_coords(file_of(m.from), side_rank(side, rank_3));
                zobrist_hash ^= en_passant_zobrist_hash[file_of(en_passant)];
            }
            else if(m.to == ep)
            {
                // en passant capture
                square ep_capture = cat_coords(file_of(ep), side_rank(side, rank_5));
                pieces.set(ep_capture, side_none, piece_none);
            }
        }
        else if(piece == piece_king)
        {
            if(kingside_castle[side])
            {
                kingside_castle[side] = false;
                zobrist_hash ^= kingside_castle_zobrist_hash[side];
            }
            if(queenside_castle[side])
            {
                queenside_castle[side] = false;
                zobrist_hash ^= queenside_castle_zobrist_hash[side];
            }
            
            rank rank_first = side_rank(side, rank_1);

            if(m.from == cat_coords(file_e, rank_first))
            {
                if(m.to == cat_coords(file_g, rank_first))
                {
                    pieces.set(cat_coords(file_h, rank_first), side_none, piece_none);
                    pieces.set(cat_coords(file_f, rank_first), side, piece_rook);
                }
                else if(m.to == cat_coords(file_c, rank_first))
                {
                    pieces.set(cat_coords(file_a, rank_first), side_none, piece_none);
                    pieces.set(cat_coords(file_d, rank_first), side, piece_rook);
                }
            }
        }

        // todo: these (and en passant) should only be updated if they changed from the previous move
        // can probably be done by xoring with current state before each move

        if(queenside_castle[side_white] && (m.from == square_a1 || m.to == square_a1))
        {
            queenside_castle[side_white] = false;
            zobrist_hash ^= queenside_castle_zobrist_hash[side_white];
        }
        if(kingside_castle[side_white] && (m.from == square_h1 || m.to == square_h1))
        {
            kingside_castle[side_white] = false;
            zobrist_hash ^= kingside_castle_zobrist_hash[side_white];
        }
        if(queenside_castle[side_black] && (m.from == square_a8 || m.to == square_a8))
        {
            queenside_castle[side_black] = false;
            zobrist_hash ^= queenside_castle_zobrist_hash[side_black];
        }
        if(kingside_castle[side_black] && (m.from == square_h8 || m.to == square_h8))
        {
            kingside_castle[side_black] = false;
            zobrist_hash ^= kingside_castle_zobrist_hash[side_black];
        }

        halfmove_clock++; // todo: only if silent
        fullmove_number += turn;
        turn = opponent(turn);
        zobrist_hash ^= side_zobrist_hash;

        return u;
    }


    void undo_move(const move& m, const undo& u)
    {
        auto [side, piece] = pieces.get(m.to);

        pieces.set(m.from, side, piece);
        pieces.set(m.to, side_none, piece_none);
        if(u.capture != piece_none) pieces.set(m.to, opponent(side), u.capture);
        if(m.promote != piece_none) pieces.set(m.from, side, piece_pawn);

        if(en_passant != square_none) zobrist_hash ^= en_passant_zobrist_hash[file_of(en_passant)];
        if(u.en_passant != square_none) zobrist_hash ^= en_passant_zobrist_hash[file_of(u.en_passant)];
        en_passant = u.en_passant;

        if(kingside_castle[side_white] != u.kingside_castle[side_white])
        {
            kingside_castle[side_white] = u.kingside_castle[side_white];
            zobrist_hash ^= kingside_castle_zobrist_hash[side_white];
        }
        if(kingside_castle[side_black] != u.kingside_castle[side_black])
        {
            kingside_castle[side_black] = u.kingside_castle[side_black];
            zobrist_hash ^= kingside_castle_zobrist_hash[side_black];
        }
        if(queenside_castle[side_white] != u.queenside_castle[side_white])
        {
            queenside_castle[side_white] = u.queenside_castle[side_white];
            zobrist_hash ^= queenside_castle_zobrist_hash[side_white];
        }
        if(queenside_castle[side_black] != u.queenside_castle[side_black])
        {
            queenside_castle[side_black] = u.queenside_castle[side_black];
            zobrist_hash ^= queenside_castle_zobrist_hash[side_black];
        }

        if(piece == piece_pawn)
        {
            if(m.to == u.en_passant)
            {
                square ep_capture = cat_coords(file_of(u.en_passant), side_rank(side, rank_5));
                pieces.set(ep_capture, opponent(side), piece_pawn);
            }
        }
        else if(piece == piece_king)
        {
            rank rank_first = side_rank(side, rank_1);

            if(m.from == cat_coords(file_e, rank_first))
            {
                if(m.to == cat_coords(file_g, rank_first))
                {
                    pieces.set(cat_coords(file_h, rank_first), side, piece_rook);
                    pieces.set(cat_coords(file_f, rank_first), side_none, piece_none);
                }
                else if(m.to == cat_coords(file_c, side_rank(side, rank_1)))
                {
                    pieces.set(cat_coords(file_a, rank_first), side, piece_rook);
                    pieces.set(cat_coords(file_d, rank_first), side_none, piece_none);
                }
            }
        }

        halfmove_clock--; // todo: only if silent move
        fullmove_number -= opponent(turn); // decrease if white
        turn = opponent(turn);
        zobrist_hash ^= side_zobrist_hash;
    }

    std::vector<move> moves()
    {
        std::vector<move> moves;

        bitboard occupied = pieces.occupied_mask();
        
        bitboard pawns = pieces.side_piece_mask(turn, piece_pawn);
        bitboard rooks = pieces.side_piece_mask(turn, piece_rook);
        bitboard knights = pieces.side_piece_mask(turn, piece_knight);
        bitboard bishops = pieces.side_piece_mask(turn, piece_bishop);
        bitboard queens = pieces.side_piece_mask(turn, piece_queen);
        bitboard kings = pieces.side_piece_mask(turn, piece_king);
        
        bitboard attack_mask = ~pieces.side_mask(turn);
        bitboard capture_mask = pieces.side_mask(opponent(turn));
        bitboard ep_mask = 0;
        if(en_passant != square_none) ep_mask = square_mask(en_passant);

        // pawn moves
        bitboard single_push_tos = bitboard_shift(pawns, forwards(turn)) & ~occupied;
        bitboard single_push_froms = bitboard_shift(single_push_tos, forwards(opponent(turn)));
        bitboard double_push_tos = bitboard_shift(single_push_tos & rank_mask(side_rank(turn, rank_3)), forwards(turn)) & ~occupied;
        bitboard double_push_froms = bitboard_shift(bitboard_shift(double_push_tos, forwards(opponent(turn))), forwards(opponent(turn)));

        bitboard attack_east_tos = bitboard_shift(pawns, static_cast<direction>(forwards(turn) + direction_e)) & (capture_mask | ep_mask);
        bitboard attack_east_froms = bitboard_shift(attack_east_tos, static_cast<direction>(forwards(opponent(turn)) + direction_w));
        bitboard attack_west_tos = bitboard_shift(pawns, static_cast<direction>(forwards(turn) + direction_w)) & (capture_mask | ep_mask);
        bitboard attack_west_froms = bitboard_shift(attack_west_tos, static_cast<direction>(forwards(opponent(turn)) + direction_e));
        
        bitboard promote_push_tos = single_push_tos & rank_mask(side_rank(turn, rank_8));
        bitboard promote_push_froms = single_push_froms & rank_mask(side_rank(turn, rank_7));
        bitboard promote_east_tos = attack_east_tos & rank_mask(side_rank(turn, rank_8));
        bitboard promote_east_froms = attack_east_froms & rank_mask(side_rank(turn, rank_7));
        bitboard promote_west_tos = attack_west_tos & rank_mask(side_rank(turn, rank_8));
        bitboard promote_west_froms = attack_west_froms & rank_mask(side_rank(turn, rank_7));;

        single_push_tos ^= promote_push_tos;
        single_push_froms ^= promote_push_froms;
        attack_east_tos ^= promote_east_tos;
        attack_east_froms ^= promote_east_froms;
        attack_west_tos ^= promote_west_tos;
        attack_west_froms ^= promote_west_froms;

        setwise_moves(single_push_froms, single_push_tos, piece_none, moves);
        setwise_moves(double_push_froms, double_push_tos, piece_none, moves);
        
        setwise_moves(attack_east_froms, attack_east_tos, piece_none, moves);
        setwise_moves(attack_west_froms, attack_west_tos, piece_none, moves);
        
        setwise_moves(promote_push_froms, promote_push_tos, piece_rook, moves);
        setwise_moves(promote_push_froms, promote_push_tos, piece_knight, moves);
        setwise_moves(promote_push_froms, promote_push_tos, piece_bishop, moves);
        setwise_moves(promote_push_froms, promote_push_tos, piece_queen, moves);

        setwise_moves(promote_east_froms, promote_east_tos, piece_rook, moves);
        setwise_moves(promote_east_froms, promote_east_tos, piece_knight, moves);
        setwise_moves(promote_east_froms, promote_east_tos, piece_bishop, moves);
        setwise_moves(promote_east_froms, promote_east_tos, piece_queen, moves);

        setwise_moves(promote_west_froms, promote_west_tos, piece_rook, moves);
        setwise_moves(promote_west_froms, promote_west_tos, piece_knight, moves);
        setwise_moves(promote_west_froms, promote_west_tos, piece_bishop, moves);
        setwise_moves(promote_west_froms, promote_west_tos, piece_queen, moves);

        // rook moves
        while(rooks)
        {
            square from = first_set_square(rooks);
            rooks = unset_square(rooks, from);
            bitboard attacks = rook_attack_mask(from, occupied) & attack_mask;
            piecewise_moves(from, attacks, piece_none, moves);
        }

        // knight moves
        while(knights)
        {
            square from = first_set_square(knights);
            knights = unset_square(knights, from);
            bitboard attacks = knight_attack_mask(from) & attack_mask;
            piecewise_moves(from, attacks, piece_none, moves);
        }

        // bishop moves
        while(bishops)
        {
            square from = first_set_square(bishops);
            bishops = unset_square(bishops, from);
            bitboard attacks = bishop_attack_mask(from, occupied) & attack_mask;
            piecewise_moves(from, attacks, piece_none, moves);
        }

        // queen moves
        while(queens)
        {
            square from = first_set_square(queens);
            queens = unset_square(queens, from);
            bitboard attacks = (rook_attack_mask(from, occupied) | bishop_attack_mask(from, occupied)) & attack_mask;
            piecewise_moves(from, attacks, piece_none, moves);
        }

        // king moves
        if(kingside_castle[turn])
        {
            square from = first_set_square(kings);
            square to = cat_coords(file_g, rank_of(from));
            bitboard path = kings;
            path |= bitboard_shift(path, direction_e);
            path |= bitboard_shift(path, direction_e);
            bitboard between = path & ~kings;

            if(!(between & occupied) && !(path & pieces.attack_mask(opponent(turn))))
            {
                moves.emplace_back(from, to, piece_none);
            }
        }
        if(queenside_castle[turn])
        {
            square from = first_set_square(kings);
            square to = cat_coords(file_c, rank_of(from));
            bitboard path = kings;
            path |= bitboard_shift(path, direction_w);
            path |= bitboard_shift(path, direction_w);
            bitboard between = bitboard_shift(path, direction_w);

            if(!(between & occupied) && !(path & pieces.attack_mask(opponent(turn))))
            {
                moves.emplace_back(from, to, piece_none);
            }
        }
        while(kings)
        {
            square from = first_set_square(kings);
            kings = unset_square(kings, from);
            bitboard attacks = king_attack_mask(from) & attack_mask;
            piecewise_moves(from, attacks, piece_none, moves);
        }

        // remove illegal moves
        side player = turn;
        int n = 0;
        for(move& m: moves)
        {
            undo u = make_move(m);
            if(!(pieces.attack_mask(opponent(player)) & pieces.side_piece_mask(player, piece_king)))
            {
                moves[n++] = m;
            }
            undo_move(m, u);
        }

        moves.resize(n);

        return moves;
    }

    inline int fullmove() const
    {
        return fullmove_number;
    }

    inline int halfmove() const
    {
        return (fullmove_number - 1)*2 + turn;
    }

    inline std::size_t hash() const
    {
        return zobrist_hash ^ pieces.hash();
    }

private:
    inline void piecewise_moves(square from, bitboard tos, piece promote, std::vector<move>& moves)
    {
        while(tos)
        {
            square to = first_set_square(tos);
            tos = unset_square(tos, to);
            moves.emplace_back(from, to, promote);
        }
    }

    inline void setwise_moves(bitboard froms, bitboard tos, piece promote, std::vector<move>& moves)
    {
        while(froms && tos)
        {
            square from = first_set_square(froms);
            square to = first_set_square(tos);
            froms = unset_square(froms, from);
            tos = unset_square(tos, to);
            moves.emplace_back(from, to, promote);
        }
    }

    board pieces;
    side turn;
    std::array<bool, sides> kingside_castle;
    std::array<bool, sides> queenside_castle;
    square en_passant;
    int halfmove_clock;
    int fullmove_number;
    std::size_t zobrist_hash;
};


}


#endif
