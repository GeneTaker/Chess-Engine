#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include "move.h"
#include "../include/board.h"
#include <vector>

class Board;

class MoveGenerator {
private:
    int pop_bit(uint64_t* mask);

    // Generates pseudo-legal moves for each piece
    void generate_pawn_moves(int from, bool is_white, std::vector<Move>& moves);
    void generate_knight_moves(int from, std::vector<Move>& moves);
    void generate_rook_moves(int from, std::vector<Move>& moves);
    void generate_bishop_moves(int from, std::vector<Move>& moves);
    void generate_queen_moves(int from, std::vector<Move>& moves);
    void generate_king_moves(int from, bool is_white, std::vector<Move>& moves);

    // Helper methods for the sliding move pieces
    void generate_diagonals(int from, std::vector<Move>& moves, int piece);
    void generate_cardinals(int from, std::vector<Move>& moves, int piece);

public:
    // Generates all pseudo-legal moves for a board state
    std::vector<Move> generate_moves(Board& board, bool is_white);
};

#endif