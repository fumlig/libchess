#ifndef RANDOM_H
#define RANDOM_H


#include <stdint.h>
#include <stdio.h>


namespace chess
{


// pseudorandom number generator
struct random
{
    uint64_t seed;
};

void random_init(random* r, uint64_t seed);

uint64_t random_generate(random* r);

// only 1/8th bits set on average
uint64_t random_sparse(random* r);




void random_init(random* r, uint64_t seed)
{
    r->seed = seed;
}

uint64_t random_generate(random* r)
{
    r->seed ^= r->seed >> 12;
    r->seed ^= r->seed << 25;
    r->seed ^= r->seed >> 27;
    return r->seed * 2685821657736338717ULL;
}

uint64_t random_sparse(random* r)
{
    return random_generate(r) & random_generate(r) & random_generate(r);
}


}


#endif
