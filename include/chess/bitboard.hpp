#ifndef CHESS_BITBOARD_HPP
#define CHESS_BITBOARD_HPP


#include <iostream>
#include <bit>
#include <cstdint>

#include "piece.hpp"
#include "square.hpp"
#include "direction.hpp"
#include "random.hpp"


namespace chess
{


using bitboard = std::uint64_t;


#define BITBOARD_EMPTY  (bitboard)0
#define BITBOARD_FULL   (bitboard)~0ULL

#define BITBOARD_FILE_A (bitboard)0x0101010101010101
#define BITBOARD_RANK_1 (bitboard)0xFF




bitboard bitboard_square(square sq)
{
    return (bitboard)1 << sq;
}

bitboard bitboard_file(file f)
{
    return (bitboard)BITBOARD_FILE_A << f;
}

bitboard bitboard_rank(rank r)
{
    return (bitboard)BITBOARD_RANK_1 << (r*8);
}


bool bitboard_get(bitboard bb, square sq)
{
    return bb & bitboard_square(sq);
}

bitboard bitboard_set(bitboard bb, square sq)
{
    return bb | bitboard_square(sq);
}

bitboard bitboard_toggle(bitboard bb, square sq)
{
    return bb ^ bitboard_square(sq);
}

bitboard bitboard_reset(bitboard bb, square sq)
{
    return bb & ~bitboard_square(sq);
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
    case DIRECTION_E:
    case DIRECTION_NE:
    case DIRECTION_SE:
    case DIRECTION_NNE:
    case DIRECTION_SSE:
        bb &= ~bitboard_file(FILE_A);
        break;
    case DIRECTION_W:
    case DIRECTION_NW:
    case DIRECTION_SW:
    case DIRECTION_NNW:
    case DIRECTION_SSW:
        bb &= ~bitboard_file(FILE_H);
        break;
    case DIRECTION_ENE:
    case DIRECTION_ESE:
        bb &= ~(bitboard_file(FILE_A) | bitboard_file(FILE_B));
        break;
    case DIRECTION_WNW:
    case DIRECTION_WSW:
        bb &= ~(bitboard_file(FILE_G) | bitboard_file(FILE_H));
        break;
    case DIRECTION_N:
    case DIRECTION_S:
    case DIRECTION_NONE:
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


square bitboard_lsb(bitboard bb)
{
    return static_cast<square>(std::countr_zero(bb));
}


square bitboard_msb(bitboard bb)
{
    return static_cast<square>(SQUARE_H8 - std::countl_zero(bb));
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

static magic rook_magics[SQUARES];
static magic bishop_magics[SQUARES];


static bitboard rook_attacks[0x19000];
static bitboard knight_attacks[SQUARES];
static bitboard bishop_attacks[0x1480];
static bitboard king_attacks[SQUARES];


static direction rook_directions[] = {DIRECTION_N, DIRECTION_E, DIRECTION_S, DIRECTION_W};
static direction knight_directions[] = {DIRECTION_NNE, DIRECTION_ENE, DIRECTION_ESE, DIRECTION_SSE, DIRECTION_SSW, DIRECTION_WSW, DIRECTION_WNW, DIRECTION_NNW};
static direction bishop_directions[] = {DIRECTION_NE, DIRECTION_SE, DIRECTION_SW, DIRECTION_NW};
static direction king_directions[] = {DIRECTION_N, DIRECTION_NE, DIRECTION_E, DIRECTION_SE, DIRECTION_S, DIRECTION_SW, DIRECTION_W, DIRECTION_NW};


// optimal PRNG seeds to pick the correct magics in the shortest time ~stockfish
//static int magic_seeds[RANKS] = {728, 10316, 55013, 32803, 12281, 15100, 16645, 255};


void shift_table_init(bitboard* attacks, direction* directions)
{
    for(int i = SQUARE_A1; i <= SQUARE_H8; i++)
    {
        square sq = static_cast<square>(i);

        bitboard sq_bb = bitboard_square(sq);

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

    for(int i = SQUARE_A1; i <= SQUARE_H8; i++)
    {
        square sq = static_cast<square>(i);

        bitboard sq_bb = bitboard_square(sq);
        bitboard edges = ((bitboard_rank(RANK_1) | bitboard_rank(RANK_8)) & ~bitboard_rank(square_rank(sq)))
                       | ((bitboard_file(FILE_A) | bitboard_file(FILE_H)) & ~bitboard_file(square_file(sq)));

        magics[sq].mask = bitboard_ray(sq_bb, directions[0], BITBOARD_EMPTY)
                        | bitboard_ray(sq_bb, directions[1], BITBOARD_EMPTY)
                        | bitboard_ray(sq_bb, directions[2], BITBOARD_EMPTY)
                        | bitboard_ray(sq_bb, directions[3], BITBOARD_EMPTY);
        magics[sq].mask &= ~edges;

        magics[sq].shift = SQUARES - bitboard_count(magics[sq].mask);

        // attack table of this square continues from the end of the last table
        magics[sq].attacks = sq == SQUARE_A1 ? attacks : magics[sq - 1].attacks + size;

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
    return bitboard_shift(bb, static_cast<direction>(direction_forward(s) + DIRECTION_E));
}

bitboard bitboard_pawn_west_attacks(bitboard bb, side s)
{
    return bitboard_shift(bb, static_cast<direction>(direction_forward(s) + DIRECTION_W));
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
