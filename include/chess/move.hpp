#ifndef CHESS_MOVE_HPP
#define CHESS_MOVE_HPP


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "move.hpp"
#include "board.hpp"
#include "bitboard.hpp"


namespace chess
{


#define MOVE_UCI_LENGTH 6 // "d2d4", "a7a8q"


struct position;


struct move
{
    square from;
    square to;
    piece promote;
};

struct undo
{
    // irreversible information of a move
    piece capture;
    square en_passant;
    bool kingside_castle[SIDES];
    bool queenside_castle[SIDES];
};


void move_init(move* m, square from, square to, piece promote);


void move_from_uci(move* m, const char* uci);
void move_to_uci(const move* m, char* uci);

int move_list_piecewise(square from, bitboard tos, piece promote, move* moves);
int move_list_setwise(bitboard froms, bitboard tos, piece promote, move* moves);


typedef int (*move_cmp)(const move*, const move*, const position*);

int move_cmp_uci(const move* m1, const move* m2, const position*);
int move_cmp_nop(const move* m1, const move* m2, const position* p);

void move_list_sort(move* moves, int m, const position* p, move_cmp cmp);



void move_init(move* m, square from, square to, piece promote)
{
    m->from = from;
    m->to = to;
    m->promote = promote;
}


void move_from_uci(move* m, const char* uci)
{
    square from = square_from_string(uci);
    square to = square_from_string(uci+2);
    piece promote = PIECE_NONE;
    if(uci[4] != '\0') promote = piece_from_san(uci[4], NULL);
    move_init(m, from, to, promote);
}

void move_to_uci(const move* m, char* uci)
{
    square_to_string(m->from, uci);
    square_to_string(m->to, uci+2);
    if(m->promote != PIECE_NONE)
    {
        uci[4] = piece_to_san(m->promote, SIDE_BLACK); // lowercase
        uci[5] = '\0';
    }
    else uci[4] = '\0';
}



int move_list_piecewise(square from, bitboard tos, piece promote, move* moves)
{
    int m = 0;
    while(tos)
    {
        square to = bitboard_lsb(tos);
        tos = bitboard_reset(tos, to);
        move_init(&moves[m++], from, to, promote);
    }
    return m;
}

int move_list_setwise(bitboard froms, bitboard tos, piece promote, move* moves)
{
    int m = 0;
    while(froms && tos)
    {
        square from = bitboard_lsb(froms);
        square to = bitboard_lsb(tos);
        froms = bitboard_reset(froms, from);
        tos = bitboard_reset(tos, to);
        move_init(&moves[m++], from, to, promote);
    }
    return m;
}



int move_cmp_nop(const move* m1, const move* m2, const position* p)
{
    return 0;
}


int move_list_partition(move* moves, const position* p, move_cmp cmp, int lo, int hi)
{
    move* pivot = &moves[hi];
    int i = lo;
    for(int j = lo; j <= hi; j++)
    {
        if(cmp(&moves[j], pivot, p) < 0)
        {
            move tmp = moves[i];
            moves[i] = moves[j];
            moves[j] = tmp;
            i++;
        }
    }

    move tmp = moves[i];
    moves[i] = moves[hi];
    moves[hi] = tmp;

    return i;
}

void move_list_quicksort(move* moves, const position* p, move_cmp cmp, int lo, int hi)
{
    if(lo >= hi) return;

    int pi = move_list_partition(moves, p, cmp, lo, hi);
    move_list_quicksort(moves, p, cmp, lo, pi-1);
    move_list_quicksort(moves, p, cmp, pi+1, hi);
}

void move_list_sort(move* moves, int m, const position* p, move_cmp cmp)
{
    //qsort_r(moves, m, sizeof(move), (int (*)(const void*, const void*, void*))cmp, p);
    //qsort(moves, m, sizeof(move), (int (*)(const void*, const void*))cmp);
    move_list_quicksort(moves, p, cmp, 0, m-1);
}


void move_print_uci(const move* m)
{
    square_print(m->from);
    square_print(m->to);
    if(m->promote != PIECE_NONE) putchar(piece_to_san(m->promote, SIDE_BLACK)); // lowercase
}


}


#endif
