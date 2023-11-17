// StateReader.h
#ifndef STATEREADER_H
#define STATEREADER_H

#include <string>
#include <vector>

const int GRID_SIZE = 4; // Assuming a 5x5 grid for simplicity

class State {
public:
    bool grid[GRID_SIZE][GRID_SIZE];
};

class StateReader {
public:
    static std::vector<State> readGameOfLifeStates(const std::string& filename);
};

#endif // STATEREADER_H
