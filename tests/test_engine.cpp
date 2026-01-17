#include "catch_amalgamated.hpp"

#include "../include/board.h"
#include "../include/evaluator.h"
#include "../include/engine.h"

TEST_CASE("perft test") {
    Board board;
    Engine engine;

    REQUIRE(engine.perft(board, 1, true) == 20);
    REQUIRE(engine.perft(board, 2, true) == 400);
    REQUIRE(engine.perft(board, 3, true) == 8902);
}

TEST_CASE("play the engine") {
    Board board;
    Engine engine;

    Move eng1(1, 1, Board::PAWN);
    Move eng2(1, 1, Board::PAWN);

    while (!board.is_over(true) && !board.is_over(false)) {
        eng1 = engine.best_move(board, true);
        board.move(eng1, true);

        std::cout << "As white, I choose to move " << eng1.type << " from " << eng1.from << " to " << eng1.to << endl;
        eng2 = engine.best_move(board, false);
        board.move(eng2, false);

        std::cout << "As black, I choose to move " << eng2.type << " from " << eng2.from << " to " << eng2.to << endl;
    }
    // for (int i = 0; i < 8 && !board.is_over(true) && !board.is_over(false); i++) {
        
    // }
}