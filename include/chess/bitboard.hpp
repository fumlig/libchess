#ifndef CHESS_BITBOARD_HPP
#define CHESS_BITBOARD_HPP


#include <iostream>
#include <bit>
#include <cstdint>

#include "types.hpp"
#include "random.hpp"


namespace chess
{


using bitboard = std::uint64_t;


const bitboard empty_mask = 0ULL;
const bitboard full_mask = ~0ULL;



bitboard square_mask(square sq)
{
    return 1ULL << sq;
}

bitboard file_mask(file f)
{
    return 0x0101010101010101ULL << f;
}

bitboard rank_mask(rank r)
{
    return 0xFFULL << (r*8);
}


bool bitboard_get(bitboard bb, square sq)
{
    return bb & square_mask(sq);
}

bitboard bitboard_set(bitboard bb, square sq)
{
    return bb | square_mask(sq);
}

bitboard bitboard_toggle(bitboard bb, square sq)
{
    return bb ^ square_mask(sq);
}

bitboard bitboard_reset(bitboard bb, square sq)
{
    return bb & ~square_mask(sq);
}


bitboard bitboard_shift(bitboard bb, direction d)
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

bitboard bitboard_ray(bitboard bb, direction d, bitboard occupied)
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


square bitboard_ls1b(bitboard bb)
{
    return static_cast<square>(std::countr_zero(bb));
}


square bitboard_ms1b(bitboard bb)
{
    return static_cast<square>(square_h8 - std::countl_zero(bb));
}

int bitboard_count(bitboard bb)
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

unsigned magic_index(magic* m, bitboard occupied)
{
    return ((occupied & m->mask) * m->magic) >> m->shift;
}

static magic rook_magics[squares];
static magic bishop_magics[squares];


static bitboard rook_attacks[0x19000];
static bitboard knight_attacks[squares];
static bitboard bishop_attacks[0x1480];
static bitboard king_attacks[squares];


static direction rook_directions[] = {direction_n, direction_e, direction_s, direction_w};
static direction knight_directions[] = {direction_nne, direction_ene, direction_ese, direction_sse, direction_ssw, direction_wsw, direction_wnw, direction_nnw};
static direction bishop_directions[] = {direction_ne, direction_se, direction_sw, direction_nw};
static direction king_directions[] = {direction_n, direction_ne, direction_e, direction_se, direction_s, direction_sw, direction_w, direction_nw};


// optimal PRNG seeds to pick the correct magics in the shortest time ~stockfish
//static int magic_seeds[ranks] = {728, 10316, 55013, 32803, 12281, 15100, 16645, 255};


void shift_table_init(bitboard* attacks, direction* directions)
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

void ray_table_init(bitboard* attacks, magic* magics, direction* directions, random* r)
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
        bitboard edges = ((rank_mask(rank_1) | rank_mask(rank_8)) & ~rank_mask(square_rank(sq)))
                       | ((file_mask(file_a) | file_mask(file_h)) & ~file_mask(square_file(sq)));

        magics[sq].mask = bitboard_ray(sq_bb, directions[0], empty_mask)
                        | bitboard_ray(sq_bb, directions[1], empty_mask)
                        | bitboard_ray(sq_bb, directions[2], empty_mask)
                        | bitboard_ray(sq_bb, directions[3], empty_mask);
        magics[sq].mask &= ~edges;

        magics[sq].shift = squares - bitboard_count(magics[sq].mask);

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
            while(bitboard_count((magics[sq].magic * magics[sq].mask) >> 56) < 6) magics[sq].magic = random_sparse(r);

            // make sure magic number maps every possible occupancy to index
            // that looks up the correct sliding attack in attacks database

            // attacks database is built up alongside verifying the magic number
            // use epoch to avoid resetting the attack table on failure
            for (++count, j = 0; j < size; ++j)
            {
                unsigned index = magic_index(&magics[sq], occupancy[j]);
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


void bitboard_init(random* r)
{
    ray_table_init(rook_attacks, rook_magics, rook_directions, r);
    shift_table_init(knight_attacks, knight_directions);
    ray_table_init(bishop_attacks, bishop_magics, bishop_directions, r);
    shift_table_init(king_attacks, king_directions);
}


bitboard bitboard_pawn_east_attacks(bitboard bb, side s)
{
    return bitboard_shift(bb, static_cast<direction>(direction_forward(s) + direction_e));
}

bitboard bitboard_pawn_west_attacks(bitboard bb, side s)
{
    return bitboard_shift(bb, static_cast<direction>(direction_forward(s) + direction_w));
}

bitboard bitboard_rook_attacks(square sq, bitboard occupied)
{
    unsigned index = magic_index(&rook_magics[sq], occupied);
    return rook_magics[sq].attacks[index];
}

bitboard bitboard_knight_attacks(square sq)
{
    return knight_attacks[sq];
}

bitboard bitboard_bishop_attacks(square sq, bitboard occupied)
{
    unsigned index = magic_index(&bishop_magics[sq], occupied);
    return bishop_magics[sq].attacks[index];
}

bitboard bitboard_queen_attacks(square sq, bitboard occupied)
{
    return bitboard_rook_attacks(sq, occupied) | bitboard_bishop_attacks(sq, occupied);
}


bitboard bitboard_king_attacks(square sq)
{
    return king_attacks[sq];
}



}


#endif
