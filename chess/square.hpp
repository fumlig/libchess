#ifndef CHESS_SQUARE_HPP
#define CHESS_SQUARE_HPP


#include <string>
#include <stdexcept>

#include "side.hpp"


namespace chess
{


/// Files of a chess board.
enum file
{
    file_a, file_b, file_c, file_d, file_e, file_f, file_g, file_h
};

/// Number of files.
const int files = 8;

/// Ranks of a chess board.
enum rank
{
    rank_1, rank_2, rank_3, rank_4, rank_5, rank_6, rank_7, rank_8
};

/// Number of ranks.
const int ranks = 8;

/// Squares on a chess board.
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

/// Number of squares.
const int squares = 64;


/// Rank number for a given side.
///
/// For white, the rank will be the same. For black rank 8 is referred to as
/// rank 1, rank 7 as rank 2 and so on.
///
/// \param s Side to get rank number for.
/// \param r Rank to get for side.
/// \returns The rank for the given side.
constexpr rank side_rank(side s, rank r)
{
    return static_cast<rank>(r*static_cast<int>(opponent(s)) + (rank_8-r)*s);
}

/// File of given square.
///
/// As an example, the file of square A1 is A.
///
/// \param sq Square to get file of.
/// \returns File of the square.
constexpr file file_of(square sq)
{
    return static_cast<file>(sq % 8);
}

/// Rank of given square.
///
/// As an example, the rank of square A1 is 1.
///
/// \param sq Square to get rank of.
/// \returns Rank of the square.
constexpr rank rank_of(square sq)
{
    return static_cast<rank>(sq / 8);
}


constexpr side color_of(square sq)
{
    return static_cast<int>(sq) % 2 == 1 ? side_white : side_black;
}

/// Concatenate coordinates into square.
///
/// Given a file and a rank, return the corresponding square.
///
/// \param f File of square.
/// \param r Rank of square.
/// \returns Square with given coordinates.
constexpr square cat_coords(file f, rank r)
{
    return static_cast<square>(r*8 + f);
}

/// File from Short Algebraic Notation (SAN).
///
/// Converts a lowercase char to its corresponding file ('a' is file A).
///
/// \param san SAN of file.
/// \returns File with given SAN.
/// \throws Invalid argument if SAN does not denote a file.
file file_from_san(char san);


/// Rank from Short Algebraic Notation (SAN).
///
/// Converts a digit char to its corresponding rank ('1' is rank 1).
///
/// \param san SAN of rank.
/// \returns Rank with given SAN.
/// \throws Invalid argument if SAN does not denote a rank.
rank rank_from_san(char san);


/// Square from Short Algebraic Notation (SAN).
///
/// Converts the first char to a file and the second to a rank ("a1" is square A1).
///
/// \param san SAN of rank.
/// \returns Rank with given SAN.
/// \throws Invalid argument if SAN does not denote a rank.
square square_from_san(std::string_view san);


/// File to Short Algebraic Notation (SAN).
///
/// Converts the given file to a lowercase char (file A is 'a').
///
/// \param f File to get SAN for.
/// \returns SAN for file.
char file_to_san(file f);


/// Rank to Short Algebraic Notation (SAN).
///
/// Converts the given rank to a lowercase char (rank 1 is '1').
///
/// \param r Rank to get SAN for.
/// \returns SAN for rank.
char rank_to_san(rank r);


/// Square tp Short Algebraic Notation (SAN).
///
/// Converts the file and rank of the square to chars and concatenates them 
/// (square A1 is "a1").
///
/// \param sq Square to convert.
/// \returns SAN of square.
std::string square_to_san(square sq);


square flip(square sq);


}


#endif