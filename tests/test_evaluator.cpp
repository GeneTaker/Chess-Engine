#include "catch_amalgamated.hpp"

#include "../include/board.h"
#include "../include/evaluator.h"

TEST_CASE("Evaluate the original board") {
    Board board;
    Evaluator evaluator;

    int white_score = evaluator.evaluate_position(board, true); 
    int black_score = evaluator.evaluate_position(board, false);
    
    REQUIRE(white_score == 0);
    REQUIRE(black_score == 0);
}

TEST_CASE("Evaluate when white knight takes center") {
    Board board;
    Evaluator evaluator;

    Move nf3(6, 23, Board::KNIGHT);
    board.move(nf3, true);

    Move ne5(23, 40, Board::KNIGHT);
    board.move(ne5, true);

    int white_score = evaluator.evaluate_position(board, true);
    int black_score = evaluator.evaluate_position(board, false);

    REQUIRE(white_score > 0);
    REQUIRE(black_score < 0);
}

TEST_CASE("Evaluate when black knight takes center") {
    Board board;
    Evaluator evaluator;

    Move nc6(57, 42, Board::KNIGHT);
    board.move(nc6, false);

    Move nd4(42, 27, Board::KNIGHT);
    board.move(nd4, false);

    int white_score = evaluator.evaluate_position(board, true);
    int black_score = evaluator.evaluate_position(board, false);

    REQUIRE(white_score < 0);
    REQUIRE(black_score > 0);
}

TEST_CASE("Evaluate when pawn takes pawn and takes center") {
    Board board;
    Evaluator evaluator;

    Move e4(12, 28, Board::PAWN);
    board.move(e4, true);

    Move d5(51, 35, Board::PAWN);
    board.move(d5, false);

    Move xd5(28, 35, Board::PAWN);
    board.move(xd5, true);

    int white_score = evaluator.evaluate_position(board, true);
    int black_score = evaluator.evaluate_position(board, false);

    REQUIRE(white_score > 0);
    REQUIRE(black_score < 0);

    Move a5(48, 32, Board::PAWN);
    board.move(a5, false);

    white_score = evaluator.evaluate_position(board, true);
    black_score = evaluator.evaluate_position(board, false);

    REQUIRE(white_score > 0);
    REQUIRE(black_score < 0);
}

TEST_CASE("Evaluate line advantage") {
    Board board;
    Evaluator evaluator;

    Move e3(12, 20, Board::PAWN);
    Move f6(53, 45, Board::PAWN);

    board.move(e3, true);
    board.move(f6, false);

    int white_score = evaluator.evaluate_position(board, true);
    int black_score = evaluator.evaluate_position(board, false);

    REQUIRE(white_score > 0);
    REQUIRE(black_score < 0);
}

TEST_CASE("Evaluate general advantageous vs terrible positions") {
    Board board;
    Evaluator evaluator;

    Move e4(12, 20, Board::PAWN);
    Move f6(53, 45, Board::PAWN);

    Move nc3(1, 18, Board::KNIGHT);
    Move nh6(62, 47, Board::KNIGHT);

    Move d4(11, 27, Board::PAWN);
    Move f5(45, 37, Board::PAWN);

    Move bc4(5, 26, Board::BISHOP);

    board.move(e4, true);
    board.move(f6, false);
    board.move(nc3, true);
    board.move(nh6, false);
    board.move(d4, true);
    board.move(f5, false);
    board.move(bc4, true);

    int white_score = evaluator.evaluate_position(board, true);
    int black_score = evaluator.evaluate_position(board, false);

    REQUIRE(white_score > 0);
    REQUIRE(black_score < 0);
}

