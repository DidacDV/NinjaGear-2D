#pragma once
#include "Enemy.h"
#include "Game.h"

class MeleeEnemy :
    public Enemy
{
public:
    void update(int deltaTime) override;
protected:
    void initializeAnimations() override;
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

    // Path following
    std::vector<glm::ivec2> currentPath;
    int currentPathIndex = 0;
    int pathUpdateTimer = 0;
    float moveSpeed = 5.0f; // tiles per second
    float moveProgress = 0.0f;
    glm::ivec2 lastTargetTile;

    // Patrolling
    glm::ivec2 patrolStartTile;
    glm::ivec2 patrolEndTile;
    bool patrolInitialized = false;
    bool movingToEnd = true;

    // Methods
    void startTracking(const glm::vec2& playerPos);
    void updateTracking(int deltaTime, const glm::vec2& playerPos);
    void stopTracking();
    void followPath(int deltaTime);
    int findClosestPathIndex(const std::vector<glm::ivec2>& path, const glm::vec2& worldPos) const;
    void recalculatePathToPlayer(const glm::vec2& playerPos);

    void initializePatrol();
    void startPatrol();
    void calculatePatrolPath(const glm::ivec2& targetTile);
    void updatePatrol(int deltaTime, const glm::vec2& playerPos);
};

