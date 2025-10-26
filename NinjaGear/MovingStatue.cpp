#include "MovingStatue.h"
#include <glm/gtx/norm.hpp> 

MovingStatue* MovingStatue::createMovingStatue(const glm::vec2& quadSize,
    const glm::vec2& sizeInSpritesheet,
    Texture* spritesheet,
    ShaderProgram* program,
    const glm::vec2& startPos,
    const glm::vec2& endPos,
    float speed)
{
    MovingStatue* statue = new MovingStatue(quadSize, sizeInSpritesheet, spritesheet, program);
    statue->setMovementPath(startPos, endPos, speed);
    return statue;
}

MovingStatue::MovingStatue(const glm::vec2& quadSize,
    const glm::vec2& sizeInSpritesheet,
    Texture* spritesheet,
    ShaderProgram* program)
    : MovingObject(quadSize, sizeInSpritesheet, spritesheet, program)
{
    movementSpeed = 200.0f;  // Default speed
    movingToEnd = true;
    startPosition = glm::vec2(0.0f);
    endPosition = glm::vec2(0.0f);
    targetPosition = endPosition;
    isPaused = true;           // Start paused
    pauseTimer = 0.0f;
    pauseDuration = 2000.0f;
}

void MovingStatue::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram,
    const glm::vec2& spriteSize, const glm::vec2& texCoordSize, Texture* texture)
{
    tileMapDispl = tileMapPos;
    setPosition(glm::vec2(float(tileMapDispl.x + objectPosition.x),
        float(tileMapDispl.y + objectPosition.y)));
}

void MovingStatue::setMovementPath(const glm::vec2& startPos, const glm::vec2& endPos, float speed)
{
    startPosition = startPos;
    endPosition = endPos;
    movementSpeed = speed;
    objectPosition = startPosition;
    targetPosition = endPosition;
    movingToEnd = true;

    isPaused = true;
    pauseTimer = 0.0f;
}

void MovingStatue::updateMovementPattern(int deltaTime)
{
    // If paused, just wait
    if (isPaused) {
        pauseTimer += deltaTime;
        if (pauseTimer >= pauseDuration) {
            isPaused = false;
            pauseTimer = 0.0f;
        }
        return;  // Don't move while paused
    }

    float dt = deltaTime / 1000.0f;  // Convert to seconds

    // Calculate direction to target
    glm::vec2 direction = targetPosition - objectPosition;
    float distanceToTarget = glm::length(direction);

    // If we reached the target, pause and switch direction
    if (distanceToTarget < 2.0f) {
        // Snap to exact position
        objectPosition = targetPosition;

        // Switch target
        if (movingToEnd) {
            targetPosition = startPosition;
            movingToEnd = false;
        }
        else {
            targetPosition = endPosition;
            movingToEnd = true;
        }
		playSoundIfOnScreen("sounds/fastslide.wav");
        // Start pause
        isPaused = true;
        pauseTimer = 0.0f;
        return;
    }

    if (distanceToTarget > 0.0f) {
        direction = glm::normalize(direction);
        glm::vec2 movement = direction * movementSpeed * dt;
        if (glm::length(movement) > distanceToTarget) {
            movement = direction * distanceToTarget;
        }
        objectPosition += movement;
    }
}