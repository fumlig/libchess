#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include <chess/move.hpp>
#include <chess/random.hpp>
#include <chess/position.hpp>
#include <chess/bitboard.hpp>


using namespace chess;


#define PERFT_TABLE_SIZE (1 << 24)
#define PERFT_TABLE_MASK ((PERFT_TABLE_SIZE) - 1)


static struct perft_entry
{
    uint64_t key;
    unsigned long long nodes;
    int depth;
} perft_table[PERFT_TABLE_SIZE];
static unsigned int perft_hits = 0;


unsigned long long perft(int depth, position* p, bool divide)
{
    if(depth == 0) return 1;

    uint64_t hash = position_hash(p);
    perft_entry* entry = &perft_table[hash & PERFT_TABLE_MASK];
    if(hash == entry->key && depth == entry->depth)
    {
        perft_hits++;
        return entry->nodes;
    }

    unsigned long long nodes = 0;

    move moves[POSITION_MOVES_SIZE];
    undo undo;
    int m = position_moves(p, moves);

    for(int i = 0; i < m; i++)
    {
        position_move(p, &moves[i], &undo);
        unsigned long long move_nodes = perft(depth - 1, p, false);
        position_undo(p, &moves[i], &undo);

        if(divide)
        {
            move_print_uci(&moves[i]);
            printf(": %llu\n", move_nodes);
        }

        nodes += move_nodes;
    }

    entry->key = hash;
    entry->nodes = nodes;
    entry->depth = depth;

    return nodes;
}


struct perft_result
{
    const char* name;
    const char* fen;
    int depth;
    unsigned long long nodes[11];
};

static perft_result perfts[] =
{
    
    {.name = "startpos", .fen = POSITION_FEN_START, .depth = 10, .nodes = {1, 20, 400, 8902, 197281, 4865609, 119060324, 3195901860, 84998978956, 2439530234167, 69352859712417}},
    // positions from Chess Programming Wiki
    {.name = "p2", .fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", .depth = 6, .nodes = {1, 48, 2039, 97862, 4085603, 193690690, 8031647685}},
    {.name = "p3", .fen = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -", .depth = 8, .nodes = {1, 14, 191, 2812, 43238, 674624, 11030083, 178633661, 3009794393}},
    {.name = "p4", .fen = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", .depth = 6, .nodes = {1, 6, 264, 9467, 422333, 15833292, 706045033}},
    {.name = "p5", .fen = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", .depth = 5, .nodes = {1, 44, 14686, 62379, 2103487, 89941194}},
    {.name = "p6", .fen = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", .depth = 9, .nodes = {1, 46, 2079, 89890, 3894594, 164075551, 6923051137, 287188994746, 11923589843526, 490154852788714}},
};


void perft_help(int argc, char* argv[])
{
    printf
    (
        "usage: %s {perft,divide} {<name>,<fen>} <depth>\n"
        "\n"
        "names:\n",
        argv[0]
    );

    int n = sizeof(perfts)/sizeof(perft_result);
    for(int i = 0; i < n; i++)
    {
        printf("%s, depth %d\n", perfts[i].name, perfts[i].depth);
    }
}

int main(int argc, char* argv[])
{   
    if(argc < 4)
    {
        perft_help(argc, argv);
        return 1;
    }

    chess::random r;
    random_init(&r, 2147483647);
    bitboard_init(&r);
    board_init(&r);
    position_init(&r);

    bool divide = strcmp(argv[1], "divide") == 0;
    const char* fen = argv[2];
    int depth = atoi(argv[3]);

    int result_depth = 0;
    unsigned long long* result_nodes = NULL;

    int n = sizeof(perfts)/sizeof(perft_result);
    for(int i = 0; i < n; i++)
    {
        if(strcmp(fen, perfts[i].name) == 0)
        {
            fen = perfts[i].fen;
            result_depth = perfts[i].depth;
            result_nodes = perfts[i].nodes;
            break;
        }
    }

    position p;
    position_from_fen(&p, fen);

    timespec begin, end;
    clock_gettime(CLOCK_MONOTONIC, &begin);
    
    unsigned long long nodes = perft(depth, &p, divide);
    
    clock_gettime(CLOCK_MONOTONIC, &end); 
    unsigned long long ns = (end.tv_sec - begin.tv_sec) * 1e9 + (end.tv_nsec - begin.tv_nsec);

    double nps = (1e9*nodes)/ns;
    printf("total: %llu\n", nodes);
    if(depth <= result_depth && result_nodes != NULL) printf("expected: %llu\n", result_nodes[depth]);
    printf("time: %lluns (%f nps, %d hits)\n", ns, nps, perft_hits);

    return 0;
}
