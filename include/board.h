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
    static constexpr int PIECES = 6;

    uint64_t bitboards[UNIQUE_PIECES] = {};
    uint64_t white_bitboard = 0ULL;
    uint64_t black_bitboard = 0ULL;

    static constexpr int TILES = 64;
    static constexpr int SIDE = 8;

    static constexpr uint64_t FILE_A = 0x0101010101010101ULL;
    static constexpr uint64_t FILE_H = 0x8080808080808080ULL;

    uint64_t knight_attacks[TILES] = {};

    uint16_t enpassant = 0;

    bool is_legal_move(Move move, bool is_white);
    bool legal_pawn_move(Move move, bool is_white);
    bool legal_rook_move(Move move, bool is_white);
    bool legal_knight_move(Move move, bool is_white);
    bool legal_queen_move(Move move, bool is_white);
    bool legal_bishop_move(Move move, bool is_white);
    bool legal_king_move(Move move, bool is_white);


    bool can_promote(Move move, bool is_white);


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

    // initialises a board of possible knight attacks from every square
    void init_knight_moves(int index);

    // updates the board with a move
    bool move(Move move, bool is_white);

    // prints all bitboards for debugging purposes
    void print_bitboards();

    // gets a particular bitboard from the board
    uint64_t get_bitboard(int index);

    // gets the occupancy table for a side
    uint64_t get_occupancy(bool is_white);

    // returns the index of a coloured piecetype within the bitboard array
    constexpr int bb_index(int type, bool is_white);
};

#endif