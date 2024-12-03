#ifndef GRIDSTATEREADER_H
#define GRIDSTATEREADER_H

#include <string>
#include <vector>

class GridState {
public:
    GridState(int size) : grid(size, std::vector<bool>(size, false)) {}

    std::vector<std::vector<bool>> grid;
};

class GridStateReader {
public:
    static std::vector<GridState> readGridStates(const std::string& filePath);
};

#endif // GRIDSTATEREADER_H
