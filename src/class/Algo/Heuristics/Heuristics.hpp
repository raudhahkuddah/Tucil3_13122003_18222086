#ifndef __HEURISTICS__
#define __HEURISTICS__

#include "../../Board/Board.hpp"

#include <string>

class Heuristics
{
private:
    std::string type;

    int DUMBASS(Board &board);
    int LAZY(Board &board);

public:
    Heuristics(std::string type) : type(type) {};

    int calculate(Board &board);
};

#endif