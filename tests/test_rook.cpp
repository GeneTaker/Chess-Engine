#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"

#include "../include/board.h"

TEST_CASE("Rook test, test that rooks can move") {
    Board board;

    Move h4(15, 31, Board::PAWN);
    board.move(h4, true);

    Move h5(55, 39, Board::PAWN);
    board.move(h5, false);

    Move rh3(7, 23, Board::ROOK);
    bool b1 = board.move(rh3, true);
    REQUIRE(b1);

    uint64_t white_rooks = board.get_bitboards(Board::ROOK);
    REQUIRE(white_rooks & (1ULL << 23));
    REQUIRE_FALSE(white_rooks & (1ULL << 7));

    Move rh6(63, 47, Board::ROOK);
    bool b2 = board.move(rh6, false);
    REQUIRE(b2);

    uint64_t black_rooks = board.get_bitboards(Board::BLACK_ROOK);
    REQUIRE(black_rooks & (1ULL << 47));
    REQUIRE_FALSE(black_rooks & (1ULL << 63));

    bool black = board.get_occupancy(false);
    bool white = board.get_occupancy(true);
    REQUIRE(black & (1ULL << 47));
    REQUIRE(white & (1ULL << 23));
}

TEST_CASE("Rook test, test that rooks can take") {
    Board board;

    Move h4(15, 31, Board::PAWN);
    board.move(h4, true);

    Move h5(55, 39, Board::PAWN);
    board.move(h5, false);

    Move rh3(7, 23, Board::ROOK);
    board.move(rh3, true);

    Move rh6(63, 47, Board::ROOK);
    board.move(rh6, false);

    Move rg3(23, 22, Board::ROOK);
    board.move(rg3, true);

    Move rg6(47, 46, Board::ROOK);
    board.move(rg6, false);

    Move rxg6(22, 46, Board::ROOK);
    bool check = board.move(rxg3, true);
    REQUIRE(check);

    uint64_t black_rooks = board.get_bitboards(Board::BLACK_ROOK);
    uint64_t white_rooks = board.get_bitboards(Board::ROOK);

    REQUIRE(white_rooks & (1ULL << 46));
    REQUIRE_FALSE(black_rooks & (1ULL << 46));

    Move rxg7(46, 54, Board::ROOK);
    bool check2 = board.move(rxg7, true);
    REQUIRE(check2);
}

TEST_CASE("Rook test, test rooks on the sides of the board") {
    Board board;

    Move a3(8, 16, Board::PAWN);
    board.move(a3, true);

    Move try_a2(7, 8, Board::ROOK);
    bool b = board.move(try_a2, true);
    REQUIRE_FALSE(b);
    
    uint64_t rooks = board.get_bitboard(Board::ROOK);
    REQUIRE_FALSE(rooks & (1ULL << 8));


    Move h4(7, 23, Board::PAWN);
    board.move(h4, true);

    Move ra3(0, 16, Board::PAWN);
    board.move(ra3, true);

    Move try_h2(16, 15, Board::ROOK);
    bool b2 = board.move(try_h2, true);
    REQUIRE_FALSE(b2);
    
    uint64_t rooks = board.get_bitboard(Board::ROOK);
    REQUIRE_FALSE(rooks & (1ULL << 15));
}

