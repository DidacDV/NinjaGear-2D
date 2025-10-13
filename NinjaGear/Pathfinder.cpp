#include "Pathfinder.h"
#include <queue>
#include <cmath>
#include <algorithm>
#include <iostream>

vector<glm::ivec2> Pathfinder::findPath(
    const glm::ivec2& start,
    const glm::ivec2& goal,
    TileMap* tileMap)
{
    if (!tileMap || !isWalkable(goal, tileMap)) {
        return vector<glm::ivec2>();
    }

    // A* implementation
    using Node = pair<float, glm::ivec2>; // (fScore, position)
    priority_queue<Node, vector<Node>, NodeCompare> openSet;

    map<glm::ivec2, glm::ivec2, ivec2Compare> cameFrom;
    map<glm::ivec2, float, ivec2Compare> gScore;
    map<glm::ivec2, float, ivec2Compare> fScore;

    gScore[start] = 0;
    fScore[start] = heuristic(start, goal);
    openSet.push({ fScore[start], start });

    while (!openSet.empty()) {
        glm::ivec2 current = openSet.top().second;
        openSet.pop();

        if (current == goal) {
            return reconstructPath(cameFrom, current);
        }

        for (const glm::ivec2& neighbor : getNeighbors(current)) {
            if (!isWalkable(neighbor, tileMap)) continue;

            float tentativeGScore = gScore[current] + 1.0f;

            if (gScore.find(neighbor) == gScore.end() ||
                tentativeGScore < gScore[neighbor]) {

                cameFrom[neighbor] = current;
                gScore[neighbor] = tentativeGScore;
                fScore[neighbor] = tentativeGScore + heuristic(neighbor, goal);
                openSet.push({ fScore[neighbor], neighbor });
            }
        }
    }

    return vector<glm::ivec2>(); // No path found
}

float Pathfinder::heuristic(const glm::ivec2& a, const glm::ivec2& b) const {
    // Manhattan distance for grid-based movement
    return static_cast<float>(abs(a.x - b.x) + abs(a.y - b.y));
}

vector<glm::ivec2> Pathfinder::reconstructPath(
    const map<glm::ivec2, glm::ivec2, ivec2Compare>& cameFrom,
    glm::ivec2 current) const
{
    vector<glm::ivec2> path = { current };
    while (cameFrom.find(current) != cameFrom.end()) {
        current = cameFrom.at(current);
        path.push_back(current);
    }
    reverse(path.begin(), path.end());
    return path;
}

bool Pathfinder::isWalkable(const glm::ivec2& pos, TileMap* tileMap) const {
    return !tileMap->isTileBlocked(pos.x, pos.y);
}

vector<glm::ivec2> Pathfinder::getNeighbors(const glm::ivec2& pos) const {
    return {
        {pos.x + 1, pos.y},     // Right
        {pos.x - 1, pos.y},     // Left
        {pos.x, pos.y + 1},     // Down
        {pos.x, pos.y - 1}      // Up
    };
}

glm::ivec2 Pathfinder::worldToTile(const glm::vec2& worldPos, int tileSize) const {
    return glm::ivec2(worldPos.x / tileSize, worldPos.y / tileSize);
}
