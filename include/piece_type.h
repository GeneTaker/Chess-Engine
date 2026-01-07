#ifndef PIECE_TYPE_H
#define PIECE_TYPE_H

#include <cstdint>

enum class PieceType : uint8_t {
    None = -1,
    Pawn = 0,
    Rook = 1,
    Knight = 2,
    Bishop = 3,
    Queen = 4,
    King = 5,
    Black = 6
};

#endif