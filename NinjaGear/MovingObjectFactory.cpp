#include "MovingObjectFactory.h"
#include "MovingStatue.h"
#include "SpikeTrap.h"

MovingObject* MovingObjectFactory::create(
    const MovingObjectConfig& config,
    Texture* texture,
    ShaderProgram* program,
    const std::vector<TileMap*>& maps,
    const glm::ivec2& screenPos)
{
    MovingObject* obj = nullptr;

    switch (config.type) {
    case MovingObjectType::MOVING_STATUE:
        obj = createMovingStatue(config, texture, program);
        break;
    case MovingObjectType::SPIKE_TRAP:
        obj = createSpikeTrap(config, texture, program);
        break;
    default:
        return nullptr;
    }

    if (obj) {
        obj->setTileMapPosition(glm::ivec2(config.startPos.x, config.startPos.y), screenPos);
        obj->setTileMaps(maps);

        if (config.type != MovingObjectType::SPIKE_TRAP) {
            obj->setDamageProperties(config.dealsDamage, config.damage);
        }
    }

    return obj;
}

MovingObject* MovingObjectFactory::createMovingStatue(
    const MovingObjectConfig& config,
    Texture* texture,
    ShaderProgram* program)
{
    MovingStatue* statue = new MovingStatue(config.spriteSize, config.texCoordSize, texture, program);

    statue->setMovementPath(config.startPos, config.endPos, config.speed);
    statue->setCollisionBox(glm::vec2(30.0f, 35.0f));
    statue->setNumberAnimations(1);
    statue->setAnimationSpeed(0, 8);
    statue->addKeyframe(0, glm::vec2(0.0f, 0.0f));
    statue->changeAnimation(0);

    return statue;
}

MovingObject* MovingObjectFactory::createSpikeTrap(
    const MovingObjectConfig& config,
    Texture* texture,
    ShaderProgram* program)
{
    SpikeTrap* trap = new SpikeTrap(config.spriteSize, config.texCoordSize, texture, program);

    trap->setNumberAnimations(3);

    trap->setAnimationSpeed(0, 8);
    trap->addKeyframe(0, glm::vec2(0.0f, 0.0f));

    trap->setAnimationSpeed(1, 8);
    trap->addKeyframe(1, glm::vec2(0.5f, 0.0f));

    trap->setAnimationSpeed(2, 8);
    trap->addKeyframe(2, glm::vec2(0.0f, 0.5f));

    trap->changeAnimation(0);
    trap->setCollisionBox(glm::vec2(8.0f, 8.0f));
    trap->setCycleDuration(config.idleDuration, config.spikeDuration, 500);
    trap->setTimerOffset(config.timerOffset);
    trap->setDamage(config.damage);

    return trap;
}