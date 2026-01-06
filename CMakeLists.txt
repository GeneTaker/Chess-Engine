cmake_minimum_required(VERSION 3.16)

project(chess_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(chess_engine src/main.cpp)

# FORCE console subsystem at the CMake level
set_property(TARGET chess_engine PROPERTY WIN32_EXECUTABLE OFF)