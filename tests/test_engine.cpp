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