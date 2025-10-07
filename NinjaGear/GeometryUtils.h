#ifndef GEOMETRY_UTILS_H
#define GEOMETRY_UTILS_H

#include <glm/glm.hpp>
#include <cmath>
#include <algorithm>

namespace GeometryUtils {
    
    /* Code from https://www.redblobgames.com/grids/line-drawing/ */
    inline float lerp(float start, float end, float t) {
        return start * (1.0f - t) + t * end;
    }
    
    inline glm::vec2 lerpPoint(const glm::vec2& p0, const glm::vec2& p1, float t) {
        return glm::vec2(lerp(p0.x, p1.x, t), lerp(p0.y, p1.y, t));
    }
    
    inline glm::ivec2 roundPoint(const glm::vec2& p) {
        return glm::ivec2(std::round(p.x), std::round(p.y));
    }
    
    inline float diagonalDistance(const glm::vec2& p0, const glm::vec2& p1) {
        float dx = p1.x - p0.x;
        float dy = p1.y - p0.y;
        return std::max(std::abs(dx), std::abs(dy));
    }
}

#endif