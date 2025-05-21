#include "Solver.hpp"

#include <queue>
#include <vector>
#include <memory>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <functional>

std::vector<Board> Solver::SolveComplete()
{
    auto cmp = [](const std::shared_ptr<Node> &a, const std::shared_ptr<Node> &b)
    {
        return a->f > b->f; // Min-heap
    };

    std::priority_queue<
        std::shared_ptr<Node>,
        std::vector<std::shared_ptr<Node>>,
        decltype(cmp)>
        openSet(cmp);

    std::unordered_set<std::string> visited;

    auto startNode = std::make_shared<Node>(INITIAL, 0, H.calculate(INITIAL));
    openSet.push(startNode);
    while (!openSet.empty())
    {
        auto current = openSet.top();
        openSet.pop();

        std::string stateKey = current->board.Serialize();
        if (visited.count(stateKey))
            continue;
        visited.insert(stateKey);
        this->VISITED_NODES++;

        if (current->board.IsSolved())
        {
            std::vector<Board> path;
            for (auto node = current; node != nullptr; node = node->parent)
            {
                path.push_back(node->board);
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        auto successors = current->board.GenerateSuccessors();
        for (auto &succ : successors)
        {
            std::string succKey = succ->Serialize();
            if (visited.count(succKey))
                continue;

            int g = current->g + 1;
            int h = H.calculate(*succ);
            auto nextNode = std::make_shared<Node>(*succ, g, h, current);
            openSet.push(nextNode);
        }
    }

    return {}; // No solution found
}

std::vector<Board> Solver::SolveGreedy()
{
    auto cmp = [](const std::shared_ptr<Node> &a, const std::shared_ptr<Node> &b)
    {
        return a->h > b->h; // Min-heap based only on heuristic
    };

    std::priority_queue<
        std::shared_ptr<Node>,
        std::vector<std::shared_ptr<Node>>,
        decltype(cmp)>
        openSet(cmp);

    std::unordered_set<std::string> visited;

    int h = H.calculate(INITIAL);
    auto startNode = std::make_shared<Node>(INITIAL, 0, h); // g=0, h=heuristic
    openSet.push(startNode);

    while (!openSet.empty())
    {
        auto current = openSet.top();
        openSet.pop();

        std::string stateKey = current->board.Serialize();
        if (visited.count(stateKey))
            continue;
        visited.insert(stateKey);
        this->VISITED_NODES++;

        if (current->board.IsSolved())
        {
            std::vector<Board> path;
            for (auto node = current; node != nullptr; node = node->parent)
            {
                path.push_back(node->board);
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        auto successors = current->board.GenerateSuccessors();
        std::sort(successors.begin(), successors.end(), [](const auto &a, const auto &b)
                  {
                      return a->Serialize() < b->Serialize(); // Ensure consistent expansion
                  });

        for (auto &succ : successors)
        {
            std::string succKey = succ->Serialize();
            if (visited.count(succKey))
                continue;

            int h = H.calculate(*succ);
            auto nextNode = std::make_shared<Node>(*succ, 0, h, current); // g is 0
            openSet.push(nextNode);
        }
    }

    return {}; // No solution found
}

std::vector<Board> Solver::SolveLowMemory()
{
    struct SearchResult
    {
        std::shared_ptr<Node> result;
        int nextThreshold;
    };

    std::unordered_map<std::string, int> globalF; // Global visited map: stateKey -> min f-cost

    std::function<SearchResult(std::shared_ptr<Node>, int, std::unordered_set<std::string> &)> dfs;
    dfs = [&](std::shared_ptr<Node> current, int threshold, std::unordered_set<std::string> &pathVisited) -> SearchResult
    {
        std::string stateKey = current->board.Serialize();
        int f = current->g + current->h;

        if (f > threshold)
            return {nullptr, f};

        // Global pruning: skip if already visited with equal or better f
        auto it = globalF.find(stateKey);
        if (it != globalF.end() && it->second <= f)
            return {nullptr, INT_MAX};
        globalF[stateKey] = f;

        // Cycle avoidance on current path
        if (pathVisited.count(stateKey))
            return {nullptr, INT_MAX};

        if (current->board.IsSolved())
            return {current, threshold};

        pathVisited.insert(stateKey);
        this->VISITED_NODES++;

        int minThreshold = INT_MAX;
        auto successors = current->board.GenerateSuccessors();

        // Cache heuristic for each successor before sorting
        std::vector<std::pair<std::shared_ptr<Board>, int>> successorsWithH;
        for (auto &succ : successors)
        {
            int h = H.calculate(*succ);
            successorsWithH.emplace_back(succ, h);
        }

        // Sort by f = g + h ascending
        std::sort(successorsWithH.begin(), successorsWithH.end(),
                  [&](const auto &a, const auto &b)
                  {
                      int fa = current->g + 1 + a.second;
                      int fb = current->g + 1 + b.second;
                      return fa < fb;
                  });

        for (auto &[succ, h] : successorsWithH)
        {
            std::string succKey = succ->Serialize();
            if (pathVisited.count(succKey))
                continue;

            int g = current->g + 1;
            auto nextNode = std::make_shared<Node>(*succ, g, h, current);

            auto result = dfs(nextNode, threshold, pathVisited);
            if (result.result != nullptr)
            {
                pathVisited.erase(stateKey); // backtrack
                return result;
            }

            minThreshold = std::min(minThreshold, result.nextThreshold);
        }

        pathVisited.erase(stateKey); // backtrack
        return {nullptr, minThreshold};
    };

    int h0 = H.calculate(INITIAL);
    auto root = std::make_shared<Node>(INITIAL, 0, h0);

    int threshold = h0;

    while (true)
    {
        globalF.clear(); // Clear global visited at each iteration
        std::unordered_set<std::string> pathVisited;

        auto result = dfs(root, threshold, pathVisited);

        if (result.result != nullptr)
        {
            // Reconstruct path from result node
            std::vector<Board> path;
            for (auto node = result.result; node != nullptr; node = node->parent)
                path.push_back(node->board);
            std::reverse(path.begin(), path.end());
            return path;
        }

        if (result.nextThreshold == INT_MAX)
        {
            break;
        }

        threshold = result.nextThreshold;
    }

    return {}; // No solution found
}
