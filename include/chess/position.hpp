#ifndef CHESS_POSITION_HPP
#define CHESS_POSITION_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <cstdint>
#include <array>
#include <string.h>

#include "types.hpp"
#include "move.hpp"
#include "random.hpp"
#include "bitboard.hpp"
#include "board.hpp"


namespace chess
{


#define POSITION_FEN_START  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define POSITION_FEN_EMPTY  "8/8/8/8/8/8/8/8 w - - 0 1"
#define POSITION_FEN_PIECES "rnbqkbnr/8/8/8/8/8/8/RNBQKBNR w KQkq - 0 1"
#define POSITION_FEN_CASTLE "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1"
#define POSITION_FEN_LENGTH 85

#define POSITION_MOVES_SIZE 218 // no positions have been found with more legal moves


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

    board pieces;
    side turn;
    std::array<bool, sides> kingside_castle;
    std::array<bool, sides> queenside_castle;
    square en_passant;
    int halfmove_clock;
    int fullmove_number;
    std::size_t zobrist_hash;
};


#if 0
void position_from_fen(struct position* p, const char* fen)
{
    char board[8*8+7+1];
    char turn;
    char castle[4+1];
    char en_passant[2+1];
    int halfmove_clock;
    int fullmove_number;

    sscanf(fen, "%s %c %s %s %d %d", board, &turn, castle, en_passant, &halfmove_clock, &fullmove_number);

    // board
    board_clear(&p->pieces);
    int r = rank_8;
    int f = file_a;

    for(int i = 0; i < strlen(board); i++)
    {
        if(board[i] == '/')
        {
            r--;
            f = file_a;
            continue;
        }

        if('1' <= board[i] && board[i] <= '8')
        {
            f += board[i] - '0';
            continue;
        }

        enum side side;
        enum piece piece;

        from_san(std::string{board[i]}, side, piece);

        enum square square = square_from_file_rank(static_cast<file>(f), static_cast<rank>(r));

        board_set(&p->pieces, square, side, piece);

        f++;
    }

    // turn
	if(turn == 'w')
	{
		p->turn = side_white;
	}
	else if(turn == 'b')
	{
		p->turn = side_black;
        if(p->turn == side_black) p->hash ^= side_hash;
	}

    // castle
    p->kingside_castle[side_white] = false;
    p->queenside_castle[side_white] = false;
    p->kingside_castle[side_black] = false;
    p->queenside_castle[side_black] = false;

    for(int i = 0; i < strlen(castle); i++)
    {
        switch(castle[i])
        {
        case 'K':
            p->kingside_castle[side_white] = true;
            p->hash ^= kingside_castle_hash[side_white];
            break;
        case 'Q':
            p->queenside_castle[side_white] = true;
            p->hash ^= queenside_castle_hash[side_white];
            break;
        case 'k':
            p->kingside_castle[side_black] = true;
            p->hash ^= kingside_castle_hash[side_black];
            break;
        case 'q':
            p->queenside_castle[side_black] = true;
            p->hash ^= queenside_castle_hash[side_black];
            break;
        case '-':
        default:
            break;
        }
    }

    // en passant
    if(strcmp(en_passant, "-") == 0)
    {
        p->en_passant = square_none;
    }
    else
    {
        from_san(en_passant, p->en_passant);
        p->hash ^= en_passant_hash[square_file(p->en_passant)];
    }

    // move count
    p->halfmove_clock = halfmove_clock;
    p->fullmove_number = fullmove_number;
}
#endif


void position_move(position *p, const move* m, undo* u)
{
    if(u != NULL)
    {
        auto [_, capture] = p->pieces.get(m->to);

        u->capture = capture;
        u->en_passant = p->en_passant;
        u->kingside_castle[side_white] = p->kingside_castle[side_white];
        u->kingside_castle[side_black] = p->kingside_castle[side_black];
        u->queenside_castle[side_white] = p->queenside_castle[side_white];
        u->queenside_castle[side_black] = p->queenside_castle[side_black];
    }

    board* b = &p->pieces;
    auto [side, piece] = b->get(m->from);
    square ep = p->en_passant;

    b->set(m->from, side_none, piece_none);

    if(m->promote != piece_none) b->set(m->to, side, m->promote);
    else b->set(m->to, side, piece);

    p->en_passant = square_none;
    if(ep != square_none) p->zobrist_hash ^= en_passant_hash[square_file(ep)];

    if(piece == piece_pawn)
    {
        if(square_rank(m->from) == rank_side(rank_2, side) && square_rank(m->to) == rank_side(rank_4, side))
        {
            // double push
            p->en_passant = square_from_file_rank(square_file(m->from), rank_side(rank_3, side));
            p->zobrist_hash ^= en_passant_hash[square_file(p->en_passant)];
        }
        else if(m->to == ep)
        {
            // en passant capture
            square ep_capture = square_from_file_rank(square_file(ep), rank_side(rank_5, side));
            b->set(ep_capture, side_none, piece_none);
        }
    }
    else if(piece == piece_king)
    {
        if(p->kingside_castle[side])
        {
            p->kingside_castle[side] = false;
            p->zobrist_hash ^= kingside_castle_hash[side];
        }
        if(p->queenside_castle[side])
        {
            p->queenside_castle[side] = false;
            p->zobrist_hash ^= queenside_castle_hash[side];
        }
        
        rank rank_first = rank_side(rank_1, side);

        if(m->from == square_from_file_rank(file_e, rank_first))
        {
            if(m->to == square_from_file_rank(file_g, rank_first))
            {
                b->set(square_from_file_rank(file_h, rank_first), side_none, piece_none);
                b->set(square_from_file_rank(file_f, rank_first), side, piece_rook);
            }
            else if(m->to == square_from_file_rank(file_c, rank_side(rank_1, side)))
            {
                b->set(square_from_file_rank(file_a, rank_first), side_none, piece_none);
                b->set(square_from_file_rank(file_d, rank_first), side, piece_rook);
            }
        }
    }

    // todo: these (and en passant) should only be updated if they changed from the previous move
    // can probably be done by xoring with current state before each move

    if(p->queenside_castle[side_white] && (m->from == square_a1 || m->to == square_a1))
    {
        p->queenside_castle[side_white] = false;
        p->zobrist_hash ^= queenside_castle_hash[side_white];
    }
    if(p->kingside_castle[side_white] && (m->from == square_h1 || m->to == square_h1))
    {
        p->kingside_castle[side_white] = false;
        p->zobrist_hash ^= kingside_castle_hash[side_white];
    }
    if(p->queenside_castle[side_black] && (m->from == square_a8 || m->to == square_a8))
    {
        p->queenside_castle[side_black] = false;
        p->zobrist_hash ^= queenside_castle_hash[side_black];
    }
    if(p->kingside_castle[side_black] && (m->from == square_h8 || m->to == square_h8))
    {
        p->kingside_castle[side_black] = false;
        p->zobrist_hash ^= kingside_castle_hash[side_black];
    }

    p->halfmove_clock++; // todo: only if silent
    p->fullmove_number += p->turn;
    p->turn = side_opposite(p->turn);
    p->zobrist_hash ^= side_hash;
}

void position_undo(position *p, const move* m, const undo* u)
{
    board* b = &p->pieces;
    auto [side, piece] = b->get(m->to);

    b->set(m->from, side, piece);
    b->set(m->to, side_none, piece_none);
    if(u->capture != piece_none) b->set(m->to, side_opposite(side), u->capture);
    if(m->promote != piece_none) b->set(m->from, side, piece_pawn);

    if(p->en_passant != square_none) p->zobrist_hash ^= en_passant_hash[square_file(p->en_passant)];
    if(u->en_passant != square_none) p->zobrist_hash ^= en_passant_hash[square_file(u->en_passant)];
    p->en_passant = u->en_passant;

    if(p->kingside_castle[side_white] != u->kingside_castle[side_white])
    {
        p->kingside_castle[side_white] = u->kingside_castle[side_white];
        p->zobrist_hash ^= kingside_castle_hash[side_white];
    }
    if(p->kingside_castle[side_black] != u->kingside_castle[side_black])
    {
        p->kingside_castle[side_black] = u->kingside_castle[side_black];
        p->zobrist_hash ^= kingside_castle_hash[side_black];
    }
    if(p->queenside_castle[side_white] != u->queenside_castle[side_white])
    {
        p->queenside_castle[side_white] = u->queenside_castle[side_white];
        p->zobrist_hash ^= queenside_castle_hash[side_white];
    }
    if(p->queenside_castle[side_black] != u->queenside_castle[side_black])
    {
        p->queenside_castle[side_black] = u->queenside_castle[side_black];
        p->zobrist_hash ^= queenside_castle_hash[side_black];
    }

    if(piece == piece_pawn)
    {
        if(m->to == u->en_passant)
        {
            square ep_capture = square_from_file_rank(square_file(u->en_passant), rank_side(rank_5, side));
            b->set(ep_capture, side_opposite(side), piece_pawn);
        }
    }
    else if(piece == piece_king)
    {
        rank rank_first = rank_side(rank_1, side);

        if(m->from == square_from_file_rank(file_e, rank_first))
        {
            if(m->to == square_from_file_rank(file_g, rank_first))
            {
                b->set(square_from_file_rank(file_h, rank_first), side, piece_rook);
                b->set(square_from_file_rank(file_f, rank_first), side_none, piece_none);
            }
            else if(m->to == square_from_file_rank(file_c, rank_side(rank_1, side)))
            {
                b->set(square_from_file_rank(file_a, rank_first), side, piece_rook);
                b->set(square_from_file_rank(file_d, rank_first), side_none, piece_none);
            }
        }
    }

    p->halfmove_clock--; // todo: only if silent
    p->fullmove_number -= side_opposite(p->turn); // decrease if white
    p->turn = side_opposite(p->turn);
    p->zobrist_hash ^= side_hash;
}



int position_moves(position* p, move* moves)
{
    const board* b = &p->pieces;
    side player = p->turn, opponent = side_opposite(p->turn);
    bitboard occupied = b->occupied_mask();

    int m = 0;
    
    bitboard pawns = b->side_piece_mask(player, piece_pawn);
    bitboard rooks = b->side_piece_mask(player, piece_rook);
    bitboard knights = b->side_piece_mask(player, piece_knight);
    bitboard bishops = b->side_piece_mask(player, piece_bishop);
    bitboard queens = b->side_piece_mask(player, piece_queen);
    bitboard kings = b->side_piece_mask(player, piece_king);
    
    bitboard attack_mask = ~b->side_mask(player);
    bitboard capture_mask = b->side_mask(opponent);
    bitboard ep_mask = 0;
    if(p->en_passant != square_none) ep_mask = square_mask(p->en_passant);

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

    m += move_list_setwise(single_push_froms, single_push_tos, piece_none, moves+m);
    m += move_list_setwise(double_push_froms, double_push_tos, piece_none, moves+m);
    
    m += move_list_setwise(attack_east_froms, attack_east_tos, piece_none, moves+m);
    m += move_list_setwise(attack_west_froms, attack_west_tos, piece_none, moves+m);
    
    m += move_list_setwise(promote_push_froms, promote_push_tos, piece_rook, moves+m);
    m += move_list_setwise(promote_push_froms, promote_push_tos, piece_knight, moves+m);
    m += move_list_setwise(promote_push_froms, promote_push_tos, piece_bishop, moves+m);
    m += move_list_setwise(promote_push_froms, promote_push_tos, piece_queen, moves+m);

    m += move_list_setwise(promote_east_froms, promote_east_tos, piece_rook, moves+m);
    m += move_list_setwise(promote_east_froms, promote_east_tos, piece_knight, moves+m);
    m += move_list_setwise(promote_east_froms, promote_east_tos, piece_bishop, moves+m);
    m += move_list_setwise(promote_east_froms, promote_east_tos, piece_queen, moves+m);

    m += move_list_setwise(promote_west_froms, promote_west_tos, piece_rook, moves+m);
    m += move_list_setwise(promote_west_froms, promote_west_tos, piece_knight, moves+m);
    m += move_list_setwise(promote_west_froms, promote_west_tos, piece_bishop, moves+m);
    m += move_list_setwise(promote_west_froms, promote_west_tos, piece_queen, moves+m);

    // rook moves
    while(rooks)
    {
        square from = bitboard_ls1b(rooks);
        rooks = bitboard_reset(rooks, from);
        bitboard attacks = bitboard_rook_attacks(from, occupied) & attack_mask;
        m += move_list_piecewise(from, attacks, piece_none, moves+m);
    }

    // knight moves
    while(knights)
    {
        square from = bitboard_ls1b(knights);
        knights = bitboard_reset(knights, from);
        bitboard attacks = bitboard_knight_attacks(from) & attack_mask;
        m += move_list_piecewise(from, attacks, piece_none, moves+m);
    }

    // bishop moves
    while(bishops)
    {
        square from = bitboard_ls1b(bishops);
        bishops = bitboard_reset(bishops, from);
        bitboard attacks = bitboard_bishop_attacks(from, occupied) & attack_mask;
        m += move_list_piecewise(from, attacks, piece_none, moves+m);
    }

    // queen moves
    while(queens)
    {
        square from = bitboard_ls1b(queens);
        queens = bitboard_reset(queens, from);
        bitboard attacks = (bitboard_rook_attacks(from, occupied) | bitboard_bishop_attacks(from, occupied)) & attack_mask;
        m += move_list_piecewise(from, attacks, piece_none, moves+m);
    }

    // king moves
    if(p->kingside_castle[player])
    {
        square from = bitboard_ls1b(kings);
        square to = square_from_file_rank(file_g, square_rank(from));
        bitboard path = kings;
        path |= bitboard_shift(kings, direction_e);
        path |= bitboard_shift(path, direction_e);
        bitboard between = path & ~kings;

        if(!(between & b->occupied_mask()) && !(path & b->attack_mask(opponent)))
        {
            move_init(&moves[m++], from, to, piece_none);
        }
    }
    if(p->queenside_castle[player])
    {
        square from = bitboard_ls1b(kings);
        square to = square_from_file_rank(file_c, square_rank(from));
        bitboard path = kings;
        path |= bitboard_shift(path, direction_w);
        path |= bitboard_shift(path, direction_w);
        bitboard between = bitboard_shift(path, direction_w);

        if(!(between & b->occupied_mask()) && !(path & b->attack_mask(opponent)))
        {
            move_init(&moves[m++], from, to, piece_none);
        }
    }
    while(kings)
    {
        square from = bitboard_ls1b(kings);
        kings = bitboard_reset(kings, from);
        bitboard attacks = bitboard_king_attacks(from) & attack_mask;
        m += move_list_piecewise(from, attacks, piece_none, moves+m);
    }

    // remove illegal moves
    int n = 0;
    for(int i = 0; i < m; i++)
    {
        undo u;
        position_move(p, &moves[i], &u);
        if(!(b->attack_mask(opponent) & b->side_piece_mask(player, piece_king))) moves[n++] = moves[i];
        position_undo(p, &moves[i], &u);
    }

    return n;
}


int position_fullmove(position* p)
{
    return p->fullmove_number;
}

int position_halfmove(position* p)
{
    return (p->fullmove_number - 1)*2 + p->turn;
}


std::size_t position_hash(const position* p)
{
    return p->zobrist_hash ^ p->pieces.zobrist_hash;
}


}


#endif
