#ifndef MOVE_H
#define MOVE_H

#include "piece_type.h"

struct Move {
    int from;
    int to;
    PieceType type;
    PieceType promotion;
    Move(int f, int t, PieceType ty) : from(f), to(t), type(ty), promotion(PieceType::None) {}
    Move(int f, int t, PieceType ty, PieceType p) : from(f), to(t), type(ty), promotion(p) {}
};

#endif