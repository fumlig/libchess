#include <string>

#include "side.hpp"


namespace chess
{


std::string to_string(side s)
{
	switch(s)
	{
	case side_white:	return "white";
	case side_black:	return "black";
	default:			break;
	}

	return "none";
}


}