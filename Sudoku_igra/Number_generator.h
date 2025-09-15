#pragma once

#include <vector>
#include <time.h>

bool unUsedInBox(std::vector<std::vector<int>>& grid, int rowStart, int colStart, int num);

void fillBox(std::vector<std::vector<int>>& grid, int row, int col);

bool unUsedInRow(std::vector<std::vector<int>>& grid, int i, int num);

bool unUsedInCol(std::vector<std::vector<int>>& grid, int j, int num);

bool checkIfSafe(std::vector<std::vector<int>>& grid, int i, int j, int num);

void fillDiagonal(std::vector<std::vector<int>>& grid);

bool fillRemaining(std::vector<std::vector<int>>& grid, int i, int j);

void removeKDigits(std::vector<std::vector<int>>& grid, int k);

std::vector<std::vector<int>> sudokuGenerator(int k);
