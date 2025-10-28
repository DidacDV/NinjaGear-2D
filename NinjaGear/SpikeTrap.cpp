#include "SpikeTrap.h"

SpikeTrap::SpikeTrap(const glm::vec2& quadSize,
    const glm::vec2& sizeInSpritesheet,
    Texture* spritesheet,
    ShaderProgram* program)
    : MovingObject(quadSize, sizeInSpritesheet, spritesheet, program)
{
    currentState = IDLE;
    stateTimer = 0;
    timerOffset = 0;             
    idleDuration = 2000;         
    spikeDuration = 1000;        
    transitionDuration = 500;    
    damageAmount = 1;
}

void SpikeTrap::update(int deltaTime)
{
    MovingObject::update(deltaTime);
    if (!isOnScreen(320, 320)) return;

    stateTimer += deltaTime;

    int currentDuration = 0;
    switch (currentState) {
    case IDLE:
        currentDuration = idleDuration;
        break;
    case TRANSITION_UP:
        currentDuration = transitionDuration;
        break;
    case SPIKE:
        currentDuration = spikeDuration;
        break;
    case TRANSITION_DOWN:
        currentDuration = transitionDuration;
        break;
    }

    if (stateTimer >= currentDuration) {
        stateTimer -= currentDuration;  
        switchState();
    }
}

void SpikeTrap::switchState()
{
    switch (currentState) {
    case IDLE:
        currentState = TRANSITION_UP;
        changeAnimation(2);
        break;
    case TRANSITION_UP:
        currentState = SPIKE;
        changeAnimation(0);
        break;
    case SPIKE:
        currentState = TRANSITION_DOWN;
        changeAnimation(2);
        break;
    case TRANSITION_DOWN:
        currentState = IDLE;
        changeAnimation(1);
        break;
    }
}

void SpikeTrap::setCycleDuration(int idleMs, int spikeMs, int transitionMs)
{
    idleDuration = idleMs;
    spikeDuration = spikeMs;
    transitionDuration = transitionMs;
}
