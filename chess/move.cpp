#include <array>
#include <string>

#include "piece.hpp"
#include "square.hpp"
#include "move.hpp"


namespace chess
{


move::move():
from{square_none},
to{square_none},
promote{piece_none}
{}

move::move(square from, square to, piece promote):
from{from},
to{to},
promote{promote}
{}

move move::from_lan(std::string_view lan)
{
	square from = square_from_san(lan.substr(0, 2));
	square to = square_from_san(lan.substr(2, 2));
	piece promote = lan.length() == 5 ? piece_from_san(lan.back()).second : piece_none;

	return move(from, to, promote);
}

std::string move::to_lan() const
{
    std::string lan = square_to_san(from) + square_to_san(to);
    if(promote != piece_none)
    {
        lan += piece_to_san(side_black, promote);
    }
    return lan;
}

bool move::is_null() const
{
    return from == square_none || to == square_none;
}


}
