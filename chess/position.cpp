#include <string>
#include <sstream>

#include "side.hpp"
#include "square.hpp"
#include "board.hpp"
#include "move.hpp"
#include "set.hpp"
#include "position.hpp"
#include "zobrist.hpp"
#include "attack.hpp"


namespace chess
{


position::position():
b(),
turn{side_white},
kingside_castle{true, true},
queenside_castle{true, true},
en_passant{square_none},
halfmove_clock{0},
fullmove_number{1},
zobrist_hash{0},
repetitions{1}
{}

position::position
(
    const board& b, 
    side turn,
    bool white_kingside_castle,
    bool white_queenside_castle,
    bool black_kingside_castle,
    bool black_queenside_castle,
    square en_passant,
    int halfmove_clock,
    int fullmove_number
):
b(b),
turn{turn},
kingside_castle{white_kingside_castle, black_kingside_castle},
queenside_castle{white_queenside_castle, black_queenside_castle},
en_passant{en_passant},
halfmove_clock{halfmove_clock},
fullmove_number{fullmove_number},
zobrist_hash{0},
repetitions{1}
{
    if(turn == side_black)              zobrist_hash ^= zobrist_side_key();
    if(kingside_castle[side_white])     zobrist_hash ^= zobrist_kingside_castle_key(side_white);
    if(queenside_castle[side_white])    zobrist_hash ^= zobrist_queenside_castle_key(side_white);
    if(kingside_castle[side_black])     zobrist_hash ^= zobrist_kingside_castle_key(side_black);
    if(queenside_castle[side_black])    zobrist_hash ^= zobrist_queenside_castle_key(side_black);
    if(en_passant != square_none)       zobrist_hash ^= zobrist_en_passant_key(file_of(en_passant));
}


position position::from_fen(const std::string& fen)
{
	std::istringstream in(fen);
    return from_fen(in);
}


position position::from_fen(std::istream& in)
{
    std::string dummy;
    if(!(in >> dummy))
    {
        throw std::invalid_argument("fen stream empty");
    }
    
    if(dummy == "startpos")
    {
        return position();
    }

	std::string pieces_string = dummy, turn_string, castle_string, en_passant_string;
	int halfmove_clock, fullmove_number;

	if(!(in >> turn_string >> castle_string >> en_passant_string >> halfmove_clock >> fullmove_number))
	{
		throw std::invalid_argument("fen does not contain 6 fields");
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

std::string position::to_fen() const
{
	std::ostringstream fen_stream;

    for(int r = rank_8; r >= rank_1; r--)
    {
        int empty = 0;
        for(int f = file_a; f <= file_h; f++)
        {
            square sq = cat_coords(static_cast<file>(f), static_cast<rank>(r));
			auto[s, p] = b.get(sq);

            if(p == piece_none)
            {
                empty++;
                continue;
            }
            else if(empty != 0)
            {
				fen_stream << empty;
                empty = 0;
            }

			fen_stream << piece_to_san(s, p);
        }

        if(empty != 0)
		{
			fen_stream << empty;
		}
        if(r != rank_1)
		{
			fen_stream << '/';
		}
	}
    
	fen_stream << (turn == side_white ? " w " : " b ");

	auto pos = fen_stream.tellp();

    if(kingside_castle[side_white])		fen_stream << 'K';
    if(queenside_castle[side_white]) 	fen_stream << 'Q';
    if(kingside_castle[side_black]) 	fen_stream << 'k';
    if(queenside_castle[side_black]) 	fen_stream << 'q';
    if(pos == fen_stream.tellp()) 		fen_stream << '-';

	fen_stream << ' ' << square_to_san(en_passant) << ' ' << halfmove_clock << ' ' << fullmove_number;

	return fen_stream.str();
}

undo position::make_move(const move& m)
{
	piece capture = b.get(m.to).second;
    undo u{capture, en_passant, kingside_castle, queenside_castle, halfmove_clock};

    auto [side, piece] = b.get(m.from);
    square ep = en_passant;

    b.set(m.from, side_none, piece_none);

    if(m.promote != piece_none) b.set(m.to, side, m.promote);
    else b.set(m.to, side, piece);

    en_passant = square_none;
    if(ep != square_none) zobrist_hash ^= zobrist_en_passant_key(file_of(ep));

    if(piece == piece_pawn)
    {
        if(rank_of(m.from) == side_rank(side, rank_2) && rank_of(m.to) == side_rank(side, rank_4))
        {
            // double push
            en_passant = cat_coords(file_of(m.from), side_rank(side, rank_3));
            zobrist_hash ^= zobrist_en_passant_key(file_of(en_passant));
        }
        else if(m.to == ep)
        {
            // en passant capture
            square ep_capture = cat_coords(file_of(ep), side_rank(side, rank_5));
            b.set(ep_capture, side_none, piece_none);
        }
    }
    else if(piece == piece_king)
    {
        if(kingside_castle[side])
        {
            kingside_castle[side] = false;
            zobrist_hash ^= zobrist_kingside_castle_key(side);
        }
        if(queenside_castle[side])
        {
            queenside_castle[side] = false;
            zobrist_hash ^= zobrist_queenside_castle_key(side);
        }
        
        rank rank_first = side_rank(side, rank_1);

        if(m.from == cat_coords(file_e, rank_first))
        {
            if(m.to == cat_coords(file_g, rank_first))
            {
                b.set(cat_coords(file_h, rank_first), side_none, piece_none);
                b.set(cat_coords(file_f, rank_first), side, piece_rook);
            }
            else if(m.to == cat_coords(file_c, rank_first))
            {
                b.set(cat_coords(file_a, rank_first), side_none, piece_none);
                b.set(cat_coords(file_d, rank_first), side, piece_rook);
            }
        }
    }

    // todo: these (and en passant) should only be updated if they changed from the previous move
    // can probably be done by xoring with current state before each move

    if(queenside_castle[side_white] && (m.from == square_a1 || m.to == square_a1))
    {
        queenside_castle[side_white] = false;
        zobrist_hash ^= zobrist_queenside_castle_key(side_white);
    }
    if(kingside_castle[side_white] && (m.from == square_h1 || m.to == square_h1))
    {
        kingside_castle[side_white] = false;
        zobrist_hash ^= zobrist_kingside_castle_key(side_white);
    }
    if(queenside_castle[side_black] && (m.from == square_a8 || m.to == square_a8))
    {
        queenside_castle[side_black] = false;
        zobrist_hash ^= zobrist_queenside_castle_key(side_black);
    }
    if(kingside_castle[side_black] && (m.from == square_h8 || m.to == square_h8))
    {
        kingside_castle[side_black] = false;
        zobrist_hash ^= zobrist_kingside_castle_key(side_black);
    }

    if(piece == piece_pawn || capture != piece_none)
    {
        halfmove_clock = 0;
    }
    else
    {
        halfmove_clock++;
    }

    fullmove_number += turn;
    turn = opponent(turn);
    zobrist_hash ^= zobrist_side_key();

    return u;
}

void position::undo_move(const move& m, const undo& u)
{
    auto [side, piece] = b.get(m.to);

    b.set(m.from, side, piece);
    b.set(m.to, side_none, piece_none);
    if(u.capture != piece_none) b.set(m.to, opponent(side), u.capture);
    if(m.promote != piece_none) b.set(m.from, side, piece_pawn);

    if(en_passant != square_none) zobrist_hash ^= zobrist_en_passant_key(file_of(en_passant));
    if(u.en_passant != square_none) zobrist_hash ^= zobrist_en_passant_key(file_of(u.en_passant));
    en_passant = u.en_passant;

    if(kingside_castle[side_white] != u.kingside_castle[side_white])
    {
        kingside_castle[side_white] = u.kingside_castle[side_white];
        zobrist_hash ^= zobrist_kingside_castle_key(side_white);
    }
    if(kingside_castle[side_black] != u.kingside_castle[side_black])
    {
        kingside_castle[side_black] = u.kingside_castle[side_black];
        zobrist_hash ^= zobrist_kingside_castle_key(side_black);
    }
    if(queenside_castle[side_white] != u.queenside_castle[side_white])
    {
        queenside_castle[side_white] = u.queenside_castle[side_white];
        zobrist_hash ^= zobrist_queenside_castle_key(side_white);
    }
    if(queenside_castle[side_black] != u.queenside_castle[side_black])
    {
        queenside_castle[side_black] = u.queenside_castle[side_black];
        zobrist_hash ^= zobrist_queenside_castle_key(side_black);
    }

    if(piece == piece_pawn)
    {
        if(m.to == u.en_passant)
        {
            square ep_capture = cat_coords(file_of(u.en_passant), side_rank(side, rank_5));
            b.set(ep_capture, opponent(side), piece_pawn);
        }
    }
    else if(piece == piece_king)
    {
        rank rank_first = side_rank(side, rank_1);

        if(m.from == cat_coords(file_e, rank_first))
        {
            if(m.to == cat_coords(file_g, rank_first))
            {
                b.set(cat_coords(file_h, rank_first), side, piece_rook);
                b.set(cat_coords(file_f, rank_first), side_none, piece_none);
            }
            else if(m.to == cat_coords(file_c, side_rank(side, rank_1)))
            {
                b.set(cat_coords(file_a, rank_first), side, piece_rook);
                b.set(cat_coords(file_d, rank_first), side_none, piece_none);
            }
        }
    }

    halfmove_clock = u.halfmove_clock;
    fullmove_number -= opponent(turn); // decrease if white
    turn = opponent(turn);
    zobrist_hash ^= zobrist_side_key();
}

position position::copy_move(const move& m) const
{
    position p = *this;
    p.make_move(m);
    return p;
}

std::vector<move> position::moves() const
{
    std::vector<move> moves;

    bitboard occupied = b.occupied_set();
    
    bitboard pawns = b.piece_set(piece_pawn, turn);
    bitboard rooks = b.piece_set(piece_rook, turn);
    bitboard knights = b.piece_set(piece_knight, turn);
    bitboard bishops = b.piece_set(piece_bishop, turn);
    bitboard queens = b.piece_set(piece_queen, turn);
    bitboard kings = b.piece_set(piece_king, turn);
    
    bitboard attack_mask = ~b.side_set(turn);
    bitboard capture_mask = b.side_set(opponent(turn));
    bitboard ep_mask = 0;
    if(en_passant != square_none) ep_mask = square_set(en_passant);

    // pawn moves
    bitboard single_push_tos = set_shift(pawns, forwards(turn)) & ~occupied;
    bitboard single_push_froms = set_shift(single_push_tos, forwards(opponent(turn)));
    bitboard double_push_tos = set_shift(single_push_tos & rank_set(side_rank(turn, rank_3)), forwards(turn)) & ~occupied;
    bitboard double_push_froms = set_shift(set_shift(double_push_tos, forwards(opponent(turn))), forwards(opponent(turn)));

    bitboard attack_east_tos = set_shift(pawns, static_cast<direction>(forwards(turn) + direction_e)) & (capture_mask | ep_mask);
    bitboard attack_east_froms = set_shift(attack_east_tos, static_cast<direction>(forwards(opponent(turn)) + direction_w));
    bitboard attack_west_tos = set_shift(pawns, static_cast<direction>(forwards(turn) + direction_w)) & (capture_mask | ep_mask);
    bitboard attack_west_froms = set_shift(attack_west_tos, static_cast<direction>(forwards(opponent(turn)) + direction_e));
    
    bitboard promote_push_tos = single_push_tos & rank_set(side_rank(turn, rank_8));
    bitboard promote_push_froms = single_push_froms & rank_set(side_rank(turn, rank_7));
    bitboard promote_east_tos = attack_east_tos & rank_set(side_rank(turn, rank_8));
    bitboard promote_east_froms = attack_east_froms & rank_set(side_rank(turn, rank_7));
    bitboard promote_west_tos = attack_west_tos & rank_set(side_rank(turn, rank_8));
    bitboard promote_west_froms = attack_west_froms & rank_set(side_rank(turn, rank_7));;

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
        square from = set_first(rooks);
        rooks = set_erase(rooks, from);
        bitboard attacks = rook_attack_set(from, occupied) & attack_mask;
        piecewise_moves(from, attacks, piece_none, moves);
    }

    // knight moves
    while(knights)
    {
        square from = set_first(knights);
        knights = set_erase(knights, from);
        bitboard attacks = knight_attack_set(from) & attack_mask;
        piecewise_moves(from, attacks, piece_none, moves);
    }

    // bishop moves
    while(bishops)
    {
        square from = set_first(bishops);
        bishops = set_erase(bishops, from);
        bitboard attacks = bishop_attack_set(from, occupied) & attack_mask;
        piecewise_moves(from, attacks, piece_none, moves);
    }

    // queen moves
    while(queens)
    {
        square from = set_first(queens);
        queens = set_erase(queens, from);
        bitboard attacks = (rook_attack_set(from, occupied) | bishop_attack_set(from, occupied)) & attack_mask;
        piecewise_moves(from, attacks, piece_none, moves);
    }

    // king moves
    if(kingside_castle[turn])
    {
        square from = set_first(kings);
        square to = cat_coords(file_g, rank_of(from));
        bitboard path = kings;
        path |= set_shift(path, direction_e);
        path |= set_shift(path, direction_e);
        bitboard between = path & ~kings;

        if(!(between & occupied) && !(path & b.attack_set(opponent(turn))))
        {
            moves.emplace_back(from, to, piece_none);
        }
    }
    if(queenside_castle[turn])
    {
        square from = set_first(kings);
        square to = cat_coords(file_c, rank_of(from));
        bitboard path = kings;
        path |= set_shift(path, direction_w);
        path |= set_shift(path, direction_w);
        bitboard between = set_shift(path, direction_w);

        if(!(between & occupied) && !(path & b.attack_set(opponent(turn))))
        {
            moves.emplace_back(from, to, piece_none);
        }
    }
    while(kings)
    {
        square from = set_first(kings);
        kings = set_erase(kings, from);
        bitboard attacks = king_attack_set(from) & attack_mask;
        piecewise_moves(from, attacks, piece_none, moves);
    }

    // remove illegal moves
    side player = turn;
    int n = 0;
    for(move& m: moves)
    {
        position p = copy_move(m);;

        if(!(p.b.attack_set(opponent(player)) & p.b.piece_set(piece_king, player)))
        {
            moves[n++] = m;
        }
    }

    moves.resize(n);

    return moves;
}

const board& position::get_board() const
{
    return b;
}

int position::get_fullmove() const
{
    return fullmove_number;
}

int position::get_halfmove() const
{
    return (fullmove_number - 1)*2 + turn;
}

int position::get_halfmove_clock() const
{
    return halfmove_clock;
}

bool position::can_castle_kingside(side s) const
{
    return kingside_castle[s];
}

bool position::can_castle_queenside(side s) const
{
    return queenside_castle[s];
}

std::size_t position::hash() const
{
    return zobrist_hash ^ b.hash();
}

side position::get_turn() const
{
    return turn;
}

std::string position::to_string(bool coords) const
{
    std::ostringstream stream;

    stream << '\n';
    stream << "turn: " << (turn == side_white ? "white" : "black") << '\n';
    stream << "white kingside castle: " << (kingside_castle[side_white] ? "yes" : "no") << '\n';
    stream << "white queenside castle: " << (queenside_castle[side_white] ? "yes" : "no") << '\n';
    stream << "black kingside castle: " << (kingside_castle[side_black] ? "yes" : "no") << '\n';
    stream << "black queenside castle: " << (queenside_castle[side_black] ? "yes" : "no") << '\n';
    stream << "halfmove clock: " << halfmove_clock << '\n';
    stream << "fullmove number: " << fullmove_number << '\n';

    return b.to_string(coords) + stream.str();
}

bool position::is_check() const
{
    return b.attack_set(opponent(turn)) & b.piece_set(piece_king, turn);
}

bool position::is_checkmate() const
{
    return is_check() && moves().empty();
}

bool position::is_stalemate() const
{
    return !is_check() && moves().empty();
}

bool position::is_threefold_repetition() const
{
    return repetitions >= 3;
}

bool position::is_fivefold_repetition() const
{
    return repetitions >= 5;
}

bool position::is_fiftymove_rule() const
{
    return halfmove_clock >= 100;
}

bool position::is_seventyfivemove_rule() const
{
    return halfmove_clock >= 150;
}

bool position::is_insufficient_material() const
{
    int pawns = set_cardinality(b.piece_set(piece_pawn));
    int rooks = set_cardinality(b.piece_set(piece_rook));
    int knights = set_cardinality(b.piece_set(piece_knight));
    int bishops = set_cardinality(b.piece_set(piece_bishop));
    int queens = set_cardinality(b.piece_set(piece_queen));

    if(pawns > 0 || rooks > 0 || queens > 0)
    {
        return false;
    }

    if((knights == 0 && bishops <= 1) || (knights <= 1 && bishops == 0))
    {
        return true;
    }

    if(bishops == 2)
    {
        bitboard white_bishop_set = b.piece_set(piece_bishop, side_white);
        bitboard black_bishop_set = b.piece_set(piece_bishop, side_black);

        if(!white_bishop_set || !white_bishop_set)
        {
            return false;
        }

        side white_bishop_color = color_of(set_first(white_bishop_set));
        side black_bishop_color = color_of(set_first(black_bishop_set));

        if(white_bishop_color == black_bishop_color)
        {
            return true;
        }
    }

    return false;
}

bool position::is_draw() const
{
    return is_stalemate() || is_threefold_repetition() || is_fiftymove_rule() || is_insufficient_material();
}

bool position::is_terminal() const
{
    return is_checkmate() || is_draw();
}

void position::piecewise_moves(square from, bitboard tos, piece promote, std::vector<move>& moves) const
{
    while(tos)
    {
        square to = set_first(tos);
        tos = set_erase(tos, to);
        moves.emplace_back(from, to, promote);
    }
}

void position::setwise_moves(bitboard froms, bitboard tos, piece promote, std::vector<move>& moves) const
{
    while(froms && tos)
    {
        square from = set_first(froms);
        square to = set_first(tos);
        froms = set_erase(froms, from);
        tos = set_erase(tos, to);
        moves.emplace_back(from, to, promote);
    }
}


}
