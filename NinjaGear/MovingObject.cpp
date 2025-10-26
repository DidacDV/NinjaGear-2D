#include "MovingObject.h"
#include "ServiceLocator.h"
#include <iostream>

MovingObject* MovingObject::createMovingObject(const glm::vec2& quadSize,
    const glm::vec2& sizeInSpritesheet,
    Texture* spritesheet,
    ShaderProgram* program)
{
    return new MovingObject(quadSize, sizeInSpritesheet, spritesheet, program);
}

MovingObject::MovingObject(const glm::vec2& quadSize,
    const glm::vec2& sizeInSpritesheet,
    Texture* spritesheet,
    ShaderProgram* program)
    : Sprite(quadSize, sizeInSpritesheet, spritesheet, program)
{
    objectPosition = glm::vec2(0.f);
    velocity = glm::vec2(0.f);
    tileMapDispl = glm::ivec2(0);
    cameraOffset = glm::vec2(0.f);
}

void MovingObject::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram,
    const glm::vec2& spriteSize, const glm::vec2& texCoordSize, Texture* texture)
{
	cout << "MovingObject init used. Should be overriden" << endl;
}

void MovingObject::setMovementPath(const glm::vec2& startPos, const glm::vec2& endPos, float speed)
{
    cout << "MovingObject setMovementPath used. Should be overriden" << endl;
}

void MovingObject::update(int deltaTime)
{
    Sprite::update(deltaTime);
    updateMovementPattern(deltaTime);
    setPosition(glm::vec2(float(tileMapDispl.x + objectPosition.x),
        float(tileMapDispl.y + objectPosition.y)));
}

void MovingObject::updateMovementPattern(int deltaTime)
{
    // Base implementation: simple velocity-based movement
    cout << "MovingObject updateMovementPattern used. Should be overriden" << endl;
}

void MovingObject::setTileMapPosition(const glm::ivec2& tilePos, const glm::ivec2& tileMapDispl)
{
    this->objectPosition = glm::vec2(tilePos);
    this->tileMapDispl = tileMapDispl;
    setPosition(glm::vec2(float(tileMapDispl.x + objectPosition.x),
        float(tileMapDispl.y + objectPosition.y)));
}

void MovingObject::setTileMaps(const std::vector<TileMap*>& tileMaps)
{
    maps = tileMaps;
}

bool MovingObject::collisionMoveLeft(const glm::ivec2& pos, const glm::ivec2& size) const
{
    for (TileMap* map : maps) {
        if (map->collisionMoveLeft(pos, size))
            return true;
    }
    return false;
}

bool MovingObject::collisionMoveRight(const glm::ivec2& pos, const glm::ivec2& size) const
{
    for (TileMap* map : maps) {
        if (map->collisionMoveRight(pos, size))
            return true;
    }
    return false;
}

bool MovingObject::collisionMoveDown(const glm::ivec2& pos, const glm::ivec2& size) const
{
    for (TileMap* map : maps) {
        if (map->collisionMoveDown(pos, size))
            return true;
    }
    return false;
}

bool MovingObject::collisionMoveUp(const glm::ivec2& pos, const glm::ivec2& size) const
{
    for (TileMap* map : maps) {
        if (map->collisionMoveUp(pos, size))
            return true;
    }
    return false;
}

bool MovingObject::isOnScreen(int cameraWidth, int cameraHeight, float margin) const
{
    glm::vec2 worldPos = glm::vec2(tileMapDispl.x + objectPosition.x,
        tileMapDispl.y + objectPosition.y);
    glm::vec2 screenPos = worldPos + cameraOffset;
    return (screenPos.x > -margin && screenPos.x < cameraWidth + margin &&
        screenPos.y > -margin && screenPos.y < cameraHeight + margin);
}

void MovingObject::playSoundIfOnScreen(const std::string& soundFile) const
{
    if (isOnScreen(320, 320,0.0f)) {
        ServiceLocator::getAudio().playSound(soundFile.c_str());
    }
}