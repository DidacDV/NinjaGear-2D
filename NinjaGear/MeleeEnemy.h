#pragma once
#include "Enemy.h"
#include "Game.h"

enum MeleeEnemyAnims
{
    STAND_LEFT, STAND_RIGHT, STAND_UP, STAND_DOWN,
    MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN,
    DANCE
};


class MeleeEnemy :
    public Enemy
{
public:
    enum Anims {
        STAND_LEFT, STAND_RIGHT, STAND_UP, STAND_DOWN,
        MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN,
        DANCE
    };
    void update(int deltaTime) override;

protected:
    void initializeAnimations() override;
    void changeAnimationsForDirection(glm::vec2 direction) override;
private: 
    enum class State {
        IDLE,
        TRACKING,
        RETURNING,
        PATROLLING
    };

    State currentState = State::IDLE;
    int trackingTimer = 0;
    const int MAX_TRACKING_TIME = 50000; 
    const int PATH_UPDATE_INTERVAL = 100;

    // Tracking
    void startTracking(const glm::vec2& playerPos);
    void updateTracking(int deltaTime, const glm::vec2& playerPos);
    void stopTracking();
    void recalculatePathToPlayer(const glm::vec2& playerPos);

    //Patrolling
    glm::vec2 spawnPosition;
    glm::ivec2 patrolStartTile;
    glm::ivec2 patrolEndTile;
    glm::ivec2 originalPatrolPosition;
    bool patrolInitialized = false;
    bool movingToEnd = true;
    void initializePatrol();
    void startPatrol();
    void calculatePatrolPath(const glm::ivec2& targetTile);
    void updatePatrol(int deltaTime, const glm::vec2& playerPos);

    //Returning
    void updateReturning(int deltaTime, const glm::vec2& playerPos);
};

