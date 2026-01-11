#include "catch_amalgamated.hpp"

#include "../include/board.h"

TEST_CASE("Queen test, ensure that queens can properly move") {
    Board board;
    
    Move e4(12, 28, Board::PAWN);
    Move qg4(3, 30, Board::QUEEN);
    Move qf4(30, 29, Board::QUEEN);
    Move qg5(29, 37, Board::QUEEN);

    board.move(e4, true);
    bool b1 = board.move(qg4, true);

    REQUIRE((1ULL << 30) & board.get_bitboard(Board::QUEEN));
    REQUIRE_FALSE((1ULL << 3) & board.get_bitboard(Board::QUEEN));

    bool b2 = board.move(qf4, true);

    REQUIRE((1ULL << 29) & board.get_bitboard(Board::QUEEN));
    REQUIRE_FALSE((1ULL << 30) & board.get_bitboard(Board::QUEEN));


    bool b3 = board.move(qg5, true);

    REQUIRE((1ULL << 37) & board.get_bitboard(Board::QUEEN));
    REQUIRE_FALSE((1ULL << 29) & board.get_bitboard(Board::QUEEN));


    REQUIRE(b1);
    REQUIRE(b2);
    REQUIRE(b3);
}

TEST_CASE("Queen test, ensure that queens can take pieces") {
    Board board;
    
    Move e4(12, 28, Board::PAWN);
    Move d5(51, 35, Board::PAWN);
    Move xd5(28, 35, Board::PAWN);
    Move qxd5(59, 35, Board::QUEEN);

    board.move(e4, true);
    board.move(d5, false);
    board.move(xd5, true);
    bool b = board.move(qxd5, false);

    REQUIRE(b);
    REQUIRE_FALSE((1ULL << 35) & board.get_bitboard(Board::PAWN));
    REQUIRE((1ULL << 35) & board.get_bitboard(Board::BLACK_QUEEN));
}

TEST_CASE("Queen test, blocked queen") {
    Board board;
    
    Move qd5(3, 35, Board::QUEEN);
    Move qg4(3, 30, Board::QUEEN);
    Move qb2(3, 17, Board::QUEEN);

    bool b1 = board.move(qd5, true);
    bool b2 = board.move(qg4, true);
    bool b3 = board.move(qb2, true);

    REQUIRE(b1);
    REQUIRE(b2);
    REQUIRE(b3);

    REQUIRE_FALSE((1ULL << 35) & board.get_bitboard(Board::QUEEN));
    REQUIRE_FALSE((1ULL << 30) & board.get_bitboard(Board::QUEEN));
    REQUIRE_FALSE((1ULL << 17) & board.get_bitboard(Board::QUEEN));
    REQUIRE((1ULL << 3) & board.get_bitboard(Board::QUEEN));
}