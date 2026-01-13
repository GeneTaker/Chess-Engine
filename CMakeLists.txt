cmake_minimum_required(VERSION 3.16)

project(chess_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(chess_engine src/main.cpp src/board.cpp src/move_generator.cpp)

# ==========================
# Tests (Catch2)
# ==========================

add_executable(chess_tests 
    tests/test_main.cpp
    tests/test_board.cpp
    tests/test_pawn.cpp
    tests/test_knight.cpp
    tests/test_rook.cpp
    tests/test_bishop.cpp
    tests/test_queen.cpp
    tests/test_king.cpp
    tests/catch_amalgamated.cpp  # Add this!
    src/board.cpp
    src/move_generator.cpp
)

# Add include directories
target_include_directories(chess_tests PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/tests
    ${CMAKE_SOURCE_DIR}/include
)

# Force console mode for Windows
if(WIN32)
    target_link_options(chess_tests PRIVATE -mconsole)
endif()

enable_testing()
add_test(NAME chess_tests COMMAND chess_tests)