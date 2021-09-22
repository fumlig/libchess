# libchess

Chess library with move generation and various utilities, for C++20.

## usage

See the [documentation](docs/chess.md). Simple example:

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

## documentation

The code is documented in comments. It can also be rendered with [standardese](https://standardese.github.io/):

```bash
make docs
```

## including

The include directory in this repository contains the library. The easiest way to add it to a project and keep it updated is using git submodules:

```bash
git submodule add git@gitlab.liu.se:TDDE19-2021-1/libchess.git
```

Add the the include directory in this repository to your include path. For most compilers, like clang and g++, this is done with the `-I` flag, for example:

```bash
clang++ -Ilibchess/include <...>
```

## testing

Tests can be built and run with the provided makefile:

```bash
make test
```

### perft

A method for debugging and measuring the speed of move generation is recursively counting the number of legal moves to a certain depth in the move tree, starting at a certain position. This will be run automatically on a set of default positions when testing, but can also be run manually:

```bash
# ./build/test_perft <fen|name> <depth>
./build/test_perft startpos 5
```

If the traversal is too slow you could try increasing the table key size, but this might eat up your memory!