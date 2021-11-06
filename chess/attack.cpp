#ifndef CHESS_MAGIC_HPP
#define CHESS_MAGIC_HPP


#include <array>
#include <bit>
#include <cstdint>
#include <vector>

#include "direction.hpp"
#include "random.hpp"
#include "square.hpp"
#include "set.hpp"
#include "attack.hpp"


namespace chess
{


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


static std::array<magic, squares> rook_magics;
static std::array<magic, squares> bishop_magics;
static std::array<bitboard, 0x19000> rook_attack_table;
static std::array<bitboard, squares> knight_attack_table;
static std::array<bitboard, 0x1480> bishop_attack_table;
static std::array<bitboard, squares> king_attack_table;


bitboard pawn_east_attack_set(bitboard bb, side s)
{
    return set_shift(bb, static_cast<direction>(forwards(s) + direction_e));
}


bitboard pawn_west_attack_set(bitboard bb, side s)
{
    return set_shift(bb, static_cast<direction>(forwards(s) + direction_w));
}


bitboard rook_attack_set(square sq, bitboard occupied)
{
    unsigned index = magic_index(rook_magics[sq], occupied);
    return rook_magics[sq].attacks[index];
}


bitboard knight_attack_set(square sq)
{
    return knight_attack_table[sq];
}


bitboard bishop_attack_set(square sq, bitboard occupied)
{
    unsigned index = magic_index(bishop_magics[sq], occupied);
    return bishop_magics[sq].attacks[index];
}


bitboard queen_attack_set(square sq, bitboard occupied)
{
    return rook_attack_set(sq, occupied) | bishop_attack_set(sq, occupied);
}


bitboard king_attack_set(square sq)
{
    return king_attack_table[sq];
}



static void shift_table_init(bitboard* attacks, const std::array<direction, 8>& directions)
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


static void ray_table_init(bitboard* attacks, std::array<magic, squares>& magics, const std::array<direction, 4>& directions, random& rng)
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
            while(set_cardinality((magics[sq].magic * magics[sq].mask) >> 56) < 6) magics[sq].magic = rng.sparse();

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


void attack_init(random& rng)
{
	const std::array<direction, 4> rook_directions{direction_n, direction_e, direction_s, direction_w};
	const std::array<direction, 4> bishop_directions{direction_ne, direction_se, direction_sw, direction_nw};
	const std::array<direction, 8> knight_directions{direction_nne, direction_ene, direction_ese, direction_sse, direction_ssw, direction_wsw, direction_wnw, direction_nnw};
	const std::array<direction, 8> king_directions{direction_n, direction_ne, direction_e, direction_se, direction_s, direction_sw, direction_w, direction_nw};

    ray_table_init(rook_attack_table.data(), rook_magics, rook_directions, rng);
    ray_table_init(bishop_attack_table.data(), bishop_magics, bishop_directions, rng);
    shift_table_init(knight_attack_table.data(), knight_directions);
    shift_table_init(king_attack_table.data(), king_directions);
}


}


#endif