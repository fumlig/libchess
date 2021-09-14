#ifndef CHESS_SIDE_HPP
#define CHESS_SIDE_HPP


namespace chess
{


const int sides = 2;


enum side
{
    side_none   = -1,
    side_white  = 0,
    side_black  = 1,
};


side side_opposite(side s)
{
    return static_cast<side>(!s);
}

side side_from_char(char c)
{
    switch(c)
    {
    case 'W':
    case 'w':
        return side_white;
    case 'B':
    case 'b':
        return side_black;
    default:
        return side_none;
    }
}

char side_to_char(side s)
{
    switch(s)
    {
    case side_white:
        return 'w';
    case side_black:
        return 'b';
    case side_none:
    default:
        return '-';
    }
}


}


#endif