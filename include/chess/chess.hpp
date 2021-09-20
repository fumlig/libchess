#ifndef CHESS_HPP
#define CHESS_HPP


#include <array>
#include <bit>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>


namespace chess
{


enum side
{
    side_white,
    side_black,
    side_none = -1,
};

const int sides = 2;


constexpr side opponent(side s)
{
    return static_cast<side>(!s);
}


enum piece
{
    piece_pawn,
    piece_rook,
    piece_knight,
    piece_bishop,
    piece_queen,
    piece_king,
    piece_none = -1,
};

const int pieces = 6;


std::pair<side, piece> piece_from_san(char san)
{
	side s = side_white;
	piece p = piece_none;

	if(std::islower(san))
	{
		s = side_black;
		san = std::toupper(san);
	}

    switch(san)
    {
    case 'P':
		p = piece_pawn;
		break;
    case 'R':
        p = piece_rook;
		break;
    case 'N':
        p = piece_knight;
		break;
    case 'B':
        p = piece_bishop;
		break;
    case 'Q':
        p = piece_queen;
		break;
    case 'K':
        p = piece_king;
		break;
    default:
		throw std::invalid_argument("san does not denote a piece");
    }

	return {s, p};
}

char piece_to_san(side s, piece p)
{
	char c;

	switch(p)
	{
	case piece_pawn:
		c = 'P';
	case piece_rook:
		c = 'R';
	case piece_knight:
		return 'N';
	case piece_bishop:
		return 'B';
	case piece_queen:
		return 'Q';
	case piece_king:
		return 'K';
	case piece_none:
	default:
		throw std::invalid_argument("no piece, no san");
	}

	if(s == side_black)
	{
		c = std::tolower(c);
	}

	return c;
}


enum file
{
    file_a, file_b, file_c, file_d, file_e, file_f, file_g, file_h
};

const int files = 8;


enum rank
{
    rank_1, rank_2, rank_3, rank_4, rank_5, rank_6, rank_7, rank_8
};

const int ranks = 8;


enum square
{
    square_a1, square_b1, square_c1, square_d1, square_e1, square_f1, square_g1, square_h1,
    square_a2, square_b2, square_c2, square_d2, square_e2, square_f2, square_g2, square_h2, 
    square_a3, square_b3, square_c3, square_d3, square_e3, square_f3, square_g3, square_h3,
    square_a4, square_b4, square_c4, square_d4, square_e4, square_f4, square_g4, square_h4,
    square_a5, square_b5, square_c5, square_d5, square_e5, square_f5, square_g5, square_h5,
    square_a6, square_b6, square_c6, square_d6, square_e6, square_f6, square_g6, square_h6,
    square_a7, square_b7, square_c7, square_d7, square_e7, square_f7, square_g7, square_h7,
    square_a8, square_b8, square_c8, square_d8, square_e8, square_f8, square_g8, square_h8,
    square_none = -1,
};

const int squares = 64;


constexpr rank side_rank(side s, rank r)
{
    return static_cast<rank>(r*static_cast<int>(opponent(s)) + (rank_8-r)*s);
}

constexpr file file_of(square sq)
{
    return static_cast<file>(sq % 8);
}

constexpr rank rank_of(square sq)
{
    return static_cast<rank>(sq / 8);
}

constexpr square cat_coords(file f, rank r)
{
    return static_cast<square>(r*8 + f);
}


file file_from_san(char san)
{
	if(san < 'a' || san > 'h')
	{
		throw std::invalid_argument("san does not denote a file");
	}

	return static_cast<file>(san - 'a');
}


rank rank_from_san(char san)
{
	if(san < '1' || san > '8')
	{
		throw std::invalid_argument("san does not denote a file");
	}

	return static_cast<rank>(san - '1');
}


square square_from_san(std::string_view san)
{
	if(san == "-")
	{
		return square_none;
	}

	if(san.length() != 2)
	{
		throw std::invalid_argument("san for square is of wrong length");
	}

	return cat_coords(file_from_san(san[0]), rank_from_san(san[1]));
}


char file_to_san(file f)
{
	return static_cast<file>('a' + f);
}

char rank_to_san(rank r)
{
	return static_cast<rank>('1' + r);
}


std::string square_to_san(square sq)
{
	if(sq == square_none)
	{
		return "-";
	}

	return {file_to_san(file_of(sq)), rank_to_san(rank_of(sq))};
}





const int directions = 16;

enum direction
{
    direction_n     = 8,
    direction_e     = 1,
    direction_s     = -8,
    direction_w     = -1,
    
	direction_ne    = direction_n + direction_e,
    direction_se    = direction_s + direction_e,
    direction_sw    = direction_s + direction_w,
    direction_nw    = direction_n + direction_w,
    
	direction_nne   = direction_n + direction_n + direction_e,
    direction_ene   = direction_e + direction_n + direction_e,
    direction_ese   = direction_e + direction_s + direction_e,
    direction_sse   = direction_s + direction_s + direction_e,
    direction_ssw   = direction_s + direction_s + direction_w,
    direction_wsw   = direction_w + direction_s + direction_w,
    direction_wnw   = direction_w + direction_n + direction_w,
    direction_nnw   = direction_n + direction_n + direction_w,

    direction_none  = 0,
};

constexpr direction opposite(direction d)
{
    return static_cast<direction>((d + 8) % 16);
}

constexpr direction forwards(side s)
{
    switch(s)
    {
    case side_white:
        return direction_n;
    case side_black:
        return direction_s;
    case side_none:
    default:
        return direction_none;
    }
}


using bitboard = std::uint64_t;

const bitboard bitboard_empty = 0ULL;
const bitboard bitboard_full = ~0ULL;

constexpr bitboard square_mask(square sq)
{
    return 1ULL << sq;
}

constexpr bitboard file_mask(file f)
{
    return 0x0101010101010101ULL << f;
}

constexpr bitboard rank_mask(rank r)
{
    return 0xFFULL << (r*8);
}

inline bool test_square(bitboard bb, square sq)
{
    return bb & square_mask(sq);
}

inline bitboard set_square(bitboard bb, square sq)
{
    return bb | square_mask(sq);
}

inline bitboard toggle_square(bitboard bb, square sq)
{
    return bb ^ square_mask(sq);
}

inline bitboard unset_square(bitboard bb, square sq)
{
    return bb & ~square_mask(sq);
}

// used very often
inline bitboard bitboard_shift(bitboard bb, direction d)
{
    if(d > 0)
    {
        bb <<= d;
    }
    else
    {
        bb >>= -d;
    }

    bitboard trim = bitboard_empty;

    switch(d)
    {
    case direction_e:
    case direction_ne:
    case direction_se:
    case direction_nne:
    case direction_sse:
        trim = file_mask(file_a);
        break;
    case direction_w:
    case direction_nw:
    case direction_sw:
    case direction_nnw:
    case direction_ssw:
        trim = file_mask(file_h);
        break;
    case direction_ene:
    case direction_ese:
        trim = file_mask(file_a) | file_mask(file_b);
        break;
    case direction_wnw:
    case direction_wsw:
        trim = file_mask(file_g) | file_mask(file_h);
        break;
    default:
        break;
    }

    return bb & ~trim;
}


inline bitboard bitboard_ray(bitboard bb, direction d, bitboard occupied)
{
    bitboard shift = bb;
    bitboard ray = 0;
    while(shift != 0 && !(shift & occupied))
    {
        shift = bitboard_shift(shift, d);
        ray |= shift;
    }
    return ray;
}

inline square first_set_square(bitboard bb)
{
    return static_cast<square>(std::countr_zero(bb));
}

inline square last_set_square(bitboard bb)
{
    return static_cast<square>(square_h8 - std::countl_zero(bb));
}

inline int count_set_squares(bitboard bb)
{
    return std::popcount(bb);
}


struct magic
{
    bitboard mask;
    bitboard magic;
    bitboard* attacks;
    unsigned shift;
};

inline unsigned magic_index(const magic& m, bitboard occupied)
{
    return ((occupied & m.mask) * m.magic) >> m.shift;
}


inline std::size_t random_generate(std::size_t& seed)
{
    seed ^= seed >> 12;
    seed ^= seed << 25;
    seed ^= seed >> 27;

    return seed * 2685821657736338717ULL;
}

inline std::size_t random_sparse(std::size_t& seed)
{
    return random_generate(seed) & random_generate(seed) & random_generate(seed);
}



inline std::array<std::array<std::array<std::size_t, pieces>, sides>, squares> pieces_zobrist_hash;
inline std::size_t side_zobrist_hash;
inline std::array<std::size_t, sides> kingside_castle_zobrist_hash;
inline std::array<std::size_t, sides> queenside_castle_zobrist_hash;
inline std::array<std::size_t, files> en_passant_zobrist_hash;


inline std::array<magic, squares> rook_magics;
inline std::array<magic, squares> bishop_magics;

inline std::array<bitboard, 0x19000> rook_attacks;
inline std::array<bitboard, squares> knight_attacks;
inline std::array<bitboard, 0x1480> bishop_attacks;
inline std::array<bitboard, squares> king_attacks;


inline bitboard pawn_east_attack_mask(bitboard bb, side s)
{
    return bitboard_shift(bb, static_cast<direction>(forwards(s) + direction_e));
}

inline bitboard pawn_west_attack_mask(bitboard bb, side s)
{
    return bitboard_shift(bb, static_cast<direction>(forwards(s) + direction_w));
}

inline bitboard rook_attack_mask(square sq, bitboard occupied)
{
    unsigned index = magic_index(rook_magics[sq], occupied);
    return rook_magics[sq].attacks[index];
}

inline bitboard knight_attack_mask(square sq)
{
    return knight_attacks[sq];
}

inline bitboard bishop_attack_mask(square sq, bitboard occupied)
{
    unsigned index = magic_index(bishop_magics[sq], occupied);
    return bishop_magics[sq].attacks[index];
}

inline bitboard queen_attack_mask(square sq, bitboard occupied)
{
    return rook_attack_mask(sq, occupied) | bishop_attack_mask(sq, occupied);
}

inline bitboard king_attack_mask(square sq)
{
    return king_attacks[sq];
}



void shift_table_init(bitboard* attacks, const std::array<direction, 8>& directions)
{
    for(int i = square_a1; i <= square_h8; i++)
    {
        square sq = static_cast<square>(i);

        bitboard sq_bb = square_mask(sq);

        // initialize knight attacks table
        attacks[sq] = bitboard_shift(sq_bb, directions[0])
                    | bitboard_shift(sq_bb, directions[1])
                    | bitboard_shift(sq_bb, directions[2])
                    | bitboard_shift(sq_bb, directions[3])
                    | bitboard_shift(sq_bb, directions[4])
                    | bitboard_shift(sq_bb, directions[5])
                    | bitboard_shift(sq_bb, directions[6])
                    | bitboard_shift(sq_bb, directions[7]);
    }
}

void ray_table_init(bitboard* attacks, std::array<magic, squares>& magics, const std::array<direction, 4>& directions, std::size_t& seed)
{
    bitboard occupancy[4096];
    bitboard reference[4096];
    int epoch[4096] = {0};
    int count = 0;
    int size = 0;

    for(int i = square_a1; i <= square_h8; i++)
    {
        square sq = static_cast<square>(i);

        bitboard sq_bb = square_mask(sq);
        bitboard edges = ((rank_mask(rank_1) | rank_mask(rank_8)) & ~rank_mask(rank_of(sq)))
                       | ((file_mask(file_a) | file_mask(file_h)) & ~file_mask(file_of(sq)));

        magics[sq].mask = bitboard_ray(sq_bb, directions[0], bitboard_empty)
                        | bitboard_ray(sq_bb, directions[1], bitboard_empty)
                        | bitboard_ray(sq_bb, directions[2], bitboard_empty)
                        | bitboard_ray(sq_bb, directions[3], bitboard_empty);
        magics[sq].mask &= ~edges;

        magics[sq].shift = squares - count_set_squares(magics[sq].mask);

        // attack table of this square continues from the end of the last table
        magics[sq].attacks = sq == square_a1 ? attacks : magics[sq - 1].attacks + size;

        bitboard bb = 0;
        size = 0;

        // store all relevant occupancies and their corresponding attack table entry
        do
        {
            // bitboard never becomes zero...
            occupancy[size] = bb;
            // actual attacks
            reference[size] = bitboard_ray(sq_bb, directions[0], bb)
                            | bitboard_ray(sq_bb, directions[1], bb)
                            | bitboard_ray(sq_bb, directions[2], bb)
                            | bitboard_ray(sq_bb, directions[3], bb);

            size++;
            bb = (bb - magics[sq].mask) & magics[sq].mask;
        } while (bb);
        
        // initialize pseudo-random number generator
        //int seed = magic_seeds[square_rank(sq)];
        //random r;
        //random_init(&r, seed);

        // randomize until magic number that works is found
        int j = 0;
        while(j < size)
        {
            magics[sq].magic = 0;

            // randomize magic number
            while(count_set_squares((magics[sq].magic * magics[sq].mask) >> 56) < 6) magics[sq].magic = random_sparse(seed);

            // make sure magic number maps every possible occupancy to index
            // that looks up the correct sliding attack in attacks database

            // attacks database is built up alongside verifying the magic number
            // use epoch to avoid resetting the attack table on failure
            for (++count, j = 0; j < size; ++j)
            {
                unsigned index = magic_index(magics[sq], occupancy[j]);
                if (epoch[index] < count)
                {
                    epoch[index] = count;
                    magics[sq].attacks[index] = reference[j];
                }
                else if (magics[sq].attacks[index] != reference[j]) break;
            }
        }
    }
}


class board
{
public:
    board():
    square_sides{},
    square_pieces{},
    side_masks{},
    piece_masks{},
    zobrist_hash{0}
    {
        square_sides.fill(side_none);
        square_pieces.fill(piece_none);
        side_masks.fill(bitboard_empty);
        piece_masks.fill(bitboard_empty);
    }

    board(const std::unordered_map<square, std::pair<side, piece>>& pieces):
    board()
    {
        for(auto [sq, sp]: pieces)
        {
            auto [s, p] = sp;
            set(sq, s, p);
        }
    }

    std::pair<side, piece> get(square sq) const
    {
        return {square_sides[sq], square_pieces[sq]};
    }

    void set(square sq, side s, piece p)
    {
        side s_prev = square_sides[sq];
        piece p_prev = square_pieces[sq];

        square_sides[sq] = s;
        square_pieces[sq] = p;

        if(s_prev != side_none && p_prev != piece_none)
        {
            side_masks[s_prev] = unset_square(side_masks[s_prev], sq);
            piece_masks[p_prev] = unset_square(piece_masks[p_prev], sq);
            zobrist_hash ^= pieces_zobrist_hash[sq][s_prev][p_prev];
        }

        if(s != side_none && p != piece_none)
        {
            side_masks[s] = set_square(side_masks[s], sq);
            piece_masks[p] = set_square(piece_masks[p], sq);
            zobrist_hash ^= pieces_zobrist_hash[sq][s][p];
        }
    }

    void clear()
    {
        for(int s = square_a1; s <= square_h8; s++)
        {
            square_sides[s] = side_none;
            square_pieces[s] = piece_none;
        }

        for(int p = piece_pawn; p <= piece_king; p++)
        {
            piece_masks[p] = 0;
        }
        
        side_masks[side_white] = 0;
        side_masks[side_black] = 0;

        zobrist_hash = 0;
    }


    inline bitboard side_mask(side s) const
    {
        return side_masks[s];
    }

    inline bitboard piece_mask(piece p) const
    {
        return piece_masks[p];
    }

    inline bitboard side_piece_mask(side s, piece p) const
    {
        return side_masks[s] & piece_masks[p];
    }

    inline bitboard empty_mask() const
    {
        return ~(side_masks[side_white] | side_masks[side_black]);
    }

    inline bitboard occupied_mask() const
    {
        return side_masks[side_white] | side_masks[side_black];
    }


    bitboard attack_mask(side s) const
    {
        bitboard pawns = side_piece_mask(s, piece_pawn);
        bitboard rooks = side_piece_mask(s, piece_rook);
        bitboard knights = side_piece_mask(s, piece_knight);
        bitboard bishops = side_piece_mask(s, piece_bishop);
        bitboard queens = side_piece_mask(s, piece_queen);
        bitboard kings = side_piece_mask(s, piece_king);

        bitboard attacks = 0;

        attacks |= bitboard_shift(pawns, static_cast<direction>(forwards(s) + direction_e));
        attacks |= bitboard_shift(pawns, static_cast<direction>(forwards(s) + direction_w));

        while(rooks)
        {
            square from = first_set_square(rooks);
            rooks = unset_square(rooks, from);
            attacks |= rook_attack_mask(from, occupied_mask());
        }

        while(knights)
        {
            square from = first_set_square(knights);
            knights = unset_square(knights, from);
            attacks |= knight_attack_mask(from);
        }

        while(bishops)
        {
            square from = first_set_square(bishops);
            bishops = unset_square(bishops, from);
            attacks |= bishop_attack_mask(from, occupied_mask());
        }

        while(queens)
        {
            square from = first_set_square(queens);
            queens = unset_square(queens, from);
            attacks |= (rook_attack_mask(from, occupied_mask()) | bishop_attack_mask(from, occupied_mask()));
        }

        while(kings)
        {
            square from = first_set_square(kings);
            kings = unset_square(kings, from);
            attacks |= king_attack_mask(from);
        }

        return attacks;
    }

    inline std::size_t hash() const
    {
        return zobrist_hash;
    }

private:
    std::array<side, squares> square_sides;
    std::array<piece, squares> square_pieces;

    std::array<bitboard, sides> side_masks;
    std::array<bitboard, pieces> piece_masks;

    std::size_t zobrist_hash;
};


struct move
{
	// Null-move
	move():
	from{square_none},
	to{square_none},
	promote{piece_none}
	{}

	move(square from, square to, piece promote = piece_none):
	from{from},
	to{to},
	promote{promote}
	{}

	// todo: implement
	/*
	static move from_san(std::string_view san, const position& p)
	{

	}
	*/

	static move from_lan(std::string_view lan)
	{
		square from = square_from_san(lan.substr(0, 2));
		square to = square_from_san(lan.substr(2, 2));
		piece promote = lan.length() == 5 ? piece_from_san(lan.back()).second : piece_none;

		return move(from, to, promote);
	}

	std::string to_lan() const
	{
		return square_to_san(from) + square_to_san(to) + (promote != piece_none ? std::to_string(piece_to_san(side_none, promote)) : "");
	}

    square from;
    square to;
    piece promote;
};

struct undo
{
    piece capture;
    square en_passant;
    std::array<bool, sides> kingside_castle;
    std::array<bool, sides> queenside_castle;
};


class position
{
public:
    position():
    pieces(),
    turn{side_white},
    kingside_castle{true, true},
    queenside_castle{true, true},
    en_passant{square_none},
    halfmove_clock{0},
    fullmove_number{1},
    zobrist_hash{0}
    {}

    position
    (
        board&& pieces, 
        side turn = side_white,
        bool white_kingside_castle = true,
        bool white_queenside_castle = true,
        bool black_kingside_castle = true,
        bool black_queenside_castle = true,
        square en_passant = square_none,
        int halfmove_clock = 0,
        int fullmove_number = 1
    ):
    pieces(pieces),
    turn{turn},
    kingside_castle{white_kingside_castle, black_kingside_castle},
    queenside_castle{white_queenside_castle, black_queenside_castle},
    en_passant{en_passant},
    halfmove_clock{halfmove_clock},
    fullmove_number{fullmove_number},
    zobrist_hash{0}
    {
        if(turn == side_black)              zobrist_hash ^= side_zobrist_hash;
        if(kingside_castle[side_white])     zobrist_hash ^= kingside_castle_zobrist_hash[side_white];
        if(queenside_castle[side_white])    zobrist_hash ^= queenside_castle_zobrist_hash[side_white];
        if(kingside_castle[side_black])     zobrist_hash ^= kingside_castle_zobrist_hash[side_black];
        if(queenside_castle[side_black])    zobrist_hash ^= queenside_castle_zobrist_hash[side_black];
        if(en_passant != square_none)       zobrist_hash ^= en_passant_zobrist_hash[file_of(en_passant)];
    }


	static const inline std::string_view fen_start = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	static const inline std::string_view fen_empty = "8/8/8/8/8/8/8/8 w - - 0 1";


	static position from_fen(std::string_view fen)
	{
		std::string fen_string(fen);
		std::istringstream fen_stream(fen_string);

		std::string pieces_string, turn_string, castle_string, en_passant_string;
		int halfmove_clock, fullmove_number;

		if(!(fen_stream >> pieces_string >> turn_string >> castle_string >> en_passant_string >> halfmove_clock >> fullmove_number))
		{
			throw std::invalid_argument("fen does not contain 7 fields");
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

			if(!(file_a <= f && f <= file_h) || !(rank_1 <= r && r <= rank_8))
			{
				throw std::invalid_argument("fen contains ill-formed piece placement field");
			}

			pieces[cat_coords(static_cast<file>(f), static_cast<rank>(r))] = piece_from_san(c);
	        
			f++;
	    }

		// turn
		side turn = side_none;

		if(turn_string.length() != 1)
		{
			throw std::invalid_argument("fen contains ill-formed piece turn field");
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
			throw std::invalid_argument("fen contains ill-formed piece turn field");
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
				throw std::invalid_argument("fen contains ill-formed castling availability field");
	        }
	    }

	    // en passant
		square en_passant = square_none;

	    if(en_passant_string != "-")
	    {
			en_passant = square_from_san(en_passant_string);
	    }

		return position
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
	}


    undo make_move(const move& m)
    {
		piece capture = pieces.get(m.to).second;
        undo u{capture, en_passant, kingside_castle, queenside_castle};

        auto [side, piece] = pieces.get(m.from);
        square ep = en_passant;

        pieces.set(m.from, side_none, piece_none);

        if(m.promote != piece_none) pieces.set(m.to, side, m.promote);
        else pieces.set(m.to, side, piece);

        en_passant = square_none;
        if(ep != square_none) zobrist_hash ^= en_passant_zobrist_hash[file_of(ep)];

        if(piece == piece_pawn)
        {
            if(rank_of(m.from) == side_rank(side, rank_2) && rank_of(m.to) == side_rank(side, rank_4))
            {
                // double push
                en_passant = cat_coords(file_of(m.from), side_rank(side, rank_3));
                zobrist_hash ^= en_passant_zobrist_hash[file_of(en_passant)];
            }
            else if(m.to == ep)
            {
                // en passant capture
                square ep_capture = cat_coords(file_of(ep), side_rank(side, rank_5));
                pieces.set(ep_capture, side_none, piece_none);
            }
        }
        else if(piece == piece_king)
        {
            if(kingside_castle[side])
            {
                kingside_castle[side] = false;
                zobrist_hash ^= kingside_castle_zobrist_hash[side];
            }
            if(queenside_castle[side])
            {
                queenside_castle[side] = false;
                zobrist_hash ^= queenside_castle_zobrist_hash[side];
            }
            
            rank rank_first = side_rank(side, rank_1);

            if(m.from == cat_coords(file_e, rank_first))
            {
                if(m.to == cat_coords(file_g, rank_first))
                {
                    pieces.set(cat_coords(file_h, rank_first), side_none, piece_none);
                    pieces.set(cat_coords(file_f, rank_first), side, piece_rook);
                }
                else if(m.to == cat_coords(file_c, rank_first))
                {
                    pieces.set(cat_coords(file_a, rank_first), side_none, piece_none);
                    pieces.set(cat_coords(file_d, rank_first), side, piece_rook);
                }
            }
        }

        // todo: these (and en passant) should only be updated if they changed from the previous move
        // can probably be done by xoring with current state before each move

        if(queenside_castle[side_white] && (m.from == square_a1 || m.to == square_a1))
        {
            queenside_castle[side_white] = false;
            zobrist_hash ^= queenside_castle_zobrist_hash[side_white];
        }
        if(kingside_castle[side_white] && (m.from == square_h1 || m.to == square_h1))
        {
            kingside_castle[side_white] = false;
            zobrist_hash ^= kingside_castle_zobrist_hash[side_white];
        }
        if(queenside_castle[side_black] && (m.from == square_a8 || m.to == square_a8))
        {
            queenside_castle[side_black] = false;
            zobrist_hash ^= queenside_castle_zobrist_hash[side_black];
        }
        if(kingside_castle[side_black] && (m.from == square_h8 || m.to == square_h8))
        {
            kingside_castle[side_black] = false;
            zobrist_hash ^= kingside_castle_zobrist_hash[side_black];
        }

        halfmove_clock += (piece == piece_pawn || capture != piece_none);
        fullmove_number += turn;
        turn = opponent(turn);
        zobrist_hash ^= side_zobrist_hash;

        return u;
    }


    void undo_move(const move& m, const undo& u)
    {
        auto [side, piece] = pieces.get(m.to);

        pieces.set(m.from, side, piece);
        pieces.set(m.to, side_none, piece_none);
        if(u.capture != piece_none) pieces.set(m.to, opponent(side), u.capture);
        if(m.promote != piece_none) pieces.set(m.from, side, piece_pawn);

        if(en_passant != square_none) zobrist_hash ^= en_passant_zobrist_hash[file_of(en_passant)];
        if(u.en_passant != square_none) zobrist_hash ^= en_passant_zobrist_hash[file_of(u.en_passant)];
        en_passant = u.en_passant;

        if(kingside_castle[side_white] != u.kingside_castle[side_white])
        {
            kingside_castle[side_white] = u.kingside_castle[side_white];
            zobrist_hash ^= kingside_castle_zobrist_hash[side_white];
        }
        if(kingside_castle[side_black] != u.kingside_castle[side_black])
        {
            kingside_castle[side_black] = u.kingside_castle[side_black];
            zobrist_hash ^= kingside_castle_zobrist_hash[side_black];
        }
        if(queenside_castle[side_white] != u.queenside_castle[side_white])
        {
            queenside_castle[side_white] = u.queenside_castle[side_white];
            zobrist_hash ^= queenside_castle_zobrist_hash[side_white];
        }
        if(queenside_castle[side_black] != u.queenside_castle[side_black])
        {
            queenside_castle[side_black] = u.queenside_castle[side_black];
            zobrist_hash ^= queenside_castle_zobrist_hash[side_black];
        }

        if(piece == piece_pawn)
        {
            if(m.to == u.en_passant)
            {
                square ep_capture = cat_coords(file_of(u.en_passant), side_rank(side, rank_5));
                pieces.set(ep_capture, opponent(side), piece_pawn);
            }
        }
        else if(piece == piece_king)
        {
            rank rank_first = side_rank(side, rank_1);

            if(m.from == cat_coords(file_e, rank_first))
            {
                if(m.to == cat_coords(file_g, rank_first))
                {
                    pieces.set(cat_coords(file_h, rank_first), side, piece_rook);
                    pieces.set(cat_coords(file_f, rank_first), side_none, piece_none);
                }
                else if(m.to == cat_coords(file_c, side_rank(side, rank_1)))
                {
                    pieces.set(cat_coords(file_a, rank_first), side, piece_rook);
                    pieces.set(cat_coords(file_d, rank_first), side_none, piece_none);
                }
            }
        }

        halfmove_clock--; // todo: only if silent move
        fullmove_number -= opponent(turn); // decrease if white
        turn = opponent(turn);
        zobrist_hash ^= side_zobrist_hash;
    }


    position copy_move(const move& m) const
    {
        position p = *this;
        p.make_move(m);
        return p;
    }


    std::vector<move> moves() const
    {
        std::vector<move> moves;

        bitboard occupied = pieces.occupied_mask();
        
        bitboard pawns = pieces.side_piece_mask(turn, piece_pawn);
        bitboard rooks = pieces.side_piece_mask(turn, piece_rook);
        bitboard knights = pieces.side_piece_mask(turn, piece_knight);
        bitboard bishops = pieces.side_piece_mask(turn, piece_bishop);
        bitboard queens = pieces.side_piece_mask(turn, piece_queen);
        bitboard kings = pieces.side_piece_mask(turn, piece_king);
        
        bitboard attack_mask = ~pieces.side_mask(turn);
        bitboard capture_mask = pieces.side_mask(opponent(turn));
        bitboard ep_mask = 0;
        if(en_passant != square_none) ep_mask = square_mask(en_passant);

        // pawn moves
        bitboard single_push_tos = bitboard_shift(pawns, forwards(turn)) & ~occupied;
        bitboard single_push_froms = bitboard_shift(single_push_tos, forwards(opponent(turn)));
        bitboard double_push_tos = bitboard_shift(single_push_tos & rank_mask(side_rank(turn, rank_3)), forwards(turn)) & ~occupied;
        bitboard double_push_froms = bitboard_shift(bitboard_shift(double_push_tos, forwards(opponent(turn))), forwards(opponent(turn)));

        bitboard attack_east_tos = bitboard_shift(pawns, static_cast<direction>(forwards(turn) + direction_e)) & (capture_mask | ep_mask);
        bitboard attack_east_froms = bitboard_shift(attack_east_tos, static_cast<direction>(forwards(opponent(turn)) + direction_w));
        bitboard attack_west_tos = bitboard_shift(pawns, static_cast<direction>(forwards(turn) + direction_w)) & (capture_mask | ep_mask);
        bitboard attack_west_froms = bitboard_shift(attack_west_tos, static_cast<direction>(forwards(opponent(turn)) + direction_e));
        
        bitboard promote_push_tos = single_push_tos & rank_mask(side_rank(turn, rank_8));
        bitboard promote_push_froms = single_push_froms & rank_mask(side_rank(turn, rank_7));
        bitboard promote_east_tos = attack_east_tos & rank_mask(side_rank(turn, rank_8));
        bitboard promote_east_froms = attack_east_froms & rank_mask(side_rank(turn, rank_7));
        bitboard promote_west_tos = attack_west_tos & rank_mask(side_rank(turn, rank_8));
        bitboard promote_west_froms = attack_west_froms & rank_mask(side_rank(turn, rank_7));;

        single_push_tos ^= promote_push_tos;
        single_push_froms ^= promote_push_froms;
        attack_east_tos ^= promote_east_tos;
        attack_east_froms ^= promote_east_froms;
        attack_west_tos ^= promote_west_tos;
        attack_west_froms ^= promote_west_froms;

        setwise_moves(single_push_froms, single_push_tos, piece_none, moves);
        setwise_moves(double_push_froms, double_push_tos, piece_none, moves);
        
        setwise_moves(attack_east_froms, attack_east_tos, piece_none, moves);
        setwise_moves(attack_west_froms, attack_west_tos, piece_none, moves);
        
        setwise_moves(promote_push_froms, promote_push_tos, piece_rook, moves);
        setwise_moves(promote_push_froms, promote_push_tos, piece_knight, moves);
        setwise_moves(promote_push_froms, promote_push_tos, piece_bishop, moves);
        setwise_moves(promote_push_froms, promote_push_tos, piece_queen, moves);

        setwise_moves(promote_east_froms, promote_east_tos, piece_rook, moves);
        setwise_moves(promote_east_froms, promote_east_tos, piece_knight, moves);
        setwise_moves(promote_east_froms, promote_east_tos, piece_bishop, moves);
        setwise_moves(promote_east_froms, promote_east_tos, piece_queen, moves);

        setwise_moves(promote_west_froms, promote_west_tos, piece_rook, moves);
        setwise_moves(promote_west_froms, promote_west_tos, piece_knight, moves);
        setwise_moves(promote_west_froms, promote_west_tos, piece_bishop, moves);
        setwise_moves(promote_west_froms, promote_west_tos, piece_queen, moves);

        // rook moves
        while(rooks)
        {
            square from = first_set_square(rooks);
            rooks = unset_square(rooks, from);
            bitboard attacks = rook_attack_mask(from, occupied) & attack_mask;
            piecewise_moves(from, attacks, piece_none, moves);
        }

        // knight moves
        while(knights)
        {
            square from = first_set_square(knights);
            knights = unset_square(knights, from);
            bitboard attacks = knight_attack_mask(from) & attack_mask;
            piecewise_moves(from, attacks, piece_none, moves);
        }

        // bishop moves
        while(bishops)
        {
            square from = first_set_square(bishops);
            bishops = unset_square(bishops, from);
            bitboard attacks = bishop_attack_mask(from, occupied) & attack_mask;
            piecewise_moves(from, attacks, piece_none, moves);
        }

        // queen moves
        while(queens)
        {
            square from = first_set_square(queens);
            queens = unset_square(queens, from);
            bitboard attacks = (rook_attack_mask(from, occupied) | bishop_attack_mask(from, occupied)) & attack_mask;
            piecewise_moves(from, attacks, piece_none, moves);
        }

        // king moves
        if(kingside_castle[turn])
        {
            square from = first_set_square(kings);
            square to = cat_coords(file_g, rank_of(from));
            bitboard path = kings;
            path |= bitboard_shift(path, direction_e);
            path |= bitboard_shift(path, direction_e);
            bitboard between = path & ~kings;

            if(!(between & occupied) && !(path & pieces.attack_mask(opponent(turn))))
            {
                moves.emplace_back(from, to, piece_none);
            }
        }
        if(queenside_castle[turn])
        {
            square from = first_set_square(kings);
            square to = cat_coords(file_c, rank_of(from));
            bitboard path = kings;
            path |= bitboard_shift(path, direction_w);
            path |= bitboard_shift(path, direction_w);
            bitboard between = bitboard_shift(path, direction_w);

            if(!(between & occupied) && !(path & pieces.attack_mask(opponent(turn))))
            {
                moves.emplace_back(from, to, piece_none);
            }
        }
        while(kings)
        {
            square from = first_set_square(kings);
            kings = unset_square(kings, from);
            bitboard attacks = king_attack_mask(from) & attack_mask;
            piecewise_moves(from, attacks, piece_none, moves);
        }

        // remove illegal moves
        side player = turn;
        int n = 0;
        for(move& m: moves)
        {
            position p = copy_move(m);;

            if(!(p.pieces.attack_mask(opponent(player)) & p.pieces.side_piece_mask(player, piece_king)))
            {
                moves[n++] = m;
            }
        }

        moves.resize(n);

        return moves;
    }

    int fullmove() const
    {
        return fullmove_number;
    }

    int halfmove() const
    {
        return (fullmove_number - 1)*2 + turn;
    }

    std::size_t hash() const
    {
        return zobrist_hash ^ pieces.hash();
    }

    bool is_check() const
    {
        return pieces.attack_mask(opponent(turn)) & pieces.side_piece_mask(turn, piece_king);
    }

    bool is_checkmate() const
    {
        return is_check() && moves().empty();
    }

    bool is_stalemate() const
    {
        return !is_check() && moves().empty();
    }


private:
    inline void piecewise_moves(square from, bitboard tos, piece promote, std::vector<move>& moves) const
    {
        while(tos)
        {
            square to = first_set_square(tos);
            tos = unset_square(tos, to);
            moves.emplace_back(from, to, promote);
        }
    }

    inline void setwise_moves(bitboard froms, bitboard tos, piece promote, std::vector<move>& moves) const
    {
        while(froms && tos)
        {
            square from = first_set_square(froms);
            square to = first_set_square(tos);
            froms = unset_square(froms, from);
            tos = unset_square(tos, to);
            moves.emplace_back(from, to, promote);
        }
    }

    board pieces;
    side turn;
    std::array<bool, sides> kingside_castle;
    std::array<bool, sides> queenside_castle;
    square en_passant;
    int halfmove_clock;
    int fullmove_number;
    std::size_t zobrist_hash;
};


void init(std::size_t seed = 2147483647ULL)
{
	const std::array<direction, 4> rook_directions{direction_n, direction_e, direction_s, direction_w};
	const std::array<direction, 4> bishop_directions{direction_ne, direction_se, direction_sw, direction_nw};
	const std::array<direction, 8> knight_directions{direction_nne, direction_ene, direction_ese, direction_sse, direction_ssw, direction_wsw, direction_wnw, direction_nnw};
	const std::array<direction, 8> king_directions{direction_n, direction_ne, direction_e, direction_se, direction_s, direction_sw, direction_w, direction_nw};

    ray_table_init(rook_attacks.data(), rook_magics, rook_directions, seed);
    ray_table_init(bishop_attacks.data(), bishop_magics, bishop_directions, seed);
    shift_table_init(knight_attacks.data(), knight_directions);
    shift_table_init(king_attacks.data(), king_directions);

    for(int i = square_a1; i <= square_h8; i++)
    {
        for(int j = piece_pawn; j <= piece_king; j++)
        {
            square sq = static_cast<square>(i);
            piece p = static_cast<piece>(j);

            pieces_zobrist_hash[sq][side_white][p] = random_generate(seed);
            pieces_zobrist_hash[sq][side_black][p] = random_generate(seed);
        }
    }

    side_zobrist_hash = random_generate(seed);

    kingside_castle_zobrist_hash[side_white] = random_generate(seed);
    kingside_castle_zobrist_hash[side_black] = random_generate(seed);
    queenside_castle_zobrist_hash[side_white] = random_generate(seed);
    queenside_castle_zobrist_hash[side_black] = random_generate(seed);

    for(int f = file_a; f <= file_h; f++)
    {
        en_passant_zobrist_hash[f] = random_generate(seed);
    }
}


}


#endif