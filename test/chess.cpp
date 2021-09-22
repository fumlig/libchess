#include <cassert>
#include <chess/chess.hpp>

#define test(expr, name)\
	std::cerr << "chess test " << name << ": ";\
	if((expr))\
	{\
		std::cout << "success" << std::endl;\
	}\
	else\
	{\
		std::cout << "failure" << std::endl;\
		return 1;\
	}\

using namespace chess;

int main(int argc, char* argv[])
{
	test(opponent(opponent(side_white)) == side_white, "opponent");

	return 0;
}