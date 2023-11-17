#include "StateReader.h"
#include <fstream>
#include <sstream>
#include <vector>

std::vector<State> StateReader::readGameOfLifeStates(const std::string& filename) {
    std::vector<State> states;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        if (line.size() != GRID_SIZE * GRID_SIZE) {
            continue; // Skip invalid lines
        }

        State state;
        for (int i = 0; i < GRID_SIZE; ++i) {
            for (int j = 0; j < GRID_SIZE; ++j) {
                int index = i * GRID_SIZE + j;
                state.grid[i][j] = line[index] - '0'; // Convert char to int
            }
        }

        states.push_back(state);
    }

    return states;
}
