# Chess Engine

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

## Classes:
- Main: Starts our program

- GameController: A controller class that provides low level access to our chess engine, handles the CLI interface for our engine 

- Engine: Repesents the chess engine's understanding of the board
    - Uses recursive search throughout all possible moves to ascertain the best possible moves
    - Contains an Evaluator object, and uses it to evaluate possible board states

- Board: A class that represents the current state of the board, which contains all the pieces, uses bitboards to represent the current state and magic bitboards to manage sliding pieces, contains the logic for checking the legality of all moves

- Evaluator: Encapsulates logic for evaluating the value of a board

- MoveGenerator: Generates all possible pseudo-legal moves 

### Structs:
- Move: Contains information about a move (from, to, type: what type of piece it is, promotion: which piece to promote to)
- PastMove: Contains information about a move, used to evaluate previous moves (from, to, captured_square, captured_piece, piece_moved, as well as other information that must be restored when we unmake a move)

## Optional Structures:
If there is sufficient time, we will implement Zobrist hashes and transposition tables to reduce computation time

### Classes:
- TranspositionTable: Manages the mapping from a hash representing a board state to board metadata

### Structs
- ZobristValue: A collection of data that bundles board metadata for TranspositionTable

### Notes:
Although it would be good to encapsulate data for each unique piece, the function calls are inefficient and it would be best for board states to be represented with bitboards