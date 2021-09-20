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
	chess::position p;
	from_fen(fen_start, p);

	// get legal moves
	std::vector<move> moves = p.moves();

	// make move a2a4
	p.make_move({square_a2, square_a4});

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

# time perft on start position with depth 7 (should not take more than a minute on a decent computer)
time ./build/perft startpos 7
```

If the traversal is too slow you could try increasing the table key size, but this might eat up your memory!