#ifndef MOVE_H
#define MOVE_H

#define NONE -1

struct Move {
    int from;
    int to;
    int type;
    int promotion;
    Move(int f, int t, int ty) : from(f), to(t), type(ty), promotion(NONE) {}
    Move(int f, int t, int ty, int p) : from(f), to(t), type(ty), promotion(p) {}
};

#endif