#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"

#include "../include/board.h"
#include <vector>

TEST_CASE("Pawn test, ensure Pawns can properly move") {
    Board board;
    Move m(12, 28, Board::PAWN); // e4
    Move m2(13, 21, Board::PAWN); // f3
    board.move(m, true);
    board.move(m2, true);

    uint64_t white_pawns = board.get_bitboard(Board::PAWN);

    REQUIRE(white_pawns & (1ULL << 28));
    REQUIRE_FALSE(white_pawns & (1ULL << 12));
    
    REQUIRE(white_pawns & (1ULL << 21));
    REQUIRE_FALSE(white_pawns & (1ULL << 13));
}

TEST_CASE("Pawn test, ensure both sides can play") {
    Board board;
    Move m(12, 28, Board::PAWN); // e4
    Move m2(52, 36, Board::PAWN); // e5
    board.move(m, true);
    board.move(m2, false);

    uint64_t white_pawns = board.get_bitboard(Board::PAWN);
    uint64_t black_pawns = board.get_bitboard(Board::BLACK_PAWN);

    REQUIRE(white_pawns & (1ULL << 28));
    REQUIRE_FALSE(white_pawns & (1ULL << 12));
    
    REQUIRE(black_pawns & (1ULL << 36));
    REQUIRE_FALSE(white_pawns & (1ULL << 52));
}

TEST_CASE("Pawn test, ensure taking pieces is functional") {
    Board board;
    Move m(12, 28, Board::PAWN); // e4
    Move m2(51, 35, Board::PAWN); // d5
    board.move(m, true);
    board.move(m2, false);

    uint64_t white_pawns = board.get_bitboard(Board::PAWN);
    uint64_t black_pawns = board.get_bitboard(Board::BLACK_PAWN);

    REQUIRE(white_pawns & (1ULL << 28));
    REQUIRE_FALSE(white_pawns & (1ULL << 12));
    
    REQUIRE(black_pawns & (1ULL << 35));
    REQUIRE_FALSE(black_pawns & (1ULL << 51));

    Move take(28, 35, Board::PAWN);
    
    board.move(take, true); // xd5

    white_pawns = board.get_bitboard(Board::PAWN);
    black_pawns = board.get_bitboard(Board::BLACK_PAWN);

    REQUIRE(white_pawns & (1ULL << 35));
    REQUIRE_FALSE(white_pawns & (1ULL << 28));
    REQUIRE_FALSE(black_pawns & (1ULL << 28));

    uint64_t black_occupancy = board.get_occupancy(false);
    REQUIRE_FALSE(black_occupancy & (1ULL << 28));

    Move m3(52, 44, Board::BLACK_PAWN);
    Move take2(44, 35, Board::BLACK_PAWN);
    board.move(m3, false); // e6
    board.move(take2, false); // xd5
}

TEST_CASE("Pawn test, blocked pawn") {
    Board board;
    Move m(12, 28, Board::PAWN); // e4
    Move m2(52, 36, Board::PAWN); // e5
    bool b1 = board.move(m, true);
    bool b2 = board.move(m2, false);

    Move m3(28, 36, Board::PAWN);
    bool b3 = board.move(m3, true);

    uint64_t white_pawns = board.get_bitboard(Board::PAWN);
    uint64_t black_pawns = board.get_bitboard(Board::BLACK_PAWN);

    REQUIRE(white_pawns & (1ULL << 28));
    REQUIRE_FALSE(white_pawns & (1ULL << 12));
    
    REQUIRE(black_pawns & (1ULL << 36));
    REQUIRE_FALSE(black_pawns & (1ULL << 52));

    REQUIRE(b1);
    REQUIRE(b2);
    REQUIRE_FALSE(b3);
}

TEST_CASE("Pawn test, pawn tries to double jump") {
    Board board;
    Move m(12, 28, Board::PAWN); // e4
    Move m3(28, 44, Board::PAWN); // e6
    Move m2(51, 35, Board::PAWN); // e5
    bool b1 = board.move(m, true);
    bool b2 = board.move(m2, false);
    bool b3 = board.move(m3, true);

    uint64_t white_pawns = board.get_bitboard(Board::PAWN);
    uint64_t black_pawns = board.get_bitboard(Board::BLACK_PAWN);

    REQUIRE(white_pawns & (1ULL << 28));
    REQUIRE_FALSE(white_pawns & (1ULL << 44));
    
    REQUIRE(black_pawns & (1ULL << 36));
    REQUIRE_FALSE(black_pawns & (1ULL << 52));

    REQUIRE(b1);
    REQUIRE(b2);
    REQUIRE_FALSE(b3);
}


TEST_CASE("Pawn test, en passant") {
    Board board;
    Move m(12, 28, Board::PAWN); // e4
    Move m3(28, 36, Board::PAWN); // e5
    Move m5(36, 45, Board::PAWN); // en passant
    Move m2(51, 35, Board::PAWN); // d5
    Move m4(53, 37, Board::PAWN); // f5

    board.move(m, true);
    board.move(m2, false);
    board.move(m3, true);
    board.move(m4, false);
    bool b = board.move(m5, true);

    REQUIRE(b);

    uint64_t white_pawns = board.get_bitboard(Board::PAWN);
    uint64_t black_pawns = board.get_bitboard(Board::BLACK_PAWN);

    REQUIRE(white_pawns & (1ULL << 45));
    REQUIRE_FALSE(white_pawns & (1ULL << 36));
    
    REQUIRE_FALSE(black_pawns & (1ULL << 37));
}

TEST_CASE("Pawn test, promotion") {
    Board board;
    Move m(8, 24, Board::PAWN); // a4
    Move m2_invalid(24, 32, Board::PAWN, Board::QUEEN); // a5, invalid promotion
    Move m2(24, 32, Board::PAWN); // a5
    Move m3(32, 40, Board::PAWN); // a6
    Move m4(40, 48, Board::PAWN); // b7
    board.move(m, true);
    REQUIRE_FALSE(board.move(m2_invalid, true));
    board.move(m2, true);
    board.move(m3, true);
    board.move(m4, true);

    std::vector<int> promos = {Board::QUEEN, Board::ROOK, Board::KNIGHT, Board::BISHOP};
    
    for (auto& p : promos) {
        Board temp = board;
        Move m5(48, 56, Board::PAWN, p); // promoted to queen on a8
        bool b = temp.move(m5, true);
    
        REQUIRE(b);
        
        uint64_t white_p = temp.get_bitboard(p);
        uint64_t white_pawns = temp.get_bitboard(Board::PAWN);
        
        REQUIRE(white_p & (1ULL << 56));
        REQUIRE_FALSE(white_pawns & (1ULL << 56));
    }
}
