#include "GridStateReader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

std::vector<GridState> GridStateReader::readGridStates(const std::string& filePath) {
    std::vector<GridState> grids;
    std::ifstream inputFile(filePath);
    std::string currentLine;

    int gridSize;
    if (std::getline(inputFile, currentLine)) {
        std::istringstream lineStream(currentLine);
        if (!(lineStream >> gridSize)) {
            std::cerr << "Error: Grid size format is invalid." << std::endl;
            return grids; // Return empty if format is invalid
        } else {
            std::cout << "Grid Size: " << gridSize << std::endl;
        }
    } else {
        std::cerr << "Error: Unable to read the file. It may be empty or inaccessible." << std::endl;
        return grids;
    }

    while (std::getline(inputFile, currentLine)) {
        if (currentLine.size() != static_cast<size_t>(gridSize * gridSize)) {
            std::cerr << "Warning: Skipping invalid line with mismatched length." << std::endl;
            continue; // Skip invalid lines
        }

        GridState grid(gridSize);
        for (int row = 0; row < gridSize; ++row) {
            for (int col = 0; col < gridSize; ++col) {
                int charIndex = row * gridSize + col;
                grid.grid[row][col] = currentLine[charIndex] - '0'; // Convert character to boolean
            }
        }

        grids.push_back(grid);
    }

    if (grids.empty()) {
        std::cerr << "Error: No valid grids found in the file." << std::endl;
    }

    return grids;
}
