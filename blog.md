### Chess Engine

## Requirements:
- Our chess engine should display an ascii board and be used via a cli interface
- Moves can be inputted using valid chess notation
- Moves can be undone
- All legal chess moves should be supported by the engine
- Player plays against the chess engine
- Moves are evaluated using relevant chess engine heuristics and analysis techniques

## Architectural Decisions:
- Of course, this project will be designed as a monolith, as I will be the only person working on it

## Non-functional Characteristics:
- Ideally, our chess engine will maximise:
    - Performance: We want board states to be evaluated quickly and efficiently
    - Reliability: The engine should not crash or produce illegal moves during gameplay
    - Maintainability: Code should be modular and extendable

## Design:

# Classes:
- Main: Handles cli interface 
- GameController: A controller class that provides low level access to our chess engine
- Engine: Repesents the engine's access to the board
- Board: A class that represents the current state of the board, which contains all the pieces, uses bitboards to represent the current state and magic bitboards to manage sliding pieces
- Evaluator: Encapsulates logic for evaluating the value of a board
- MoveGenerator: Abstracts Move creation logic 

# Structs:
- Move

## Optional Structures:
If there is sufficient time, we will implement Zobrist hashes and transposition tables to reduce computation time

# Classes:
- TranspositionTable: Manages the mapping from a hash representing a board state to board metadata

# Struct
- ZobristValue: A collection of data that bundles board metadata for TranspositionTable

## Notes:
Although it would be good to encapsulate data for each unique piece, the function calls are inefficient and it would be best for board states to be represented with bitboards