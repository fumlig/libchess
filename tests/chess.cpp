#include <iostream>
#include <string>
#include <chess/chess.hpp>


using namespace chess;


void test(bool expr, std::string_view name)
{
	std::cerr << "chess test '" << name << "': ";
	if(expr)
	{
		std::cerr << "success" << std::endl;
	}
	else
	{
		std::cerr << "failure" << std::endl << std::endl;
		exit(EXIT_FAILURE);
	}
}


int main(int argc, char* argv[])
{
	chess::init();

	// test functionality that might not be tested by move generation
	test(opponent(opponent(side_white)) == side_white, "opponent");
	test((piece_from_san('P') == std::pair{side_white, piece_pawn}), "piece_from_san");
	test(piece_to_san(side_white, piece_pawn) == 'P', "piece_to_san");
	test(side_rank(side_black, rank_5) == rank_4, "side_rank");
	test(file_of(square_h8) == file_h, "file_of");
	test(rank_of(square_h8) == rank_8, "rank_of");
	test(cat_coords(file_e, rank_4) == square_e4, "cat_coords");
	test(file_from_san(file_to_san(file_a)) == file_a, "file_{to,from}_san");
	test(rank_from_san(rank_to_san(rank_1)) == rank_1, "rank_{to,from}_san");
	test(square_from_san(square_to_san(square_a1)) == square_a1, "square_{to,from}_san");
	test(opposite(direction_n) == direction_s, "direction");
	test(forwards(side_black) == direction_s, "forwards");
	test(square_set(square_a1) == 0b1, "square_set");
	test(file_set(file_a) == 0x0101010101010101ULL, "file_set");
	test(rank_set(rank_1) == 0xFF, "rank_set");
	test(set_contains(square_set(square_a1), square_a1), "set_contains");
	test(set_insert(empty_set, square_a1) == square_set(square_a1), "set_insert");
	test(set_erase(square_set(square_a1), square_a1) == empty_set, "set_erase");
	test(set_first(file_set(file_h)) == square_h1, "set_first");
	test(set_last(file_set(file_a)) == square_a8, "set_last");
	test(set_cardinality(universal_set) == 64, "set_cardinality");
	test(set_union(square_set(square_a1), square_set(square_b1)) == 0b11, "set_union");
	test(set_intersection(file_set(file_a), rank_set(rank_1)) == square_set(square_a1), "set_intersection");
	test(set_elements(square_set(square_e4)).front() == square_e4, "set_elements");
	test(set_shift(file_set(file_a), direction_e) == file_set(file_b), "set_shift");
	test(set_ray(square_set(square_a1), direction_e, empty_set) == set_erase(rank_set(rank_1), square_a1), "set_ray");
	test(move::from_lan("h7h8q").to_lan() == "h7h8q", "move::{from,to}_lan");
	test(position::from_fen(position::fen_start).to_fen() == position::fen_start, "position::{from,to}_fen");
	test(position::from_fen("k6R/8/8/8/8/8/8/7K b - - 0 1").is_check(), "position::is_check");
	test(position::from_fen("k6R/7R/8/8/8/8/8/7K b - - 0 1").is_checkmate(), "position::is_checkmate");
	test(position::from_fen("k7/7R/8/8/8/8/8/1R5K b - - 0 1").is_stalemate(), "position::is_stalemate");
	
	exit(EXIT_SUCCESS);
}