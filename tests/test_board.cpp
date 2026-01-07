#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"

#include "../include/board.h"
#include "../include/piece_type.h"

TEST_CASE("Board test, ensure board is initialised properly") {
    Board board;
    
    uint64_t white_pawns = board.get_bitboard(static_cast<int>(PieceType::Pawn));
    uint64_t black_pawns = board.get_bitboard(Board::bb_index(PieceType::Pawn, false));
    
    for (int i = 0; i < 8; i++) {
        REQUIRE(white_pawns & (1ULL << (8 + i)));
        REQUIRE(black_pawns & (1ULL << (48 + i)));
    }
    
    REQUIRE_FALSE(white_pawns & (1ULL << (28)));
    REQUIRE_FALSE(white_pawns & (1ULL << (48)));
}

// TEST_CASE("Pawn test, ensure Pawns can properly move") {
//     Board board;
//     Move m(12, 28, PieceType::Pawn); // e4
//     Move m2(13, 21, PieceType::Pawn); // f3
//     board.move(m, true);
//     board.move(m2, true);

//     uint64_t white_pawns = board.get_bitboard(static_cast<int>(PieceType::Pawn));

//     REQUIRE(white_pawns & (1ULL << 28));
//     REQUIRE_FALSE(white_pawns & (1ULL << 12));
    
//     REQUIRE(white_pawns & (1ULL << 21));
//     REQUIRE_FALSE(white_pawns & (1ULL << 13));
// }

// TEST_CASE("Pawn test, ensure both sides can play") {
//     Board board;
//     Move m(12, 28, PieceType::Pawn); // e4
//     Move m2(52, 36, Board::bb_index(PieceType::Pawn, false)); // e5
//     board.move(m, true);
//     board.move(m2, false);

//     uint64_t white_pawns = board.get_bitboard(static_cast<int>(PieceType::Pawn));
//     uint64_t black_pawns = board.get_bitboard(Board::bb_index(PieceType::Pawn, false));

//     REQUIRE(white_pawns & (1ULL << 28));
//     REQUIRE_FALSE(white_pawns & (1ULL << 12));
    
//     REQUIRE(black_pawns & (1ULL << 36));
//     REQUIRE_FALSE(white_pawns & (1ULL << 52));
// }

// TEST_CASE("Pawn test, ensure taking pieces is functional") {
//     Board board;
//     Move m(12, 28, PieceType::Pawn); // e4
//     Move m2(51, 35, Board::bb_index(PieceType::Pawn, false)); // d5
//     board.move(m, true);
//     board.move(m2, false);

//     uint64_t white_pawns = board.get_bitboard(static_cast<int>(PieceType::Pawn));
//     uint64_t black_pawns = board.get_bitboard(Board::bb_index(PieceType::Pawn, false));

//     REQUIRE(white_pawns & (1ULL << 28));
//     REQUIRE_FALSE(white_pawns & (1ULL << 12));
    
//     REQUIRE(black_pawns & (1ULL << 35));
//     REQUIRE_FALSE(black_pawns & (1ULL << 51));

//     Move take(28, 35, PieceType::Pawn);
    
//     board.move(take, true); // xd5

//     white_pawns = board.get_bitboard(static_cast<int>(PieceType::Pawn));
//     black_pawns = board.get_bitboard(Board::bb_index(PieceType::Pawn, false));

//     REQUIRE(white_pawns & (1ULL << 35));
//     REQUIRE_FALSE(white_pawns & (1ULL << 28));
//     REQUIRE_FALSE(black_pawns & (1ULL << 28));

//     uint64_t black_occupancy = board.get_occupancy(false);
//     REQUIRE_FALSE(black_occupancy & (1ULL << 28));

//     Move m3(52, 44, Board::bb_index(PieceType::Pawn, false));
//     Move take2(44, 35, Board::bb_index(PieceType::Pawn, false));
//     board.move(m3, false); // e6
//     board.move(take2, false); // xd5
// }

// TEST_CASE("Pawn test, blocked pawn") {
//     Board board;
//     Move m(12, 28, PieceType::Pawn); // e4
//     Move m2(52, 36, Board::bb_index(PieceType::Pawn, false)); // e5
//     bool b1 = board.move(m, true);
//     bool b2 = board.move(m2, false);

//     Move m3(28, 36, PieceType::Pawn);
//     bool b3 = board.move(m3, true);

//     uint64_t white_pawns = board.get_bitboard(static_cast<int>(PieceType::Pawn));
//     uint64_t black_pawns = board.get_bitboard(Board::bb_index(PieceType::Pawn, false));

//     REQUIRE(white_pawns & (1ULL << 28));
//     REQUIRE_FALSE(white_pawns & (1ULL << 12));
    
//     REQUIRE(black_pawns & (1ULL << 36));
//     REQUIRE_FALSE(black_pawns & (1ULL << 52));

//     REQUIRE(b1 && b2 && !b3);
// }

// TEST_CASE("Pawn test, pawn tries to double jump") {
//     Board board;
//     Move m(12, 28, PieceType::Pawn); // e4
//     Move m3(28, 44, PieceType::Pawn); // e6
//     Move m2(51, 35, Board::bb_index(PieceType::Pawn, false)); // e5
//     bool b1 = board.move(m, true);
//     bool b2 = board.move(m2, false);
//     bool b3 = board.move(m3, true);

//     uint64_t white_pawns = board.get_bitboard(static_cast<int>(PieceType::Pawn));
//     uint64_t black_pawns = board.get_bitboard(Board::bb_index(PieceType::Pawn, false));

//     REQUIRE(white_pawns & (1ULL << 28));
//     REQUIRE_FALSE(white_pawns & (1ULL << 44));
    
//     REQUIRE(black_pawns & (1ULL << 36));
//     REQUIRE_FALSE(black_pawns & (1ULL << 52));

//     REQUIRE(b1 && b2 && !b3);
// }


// TEST_CASE("Pawn test, en passant") {
//     Board board;
//     Move m(12, 28, PieceType::Pawn); // e4
//     Move m3(28, 36, PieceType::Pawn); // e5
//     Move m5(36, 45, PieceType::Pawn); // en passant
//     Move m2(51, 35, Board::bb_index(PieceType::Pawn, false)); // d5
//     Move m4(53, 37, Board::bb_index(PieceType::Pawn, false)); // f5

//     board.move(m, true);
//     board.move(m2, false);
//     board.move(m3, true);
//     board.move(m4, false);
//     bool b = board.move(m5, true);

//     REQUIRE(b);

//     uint64_t white_pawns = board.get_bitboard(static_cast<int>(PieceType::Pawn));
//     uint64_t black_pawns = board.get_bitboard(Board::bb_index(PieceType::Pawn, false));

//     REQUIRE(white_pawns & (1ULL << 45));
//     REQUIRE_FALSE(white_pawns & (1ULL << 36));
    
//     REQUIRE_FALSE(black_pawns & (1ULL << 37));
// }

// TEST_CASE("Pawn test, promotion") {
//     Board board;
//     Move m(8, 24, PieceType::Pawn); // a4
//     Move m2_invalid(24, 32, PieceType::Pawn, PieceType::Queen); // a5, invalid promotion
//     Move m2(24, 32, PieceType::Pawn); // a5
//     Move m3(32, 40, PieceType::Pawn); // a6
//     Move m4(40, 48, PieceType::Pawn); // b7
//     Move m5(48, 56, PieceType::Pawn, PieceType::Queen); // promoted to queen on a8

//     board.move(m, true);
//     board.move(m2, true);
//     board.move(m3, true);
//     board.move(m4, true);
//     bool b = board.move(m5, true);

//     REQUIRE(b);

//     uint64_t white_queens = board.get_bitboard(static_cast<int>(PieceType::Queen));
//     uint64_t white_pawns = board.get_bitboard(static_cast<int>(PieceType::Pawn));

//     REQUIRE(white_queens & (1ULL << 56));
//     REQUIRE_FALSE(white_pawns & (1ULL << 56));
// }
