#ifndef STATEREADER_H
#define STATEREADER_H

#include <string>
#include <vector>

class State {
public:
    State(int size) : grid(size, std::vector<bool>(size, false)) {}

    std::vector<std::vector<bool>> grid;
};

class StateReader {
public:
    static std::vector<State> readGameOfLifeStates(const std::string& filename);
};

#endif // STATEREADER_H
