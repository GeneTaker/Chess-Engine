#include "../include/engine.h"
#include "../include/evaluator.h"
#include <vector>


int Engine::search(Board& board, int depth, bool is_white, int alpha, int beta) {
    if (depth == 0) {
        return quiescence(board, is_white, alpha, beta, 0);
    }

    std::vector<Move> all_moves = board.find_legal_moves(is_white);

    if (board.is_checkmate(is_white)) {
        // Worst possible board state
        return -999999;
    } else if (board.is_draw(is_white)) {
        return 0;
    }

    if (all_moves.size() == 0) {
        return 0;
    }

    int best_val = -9999999;

    for (auto m : all_moves) {
        board.make_move_unchecked(m, is_white);
        // Searches from the perspective of the other side after the move
        int score = -search(board, depth - 1, !is_white, -beta, -alpha);
        board.unmake_move();

        // Searches for the best possible value
        best_val = std::max(best_val, score);
        alpha = std::max(alpha, best_val);

        // If the other player can guarantee us a worse position
        if (alpha >= beta) {
            break;
        }
    } 
    return best_val;
}

int Engine::quiescence(Board& board, bool is_white, int alpha, int beta, int q_depth) {
    int initial = evaluator.evaluate_position(board, is_white);

    // If the opponent can guarantee a better board state down the line than our current one,
    // then that figure is most relevant
    if (initial >= beta) return beta;
    // If is our best current move, then remember its value
    if (alpha < initial) alpha = initial;

    // If our current depth is too long (avoids rare search explosions)
    if (q_depth >= MAX_QUIESCENCE) return initial;

    vector<Move> attacks = board.find_legal_attacks(is_white);

    for (auto a : attacks) {
        board.make_move_unchecked(a, is_white);
        //Searches from the other player's perspective
        int score = -quiescence(board, !is_white, -beta, -alpha, q_depth + 1);
        board.unmake_move();

        if (score >= beta) return beta;
        if (alpha < score) alpha = score;
    }

    return alpha;
}

Move Engine::best_move(Board& board, bool is_white) {
    std::vector<Move> all_moves = board.find_legal_moves(is_white);

    Move best(1, 1, Board::KING);
    int best_score = -9999999;

    for (auto move : all_moves) {
        board.make_move_unchecked(move, is_white);

        int score = -search(board, Engine::MAX_DEPTH - 1, !is_white, -999999, 999999);

        board.unmake_move();

        if (best_score < score) {
            best_score = score;
            best = move;            
        }
    }

    return best;
}

uint64_t Engine::perft(Board& board, int depth, bool is_white) {
    if (depth == 0) {
        return 1ULL;
    }

    std::vector<Move> moves = board.find_legal_moves(is_white);

    uint64_t result = 0;

    for (auto m : moves) {
        board.make_move_unchecked(m, is_white);
        
        result += perft(board, depth - 1, !is_white);
        board.unmake_move();
    }

    return result;
}