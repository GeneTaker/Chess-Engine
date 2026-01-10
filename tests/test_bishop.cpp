#include "catch_amalgamated.hpp"

#include "../include/board.h"

TEST_CASE("Bishop test, ensure bishops are capable of moving") {
    Board board;

    Move e4(12, 28, Board::PAWN);
    Move e5(51, 35, Board::PAWN);
    Move bc4(5, 26, Board::BISHOP);
    Move bc5(61, 34, Board::BISHOP);

    board.move(e4, true);
    board.move(e5, false);

    bool check1 = board.move(bc4, true);
    REQUIRE(check1);

    uint64_t white_bishops = board.get_bitboard(Board::BISHOP);
    REQUIRE(white_bishops & (1ULL << 26));
    REQUIRE_FALSE(white_bishops & (1ULL << 5));

    bool check2 = board.move(bc5, false);
    REQUIRE(check2);

    uint64_t black_bishops = board.get_bitboard(Board::BLACK_BISHOP);
    REQUIRE(black_bishops & (1ULL << 34));
    REQUIRE_FALSE(white_bishops & (1ULL << 61));
}

TEST_CASE("Bishop test, blocked bishop (by own pieces)") {
    Board board;

    Move bf4(2, 29, Board::BISHOP);
    bool b = board.move(bf4, true);
    Move bf5(58, 37, Board::BISHOP);
    bool b2 = board.move(bf5, false);

    REQUIRE_FALSE(b);
    REQUIRE_FALSE(b2);
}

TEST_CASE("Bishop test, ensure bishops can take pieces but are blocked by opposing pieces") {
    Board board;

    Move e4(12, 28, Board::PAWN);
    Move e5(52, 36, Board::PAWN);
    Move bc4(5, 26, Board::BISHOP);
    Move d5(51, 35, Board::PAWN);
    Move bxd5(26, 35, Board::BISHOP);

    board.move(e4, true);
    board.move(e5, false);
    board.move(bc4, true);
    board.move(d5, false);

    Move invalid(35, 53, Board::BISHOP);
    bool inv = board.move(invalid, true);
    REQUIRE_FALSE(inv);
    uint64_t curr = board.get_bitboard(Board::BISHOP);

    REQUIRE(curr & (1ULL << 35));
    REQUIRE_FALSE(curr & (1ULL << 53));

    bool b = board.move(bxd5, true);
    
    REQUIRE(b);

    uint64_t white_bishops = board.get_bitboard(Board::BISHOP);
    uint64_t black = board.get_bitboard(Board::BLACK_PAWN);
    REQUIRE((1ULL << 35) & white_bishops);
    REQUIRE_FALSE((1ULL << 35) & black);
}

TEST_CASE("Bishop test, ensure bishops cannot wrap around the sides of the board") {
    Board board;

    Move e3(12, 20, Board::PAWN);
    Move be2(5, 12, Board::BISHOP);
    Move invalid(12, 48, Board::BISHOP);

    board.move(e3, true);
    board.move(be2, true);
    bool b = board.move(invalid, true);

    REQUIRE_FALSE(b);

    uint64_t white_bishops = board.get_bitboard(Board::BISHOP);
    REQUIRE_FALSE(white_bishops & (1ULL << 48));
    REQUIRE(white_bishops & (1ULL << 12));
}   