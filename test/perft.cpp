#include <iostream>
#include <ctime>
#include <cassert>

#include <chess/move.hpp>
#include <chess/random.hpp>
#include <chess/position.hpp>
#include <chess/bitboard.hpp>
#include <chess/notation.hpp>


// todo: divide


using namespace chess;


#define PERFT_TABLE_SIZE (1 << 24)
#define PERFT_TABLE_MASK ((PERFT_TABLE_SIZE) - 1)


static struct perft_entry
{
    std::size_t key;
    unsigned long long nodes;
    int depth;
} perft_table[PERFT_TABLE_SIZE];
static unsigned int perft_hits = 0;


unsigned long long perft(int depth, position* p)
{
    if(depth == 0) return 1;

    std::size_t hash = p->hash();
    perft_entry* entry = &perft_table[hash & PERFT_TABLE_MASK];
    if(hash == entry->key && depth == entry->depth)
    {
        perft_hits++;
        return entry->nodes;
    }

    unsigned long long nodes = 0;

    for(move& move: p->moves())
    {
        undo undo = p->make_move(move);
        unsigned long long move_nodes = perft(depth - 1, p);
        p->undo_move(move, undo);

        nodes += move_nodes;
    }

    entry->key = hash;
    entry->nodes = nodes;
    entry->depth = depth;

    return nodes;
}


struct perft_result
{
    std::string name;
    std::string fen;
    int depth;
    unsigned long long nodes[11];
};

static perft_result perfts[] =
{
    
    {.name = "startpos", .fen = std::string(fen_start), .depth = 10, .nodes = {1, 20, 400, 8902, 197281, 4865609, 119060324, 3195901860, 84998978956, 2439530234167, 69352859712417}},
    // positions from Chess Programming Wiki
    {.name = "p2", .fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", .depth = 6, .nodes = {1, 48, 2039, 97862, 4085603, 193690690, 8031647685}},
    {.name = "p3", .fen = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", .depth = 8, .nodes = {1, 14, 191, 2812, 43238, 674624, 11030083, 178633661, 3009794393}},
    {.name = "p4", .fen = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", .depth = 6, .nodes = {1, 6, 264, 9467, 422333, 15833292, 706045033}},
    {.name = "p5", .fen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", .depth = 5, .nodes = {1, 44, 14686, 62379, 2103487, 89941194}},
    {.name = "p6", .fen = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", .depth = 9, .nodes = {1, 46, 2079, 89890, 3894594, 164075551, 6923051137, 287188994746, 11923589843526, 490154852788714}},
};


void perft_help(int argc, char* argv[])
{
    std::cout
        << "usage: " << argv[0] << " {<name>,<fen>} <depth>" << std::endl
        << std::endl
        << "names:" << std::endl;

    int n = sizeof(perfts)/sizeof(perft_result);
    for(int i = 0; i < n; i++)
    {
        std::cout << perfts[i].name << ", depth " << perfts[i].depth << std::endl;
    }
}

int main(int argc, char* argv[])
{   
    if(argc < 3)
    {
        perft_help(argc, argv);
        return 1;
    }

    chess::random r;
    random_init(&r, 2147483647);
    bitboard_init(&r);
    board_init(&r);
    position_init(&r);

    std::string fen = argv[1];
    int depth = atoi(argv[2]);

    int result_depth = 0;
    unsigned long long* result_nodes = NULL;

    int n = sizeof(perfts)/sizeof(perft_result);
    for(int i = 0; i < n; i++)
    {
        if(fen == perfts[i].name)
        {
            std::cout << "using position from table" << std::endl;
            fen = perfts[i].fen;
            result_depth = perfts[i].depth;
            result_nodes = perfts[i].nodes;
            break;
        }
    }

    position p;
    assert(from_fen(fen, p));
    //position_from_fen(&p, fen.data());

    timespec begin, end;
    clock_gettime(CLOCK_MONOTONIC, &begin);
    
    unsigned long long nodes = perft(depth, &p);
    
    clock_gettime(CLOCK_MONOTONIC, &end); 
    unsigned long long ns = (end.tv_sec - begin.tv_sec) * 1e9 + (end.tv_nsec - begin.tv_nsec);

    double nps = (1e9*nodes)/ns;
    std::cout << "total: " << nodes << std::endl;
    if(depth <= result_depth && result_nodes != NULL) std::cout << "expected: " << result_nodes[depth] << std::endl;
    std::cout << "time: " << ns << " ns (" << nps << " nps, " << perft_hits << " hits)" << std::endl;

    return 0;
}
