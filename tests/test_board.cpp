#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"

#include "../include/board.h"

TEST_CASE("Board test, ensure board is initialised properly") {
    Board board;

    uint64_t white = board.get_occupancy(true);
    uint64_t black = board.get_occupancy(false);
    
    uint64_t white_pawns = board.get_bitboard(Board::PAWN);
    uint64_t black_pawns = board.get_bitboard(Board::BLACK_PAWN);
    
    for (int i = 0; i < 8; i++) {
        REQUIRE(white_pawns & (1ULL << (8 + i)));
        REQUIRE(white & (1ULL << (8 + i)));
        REQUIRE(black_pawns & (1ULL << (48 + i)));
        REQUIRE(black & (1ULL << (48 + i)));
    }
    
    uint64_t white_queens = board.get_bitboard(Board::QUEEN);
    uint64_t black_queens = board.get_bitboard(Board::BLACK_QUEEN);
    
    REQUIRE(white_queens & (1ULL << 3));
    REQUIRE(black_queens & (1ULL << 59));
    REQUIRE(white & (1ULL << 3));
    REQUIRE(black & (1ULL << 59));

    uint64_t white_kings = board.get_bitboard(Board::KING);
    uint64_t black_kings = board.get_bitboard(Board::BLACK_KING);
    
    REQUIRE(white_kings & (1ULL << 4));
    REQUIRE(black_kings & (1ULL << 60));
    REQUIRE(white & (1ULL << 4));
    REQUIRE(black & (1ULL << 60));

    uint64_t white_knights = board.get_bitboard(Board::KNIGHT);
    uint64_t black_knights = board.get_bitboard(Board::BLACK_KNIGHT);
    
    REQUIRE(white_knights & (1ULL << 1));
    REQUIRE(black_knights & (1ULL << 57));
    REQUIRE(white_knights & (1ULL << 6));
    REQUIRE(black_knights & (1ULL << 62));
    REQUIRE(white & (1ULL << 1));
    REQUIRE(black & (1ULL << 57));
    REQUIRE(white & (1ULL << 6));
    REQUIRE(black & (1ULL << 62));


    uint64_t white_bishops = board.get_bitboard(Board::BISHOP);
    uint64_t black_bishops = board.get_bitboard(Board::BLACK_BISHOP);
    
    REQUIRE(white_bishops & (1ULL << 2));
    REQUIRE(black_bishops & (1ULL << 58));
    REQUIRE(white_bishops & (1ULL << 5));
    REQUIRE(black_bishops & (1ULL << 61));
    REQUIRE(white & (1ULL << 2));
    REQUIRE(black & (1ULL << 58));
    REQUIRE(white & (1ULL << 5));
    REQUIRE(black & (1ULL << 61));
}