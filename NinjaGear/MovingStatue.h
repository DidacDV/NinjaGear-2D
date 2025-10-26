#pragma once
#include "MovingObject.h"
class MovingStatue :
    public MovingObject
{
public:
    static MovingStatue* createMovingStatue(const glm::vec2& quadSize,
        const glm::vec2& sizeInSpritesheet,
        Texture* spritesheet,
        ShaderProgram* program,
        const glm::vec2& startPos,
        const glm::vec2& endPos,
        float speed = 200.0f);

    MovingStatue(const glm::vec2& quadSize,
        const glm::vec2& sizeInSpritesheet,
        Texture* spritesheet,
        ShaderProgram* program);
    void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram,
        const glm::vec2& spriteSize, const glm::vec2& texCoordSize, Texture* texture) override;
    void setMovementPath(const glm::vec2& startPos, const glm::vec2& endPos, float speed) override;
    void updateMovementPattern(int deltaTime) override;

private:
    glm::vec2 startPosition;
    glm::vec2 endPosition;
    glm::vec2 targetPosition;
    float movementSpeed;
    bool movingToEnd;

    bool isPaused;
    float pauseTimer;
    float pauseDuration;
};

