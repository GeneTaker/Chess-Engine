#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <iostream>
#include <cstdint>
#include "board.h"

class Evaluator {
private:
    static constexpr int DANGER = 17;
    static constexpr int PROTECTION = 7;
    static constexpr int CENTRE = 8;
    static constexpr int OCCUPIED_CENTRE = 18;
    
    static constexpr int QUEEN = 900;
    static constexpr int ROOK = 500;
    static constexpr int BISHOP = 340;
    static constexpr int KNIGHT = 320;
    static constexpr int PAWN = 100;

    static constexpr int QUEEN_CONTROL = 8;
    static constexpr int ROOK_CONTROL = 5;
    static constexpr int BISHOP_CONTROL = 3;

    static constexpr int PAWN_SEVENTH_RANK = 120;
    static constexpr int PAWN_SIXTH_RANK = 80;
    static constexpr int PAWN_FIFTH_RANK = 15;
    static constexpr int DOUBLED_PAWN = 30;

    static constexpr int KING_PENALTY = 50;

    
public:
    int evaluate_position(Board& board, bool is_white);
    int king_safety(Board& board, bool is_white);
    int reduce_squares(Board& board, uint64_t zone, int score_constant, bool is_white);
    int centre_control(Board& board, bool is_white);
    int material_evaluation(Board& board, bool is_white);
    int line_control(Board& board, bool is_white);
    int pawn_structure(Board& board, bool is_white);
};


#endif