#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <iostream>
#include <string>
#include "move.h"

using namespace std;

class Board {
private:
    static constexpr int UNIQUE_PIECES = 12;
    
    static constexpr int SIDES = 2;
    static constexpr int WHITE = 0;
    static constexpr int BLACK = 1;

    uint64_t bitboards[UNIQUE_PIECES] = {};
    uint64_t occupied[SIDES] = {};

    
public:
    
    static constexpr int PAWN = 0;
    static constexpr int ROOK = 1;
    static constexpr int KNIGHT = 2;
    static constexpr int BISHOP = 3;
    static constexpr int QUEEN = 4;
    static constexpr int KING = 5;
    static constexpr int BLACK_SHIFT = 6;

    static constexpr int BLACK_PAWN = PAWN + BLACK_SHIFT;
    static constexpr int BLACK_ROOK = ROOK + BLACK_SHIFT;
    static constexpr int BLACK_KNIGHT = KNIGHT + BLACK_SHIFT;
    static constexpr int BLACK_BISHOP = BISHOP + BLACK_SHIFT;
    static constexpr int BLACK_QUEEN = QUEEN + BLACK_SHIFT;
    static constexpr int BLACK_KING = KING + BLACK_SHIFT;

    // initialises the initial state of the board
    Board();

    // updates the board with a move
    bool move(Move move, bool is_white);

    // prints all bitboards for debugging purposes
    void print_bitboards();

    // gets a particular bitboard from the board
    uint64_t get_bitboard(int index);

    // gets the occupancy table for a side
    uint64_t get_occupancy(bool is_white);

    // returns the index of a coloured piecetype within the bitboard array
    static constexpr int bb_index(int type, bool is_white);
};

#endif