#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include <iostream>
#include "board.h"
#include "engine.h"
#include "move.h"
#include <string>

class GameController {
private:
    Engine engine;
    Board board;

    // Converts a string input (e.g. "e2") to an index on our bitboards
    int parse_to_square(std::string input);

    // Converts an index on our bitboards to a string input
    std::string square_to_string(int input);

    // Moves a piece on the board
    bool move_piece(std::string from, std::string to, bool is_white);
    
    // Allows the engine to make a move
    void engine_move(bool as_colour);

    // Perpetuates the turn-by-turn loop of the game
    void game_loop(bool as_colour);
public:
    // Begins the game
    void start_game();
};

#endif