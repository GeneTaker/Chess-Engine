#ifndef PIECE_TYPE_H
#define PIECE_TYPE_H

#include <cstdint>

enum class PieceType : uint8_t {
    PAWN = 0,
    ROOK = 1,
    KNIGHT = 2,
    BISHOP = 3,
    QUEEN = 4,
    KING = 5,
    BLACK_SHIFT = 6
};

#endif