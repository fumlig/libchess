#ifndef CHESS_RANDOM_HPP
#define CHESS_RANDOM_HPP


#include <cstdint>


namespace chess
{


// pseudorandom number generator
struct random
{
    std::size_t seed;
};


void random_init(random* r, std::size_t seed)
{
    r->seed = seed;
}

std::size_t random_generate(random* r)
{
    r->seed ^= r->seed >> 12;
    r->seed ^= r->seed << 25;
    r->seed ^= r->seed >> 27;
    return r->seed * 2685821657736338717ULL;
}

std::size_t random_sparse(random* r)
{
    return random_generate(r) & random_generate(r) & random_generate(r);
}


}


#endif
