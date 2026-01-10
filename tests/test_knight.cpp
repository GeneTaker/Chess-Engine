#include "catch_amalgamated.hpp"

#include "../include/board.h"
#include <vector>

TEST_CASE("Knight test, ensure that knights can properly move") {
    Board board;
    Move nc3(1, 18, Board::KNIGHT);

    bool b = board.move(nc3, true);
    REQUIRE(b);

    uint64_t white_knights = board.get_bitboard(Board::KNIGHT);
    REQUIRE(white_knights & (1ULL << 18));
    REQUIRE_FALSE(white_knights & (1ULL << 1));

    uint64_t white_occ = board.get_occupancy(true);
    REQUIRE(white_occ & (1ULL << 18));
    REQUIRE_FALSE(white_occ & (1ULL << 1));

    Move invalid(6, 16, Board::KNIGHT);
    bool fail = board.move(invalid, true);
    REQUIRE_FALSE(fail);
}

TEST_CASE("Knight test, ensure all knights are functional") {
    Board board;

    Move m1(1, 18, Board::KNIGHT);
    Move m2(6, 21, Board::KNIGHT);
    Move m3(57, 42, Board::KNIGHT);
    Move m4(62, 45, Board::KNIGHT);

    board.move(m1, true);
    board.move(m3, false);
    board.move(m2, true);
    board.move(m4, false);

    uint64_t white_knights = board.get_bitboard(Board::KNIGHT);
    REQUIRE(white_knights & (1ULL << 18));
    REQUIRE(white_knights & (1ULL << 21));

    REQUIRE_FALSE(white_knights & (1ULL << 1));
    REQUIRE_FALSE(white_knights & (1ULL << 6));

    uint64_t black_knights = board.get_bitboard(Board::BLACK_KNIGHT);
    REQUIRE(black_knights & (1ULL << 42));
    REQUIRE(black_knights & (1ULL << 45));

    REQUIRE_FALSE(black_knights & (1ULL << 57));
    REQUIRE_FALSE(black_knights & (1ULL << 62));
}

TEST_CASE("Knight test, ensure that knights can take pieces") {
    Board board;

    Move d4(11, 27, Board::PAWN);
    Move nc6(57, 42, Board::KNIGHT);
    Move nf3(6, 21, Board::KNIGHT);
    Move ncxd4(42, 27, Board::KNIGHT);
    Move nfxd4(21, 27, Board::KNIGHT);

    vector<Move> moves = {d4, nc6, nf3, ncxd4, nfxd4};

    board.move(d4, true);
    board.move(nc6, false);
    board.move(nf3, true);
    board.move(ncxd4, false);
    board.move(nfxd4, true);

    uint64_t white_knights = board.get_bitboard(Board::KNIGHT);
    REQUIRE(white_knights & (1ULL << 27));
    REQUIRE_FALSE(white_knights & (1ULL << 21));
    uint64_t black_knights = board.get_bitboard(Board::BLACK_KNIGHT);
    REQUIRE_FALSE(black_knights & (1ULL << 27));
    REQUIRE_FALSE(black_knights & (1ULL << 42));

    uint64_t white_pawns = board.get_bitboard(Board::PAWN);
    REQUIRE_FALSE(white_pawns & (1ULL << 27));
}

TEST_CASE("Knight test, ensure can't take own pieces") {
    Board board;

    Move m1(11, 27, Board::PAWN);
    Move m2(51, 35, Board::PAWN);
    Move m3(6, 21, Board::KNIGHT);
    Move m4(62, 45, Board::KNIGHT);

    Move m5(21, 27, Board::KNIGHT);
    Move m6(45, 35, Board::KNIGHT);

    board.move(m1, true);
    board.move(m2, false);
    board.move(m3, true);
    board.move(m4, false);
    REQUIRE_FALSE(board.move(m5, true));

    uint64_t white_knights = board.get_bitboard(Board::KNIGHT);
    REQUIRE_FALSE(white_knights & (1ULL << 27));
    REQUIRE(white_knights & (1ULL << 21));

    REQUIRE_FALSE(board.move(m6, false));

    uint64_t black_knights = board.get_bitboard(Board::BLACK_KNIGHT);
    REQUIRE_FALSE(black_knights & (1ULL << 35));
    REQUIRE(black_knights & (1ULL << 45));
}
