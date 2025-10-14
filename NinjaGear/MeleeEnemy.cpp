#include "MeleeEnemy.h"
#include <iostream>
#include "Pathfinder.h"
#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 96
#define FALL_STEP 4
#define SPRITE_SIZE 16

enum MeleeEnemyAnims
{
	STAND_LEFT, STAND_RIGHT, STAND_UP, STAND_DOWN,
	MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN,
	DANCE
};

void MeleeEnemy::update(int deltaTime)
{
	sprite->update(deltaTime);
    glm::vec2 playerPos = Game::instance().getPlayerPosition();

    // State machine logic
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
        stopTracking();
        break;
    }

    sprite->setPosition(glm::vec2(
        float(tileMapDispl.x + posEnemy.x),
        float(tileMapDispl.y + posEnemy.y)
    ));
}

void MeleeEnemy::initializeAnimations()
{
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

// Path following logic
void MeleeEnemy::startTracking(const glm::vec2& playerPos)
{
    //State change
    currentState = State::TRACKING;
    //Start timers
    trackingTimer = 0;
    pathUpdateTimer = 0;
	//Calculate initial path
    recalculatePathToPlayer(playerPos);
}

void MeleeEnemy::updateTracking(int deltaTime, const glm::vec2& playerPos)
{
    trackingTimer += deltaTime;
    pathUpdateTimer += deltaTime;

    const float MAX_TRACKING_DISTANCE = 160.0f; 
    cout << glm::length(playerPos - posEnemy) << endl;
    if ((glm::length(playerPos - posEnemy) > MAX_TRACKING_DISTANCE) || trackingTimer >= MAX_TRACKING_TIME) {
        stopTracking();
        return;
    }


    if (pathUpdateTimer >= PATH_UPDATE_INTERVAL) {
		// Recalculate path to detect if the player has moved
        glm::ivec2 goalTile = Pathfinder::instance().worldToTile(playerPos, map->getTileSize());
        if (goalTile != lastTargetTile) {
            recalculatePathToPlayer(playerPos);
        }
        pathUpdateTimer = 0;
    }

    if (!currentPath.empty()) {
		//If the path is not empty, follow it
        followPath(deltaTime);
    }
}

void MeleeEnemy::followPath(int deltaTime)
{
    if (currentPathIndex >= static_cast<int>(currentPath.size())) return;

    const float tileSize = static_cast<float>(map->getTileSize());
    glm::vec2 enemyPos(posEnemy);

    while (currentPathIndex < static_cast<int>(currentPath.size())) {
        glm::vec2 targetPos(currentPath[currentPathIndex].x * tileSize,
            currentPath[currentPathIndex].y * tileSize);
        glm::vec2 direction = targetPos - enemyPos;
        float distance = glm::length(direction);

        if (distance < 4.0f) { // NODE_THRESHOLD
            ++currentPathIndex;
            continue;
        }

        if (distance > 0.1f) { // MIN_MOVE_DISTANCE
            direction = glm::normalize(direction);
            float moveAmount = moveSpeed * tileSize * (deltaTime / 1000.0f);
            posEnemy += direction * moveAmount;

            if (std::abs(direction.x) > std::abs(direction.y)) {
                sprite->changeAnimation(direction.x > 0 ? MOVE_RIGHT : MOVE_LEFT);
            }
            else {
                sprite->changeAnimation(direction.y > 0 ? MOVE_DOWN : MOVE_UP);
            }
        }
        return;
    }

    sprite->changeAnimation(STAND_DOWN);
}

void MeleeEnemy::stopTracking()
{
    startPatrol();
    currentPath.clear();
    currentPathIndex = 0;
}

int MeleeEnemy::findClosestPathIndex(const std::vector<glm::ivec2>& path, const glm::vec2& worldPos) const
{
    if (path.empty()) return 0;

    const float tileSize = static_cast<float>(map->getTileSize());
    float bestDist = std::numeric_limits<float>::max();
    int bestIdx = 0;

    for (int i = 0; i < static_cast<int>(path.size()); ++i) {
        float d = glm::length(glm::vec2(path[i].x * tileSize, path[i].y * tileSize) - worldPos);
        if (d < bestDist) {
            bestDist = d;
            bestIdx = i;
        }
    }

    if (bestIdx == 0 && bestDist < tileSize * 0.35f && static_cast<int>(path.size()) > 1) {
        return 1;
    }

    return bestIdx;
}

void MeleeEnemy::recalculatePathToPlayer(const glm::vec2& playerPos)
{
    currentPath = Pathfinder::instance().findPath(
        // Enemy position
        Pathfinder::instance().worldToTile(glm::vec2(posEnemy), map->getTileSize()),
        //Player position
        Pathfinder::instance().worldToTile(playerPos, map->getTileSize()),
        map
    );

    currentPathIndex = findClosestPathIndex(currentPath, glm::vec2(posEnemy));
    lastTargetTile = Pathfinder::instance().worldToTile(playerPos, map->getTileSize());

}

// Patrolling logic

void MeleeEnemy::initializePatrol()
{
    patrolStartTile = glm::ivec2(
        Pathfinder::instance().worldToTile(glm::vec2(posEnemy), map->getTileSize()).x,
        std::max(0, Pathfinder::instance().worldToTile(glm::vec2(posEnemy), map->getTileSize()).y - 5)
    );

    patrolEndTile = glm::ivec2(
        Pathfinder::instance().worldToTile(glm::vec2(posEnemy), map->getTileSize()).x,
        std::min(map->height() - 1, Pathfinder::instance().worldToTile(glm::vec2(posEnemy), map->getTileSize()).y + 5)
    );

    // Initialize animation to DOWN (moving to end)
    sprite->changeAnimation(MOVE_DOWN);
    patrolInitialized = true;
    movingToEnd = true;
}

void MeleeEnemy::startPatrol()
{
    if (!patrolInitialized) {
        initializePatrol();
    }

    currentState = State::PATROLLING;

    glm::ivec2 targetTile = movingToEnd ? patrolEndTile : patrolStartTile;
    calculatePatrolPath(targetTile);
}

void MeleeEnemy::calculatePatrolPath(const glm::ivec2& targetTile)
{
    glm::ivec2 startTile = Pathfinder::instance().worldToTile(glm::vec2(posEnemy), map->getTileSize());

    currentPath = Pathfinder::instance().findPath(startTile, targetTile, map);
    currentPathIndex = findClosestPathIndex(currentPath, glm::vec2(posEnemy));
}

void MeleeEnemy::updatePatrol(int deltaTime, const glm::vec2& playerPos)
{
    // Check if player became visible
    if (checkPlayerVisibility(playerPos)) {
        startTracking(playerPos);
        return;
    }

    // Follow current patrol path
    if (!currentPath.empty() && currentPathIndex < static_cast<int>(currentPath.size())) {
        followPath(deltaTime);
    }
    else {
        // Reached endpoint, switch direction
        (movingToEnd) ? sprite->changeAnimation(MOVE_DOWN) : sprite->changeAnimation(MOVE_UP);
        movingToEnd = !movingToEnd;
        glm::ivec2 nextTarget = movingToEnd ? patrolEndTile : patrolStartTile;
        calculatePatrolPath(nextTarget);
    }
}