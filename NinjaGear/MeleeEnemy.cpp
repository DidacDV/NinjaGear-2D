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
        break;

    case State::TRACKING:
        updateTracking(deltaTime, playerPos);
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

	sprite->changeAnimation(STAND_DOWN); // Cambia a una animación que tenga keyframes válidos
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));
}

void MeleeEnemy::startTracking(const glm::vec2& playerPos)
{
    std::cout << "I see you! Starting tracking..." << std::endl;
    currentState = State::TRACKING;
    trackingTimer = 0;
    pathUpdateTimer = 0;

    // Calculate initial path
    glm::ivec2 startTile = Pathfinder::instance().worldToTile(
        posEnemy,
        map->getTileSize()
    );
    glm::ivec2 goalTile = Pathfinder::instance().worldToTile(
        playerPos, map->getTileSize()
    );


    currentPath = Pathfinder::instance().findPath(startTile, goalTile, map);
    currentPathIndex = 0;
    lastTargetTile = goalTile;
}

void MeleeEnemy::updateTracking(int deltaTime, const glm::vec2& playerPos)
{
    trackingTimer += deltaTime;
    pathUpdateTimer += deltaTime;

    // Check if tracking time expired
    if (trackingTimer >= MAX_TRACKING_TIME) {
        stopTracking();
        return;
    }

    // Periodically recalculate path (player is moving)
    if (pathUpdateTimer >= PATH_UPDATE_INTERVAL) {
        glm::ivec2 goalTile = Pathfinder::instance().worldToTile(
            playerPos, map->getTileSize()
        );

        // Only recalculate if player moved to different tile
        if (goalTile != lastTargetTile) {
            glm::ivec2 startTile = Pathfinder::instance().worldToTile(
                glm::vec2(posEnemy), map->getTileSize()
            );
            currentPath = Pathfinder::instance().findPath(
                startTile, goalTile, map
            );
            currentPathIndex = 0;
            lastTargetTile = goalTile;
        }
        pathUpdateTimer = 0;
    }

    // Follow the current path
    if (!currentPath.empty()) {
        followPath(deltaTime);
    }
}

void MeleeEnemy::followPath(int deltaTime)
{
    std::cout << "followPath called: index=" << currentPathIndex
        << "/" << currentPath.size() << std::endl; 

    if (currentPathIndex >= currentPath.size()) {
        std::cout << "Reached end of path!" << std::endl; 
        return;
    }

    glm::ivec2 targetTile = currentPath[currentPathIndex];
    glm::vec2 targetPos = glm::vec2(
        targetTile.x * map->getTileSize(),
        targetTile.y * map->getTileSize()
    );

    std::cout << "Current pos: (" << posEnemy.x << ", " << posEnemy.y << ")" << std::endl; 
    std::cout << "Target pos: (" << targetPos.x << ", " << targetPos.y << ")" << std::endl; 

    glm::vec2 direction = targetPos - glm::vec2(posEnemy);
    float distance = glm::length(direction);

    std::cout << "Distance: " << distance << std::endl; 
    if (distance < 2.0f) {
        currentPathIndex++;
        std::cout << "Reached waypoint, moving to next" << std::endl; 
    }
    else {
        direction = glm::normalize(direction);
        float moveAmount = moveSpeed * map->getTileSize() * (deltaTime / 1000.0f);

        std::cout << "Moving by: " << moveAmount << std::endl; 
        posEnemy.x += direction.x * moveAmount;
        posEnemy.y += direction.y * moveAmount;

        std::cout << "New pos: (" << posEnemy.x << ", " << posEnemy.y << ")" << std::endl;
    }
}

void MeleeEnemy::stopTracking()
{
    std::cout << "Stopping tracking..." << std::endl;
    currentState = State::IDLE;
    currentPath.clear();
    currentPathIndex = 0;
}