#ifndef CHESS_square_HPP
#define CHESS_square_HPP


#include <iostream>
#include <string>

#include "piece.hpp"


namespace chess
{


const int squares = 64;
const int ranks = 8;
const int files = 8;

enum file
{
    file_a, file_b, file_c, file_d, file_e, file_f, file_g, file_h
};


file file_from_char(char c);
char file_to_char(file f);


enum rank
{
    rank_1, rank_2, rank_3, rank_4, rank_5, rank_6, rank_7, rank_8
};


rank rank_side(rank r, side s);

rank rank_from_char(char c);
char rank_to_char(rank f);


enum square
{
    square_a1, square_b1, square_c1, square_d1, square_e1, square_f1, square_g1, square_h1,
    square_a2, square_b2, square_c2, square_d2, square_e2, square_f2, square_g2, square_h2, 
    square_a3, square_b3, square_c3, square_d3, square_e3, square_f3, square_g3, square_h3,
    square_a4, square_b4, square_c4, square_d4, square_e4, square_f4, square_g4, square_h4,
    square_a5, square_b5, square_c5, square_d5, square_e5, square_f5, square_g5, square_h5,
    square_a6, square_b6, square_c6, square_d6, square_e6, square_f6, square_g6, square_h6,
    square_a7, square_b7, square_c7, square_d7, square_e7, square_f7, square_g7, square_h7,
    square_a8, square_b8, square_c8, square_d8, square_e8, square_f8, square_g8, square_h8,
    square_none = -1,
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
    return static_cast<rank>(r*side_opposite(s) + (rank_8-r)*s);
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
