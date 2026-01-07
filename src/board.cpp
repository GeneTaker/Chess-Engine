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
    bitboards[Board::PAWN] = 0x000000000000FF00ULL;
    bitboards[Board::BLACK_PAWN] = 0x00FF000000000000ULL;

    bitboards[Board::KNIGHT] = 0x0000000000000042ULL;
    bitboards[Board::BLACK_KNIGHT] = 0x4200000000000000ULL;

    bitboards[Board::ROOK] = 0x0000000000000081ULL;
    bitboards[Board::BLACK_ROOK] = 0x8100000000000000ULL;

    bitboards[Board::BISHOP] = 0x0000000000000024ULL;
    bitboards[Board::BLACK_BISHOP] = 0x2400000000000000ULL;

    bitboards[Board::QUEEN] = 0x0000000000000008ULL;
    bitboards[Board::BLACK_QUEEN] = 0x0800000000000000ULL;

    bitboards[Board::KING] = 0x0000000000000010ULL;
    bitboards[Board::BLACK_KING] = 0x1000000000000000ULL;

    for (int i = 0; i < Board::PIECES; i++) {
        white_bitboard |= bitboards[i];
        black_bitboard |= bitboards[i + Board::PIECES];
    }
}

bool Board::move(Move move, bool is_white) {
    //TODO
    return false;
}

void Board::print_bitboards() {
    //TODO
}

uint64_t Board::get_bitboard(int index) {
    return bitboards[index];
}

uint64_t Board::get_occupancy(bool is_white) {
    return (is_white) ? white_bitboard : black_bitboard;
}

constexpr int Board::bb_index(int type, bool is_white) {
    return 0;
}


