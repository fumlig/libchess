# libchess

Chess library with move generation and various utilities, for C++20.

## usage

```cpp
#include <chess/chess.hpp>


using namespace chess;


int main()
{
	// create start position
	position start_position;
	from_fen(fen_start, p1);

	// get legal moves
	std::vector<move> moves = p1.moves();

	// make move
	
	p1.make_move({square_a2, square_a4});



	return 0;
}
```