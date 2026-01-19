#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>
#include "board.h"
#include "move.h"
#include "evaluator.h"
#include <vector>

class Engine {
private:
    // Max depth for main search
    static constexpr int MAX_DEPTH = 4;

    // Max depth for quiescence search
    static constexpr int MAX_QUIESCENCE = 6;
    Evaluator evaluator;

    // Recursively evaluates board states for the best move with a negamax algorith,
    // uses alpha beta pruning to save time and disregard useless moves and quiescence search
    int search(Board& board, int depth, bool is_white, int alpha, int beta);

    // Continues evaluating past "loud" board positions where capture chains follow
    int quiescence(Board& board, bool is_white, int alpha, int beta, int depth);

public:
    // Finds the best move, given a board state
    Move best_move(Board& board, bool is_white);

    // Used for testing/debugging, finds the number of perceivable board states up to depth
    // turns after
    uint64_t perft(Board& board, int depth, bool is_white);
};

#endif