#include "../include/board.h"

    // static constexpr int UNIQUE_PIECES = 12;

    // enum class PieceType : uint8_t {
    //     PAWN = 0,
    //     ROOK = 1,
    //     KNIGHT = 2,
    //     BISHOP = 3,
    //     QUEEN = 4,
    //     KING = 5,
    //     BLACK_SHIFT = 6
    // };

    // uint64_t bitboards[UNIQUE_PIECES] = {};

Board::Board() {
    //TODO
}

bool Board::move(Move move, bool is_white) {
    //TODO
    return false;
}

void Board::print_bitboards() {
    //TODO
}

uint64_t Board::get_bitboard(int index) {
    return 0ULL;
}

uint64_t Board::get_occupancy(bool is_white) {
    return 0ULL;
}

static constexpr int bb_index(PieceType type, bool is_white) {
    return 0;
}


