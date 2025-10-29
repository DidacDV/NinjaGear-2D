#include "Projectile.h"
#include <iostream>

void Projectile::init(const glm::vec2& startPos, const glm::vec2& direction,
    float speed, ShaderProgram& shaderProgram,
    const string& spritePath, TileMap* map,
    const glm::vec2& sizeInSprite,
    int animationSpeed, 
    const std::vector<glm::vec2>& animationKeyframes,
    bool ignoreCollision
)
{
	this->ignoreCollision = ignoreCollision;
    this->position = startPos;
    this->direction = glm::normalize(direction);
    this->speed = speed;
    this->map = map;
    float rotationAngle = atan2(this->direction.y, this->direction.x);
    const float BASE_OFFSET_RADIANS = 0.785398f;
    rotationAngle += BASE_OFFSET_RADIANS;

    // Load sprite
    spritesheet.loadFromFile(spritePath, TEXTURE_PIXEL_FORMAT_RGBA);
    sprite = Sprite::createSprite(size, sizeInSprite,
        &spritesheet, &shaderProgram);

    if (animationKeyframes.empty()) {
        sprite->setNumberAnimations(1);
        sprite->setAnimationSpeed(0, 8);
        sprite->addKeyframe(0, glm::vec2(0.0f, 0.0f));
    }
    else {
        sprite->setNumberAnimations(1);
        sprite->setAnimationSpeed(0, animationSpeed);
        for (const glm::vec2& keyframeDisplacement : animationKeyframes) {
            sprite->addKeyframe(0, keyframeDisplacement);
        }
    }

    sprite->changeAnimation(0);
    sprite->setRotation(rotationAngle);
}

void Projectile::update(int deltaTime)
{
    if (!active) return;

    // Update lifetime
    lifetime += deltaTime;
    if (lifetime >= maxLifetime) {
        active = false;
        return;
    }

    // Move projectile using lerp for smooth movement
    float moveAmount = speed * (deltaTime / 1000.0f);
    glm::vec2 newPosition = position + (direction * moveAmount);

    // Check wall collision
    position = newPosition;
    if (checkWallCollision() && !ignoreCollision) {
        active = false;
        return;
    }

    sprite->update(deltaTime);
    sprite->setPosition(position);
}

void Projectile::render(const glm::mat4& view)
{
    if (active) {
        sprite->render(view);
    }
}

bool Projectile::checkWallCollision() const
{
    glm::ivec2 tilePos = glm::ivec2(
        position.x / map->getTileSize(),
        position.y / map->getTileSize()
    );

    // Check if tile is solid
    return map->collisionMoveDown(position, size) ||
        map->collisionMoveUp(position, size) ||
        map->collisionMoveLeft(position, size) ||
        map->collisionMoveRight(position, size);
}