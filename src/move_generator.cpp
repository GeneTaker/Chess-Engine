#include "../include/board.h"
#include "../include/move_generator.h"
#include <vector>

#define UP 1
#define DOWN -1

#define NORTH 8
#define EAST 1
#define SOUTH -8
#define WEST -1

void MoveGenerator::generate_pawn_moves(int from, bool is_white, std::vector<Move>& moves) {
    int direction = (is_white) ? UP : DOWN;
    
    std::vector<int> offsets = {7, 8, 9};

    int second_rank = (is_white) ? 1 : 6;
    int rank = from / Board::SIDE;
    if (rank == second_rank) offsets.push_back(16);

    int from_file = from % Board::SIDE;

    for (int o : offsets) {
        int to = from + o * direction;

        if (to < 0 || to >= 64) continue;

        int to_file = to % Board::SIDE;

        if (o == 7 || o == 9) {
            if (abs(to_file - from_file) != 1) continue; 
        }

        if (to / Board::SIDE == 0 || to / Board::SIDE == (Board::SIDE - 1)) {
            vector<int> promotables = {Board::KNIGHT, Board::ROOK, Board::QUEEN, Board::BISHOP};

            for (int p : promotables) {
                Move m(from, to, Board::PAWN, p);
                moves.push_back(m);
            }
        } else {
            Move m(from, to, Board::PAWN);
            moves.push_back(m);
        }
    }
}

void MoveGenerator::generate_knight_moves(int from, std::vector<Move>& moves) {
    std::vector<int> offsets = {6, 10, 15, 17};

    for (int o : offsets) {
        Move m1(from, from + o, Board::KNIGHT);
        Move m2(from, from - o, Board::KNIGHT);

        moves.push_back(m1);
        moves.push_back(m2);
    }
}

void MoveGenerator::generate_rook_moves(int from, std::vector<Move>& moves) {
    generate_cardinals(from, moves, Board::ROOK);
}

void MoveGenerator::generate_cardinals(int from, std::vector<Move>& moves, int piece) {
    int file = from % Board::SIDE;
    int rank = from / Board::SIDE;
    
    for (int r = 0; r < Board::SIDE; r++) {
        if (r != rank) {
            Move m(from, r * Board::SIDE + file, piece);
            moves.push_back(m);
        }
    }
    
    for (int f = 0; f < Board::SIDE; f++) {
        if (f != file) {
            Move m(from, rank * Board::SIDE + f, piece);
            moves.push_back(m);
        }
    }
    
}

void MoveGenerator::generate_bishop_moves(int from, std::vector<Move>& moves) {
    generate_diagonals(from, moves, Board::BISHOP);
}


void MoveGenerator::generate_diagonals(int from, std::vector<Move>& moves, int piece) {
    int file = from % Board::SIDE;
    int rank = from / Board::SIDE;
    
    //NE
    for (int x = file + 1, y = rank + 1; x < Board::SIDE && y < Board::SIDE; x++, y++) {
        Move m(from, x + Board::SIDE * y, piece);
        moves.push_back(m);
    }
    
    //NW
    for (int x = file - 1, y = rank + 1; x >= 0 && y < Board::SIDE; x--, y++) {
        Move m(from, x + Board::SIDE * y, piece);
        moves.push_back(m);
    }
    
    //SW
    for (int x = file - 1, y = rank - 1; x >= 0 && y >= 0; x--, y--) {
        Move m(from, x + Board::SIDE * y, piece);
        moves.push_back(m);
    }
    
    //SE    
    for (int x = file + 1, y = rank - 1; x < Board::SIDE && y >= 0; x++, y--) {
        Move m(from, x + Board::SIDE * y, piece);
        moves.push_back(m);
    }    

}


void MoveGenerator::generate_queen_moves(int from, std::vector<Move>& moves) {
    generate_diagonals(from, moves, Board::QUEEN);
    generate_cardinals(from, moves, Board::QUEEN);
}

void MoveGenerator::generate_king_moves(int from, bool is_white, std::vector<Move>& moves) {
    std::vector<int> offsets = {1, 7, 8, 9};

    for (int o : offsets) {
        Move pos(from, from + o, Board::KING);
        Move neg(from, from - o, Board::KING);

        moves.push_back(pos);
        moves.push_back(neg);
    }

    if (is_white && from == 4) {
        moves.push_back(Move(4, 6, Board::KING));
        moves.push_back(Move(4, 2, Board::KING));
    } else if (!is_white && from == 60) {
        moves.push_back(Move(60, 62, Board::KING));
        moves.push_back(Move(60, 58, Board::KING));
    }
}

std::vector<Move> MoveGenerator::generate_moves(Board& board, bool is_white) {
    std::vector<Move> result;

    for (int i = 0; i < Board::PIECES; i++) {
        int index = board.bb_index(i, is_white);
        uint64_t bitboard = board.get_bitboard(index);
        uint64_t temp = bitboard;

        while (temp != 0) {
            int from = Board::pop_bit(&temp);

            switch (i) {
                case Board::PAWN:
                    generate_pawn_moves(from, is_white, result);
                    break;  
                case Board::ROOK:
                    generate_rook_moves(from, result);
                    break;  
                case Board::BISHOP:
                    generate_bishop_moves(from, result);
                    break;  
                case Board::KNIGHT:
                    generate_knight_moves(from, result);
                    break;  
                case Board::QUEEN:
                    generate_queen_moves(from, result);
                    break;  
                case Board::KING:
                    generate_king_moves(from, is_white, result);
                    break;  
            }
        }
    }

    return result;
}