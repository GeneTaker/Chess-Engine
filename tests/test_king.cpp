#include "catch_amalgamated.hpp"

#include "../include/board.h"

TEST_CASE("King test, ensure kings can move properly") {
    Board board;
    
    Move e4(12, 28, Board::PAWN);
    Move ke2(4, 12, Board::KING);
    
    bool b1 = board.move(e4, true);
    bool b2 = board.move(ke2, true);
    REQUIRE(b1);
    REQUIRE(b2);

    REQUIRE(board.get_occupancy(true) & (1ULL << 12));
    REQUIRE(board.get_bitboard(Board::KING) & (1ULL << 12));

    Move kf4_invalid(12, 29, Board::KING);
    bool b = board.move(kf4_invalid, true);
    REQUIRE_FALSE(b);

    REQUIRE_FALSE(board.get_bitboard(Board::KING) & (1ULL << 29));
    REQUIRE(board.get_bitboard(Board::KING) & (1ULL << 12));
}

TEST_CASE("King test, ensure kings can take other pieces") {
    Board board;
    
    Move nf3(6, 21, Board::KNIGHT);
    Move ne5(21, 36, Board::KNIGHT);
    Move nf7(36, 53, Board::KNIGHT);
    
    board.move(nf3, true);
    board.move(ne5, true);
    board.move(nf7, true);
    
    Move kf7(60, 53, Board::KING);
    bool b = board.move(kf7, false);
    REQUIRE(b);

    REQUIRE((1ULL << 53) & board.get_bitboard(Board::BLACK_KING));
    REQUIRE_FALSE((1ULL << 53) & board.get_bitboard(Board::KNIGHT));
}

TEST_CASE("King test, test check and illegal moves") {
    Board board;
    
    Move e4(12, 28, Board::PAWN);
    Move qf3(3, 21, Board::QUEEN);
    Move qxf7(21, 53, Board::QUEEN);

    board.move(e4, true);
    board.move(qf3, true);
    board.move(qxf7, true);

    Move illegal1(62, 45, false);
    Move illegal2(52, 36, false);

    bool b1 = board.move(illegal1, false);
    bool b2 = board.move(illegal2, false);

    REQUIRE_FALSE(b1);
    REQUIRE_FALSE(b2);

    bool checkmate = board.is_checkmate(false);
    REQUIRE_FALSE(checkmate);

    Move kxf7(60, 53, Board::KING);
    bool b3 = board.move(kxf7, false);
    REQUIRE(b3);

    checkmate = board.is_checkmate(false);
    REQUIRE_FALSE(checkmate);

    REQUIRE((1ULL << 53) & board.get_bitboard(Board::BLACK_KING));
}

TEST_CASE("King test, test checkmate") {
    Board board;

    Move e4(12, 28, Board::PAWN);
    Move qf3(3, 21, Board::QUEEN);
    Move bc4(5, 26, Board::BISHOP);

    board.move(e4, true);
    board.move(qf3, true);
    board.move(bc4, true);

    Move qxf7(21, 53, Board::QUEEN);
    
    board.move(qxf7, true);

    bool mate = board.is_checkmate(false);
    REQUIRE(mate);

    Move kxf7(60, 53, Board::KING);
    bool b = board.move(kxf7, false);
    REQUIRE_FALSE(b);
    REQUIRE_FALSE((1ULL << 53) & board.get_bitboard(Board::BLACK_KING));
    REQUIRE((1ULL << 60) & board.get_bitboard(Board::BLACK_KING));
}

TEST_CASE("King test, test castling both ways") {
    Board board;

    Move e4(12, 28, Board::PAWN);
    Move d4(11, 27, Board::PAWN);
    Move qf3(3, 21, Board::QUEEN);
    Move be3(2, 20, Board::BISHOP);
    Move nc3(1, 18, Board::KNIGHT);

    board.move(e4, true);
    board.move(d4, true);
    board.move(qf3, true);
    board.move(be3, true);

    board.move(nc3, true);

    //castle long

    Move o_o_o(4, 2, Board::KING);
    bool b = board.move(o_o_o, true);
    
    REQUIRE(b);
    
    REQUIRE(board.get_bitboard(Board::KING) & (1ULL << 2));
    REQUIRE_FALSE(board.get_bitboard(Board::KING) & (1ULL << 4));

    REQUIRE(board.get_bitboard(Board::ROOK) & (1ULL << 3));
    REQUIRE_FALSE(board.get_bitboard(Board::ROOK) & (1ULL << 0));

    Move e5(52, 36, Board::PAWN);
    Move nf6(62, 45, Board::KNIGHT);
    Move bd6(61, 43, Board::BISHOP);
    Move o_o(60, 62, Board::KING); 

    board.move(e5, false);
    board.move(nf6, false);
    board.move(bd6, false);

    // castle short
    bool b2 = board.move(o_o, false);
    REQUIRE(b2);

    REQUIRE(board.get_bitboard(Board::BLACK_KING) & (1ULL << 62));
    REQUIRE_FALSE(board.get_bitboard(Board::BLACK_KING) & (1ULL << 60));

    REQUIRE(board.get_bitboard(Board::BLACK_ROOK) & (1ULL << 61));
    REQUIRE_FALSE(board.get_bitboard(Board::BLACK_ROOK) & (1ULL << 63));
}


