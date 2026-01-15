#include "../include/evaluator.h"

int Evaluator::king_safety(Board& board, bool is_white) {
    uint64_t adjacent = board.king_adjacent(is_white);
    uint64_t own_occ = board.get_occupancy(is_white);

    int score = 0;

    uint64_t protection = adjacent & own_occ;
    int protectors = __builtin_popcountll(protection);
    score += protectors * Evaluator::PROTECTION;

    uint64_t danger_zone = (1ULL << board.king_position(is_white)) | adjacent;
    score -= reduce_squares(board, danger_zone, Evaluator::DANGER, is_white); 

    return score;
}

int Evaluator::reduce_squares(Board& board, uint64_t zone, int score_constant, bool is_white) {
    int val = 0;

    while (zone) {
        int position = __builtin_ctzll(zone);
        zone &= zone - 1;

        if (board.is_attacked(position, is_white)) {
            val += score_constant;
        }
    }
    return val;
}

int Evaluator::centre_control(Board& board, bool is_white) {
    uint64_t centre = 0x0000001818000000ULL;
    uint64_t own = board.get_occupancy(is_white);

    uint64_t occupied_centre = centre & own;
    int occupied_count = __builtin_popcountll(occupied_centre);
    
    int centre_attacked = reduce_squares(board, centre, Evaluator::CENTRE, is_white);
    
    return centre_attacked + occupied_count * Evaluator::OCCUPIED_CENTRE;
}

int Evaluator::material_evaluation(Board& board, bool is_white) {
    int score = 0;

    for (int i = 0; i < Board::PIECES; i++) {
        int piece_type = board.bb_index(i, is_white);
        uint64_t pieces = board.get_bitboard(piece_type);
        int count = __builtin_popcountll(pieces);

        int to_add = 0;
        
        switch (i) {
            case Board::PAWN:
                to_add = Evaluator::PAWN;
                break;
            case Board::KNIGHT:
                to_add = Evaluator::KNIGHT;
                break;
            case Board::BISHOP:
                to_add = Evaluator::BISHOP;
                break;
            case Board::ROOK:
                to_add = Evaluator::ROOK;
                break;
            case Board::QUEEN:
                to_add = Evaluator::QUEEN;
                break;
            default:
                break;
        }

        score += to_add * count;
    }

    return score;
}

int Evaluator::line_control(Board& board, bool is_white) {
    uint64_t bishops = board.get_bitboard(board.bb_index(Board::BISHOP, is_white));
    uint64_t rooks = board.get_bitboard(board.bb_index(Board::ROOK, is_white));
    uint64_t queens = board.get_bitboard(board.bb_index(Board::QUEEN, is_white));

    int score = 0;
    
    while (bishops) {
        int square = Board::pop_bit(&bishops);
        uint64_t attacks = board.get_bishop_attacks(square);
        score += __builtin_popcountll(attacks) * BISHOP_CONTROL;
    }

    while (rooks) {
        int square = Board::pop_bit(&rooks);
        uint64_t attacks = board.get_rook_attacks(square);
        score += __builtin_popcountll(attacks) * ROOK_CONTROL;
    }

    while (queens) {
        int square = Board::pop_bit(&queens);
        uint64_t attacks = board.get_queen_attacks(square);
        score += __builtin_popcountll(attacks) * QUEEN_CONTROL;
    }

    return score;
}

int Evaluator::pawn_structure(Board& board, bool is_white) {
    uint64_t pawns = board.get_bitboard(board.bb_index(Board::PAWN, is_white));
    
    int score = 0;

    int final_rank = (is_white) ? Board::SIDE - 1 : 0;
    while (pawns) {
        int square = Board::pop_bit(&pawns);
        int rank = square / Board::SIDE;
        if (abs(rank - final_rank) == 1) {
            score += PAWN_SIXTH_RANK;
        } else if (abs(rank - final_rank) == 2) {
            score += PAWN_FIFTH_RANK;
        }

        uint64_t file_mask = 0x0101010101010101ULL;
        int curr_file = square % Board::SIDE;

        uint64_t curr_file_mask = file_mask << curr_file;
        
        if (curr_file_mask & pawns) {
            score -= DOUBLED_PAWN;
        }
    }

    return score;
}

int Evaluator::evaluate_position(Board& board, bool is_white) {
    int score = king_safety(board, is_white) - king_safety(board, !is_white) +
        centre_control(board, is_white) - centre_control(board, !is_white) +
        material_evaluation(board, is_white) - material_evaluation(board, !is_white) +
        line_control(board, is_white) - line_control(board, !is_white) +
        pawn_structure(board, is_white) - pawn_structure(board, !is_white);

    return score;
}