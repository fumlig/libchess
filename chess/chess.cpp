#include <array>

#include "chess.hpp"


namespace chess
{ 


void init(std::size_t seed)
{
    random rng(seed);
   
    attack_init(rng);
    zobrist_init(rng);
}


}