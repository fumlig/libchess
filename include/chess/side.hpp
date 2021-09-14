#ifndef CHESS_SIDE_HPP
#define CHESS_SIDE_HPP


namespace chess
{


#define SIDES   2


enum side
{
    SIDE_NONE   = -1,
    SIDE_WHITE  = 0,
    SIDE_BLACK  = 1,
};

side side_opposite(side s);

side side_from_char(char c);

char side_to_char(side s);



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
        return SIDE_WHITE;
    case 'B':
    case 'b':
        return SIDE_BLACK;
    default:
        return SIDE_NONE;
    }
}

char side_to_char(side s)
{
    switch(s)
    {
    case SIDE_WHITE:
        return 'w';
    case SIDE_BLACK:
        return 'b';
    case SIDE_NONE:
    default:
        return '-';
    }
}


}


#endif