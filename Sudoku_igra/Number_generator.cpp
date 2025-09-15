#include "pch.h"
#include "Number_generator.h"

//kod je uzet sa stranice https://www.geeksforgeeks.org/dsa/program-sudoku-generator/


// Returns false if given 3x3 block contains num
// Ensure the number is not used in the box
bool unUsedInBox(std::vector<std::vector<int>>& grid, int rowStart, int colStart, int num) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (grid[rowStart + i][colStart + j] == num) {
                return false;
            }
        }
    }
    return true;
}

// Fill a 3x3 matrix
// Assign valid random numbers to the 3x3 subgrid
void fillBox(std::vector<std::vector<int>>& grid, int row, int col) {
    int num;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            do {

                // Generate a random number between 1 and 9
                num = (rand() % 9) + 1;
            } while (!unUsedInBox(grid, row, col, num));
            grid[row + i][col + j] = num;
        }
    }
}

// Check if it's safe to put num in row i
// Ensure num is not already used in the row
bool unUsedInRow(std::vector<std::vector<int>>& grid, int i, int num) {
    for (int j = 0; j < 9; j++) {
        if (grid[i][j] == num) {
            return false;
        }
    }
    return true;
}

// Check if it's safe to put num in column j
// Ensure num is not already used in the column
bool unUsedInCol(std::vector<std::vector<int>>& grid, int j, int num) {
    for (int i = 0; i < 9; i++) {
        if (grid[i][j] == num) {
            return false;
        }
    }
    return true;
}

// Check if it's safe to put num in the cell (i, j)
// Ensure num is not used in row, column, or box
bool checkIfSafe(std::vector<std::vector<int>>& grid, int i, int j, int num) {
    return (unUsedInRow(grid, i, num) && unUsedInCol(grid, j, num) &&
        unUsedInBox(grid, i - i % 3, j - j % 3, num));
}

// Fill the diagonal 3x3 matrices
// The diagonal blocks are filled to simplify the process
void fillDiagonal(std::vector<std::vector<int>>& grid) {
    for (int i = 0; i < 9; i = i + 3) {

        // Fill each 3x3 subgrid diagonally
        fillBox(grid, i, i);
    }
}

// Fill remaining blocks in the grid
// Recursively fill the remaining cells with valid numbers
bool fillRemaining(std::vector<std::vector<int>>& grid, int i, int j) {

    // If we've reached the end of the grid
    if (i == 9) {
        return true;
    }

    // Move to next row when current row is finished
    if (j == 9) {
        return fillRemaining(grid, i + 1, 0);
    }

    // Skip if cell is already filled
    if (grid[i][j] != 0) {
        return fillRemaining(grid, i, j + 1);
    }

    // Try numbers 1-9 in current cell
    for (int num = 1; num <= 9; num++) {
        if (checkIfSafe(grid, i, j, num)) {
            grid[i][j] = num;
            if (fillRemaining(grid, i, j + 1)) {
                return true;
            }
            grid[i][j] = 0;
        }
    }

    return false;
}

// Remove K digits randomly from the grid
// This will create a Sudoku puzzle by removing digits
void removeKDigits(std::vector<std::vector<int>>& grid, int k) {
    while (k > 0) {

        // Pick a random cell
        int cellId = rand() % 81;

        // Get the row index
        int i = cellId / 9;

        // Get the column index
        int j = cellId % 9;

        // Remove the digit if the cell is not already empty
        if (grid[i][j] != 0) {

            // Empty the cell
            grid[i][j] = 0;

            // Decrease the count of digits to remove
            k--;
        }
    }
}

// Generate a Sudoku grid with K empty cells
std::vector<std::vector<int>> sudokuGenerator(int k) {

    // Initialize an empty 9x9 grid
    std::vector<std::vector<int>> grid(9, std::vector<int>(9, 0));

    // Fill the diagonal 3x3 matrices
    fillDiagonal(grid);

    // Fill the remaining blocks in the grid
    fillRemaining(grid, 0, 0);

    // Remove K digits randomly to create the puzzle
    removeKDigits(grid, k);

    return grid;
}