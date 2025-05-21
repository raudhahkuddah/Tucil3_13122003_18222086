#ifndef __PIECE__
#define __PIECE__

#include <utility>

class Piece
{

public:
    char id;                 // identifier
    int length;              // length of this piece
    bool isVertical;         // vertical / horizontal
    std::pair<int, int> pos; // top-left index

    Piece(char id, int length, bool isVertical, std::pair<int, int> pos) : id(id), length(length), isVertical(isVertical), pos(pos) {};
};

#endif
