#include <array>
#include <cstdint>

#include "side.hpp"
#include "piece.hpp"
#include "square.hpp"
#include "random.hpp"


namespace chess
{


static std::array<std::array<std::array<std::size_t, pieces>, sides>, squares> piece_keys;
static std::array<std::size_t, sides> kingside_castle_keys;
static std::array<std::size_t, sides> queenside_castle_keys;
static std::array<std::size_t, files> en_passant_keys;
static std::size_t side_key;


std::size_t zobrist_piece_key(square sq, side s, piece p)
{
	return piece_keys[sq][s][p];
}

std::size_t zobrist_kingside_castle_key(side s)
{
	return kingside_castle_keys[s];
}

std::size_t zobrist_queenside_castle_key(side s)
{
	return queenside_castle_keys[s];
}

std::size_t zobrist_en_passant_key(file f)
{
	return en_passant_keys[f];
}

std::size_t zobrist_side_key()
{
	return side_key;
}


void zobrist_init(random& rng)
{
    for(int i = square_a1; i <= square_h8; i++)
    {
        for(int j = piece_pawn; j <= piece_king; j++)
        {
            square sq = static_cast<square>(i);
            piece p = static_cast<piece>(j);

            piece_keys[sq][side_white][p] = rng();
            piece_keys[sq][side_black][p] = rng();
        }
    }

    kingside_castle_keys[side_white] = rng();
    kingside_castle_keys[side_black] = rng();
    queenside_castle_keys[side_white] = rng();
    queenside_castle_keys[side_black] = rng();

    for(int f = file_a; f <= file_h; f++)
    {
        en_passant_keys[f] = rng();
    }

    side_key = rng();
}


}
