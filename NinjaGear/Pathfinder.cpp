#include "Pathfinder.h"
#include <queue>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <set>

vector<glm::ivec2> Pathfinder::findPath(
    const glm::ivec2& start,
    const glm::ivec2& goal,
    const TileMap* tileMap)
{
    if ( !isWalkable(goal, tileMap)) return {};

    using Node = pair<float, glm::ivec2>;
    priority_queue<Node, vector<Node>, NodeCompare> openSet;
    map<glm::ivec2, glm::ivec2, ivec2Compare> cameFrom;
    map<glm::ivec2, float, ivec2Compare> gScore;
    set<glm::ivec2, ivec2Compare> closedSet;

    gScore[start] = 0.0f;
    openSet.push({ heuristic(start, goal), start });

    while (!openSet.empty()) {
        glm::ivec2 current = openSet.top().second;
        openSet.pop();

        if (closedSet.count(current)) continue;
        closedSet.insert(current);

        if (current == goal) return reconstructPath(cameFrom, current);

        for (const glm::ivec2& neighbor : getNeighbors(current)) {
            if (!isWalkable(neighbor, tileMap) || closedSet.count(neighbor)) continue;

            float g = gScore[current] + 1.0f;

            if (gScore.find(neighbor) == gScore.end() || g < gScore[neighbor]) {
                cameFrom[neighbor] = current;
                gScore[neighbor] = g;
                openSet.push({ g + heuristic(neighbor, goal), neighbor });
            }
        }
    }

    return {};
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

bool Pathfinder::isWalkable(const glm::ivec2& pos, const TileMap* tileMap) const {
    if (!tileMap) return false;

    if (pos.x < 0 || pos.y < 0 || pos.x >= tileMap->mapSize.x || pos.y >= tileMap->mapSize.y) {
        return false;
    }

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
