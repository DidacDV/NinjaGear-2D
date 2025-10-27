#include "SpikeTrap.h"

SpikeTrap::SpikeTrap(const glm::vec2& quadSize,
    const glm::vec2& sizeInSpritesheet,
    Texture* spritesheet,
    ShaderProgram* program)
    : MovingObject(quadSize, sizeInSpritesheet, spritesheet, program)
{
    currentState = IDLE;
    stateTimer = 0;
    idleDuration = 2000;   // Default: 2 seconds idle
    spikeDuration = 1000;  // Default: 1 second with spikes


    dealsDamage = false; 
    damageAmount = 1;
}

void SpikeTrap::update(int deltaTime)
{
    MovingObject::update(deltaTime);

    if (!isOnScreen(320, 320)) return;

    stateTimer += deltaTime;

    int currentDuration = (currentState == IDLE) ? idleDuration : spikeDuration;

    if (stateTimer >= currentDuration) {
        stateTimer = 0; 
        switchState();
    }
}

void SpikeTrap::switchState()
{
    if (currentState == IDLE) {
        currentState = SPIKE;
        changeAnimation(0); 
    }
    else {
        currentState = IDLE;
        changeAnimation(1);  
    }
}

void SpikeTrap::setCycleDuration(int idleMs, int spikeMs)
{
    idleDuration = idleMs;
    spikeDuration = spikeMs;
}