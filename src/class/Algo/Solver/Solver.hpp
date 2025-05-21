#ifndef __Solver__
#define __Solver__

#include "../../Board/Board.hpp"
#include "../Heuristics/Heuristics.hpp"

#include <memory>
#include <unordered_set>

class Solver
{
private:
    Board INITIAL;
    Heuristics H;

    // helper structure
    struct Node
    {
        Board board;
        int g;                        // cost from start
        int h;                        // heuristic
        int f;                        // total = g + h
        std::shared_ptr<Node> parent; // parent node

        Node(Board board, int g, int h, std::shared_ptr<Node> parent = nullptr)
            : board(board), g(g), h(h), f(g + h), parent(parent) {}

        // For priority queue comparison
        bool operator>(const Node &other) const
        {
            return f > other.f;
        }
    };

public:
    Solver(Board board, Heuristics H) : INITIAL(board), H(H) {};

    std::vector<Board> SolveComplete();  // A* and UCS
    std::vector<Board> SolveGreedy();    // GBFS
    std::vector<Board> SolveLowMemory(); // IDA*
};

#endif