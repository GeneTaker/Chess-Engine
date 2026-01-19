#include <iostream>
#include <string>
#include "../include/board.h"
#include "../include/game_controller.h"

#define NONE NONE

int GameController::parse_to_square(string input) {
    if (input.size() != 2) return NONE;

    int file = input[0] - 'a';
    int rank = input[1] - '1';

    if (file < 0 || file > 7 || rank < 0 || rank > 7) return NONE;
    
    return rank * Board::SIDE + file;
}

std::string GameController::square_to_string(int input) {
    if (input < 0 || input >= 64) return "";

    int file = input % Board::SIDE;
    int rank = input / Board::SIDE;

    std::string move;
    move.push_back('a' + file);
    move.push_back('1' + rank);
    
    return move;
}

void GameController::start_game() {
    std::string colour_input;

    std::cout << "Would you like to play as white or black: ";
    std::cin >> colour_input;

    bool as_colour;

    while (true) {
        if (colour_input == "white") {
            as_colour = true;
            break;
        } else if (colour_input == "black") {
            as_colour = false;
            break;
        } else {
            std::cout << "Please enter which colour you would like to play as. [\"white\"] or [\"black\"]" << std::endl;
            std::cin >> colour_input;
        }
    }

    std::cout << "Game started! You are playing as " << ((as_colour) ? "white" : "black") << std::endl;

    game_loop(as_colour);
}

void GameController::game_loop(bool as_colour) {
    bool human_is_white = as_colour;
    bool white_to_move = true;

    while (true) {
        board.print_board(human_is_white);

        if (board.is_over(white_to_move)) {
            std::cout << "Game over!" << std::endl;
            break;
        }

        if (white_to_move == human_is_white) {
            std::string from;
            std::string to;
    
            std::cout << "Move a piece from: ";
            std::cin >> from;
    
            std::cout << "Move a piece to: ";
            std::cin >> to;
    
            if (!move_piece(from, to, white_to_move)) {
                std::cout << "Invalid move, try again!" << std::endl;
                continue;
            }
        } else {
            std::cout << "Engine is thinking" << std::endl;
            engine_move(white_to_move);
        }

        white_to_move = !white_to_move;
    }
}

bool GameController::move_piece(string from, string to, bool is_white) {
    int square_from = parse_to_square(from);
    int square_to = parse_to_square(to);

    if (square_from == NONE || square_to == NONE) return false;

    int to_rank = square_to / Board::SIDE;
    int final_rank = (is_white) ? Board::SIDE - 1 : 0;
    
    int piece_moved = board.find_piece_moved(square_from) % Board::PIECES;
    if (piece_moved == NONE) return false;   
    
    int promote_to = NONE;

    // Promotion logic
    if (to_rank == final_rank && piece_moved % Board::PIECES == Board::PAWN) {
        std::cout << "Enter which piece you would like to promote your pawn to";
        std::cout << "(N - Knight, B - Bishop, R - Rook, Q - Queen): ";
        
        char input;
        std::cin >> input;
        
        switch (input) {
            case 'N':
                promote_to = Board::KNIGHT;
                break;
            case 'B':
                promote_to = Board::BISHOP;
                break;
            case 'R':
                promote_to = Board::ROOK;
                break;
            case 'Q':
                promote_to = Board::QUEEN;
                break;
            default:
                break;
        }
    }

    if (promote_to != NONE) {
        return board.move(Move(square_from, square_to, piece_moved, promote_to), is_white);
    }

    return board.move(Move(square_from, square_to, piece_moved), is_white);
}   

void GameController::engine_move(bool as_colour) {
    Move choose = engine.best_move(board, as_colour);
    board.make_move_unchecked(choose, as_colour);

    std::cout << "Engine plays: " << square_to_string(choose.from) << " to " << square_to_string(choose.to) << std::endl;
}