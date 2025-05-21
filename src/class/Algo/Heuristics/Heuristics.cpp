#include "Heuristics.hpp"

int Heuristics::DUMBASS(Board &board)
{
    if (board.IsSolved())
        return 0;

    const auto &pieces = board.Pieces;
    const Piece &primary = pieces.at('P');

    int row = primary.pos.first;
    int colStart = primary.pos.second;
    int colEnd = colStart + primary.length - 1;

    int blockingCells = 0;
    // Check for vertical pieces that block the path to the exit
    for (const auto &[id, piece] : pieces)
    {
        if (id == 'P')
            continue;
        if (piece.isVertical)
        {
            int col = piece.pos.second;
            int rowStart = piece.pos.first;
            int rowEnd = rowStart + piece.length - 1;

            if (col > colEnd && col <= board.M - 1 && rowStart <= row && rowEnd >= row)
                blockingCells++;
        }
    }

    return blockingCells;
}

int Heuristics::LAZY(Board &board)
{
    Piece primary = board.Pieces.at('P');
    return board.M - (primary.pos.second + primary.length) - 1;
}

int Heuristics::calculate(Board &board)
{
    if (this->type == "DUMBASS")
        return DUMBASS(board);
    if (this->type == "LAZY")
        return LAZY(board);
    if (this->type == "UCS")
        return 0;

    return DUMBASS(board);
}
