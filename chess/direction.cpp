#include "side.hpp"
#include "direction.hpp"


namespace chess
{


direction opposite(direction d)
{
    return static_cast<direction>(-d);
}

direction forwards(side s)
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

direction direction_of(square from, square to)
{
    return static_cast<direction>(static_cast<int>(to) - static_cast<int>(from));
}

}