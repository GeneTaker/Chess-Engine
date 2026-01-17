#include "../include/engine.h"
#include <vector>


int Engine::search(Board& board, int depth, bool is_white, int alpha, int beta) {
    return -1;
}

Move Engine::best_move(Board& board, bool is_white) {
    return Move(1, 1, Board::PAWN);
}

uint64_t Engine::perft(Board& board, int depth, bool is_white) {
    if (depth == 0) {
        return 1ULL;
    }

    std::vector<Move> moves = board.find_legal_moves(is_white);

    uint64_t result = 0;

    for (auto m : moves) {
        board.make_move_unchecked(m, is_white);
        
        result += perft(board, depth - 1, !is_white);
        board.unmake_move();
    }

    return result;
}

// void Engine::helper(Board& board, int depth, bool is_white) {
//     Results results;
//     results.all = 0;
//     results.pawns = 0;
//     results.bishops = 0;
//     results.knights = 0;
//     results.rooks = 0;
//     results.queens = 0;
//     results.kings = 0;
//     results.promos = 0;
//     results.pawn_capture = 0;
//     results.pawn_double = 0;
//     results.enpassant = 0;

//     perft(board, depth, is_white, results);
//     std::cout << ";asdjf " << results.all << endl;
//     std::cout << "pawns: " << results.pawns << endl;
//     std::cout << "bishops: " << results.bishops << endl;
//     std::cout << "knights: " << results.knights << endl;
//     std::cout << "rooks: " << results.rooks << endl;
//     std::cout << "queens: " << results.queens << endl;
//     std::cout << "kings: " << results.kings << endl;
//     std::cout << "promos: " << results.promos << endl;
//     std::cout << "pawn capture another piece: " << results.pawn_capture << endl;
//     std::cout << "pawn double jump: " << results.pawn_double << endl;
//     std::cout << "enpassant: " << results.enpassant << endl;
// }