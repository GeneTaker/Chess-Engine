# Chess-Engine
A project that aims to create a chess engine using relevant chess evaluation techniques to practice skills and learn about algorithmically creating a chess engine

## Tech Stack:
- This project was created entirely with C++, and tested with Catch2 and CMake tools 

## Description
- A chess engine, can be played against, can be used as an aid
- Created as a learning project
- Uses bitboards to optimise performance, and relevant search techniques (alpha beta pruning, quiescence search,
 etc.) to search for the best moves

## To Use It:
- Run the following in your terminal, compile it with g++ and execute it to run the engine

> git clone https://github.com/GeneTaker/Chess-Engine.git

## Design Notes:
- For notes on how the engine was designed, please refer to __blog.md__

## Future Considerations:
- This project could be extended upon by adding iterative deepening search, transposition tables or adding further tactical moves that could be made into the evaluator (e.g. forks, skewers, pins, etc)