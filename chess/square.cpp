#include <string>
#include <stdexcept>

#include "side.hpp"
#include "square.hpp"


namespace chess
{


file file_from_san(char san)
{
	if(san < 'a' || san > 'h')
	{
		throw std::invalid_argument("san does not denote a file");
	}

	return static_cast<file>(san - 'a');
}


rank rank_from_san(char san)
{
	if(san < '1' || san > '8')
	{
		throw std::invalid_argument("san does not denote a file");
	}

	return static_cast<rank>(san - '1');
}


square square_from_san(std::string_view san)
{
	if(san == "-")
	{
		return square_none;
	}

	if(san.length() != 2)
	{
		throw std::invalid_argument("san for square is of wrong length");
	}

	return cat_coords(file_from_san(san[0]), rank_from_san(san[1]));
}


char file_to_san(file f)
{
	return static_cast<file>('a' + f);
}


char rank_to_san(rank r)
{
	return static_cast<rank>('1' + r);
}


std::string square_to_san(square sq)
{
	if(sq == square_none)
	{
		return "-";
	}

	return {file_to_san(file_of(sq)), rank_to_san(rank_of(sq))};
}


square flip(square sq)
{
	return static_cast<square>(sq ^ 56);
}


}