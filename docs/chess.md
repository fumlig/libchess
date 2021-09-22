# Header file `chess.hpp`

``` cpp
namespace chess
{
    enum side;

    int const sides = 2;

    constexpr chess::side opponent(chess::side s);

    enum piece;

    int const pieces = 6;

    std::pair<side, piece> piece_from_san(char san);

    char piece_to_san(chess::side s, chess::piece p);

    enum file;

    int const files = 8;

    enum rank;

    int const ranks = 8;

    enum square;

    int const squares = 64;

    constexpr chess::rank side_rank(chess::side s, chess::rank r);

    constexpr chess::file file_of(chess::square sq);

    constexpr chess::rank rank_of(chess::square sq);

    constexpr chess::square cat_coords(chess::file f, chess::rank r);

    chess::file file_from_san(char san);

    chess::rank rank_from_san(char san);

    chess::square square_from_san(std::string_view san);

    char file_to_san(chess::file f);

    char rank_to_san(chess::rank r);

    std::string square_to_san(chess::square sq);

    enum direction;

    int const directions = 16;

    constexpr chess::direction opposite(chess::direction d);

    constexpr chess::direction forwards(chess::side s);

    using bitboard = std::uint64_t;

    bitboard empty_set = 0ULL;

    bitboard universal_set = ~0ULL;

    constexpr bitboard square_set(chess::square sq);

    constexpr bitboard file_set(chess::file f);

    constexpr bitboard rank_set(chess::rank r);

    bitboard shift_set(chess::bitboard bb, chess::direction d);

    bitboard ray_set(chess::bitboard bb, chess::direction d, chess::bitboard occupied);

    bool set_contains(chess::bitboard bb, chess::square sq);

    bitboard set_insert(chess::bitboard bb, chess::square sq);

    bitboard set_erase(chess::bitboard bb, chess::square sq);

    chess::square set_first(chess::bitboard bb);

    chess::square set_last(chess::bitboard bb);

    int set_cardinality(chess::bitboard bb);

    bitboard set_union(chess::bitboard a, chess::bitboard b);

    bitboard set_intersection(chess::bitboard a, chess::bitboard b);

    bitboard set_complement(chess::bitboard bb);

    std::vector<square> set_elements(chess::bitboard bb);

    struct magic;

    bitboard pawn_east_attack_set(chess::bitboard bb, chess::side s);

    bitboard pawn_west_attack_mask(chess::bitboard bb, chess::side s);

    bitboard rook_attack_mask(chess::square sq, chess::bitboard occupied);

    bitboard knight_attack_mask(chess::square sq);

    bitboard bishop_attack_mask(chess::square sq, chess::bitboard occupied);

    bitboard queen_attack_mask(chess::square sq, chess::bitboard occupied);

    bitboard king_attack_mask(chess::square sq);

    class board;

    struct move;

    struct undo;

    class position;

    void init(std::size_t seed = 2147483647ULL);
}
```

### Enumeration `chess::side`

``` cpp
enum side
{
    side_white,
    side_black,
    side_none = -1
};
```

Sides in chess.

In some places, a none-side is useful (for example for empty board squares).

-----

### Variable `chess::sides`

``` cpp
int const sides = 2;
```

Number of sides.

-----

### Function `chess::opponent`

``` cpp
constexpr chess::side opponent(chess::side s);
```

Opponent of given side.

*Return values:* Opponent of side.

The opponent of white is black and vice versa.

#### Parameters

  - `s` - Side to get opponent of.

-----

### Enumeration `chess::piece`

``` cpp
enum piece
{
    piece_pawn,
    piece_rook,
    piece_knight,
    piece_bishop,
    piece_queen,
    piece_king,
    piece_none = -1
};
```

Pieces in chess.

In some places, a none-piece is useful (for example for empty board squares).

-----

### Variable `chess::pieces`

``` cpp
int const pieces = 6;
```

Number of pieces.

-----

### Function `chess::piece_from_san`

``` cpp
std::pair<side, piece> piece_from_san(char san);
```

Piece and side from Short Algebraic Notation (SAN).

*Return values:* Side and piece type

*Throws:* Invalid argument if SAN does not denote a piece.

Pawn is ‘P’, rook is ‘R’, knight is ‘N’, bishop is ‘B’, queen is ‘Q’ and king is ‘K’. An uppercase letter indicates that it is a white piece and a lowercase indicates letter that it is a black piece.

#### Parameters

  - `san` - SAN of for piece.

-----

### Function `chess::piece_to_san`

``` cpp
char piece_to_san(chess::side s, chess::piece p);
```

Piece and side to Short Algebraic Notation (SAN).

*Return values:* SAN for piece of given side.

*Throws:* Invalid argument if piece is none.

Pawn is ‘P’, rook is ‘R’, knight is ‘N’, bishop is ‘B’, queen is ‘Q’ and king is ‘K’. An uppercase letter indicates that it is a white piece and a lowercase indicates letter that it is a black piece.

#### Parameters

  - `s` - The side of the piece.
  - `p` - The type of the piece.

-----

### Enumeration `chess::file`

``` cpp
enum file
{
    file_a,
    file_b,
    file_c,
    file_d,
    file_e,
    file_f,
    file_g,
    file_h
};
```

Files of a chess board.

-----

### Variable `chess::files`

``` cpp
int const files = 8;
```

Number of files.

-----

### Enumeration `chess::rank`

``` cpp
enum rank
{
    rank_1,
    rank_2,
    rank_3,
    rank_4,
    rank_5,
    rank_6,
    rank_7,
    rank_8
};
```

Ranks of a chess board.

-----

### Variable `chess::ranks`

``` cpp
int const ranks = 8;
```

Number of ranks.

-----

### Enumeration `chess::square`

``` cpp
enum square
{
    square_a1,
    square_b1,
    square_c1,
    square_d1,
    square_e1,
    square_f1,
    square_g1,
    square_h1,
    square_a2,
    square_b2,
    square_c2,
    square_d2,
    square_e2,
    square_f2,
    square_g2,
    square_h2,
    square_a3,
    square_b3,
    square_c3,
    square_d3,
    square_e3,
    square_f3,
    square_g3,
    square_h3,
    square_a4,
    square_b4,
    square_c4,
    square_d4,
    square_e4,
    square_f4,
    square_g4,
    square_h4,
    square_a5,
    square_b5,
    square_c5,
    square_d5,
    square_e5,
    square_f5,
    square_g5,
    square_h5,
    square_a6,
    square_b6,
    square_c6,
    square_d6,
    square_e6,
    square_f6,
    square_g6,
    square_h6,
    square_a7,
    square_b7,
    square_c7,
    square_d7,
    square_e7,
    square_f7,
    square_g7,
    square_h7,
    square_a8,
    square_b8,
    square_c8,
    square_d8,
    square_e8,
    square_f8,
    square_g8,
    square_h8,
    square_none = -1
};
```

Squares on a chess board.

-----

### Variable `chess::squares`

``` cpp
int const squares = 64;
```

Number of squares.

-----

### Function `chess::side_rank`

``` cpp
constexpr chess::rank side_rank(chess::side s, chess::rank r);
```

Rank number for a given side.

*Return values:* The rank for the given side.

For white, the rank will be the same. For black rank 8 is referred to as rank 1, rank 7 as rank 2 and so on.

#### Parameters

  - `s` - Side to get rank number for.
  - `r` - Rank to get for side.

-----

### Function `chess::file_of`

``` cpp
constexpr chess::file file_of(chess::square sq);
```

File of given square.

*Return values:* File of the square.

As an example, the file of square A1 is A.

#### Parameters

  - `sq` - Square to get file of.

-----

### Function `chess::rank_of`

``` cpp
constexpr chess::rank rank_of(chess::square sq);
```

Rank of given square.

*Return values:* Rank of the square.

As an example, the rank of square A1 is 1.

#### Parameters

  - `sq` - Square to get rank of.

-----

### Function `chess::cat_coords`

``` cpp
constexpr chess::square cat_coords(chess::file f, chess::rank r);
```

Concatenate coordinates into square.

*Return values:* Square with given coordinates.

Given a file and a rank, return the corresponding square.

#### Parameters

  - `f` - File of square.
  - `r` - Rank of square.

-----

### Function `chess::file_from_san`

``` cpp
chess::file file_from_san(char san);
```

File from Short Algebraic Notation (SAN).

*Return values:* File with given SAN.

*Throws:* Invalid argument if SAN does not denote a file.

Converts a lowercase char to its corresponding file (‘a’ is file A).

#### Parameters

  - `san` - SAN of file.

-----

### Function `chess::rank_from_san`

``` cpp
chess::rank rank_from_san(char san);
```

Rank from Short Algebraic Notation (SAN).

*Return values:* Rank with given SAN.

*Throws:* Invalid argument if SAN does not denote a rank.

Converts a digit char to its corresponding rank (‘1’ is rank 1).

#### Parameters

  - `san` - SAN of rank.

-----

### Function `chess::square_from_san`

``` cpp
chess::square square_from_san(std::string_view san);
```

Square from Short Algebraic Notation (SAN).

*Return values:* Rank with given SAN.

*Throws:* Invalid argument if SAN does not denote a rank.

Converts the first char to a file and the second to a rank (“a1” is square A1).

#### Parameters

  - `san` - SAN of rank.

-----

### Function `chess::file_to_san`

``` cpp
char file_to_san(chess::file f);
```

File to Short Algebraic Notation (SAN).

*Return values:* SAN for file.

Converts the given file to a lowercase char (file A is ‘a’).

#### Parameters

  - `f` - File to get SAN for.

-----

### Function `chess::rank_to_san`

``` cpp
char rank_to_san(chess::rank r);
```

Rank to Short Algebraic Notation (SAN).

*Return values:* SAN for rank.

Converts the given rank to a lowercase char (rank 1 is ‘1’).

#### Parameters

  - `r` - Rank to get SAN for.

-----

### Function `chess::square_to_san`

``` cpp
std::string square_to_san(chess::square sq);
```

Square tp Short Algebraic Notation (SAN).

*Return values:* SAN of square.

Converts the file and rank of the square to chars and concatenates them (square A1 is “a1”).

#### Parameters

  - `sq` - Square to convert.

-----

### Enumeration `chess::direction`

``` cpp
enum direction
{
    direction_n = 8,
    direction_e = 1,
    direction_s = -8,
    direction_w = -1,
    direction_ne = direction_n+direction_e,
    direction_se = direction_s+direction_e,
    direction_sw = direction_s+direction_w,
    direction_nw = direction_n+direction_w,
    direction_nne = direction_n+direction_n+direction_e,
    direction_ene = direction_e+direction_n+direction_e,
    direction_ese = direction_e+direction_s+direction_e,
    direction_sse = direction_s+direction_s+direction_e,
    direction_ssw = direction_s+direction_s+direction_w,
    direction_wsw = direction_w+direction_s+direction_w,
    direction_wnw = direction_w+direction_n+direction_w,
    direction_nnw = direction_n+direction_n+direction_w,
    direction_none = 0
};
```

Directions of piece movement.

Kings move north, east, south and west. Knights move north-northeast, east-northeast, etc.

-----

### Variable `chess::directions`

``` cpp
int const directions = 16;
```

Number of directions.

-----

### Function `chess::opposite`

``` cpp
constexpr chess::direction opposite(chess::direction d);
```

Opposite of given direction.

*Return values:* Opposite direction.

For example, the oppsoite of north is south.

#### Parameters

  - `d` - Direction to get opposite of.

-----

### Function `chess::forwards`

``` cpp
constexpr chess::direction forwards(chess::side s);
```

Forward direction for given side.

*Return values:* Forwards for given side.

This assumes the board is seen from white’s perspective.

#### Parameters

  - `s` - The side.

-----

### Type alias `chess::bitboard`

``` cpp
using bitboard = std::uint64_t;
```

Bitboard is a set of squares.

The terms bitboard and set of squares will be used interchangeably. A bitboard is 64 bits. Square A1 is in the set if bit 0 is set and square H8 is in the set if bit 64 is set.

-----

### Variable `chess::empty_set`

``` cpp
bitboard empty_set = 0ULL;
```

Empty set.

-----

### Variable `chess::universal_set`

``` cpp
bitboard universal_set = ~0ULL;
```

Universal set.

-----

### Function `chess::square_set`

``` cpp
constexpr bitboard square_set(chess::square sq);
```

Square set.

*Return values:* Bitboard with the bit at square index set.

The set with only the given square in it.

#### Parameters

  - `sq` - The square.

-----

### Function `chess::file_set`

``` cpp
constexpr bitboard file_set(chess::file f);
```

File set.

*Return values:* Bitboard with all bits of the file set.

The set with all squares of given file in it.

#### Parameters

  - `f` - The file.

-----

### Function `chess::rank_set`

``` cpp
constexpr bitboard rank_set(chess::rank r);
```

Rank set.

*Return values:* Bitboard with all bits of the rank set.

The set with all squares of given rank in it.

#### Parameters

  - `r` - The rank.

-----

### Function `chess::shift_set`

``` cpp
bitboard shift_set(chess::bitboard bb, chess::direction d);
```

Directional shift of set.

*Return values:* The set shifted in the direction.

Shifts bitboard in given direction. For example, the bitboard where all squares in file A are set to one can be shifted east to obtain a bitboard where all squares in file B are set.

#### Parameters

  - `bb` - The set.
  - `d` - The direction.

-----

### Function `chess::ray_set`

``` cpp
bitboard ray_set(chess::bitboard bb, chess::direction d, chess::bitboard occupied);
```

Ray cast of a set.

*Return values:* Set with rays.

Given a bitboard, returns a bitboard with rays in the given direction from all set bits. The rays stop at bits set in an occupancy bitboard.

#### Parameters

  - `bb` - The set.
  - `d` - The direction to cast ray in.
  - `occupied` - Occupancy set for collisions.

-----

### Function `chess::set_contains`

``` cpp
bool set_contains(chess::bitboard bb, chess::square sq);
```

Check if set contains square.

*Return values:* Whether the square is in the set.

Returns whether the bit corresponding to the square is set to 1.

#### Parameters

  - `bb` - The set.
  - `sq` - The square.

-----

### Function `chess::set_insert`

``` cpp
bitboard set_insert(chess::bitboard bb, chess::square sq);
```

Insert square to set.

*Return values:* The set with the square in it.

Returns the bit with the bit corresponding to the square set.

#### Parameters

  - `bb` - The set.
  - `sq` - The square.

-----

### Function `chess::set_erase`

``` cpp
bitboard set_erase(chess::bitboard bb, chess::square sq);
```

Erase square from set.

*Return values:* The set without the square in it.

Returns bitboard with the bit corresponding to the square unset.

#### Parameters

  - `bb` - The set.
  - `sq` - The square.

-----

### Function `chess::set_first`

``` cpp
chess::square set_first(chess::bitboard bb);
```

First square in set.

*Return values:* The first square.

Returns the first square in the set, starting at A1. In other words, the first square whose bit is set to 1.

#### Parameters

  - `bb` - The set.

-----

### Function `chess::set_last`

``` cpp
chess::square set_last(chess::bitboard bb);
```

Last square in set.

*Return values:* The last square.

Returns the last square in the set, starting at A1. In other words, the last square whose bit is set to 1.

#### Parameters

  - `bb` - The set.

-----

### Function `chess::set_cardinality`

``` cpp
int set_cardinality(chess::bitboard bb);
```

Cardinality of set.

*Return values:* Squares in set.

Returns the number of squares in the given set (the number of bits set).

#### Parameters

  - `bb` - The set.

-----

### Function `chess::set_union`

``` cpp
bitboard set_union(chess::bitboard a, chess::bitboard b);
```

Union of sets.

*Return values:* Union of both sets.

Returns the union of two sets. Equivalent to bitwise or.

#### Parameters

  - `a` - First set.
  - `b` - Second set.

-----

### Function `chess::set_intersection`

``` cpp
bitboard set_intersection(chess::bitboard a, chess::bitboard b);
```

Intersection of sets.

*Return values:* Union of both sets.

Returns the intersection of two sets. Equivalent to bitwise and.

#### Parameters

  - `a` - First set.
  - `b` - Second set.

-----

### Function `chess::set_complement`

``` cpp
bitboard set_complement(chess::bitboard bb);
```

Complement of set.

*Return values:* The complement of the set.

Returns the complement of two sets. Equivalent to bitwise negation.

#### Parameters

  - `bb` - The set.

-----

### Function `chess::set_elements`

``` cpp
std::vector<square> set_elements(chess::bitboard bb);
```

Squares in set.

*Return values:* Vector of squares.

Returns list of all the squares in a set. Equivalent to all squares whose corresponding bits are set.

#### Parameters

  - `bb` - The set.

-----

### Function `chess::pawn_east_attack_set`

``` cpp
bitboard pawn_east_attack_set(chess::bitboard bb, chess::side s);
```

Set of all east pawn attacks.

*Return values:* Attacked squares.

Given a set of pawns of a given side, returns the set of squares attacked in the east direction by those pawns.

#### Parameters

  - `bb` - Set of pawns.
  - `s` - Side of pawns.

-----

### Function `chess::pawn_west_attack_mask`

``` cpp
bitboard pawn_west_attack_mask(chess::bitboard bb, chess::side s);
```

Set of all west pawn attacks.

*Return values:* Attacked squares.

Given a set of pawns of a given side, returns the set of squares attacked in the west direction by those pawns.

#### Parameters

  - `bb` - Set of pawns.
  - `s` - Side of pawns.

-----

### Function `chess::rook_attack_mask`

``` cpp
bitboard rook_attack_mask(chess::square sq, chess::bitboard occupied);
```

Set of all rook attacks.

*Return values:* Attacked squares.

Given a rook position and occupied squares, returns the set of squares attacked by the rook.

#### Parameters

  - `sq` - Position of rook.
  - `occupied` - Set of occupied squares.

-----

### Function `chess::knight_attack_mask`

``` cpp
bitboard knight_attack_mask(chess::square sq);
```

Set of all knight attacks.

*Return values:* Attacked squares.

Given a knight, returns the set of squares attacked by the knight.

#### Parameters

  - `sq` - Position of knight.

-----

### Function `chess::bishop_attack_mask`

``` cpp
bitboard bishop_attack_mask(chess::square sq, chess::bitboard occupied);
```

Set of all bishop attacks.

*Return values:* Attacked squares.

Given a bishop position and occupied squares, returns the set of squares attacked by the bishop.

#### Parameters

  - `sq` - Position of bishop.
  - `occupied` - Set of occupied squares.

-----

### Function `chess::queen_attack_mask`

``` cpp
bitboard queen_attack_mask(chess::square sq, chess::bitboard occupied);
```

Set of all queen attacks.

*Return values:* Attacked squares.

Given a queen position and occupied squares, returns the set of squares attacked by the queen.

#### Parameters

  - `sq` - Position of queen.
  - `occupied` - Set of occupied squares.

-----

### Function `chess::king_attack_mask`

``` cpp
bitboard king_attack_mask(chess::square sq);
```

Set of all king attacks.

*Return values:* Attacked squares.

Given the position of a king, returns the set of squares attacked by the king.

#### Parameters

  - `sq` - Position of king.

-----

### Class `chess::board`

``` cpp
class board
{
public:
    board();

    board(std::unordered_map<square, std::pair<side, piece>> const& pieces);

    std::pair<side, piece> get(chess::square sq) const;

    void set(chess::square sq, chess::side s, chess::piece p);

    void clear();

    chess::bitboard side_set(chess::side s) const;

    chess::bitboard piece_set(chess::piece p, chess::side s = side_none) const;

    chess::bitboard occupied_set() const;

    chess::bitboard attack_set(chess::side s) const;
};
```

Chess board.

Data structure that holds placement of pieces with efficient retrieval of certain sets, such as which squares are occupied by which side. In most cases, a chess position should be used instead since it holds more information.

### Constructor `chess::board::board`

``` cpp
board();
```

Empty board.

Construct board without any pieces on it.

-----

### Constructor `chess::board::board`

``` cpp
board(std::unordered_map<square, std::pair<side, piece>> const& pieces);
```

Arbitrary board.

Construct board with the given piece placement. Takes a map from squares to piece and color and places the given pieces at the given square.

#### Parameters

  - `pieces` - The piece placement.

-----

### Function `chess::board::get`

``` cpp
std::pair<side, piece> get(chess::square sq) const;
```

Get side and piece at the given square.

*Return values:* Pair of side and piece of given square.

Returns the side and piece at the given square. Both side and piece can be none, in which case the square is empty.

#### Parameters

  - `sq` - The square.

-----

### Function `chess::board::set`

``` cpp
void set(chess::square sq, chess::side s, chess::piece p);
```

Set side and piece at the given square.

Sets the side and piece of given square, and updates other internal state accordingly.

#### Parameters

  - `sq` - The square.
  - `s` - New side of square.
  - `p` - New piece of square.

-----

### Function `chess::board::clear`

``` cpp
void clear();
```

Clear board.

Sets the side and piece of all squares to none, and updates other internal state accordingly.

-----

### Function `chess::board::side_set`

``` cpp
chess::bitboard side_set(chess::side s) const;
```

Side set.

*Return values:* The set of squares.

Returns the set of all squares with pieces of the given side on them.

#### Parameters

  - `s` - The side.

-----

### Function `chess::board::piece_set`

``` cpp
chess::bitboard piece_set(chess::piece p, chess::side s = side_none) const;
```

Piece set.

*Return values:* The set of squares.

Returns the set of all squares with the given piece type on them. A side can optionally be specified to only get pieces of one side.

#### Parameters

  - `p` - The piece.
  - `s` - The side.

-----

### Function `chess::board::occupied_set`

``` cpp
chess::bitboard occupied_set() const;
```

Occupied set.

*Return values:* Occupied squares.

Returns the set of all squares occupied by pieces of any side.

-----

### Function `chess::board::attack_set`

``` cpp
chess::bitboard attack_set(chess::side s) const;
```

Attack set.

*Return values:* Squares attacked by side.

Returns the set of all squares attacked by a given side.

#### Parameters

  - `s` - The side.

-----

-----

### Struct `chess::move`

``` cpp
struct move
{
    move();

    move(chess::square from, chess::square to, chess::piece promote = piece_none);

    static chess::move from_lan(std::string_view lan);

    std::string to_lan() const;
};
```

Chess move.

Contains all information needed to make a move.

### Constructor `chess::move::move`

``` cpp
move();
```

Null-move.

Constructs move which does nothing. Should probably not be used.

-----

### Constructor `chess::move::move`

``` cpp
move(chess::square from, chess::square to, chess::piece promote = piece_none);
```

Move constructor.

Constructs move from given square to given square. If the move is a pawn push or attack to the final rank for the side making the move, a promotion piece should be specified.

#### Parameters

  - `from` - Source square.
  - `to` - Destination square.
  - `promote` - Promotion piece.

-----

### Function `chess::move::from_lan`

``` cpp
static chess::move from_lan(std::string_view lan);
```

Move from Long Algebraic Notation (LAN).

*Return values:* Move corresponding to LAN.

*Throws:* Invalid argument if LAN does not denote a move.

Create move from LAN. “a2a4” is the move from square A2 to A4. “h8h1q” is a promotion to a queen on the H file.

#### Parameters

  - `lan` - LAN string.

-----

### Function `chess::move::to_lan`

``` cpp
std::string to_lan() const;
```

Move to Long Algebraic Notation (LAN).

*Return values:* LAN of move.

Covnert move to LAN. “a2a4” is the move from square A2 to A4. “h8h1q” is a promotion to a queen on the H file.

-----

-----

### Struct `chess::undo`

``` cpp
struct undo
{
};
```

Chess move undo.

Contains all information needed to undo a move on a given position. This is the information that can not be deduced from the position state and move information alone.

-----

### Class `chess::position`

``` cpp
class position
{
public:
    position();

    position(chess::board&& pieces, chess::side turn = side_white, bool white_kingside_castle = true, bool white_queenside_castle = true, bool black_kingside_castle = true, bool black_queenside_castle = true, chess::square en_passant = square_none, int halfmove_clock = 0, int fullmove_number = 1);

    static std::string_view const fen_start = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    static std::string_view const fen_empty = "8/8/8/8/8/8/8/8 w - - 0 1";

    static chess::position from_fen(std::string_view fen = fen_start);

    std::string to_fen() const;

    chess::undo make_move(chess::move const& m);

    void undo_move(chess::move const& m, chess::undo const& u);

    chess::position copy_move(chess::move const& m) const;

    std::vector<move> moves() const;

    chess::board const& pieces() const;

    int fullmove() const;

    int halfmove() const;

    std::size_t hash() const;

    bool is_check() const;

    bool is_checkmate() const;

    bool is_stalemate() const;
};
```

Chess position.

Contains information about a chess position including piece placement, whose turn it is, castling avalailability, en passant target square…

### Constructor `chess::position::position`

``` cpp
position();
```

Empty position.

-----

### Constructor `chess::position::position`

``` cpp
position(chess::board&& pieces, chess::side turn = side_white, bool white_kingside_castle = true, bool white_queenside_castle = true, bool black_kingside_castle = true, bool black_queenside_castle = true, chess::square en_passant = square_none, int halfmove_clock = 0, int fullmove_number = 1);
```

Construct position.

Construct a position frmo the given information. It is assumed that the position is valid.

#### Parameters

  - `pieces` - Piece placement.
  - `turn` - The side whose turn it is.
  - `white_kingside_castle` - White kingside castling availability.
  - `white_queenside_castle` - White queenside castling availability.
  - `black_kingside_castle` - Black kingside castling availability.
  - `black_queenside_castle` - Black queenside castling availability.
  - `en_passant` - En passant target square (if last move was a double pawn push).
  - `halfmove_clock` - Number of silent halfmoves (for 50 move rule).
  - `fullmove_number` - Number of fullmoves made.

-----

### Variable `chess::position::fen_start`

``` cpp
static std::string_view const fen_start = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
```

Fen for starting position.

-----

### Variable `chess::position::fen_empty`

``` cpp
static std::string_view const fen_empty = "8/8/8/8/8/8/8/8 w - - 0 1";
```

Fen for empty position.

-----

### Function `chess::position::from_fen`

``` cpp
static chess::position from_fen(std::string_view fen = fen_start);
```

Create position from Forsyth-Edwards Notation (FEN).

*Return values:* Position encoded by FEN.

*Throws:* Invalid argument if FEN seems to be invalid.

Parse FEN string and returns the position it encodes.

#### Parameters

  - `fen` - FEN string. Defaults to starting position.

-----

### Function `chess::position::to_fen`

``` cpp
std::string to_fen() const;
```

Convert position to Forsyth-Edwards Notation (FEN).

*Return values:* FEN encoding position.

Serialize position to FEN string.

-----

### Function `chess::position::make_move`

``` cpp
chess::undo make_move(chess::move const& m);
```

Make move.

*Return values:* Undo data.

Make move on given position by updating internal state.

#### Parameters

  - `m` - The move.

-----

### Function `chess::position::undo_move`

``` cpp
void undo_move(chess::move const& m, chess::undo const& u);
```

Undo move.

Undo move on given position by updating internal state. The undo data used should be the one returned when making the move.

#### Parameters

  - `m` - The move.
  - `u` - Undo data.

-----

### Function `chess::position::copy_move`

``` cpp
chess::position copy_move(chess::move const& m) const;
```

Copy move.

*Return values:* Position with the move made.

Make move on given position by copying state and updating the copy. This is a little more expensive than making a move by updating internal state, but when a lot of move undos are involved, copying seems to perform similarly (as undoing is not necessary).

#### Parameters

  - `m` - The move.

-----

### Function `chess::position::moves`

``` cpp
std::vector<move> moves() const;
```

Legal moves.

*Return values:* List of legal moves.

Returns list of legal moves in position.

-----

### Function `chess::position::pieces`

``` cpp
chess::board const& pieces() const;
```

Position pieces.

*Return values:* The pieces.

Returns the piece placement of the position.

-----

### Function `chess::position::fullmove`

``` cpp
int fullmove() const;
```

Fullmove number.

*Return values:* The fullmove number.

Returns the number of fullmoves since initial position.

-----

### Function `chess::position::halfmove`

``` cpp
int halfmove() const;
```

Halfmove number.

*Return values:* The halfmove number.

Returns the number of halfmove (often referred to as ply) since initial position.

-----

### Function `chess::position::hash`

``` cpp
std::size_t hash() const;
```

Position hash.

*Return values:* The hash.

Returns the Zobrist hash of the position.

-----

### Function `chess::position::is_check`

``` cpp
bool is_check() const;
```

Check flag.

*Return values:* Check flag.

Returns whether the side whose turn it is is checked.

-----

### Function `chess::position::is_checkmate`

``` cpp
bool is_checkmate() const;
```

Checkmate flag.

*Return values:* Checkmate flag.

Returns whether the side whose turn it is is checkmated.

-----

### Function `chess::position::is_stalemate`

``` cpp
bool is_stalemate() const;
```

Stalemate flag.

*Return values:* Stalemate flag.

Returns whether the side whose turn it is is stalemated.

-----

-----

### Function `chess::init`

``` cpp
void init(std::size_t seed = 2147483647ULL);
```

Initialize library.

Sets up internal state such as attack tables and Zobrist hash keys. The default seed seems to work fine but there might exist one that is both faster and yields better pseudorandom numbers.

-----
