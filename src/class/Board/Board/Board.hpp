#ifndef __BOARD__
#define __BOARD__

#include "Piece.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>

class Board
{
public:
    int N, M;                     // Row, Column
    std::map<char, Piece> Pieces; // Board representation

    Board(std::vector<std::string> INPUT, int N, int M); // constructor

    std::vector<std::shared_ptr<Board>> GenerateSuccessors() const;
    std::string Serialize() const;
    bool IsSolved();   // check if board is solved
    void PrintBoard(); // print board (debugging)
};

#endif