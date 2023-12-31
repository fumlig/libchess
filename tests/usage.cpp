#include <iostream>
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

		// select random move
		const chess::move move = p.moves().front();

		std::cout << "playing: " << move.to_lan() << std::endl;
		p.make_move(move);
	}

	// print final fen
	std::cout << p.to_fen() << std::endl;

	std::cout << p.to_string() << std::endl;

	return 0;
}