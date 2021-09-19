#ifndef CHESS_COMMON_HPP
#define CHESS_COMMON_HPP


#include <array>
#include <bit>
#include <cstdint>


namespace chess
{


const int sides = 2;

enum side
{
    side_white,
    side_black,
    side_none = -1,
};


constexpr side opponent(side s)
{
    return static_cast<side>(!s);
}


const int pieces = 6;

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


constexpr rank side_rank(side s, rank r)
{
    return static_cast<rank>(r*static_cast<int>(opponent(s)) + (rank_8-r)*s);
}


constexpr file file_of(square sq)
{
    return static_cast<file>(sq % 8);
}

constexpr rank rank_of(square sq)
{
    return static_cast<rank>(sq / 8);
}

constexpr square cat_coords(file f, rank r)
{
    return static_cast<square>(r*8 + f);
}


struct move
{
    square from;
    square to;
    piece promote{piece_none};
};

struct undo
{
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

constexpr direction opposite(direction d)
{
    return static_cast<direction>((d + 8) % 16);
}

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



using bitboard = std::uint64_t;


const bitboard bitboard_empty = 0ULL;
const bitboard bitboard_full = ~0ULL;


constexpr bitboard square_mask(square sq)
{
    return 1ULL << sq;
}

constexpr bitboard file_mask(file f)
{
    return 0x0101010101010101ULL << f;
}

constexpr bitboard rank_mask(rank r)
{
    return 0xFFULL << (r*8);
}

inline bool test_square(bitboard bb, square sq)
{
    return bb & square_mask(sq);
}

inline bitboard set_square(bitboard bb, square sq)
{
    return bb | square_mask(sq);
}

inline bitboard toggle_square(bitboard bb, square sq)
{
    return bb ^ square_mask(sq);
}

inline bitboard unset_square(bitboard bb, square sq)
{
    return bb & ~square_mask(sq);
}

/*
// used very often
inline bitboard bitboard_shift(bitboard bb, direction d)
{
    if(d > 0)
    {
        bb <<= d;
    }
    else
    {
        bb >>= -d;
    }

    bitboard trim = bitboard_empty;

    switch(d)
    {
    case direction_e:
    case direction_ne:
    case direction_se:
    case direction_nne:
    case direction_sse:
        trim = file_mask(file_a);
        break;
    case direction_w:
    case direction_nw:
    case direction_sw:
    case direction_nnw:
    case direction_ssw:
        trim = file_mask(file_h);
        break;
    case direction_ene:
    case direction_ese:
        trim = file_mask(file_a) | file_mask(file_b);
        break;
    case direction_wnw:
    case direction_wsw:
        trim = file_mask(file_g) | file_mask(file_h);
        break;
    default:
        break;
    }

    return bb & ~trim;
}
*/


inline bitboard bitboard_shift(bitboard bb, direction d)
{
    if(d > 0)
    {
        bb <<= d;
    }
    else
    {
        bb >>= -d;
    }

    switch(d)
    {
    case direction_e:
    case direction_ne:
    case direction_se:
    case direction_nne:
    case direction_sse:
        bb &= ~file_mask(file_a);
        break;
    case direction_w:
    case direction_nw:
    case direction_sw:
    case direction_nnw:
    case direction_ssw:
        bb &= ~file_mask(file_h);
        break;
    case direction_ene:
    case direction_ese:
        bb &= ~(file_mask(file_a) | file_mask(file_b));
        break;
    case direction_wnw:
    case direction_wsw:
        bb &= ~(file_mask(file_g) | file_mask(file_h));
        break;
    case direction_n:
    case direction_s:
    case direction_none:
    default:
        break;
    }
    return bb;
}

// used often
inline bitboard bitboard_ray(bitboard bb, direction d, bitboard occupied)
{
    bitboard shift = bb;
    bitboard ray = 0;
    while(shift != 0 && !(shift & occupied))
    {
        shift = bitboard_shift(shift, d);
        ray |= shift;
    }
    return ray;
}

inline square first_set_square(bitboard bb)
{
    return static_cast<square>(std::countr_zero(bb));
}

inline square last_set_square(bitboard bb)
{
    return static_cast<square>(square_h8 - std::countl_zero(bb));
}

inline int count_set_squares(bitboard bb)
{
    return std::popcount(bb);
}


struct magic
{
    bitboard mask;
    bitboard magic;
    bitboard* attacks;
    unsigned shift;
};

inline unsigned magic_index(const magic& m, bitboard occupied)
{
    return ((occupied & m.mask) * m.magic) >> m.shift;
}


inline std::size_t random_generate(std::size_t& seed)
{
    seed ^= seed >> 12;
    seed ^= seed << 25;
    seed ^= seed >> 27;

    return seed * 2685821657736338717ULL;
}

inline std::size_t random_sparse(std::size_t& seed)
{
    return random_generate(seed) & random_generate(seed) & random_generate(seed);
}



inline std::array<std::array<std::array<std::size_t, pieces>, sides>, squares> pieces_zobrist_hash;
inline std::size_t side_zobrist_hash;
inline std::array<std::size_t, sides> kingside_castle_zobrist_hash;
inline std::array<std::size_t, sides> queenside_castle_zobrist_hash;
inline std::array<std::size_t, files> en_passant_zobrist_hash;


inline std::array<magic, squares> rook_magics;
inline std::array<magic, squares> bishop_magics;

inline std::array<bitboard, 0x19000> rook_attacks;
inline std::array<bitboard, squares> knight_attacks;
inline std::array<bitboard, 0x1480> bishop_attacks;
inline std::array<bitboard, squares> king_attacks;


inline bitboard pawn_east_attack_mask(bitboard bb, side s)
{
    return bitboard_shift(bb, static_cast<direction>(forwards(s) + direction_e));
}

inline bitboard pawn_west_attack_mask(bitboard bb, side s)
{
    return bitboard_shift(bb, static_cast<direction>(forwards(s) + direction_w));
}

inline bitboard rook_attack_mask(square sq, bitboard occupied)
{
    unsigned index = magic_index(rook_magics[sq], occupied);
    return rook_magics[sq].attacks[index];
}

inline bitboard knight_attack_mask(square sq)
{
    return knight_attacks[sq];
}

inline bitboard bishop_attack_mask(square sq, bitboard occupied)
{
    unsigned index = magic_index(bishop_magics[sq], occupied);
    return bishop_magics[sq].attacks[index];
}

inline bitboard queen_attack_mask(square sq, bitboard occupied)
{
    return rook_attack_mask(sq, occupied) | bishop_attack_mask(sq, occupied);
}

inline bitboard king_attack_mask(square sq)
{
    return king_attacks[sq];
}



void shift_table_init(bitboard* attacks, const std::array<direction, 8>& directions)
{
    for(int i = square_a1; i <= square_h8; i++)
    {
        square sq = static_cast<square>(i);

        bitboard sq_bb = square_mask(sq);

        // initialize knight attacks table
        attacks[sq] = bitboard_shift(sq_bb, directions[0])
                    | bitboard_shift(sq_bb, directions[1])
                    | bitboard_shift(sq_bb, directions[2])
                    | bitboard_shift(sq_bb, directions[3])
                    | bitboard_shift(sq_bb, directions[4])
                    | bitboard_shift(sq_bb, directions[5])
                    | bitboard_shift(sq_bb, directions[6])
                    | bitboard_shift(sq_bb, directions[7]);
    }
}

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

        bitboard sq_bb = square_mask(sq);
        bitboard edges = ((rank_mask(rank_1) | rank_mask(rank_8)) & ~rank_mask(rank_of(sq)))
                       | ((file_mask(file_a) | file_mask(file_h)) & ~file_mask(file_of(sq)));

        magics[sq].mask = bitboard_ray(sq_bb, directions[0], bitboard_empty)
                        | bitboard_ray(sq_bb, directions[1], bitboard_empty)
                        | bitboard_ray(sq_bb, directions[2], bitboard_empty)
                        | bitboard_ray(sq_bb, directions[3], bitboard_empty);
        magics[sq].mask &= ~edges;

        magics[sq].shift = squares - count_set_squares(magics[sq].mask);

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
            reference[size] = bitboard_ray(sq_bb, directions[0], bb)
                            | bitboard_ray(sq_bb, directions[1], bb)
                            | bitboard_ray(sq_bb, directions[2], bb)
                            | bitboard_ray(sq_bb, directions[3], bb);

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
            while(count_set_squares((magics[sq].magic * magics[sq].mask) >> 56) < 6) magics[sq].magic = random_sparse(seed);

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


}



#endif