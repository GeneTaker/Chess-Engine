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

    int parse_to_square(std::string input);
    std::string square_to_string(int input);
    bool move_piece(std::string from, std::string to, bool is_white);
    void engine_move(bool as_colour);
    void game_loop(bool as_colour);
public:
    void start_game();
};

#endif