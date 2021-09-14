#ifndef CHESS_POSITION_HPP
#define CHESS_POSITION_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <cstdint>

#include "move.hpp"
#include "random.hpp"
#include "bitboard.hpp"
#include "direction.hpp"
#include "board.hpp"


namespace chess
{


#define POSITION_FEN_START  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define POSITION_FEN_EMPTY  "8/8/8/8/8/8/8/8 w - - 0 1"
#define POSITION_FEN_PIECES "rnbqkbnr/8/8/8/8/8/8/RNBQKBNR w KQkq - 0 1"
#define POSITION_FEN_CASTLE "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1"
#define POSITION_FEN_LENGTH 85

#define POSITION_MOVES_SIZE 218 // no positions have been found with more legal moves


void position_init(random* r);


class position
{
public:
    board pieces;
    side turn;
    bool kingside_castle[sides];
    bool queenside_castle[sides];
    square en_passant;
    int halfmove_clock;
    int fullmove_number;
    std::size_t hash;
};


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


void position_from_fen(position* p, const std::string& fen)
{
    if(fen == "startpos")
    {
        // todo: not very clean...
        position_from_fen(p, POSITION_FEN_START);
        return;
    }

    std::string pieces, turn, castle, en_passant;
    int halfmove_clock, fullmove_number;

    std::stringstream fen_stream(fen);

    fen_stream >> pieces >> turn >> castle >> en_passant >> halfmove_clock >> fullmove_number;
 

    // board
    board_clear(&p->pieces);
    int r = rank_8;
    int f = file_a;

    for(char c : pieces)
    {
        if(c == '/')
        {
            r--;
            f = file_a;
            continue;
        }

        if('1' <= c && c <= '8')
        {
            f += c - '0';
            continue;
        }

        side side;
        piece piece = piece_from_san(c, &side);
        square square = square_from_file_rank(static_cast<file>(f), static_cast<rank>(r));

        board_set(&p->pieces, square, side, piece);

        f++;
    }

    // turn
    p->turn = side_from_char(turn.front());
    if(p->turn == side_black) p->hash ^= side_hash;

    // castle
    p->kingside_castle[side_white] = false;
    p->queenside_castle[side_white] = false;
    p->kingside_castle[side_black] = false;
    p->queenside_castle[side_black] = false;

    for(char c : castle)
    {
        switch(c)
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
    if(en_passant == "-")
    {
        p->en_passant = square_none;
    }
    else
    {
        p->en_passant = square_from_string(en_passant);
        p->hash ^= en_passant_hash[square_file(p->en_passant)];
    }

    // move count
    p->halfmove_clock = halfmove_clock;
    p->fullmove_number = fullmove_number;
}



void position_move(position *p, const move* m, undo* u)
{
    if(u != NULL)
    {
        u->capture = board_get(&p->pieces, m->to, NULL);
        u->en_passant = p->en_passant;
        u->kingside_castle[side_white] = p->kingside_castle[side_white];
        u->kingside_castle[side_black] = p->kingside_castle[side_black];
        u->queenside_castle[side_white] = p->queenside_castle[side_white];
        u->queenside_castle[side_black] = p->queenside_castle[side_black];
    }

    board* b = &p->pieces;
    side side;
    piece piece = board_get(b, m->from, &side);
    square ep = p->en_passant;

    board_set(b, m->from, side_none, piece_none);
    if(m->promote != piece_none) board_set(b, m->to, side, m->promote);
    else board_set(b, m->to, side, piece);

    p->en_passant = square_none;
    if(ep != square_none) p->hash ^= en_passant_hash[square_file(ep)];

    if(piece == piece_pawn)
    {
        if(square_rank(m->from) == rank_side(rank_2, side) && square_rank(m->to) == rank_side(rank_4, side))
        {
            // double push
            p->en_passant = square_from_file_rank(square_file(m->from), rank_side(rank_3, side));
            p->hash ^= en_passant_hash[square_file(p->en_passant)];
        }
        else if(m->to == ep)
        {
            // en passant capture
            square ep_capture = square_from_file_rank(square_file(ep), rank_side(rank_5, side));
            board_set(b, ep_capture, side_none, piece_none);
        }
    }
    else if(piece == piece_king)
    {
        if(p->kingside_castle[side])
        {
            p->kingside_castle[side] = false;
            p->hash ^= kingside_castle_hash[side];
        }
        if(p->queenside_castle[side])
        {
            p->queenside_castle[side] = false;
            p->hash ^= queenside_castle_hash[side];
        }
        
        rank rank_first = rank_side(rank_1, side);

        if(m->from == square_from_file_rank(file_e, rank_first))
        {
            if(m->to == square_from_file_rank(file_g, rank_first))
            {
                board_set(b, square_from_file_rank(file_h, rank_first), side_none, piece_none);
                board_set(b, square_from_file_rank(file_f, rank_first), side, piece_rook);
            }
            else if(m->to == square_from_file_rank(file_c, rank_side(rank_1, side)))
            {
                board_set(b, square_from_file_rank(file_a, rank_first), side_none, piece_none);
                board_set(b, square_from_file_rank(file_d, rank_first), side, piece_rook);
            }
        }
    }

    // todo: these (and en passant) should only be updated if they changed from the previous move
    // can probably be done by xoring with current state before each move

    if(p->queenside_castle[side_white] && (m->from == square_a1 || m->to == square_a1))
    {
        p->queenside_castle[side_white] = false;
        p->hash ^= queenside_castle_hash[side_white];
    }
    if(p->kingside_castle[side_white] && (m->from == square_h1 || m->to == square_h1))
    {
        p->kingside_castle[side_white] = false;
        p->hash ^= kingside_castle_hash[side_white];
    }
    if(p->queenside_castle[side_black] && (m->from == square_a8 || m->to == square_a8))
    {
        p->queenside_castle[side_black] = false;
        p->hash ^= queenside_castle_hash[side_black];
    }
    if(p->kingside_castle[side_black] && (m->from == square_h8 || m->to == square_h8))
    {
        p->kingside_castle[side_black] = false;
        p->hash ^= kingside_castle_hash[side_black];
    }

    p->halfmove_clock++; // todo: only if silent
    p->fullmove_number += p->turn;
    p->turn = side_opposite(p->turn);
    p->hash ^= side_hash;
}

void position_undo(position *p, const move* m, const undo* u)
{
    board* b = &p->pieces;
    side side;
    piece piece = board_get(b, m->to, &side);

    board_set(b, m->from, side, piece);
    board_set(b, m->to, side_none, piece_none);
    if(u->capture != piece_none) board_set(b, m->to, side_opposite(side), u->capture);
    if(m->promote != piece_none) board_set(b, m->from, side, piece_pawn);

    if(p->en_passant != square_none) p->hash ^= en_passant_hash[square_file(p->en_passant)];
    if(u->en_passant != square_none) p->hash ^= en_passant_hash[square_file(u->en_passant)];
    p->en_passant = u->en_passant;

    if(p->kingside_castle[side_white] != u->kingside_castle[side_white])
    {
        p->kingside_castle[side_white] = u->kingside_castle[side_white];
        p->hash ^= kingside_castle_hash[side_white];
    }
    if(p->kingside_castle[side_black] != u->kingside_castle[side_black])
    {
        p->kingside_castle[side_black] = u->kingside_castle[side_black];
        p->hash ^= kingside_castle_hash[side_black];
    }
    if(p->queenside_castle[side_white] != u->queenside_castle[side_white])
    {
        p->queenside_castle[side_white] = u->queenside_castle[side_white];
        p->hash ^= queenside_castle_hash[side_white];
    }
    if(p->queenside_castle[side_black] != u->queenside_castle[side_black])
    {
        p->queenside_castle[side_black] = u->queenside_castle[side_black];
        p->hash ^= queenside_castle_hash[side_black];
    }

    if(piece == piece_pawn)
    {
        if(m->to == u->en_passant)
        {
            square ep_capture = square_from_file_rank(square_file(u->en_passant), rank_side(rank_5, side));
            board_set(b, ep_capture, side_opposite(side), piece_pawn);
        }
    }
    else if(piece == piece_king)
    {
        rank rank_first = rank_side(rank_1, side);

        if(m->from == square_from_file_rank(file_e, rank_first))
        {
            if(m->to == square_from_file_rank(file_g, rank_first))
            {
                board_set(b, square_from_file_rank(file_h, rank_first), side, piece_rook);
                board_set(b, square_from_file_rank(file_f, rank_first), side_none, piece_none);
            }
            else if(m->to == square_from_file_rank(file_c, rank_side(rank_1, side)))
            {
                board_set(b, square_from_file_rank(file_a, rank_first), side, piece_rook);
                board_set(b, square_from_file_rank(file_d, rank_first), side_none, piece_none);
            }
        }
    }

    p->halfmove_clock--; // todo: only if silent
    p->fullmove_number -= side_opposite(p->turn); // decrease if white
    p->turn = side_opposite(p->turn);
    p->hash ^= side_hash;
}



int position_moves(position* p, move* moves)
{
    const board* b = &p->pieces;
    side player = p->turn, opponent = side_opposite(p->turn);
    bitboard occupied = board_occupied(b);

    int m = 0;
    
    bitboard pawns = board_side_piece(b, player, piece_pawn);
    bitboard rooks = board_side_piece(b, player, piece_rook);
    bitboard knights = board_side_piece(b, player, piece_knight);
    bitboard bishops = board_side_piece(b, player, piece_bishop);
    bitboard queens = board_side_piece(b, player, piece_queen);
    bitboard kings = board_side_piece(b, player, piece_king);
    
    bitboard attack_mask = ~board_side(b, player);
    bitboard capture_mask = board_side(b, opponent);
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
        square from = bitboard_lsb(rooks);
        rooks = bitboard_reset(rooks, from);
        bitboard attacks = bitboard_rook_attacks(from, occupied) & attack_mask;
        m += move_list_piecewise(from, attacks, piece_none, moves+m);
    }

    // knight moves
    while(knights)
    {
        square from = bitboard_lsb(knights);
        knights = bitboard_reset(knights, from);
        bitboard attacks = bitboard_knight_attacks(from) & attack_mask;
        m += move_list_piecewise(from, attacks, piece_none, moves+m);
    }

    // bishop moves
    while(bishops)
    {
        square from = bitboard_lsb(bishops);
        bishops = bitboard_reset(bishops, from);
        bitboard attacks = bitboard_bishop_attacks(from, occupied) & attack_mask;
        m += move_list_piecewise(from, attacks, piece_none, moves+m);
    }

    // queen moves
    while(queens)
    {
        square from = bitboard_lsb(queens);
        queens = bitboard_reset(queens, from);
        bitboard attacks = (bitboard_rook_attacks(from, occupied) | bitboard_bishop_attacks(from, occupied)) & attack_mask;
        m += move_list_piecewise(from, attacks, piece_none, moves+m);
    }

    // king moves
    if(p->kingside_castle[player])
    {
        square from = bitboard_lsb(kings);
        square to = square_from_file_rank(file_g, square_rank(from));
        bitboard path = kings;
        path |= bitboard_shift(kings, direction_e);
        path |= bitboard_shift(path, direction_e);
        bitboard between = path & ~kings;

        if(!(between & board_occupied(b)) && !(path & board_attacks(b, opponent)))
        {
            move_init(&moves[m++], from, to, piece_none);
        }
    }
    if(p->queenside_castle[player])
    {
        square from = bitboard_lsb(kings);
        square to = square_from_file_rank(file_c, square_rank(from));
        bitboard path = kings;
        path |= bitboard_shift(path, direction_w);
        path |= bitboard_shift(path, direction_w);
        bitboard between = bitboard_shift(path, direction_w);

        if(!(between & board_occupied(b)) && !(path & board_attacks(b, opponent)))
        {
            move_init(&moves[m++], from, to, piece_none);
        }
    }
    while(kings)
    {
        square from = bitboard_lsb(kings);
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
        if(!(board_attacks(b, opponent) & board_side_piece(b, player, piece_king))) moves[n++] = moves[i];
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
    return p->hash ^ p->pieces.hash;
}

void position_copy(const position* src, position* dst)
{
    board_copy(&src->pieces, &dst->pieces);
    
    dst->turn = src->turn;
    dst->kingside_castle[side_white] = src->kingside_castle[side_white];
    dst->kingside_castle[side_black] = src->kingside_castle[side_black];
    dst->queenside_castle[side_white] = src->queenside_castle[side_white];
    dst->queenside_castle[side_black] = src->queenside_castle[side_black];
    dst->en_passant = src->en_passant;
    dst->halfmove_clock = src->halfmove_clock;
    dst->fullmove_number = src->fullmove_number;
    
    dst->hash = src->hash;
}

}


#endif
