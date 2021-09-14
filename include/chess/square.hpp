#ifndef CHESS_SQUARE_HPP
#define CHESS_SQUARE_HPP


#include <iostream>
#include <string>

#include "piece.hpp"


namespace chess
{


#define SQUARES 64
#define RANKS   8
#define FILES   8

enum file
{
    FILE_A = 0,
    FILE_B = 1,
    FILE_C = 2,
    FILE_D = 3,
    FILE_E = 4,
    FILE_F = 5,
    FILE_G = 6,
    FILE_H = 7
};

file file_from_char(char c);
char file_to_char(file f);

enum rank
{
    RANK_1 = 0,
    RANK_2 = 1,
    RANK_3 = 2,
    RANK_4 = 3,
    RANK_5 = 4,
    RANK_6 = 5,
    RANK_7 = 6,
    RANK_8 = 7,
};

rank rank_side(rank r, side s);

rank rank_from_char(char c);
char rank_to_char(rank f);


enum square
{
    SQUARE_NONE = -1,
    SQUARE_A1 = 0,
    SQUARE_B1 = 1,
    SQUARE_C1 = 2,
    SQUARE_D1 = 3,
    SQUARE_E1 = 4,
    SQUARE_F1 = 5,
    SQUARE_G1 = 6,
    SQUARE_H1 = 7,
    SQUARE_A2 = 8,
    SQUARE_B2 = 9,
    SQUARE_C2 = 10,
    SQUARE_D2 = 11,
    SQUARE_E2 = 12,
    SQUARE_F2 = 13,
    SQUARE_G2 = 14,
    SQUARE_H2 = 15,
    SQUARE_A3 = 16,
    SQUARE_B3 = 17,
    SQUARE_C3 = 18,
    SQUARE_D3 = 19,
    SQUARE_E3 = 20,
    SQUARE_F3 = 21,
    SQUARE_G3 = 22,
    SQUARE_H3 = 23,
    SQUARE_A4 = 24,
    SQUARE_B4 = 25,
    SQUARE_C4 = 26,
    SQUARE_D4 = 27,
    SQUARE_E4 = 28,
    SQUARE_F4 = 29,
    SQUARE_G4 = 30,
    SQUARE_H4 = 31,
    SQUARE_A5 = 32,
    SQUARE_B5 = 33,
    SQUARE_C5 = 34,
    SQUARE_D5 = 35,
    SQUARE_E5 = 36,
    SQUARE_F5 = 37,
    SQUARE_G5 = 38,
    SQUARE_H5 = 39,
    SQUARE_A6 = 40,
    SQUARE_B6 = 41,
    SQUARE_C6 = 42,
    SQUARE_D6 = 43,
    SQUARE_E6 = 44,
    SQUARE_F6 = 45,
    SQUARE_G6 = 46,
    SQUARE_H6 = 47,
    SQUARE_A7 = 48,
    SQUARE_B7 = 49,
    SQUARE_C7 = 50,
    SQUARE_D7 = 51,
    SQUARE_E7 = 52,
    SQUARE_F7 = 53,
    SQUARE_G7 = 54,
    SQUARE_H7 = 55,
    SQUARE_A8 = 56,
    SQUARE_B8 = 57,
    SQUARE_C8 = 58,
    SQUARE_D8 = 59,
    SQUARE_E8 = 60,
    SQUARE_F8 = 61,
    SQUARE_G8 = 62,
    SQUARE_H8 = 63
};


file file_from_char(char c)
{
    return static_cast<file>(c - 'a');
}

char file_to_char(file f)
{
    return 'a' + f;
}


rank rank_side(rank r, side s)
{
    return static_cast<rank>(r*side_opposite(s) + (RANK_8-r)*s);
}

rank rank_from_char(char c)
{
    return static_cast<rank>(c - '1');
}

char rank_to_char(rank r)
{
    return '1' + r;
}

file square_file(square sq)
{
    return static_cast<file>(sq % 8);
}

rank square_rank(square sq)
{
    return static_cast<rank>(sq / 8);
}

square square_from_file_rank(file f, rank r)
{
    return static_cast<square>(r*8 + f);
}

square square_from_string(std::string_view string)
{
    file f = file_from_char(string[0]);
    rank r = rank_from_char(string[1]);
    return square_from_file_rank(f, r);
}

}


#endif