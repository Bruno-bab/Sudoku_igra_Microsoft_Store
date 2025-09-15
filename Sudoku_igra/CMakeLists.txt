cmake_minimum_required(VERSION 3.10)
project(Sudoku_igra)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(sudoku
    main.cpp
    Sudoku_igra.cpp
    Number_generator.cpp
    Sudoku_igra.h
    Number_generator.h
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set_target_properties(sudoku PROPERTIES WIN32_EXECUTABLE TRUE)
