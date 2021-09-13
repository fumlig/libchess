#ifndef MOVE_H
#define MOVE_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "move.hpp"
#include "board.hpp"
#include "bitboard.hpp"


#define MOVE_UCI_LENGTH 6 // "d2d4", "a7a8q"


struct position;


struct move
{
    enum square from;
    enum square to;
    enum piece promote;
};

struct undo
{
    // irreversible information of a move
    enum piece capture;
    enum square en_passant;
    bool kingside_castle[SIDES];
    bool queenside_castle[SIDES];
};


void move_init(struct move* m, enum square from, enum square to, enum piece promote);


void move_from_uci(struct move* m, const char* uci);
void move_to_uci(const struct move* m, char* uci);

int move_list_piecewise(enum square from, bitboard tos, enum piece promote, struct move* moves);
int move_list_setwise(bitboard froms, bitboard tos, enum piece promote, struct move* moves);


typedef int (*move_cmp)(const struct move*, const struct move*, const struct position*);

int move_cmp_uci(const struct move* m1, const struct move* m2, const struct position*);
int move_cmp_nop(const struct move* m1, const struct move* m2, const struct position* p);

void move_list_sort(struct move* moves, int m, const struct position* p, move_cmp cmp);



void move_init(struct move* m, enum square from, enum square to, enum piece promote)
{
    m->from = from;
    m->to = to;
    m->promote = promote;
}


void move_from_uci(struct move* m, const char* uci)
{
    enum square from = square_from_string(uci);
    enum square to = square_from_string(uci+2);
    enum piece promote = PIECE_NONE;
    if(uci[4] != '\0') promote = piece_from_san(uci[4], NULL);
    move_init(m, from, to, promote);
}

void move_to_uci(const struct move* m, char* uci)
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



int move_list_piecewise(enum square from, bitboard tos, enum piece promote, struct move* moves)
{
    int m = 0;
    while(tos)
    {
        enum square to = bitboard_lsb(tos);
        tos = bitboard_reset(tos, to);
        move_init(&moves[m++], from, to, promote);
    }
    return m;
}

int move_list_setwise(bitboard froms, bitboard tos, enum piece promote, struct move* moves)
{
    int m = 0;
    while(froms && tos)
    {
        enum square from = bitboard_lsb(froms);
        enum square to = bitboard_lsb(tos);
        froms = bitboard_reset(froms, from);
        tos = bitboard_reset(tos, to);
        move_init(&moves[m++], from, to, promote);
    }
    return m;
}



int move_cmp_nop(const struct move* m1, const struct move* m2, const struct position* p)
{
    return 0;
}


int move_list_partition(struct move* moves, const struct position* p, move_cmp cmp, int lo, int hi)
{
    struct move* pivot = &moves[hi];
    int i = lo;
    for(int j = lo; j <= hi; j++)
    {
        if(cmp(&moves[j], pivot, p) < 0)
        {
            struct move tmp = moves[i];
            moves[i] = moves[j];
            moves[j] = tmp;
            i++;
        }
    }

    struct move tmp = moves[i];
    moves[i] = moves[hi];
    moves[hi] = tmp;

    return i;
}

void move_list_quicksort(struct move* moves, const struct position* p, move_cmp cmp, int lo, int hi)
{
    if(lo >= hi) return;

    int pi = move_list_partition(moves, p, cmp, lo, hi);
    move_list_quicksort(moves, p, cmp, lo, pi-1);
    move_list_quicksort(moves, p, cmp, pi+1, hi);
}

void move_list_sort(struct move* moves, int m, const struct position* p, move_cmp cmp)
{
    //qsort_r(moves, m, sizeof(struct move), (int (*)(const void*, const void*, void*))cmp, p);
    //qsort(moves, m, sizeof(struct move), (int (*)(const void*, const void*))cmp);
    move_list_quicksort(moves, p, cmp, 0, m-1);
}


void move_print_uci(const struct move* m)
{
    square_print(m->from);
    square_print(m->to);
    if(m->promote != PIECE_NONE) putchar(piece_to_san(m->promote, SIDE_BLACK)); // lowercase
}



#endif
