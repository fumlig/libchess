#include <utility>
#include <string>
#include <stdexcept>
#include <limits>

#include "side.hpp"
#include "piece.hpp"


namespace chess
{


std::pair<side, piece> piece_from_san(char san)
{
	side s = side_white;
	piece p = piece_none;

	if(std::islower(san))
	{
		s = side_black;
		san = std::toupper(san);
	}

    switch(san)
    {
    case 'P':
		p = piece_pawn;
		break;
    case 'R':
        p = piece_rook;
		break;
    case 'N':
        p = piece_knight;
		break;
    case 'B':
        p = piece_bishop;
		break;
    case 'Q':
        p = piece_queen;
		break;
    case 'K':
        p = piece_king;
		break;
    default:
		throw std::invalid_argument("san does not denote a piece");
    }

	return {s, p};
}


char piece_to_san(side s, piece p)
{
	char c;

	switch(p)
	{
	case piece_pawn:
		c = 'P';
		break;
	case piece_rook:
		c = 'R';
		break;
	case piece_knight:
		c = 'N';
		break;
	case piece_bishop:
		c = 'B';
		break;
	case piece_queen:
		c = 'Q';
		break;
	case piece_king:
		c = 'K';
		break;
	case piece_none:
	default:
        c = ' ';
        break;
	}

	if(s == side_black && p != piece_none)
	{
		c = std::tolower(c);
	}

	return c;
}


float value_of(piece p)
{
	float v = 0.0f;

	switch(p)
	{
	case piece_pawn:
		v = 1.0f;
		break;
	case piece_rook:
		v = 5.0f;
		break;
	case piece_knight:
		v = 3.0f;
		break;
	case piece_bishop:
		v = 3.0f;
		break;
	case piece_queen:
		v = 9.0f;
		break;
	case piece_king:
		v = std::numeric_limits<float>::infinity();
		break;
	case piece_none:
	default:
        v = 0.0f;
        break;	
	}

	return v;
}


}
