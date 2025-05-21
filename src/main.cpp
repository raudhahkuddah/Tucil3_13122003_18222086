#include "class/Board/Board.hpp"
#include "class/Algo/Heuristics/Heuristics.hpp"
#include "class/Algo/Solver/Solver.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>
#include <chrono>

int main(int argc, char *argv[])
{
    std::string filename;
    std::string algorithm;
    std::string heuristic;

    if (argc >= 3)
    {
        filename = argv[1];
        algorithm = argv[2];
        if (algorithm != "UCS")
            heuristic = argv[3];
    }
    else
    {
        std::cout << "[INPUT] ENTER PATH TO PROBLEM FOLDER: ";
        std::cin >> filename;
        std::cout << "[INPUT] ALGORITHM: (UCS / GBFS / A*/ IDA*)";
        std::cin >> algorithm;
        if (algorithm != "UCS")
        {
            std::cout << "[INPUT] SELECT HEURISTIC (DUMBASS / LAZY) : ";
            std::cin >> heuristic;
        }
    }

    // read testcase
    std::ifstream file(filename + "/problem.txt");
    if (!file)
    {
        std::cerr << "Cannot open file.\n";
        return 1;
    }

    int N, M, P;
    std::vector<std::string> INPUT;

    file >> N >> M;
    file >> P;
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // newline

    std::string line;
    while (std::getline(file, line))
        INPUT.push_back(line);
    Board board(INPUT, N, M);
    Heuristics H((algorithm == "UCS") ? "UCS" : heuristic);
    Solver solver(board, H);

    std::vector<Board> solutions;
    auto start = std::chrono::high_resolution_clock::now();
    if (algorithm == "UCS" || algorithm == "A*")
        solutions = solver.SolveComplete();
    else if (algorithm == "GBFS")
        solutions = solver.SolveGreedy();
    else
        solutions = solver.SolveLowMemory();
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::ofstream outfile(filename + "/solutions.txt");
    if (!outfile)
    {
        std::cerr << "Cannot create solutions.txt in folder.\n";
        return 1;
    }

    outfile << duration.count() << "\n";
    outfile << solver.VISITED_NODES << "\n";
    outfile << solutions.size() << "\n";
    outfile << solutions[0].N << " " << solutions[0].M << "\n"; // possibly rotated
    for (Board b : solutions)
        b.PrintBoard(outfile);
}