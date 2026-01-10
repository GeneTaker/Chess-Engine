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

    uint64_t rook_masks[TILES];
    static uint64_t rook_attacks[TILES][4096];

    const uint64_t rook_magic[TILES] = {
        0x0080001020400080ULL, 0x0040001000200040ULL, 0x0080081000200080ULL, 0x0080040800100080ULL,
        0x0080020400080080ULL, 0x0080010200040080ULL, 0x0080008001000200ULL, 0x0080002040800100ULL,
        0x0000800020400080ULL, 0x0000400020005000ULL, 0x0000801000200080ULL, 0x0000800800100080ULL,
        0x0000800400080080ULL, 0x0000800200040080ULL, 0x0000800100020080ULL, 0x0000800040800100ULL,
        0x0000208000400080ULL, 0x0000404000201000ULL, 0x0000808010002000ULL, 0x0000808008001000ULL,
        0x0000808004000800ULL, 0x0000808002000400ULL, 0x0000010100020004ULL, 0x0000020000408104ULL,
        0x0000208080004000ULL, 0x0000200040005000ULL, 0x0000100080200080ULL, 0x0000080080100080ULL,
        0x0000040080080080ULL, 0x0000020080040080ULL, 0x0000010080800200ULL, 0x0000800080004100ULL,
        0x0000204000800080ULL, 0x0000200040401000ULL, 0x0000100080802000ULL, 0x0000080080801000ULL,
        0x0000040080800800ULL, 0x0000020080800400ULL, 0x0000020001010004ULL, 0x0000800040800100ULL,
        0x0000204000808000ULL, 0x0000200040008080ULL, 0x0000100020008080ULL, 0x0000080010008080ULL,
        0x0000040008008080ULL, 0x0000020004008080ULL, 0x0000010002008080ULL, 0x0000004081020004ULL,
        0x0000204000800080ULL, 0x0000200040008080ULL, 0x0000100020008080ULL, 0x0000080010008080ULL,
        0x0000040008008080ULL, 0x0000020004008080ULL, 0x0000800100020080ULL, 0x0000800041000080ULL,
        0x00FFFCDDFCED714AULL, 0x007FFCDDFCED714AULL, 0x003FFFCDFFD88096ULL, 0x0000040810002101ULL,
        0x0001000204080011ULL, 0x0001000204000801ULL, 0x0001000082000401ULL, 0x0001FFFAABFAD1A2ULL
    };

    const int rook_shift[TILES] = {
        12, 11, 11, 11, 11, 11, 11, 12,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        11, 10, 10, 10, 10, 10, 10, 11,
        12, 11, 11, 11, 11, 11, 11, 12
    };

    bool is_legal_move(Move move, bool is_white);
    bool legal_pawn_move(Move move, bool is_white);
    bool legal_rook_move(Move move, bool is_white);
    bool legal_knight_move(Move move, bool is_white);
    bool legal_queen_move(Move move, bool is_white);
    bool legal_bishop_move(Move move, bool is_white);
    bool legal_king_move(Move move, bool is_white);
    
    
    bool can_promote(Move move, bool is_white);
    
    // initialises a board of possible knight attacks from every square
    void init_knight_moves(int index);
    void init_rook_moves();
    
    int num_bits(uint64_t mask);

    uint64_t set_occupancy(uint64_t mask, int bits, int index);

    uint64_t find_rook_attacks(int sq, uint64_t blockers);

    int pop_bit(uint64_t *mask);

    uint64_t find_rook_mask(int index);
    

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
    constexpr int bb_index(int type, bool is_white);
};

#endif