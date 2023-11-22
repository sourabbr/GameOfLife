#include "StateReader.h"
#include <fstream>
#include <iostream> // Include for debugging output
#include <sstream>
#include <vector>

std::vector<State> StateReader::readGameOfLifeStates(const std::string& filename) {
    std::vector<State> states;
    std::ifstream file(filename);
    std::string line;

    // Read the first line for grid size
    int gridSize;
    if (std::getline(file, line)) {
        std::istringstream iss(line);
        if (!(iss >> gridSize)) {
            std::cerr << "Error: Grid size is invalid in the file." << std::endl; // Debugging output
            return states; // Return an empty vector if grid size is invalid
        }
        else {
            std::cout << "Grid Size: " << gridSize << std::endl; 
        }
    }
    else {
        std::cerr << "Error: File is empty." << std::endl;
        return states; 
    }

    while (std::getline(file, line)) {
        if (line.size() != gridSize * gridSize) {
            continue; // Skip invalid lines
        }

        State state(gridSize); 
        for (int i = 0; i < gridSize; ++i) {
            for (int j = 0; j < gridSize; ++j) {
                int index = i * gridSize + j;
                state.grid[i][j] = line[index] - '0'; // Convert char to int
            }
        }

        states.push_back(state);
    }

    return states;
}

