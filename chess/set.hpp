#ifndef CHESS_SET_HPP
#define CHESS_SET_HPP


#include <bit>
#include <cstdint>
#include <vector>

#include "direction.hpp"
#include "random.hpp"
#include "square.hpp"


namespace chess
{


/// Bitboard is a set of squares.
///
/// The terms bitboard and set of squares will be used interchangeably.
/// A bitboard is 64 bits. Square A1 is in the set if bit 0 is set and square
/// H8 is in the set if bit 64 is set.
using bitboard = std::uint64_t;

/// Empty set.
const bitboard empty_set = 0ULL;

/// Universal set.
const bitboard universal_set = ~0ULL;

/// Square set.
///
/// The set with only the given square in it.
///
/// \param sq The square.
/// \returns Bitboard with the bit at square index set.
constexpr bitboard square_set(square sq)
{
    return 1ULL << sq;
}

/// File set.
///
/// The set with all squares of given file in it.
///
/// \param f The file.
/// \returns Bitboard with all bits of the file set.
constexpr bitboard file_set(file f)
{
    return 0x0101010101010101ULL << f;
}

/// Rank set.
///
/// The set with all squares of given rank in it.
///
/// \param r The rank.
/// \returns Bitboard with all bits of the rank set.
constexpr bitboard rank_set(rank r)
{
    return 0xFFULL << (r*8);
}

/// Check if set contains square.
///
/// Returns whether the bit corresponding to the square is set to 1.
///
/// \param bb The set.
/// \param sq The square.
/// \returns Whether the square is in the set.
inline bool set_contains(bitboard bb, square sq)
{
    return bb & square_set(sq);
}

/// Insert square to set.
///
/// Returns the bit with the bit corresponding to the square set.
///
/// \param bb The set.
/// \param sq The square.
/// \returns The set with the square in it.
inline bitboard set_insert(bitboard bb, square sq)
{
    return bb | square_set(sq);
}

/// Erase square from set.
///
/// Returns bitboard with the bit corresponding to the square unset.
///
/// \param bb The set.
/// \param sq The square.
/// \returns The set without the square in it.
inline bitboard set_erase(bitboard bb, square sq)
{
    return bb & ~square_set(sq);
}


/// First square in set.
///
/// Returns the first square in the set, starting at A1.
/// In other words, the first square whose bit is set to 1.
///
/// \param bb The set.
/// \returns The first square.
inline square set_first(bitboard bb)
{
    return static_cast<square>(std::countr_zero(bb));
}

/// Last square in set.
///
/// Returns the last square in the set, starting at A1.
/// In other words, the last square whose bit is set to 1.
///
/// \param bb The set.
/// \returns The last square.
inline square set_last(bitboard bb)
{
    return static_cast<square>(square_h8 - std::countl_zero(bb));
}

/// Cardinality of set.
///
/// Returns the number of squares in the given set (the number of bits set).
///
/// \param bb The set.
/// \returns Squares in set.
inline int set_cardinality(bitboard bb)
{
    return std::popcount(bb);
}

/// Union of sets.
///
/// Returns the union of two sets. Equivalent to bitwise or.
///
/// \param a First set.
/// \param b Second set.
/// \returns Union of both sets.
inline bitboard set_union(bitboard a, bitboard b)
{
    return a | b;
}


/// Intersection of sets.
///
/// Returns the intersection of two sets. Equivalent to bitwise and.
///
/// \param a First set.
/// \param b Second set.
/// \returns Union of both sets.
inline bitboard set_intersection(bitboard a, bitboard b)
{
    return a & b;
}

/// Complement of set.
///
/// Returns the complement of two sets. Equivalent to bitwise negation.
///
/// \param bb The set.
/// \returns The complement of the set.
inline bitboard set_complement(bitboard bb)
{
    return ~bb;
}

/// Squares in set.
///
/// Returns list of all the squares in a set. Equivalent to all squares whose
/// corresponding bits are set.
///
/// \param bb The set.
/// \returns Vector of squares.
std::vector<square> set_elements(bitboard bb);


/// Directional shift of set.
///
/// Shifts bitboard in given direction. For example, the bitboard
/// where all squares in file A are set to one can be shifted east to obtain
/// a bitboard where all squares in file B are set.
///
/// \param bb The set.
/// \param d The direction.
/// \returns The set shifted in the direction.
bitboard set_shift(bitboard bb, direction d);


/// Ray cast of a set.
///
/// Given a bitboard, returns a bitboard with rays in the given direction from 
/// all set bits. The rays stop at bits set in an occupancy bitboard.
///
/// \param bb The set.
/// \param d The direction to cast ray in.
/// \param occupied Occupancy set for collisions.
/// \returns Set with rays.
bitboard set_ray(bitboard bb, direction d, bitboard occupied);


}

#endif