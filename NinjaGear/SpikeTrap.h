#pragma once
#include "MovingObject.h"

class SpikeTrap : public MovingObject
{
public:
    enum State {
        IDLE,
        SPIKE
    };

    SpikeTrap(const glm::vec2& quadSize,
        const glm::vec2& sizeInSpritesheet,
        Texture* spritesheet,
        ShaderProgram* program);

    void update(int deltaTime) override;

    void setCycleDuration(int idleMs, int spikeMs);
    void updateMovementPattern(int deltaTime) override {};

    bool isActive() const { return currentState == SPIKE; }
    State getCurrentState() const { return currentState; }

    bool getDealsDamage() const override { return currentState == SPIKE; }
private:
    State currentState;
    int stateTimer;
    int idleDuration;   // Time in IDLE state (ms)
    int spikeDuration;  // Time in SPIKE state (ms)

    void switchState();
};

