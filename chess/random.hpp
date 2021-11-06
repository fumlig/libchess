#ifndef CHESS_RANDOM_HPP
#define CHESS_RANDOM_HPP


#include <cstdint>


namespace chess
{


struct random
{
    std::size_t seed;

    random(std::size_t seed);

    std::size_t operator()();

    // Pseudorandom number generation given a seed.
    //
    // Internal function.
    // xorshift64star method. Fast and good for Zobrist hashing and other things. 
    std::size_t generate();

    // Sparse pseudorandom number given a seed.
    //
    // Internal function.
    // Achieved by taking bitwise and of multiple random numbers.
    std::size_t sparse();
};


}


#endif