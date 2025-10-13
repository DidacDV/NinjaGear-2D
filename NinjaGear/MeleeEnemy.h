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
        RETURNING
    };

    State currentState = State::IDLE;
    int trackingTimer = 0;
    const int MAX_TRACKING_TIME = 50000; 
    const int PATH_UPDATE_INTERVAL = 500;

    // Path following
    std::vector<glm::ivec2> currentPath;
    int currentPathIndex = 0;
    int pathUpdateTimer = 0;
    float moveSpeed = 3.0f; // tiles per second
    float moveProgress = 0.0f;
    glm::ivec2 lastTargetTile;

    // Methods
    void startTracking(const glm::vec2& playerPos);
    void updateTracking(int deltaTime, const glm::vec2& playerPos);
    void stopTracking();
    void followPath(int deltaTime);
};

