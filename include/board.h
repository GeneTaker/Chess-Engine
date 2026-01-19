#ifndef BOARD_H
#define BOARD_H

#include <cstdint>
#include <iostream>
#include <string>
#include "move.h"
#include <vector>
#include "past_move.h"
#include "move_generator.h"

using namespace std;

class Board {
private:
    static constexpr int UNIQUE_PIECES = 12;

    uint64_t bitboards[UNIQUE_PIECES] = {};
    uint64_t white_bitboard = 0ULL;
    uint64_t black_bitboard = 0ULL;

    static constexpr int TILES = 64;

    uint64_t knight_attacks[TILES] = {};
    uint64_t king_attacks[TILES] = {};

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
    
    uint64_t bishop_masks[TILES];

    static uint64_t bishop_attacks[TILES][512];

    const uint64_t bishop_magic[TILES] = {
        0x40040844404084ULL,   0x2004208a004208ULL,   0x10190041080202ULL,   0x108060845042010ULL,
        0x581104180800210ULL,  0x2112080446200010ULL, 0x1080820820060210ULL, 0x3c0808410220200ULL,
        0x4050404440404ULL,    0x21001420088ULL,      0x24d0080801082102ULL, 0x1020a0a020400ULL,
        0x40308200402ULL,      0x4011002100800ULL,    0x401484104104005ULL,  0x801010402020200ULL,
        0x400210c3880100ULL,   0x404022024108200ULL,  0x810018200204102ULL,  0x4002801a02003ULL,
        0x85040820080400ULL,   0x810102c808880400ULL, 0xe900410884800ULL,    0x8002020480840102ULL,
        0x220200865090201ULL,  0x2010100a02021202ULL, 0x152048408022401ULL,  0x20080002081110ULL,
        0x4001001021004000ULL, 0x800040400a011002ULL, 0xe4004081011002ULL,   0x1c004001012080ULL,
        0x8004200962a00220ULL, 0x8422100208500202ULL, 0x2000402200300c08ULL, 0x8646020080080080ULL,
        0x80020a0200100808ULL, 0x2010004880111000ULL, 0x623000a080011400ULL, 0x42008c0340209202ULL,
        0x209188240001000ULL,  0x400408a884001800ULL, 0x110400a6080400ULL,   0x1840060a44020800ULL,
        0x90080104000041ULL,   0x201011000808101ULL,  0x1a2208080504f080ULL, 0x8012020600211212ULL,
        0x500861011240000ULL,  0x180806108200800ULL,  0x4000020e01040044ULL, 0x300000261044000aULL,
        0x802241102020002ULL,  0x20906061210001ULL,   0x5a84841004010310ULL, 0x4010801011c04ULL,
        0xa010109502200ULL,    0x4a02012000ULL,       0x500201010098b028ULL, 0x8040002811040900ULL,
        0x28000010020204ULL,   0x6000020202d0240ULL,  0x8918844842082200ULL, 0x4010011029020020ULL
    };

    const int bishop_shift[TILES] = {
        6, 5, 5, 5, 5, 5, 5, 6,
        5, 5, 5, 5, 5, 5, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 9, 9, 7, 5, 5,
        5, 5, 7, 7, 7, 7, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5,
        6, 5, 5, 5, 5, 5, 5, 6
    };

    static constexpr int SIDES = 2;
    
    static constexpr int CASTLE_WS = (1ULL << 0);
    static constexpr int CASTLE_WL = (1ULL << 1);
    static constexpr int CASTLE_BS = (1ULL << 2);
    static constexpr int CASTLE_BL = (1ULL << 3);
    uint8_t castle_rights = 0xF;

    int turns_since_capture = 0;

    
    bool legal_rook_move(Move move);
    bool legal_knight_move(Move move);
    bool legal_queen_move(Move move);
    bool legal_bishop_move(Move move);
    bool legal_king_move(Move move, bool is_white);
    
    
    bool can_promote(Move move, bool is_white);
    
    // initialises a board of possible knight attacks from every square
    void init_knight_moves(int index);
    void init_rook_moves(); 
    void init_bishop_moves(); 
    void init_king_moves(); 
    void init_pawn_attacks();
    void pawn_helper(vector<int> offsets, int colour, int index);
    
    int num_bits(uint64_t mask);

    uint64_t set_occupancy(uint64_t mask, int bits, int index);
    
    uint64_t find_rook_attacks(int sq, uint64_t blockers);
    uint64_t find_bishop_attacks(int sq, uint64_t blockers);
    
    
    uint64_t find_rook_mask(int index);
    uint64_t find_bishop_mask(int index);
    
    static constexpr uint64_t FILE_A = 0x0101010101010101ULL;
    static constexpr uint64_t FILE_H = 0x8080808080808080ULL;
    uint64_t pawn_attacks[SIDES][TILES] = {};

    bool check_evade(Move move, bool is_white);
    bool legal_pawn_move(Move move, bool is_white);
    bool is_legal_move(Move move, bool is_white);
  
    bool bitboards_equal(Board board);

    void print_row(int r);

public:
    bool is_attacked(int square, bool is_white);

    vector<PastMove> move_history;
    uint16_t enpassant = 0;

    static constexpr int PAWN = 0;
    static constexpr int ROOK = 1;
    static constexpr int KNIGHT = 2;
    static constexpr int BISHOP = 3;
    static constexpr int QUEEN = 4;
    static constexpr int KING = 5;
    static constexpr int BLACK_SHIFT = 6;
    static constexpr int PIECES = 6;
    static constexpr int SIDE = 8;
    
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

    // makes a move regardless of legality
    void make_move_unchecked(Move move, bool is_white);
    
    // prints all bitboards for debugging purposes
    void print_board(bool is_white);

    std::string get_piece_at(int square);

    // gets a particular bitboard from the board
    uint64_t get_bitboard(int index);

    // gets the occupancy table for a side
    uint64_t get_occupancy(bool is_white);
    
    // returns the index of a coloured piecetype within the bitboard array
    int bb_index(int type, bool is_white);

    // returns whether or not it is currently checkmate
    bool in_check(bool is_white);

    // reverts the board to its state before the last made move
    void unmake_move();

    // gets the number of turns since the last made capture
    int get_turns_since_capture();

    // returns whether or not it is currently checkmate
    bool is_checkmate(bool is_white);
    
    // returns whether or not it is currently stalemate
    bool is_stalemate(bool is_white);
    
    // returns all legal moves
    vector<Move> find_legal_moves(bool is_white);

    // returns all legal capture moves 
    vector<Move> find_legal_attacks(bool is_white);

    // checks whether it is a draw, through repeated moves
    bool is_three_fold();
    
    bool is_over(bool is_white);
    
    bool is_draw(bool is_white);

    bool insufficient_material();
    
    uint64_t get_rook_attacks(int square);
    
    uint64_t get_bishop_attacks(int square);
    
    uint64_t get_queen_attacks(int square);
    
    uint64_t king_adjacent(bool is_white);
    
    int king_position(bool is_white);

    static int pop_bit(uint64_t *mask);

    int find_piece_moved(int from);
};

#endif