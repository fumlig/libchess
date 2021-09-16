#ifndef CHESS_POSITION_HPP
#define CHESS_POSITION_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <cstdint>
#include <array>
#include <vector>

#include "types.hpp"
#include "random.hpp"
#include "bitboard.hpp"
#include "board.hpp"


namespace chess
{


static std::size_t side_hash;
static std::size_t kingside_castle_hash[sides];
static std::size_t queenside_castle_hash[sides];
static std::size_t en_passant_hash[files];

void position_init(random* r)
{
    side_hash = random_generate(r);

    kingside_castle_hash[side_white] = random_generate(r);
    kingside_castle_hash[side_black] = random_generate(r);
    queenside_castle_hash[side_white] = random_generate(r);
    queenside_castle_hash[side_black] = random_generate(r);

    for(int f = file_a; f <= file_h; f++)
    {
        en_passant_hash[f] = random_generate(r);
    }
}




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
        if(turn == side_black)              zobrist_hash ^= side_hash;
        if(kingside_castle[side_white])     zobrist_hash ^= kingside_castle_hash[side_white];
        if(queenside_castle[side_white])    zobrist_hash ^= queenside_castle_hash[side_white];
        if(kingside_castle[side_black])     zobrist_hash ^= kingside_castle_hash[side_black];
        if(queenside_castle[side_black])    zobrist_hash ^= queenside_castle_hash[side_black];
        if(en_passant != square_none)       zobrist_hash ^= en_passant_hash[square_file(en_passant)];
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
        if(ep != square_none) zobrist_hash ^= en_passant_hash[square_file(ep)];

        if(piece == piece_pawn)
        {
            if(square_rank(m.from) == rank_side(rank_2, side) && square_rank(m.to) == rank_side(rank_4, side))
            {
                // double push
                en_passant = square_from_file_rank(square_file(m.from), rank_side(rank_3, side));
                zobrist_hash ^= en_passant_hash[square_file(en_passant)];
            }
            else if(m.to == ep)
            {
                // en passant capture
                square ep_capture = square_from_file_rank(square_file(ep), rank_side(rank_5, side));
                pieces.set(ep_capture, side_none, piece_none);
            }
        }
        else if(piece == piece_king)
        {
            if(kingside_castle[side])
            {
                kingside_castle[side] = false;
                zobrist_hash ^= kingside_castle_hash[side];
            }
            if(queenside_castle[side])
            {
                queenside_castle[side] = false;
                zobrist_hash ^= queenside_castle_hash[side];
            }
            
            rank rank_first = rank_side(rank_1, side);

            if(m.from == square_from_file_rank(file_e, rank_first))
            {
                if(m.to == square_from_file_rank(file_g, rank_first))
                {
                    pieces.set(square_from_file_rank(file_h, rank_first), side_none, piece_none);
                    pieces.set(square_from_file_rank(file_f, rank_first), side, piece_rook);
                }
                else if(m.to == square_from_file_rank(file_c, rank_side(rank_1, side)))
                {
                    pieces.set(square_from_file_rank(file_a, rank_first), side_none, piece_none);
                    pieces.set(square_from_file_rank(file_d, rank_first), side, piece_rook);
                }
            }
        }

        // todo: these (and en passant) should only be updated if they changed from the previous move
        // can probably be done by xoring with current state before each move

        if(queenside_castle[side_white] && (m.from == square_a1 || m.to == square_a1))
        {
            queenside_castle[side_white] = false;
            zobrist_hash ^= queenside_castle_hash[side_white];
        }
        if(kingside_castle[side_white] && (m.from == square_h1 || m.to == square_h1))
        {
            kingside_castle[side_white] = false;
            zobrist_hash ^= kingside_castle_hash[side_white];
        }
        if(queenside_castle[side_black] && (m.from == square_a8 || m.to == square_a8))
        {
            queenside_castle[side_black] = false;
            zobrist_hash ^= queenside_castle_hash[side_black];
        }
        if(kingside_castle[side_black] && (m.from == square_h8 || m.to == square_h8))
        {
            kingside_castle[side_black] = false;
            zobrist_hash ^= kingside_castle_hash[side_black];
        }

        halfmove_clock++; // todo: only if silent
        fullmove_number += turn;
        turn = side_opposite(turn);
        zobrist_hash ^= side_hash;

        return u;
    }


    void undo_move(const move& m, const undo& u)
    {
        auto [side, piece] = pieces.get(m.to);

        pieces.set(m.from, side, piece);
        pieces.set(m.to, side_none, piece_none);
        if(u.capture != piece_none) pieces.set(m.to, side_opposite(side), u.capture);
        if(m.promote != piece_none) pieces.set(m.from, side, piece_pawn);

        if(en_passant != square_none) zobrist_hash ^= en_passant_hash[square_file(en_passant)];
        if(u.en_passant != square_none) zobrist_hash ^= en_passant_hash[square_file(u.en_passant)];
        en_passant = u.en_passant;

        if(kingside_castle[side_white] != u.kingside_castle[side_white])
        {
            kingside_castle[side_white] = u.kingside_castle[side_white];
            zobrist_hash ^= kingside_castle_hash[side_white];
        }
        if(kingside_castle[side_black] != u.kingside_castle[side_black])
        {
            kingside_castle[side_black] = u.kingside_castle[side_black];
            zobrist_hash ^= kingside_castle_hash[side_black];
        }
        if(queenside_castle[side_white] != u.queenside_castle[side_white])
        {
            queenside_castle[side_white] = u.queenside_castle[side_white];
            zobrist_hash ^= queenside_castle_hash[side_white];
        }
        if(queenside_castle[side_black] != u.queenside_castle[side_black])
        {
            queenside_castle[side_black] = u.queenside_castle[side_black];
            zobrist_hash ^= queenside_castle_hash[side_black];
        }

        if(piece == piece_pawn)
        {
            if(m.to == u.en_passant)
            {
                square ep_capture = square_from_file_rank(square_file(u.en_passant), rank_side(rank_5, side));
                pieces.set(ep_capture, side_opposite(side), piece_pawn);
            }
        }
        else if(piece == piece_king)
        {
            rank rank_first = rank_side(rank_1, side);

            if(m.from == square_from_file_rank(file_e, rank_first))
            {
                if(m.to == square_from_file_rank(file_g, rank_first))
                {
                    pieces.set(square_from_file_rank(file_h, rank_first), side, piece_rook);
                    pieces.set(square_from_file_rank(file_f, rank_first), side_none, piece_none);
                }
                else if(m.to == square_from_file_rank(file_c, rank_side(rank_1, side)))
                {
                    pieces.set(square_from_file_rank(file_a, rank_first), side, piece_rook);
                    pieces.set(square_from_file_rank(file_d, rank_first), side_none, piece_none);
                }
            }
        }

        halfmove_clock--; // todo: only if silent
        fullmove_number -= side_opposite(turn); // decrease if white
        turn = side_opposite(turn);
        zobrist_hash ^= side_hash;
    }

    std::vector<move> moves()
    {
        std::vector<move> moves;

        side player = turn, opponent = side_opposite(turn);
        bitboard occupied = pieces.occupied_mask();
        
        bitboard pawns = pieces.side_piece_mask(player, piece_pawn);
        bitboard rooks = pieces.side_piece_mask(player, piece_rook);
        bitboard knights = pieces.side_piece_mask(player, piece_knight);
        bitboard bishops = pieces.side_piece_mask(player, piece_bishop);
        bitboard queens = pieces.side_piece_mask(player, piece_queen);
        bitboard kings = pieces.side_piece_mask(player, piece_king);
        
        bitboard attack_mask = ~pieces.side_mask(player);
        bitboard capture_mask = pieces.side_mask(opponent);
        bitboard ep_mask = 0;
        if(en_passant != square_none) ep_mask = square_mask(en_passant);

        // pawn moves
        bitboard single_push_tos = bitboard_shift(pawns, direction_forward(player)) & ~occupied;
        bitboard single_push_froms = bitboard_shift(single_push_tos, direction_forward(opponent));  
        bitboard double_push_tos = bitboard_shift(single_push_tos & rank_mask(rank_side(rank_3, player)), direction_forward(player)) & ~occupied;
        bitboard double_push_froms = bitboard_shift(bitboard_shift(double_push_tos, direction_forward(opponent)), direction_forward(opponent));

        bitboard attack_east_tos = bitboard_shift(pawns, static_cast<direction>(direction_forward(player) + direction_e)) & (capture_mask | ep_mask);
        bitboard attack_east_froms = bitboard_shift(attack_east_tos, static_cast<direction>(direction_forward(opponent) + direction_w));
        bitboard attack_west_tos = bitboard_shift(pawns, static_cast<direction>(direction_forward(player) + direction_w)) & (capture_mask | ep_mask);
        bitboard attack_west_froms = bitboard_shift(attack_west_tos, static_cast<direction>(direction_forward(opponent) + direction_e));
        
        bitboard promote_push_tos = single_push_tos & rank_mask(rank_side(rank_8, player));
        bitboard promote_push_froms = single_push_froms & rank_mask(rank_side(rank_7, player));
        bitboard promote_east_tos = attack_east_tos & rank_mask(rank_side(rank_8, player));
        bitboard promote_east_froms = attack_east_froms & rank_mask(rank_side(rank_7, player));
        bitboard promote_west_tos = attack_west_tos & rank_mask(rank_side(rank_8, player));
        bitboard promote_west_froms = attack_west_froms & rank_mask(rank_side(rank_7, player));;

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
            square from = bitboard_ls1b(rooks);
            rooks = bitboard_reset(rooks, from);
            bitboard attacks = bitboard_rook_attacks(from, occupied) & attack_mask;
            piecewise_moves(from, attacks, piece_none, moves);
        }

        // knight moves
        while(knights)
        {
            square from = bitboard_ls1b(knights);
            knights = bitboard_reset(knights, from);
            bitboard attacks = bitboard_knight_attacks(from) & attack_mask;
            piecewise_moves(from, attacks, piece_none, moves);
        }

        // bishop moves
        while(bishops)
        {
            square from = bitboard_ls1b(bishops);
            bishops = bitboard_reset(bishops, from);
            bitboard attacks = bitboard_bishop_attacks(from, occupied) & attack_mask;
            piecewise_moves(from, attacks, piece_none, moves);
        }

        // queen moves
        while(queens)
        {
            square from = bitboard_ls1b(queens);
            queens = bitboard_reset(queens, from);
            bitboard attacks = (bitboard_rook_attacks(from, occupied) | bitboard_bishop_attacks(from, occupied)) & attack_mask;
            piecewise_moves(from, attacks, piece_none, moves);
        }

        // king moves
        if(kingside_castle[player])
        {
            square from = bitboard_ls1b(kings);
            square to = square_from_file_rank(file_g, square_rank(from));
            bitboard path = kings;
            path |= bitboard_shift(kings, direction_e);
            path |= bitboard_shift(path, direction_e);
            bitboard between = path & ~kings;

            if(!(between & pieces.occupied_mask()) && !(path & pieces.attack_mask(opponent)))
            {
                moves.emplace_back(from, to, piece_none);
            }
        }
        if(queenside_castle[player])
        {
            square from = bitboard_ls1b(kings);
            square to = square_from_file_rank(file_c, square_rank(from));
            bitboard path = kings;
            path |= bitboard_shift(path, direction_w);
            path |= bitboard_shift(path, direction_w);
            bitboard between = bitboard_shift(path, direction_w);

            if(!(between & pieces.occupied_mask()) && !(path & pieces.attack_mask(opponent)))
            {
                moves.emplace_back(from, to, piece_none);
            }
        }
        while(kings)
        {
            square from = bitboard_ls1b(kings);
            kings = bitboard_reset(kings, from);
            bitboard attacks = bitboard_king_attacks(from) & attack_mask;
            piecewise_moves(from, attacks, piece_none, moves);
        }

        // remove illegal moves
        int n = 0;
        for(move& m: moves)
        {
            undo u = make_move(m);
            if(!(pieces.attack_mask(opponent) & pieces.side_piece_mask(player, piece_king)))
            {
                moves[n++] = m;
            }
            undo_move(m, u);
        }

        moves.resize(n);

        return moves;
    }

    int fullmove(position* p) const
    {
        return fullmove_number;
    }

    int halfmove(position* p) const
    {
        return (fullmove_number - 1)*2 + turn;
    }

    std::size_t hash() const
    {
        return zobrist_hash ^ pieces.hash();
    }

    board pieces;
    side turn;
    std::array<bool, sides> kingside_castle;
    std::array<bool, sides> queenside_castle;
    square en_passant;
    int halfmove_clock;
    int fullmove_number;
    std::size_t zobrist_hash;

private:
    void piecewise_moves(square from, bitboard tos, piece promote, std::vector<move>& moves)
    {
        while(tos)
        {
            square to = bitboard_ls1b(tos);
            tos = bitboard_reset(tos, to);
            moves.emplace_back(from, to, promote);
        }
    }

    void setwise_moves(bitboard froms, bitboard tos, piece promote, std::vector<move>& moves)
    {
        while(froms && tos)
        {
            square from = bitboard_ls1b(froms);
            square to = bitboard_ls1b(tos);
            froms = bitboard_reset(froms, from);
            tos = bitboard_reset(tos, to);
            moves.emplace_back(from, to, promote);
        }
    }
};


}


#endif
