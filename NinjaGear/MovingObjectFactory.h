#pragma once
#include "MovingObject.h"
#include <vector>

class MovingObjectFactory
{
public:
    static MovingObject* create(
        const MovingObjectConfig& config,
        Texture* texture,
        ShaderProgram* program,
        const std::vector<TileMap*>& maps,
        const glm::ivec2& screenPos);

private:
    static MovingObject* createMovingStatue(
        const MovingObjectConfig& config,
        Texture* texture,
        ShaderProgram* program);

    static MovingObject* createSpikeTrap(
        const MovingObjectConfig& config,
        Texture* texture,
        ShaderProgram* program);
};

