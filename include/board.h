#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <iostream>
#include <string>
#include "piece_type.h"

using namespace std;

class Board {
private:
    static constexpr int UNIQUE_PIECES = 12;

    uint64_t bitboards[UNIQUE_PIECES] = {};

public:
    Board();

    void move(Move move, bool isWhite);
    void print_bitboards();
}

#endif