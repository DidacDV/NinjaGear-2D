#pragma once
#include "Enemy.h"
class RangedEnemy :
    public Enemy
{
public:
    void update(int deltaTime) override;

protected:
    void initializeAnimations() override;
	void changeAnimationsForDirection(glm::vec2 direction) override;
private:
    enum class State {
        IDLE,
        ATTACKING,
        PATROLLING
    };

    State currentState = State::IDLE;


    // Patrolling
    glm::vec2 spawnPosition;
    glm::ivec2 patrolStartTile;
    glm::ivec2 patrolEndTile;
    bool patrolInitialized = false;
    bool movingToEnd = true;
    void initializePatrol();
    void startPatrol();
    void calculatePatrolPath(const glm::ivec2& targetTile);
    void updatePatrol(int deltaTime, const glm::vec2& playerPos);

};

