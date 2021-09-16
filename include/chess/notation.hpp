#ifndef CHESS_NOTATION_HPP
#define CHESS_NOTATION_HPP


#include <array>
#include <string>
#include <sstream>
#include <cctype>
#include <ranges>
#include <unordered_map>
#include <utility>

#include "types.hpp"
#include "position.hpp"


namespace chess
{


bool from_san(std::string_view san, piece& p)
{
	if(san.length() != 1)
	{
		return false;
	}

    switch(san.front())
    {
    case 'P':
	case 'p':
		p = piece_pawn;
		break;
    case 'R':
	case 'r':
        p = piece_rook;
		break;
    case 'N':
	case 'n':
        p = piece_knight;
		break;
    case 'B':
	case 'b':
        p = piece_bishop;
		break;
    case 'Q':
	case 'q':
        p = piece_queen;
		break;
    case 'K':
	case 'k':
        p = piece_king;
		break;
    default:
		return false;
    }

	return true;
}

bool from_san(std::string_view san, side& side, piece& piece)
{
	if(san.length() != 1)
	{
		return false;
	}

	if(std::isupper(san.front()))
	{
		side = side_white;
	}
	else
	{
		side = side_black;
	}

	return from_san(san, piece);
}


std::string to_san(piece p)
{
	switch(p)
	{
	case piece_pawn:
		return "P";
	case piece_rook:
		return "R";
	case piece_knight:
		return "N";
	case piece_bishop:
		return "B";
	case piece_queen:
		return "Q";
	case piece_king:
		return "K";
	case piece_none:
	default:
		return "?";
	}
}


bool from_san(std::string_view san, file& f)
{
	if(san.length() != 1)
	{
		return false;
	}

	switch(san.front())
	{
	case 'a':
		f = file_a;
		break;
	case 'b':
		f = file_b;
		break;
	case 'c':
		f = file_c;
		break;
	case 'd':
		f = file_d;
		break;
	case 'e':
		f = file_e;
		break;
	case 'f':
		f = file_f;
		break;
	case 'g':
		f = file_g;
		break;
	case 'h':
		f = file_h;
		break;
	default:
		return false;
	}

	return true;
}


bool from_san(std::string_view san, rank& r)
{
	if(san.length() != 1)
	{
		return false;
	}

	switch(san.front())
	{
	case '1':
		r = rank_1;
		break;
	case '2':
		r = rank_2;
		break;
	case '3':
		r = rank_3;
		break;
	case '4':
		r = rank_4;
		break;
	case '5':
		r = rank_5;
		break;
	case '6':
		r = rank_6;
		break;
	case '7':
		r = rank_7;
		break;
	case '8':
		r = rank_8;
		break;
	default:
		return false;
	}

	return true;
}


bool from_san(std::string_view san, square& sq)
{
	if(san.length() != 2)
	{
		return false;
	}

	file f;
	rank r;

	if(!from_san(san.substr(0, 1), f) || !from_san(san.substr(1, 1), r))
	{
		return false;
	}

	sq = square_from_file_rank(f, r);

	return true;
}



const std::string_view fen_start = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const std::string_view fen_empty = "8/8/8/8/8/8/8/8 w - - 0 1";

bool from_fen(std::string_view fen, position& p)
{
	std::string fen_string(fen);
	std::istringstream fen_stream(fen_string);

	std::string pieces_string, turn_string, castle_string, en_passant_string;
	int halfmove_clock, fullmove_number;

	if(!(fen_stream >> pieces_string >> turn_string >> castle_string >> en_passant_string >> halfmove_clock >> fullmove_number))
	{
		return false;
	}

	// pieces
	std::unordered_map<square, std::pair<side, piece>> pieces;
	int r = rank_8;
    int f = file_a;

    for(char c : pieces_string)
    {
        if(c == '/')
        {
            r--;
            f = file_a;
            continue;
        }

        if('1' <= c && c <= '8')
        {
            f += c - '0';
            continue;
        }

        side side;
        piece piece;
		
		if(!from_san(std::string{c}, side, piece))
		{
			return false;
		}

		if(!(file_a <= f && f <= file_h) || !(rank_1 <= r && r <= rank_8))
		{
			return false;
		}

        square square = square_from_file_rank(static_cast<file>(f), static_cast<rank>(r));

		pieces[square] = {side, piece};
        
		f++;
    }

	// turn
	side turn = side_none;

	if(turn_string.length() != 1)
	{
		return false;
	}

	if(turn_string.front() == 'w')
	{
		turn = side_white;
	}
	else if(turn_string.front() == 'b')
	{
		turn = side_black;
	}
	else
	{
		return false;
	}

	// castle
	bool white_kingside_castle = false;
	bool white_queenside_castle = false;
	bool black_kingside_castle = false;
	bool black_queenside_castle = false;

    for(char c : castle_string)
    {
        switch(c)
        {
        case 'K':
            white_kingside_castle = true;
            break;
        case 'Q':
            white_queenside_castle = true;
            break;
        case 'k':
            black_kingside_castle = true;
            break;
        case 'q':
            black_queenside_castle = true;
            break;
        case '-':
			break;
        default:
			return false;
        }
    }

    // en passant
	square en_passant;

    if(en_passant_string == "-")
    {
        en_passant = square_none;
    }
    else if(!from_san(en_passant_string, en_passant))
	{
		return false;
	}

	p = position
	(
		board(pieces),
		turn,
		white_kingside_castle,
		white_queenside_castle,
		black_kingside_castle,
		black_queenside_castle,
		en_passant,
		halfmove_clock,
		fullmove_number
	);

	return true;
}


}


#endif