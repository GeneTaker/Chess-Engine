#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>
#include "board.h"
#include "move.h"
#include "evaluator.h"
#include <vector>

class Engine {
private:
    static constexpr int MAX_DEPTH = 5;
    Evaluator evaluator;

    int search(Board& board, int depth, bool is_white, int alpha, int beta);

public:
    Move best_move(Board& board, bool is_white);
    uint64_t perft(Board& board, int depth, bool is_white);
};

#endif