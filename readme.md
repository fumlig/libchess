# libchess

Chess library with move generation and various utilities, for C++20.

## usage

```cpp
#include <chess/chess.hpp>


int main()
{
	// initialize chess library
	chess::init();

	// create start position
	chess::position p = position::from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); // fen also declared as position::fen_start

	// get legal moves
	std::vector<move> moves = p.moves();

	// make move a2a4
	p.make_move(move::from_lan());

	return 0;
}
```

## perft

A method for debugging and measuring the speed of move generation is recursively counting the number of legal moves to a certain depth in the move tree, starting at a certain position.

```bash
# build perft
./build.sh

# show usage and list positions with known results
./build/perft

# run perft with depth 5 on start position
./build/perft startpos 5
```

If the traversal is too slow you could try increasing the table key size, but this might eat up your memory!