#include "Board.hpp"

#include <functional>
#include <iostream>

Board::Board(std::vector<std::string> INPUT, int N, int M)
{
    this->N = N;
    this->M = M;
    std::vector<std::vector<char>> DATA(N, std::vector<char>(M));

    // Generate DATA[][]
    // Recursive rotate function: rotates DATA 'times' times 90 deg clockwise
    std::function<void(int)> rotateDATA = [&](int times)
    {
        if (times <= 0)
            return;

        int n = DATA.size();
        int m = DATA[0].size();

        std::vector<std::vector<char>> rotated(m, std::vector<char>(n));
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < m; j++)
            {
                rotated[j][n - i - 1] = DATA[i][j];
            }
        }
        DATA = rotated;
        std::swap(this->N, this->M);

        rotateDATA(times - 1);
    };

    // Case 1 : exit is at the top / bottom
    if (INPUT.size() > static_cast<std::size_t>(N))
    {
        // Case 1.1 : exit is at the top
        if (INPUT[0].find('K') != std::string::npos)
        {
            for (int i = 1; i <= N; i++)
            {
                for (int j = 0; j < M; j++)
                {
                    DATA[i - 1][j] = INPUT[i][j];
                }
            }
            // rotate 90 degree clockwise
            rotateDATA(1);
        }
        // Case 2.2 : exit is at the bottom
        else
        {
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < M; j++)
                {
                    DATA[i][j] = INPUT[i][j];
                }
            }
            // rotate 270 degree clockwise
            rotateDATA(3);
        }
    }
    // Case 2 : exit is at the left / right
    else
    {
        // Case 2.1 : exit is at the left
        if (INPUT[0].size() > static_cast<std::size_t>(M))
        {
            for (int i = 0; i < N; i++)
            {
                for (int j = 1; j <= M; j++)
                {
                    DATA[i][j - 1] = INPUT[i][j];
                }
            }
            rotateDATA(2);
        }
        // Case 2.2 (default) : exit is at the right
        else
        {
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < M; j++)
                {
                    DATA[i][j] = INPUT[i][j];
                }
            }
        }
    }

    // Turn DATA[][] into map<char, Piece>
    std::vector<std::vector<bool>> checked(N, std::vector<bool>(M));
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            if (DATA[i][j] == '.' || checked[i][j])
                continue;
            checked[i][j] = true;

            char id = DATA[i][j];
            int length = 1;
            std::pair<int, int> pos = {i, j};
            bool isVertical = (i + 1 < N && DATA[i + 1][j] == id);

            if (isVertical)
            {
                while (i + length < N && DATA[i + length][j] == id)
                    checked[i + length++][j] = true;
            }
            else
            {
                while (j + length < M && DATA[i][j + length] == id)
                    checked[i][j + length++] = true;
            }

            this->Pieces.insert({id, Piece(id, length, isVertical, pos)});
        }
    }
}

std::vector<std::shared_ptr<Board>> Board::GenerateSuccessors() const
{
    // Reconstruct 2D board
    std::vector<std::vector<char>> board(this->N, std::vector<char>(this->M, '.'));
    for (const auto &[id, piece] : this->Pieces)
    {
        for (int i = 0; i < piece.length; i++)
        {
            if (piece.isVertical)
                board[piece.pos.first + i][piece.pos.second] = id;
            else
                board[piece.pos.first][piece.pos.second + i] = id;
        }
    }

    std::vector<std::shared_ptr<Board>> successors;

    for (const auto &[id, piece] : this->Pieces)
    {
        if (piece.isVertical)
        {
            for (int i = piece.pos.first - 1; i >= 0; i--)
            {
                if (board[i][piece.pos.second] != '.')
                    break;

                auto newBoard = std::make_shared<Board>(*this);
                newBoard->Pieces.at(id).pos.first = i;
                successors.push_back(newBoard);
            }
            for (int i = piece.pos.first + piece.length; i < N; i++)
            {
                if (board[i][piece.pos.second] != '.')
                    break;

                auto newBoard = std::make_shared<Board>(*this);
                newBoard->Pieces.at(id).pos.first = i - piece.length + 1;
                successors.push_back(newBoard);
            }
        }
        else
        {
            for (int j = piece.pos.second - 1; j >= 0; j--)
            {
                if (board[piece.pos.first][j] != '.')
                    break;

                auto newBoard = std::make_shared<Board>(*this);
                newBoard->Pieces.at(id).pos.second = j;
                successors.push_back(newBoard);
            }
            for (int j = piece.pos.second + piece.length; j < M; j++)
            {
                if (board[piece.pos.first][j] != '.')
                    break;

                auto newBoard = std::make_shared<Board>(*this);
                newBoard->Pieces.at(id).pos.second = j - piece.length + 1;
                successors.push_back(newBoard);
            }
        }
    }

    return successors;
}

std::string Board::Serialize() const
{
    std::string key;
    for (const auto &[id, piece] : this->Pieces)
    {
        const Piece &p = piece;
        key += p.id;
        key += std::to_string(p.pos.first) + "," + std::to_string(p.pos.second) + ",";
        key += (p.isVertical ? "V" : "H") + std::to_string(p.length) + ";";
    }
    return key;
}

bool Board::IsSolved()
{
    Piece primary = this->Pieces.at('P');
    return primary.pos.second + primary.length == this->M;
}

void Board::PrintBoard(std::ostream &out) const
{
    std::vector<std::vector<char>> board(this->N, std::vector<char>(this->M, '.'));

    for (const auto &[id, piece] : this->Pieces)
    {
        for (int i = 0; i < piece.length; i++)
        {
            if (piece.isVertical)
                board[piece.pos.first + i][piece.pos.second] = id;
            else
                board[piece.pos.first][piece.pos.second + i] = id;
        }
    }

    for (int i = 0; i < this->N; i++)
    {
        for (int j = 0; j < this->M; j++)
            out << board[i][j];
        out << '\n';
    }
}
