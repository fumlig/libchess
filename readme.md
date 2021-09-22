# libchess

Chess library with move generation and various utilities, for C++20.

## usage

See the [docs](docs/chess.md). Simple example:

```cpp
#include <chess/chess.hpp>

int main()
{
	// initialize chess library
	chess::init();

	// create start position
	chess::position p = chess::position::from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); // fen also declared as position::fen_start

	// play until first check
	while(!p.is_check())
	{
		// print available moves
		std::cout << "moves: " << std::endl;
		for(chess::move& move: p.moves())
		{
			std::cout << move.to_lan() << ", ";
		}
		std::cout << std::endl;

		// select first legal move
		const chess::move move = p.moves().front();

		std::cout << "playing: " << move.to_lan() << std::endl;
		p.make_move(move);
	}

	// print final fen
	std::cout << p.to_fen() << std::endl;

	return 0;
}
```

## perft

A method for debugging and measuring the speed of move generation is recursively counting the number of legal moves to a certain depth in the move tree, starting at a certain position.

```bash
# build perft
make
# show usage and list positions with known results
./build/perft

# run perft with depth 5 on start position
./build/perft startpos 5
```

If the traversal is too slow you could try increasing the table key size, but this might eat up your memory!