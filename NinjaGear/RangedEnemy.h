#pragma once
#include "Enemy.h"

class RangedEnemy :
    public Enemy
{
public:
    enum Anims {
        STAND_LEFT, STAND_RIGHT, STAND_UP, STAND_DOWN,
        MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN,
        DANCE
    };
    bool isInAttackState() const override;

protected:
    void initializeAnimations() override;
	void changeAnimationsForDirection(glm::vec2 direction) override;
    void updateStateMachine(int deltaTime) override;
	void onDamageReceived() override;

private:
    enum class State {
        IDLE,
        ATTACKING,
        PATROLLING
    };
    State currentState = State::IDLE;

    // Patrolling
    glm::ivec2 patrolStartTile;
    glm::ivec2 patrolEndTile;
    bool patrolInitialized = false;
    bool movingToEnd = true;
    void initializePatrol();
    void startPatrol();
    void calculatePatrolPath(const glm::ivec2& targetTile);
    void updatePatrol(int deltaTime, const glm::vec2& playerPos);

    // Attacking
    int attackingTimer = 0;
    const int MAX_ATTACKING_TIME = 5000;  
    const float MAX_ATTACKING_DISTANCE = 160.0f; 
    glm::ivec2 attackingTile; 
    void startAttacking(const glm::vec2& playerPos);
    void updateAttacking(int deltaTime, const glm::vec2& playerPos);
    void stopAttacking();

    int shootCooldownTimer = 0;
    const int SHOOT_COOLDOWN = 1000;
    void shootAtPlayer(const glm::vec2& playerPos);

};

