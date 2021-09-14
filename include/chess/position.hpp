#ifndef CHESS_POSITION_HPP
#define CHESS_POSITION_HPP


#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


struct position
{
    board board;
    side turn;
    bool kingside_castle[SIDES];
    bool queenside_castle[SIDES];
    square en_passant;
    int halfmove_clock;
    int fullmove_number;
    uint64_t hash;
};

void position_from_fen(position* p, const char* fen);
void position_to_fen(const position* p, char* fen);

int position_moves(position* p, move* moves);
void position_move(position* p, const move* m, undo* u);
void position_undo(position* p, const move* m, const undo* u);

int position_fullmove(position* p);
int position_halfmove(position* p);

uint64_t position_hash(const position* p);
void position_copy(const position* src, position* dst);
void position_print(const position* p);




static uint64_t side_hash;
static uint64_t kingside_castle_hash[SIDES];
static uint64_t queenside_castle_hash[SIDES];
static uint64_t en_passant_hash[FILES];

void position_init(random* r)
{
    side_hash = random_generate(r);

    kingside_castle_hash[SIDE_WHITE] = random_generate(r);
    kingside_castle_hash[SIDE_BLACK] = random_generate(r);
    queenside_castle_hash[SIDE_WHITE] = random_generate(r);
    queenside_castle_hash[SIDE_BLACK] = random_generate(r);

    for(int f = FILE_A; f <= FILE_H; f++)
    {
        en_passant_hash[f] = random_generate(r);
    }
}


void position_from_fen(position* p, const char* fen)
{
    if(strcmp(fen, "startpos") == 0)
    {
        // todo: not very clean...
        position_from_fen(p, POSITION_FEN_START);
        return;
    }

    char board[8*8+7+1];
    char turn;
    char castle[4+1];
    char en_passant[2+1];
    int halfmove_clock;
    int fullmove_number;

    sscanf(fen, "%s %c %s %s %d %d", board, &turn, castle, en_passant, &halfmove_clock, &fullmove_number);

    // board
    board_clear(&p->board);
    int r = RANK_8;
    int f = FILE_A;

    for(int i = 0; i < strlen(board); i++)
    {
        if(board[i] == '/')
        {
            r--;
            f = FILE_A;
            continue;
        }

        if('1' <= board[i] && board[i] <= '8')
        {
            f += board[i] - '0';
            continue;
        }

        side side;
        piece piece = piece_from_san(board[i], &side);
        square square = square_from_file_rank(static_cast<file>(f), static_cast<rank>(r));

        board_set(&p->board, square, side, piece);

        f++;
    }

    // turn
    p->turn = side_from_char(turn);
    if(p->turn == SIDE_BLACK) p->hash ^= side_hash;

    // castle
    p->kingside_castle[SIDE_WHITE] = false;
    p->queenside_castle[SIDE_WHITE] = false;
    p->kingside_castle[SIDE_BLACK] = false;
    p->queenside_castle[SIDE_BLACK] = false;

    for(int i = 0; i < strlen(castle); i++)
    {
        switch(castle[i])
        {
        case 'K':
            p->kingside_castle[SIDE_WHITE] = true;
            p->hash ^= kingside_castle_hash[SIDE_WHITE];
            break;
        case 'Q':
            p->queenside_castle[SIDE_WHITE] = true;
            p->hash ^= queenside_castle_hash[SIDE_WHITE];
            break;
        case 'k':
            p->kingside_castle[SIDE_BLACK] = true;
            p->hash ^= kingside_castle_hash[SIDE_BLACK];
            break;
        case 'q':
            p->queenside_castle[SIDE_BLACK] = true;
            p->hash ^= queenside_castle_hash[SIDE_BLACK];
            break;
        case '-':
        default:
            break;
        }
    }

    // en passant
    if(strcmp(en_passant, "-") == 0)
    {
        p->en_passant = SQUARE_NONE;
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

void position_to_fen(const position* p, char* fen)
{
    int i = 0;

    const board* b = &p->board;
    for(int r = RANK_8; r >= RANK_1; r--)
    {
        int empty = 0;
        for(int f = FILE_A; f <= FILE_H; f++)
        {
            square sq = square_from_file_rank(static_cast<file>(f), static_cast<rank>(r));
            side s;
            piece p = board_get(b, sq, &s);

            if(p == PIECE_NONE)
            {
                empty++;
                continue;
            }
            else if(empty != 0)
            {
                fen[i++] = '0' + empty;
                empty = 0;
            }

            fen[i++] = piece_to_san(p, s);
        }

        if(empty != 0) fen[i++] = '0' + empty;
        if(r != RANK_1) fen[i++] = '/';
    }
    
    char turn = side_to_char(p->turn);
    char castle[5]; int c = 0;
    if(p->kingside_castle[SIDE_WHITE]) castle[c++] = 'K';
    if(p->queenside_castle[SIDE_WHITE]) castle[c++] = 'Q';
    if(p->kingside_castle[SIDE_BLACK]) castle[c++] = 'k';
    if(p->queenside_castle[SIDE_BLACK]) castle[c++] = 'q';
    if(c == 0) castle[c++] = '-';
    castle[c] = '\0';
    char en_passant[3];
    square_to_string(p->en_passant, en_passant);

    sprintf(fen+i, " %c %s %s %d %d", turn, castle, en_passant, p->halfmove_clock, p->fullmove_number);
}


int position_moves(position* p, move* moves)
{
    const board* b = &p->board;
    side player = p->turn, opponent = side_opposite(p->turn);
    bitboard occupied = board_occupied(b);

    int m = 0;
    
    bitboard pawns = board_side_piece(b, player, PIECE_PAWN);
    bitboard rooks = board_side_piece(b, player, PIECE_ROOK);
    bitboard knights = board_side_piece(b, player, PIECE_KNIGHT);
    bitboard bishops = board_side_piece(b, player, PIECE_BISHOP);
    bitboard queens = board_side_piece(b, player, PIECE_QUEEN);
    bitboard kings = board_side_piece(b, player, PIECE_KING);
    
    bitboard attack_mask = ~board_side(b, player);
    bitboard capture_mask = board_side(b, opponent);
    bitboard ep_mask = 0;
    if(p->en_passant != SQUARE_NONE) ep_mask = bitboard_square(p->en_passant);

    // pawn moves
    bitboard single_push_tos = bitboard_shift(pawns, direction_forward(player)) & ~occupied;
    bitboard single_push_froms = bitboard_shift(single_push_tos, direction_forward(opponent));  
    bitboard double_push_tos = bitboard_shift(single_push_tos & bitboard_rank(rank_side(RANK_3, player)), direction_forward(player)) & ~occupied;
    bitboard double_push_froms = bitboard_shift(bitboard_shift(double_push_tos, direction_forward(opponent)), direction_forward(opponent));

    bitboard attack_east_tos = bitboard_shift(pawns, static_cast<direction>(direction_forward(player) + DIRECTION_E)) & (capture_mask | ep_mask);
    bitboard attack_east_froms = bitboard_shift(attack_east_tos, static_cast<direction>(direction_forward(opponent) + DIRECTION_W));
    bitboard attack_west_tos = bitboard_shift(pawns, static_cast<direction>(direction_forward(player) + DIRECTION_W)) & (capture_mask | ep_mask);
    bitboard attack_west_froms = bitboard_shift(attack_west_tos, static_cast<direction>(direction_forward(opponent) + DIRECTION_E));
    
    bitboard promote_push_tos = single_push_tos & bitboard_rank(rank_side(RANK_8, player));
    bitboard promote_push_froms = single_push_froms & bitboard_rank(rank_side(RANK_7, player));
    bitboard promote_east_tos = attack_east_tos & bitboard_rank(rank_side(RANK_8, player));
    bitboard promote_east_froms = attack_east_froms & bitboard_rank(rank_side(RANK_7, player));
    bitboard promote_west_tos = attack_west_tos & bitboard_rank(rank_side(RANK_8, player));
    bitboard promote_west_froms = attack_west_froms & bitboard_rank(rank_side(RANK_7, player));;

    single_push_tos ^= promote_push_tos;
    single_push_froms ^= promote_push_froms;
    attack_east_tos ^= promote_east_tos;
    attack_east_froms ^= promote_east_froms;
    attack_west_tos ^= promote_west_tos;
    attack_west_froms ^= promote_west_froms;

    m += move_list_setwise(single_push_froms, single_push_tos, PIECE_NONE, moves+m);
    m += move_list_setwise(double_push_froms, double_push_tos, PIECE_NONE, moves+m);
    
    m += move_list_setwise(attack_east_froms, attack_east_tos, PIECE_NONE, moves+m);
    m += move_list_setwise(attack_west_froms, attack_west_tos, PIECE_NONE, moves+m);
    
    m += move_list_setwise(promote_push_froms, promote_push_tos, PIECE_ROOK, moves+m);
    m += move_list_setwise(promote_push_froms, promote_push_tos, PIECE_KNIGHT, moves+m);
    m += move_list_setwise(promote_push_froms, promote_push_tos, PIECE_BISHOP, moves+m);
    m += move_list_setwise(promote_push_froms, promote_push_tos, PIECE_QUEEN, moves+m);

    m += move_list_setwise(promote_east_froms, promote_east_tos, PIECE_ROOK, moves+m);
    m += move_list_setwise(promote_east_froms, promote_east_tos, PIECE_KNIGHT, moves+m);
    m += move_list_setwise(promote_east_froms, promote_east_tos, PIECE_BISHOP, moves+m);
    m += move_list_setwise(promote_east_froms, promote_east_tos, PIECE_QUEEN, moves+m);

    m += move_list_setwise(promote_west_froms, promote_west_tos, PIECE_ROOK, moves+m);
    m += move_list_setwise(promote_west_froms, promote_west_tos, PIECE_KNIGHT, moves+m);
    m += move_list_setwise(promote_west_froms, promote_west_tos, PIECE_BISHOP, moves+m);
    m += move_list_setwise(promote_west_froms, promote_west_tos, PIECE_QUEEN, moves+m);

    // rook moves
    while(rooks)
    {
        square from = bitboard_lsb(rooks);
        rooks = bitboard_reset(rooks, from);
        bitboard attacks = bitboard_rook_attacks(from, occupied) & attack_mask;
        m += move_list_piecewise(from, attacks, PIECE_NONE, moves+m);
    }

    // knight moves
    while(knights)
    {
        square from = bitboard_lsb(knights);
        knights = bitboard_reset(knights, from);
        bitboard attacks = bitboard_knight_attacks(from) & attack_mask;
        m += move_list_piecewise(from, attacks, PIECE_NONE, moves+m);
    }

    // bishop moves
    while(bishops)
    {
        square from = bitboard_lsb(bishops);
        bishops = bitboard_reset(bishops, from);
        bitboard attacks = bitboard_bishop_attacks(from, occupied) & attack_mask;
        m += move_list_piecewise(from, attacks, PIECE_NONE, moves+m);
    }

    // queen moves
    while(queens)
    {
        square from = bitboard_lsb(queens);
        queens = bitboard_reset(queens, from);
        bitboard attacks = (bitboard_rook_attacks(from, occupied) | bitboard_bishop_attacks(from, occupied)) & attack_mask;
        m += move_list_piecewise(from, attacks, PIECE_NONE, moves+m);
    }

    // king moves
    if(p->kingside_castle[player])
    {
        square from = bitboard_lsb(kings);
        square to = square_from_file_rank(FILE_G, square_rank(from));
        bitboard path = kings;
        path |= bitboard_shift(kings, DIRECTION_E);
        path |= bitboard_shift(path, DIRECTION_E);
        bitboard between = path & ~kings;

        if(!(between & board_occupied(b)) && !(path & board_attacks(b, opponent)))
        {
            move_init(&moves[m++], from, to, PIECE_NONE);
        }
    }
    if(p->queenside_castle[player])
    {
        square from = bitboard_lsb(kings);
        square to = square_from_file_rank(FILE_C, square_rank(from));
        bitboard path = kings;
        path |= bitboard_shift(path, DIRECTION_W);
        path |= bitboard_shift(path, DIRECTION_W);
        bitboard between = bitboard_shift(path, DIRECTION_W);

        if(!(between & board_occupied(b)) && !(path & board_attacks(b, opponent)))
        {
            move_init(&moves[m++], from, to, PIECE_NONE);
        }
    }
    while(kings)
    {
        square from = bitboard_lsb(kings);
        kings = bitboard_reset(kings, from);
        bitboard attacks = bitboard_king_attacks(from) & attack_mask;
        m += move_list_piecewise(from, attacks, PIECE_NONE, moves+m);
    }

    // remove illegal moves
    int n = 0;
    for(int i = 0; i < m; i++)
    {
        undo u;
        position_move(p, &moves[i], &u);
        if(!(board_attacks(b, opponent) & board_side_piece(b, player, PIECE_KING))) moves[n++] = moves[i];
        position_undo(p, &moves[i], &u);
    }

    return n;
}

void position_move(position *p, const move* m, undo* u)
{
    if(u != NULL)
    {
        u->capture = board_get(&p->board, m->to, NULL);
        u->en_passant = p->en_passant;
        u->kingside_castle[SIDE_WHITE] = p->kingside_castle[SIDE_WHITE];
        u->kingside_castle[SIDE_BLACK] = p->kingside_castle[SIDE_BLACK];
        u->queenside_castle[SIDE_WHITE] = p->queenside_castle[SIDE_WHITE];
        u->queenside_castle[SIDE_BLACK] = p->queenside_castle[SIDE_BLACK];
    }

    board* b = &p->board;
    side side;
    piece piece = board_get(b, m->from, &side);
    square ep = p->en_passant;

    board_set(b, m->from, SIDE_NONE, PIECE_NONE);
    if(m->promote != PIECE_NONE) board_set(b, m->to, side, m->promote);
    else board_set(b, m->to, side, piece);

    p->en_passant = SQUARE_NONE;
    if(ep != SQUARE_NONE) p->hash ^= en_passant_hash[square_file(ep)];

    if(piece == PIECE_PAWN)
    {
        if(square_rank(m->from) == rank_side(RANK_2, side) && square_rank(m->to) == rank_side(RANK_4, side))
        {
            // double push
            p->en_passant = square_from_file_rank(square_file(m->from), rank_side(RANK_3, side));
            p->hash ^= en_passant_hash[square_file(p->en_passant)];
        }
        else if(m->to == ep)
        {
            // en passant capture
            square ep_capture = square_from_file_rank(square_file(ep), rank_side(RANK_5, side));
            board_set(b, ep_capture, SIDE_NONE, PIECE_NONE);
        }
    }
    else if(piece == PIECE_KING)
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
        
        rank rank_first = rank_side(RANK_1, side);

        if(m->from == square_from_file_rank(FILE_E, rank_first))
        {
            if(m->to == square_from_file_rank(FILE_G, rank_first))
            {
                board_set(b, square_from_file_rank(FILE_H, rank_first), SIDE_NONE, PIECE_NONE);
                board_set(b, square_from_file_rank(FILE_F, rank_first), side, PIECE_ROOK);
            }
            else if(m->to == square_from_file_rank(FILE_C, rank_side(RANK_1, side)))
            {
                board_set(b, square_from_file_rank(FILE_A, rank_first), SIDE_NONE, PIECE_NONE);
                board_set(b, square_from_file_rank(FILE_D, rank_first), side, PIECE_ROOK);
            }
        }
    }

    // todo: these (and en passant) should only be updated if they changed from the previous move
    // can probably be done by xoring with current state before each move

    if(p->queenside_castle[SIDE_WHITE] && (m->from == SQUARE_A1 || m->to == SQUARE_A1))
    {
        p->queenside_castle[SIDE_WHITE] = false;
        p->hash ^= queenside_castle_hash[SIDE_WHITE];
    }
    if(p->kingside_castle[SIDE_WHITE] && (m->from == SQUARE_H1 || m->to == SQUARE_H1))
    {
        p->kingside_castle[SIDE_WHITE] = false;
        p->hash ^= kingside_castle_hash[SIDE_WHITE];
    }
    if(p->queenside_castle[SIDE_BLACK] && (m->from == SQUARE_A8 || m->to == SQUARE_A8))
    {
        p->queenside_castle[SIDE_BLACK] = false;
        p->hash ^= queenside_castle_hash[SIDE_BLACK];
    }
    if(p->kingside_castle[SIDE_BLACK] && (m->from == SQUARE_H8 || m->to == SQUARE_H8))
    {
        p->kingside_castle[SIDE_BLACK] = false;
        p->hash ^= kingside_castle_hash[SIDE_BLACK];
    }

    p->halfmove_clock++; // todo: only if silent
    p->fullmove_number += p->turn;
    p->turn = side_opposite(p->turn);
    p->hash ^= side_hash;
}

void position_undo(position *p, const move* m, const undo* u)
{
    board* b = &p->board;
    side side;
    piece piece = board_get(b, m->to, &side);

    board_set(b, m->from, side, piece);
    board_set(b, m->to, SIDE_NONE, PIECE_NONE);
    if(u->capture != PIECE_NONE) board_set(b, m->to, side_opposite(side), u->capture);
    if(m->promote != PIECE_NONE) board_set(b, m->from, side, PIECE_PAWN);

    if(p->en_passant != SQUARE_NONE) p->hash ^= en_passant_hash[square_file(p->en_passant)];
    if(u->en_passant != SQUARE_NONE) p->hash ^= en_passant_hash[square_file(u->en_passant)];
    p->en_passant = u->en_passant;

    if(p->kingside_castle[SIDE_WHITE] != u->kingside_castle[SIDE_WHITE])
    {
        p->kingside_castle[SIDE_WHITE] = u->kingside_castle[SIDE_WHITE];
        p->hash ^= kingside_castle_hash[SIDE_WHITE];
    }
    if(p->kingside_castle[SIDE_BLACK] != u->kingside_castle[SIDE_BLACK])
    {
        p->kingside_castle[SIDE_BLACK] = u->kingside_castle[SIDE_BLACK];
        p->hash ^= kingside_castle_hash[SIDE_BLACK];
    }
    if(p->queenside_castle[SIDE_WHITE] != u->queenside_castle[SIDE_WHITE])
    {
        p->queenside_castle[SIDE_WHITE] = u->queenside_castle[SIDE_WHITE];
        p->hash ^= queenside_castle_hash[SIDE_WHITE];
    }
    if(p->queenside_castle[SIDE_BLACK] != u->queenside_castle[SIDE_BLACK])
    {
        p->queenside_castle[SIDE_BLACK] = u->queenside_castle[SIDE_BLACK];
        p->hash ^= queenside_castle_hash[SIDE_BLACK];
    }

    if(piece == PIECE_PAWN)
    {
        if(m->to == u->en_passant)
        {
            square ep_capture = square_from_file_rank(square_file(u->en_passant), rank_side(RANK_5, side));
            board_set(b, ep_capture, side_opposite(side), PIECE_PAWN);
        }
    }
    else if(piece == PIECE_KING)
    {
        rank rank_first = rank_side(RANK_1, side);

        if(m->from == square_from_file_rank(FILE_E, rank_first))
        {
            if(m->to == square_from_file_rank(FILE_G, rank_first))
            {
                board_set(b, square_from_file_rank(FILE_H, rank_first), side, PIECE_ROOK);
                board_set(b, square_from_file_rank(FILE_F, rank_first), SIDE_NONE, PIECE_NONE);
            }
            else if(m->to == square_from_file_rank(FILE_C, rank_side(RANK_1, side)))
            {
                board_set(b, square_from_file_rank(FILE_A, rank_first), side, PIECE_ROOK);
                board_set(b, square_from_file_rank(FILE_D, rank_first), SIDE_NONE, PIECE_NONE);
            }
        }
    }

    p->halfmove_clock--; // todo: only if silent
    p->fullmove_number -= side_opposite(p->turn); // decrease if white
    p->turn = side_opposite(p->turn);
    p->hash ^= side_hash;
}


int position_fullmove(position* p)
{
    return p->fullmove_number;
}

int position_halfmove(position* p)
{
    return (p->fullmove_number - 1)*2 + p->turn;
}


uint64_t position_hash(const position* p)
{
    return p->hash ^ p->board.hash;
}

void position_copy(const position* src, position* dst)
{
    board_copy(&src->board, &dst->board);
    
    dst->turn = src->turn;
    dst->kingside_castle[SIDE_WHITE] = src->kingside_castle[SIDE_WHITE];
    dst->kingside_castle[SIDE_BLACK] = src->kingside_castle[SIDE_BLACK];
    dst->queenside_castle[SIDE_WHITE] = src->queenside_castle[SIDE_WHITE];
    dst->queenside_castle[SIDE_BLACK] = src->queenside_castle[SIDE_BLACK];
    dst->en_passant = src->en_passant;
    dst->halfmove_clock = src->halfmove_clock;
    dst->fullmove_number = src->fullmove_number;
    
    dst->hash = src->hash;
}

void position_print(const position* p)
{
    board_print(&p->board, false, true);
    printf("turn: %c\n", side_to_char(p->turn));
    printf("white kingside castle: %d\n", p->kingside_castle[SIDE_WHITE]);
    printf("white queenside castle: %d\n", p->queenside_castle[SIDE_WHITE]);
    printf("black kingside castle: %d\n", p->kingside_castle[SIDE_BLACK]);
    printf("black queenside castle: %d\n", p->queenside_castle[SIDE_BLACK]);
    printf("en passant target square: "); square_print(p->en_passant); putchar('\n');
    printf("halfmove clock: %d\n", p->halfmove_clock);
    printf("fullmove number: %d\n", p->fullmove_number);
}


}


#endif
