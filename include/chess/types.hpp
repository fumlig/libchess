#ifndef CHESS_TYPES_HPP
#define CHESS_TYPES_HPP


#include <array>


namespace chess
{


const int sides = 2;

enum side
{
    side_none   = -1,
    side_white  = 0,
    side_black  = 1,
};


side side_opposite(side s)
{
    return static_cast<side>(!s);
}


const int pieces = 6;

enum piece
{
    piece_none      = -1,
    piece_pawn      = 0,
    piece_rook      = 1,
    piece_knight    = 2,
    piece_bishop    = 3,
    piece_queen     = 4,
    piece_king      = 5,
};


const int files = 8;

enum file
{
    file_a, file_b, file_c, file_d, file_e, file_f, file_g, file_h
};


const int ranks = 8;

enum rank
{
    rank_1, rank_2, rank_3, rank_4, rank_5, rank_6, rank_7, rank_8
};


const int squares = 64;

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



rank rank_side(rank r, side s)
{
    return static_cast<rank>(r*static_cast<int>(side_opposite(s)) + (rank_8-r)*s);
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
    std::array<bool, sides> kingside_castle;
    std::array<bool, sides> queenside_castle;
};


const int directions = 16;

enum direction
{
    direction_n     = 8,
    direction_e     = 1,
    direction_s     = -8,
    direction_w     = -1,
    
	direction_ne    = direction_n + direction_e,
    direction_se    = direction_s + direction_e,
    direction_sw    = direction_s + direction_w,
    direction_nw    = direction_n + direction_w,
    
	direction_nne   = direction_n + direction_n + direction_e,
    direction_ene   = direction_e + direction_n + direction_e,
    direction_ese   = direction_e + direction_s + direction_e,
    direction_sse   = direction_s + direction_s + direction_e,
    direction_ssw   = direction_s + direction_s + direction_w,
    direction_wsw   = direction_w + direction_s + direction_w,
    direction_wnw   = direction_w + direction_n + direction_w,
    direction_nnw   = direction_n + direction_n + direction_w,

    direction_none  = 0,
};

direction direction_opposite(direction d)
{
    return static_cast<direction>((d + 8) % 16);
}

direction direction_forward(side s)
{
    switch(s)
    {
    case side_white:
        return direction_n;
    case side_black:
        return direction_s;
    case side_none:
    default:
        return direction_none;
    }
}


}



#endif