#include <cstdint>

#include "random.hpp"


namespace chess
{


random::random(std::size_t seed): seed{seed} {}


std::size_t random::operator()()
{
    return generate();
}


std::size_t random::generate()
{
    seed ^= seed >> 12;
    seed ^= seed << 25;
    seed ^= seed >> 27;

    return seed * 2685821657736338717ULL;
}


std::size_t random::sparse()
{
    return generate() & generate() & generate();
}


}