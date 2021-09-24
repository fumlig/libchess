#ifndef CHESS_HPP
#define CHESS_HPP


#include <array>
#include <bit>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>


namespace chess
{


/// Sides in chess.
///
/// In some places, a none-side is useful (for example for empty board squares).
enum side
{
    side_white,
    side_black,
    side_none = -1,
};

/// Number of sides.
const int sides = 2;

/// Opponent of given side.
///
/// The opponent of white is black and vice versa.
///
/// \param s Side to get opponent of.
/// \returns Opponent of side.
constexpr side opponent(side s)
{
    return static_cast<side>(!s);
}


/// Pieces in chess.
///
/// In some places, a none-piece is useful (for example for empty board squares).
enum piece
{
    piece_pawn,
    piece_rook,
    piece_knight,
    piece_bishop,
    piece_queen,
    piece_king,
    piece_none = -1,
};

/// Number of pieces.
const int pieces = 6;

/// Piece and side from Short Algebraic Notation (SAN).
///
/// Pawn is 'P', rook is 'R', knight is 'N', bishop is 'B', queen is 'Q'
/// and king is 'K'. An uppercase letter indicates that it is a white piece 
/// and a lowercase indicates letter that it is a black piece.
///
/// \param san SAN of for piece.
/// \returns Side and piece type
/// \throws Invalid argument if SAN does not denote a piece.
std::pair<side, piece> piece_from_san(char san)
{
	side s = side_white;
	piece p = piece_none;

	if(std::islower(san))
	{
		s = side_black;
		san = std::toupper(san);
	}

    switch(san)
    {
    case 'P':
		p = piece_pawn;
		break;
    case 'R':
        p = piece_rook;
		break;
    case 'N':
        p = piece_knight;
		break;
    case 'B':
        p = piece_bishop;
		break;
    case 'Q':
        p = piece_queen;
		break;
    case 'K':
        p = piece_king;
		break;
    default:
		throw std::invalid_argument("san does not denote a piece");
    }

	return {s, p};
}

/// Piece and side to Short Algebraic Notation (SAN).
///
/// Pawn is 'P', rook is 'R', knight is 'N', bishop is 'B', queen is 'Q'
/// and king is 'K'. An uppercase letter indicates that it is a white piece 
/// and a lowercase indicates letter that it is a black piece.
///
/// \param s The side of the piece.
/// \param p The type of the piece.
/// \returns SAN for piece of given side.
char piece_to_san(side s, piece p)
{
	char c;

	switch(p)
	{
	case piece_pawn:
		c = 'P';
		break;
	case piece_rook:
		c = 'R';
		break;
	case piece_knight:
		c = 'N';
		break;
	case piece_bishop:
		c = 'B';
		break;
	case piece_queen:
		c = 'Q';
		break;
	case piece_king:
		c = 'K';
		break;
	case piece_none:
	default:
        c = ' ';
        break;
	}

	if(s == side_black && p != piece_none)
	{
		c = std::tolower(c);
	}

	return c;
}


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
file file_from_san(char san)
{
	if(san < 'a' || san > 'h')
	{
		throw std::invalid_argument("san does not denote a file");
	}

	return static_cast<file>(san - 'a');
}

/// Rank from Short Algebraic Notation (SAN).
///
/// Converts a digit char to its corresponding rank ('1' is rank 1).
///
/// \param san SAN of rank.
/// \returns Rank with given SAN.
/// \throws Invalid argument if SAN does not denote a rank.
rank rank_from_san(char san)
{
	if(san < '1' || san > '8')
	{
		throw std::invalid_argument("san does not denote a file");
	}

	return static_cast<rank>(san - '1');
}

/// Square from Short Algebraic Notation (SAN).
///
/// Converts the first char to a file and the second to a rank ("a1" is square A1).
///
/// \param san SAN of rank.
/// \returns Rank with given SAN.
/// \throws Invalid argument if SAN does not denote a rank.
square square_from_san(std::string_view san)
{
	if(san == "-")
	{
		return square_none;
	}

	if(san.length() != 2)
	{
		throw std::invalid_argument("san for square is of wrong length");
	}

	return cat_coords(file_from_san(san[0]), rank_from_san(san[1]));
}

/// File to Short Algebraic Notation (SAN).
///
/// Converts the given file to a lowercase char (file A is 'a').
///
/// \param f File to get SAN for.
/// \returns SAN for file.
char file_to_san(file f)
{
	return static_cast<file>('a' + f);
}

/// Rank to Short Algebraic Notation (SAN).
///
/// Converts the given rank to a lowercase char (rank 1 is '1').
///
/// \param r Rank to get SAN for.
/// \returns SAN for rank.
char rank_to_san(rank r)
{
	return static_cast<rank>('1' + r);
}

/// Square tp Short Algebraic Notation (SAN).
///
/// Converts the file and rank of the square to chars and concatenates them 
/// (square A1 is "a1").
///
/// \param sq Square to convert.
/// \returns SAN of square.
std::string square_to_san(square sq)
{
	if(sq == square_none)
	{
		return "-";
	}

	return {file_to_san(file_of(sq)), rank_to_san(rank_of(sq))};
}

/// Directions of piece movement.
///
/// Kings move north, east, south and west. Knights move north-northeast,
/// east-northeast, etc.
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

/// Number of directions.
const int directions = 16;

/// Opposite of given direction.
///
/// For example, the oppsoite of north is south.
///
/// \param d Direction to get opposite of.
/// \returns Opposite direction. 
constexpr direction opposite(direction d)
{
    return static_cast<direction>(-d);
}

/// Forward direction for given side.
///
/// This assumes the board is seen from white's perspective.
///
/// \param s The side.
/// \returns Forwards for given side.
constexpr direction forwards(side s)
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
inline std::vector<square> set_elements(bitboard bb)
{
    std::vector<square> elements;

    while(bb)
    {
        square sq = set_first(bb);
        elements.push_back(sq);
        bb = set_erase(bb, sq);
    }

    return elements;
}

/// Directional shift of set.
///
/// Shifts bitboard in given direction. For example, the bitboard
/// where all squares in file A are set to one can be shifted east to obtain
/// a bitboard where all squares in file B are set.
///
/// \param bb The set.
/// \param d The direction.
/// \returns The set shifted in the direction.
inline bitboard set_shift(bitboard bb, direction d)
{
    if(d > 0)
    {
        bb <<= d;
    }
    else
    {
        bb >>= -d;
    }

    bitboard trim = empty_set;

    switch(d)
    {
    case direction_e:
    case direction_ne:
    case direction_se:
    case direction_nne:
    case direction_sse:
        trim = file_set(file_a);
        break;
    case direction_w:
    case direction_nw:
    case direction_sw:
    case direction_nnw:
    case direction_ssw:
        trim = file_set(file_h);
        break;
    case direction_ene:
    case direction_ese:
        trim = file_set(file_a) | file_set(file_b);
        break;
    case direction_wnw:
    case direction_wsw:
        trim = file_set(file_g) | file_set(file_h);
        break;
    default:
        break;
    }

    return bb & ~trim;
}

/// Ray cast of a set.
///
/// Given a bitboard, returns a bitboard with rays in the given direction from 
/// all set bits. The rays stop at bits set in an occupancy bitboard.
///
/// \param bb The set.
/// \param d The direction to cast ray in.
/// \param occupied Occupancy set for collisions.
/// \returns Set with rays.
inline bitboard set_ray(bitboard bb, direction d, bitboard occupied)
{
    bitboard shift = bb;
    bitboard ray = 0;
    while(shift != 0 && !(shift & occupied))
    {
        shift = set_shift(shift, d);
        ray |= shift;
    }
    return ray;
}


// Magic table entry.
//
// Internal struct.
// This is used for fast generation of sliding piece attacks.
struct magic
{
    bitboard mask;
    bitboard magic;
    bitboard* attacks;
    unsigned shift;
};

// Magic index.
//
// Internal function.
// Index of attacks in a pregenerated attack table for certain occupancy masks.
inline unsigned magic_index(const magic& m, bitboard occupied)
{
    return ((occupied & m.mask) * m.magic) >> m.shift;
}


// Pseudorandom number generation given a seed.
//
// Internal function.
// xorshift64star method. Fast and good for Zobrist hashing and other things. 
inline std::size_t random_generate(std::size_t& seed)
{
    seed ^= seed >> 12;
    seed ^= seed << 25;
    seed ^= seed >> 27;

    return seed * 2685821657736338717ULL;
}

// Sparse pseudorandom number given a seed.
//
// Internal function.
// Achieved by taking bitwise and of multiple random numbers.
inline std::size_t random_sparse(std::size_t& seed)
{
    return random_generate(seed) & random_generate(seed) & random_generate(seed);
}

// Zobrist hash keys for different board and position properties.
//
// Internal tables.
// Used for incrementally updating hash of positions and boards.
// Filled during initialization.
inline std::array<std::array<std::array<std::size_t, pieces>, sides>, squares> pieces_zobrist_hash;
inline std::size_t side_zobrist_hash;
inline std::array<std::size_t, sides> kingside_castle_zobrist_hash;
inline std::array<std::size_t, sides> queenside_castle_zobrist_hash;
inline std::array<std::size_t, files> en_passant_zobrist_hash;

// Attack & magic tables.
//
// Internal tables filled during initialization.
// Used for efficient attack-set lookups.
inline std::array<magic, squares> rook_magics;
inline std::array<magic, squares> bishop_magics;
inline std::array<bitboard, 0x19000> rook_attacks;
inline std::array<bitboard, squares> knight_attacks;
inline std::array<bitboard, 0x1480> bishop_attacks;
inline std::array<bitboard, squares> king_attacks;

/// Set of all east pawn attacks.
///
/// Given a set of pawns of a given side, returns the set of squares attacked 
/// in the east direction by those pawns.
///
/// \param bb Set of pawns.
/// \param s Side of pawns.
/// \returns Attacked squares.
inline bitboard pawn_east_attack_set(bitboard bb, side s)
{
    return set_shift(bb, static_cast<direction>(forwards(s) + direction_e));
}

/// Set of all west pawn attacks.
///
/// Given a set of pawns of a given side, returns the set of squares attacked 
/// in the west direction by those pawns.
///
/// \param bb Set of pawns.
/// \param s Side of pawns.
/// \returns Attacked squares.
inline bitboard pawn_west_attack_mask(bitboard bb, side s)
{
    return set_shift(bb, static_cast<direction>(forwards(s) + direction_w));
}

/// Set of all rook attacks.
///
/// Given a rook position and occupied squares, returns the set of squares 
/// attacked by the rook.
///
/// \param sq Position of rook.
/// \param occupied Set of occupied squares.
/// \returns Attacked squares.
inline bitboard rook_attack_mask(square sq, bitboard occupied)
{
    unsigned index = magic_index(rook_magics[sq], occupied);
    return rook_magics[sq].attacks[index];
}

/// Set of all knight attacks.
///
/// Given a knight, returns the set of squares attacked by the knight.
///
/// \param sq Position of knight.
/// \returns Attacked squares.
inline bitboard knight_attack_mask(square sq)
{
    return knight_attacks[sq];
}

/// Set of all bishop attacks.
///
/// Given a bishop position and occupied squares, returns the set of squares 
/// attacked by the bishop.
///
/// \param sq Position of bishop.
/// \param occupied Set of occupied squares.
/// \returns Attacked squares.
inline bitboard bishop_attack_mask(square sq, bitboard occupied)
{
    unsigned index = magic_index(bishop_magics[sq], occupied);
    return bishop_magics[sq].attacks[index];
}

/// Set of all queen attacks.
///
/// Given a queen position and occupied squares, returns the set of squares 
/// attacked by the queen.
///
/// \param sq Position of queen.
/// \param occupied Set of occupied squares.
/// \returns Attacked squares.
inline bitboard queen_attack_mask(square sq, bitboard occupied)
{
    return rook_attack_mask(sq, occupied) | bishop_attack_mask(sq, occupied);
}


/// Set of all king attacks.
///
/// Given the position of a king, returns the set of squares attacked by the king.
///
/// \param sq Position of king.
/// \returns Attacked squares.
inline bitboard king_attack_mask(square sq)
{
    return king_attacks[sq];
}


// Initialize shift table.
// 
// Internal function.
// Initializes a table of shift attacks.
// Used for knight and king attacks.
void shift_table_init(bitboard* attacks, const std::array<direction, 8>& directions)
{
    for(int i = square_a1; i <= square_h8; i++)
    {
        square sq = static_cast<square>(i);

        bitboard sq_bb = square_set(sq);

        // initialize knight attacks table
        attacks[sq] = set_shift(sq_bb, directions[0])
                    | set_shift(sq_bb, directions[1])
                    | set_shift(sq_bb, directions[2])
                    | set_shift(sq_bb, directions[3])
                    | set_shift(sq_bb, directions[4])
                    | set_shift(sq_bb, directions[5])
                    | set_shift(sq_bb, directions[6])
                    | set_shift(sq_bb, directions[7]);
    }
}

// Initialize ray table.
//
// Internal function.
// Initializes a table of ray attacks using magic bitboards.
// Used for rook, bishop and queen attacks.
void ray_table_init(bitboard* attacks, std::array<magic, squares>& magics, const std::array<direction, 4>& directions, std::size_t& seed)
{
    bitboard occupancy[4096];
    bitboard reference[4096];
    int epoch[4096] = {0};
    int count = 0;
    int size = 0;

    for(int i = square_a1; i <= square_h8; i++)
    {
        square sq = static_cast<square>(i);

        bitboard sq_bb = square_set(sq);
        bitboard edges = ((rank_set(rank_1) | rank_set(rank_8)) & ~rank_set(rank_of(sq)))
                       | ((file_set(file_a) | file_set(file_h)) & ~file_set(file_of(sq)));

        magics[sq].mask = set_ray(sq_bb, directions[0], empty_set)
                        | set_ray(sq_bb, directions[1], empty_set)
                        | set_ray(sq_bb, directions[2], empty_set)
                        | set_ray(sq_bb, directions[3], empty_set);
        magics[sq].mask &= ~edges;

        magics[sq].shift = squares - set_cardinality(magics[sq].mask);

        // attack table of this square continues from the end of the last table
        magics[sq].attacks = sq == square_a1 ? attacks : magics[sq - 1].attacks + size;

        bitboard bb = 0;
        size = 0;

        // store all relevant occupancies and their corresponding attack table entry
        do
        {
            // bitboard never becomes zero...
            occupancy[size] = bb;
            // actual attacks
            reference[size] = set_ray(sq_bb, directions[0], bb)
                            | set_ray(sq_bb, directions[1], bb)
                            | set_ray(sq_bb, directions[2], bb)
                            | set_ray(sq_bb, directions[3], bb);

            size++;
            bb = (bb - magics[sq].mask) & magics[sq].mask;
        } while (bb);
        
        // initialize pseudo-random number generator
        //int seed = magic_seeds[square_rank(sq)];
        //random r;
        //random_init(&r, seed);

        // randomize until magic number that works is found
        int j = 0;
        while(j < size)
        {
            magics[sq].magic = 0;

            // randomize magic number
            while(set_cardinality((magics[sq].magic * magics[sq].mask) >> 56) < 6) magics[sq].magic = random_sparse(seed);

            // make sure magic number maps every possible occupancy to index
            // that looks up the correct sliding attack in attacks database

            // attacks database is built up alongside verifying the magic number
            // use epoch to avoid resetting the attack table on failure
            for (++count, j = 0; j < size; ++j)
            {
                unsigned index = magic_index(magics[sq], occupancy[j]);
                if (epoch[index] < count)
                {
                    epoch[index] = count;
                    magics[sq].attacks[index] = reference[j];
                }
                else if (magics[sq].attacks[index] != reference[j]) break;
            }
        }
    }
}


/// Chess board.
///
/// Data structure that holds placement of pieces with efficient retrieval
/// of certain sets, such as which squares are occupied by which side.
/// In most cases, a chess position should be used instead since it holds more
/// information.
class board
{
public:
    /// Default board.
    ///
    /// Construct board with pieces in their initial positions.
    board():
    board
    (
        {
            {square_a8, {side_black, piece_rook}},
            {square_b8, {side_black, piece_knight}},
            {square_c8, {side_black, piece_bishop}},
            {square_d8, {side_black, piece_queen}},
            {square_e8, {side_black, piece_king}},
            {square_f8, {side_black, piece_bishop}},
            {square_g8, {side_black, piece_knight}},
            {square_h8, {side_black, piece_rook}},

            {square_a7, {side_black, piece_pawn}},
            {square_b7, {side_black, piece_pawn}},
            {square_c7, {side_black, piece_pawn}},
            {square_d7, {side_black, piece_pawn}},
            {square_e7, {side_black, piece_pawn}},
            {square_f7, {side_black, piece_pawn}},
            {square_g7, {side_black, piece_pawn}},
            {square_h7, {side_black, piece_pawn}},

            {square_a2, {side_white, piece_pawn}},
            {square_b2, {side_white, piece_pawn}},
            {square_c2, {side_white, piece_pawn}},
            {square_d2, {side_white, piece_pawn}},
            {square_e2, {side_white, piece_pawn}},
            {square_f2, {side_white, piece_pawn}},
            {square_g2, {side_white, piece_pawn}},
            {square_h2, {side_white, piece_pawn}},

            {square_a1, {side_white, piece_rook}},
            {square_b1, {side_white, piece_knight}},
            {square_c1, {side_white, piece_bishop}},
            {square_d1, {side_white, piece_queen}},
            {square_e1, {side_white, piece_king}},
            {square_f1, {side_white, piece_bishop}},
            {square_g1, {side_white, piece_knight}},
            {square_h1, {side_white, piece_rook}},
        }
    )
    {
    }

    /// Arbitrary board.
    ///
    /// Construct board with the given piece placement. Takes a map from
    /// squares to piece and color and places the given pieces at the given
    /// square.
    ///
    /// \param pieces The piece placement.
    board(const std::unordered_map<square, std::pair<side, piece>>& pieces):
    square_sides{},
    square_pieces{},
    side_sets{},
    piece_sets{},
    zobrist_hash{0}
    {
        square_sides.fill(side_none);
        square_pieces.fill(piece_none);
        side_sets.fill(empty_set);
        piece_sets.fill(empty_set);

        for(auto [sq, sp]: pieces)
        {
            auto [s, p] = sp;
            set(sq, s, p);
        }
    }

    /// Get side and piece at the given square.
    ///
    /// Returns the side and piece at the given square. Both side and piece
    /// can be none, in which case the square is empty.
    ///
    /// \param sq The square.
    /// \returns Pair of side and piece of given square.
    std::pair<side, piece> get(square sq) const
    {
        return {square_sides[sq], square_pieces[sq]};
    }

    /// Set side and piece at the given square.
    ///
    /// Sets the side and piece of given square, and updates other internal
    /// state accordingly.
    /// \param sq The square.
    /// \param s New side of square.
    /// \param p New piece of square.
    void set(square sq, side s, piece p)
    {
        side s_prev = square_sides[sq];
        piece p_prev = square_pieces[sq];

        square_sides[sq] = s;
        square_pieces[sq] = p;

        if(s_prev != side_none && p_prev != piece_none)
        {
            side_sets[s_prev] = set_erase(side_sets[s_prev], sq);
            piece_sets[p_prev] = set_erase(piece_sets[p_prev], sq);
            zobrist_hash ^= pieces_zobrist_hash[sq][s_prev][p_prev];
        }

        if(s != side_none && p != piece_none)
        {
            side_sets[s] = set_insert(side_sets[s], sq);
            piece_sets[p] = set_insert(piece_sets[p], sq);
            zobrist_hash ^= pieces_zobrist_hash[sq][s][p];
        }
    }

    /// Clear board.
    ///
    /// Sets the side and piece of all squares to none, and updates
    /// other internal state accordingly.
    void clear()
    {
        for(int s = square_a1; s <= square_h8; s++)
        {
            square_sides[s] = side_none;
            square_pieces[s] = piece_none;
        }

        for(int p = piece_pawn; p <= piece_king; p++)
        {
            piece_sets[p] = 0;
        }
        
        side_sets[side_white] = 0;
        side_sets[side_black] = 0;

        zobrist_hash = 0;
    }

    /// Side set.
    ///
    /// Returns the set of all squares with pieces of the given side on them.
    ///
    /// \param s The side.
    /// \returns The set of squares.
    inline bitboard side_set(side s) const
    {
        return side_sets[s];
    }

    /// Piece set.
    ///
    /// Returns the set of all squares with the given piece type on them.
    /// A side can optionally be specified to only get pieces of one side.
    ///
    /// \param p The piece.
    /// \param s The side.
    /// \returns The set of squares.
    inline bitboard piece_set(piece p, side s = side_none) const
    {
        return piece_sets[p] & (s != side_none ? side_sets[s] : universal_set);
    }

    /// Occupied set.
    ///
    /// Returns the set of all squares occupied by pieces of any side.
    ///
    /// \returns Occupied squares.
    inline bitboard occupied_set() const
    {
        return side_sets[side_white] | side_sets[side_black];
    }

    /// Attack set.
    ///
    /// Returns the set of all squares attacked by a given side.
    ///
    /// \param s The side.
    /// \returns Squares attacked by side.
    bitboard attack_set(side s) const
    {
        bitboard pawns = piece_set(piece_pawn, s);
        bitboard rooks = piece_set(piece_rook, s);
        bitboard knights = piece_set(piece_knight, s);
        bitboard bishops = piece_set(piece_bishop, s);
        bitboard queens = piece_set(piece_queen, s);
        bitboard kings = piece_set(piece_king, s);

        bitboard attacks = 0;

        attacks |= set_shift(pawns, static_cast<direction>(forwards(s) + direction_e));
        attacks |= set_shift(pawns, static_cast<direction>(forwards(s) + direction_w));

        while(rooks)
        {
            square from = set_first(rooks);
            rooks = set_erase(rooks, from);
            attacks |= rook_attack_mask(from, occupied_set());
        }

        while(knights)
        {
            square from = set_first(knights);
            knights = set_erase(knights, from);
            attacks |= knight_attack_mask(from);
        }

        while(bishops)
        {
            square from = set_first(bishops);
            bishops = set_erase(bishops, from);
            attacks |= bishop_attack_mask(from, occupied_set());
        }

        while(queens)
        {
            square from = set_first(queens);
            queens = set_erase(queens, from);
            attacks |= (rook_attack_mask(from, occupied_set()) | bishop_attack_mask(from, occupied_set()));
        }

        while(kings)
        {
            square from = set_first(kings);
            kings = set_erase(kings, from);
            attacks |= king_attack_mask(from);
        }

        return attacks;
    }

    /// Board hash.
    ///
    /// Returns the Zobrist hash of the board (piece placement).
    ///
    /// \returns Zobrist hash.
    inline std::size_t hash() const
    {
        return zobrist_hash;
    }

    /// Board to string.
    ///
    /// Returns (pretty) string representation of board, one rank per row.
    /// Starts at top left corner of board (square A8)
    ///
    /// \param coords Show coordinates.
    /// \returns Board string.
    std::string to_string(bool coords = true) const
    {
        std::ostringstream stream;

        for(int i = rank_8; i >= rank_1; i--)
        {
            rank r = static_cast<rank>(i);

            if(coords)
            {
                stream << rank_to_san(r);
            }

            for(int j = file_a; j <= file_h; j++)
            {
                file f = static_cast<file>(j);

                square sq = cat_coords(f, r);
                auto [s, p] = get(sq);

                if(p == piece_none)
                {
                    stream << '.';
                }
                else
                {
                    stream << piece_to_san(s, p);
                }
            }

            stream << '\n';
        }

        if(coords)
        {
            stream << ' ';
            for(int j = file_a; j <= file_h; j++)
            {
                file f = static_cast<file>(j);
                stream << file_to_san(f);
            }
        }

        return stream.str();
    }

private:
    std::array<side, squares> square_sides;
    std::array<piece, squares> square_pieces;

    std::array<bitboard, sides> side_sets;
    std::array<bitboard, pieces> piece_sets;

    std::size_t zobrist_hash;
};


/// Chess move.
///
/// Contains all information needed to make a move.
struct move
{
	/// Null-move.
    ///
    /// Constructs move which does nothing. Should probably not be used.
	move():
	from{square_none},
	to{square_none},
	promote{piece_none}
	{}

    /// Move constructor.
    ///
    /// Constructs move from given square to given square. If the move is a
    /// pawn push or attack to the final rank for the side making the move,
    /// a promotion piece should be specified.
    ///
    /// \param from Source square.
    /// \param to Destination square.
    /// \param promote Promotion piece.
	move(square from, square to, piece promote = piece_none):
	from{from},
	to{to},
	promote{promote}
	{}

	// todo: implement
	/*
	static move from_san(std::string_view san, const position& p)
	{

	}
	*/

    /// Move from Long Algebraic Notation (LAN).
    ///
    /// Create move from LAN. "a2a4" is the move from square A2 to A4.
    /// "h7h8q" is a promotion to a queen on the H file.
    ///
    /// \param lan LAN string.
    /// \returns Move corresponding to LAN.
    /// \throws Invalid argument if LAN does not denote a move.
	static move from_lan(std::string_view lan)
	{
		square from = square_from_san(lan.substr(0, 2));
		square to = square_from_san(lan.substr(2, 2));
		piece promote = lan.length() == 5 ? piece_from_san(lan.back()).second : piece_none;

		return move(from, to, promote);
	}

    /// Move to Long Algebraic Notation (LAN).
    ///
    /// Covnert move to LAN. "a2a4" is the move from square A2 to A4.
    /// "h8h1q" is a promotion to a queen on the H file.
    ///
    /// \returns LAN of move.
	std::string to_lan() const
	{
        std::string lan = square_to_san(from) + square_to_san(to);
        if(promote != piece_none)
        {
            lan += piece_to_san(side_black, promote);
        }
        return lan;
	}

    auto operator<=>(const move&) const = default;

    square from;
    square to;
    piece promote;
};

/// Chess move undo.
///
/// Contains all information needed to undo a move on a given position. This
/// is the information that can not be deduced from the position state and move
/// information alone.
struct undo
{
    piece capture;
    square en_passant;
    std::array<bool, sides> kingside_castle;
    std::array<bool, sides> queenside_castle;
    // todo: halfmove clock?
};


/// Chess position.
///
/// Contains information about a chess position including piece placement,
/// whose turn it is, castling avalailability, en passant target square...
class position
{
public:
    /// Start position.
    ///
    /// Constructs initial position.
    position():
    b(),
    turn{side_white},
    kingside_castle{true, true},
    queenside_castle{true, true},
    en_passant{square_none},
    halfmove_clock{0},
    fullmove_number{1},
    zobrist_hash{0}
    {}

    /// Construct position.
    ///
    /// Construct a position froo the given information. It is assumed that the
    /// position is valid.
    ///
    /// \param pieces Piece placement.
    /// \param turn The side whose turn it is.
    /// \param white_kingside_castle White kingside castling availability.
    /// \param white_queenside_castle White queenside castling availability.
    /// \param black_kingside_castle Black kingside castling availability.
    /// \param black_queenside_castle Black queenside castling availability.
    /// \param en_passant En passant target square (if last move was a double pawn push).
    /// \param halfmove_clock Number of silent halfmoves (for 50 move rule).
    /// \param fullmove_number Number of fullmoves made.
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
    b(pieces),
    turn{turn},
    kingside_castle{white_kingside_castle, black_kingside_castle},
    queenside_castle{white_queenside_castle, black_queenside_castle},
    en_passant{en_passant},
    halfmove_clock{halfmove_clock},
    fullmove_number{fullmove_number},
    zobrist_hash{0}
    {
        if(turn == side_black)              zobrist_hash ^= side_zobrist_hash;
        if(kingside_castle[side_white])     zobrist_hash ^= kingside_castle_zobrist_hash[side_white];
        if(queenside_castle[side_white])    zobrist_hash ^= queenside_castle_zobrist_hash[side_white];
        if(kingside_castle[side_black])     zobrist_hash ^= kingside_castle_zobrist_hash[side_black];
        if(queenside_castle[side_black])    zobrist_hash ^= queenside_castle_zobrist_hash[side_black];
        if(en_passant != square_none)       zobrist_hash ^= en_passant_zobrist_hash[file_of(en_passant)];
    }


    /// Fen for starting position.
	static const inline std::string_view fen_start = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	
    /// Fen for empty position.
    static const inline std::string_view fen_empty = "8/8/8/8/8/8/8/8 w - - 0 1";

    /// Create position from Forsyth-Edwards Notation (FEN).
    ///
    /// Parse FEN string and returns the position it encodes.
    ///
    /// \param fen FEN string. Defaults to starting position.
    /// \returns Position encoded by FEN.
    /// \throws Invalid argument if FEN seems to be invalid.
    /// \note To get the initial position, the default position constructor can be used.
	static position from_fen(std::string_view fen)
	{
		std::string fen_string(fen);
		std::istringstream fen_stream(fen_string);

		std::string pieces_string, turn_string, castle_string, en_passant_string;
		int halfmove_clock, fullmove_number;

		if(!(fen_stream >> pieces_string >> turn_string >> castle_string >> en_passant_string >> halfmove_clock >> fullmove_number))
		{
			throw std::invalid_argument("fen does not contain 7 fields");
		}

		// pieces
		std::unordered_map<square, std::pair<side, piece>> pieces;
		int r = rank_8;
	    int f = file_a;

	    for(char c : pieces_string)
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

			if(!(file_a <= f && f <= file_h) || !(rank_1 <= r && r <= rank_8))
			{
				throw std::invalid_argument("fen contains ill-formed piece placement field");
			}

			pieces[cat_coords(static_cast<file>(f), static_cast<rank>(r))] = piece_from_san(c);
	        
			f++;
	    }

		// turn
		side turn = side_none;

		if(turn_string.length() != 1)
		{
			throw std::invalid_argument("fen contains ill-formed piece turn field");
		}

		if(turn_string.front() == 'w')
		{
			turn = side_white;
		}
		else if(turn_string.front() == 'b')
		{
			turn = side_black;
		}
		else
		{
			throw std::invalid_argument("fen contains ill-formed piece turn field");
		}

		// castle
		bool white_kingside_castle = false;
		bool white_queenside_castle = false;
		bool black_kingside_castle = false;
		bool black_queenside_castle = false;

	    for(char c : castle_string)
	    {
	        switch(c)
	        {
	        case 'K':
	            white_kingside_castle = true;
	            break;
	        case 'Q':
	            white_queenside_castle = true;
	            break;
	        case 'k':
	            black_kingside_castle = true;
	            break;
	        case 'q':
	            black_queenside_castle = true;
	            break;
	        case '-':
				break;
	        default:
				throw std::invalid_argument("fen contains ill-formed castling availability field");
	        }
	    }

	    // en passant
		square en_passant = square_none;

	    if(en_passant_string != "-")
	    {
			en_passant = square_from_san(en_passant_string);
	    }

		return position
		(
			board(pieces),
			turn,
			white_kingside_castle,
			white_queenside_castle,
			black_kingside_castle,
			black_queenside_castle,
			en_passant,
			halfmove_clock,
			fullmove_number
		);
	}

    /// Convert position to Forsyth-Edwards Notation (FEN).
    ///
    /// Serialize position to FEN string.
    ///
    /// \returns FEN encoding position.
	std::string to_fen() const
	{
		std::ostringstream fen_stream;

	    for(int r = rank_8; r >= rank_1; r--)
	    {
	        int empty = 0;
	        for(int f = file_a; f <= file_h; f++)
	        {
	            square sq = cat_coords(static_cast<file>(f), static_cast<rank>(r));
				auto[s, p] = b.get(sq);

	            if(p == piece_none)
	            {
	                empty++;
	                continue;
	            }
	            else if(empty != 0)
	            {
					fen_stream << empty;
	                empty = 0;
	            }

				fen_stream << piece_to_san(s, p);
	        }

	        if(empty != 0)
			{
				fen_stream << empty;
			}
	        if(r != rank_1)
			{
				fen_stream << '/';
			}
		}
	    
		fen_stream << (turn == side_white ? " w " : " b ");

		auto pos = fen_stream.tellp();

	    if(kingside_castle[side_white])		fen_stream << 'K';
	    if(queenside_castle[side_white]) 	fen_stream << 'Q';
	    if(kingside_castle[side_black]) 	fen_stream << 'k';
	    if(queenside_castle[side_black]) 	fen_stream << 'q';
	    if(pos == fen_stream.tellp()) 		fen_stream << '-';

		fen_stream << ' ' << square_to_san(en_passant) << ' ' << halfmove_clock << ' ' << fullmove_number;

		return fen_stream.str();
	}


    /// Make move.
    ///
    /// Make move on given position by updating internal state.
    ///
    /// \param m The move.
    /// \returns Undo data.
    undo make_move(const move& m)
    {
		piece capture = b.get(m.to).second;
        undo u{capture, en_passant, kingside_castle, queenside_castle};

        auto [side, piece] = b.get(m.from);
        square ep = en_passant;

        b.set(m.from, side_none, piece_none);

        if(m.promote != piece_none) b.set(m.to, side, m.promote);
        else b.set(m.to, side, piece);

        en_passant = square_none;
        if(ep != square_none) zobrist_hash ^= en_passant_zobrist_hash[file_of(ep)];

        if(piece == piece_pawn)
        {
            if(rank_of(m.from) == side_rank(side, rank_2) && rank_of(m.to) == side_rank(side, rank_4))
            {
                // double push
                en_passant = cat_coords(file_of(m.from), side_rank(side, rank_3));
                zobrist_hash ^= en_passant_zobrist_hash[file_of(en_passant)];
            }
            else if(m.to == ep)
            {
                // en passant capture
                square ep_capture = cat_coords(file_of(ep), side_rank(side, rank_5));
                b.set(ep_capture, side_none, piece_none);
            }
        }
        else if(piece == piece_king)
        {
            if(kingside_castle[side])
            {
                kingside_castle[side] = false;
                zobrist_hash ^= kingside_castle_zobrist_hash[side];
            }
            if(queenside_castle[side])
            {
                queenside_castle[side] = false;
                zobrist_hash ^= queenside_castle_zobrist_hash[side];
            }
            
            rank rank_first = side_rank(side, rank_1);

            if(m.from == cat_coords(file_e, rank_first))
            {
                if(m.to == cat_coords(file_g, rank_first))
                {
                    b.set(cat_coords(file_h, rank_first), side_none, piece_none);
                    b.set(cat_coords(file_f, rank_first), side, piece_rook);
                }
                else if(m.to == cat_coords(file_c, rank_first))
                {
                    b.set(cat_coords(file_a, rank_first), side_none, piece_none);
                    b.set(cat_coords(file_d, rank_first), side, piece_rook);
                }
            }
        }

        // todo: these (and en passant) should only be updated if they changed from the previous move
        // can probably be done by xoring with current state before each move

        if(queenside_castle[side_white] && (m.from == square_a1 || m.to == square_a1))
        {
            queenside_castle[side_white] = false;
            zobrist_hash ^= queenside_castle_zobrist_hash[side_white];
        }
        if(kingside_castle[side_white] && (m.from == square_h1 || m.to == square_h1))
        {
            kingside_castle[side_white] = false;
            zobrist_hash ^= kingside_castle_zobrist_hash[side_white];
        }
        if(queenside_castle[side_black] && (m.from == square_a8 || m.to == square_a8))
        {
            queenside_castle[side_black] = false;
            zobrist_hash ^= queenside_castle_zobrist_hash[side_black];
        }
        if(kingside_castle[side_black] && (m.from == square_h8 || m.to == square_h8))
        {
            kingside_castle[side_black] = false;
            zobrist_hash ^= kingside_castle_zobrist_hash[side_black];
        }

        if(piece == piece_pawn || capture != piece_none)
        {
            halfmove_clock = 0;
        }
        else
        {
            halfmove_clock++;
        }

        fullmove_number += turn;
        turn = opponent(turn);
        zobrist_hash ^= side_zobrist_hash;

        return u;
    }

    /// Undo move.
    ///
    /// Undo move on given position by updating internal state. The undo data
    /// used should be the one returned when making the move.
    ///
    /// \param m The move.
    /// \param u Undo data.
    void undo_move(const move& m, const undo& u)
    {
        auto [side, piece] = b.get(m.to);

        b.set(m.from, side, piece);
        b.set(m.to, side_none, piece_none);
        if(u.capture != piece_none) b.set(m.to, opponent(side), u.capture);
        if(m.promote != piece_none) b.set(m.from, side, piece_pawn);

        if(en_passant != square_none) zobrist_hash ^= en_passant_zobrist_hash[file_of(en_passant)];
        if(u.en_passant != square_none) zobrist_hash ^= en_passant_zobrist_hash[file_of(u.en_passant)];
        en_passant = u.en_passant;

        if(kingside_castle[side_white] != u.kingside_castle[side_white])
        {
            kingside_castle[side_white] = u.kingside_castle[side_white];
            zobrist_hash ^= kingside_castle_zobrist_hash[side_white];
        }
        if(kingside_castle[side_black] != u.kingside_castle[side_black])
        {
            kingside_castle[side_black] = u.kingside_castle[side_black];
            zobrist_hash ^= kingside_castle_zobrist_hash[side_black];
        }
        if(queenside_castle[side_white] != u.queenside_castle[side_white])
        {
            queenside_castle[side_white] = u.queenside_castle[side_white];
            zobrist_hash ^= queenside_castle_zobrist_hash[side_white];
        }
        if(queenside_castle[side_black] != u.queenside_castle[side_black])
        {
            queenside_castle[side_black] = u.queenside_castle[side_black];
            zobrist_hash ^= queenside_castle_zobrist_hash[side_black];
        }

        if(piece == piece_pawn)
        {
            if(m.to == u.en_passant)
            {
                square ep_capture = cat_coords(file_of(u.en_passant), side_rank(side, rank_5));
                b.set(ep_capture, opponent(side), piece_pawn);
            }
        }
        else if(piece == piece_king)
        {
            rank rank_first = side_rank(side, rank_1);

            if(m.from == cat_coords(file_e, rank_first))
            {
                if(m.to == cat_coords(file_g, rank_first))
                {
                    b.set(cat_coords(file_h, rank_first), side, piece_rook);
                    b.set(cat_coords(file_f, rank_first), side_none, piece_none);
                }
                else if(m.to == cat_coords(file_c, side_rank(side, rank_1)))
                {
                    b.set(cat_coords(file_a, rank_first), side, piece_rook);
                    b.set(cat_coords(file_d, rank_first), side_none, piece_none);
                }
            }
        }

        halfmove_clock--; // todo: only if silent move
        fullmove_number -= opponent(turn); // decrease if white
        turn = opponent(turn);
        zobrist_hash ^= side_zobrist_hash;
    }

    /// Copy move.
    ///
    /// Make move on given position by copying state and updating the copy.
    /// This is a little more expensive than making a move by updating internal
    /// state, but when a lot of move undos are involved, copying seems to
    /// perform similarly (as undoing is not necessary).
    ///
    /// \param m The move.
    /// \returns Position with the move made.
    position copy_move(const move& m) const
    {
        position p = *this;
        p.make_move(m);
        return p;
    }


    /// Legal moves.
    ///
    /// Returns list of legal moves in position.
    ///
    /// \returns List of legal moves.
    std::vector<move> moves() const
    {
        std::vector<move> moves;

        bitboard occupied = b.occupied_set();
        
        bitboard pawns = b.piece_set(piece_pawn, turn);
        bitboard rooks = b.piece_set(piece_rook, turn);
        bitboard knights = b.piece_set(piece_knight, turn);
        bitboard bishops = b.piece_set(piece_bishop, turn);
        bitboard queens = b.piece_set(piece_queen, turn);
        bitboard kings = b.piece_set(piece_king, turn);
        
        bitboard attack_mask = ~b.side_set(turn);
        bitboard capture_mask = b.side_set(opponent(turn));
        bitboard ep_mask = 0;
        if(en_passant != square_none) ep_mask = square_set(en_passant);

        // pawn moves
        bitboard single_push_tos = set_shift(pawns, forwards(turn)) & ~occupied;
        bitboard single_push_froms = set_shift(single_push_tos, forwards(opponent(turn)));
        bitboard double_push_tos = set_shift(single_push_tos & rank_set(side_rank(turn, rank_3)), forwards(turn)) & ~occupied;
        bitboard double_push_froms = set_shift(set_shift(double_push_tos, forwards(opponent(turn))), forwards(opponent(turn)));

        bitboard attack_east_tos = set_shift(pawns, static_cast<direction>(forwards(turn) + direction_e)) & (capture_mask | ep_mask);
        bitboard attack_east_froms = set_shift(attack_east_tos, static_cast<direction>(forwards(opponent(turn)) + direction_w));
        bitboard attack_west_tos = set_shift(pawns, static_cast<direction>(forwards(turn) + direction_w)) & (capture_mask | ep_mask);
        bitboard attack_west_froms = set_shift(attack_west_tos, static_cast<direction>(forwards(opponent(turn)) + direction_e));
        
        bitboard promote_push_tos = single_push_tos & rank_set(side_rank(turn, rank_8));
        bitboard promote_push_froms = single_push_froms & rank_set(side_rank(turn, rank_7));
        bitboard promote_east_tos = attack_east_tos & rank_set(side_rank(turn, rank_8));
        bitboard promote_east_froms = attack_east_froms & rank_set(side_rank(turn, rank_7));
        bitboard promote_west_tos = attack_west_tos & rank_set(side_rank(turn, rank_8));
        bitboard promote_west_froms = attack_west_froms & rank_set(side_rank(turn, rank_7));;

        single_push_tos ^= promote_push_tos;
        single_push_froms ^= promote_push_froms;
        attack_east_tos ^= promote_east_tos;
        attack_east_froms ^= promote_east_froms;
        attack_west_tos ^= promote_west_tos;
        attack_west_froms ^= promote_west_froms;

        setwise_moves(single_push_froms, single_push_tos, piece_none, moves);
        setwise_moves(double_push_froms, double_push_tos, piece_none, moves);
        
        setwise_moves(attack_east_froms, attack_east_tos, piece_none, moves);
        setwise_moves(attack_west_froms, attack_west_tos, piece_none, moves);
        
        setwise_moves(promote_push_froms, promote_push_tos, piece_rook, moves);
        setwise_moves(promote_push_froms, promote_push_tos, piece_knight, moves);
        setwise_moves(promote_push_froms, promote_push_tos, piece_bishop, moves);
        setwise_moves(promote_push_froms, promote_push_tos, piece_queen, moves);

        setwise_moves(promote_east_froms, promote_east_tos, piece_rook, moves);
        setwise_moves(promote_east_froms, promote_east_tos, piece_knight, moves);
        setwise_moves(promote_east_froms, promote_east_tos, piece_bishop, moves);
        setwise_moves(promote_east_froms, promote_east_tos, piece_queen, moves);

        setwise_moves(promote_west_froms, promote_west_tos, piece_rook, moves);
        setwise_moves(promote_west_froms, promote_west_tos, piece_knight, moves);
        setwise_moves(promote_west_froms, promote_west_tos, piece_bishop, moves);
        setwise_moves(promote_west_froms, promote_west_tos, piece_queen, moves);

        // rook moves
        while(rooks)
        {
            square from = set_first(rooks);
            rooks = set_erase(rooks, from);
            bitboard attacks = rook_attack_mask(from, occupied) & attack_mask;
            piecewise_moves(from, attacks, piece_none, moves);
        }

        // knight moves
        while(knights)
        {
            square from = set_first(knights);
            knights = set_erase(knights, from);
            bitboard attacks = knight_attack_mask(from) & attack_mask;
            piecewise_moves(from, attacks, piece_none, moves);
        }

        // bishop moves
        while(bishops)
        {
            square from = set_first(bishops);
            bishops = set_erase(bishops, from);
            bitboard attacks = bishop_attack_mask(from, occupied) & attack_mask;
            piecewise_moves(from, attacks, piece_none, moves);
        }

        // queen moves
        while(queens)
        {
            square from = set_first(queens);
            queens = set_erase(queens, from);
            bitboard attacks = (rook_attack_mask(from, occupied) | bishop_attack_mask(from, occupied)) & attack_mask;
            piecewise_moves(from, attacks, piece_none, moves);
        }

        // king moves
        if(kingside_castle[turn])
        {
            square from = set_first(kings);
            square to = cat_coords(file_g, rank_of(from));
            bitboard path = kings;
            path |= set_shift(path, direction_e);
            path |= set_shift(path, direction_e);
            bitboard between = path & ~kings;

            if(!(between & occupied) && !(path & b.attack_set(opponent(turn))))
            {
                moves.emplace_back(from, to, piece_none);
            }
        }
        if(queenside_castle[turn])
        {
            square from = set_first(kings);
            square to = cat_coords(file_c, rank_of(from));
            bitboard path = kings;
            path |= set_shift(path, direction_w);
            path |= set_shift(path, direction_w);
            bitboard between = set_shift(path, direction_w);

            if(!(between & occupied) && !(path & b.attack_set(opponent(turn))))
            {
                moves.emplace_back(from, to, piece_none);
            }
        }
        while(kings)
        {
            square from = set_first(kings);
            kings = set_erase(kings, from);
            bitboard attacks = king_attack_mask(from) & attack_mask;
            piecewise_moves(from, attacks, piece_none, moves);
        }

        // remove illegal moves
        side player = turn;
        int n = 0;
        for(move& m: moves)
        {
            position p = copy_move(m);;

            if(!(p.b.attack_set(opponent(player)) & p.b.piece_set(piece_king, player)))
            {
                moves[n++] = m;
            }
        }

        moves.resize(n);

        return moves;
    }

    /// Position pieces.
    ///
    /// Returns the piece placement of the position.
    ///
    /// \returns The pieces.
    const board& pieces() const
    {
        return b;
    }

    /// Fullmove number.
    ///
    /// Returns the number of fullmoves since initial position.
    ///
    /// \returns The fullmove number.
    int fullmove() const
    {
        return fullmove_number;
    }

    /// Halfmove number.
    ///
    /// Returns the number of halfmove (often referred to as ply)
    /// since initial position.
    ///
    /// \returns The halfmove number.
    int halfmove() const
    {
        return (fullmove_number - 1)*2 + turn;
    }

    /// Position hash.
    ///
    /// Returns the Zobrist hash of the position.
    ///
    /// \returns The hash.
    std::size_t hash() const
    {
        return zobrist_hash ^ b.hash();
    }

    /// Get current turn
    ///
    /// Returns the side of the player whose turn it is
    ///
    /// \returns Current turn.
    side get_turn() const
    {
        return turn;
    }

    /// Check flag.
    ///
    /// Returns whether the side whose turn it is is checked.
    ///
    /// \returns Check flag.
    bool is_check() const
    {
        return b.attack_set(opponent(turn)) & b.piece_set(piece_king, turn);
    }

    /// Checkmate flag.
    ///
    /// Returns whether the side whose turn it is is checkmated.
    ///
    /// \returns Checkmate flag.
    bool is_checkmate() const
    {
        return is_check() && moves().empty();
    }

    /// Stalemate flag.
    ///
    /// Returns whether the side whose turn it is is stalemated.
    ///
    /// \returns Stalemate flag.
    bool is_stalemate() const
    {
        return !is_check() && moves().empty();
    }

    /// Board to string.
    ///
    /// Returns (pretty) string representation of board, one rank per row.
    /// Starts at top left corner of board (square A8)
    ///
    /// \param coords Show coordinates.
    /// \returns Board string.
    std::string to_string(bool coords = true) const
    {
        std::ostringstream stream;

        stream << '\n';
        stream << "turn: " << (turn == side_white ? "white" : "black") << '\n';
        stream << "white kingside castle: " << (kingside_castle[side_white] ? "yes" : "no") << '\n';
        stream << "white queenside castle: " << (queenside_castle[side_white] ? "yes" : "no") << '\n';
        stream << "black kingside castle: " << (kingside_castle[side_black] ? "yes" : "no") << '\n';
        stream << "black queenside castle: " << (queenside_castle[side_black] ? "yes" : "no") << '\n';
        stream << "halfmove clock: " << halfmove_clock << '\n';
        stream << "fullmove number: " << fullmove_number << '\n';

        return b.to_string() + stream.str();
    }

private:
    inline void piecewise_moves(square from, bitboard tos, piece promote, std::vector<move>& moves) const
    {
        while(tos)
        {
            square to = set_first(tos);
            tos = set_erase(tos, to);
            moves.emplace_back(from, to, promote);
        }
    }

    inline void setwise_moves(bitboard froms, bitboard tos, piece promote, std::vector<move>& moves) const
    {
        while(froms && tos)
        {
            square from = set_first(froms);
            square to = set_first(tos);
            froms = set_erase(froms, from);
            tos = set_erase(tos, to);
            moves.emplace_back(from, to, promote);
        }
    }

    board b;
    side turn;
    std::array<bool, sides> kingside_castle;
    std::array<bool, sides> queenside_castle;
    square en_passant;
    int halfmove_clock;
    int fullmove_number;
    std::size_t zobrist_hash;
};


/// Initialize library.
///
/// Sets up internal state such as attack tables and Zobrist hash keys.
/// The default seed seems to work fine but there might exist one that is both
/// faster and yields better pseudorandom numbers.
void init(std::size_t seed = 2147483647ULL)
{
	const std::array<direction, 4> rook_directions{direction_n, direction_e, direction_s, direction_w};
	const std::array<direction, 4> bishop_directions{direction_ne, direction_se, direction_sw, direction_nw};
	const std::array<direction, 8> knight_directions{direction_nne, direction_ene, direction_ese, direction_sse, direction_ssw, direction_wsw, direction_wnw, direction_nnw};
	const std::array<direction, 8> king_directions{direction_n, direction_ne, direction_e, direction_se, direction_s, direction_sw, direction_w, direction_nw};

    ray_table_init(rook_attacks.data(), rook_magics, rook_directions, seed);
    ray_table_init(bishop_attacks.data(), bishop_magics, bishop_directions, seed);
    shift_table_init(knight_attacks.data(), knight_directions);
    shift_table_init(king_attacks.data(), king_directions);

    for(int i = square_a1; i <= square_h8; i++)
    {
        for(int j = piece_pawn; j <= piece_king; j++)
        {
            square sq = static_cast<square>(i);
            piece p = static_cast<piece>(j);

            pieces_zobrist_hash[sq][side_white][p] = random_generate(seed);
            pieces_zobrist_hash[sq][side_black][p] = random_generate(seed);
        }
    }

    side_zobrist_hash = random_generate(seed);

    kingside_castle_zobrist_hash[side_white] = random_generate(seed);
    kingside_castle_zobrist_hash[side_black] = random_generate(seed);
    queenside_castle_zobrist_hash[side_white] = random_generate(seed);
    queenside_castle_zobrist_hash[side_black] = random_generate(seed);

    for(int f = file_a; f <= file_h; f++)
    {
        en_passant_zobrist_hash[f] = random_generate(seed);
    }
}


class game
{
public:
    /// New game.
    ///
    /// Constructs game with empty position and no moves played
    game();
};


}


#endif