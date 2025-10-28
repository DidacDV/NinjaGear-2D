#pragma once
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include "TileMap.h"



class Pathfinder
{
public:
	// Singleton instance access
    static Pathfinder& instance() {
        static Pathfinder instance;
        return instance;
    }

    vector<glm::ivec2> findPath(
        const glm::ivec2& start,
        const glm::ivec2& goal,
        const vector<TileMap*>& tileMaps
    );

    glm::ivec2 worldToTile(const glm::vec2& worldPos, int tileSize) const;
private:
    struct ivec2Compare {
        bool operator()(const glm::ivec2& a, const glm::ivec2& b) const {
            if (a.x != b.x) return a.x < b.x;
            return a.y < b.y;
        }
    };

    struct NodeCompare {
        bool operator()(const pair<float, glm::ivec2>& a, const pair<float, glm::ivec2>& b) const {
            if (a.first != b.first) return a.first > b.first; 
            if (a.second.x != b.second.x) return a.second.x > b.second.x;
            return a.second.y > b.second.y;
        }
    };

    Pathfinder() = default;
    Pathfinder(const Pathfinder&) = delete;
    Pathfinder& operator=(const Pathfinder&) = delete;

    // A* helper methods
    float heuristic(const glm::ivec2& a, const glm::ivec2& b) const;
    vector<glm::ivec2> reconstructPath(
        const map<glm::ivec2, glm::ivec2, ivec2Compare>& cameFrom,
        glm::ivec2 current
    ) const;
    bool isWalkable(const glm::ivec2& pos, const vector<TileMap*>& tileMaps) const;
    vector<glm::ivec2> getNeighbors(const glm::ivec2& pos) const;
};

