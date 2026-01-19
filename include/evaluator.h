#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <iostream>
#include <cstdint>
#include "board.h"

class Evaluator {
private:
    // The penalty for leaving the king exposed to danger
    static constexpr int DANGER = 17;
    // The bonus for having the king be protected by pieces
    static constexpr int PROTECTION = 7;
    // The bonus for having the centre be attacked by your pieces
    static constexpr int CENTRE = 8;
    // The bonus for having a piece occupy one of the centre squares
    static constexpr int OCCUPIED_CENTRE = 18;

    // The value of each piece
    static constexpr int QUEEN = 900;
    static constexpr int ROOK = 500;
    static constexpr int BISHOP = 340;
    static constexpr int KNIGHT = 320;
    static constexpr int PAWN = 100;

    // The bonus for having a piece have control over a square 
    static constexpr int QUEEN_CONTROL = 8;
    static constexpr int ROOK_CONTROL = 5;
    static constexpr int BISHOP_CONTROL = 3;

    // The bonus for having a pawn on a particular rank
    static constexpr int PAWN_SEVENTH_RANK = 120;
    static constexpr int PAWN_SIXTH_RANK = 80;
    static constexpr int PAWN_FIFTH_RANK = 15;

    // A penalty for having doubled pawns
    static constexpr int DOUBLED_PAWN = 30;

    // The penalty for moving the king, to discourage unnecessary king movement
    static constexpr int KING_PENALTY = 50;

    // Evaluates the safety of the king
    int king_safety(Board& board, bool is_white);

    // A helper function, determines a score based on how many squares of a zone are attacked
    int reduce_squares(Board& board, uint64_t zone, int score_constant, bool is_white);

    // Evaluates how much control the player has over the centre squares
    int centre_control(Board& board, bool is_white);

    // Evaluates a player's material advantage
    int material_evaluation(Board& board, bool is_white);

    // Evaluates the control a player has over the board with sliding pieces
    int line_control(Board& board, bool is_white);

    // Evaluates the structure of a player's pawns on the board
    int pawn_structure(Board& board, bool is_white);
    
public:
    int evaluate_position(Board& board, bool is_white);
};


#endif