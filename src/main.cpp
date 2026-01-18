#include <iostream>
#include "game_controller.h"


int main() {
    std::cout << "Chess Engine starting..." << std::endl;

    GameController controller;
    controller.start_game();

    std::cout << "Exiting the chess engine" << std::endl;
}