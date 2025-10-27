#include "RangedEnemy.h"
#include "ProjectileManager.h"

void RangedEnemy::initializeAnimations()
{
	cachedTileSize = static_cast<float>(map->getTileSize());
	const float FRAME_WIDTH = 1.0f / 4.0f;
	const float FRAME_HEIGHT = 1.0f / 4.0f;
	sprite->setNumberAnimations(8);
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

	sprite->changeAnimation(STAND_DOWN);
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));
}


void RangedEnemy::updateStateMachine(int deltaTime)
{
	glm::vec2 playerPos = Game::instance().getPlayerPosition();

	switch (currentState) {
	case State::IDLE:
		startPatrol();
		break;

	case State::PATROLLING:
		updatePatrol(deltaTime, playerPos);
		break;

	case State::ATTACKING:
		updateAttacking(deltaTime, playerPos);
		break;
	}
}


// Patrolling logic
void RangedEnemy::initializePatrol()
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

void RangedEnemy::startPatrol()
{
	if (!patrolInitialized) initializePatrol();

	currentState = State::PATROLLING;
	calculatePatrolPath(movingToEnd ? patrolEndTile : patrolStartTile);
}

void RangedEnemy::calculatePatrolPath(const glm::ivec2& targetTile)
{
	glm::ivec2 startTile = getEnemyTile();
	currentPath = Pathfinder::instance().findPath(startTile, targetTile, map);
	currentPathIndex = findClosestNodeInPath(currentPath, glm::vec2(posEnemy));
}

void RangedEnemy::updatePatrol(int deltaTime, const glm::vec2& playerPos)
{
	if (checkPlayerVisibility(playerPos)) {
		startAttacking(playerPos);
		return;
	}

	glm::ivec2 currentTarget = movingToEnd ? patrolEndTile : patrolStartTile;

	if (!followPathToTarget(deltaTime, currentTarget)) return;

	movingToEnd = !movingToEnd;
	sprite->changeAnimation(movingToEnd ? MOVE_DOWN : MOVE_UP);
	calculatePatrolPath(movingToEnd ? patrolEndTile : patrolStartTile);
}

void RangedEnemy::changeAnimationsForDirection(glm::vec2 direction)
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
		currentDirection = newDirection;
		sprite->changeAnimation(newAnimation);
	}
}

// Attacking logic 
void RangedEnemy::startAttacking(const glm::vec2& playerPos)
{
	currentState = State::ATTACKING;
	attackingTimer = 0;
	attackingTile = getEnemyTile();  

	glm::vec2 directionToPlayer = glm::normalize(playerPos - posEnemy);
	changeAnimationsForDirection(directionToPlayer);
}

void RangedEnemy::updateAttacking(int deltaTime, const glm::vec2& playerPos)
{
	attackingTimer += deltaTime;
	shootCooldownTimer += deltaTime;
	float distanceToPlayer = glm::length(playerPos - posEnemy);

	if (distanceToPlayer > MAX_ATTACKING_DISTANCE || 
		attackingTimer >= MAX_ATTACKING_TIME 
		)
	{
		stopAttacking();
		return;
	}

	if (shootCooldownTimer >= SHOOT_COOLDOWN) {
		shootAtPlayer(playerPos);
		shootCooldownTimer = 0;
	}


	glm::ivec2 currentTile = getEnemyTile();
	if (currentTile != attackingTile) {
		posEnemy = glm::vec2(
			attackingTile.x * map->getTileSize(),
			attackingTile.y * map->getTileSize()
		);
	}

	glm::vec2 directionToPlayer = glm::normalize(playerPos - posEnemy);
	changeAnimationsForDirection(directionToPlayer);
}

void RangedEnemy::stopAttacking()
{
	currentState = State::PATROLLING;

	if (!patrolInitialized) initializePatrol();
	calculatePatrolPath(movingToEnd ? patrolEndTile : patrolStartTile);
}

bool RangedEnemy::isInAttackState() const
{
	return currentState == State::ATTACKING;
}

void RangedEnemy::shootAtPlayer(const glm::vec2& playerPos)
{
	if (!projectileManager) return;

	glm::vec2 direction = glm::normalize(playerPos - posEnemy);
	float speed = 100.0f; // pixels per second
	int damage = 15;

	projectileManager->spawnProjectile(
		posEnemy,
		direction,
		speed,
		damage,
		"images/projectiles/Arrow.png"
	);
}

void RangedEnemy::onDamageReceived()
{
	// Placeholder for damage reaction (e.g., flash red, play sound)
}