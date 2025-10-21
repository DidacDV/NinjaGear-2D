#include "MeleeEnemy.h"
#include <iostream>

void MeleeEnemy::updateStateMachine(int deltaTime)
{
    glm::vec2 playerPos = Game::instance().getPlayerPosition();
    switch (currentState) {
    case State::IDLE:
        if (checkPlayerVisibility(playerPos)) {
            startTracking(playerPos);
        }
        else {
            startPatrol();
        }
        break;

    case State::TRACKING:
        updateTracking(deltaTime, playerPos);
        break;

    case State::PATROLLING:
        updatePatrol(deltaTime, playerPos);
        break;

    case State::RETURNING:
        updateReturning(deltaTime, playerPos);
        break;

    case State::ATTACKING:
        updateAttacking(deltaTime, playerPos);
        break;
    }
}

void MeleeEnemy::initializeAnimations()
{
    cachedTileSize = static_cast<float>(map->getTileSize());
    const float FRAME_WIDTH = 1.0f / 4.0f;
	const float FRAME_HEIGHT = 1.0f / 4.0f;
	sprite->setNumberAnimations(9);
	// STANDING ANIMATIONS
	sprite->setAnimationSpeed(STAND_DOWN, 8);
	sprite->addKeyframe(STAND_DOWN, glm::vec2(0.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));

	sprite->setAnimationSpeed(STAND_UP, 8);
	sprite->addKeyframe(STAND_UP, glm::vec2(1.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));

	sprite->setAnimationSpeed(STAND_LEFT, 8);
	sprite->addKeyframe(STAND_LEFT, glm::vec2(2.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));


	sprite->setAnimationSpeed(STAND_RIGHT, 8);
	sprite->addKeyframe(STAND_RIGHT, glm::vec2(3.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));


	//MOVING ANIMATIONS
	sprite->setAnimationSpeed(MOVE_DOWN, 8);
	sprite->addKeyframe(MOVE_DOWN, glm::vec2(0.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));
	sprite->addKeyframe(MOVE_DOWN, glm::vec2(0.0f * FRAME_WIDTH, 1.0f * FRAME_HEIGHT));
	sprite->addKeyframe(MOVE_DOWN, glm::vec2(0.0f * FRAME_WIDTH, 2.0f * FRAME_HEIGHT));
	sprite->addKeyframe(MOVE_DOWN, glm::vec2(0.0f * FRAME_WIDTH, 3.0f * FRAME_HEIGHT));

	sprite->setAnimationSpeed(MOVE_UP, 8);
	sprite->addKeyframe(MOVE_UP, glm::vec2(1.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));
	sprite->addKeyframe(MOVE_UP, glm::vec2(1.0f * FRAME_WIDTH, 1.0f * FRAME_HEIGHT));
	sprite->addKeyframe(MOVE_UP, glm::vec2(1.0f * FRAME_WIDTH, 2.0f * FRAME_HEIGHT));
	sprite->addKeyframe(MOVE_UP, glm::vec2(1.0f * FRAME_WIDTH, 3.0f * FRAME_HEIGHT));


	sprite->setAnimationSpeed(MOVE_LEFT, 8);
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(2.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(2.0f * FRAME_WIDTH, 1.0f * FRAME_HEIGHT));
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(2.0f * FRAME_WIDTH, 2.0f * FRAME_HEIGHT));
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(2.0f * FRAME_WIDTH, 3.0f * FRAME_HEIGHT));



	sprite->setAnimationSpeed(MOVE_RIGHT, 8);
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(3.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(3.0f * FRAME_WIDTH, 1.0f * FRAME_HEIGHT));
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(3.0f * FRAME_WIDTH, 2.0f * FRAME_HEIGHT));
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(3.0f * FRAME_WIDTH, 3.0f * FRAME_HEIGHT));

	// DANCE
	sprite->setAnimationSpeed(DANCE, 6);
	sprite->addKeyframe(DANCE, glm::vec2(0.0f * FRAME_WIDTH, 6.0f * FRAME_HEIGHT));
	sprite->addKeyframe(DANCE, glm::vec2(1.0f * FRAME_WIDTH, 6.0f * FRAME_HEIGHT));
	sprite->addKeyframe(DANCE, glm::vec2(2.0f * FRAME_WIDTH, 5.0f * FRAME_HEIGHT));
	sprite->addKeyframe(DANCE, glm::vec2(2.0f * FRAME_WIDTH, 6.0f * FRAME_HEIGHT));
	sprite->addKeyframe(DANCE, glm::vec2(3.0f * FRAME_WIDTH, 5.0f * FRAME_HEIGHT));
	sprite->addKeyframe(DANCE, glm::vec2(3.0f * FRAME_WIDTH, 6.0f * FRAME_HEIGHT));

	sprite->changeAnimation(STAND_DOWN); 
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));
}

// Tracking logic
void MeleeEnemy::startTracking(const glm::vec2& playerPos)
{
    originalPatrolPosition = movingToEnd ? patrolEndTile : patrolStartTile; 
    currentState = State::TRACKING;
    trackingTimer = 0;
    pathUpdateTimer = 0;
    recalculatePathToPlayer(playerPos);
}

void MeleeEnemy::updateTracking(int deltaTime, const glm::vec2& playerPos)
{
    trackingTimer += deltaTime;
    pathUpdateTimer += deltaTime;

    float distanceToPlayer = glm::length(playerPos - posEnemy);

    if (distanceToPlayer <= ATTACK_RANGE) {
        startAttacking();
        return;
    }

    const float MAX_TRACKING_DISTANCE = 160.0f; 
    if (distanceToPlayer > MAX_TRACKING_DISTANCE || trackingTimer >= MAX_TRACKING_TIME) {
        stopTracking();
        return;
    }

    if (pathUpdateTimer >= PATH_UPDATE_INTERVAL) {
        if (getPlayerTile(playerPos) != lastTargetTile)  recalculatePathToPlayer(playerPos);
        pathUpdateTimer = 0;
    }

    if (!currentPath.empty()) followPath(deltaTime);
}

void MeleeEnemy::stopTracking()
{
    currentState = State::RETURNING;
    if (!patrolInitialized) initializePatrol();

    glm::ivec2 startTile = getEnemyTile();
    currentPath = Pathfinder::instance().findPath(startTile, originalPatrolPosition, map);
    currentPathIndex = findClosestNodeInPath(currentPath, glm::vec2(posEnemy));

    movingToEnd = (originalPatrolPosition == patrolEndTile);
}

void MeleeEnemy::recalculatePathToPlayer(const glm::vec2& playerPos)
{
    currentPath = Pathfinder::instance().findPath(getEnemyTile(), getPlayerTile(playerPos), map);
    currentPathIndex = findClosestPathIndex(currentPath, glm::vec2(posEnemy));
    lastTargetTile = getPlayerTile(playerPos);

}

// Patrolling logic
void MeleeEnemy::initializePatrol()
{
    glm::ivec2 currentTile = getEnemyTile();

    patrolStartTile = glm::ivec2(
        currentTile.x,
        std::max(0, currentTile.y - 5)
    );

    patrolEndTile = glm::ivec2(
        currentTile.x,
        std::min(map->height() - 1, currentTile.y + 5)
    );

    patrolInitialized = true;
}

void MeleeEnemy::startPatrol()
{
    if (!patrolInitialized) initializePatrol();

    currentState = State::PATROLLING;
    originalPatrolPosition = movingToEnd ? patrolEndTile : patrolStartTile;
    calculatePatrolPath(originalPatrolPosition);
}

void MeleeEnemy::calculatePatrolPath(const glm::ivec2& targetTile)
{
    glm::ivec2 startTile = getEnemyTile();
    currentPath = Pathfinder::instance().findPath(startTile, targetTile, map);
    currentPathIndex = findClosestNodeInPath(currentPath, glm::vec2(posEnemy));

}

void MeleeEnemy::updatePatrol(int deltaTime, const glm::vec2& playerPos)
{
    if (checkPlayerVisibility(playerPos)) {
        startTracking(playerPos);
        return;
    }

    glm::ivec2 currentTarget = movingToEnd ? patrolEndTile : patrolStartTile;

    if (!followPathToTarget(deltaTime, currentTarget)) return; 

    movingToEnd = !movingToEnd;
    sprite->changeAnimation(movingToEnd ? MOVE_DOWN : MOVE_UP);
    calculatePatrolPath(movingToEnd ? patrolEndTile : patrolStartTile);
}

// Returning logic
void MeleeEnemy::updateReturning(int deltaTime, const glm::vec2& playerPos)
{
    if (checkPlayerVisibility(playerPos)) {
        startTracking(playerPos);
        return;
    }

    glm::ivec2 returnTarget = movingToEnd ? patrolEndTile : patrolStartTile;

    if (!followPathToTarget(deltaTime, returnTarget)) {
        return;
    }

    currentState = State::PATROLLING;
    calculatePatrolPath(movingToEnd ? patrolEndTile : patrolStartTile);
}

void MeleeEnemy::changeAnimationsForDirection(glm::vec2 direction)
{
    Direction newDirection;
    Anims newAnimation;

    if (std::abs(direction.x) > std::abs(direction.y)) {
        newAnimation = direction.x > 0 ? MOVE_RIGHT : MOVE_LEFT;
        newDirection = direction.x > 0 ? RIGHT : LEFT;
    }
    else {
        newAnimation = direction.y > 0 ? MOVE_DOWN : MOVE_UP;
        newDirection = direction.y > 0 ? DOWN : UP;
    }

    if (sprite->animation() != newAnimation) {
        sprite->changeAnimation(newAnimation);
        this->currentDirection = newDirection;
    }
}

// Attacking logic
void MeleeEnemy::startAttacking()
{
    currentState = State::ATTACKING;
    attackCooldownTimer = 0;

    switch (currentDirection) {
    case UP:    sprite->changeAnimation(STAND_UP); break;
    case DOWN:  sprite->changeAnimation(STAND_DOWN); break;
    case LEFT:  sprite->changeAnimation(STAND_LEFT); break;
    case RIGHT: sprite->changeAnimation(STAND_RIGHT); break;
    }
}

void MeleeEnemy::updateAttacking(int deltaTime, const glm::vec2& playerPos)
{
    float distanceToPlayer = glm::length(playerPos - posEnemy);

    // If player moves out of range, resume tracking
    if (distanceToPlayer > ATTACK_RANGE) {
        startTracking(playerPos);
        return;
    }

    // Face the player while attacking
    glm::vec2 directionToPlayer = glm::normalize(playerPos - posEnemy);

    if (std::abs(directionToPlayer.x) > std::abs(directionToPlayer.y)) {
        if (directionToPlayer.x > 0) {
            sprite->changeAnimation(STAND_RIGHT);
            currentDirection = RIGHT;
        }
        else {
            sprite->changeAnimation(STAND_LEFT);
            currentDirection = LEFT;
        }
    }
    else {
        if (directionToPlayer.y > 0) {
            sprite->changeAnimation(STAND_DOWN);
            currentDirection = DOWN;
        }
        else {
            sprite->changeAnimation(STAND_UP);
            currentDirection = UP;
        }
    }
}  

bool MeleeEnemy::isInAttackState() const
{
    return currentState == State::ATTACKING;
}

bool MeleeEnemy::canDealDamage() const
{
    return currentState == State::ATTACKING && attackCooldownTimer <= 0;
}
