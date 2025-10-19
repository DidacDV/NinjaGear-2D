#include "Enemy.h"
#include <cmath>
#include <iostream>
#include <GL/glew.h>


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 96
#define FALL_STEP 4
#define SPRITE_SIZE 16

enum EnemyAnims
{
	STAND_LEFT, STAND_RIGHT, STAND_UP, STAND_DOWN,
	MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN,
};

void Enemy::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, TileMap* tileMap)
{
	//Template that every enemy will follow. 
	initializeSprite(shaderProgram);
	map = tileMap;
	cachedTileSize = static_cast<float>(map->getTileSize());
	bJumping = false;
	tileMapDispl = tileMapPos;
	currentDirection = DOWN;
	initializeAnimations();
}

void Enemy::update(int deltaTime)
{
	sprite->update(deltaTime);
	if (Game::instance().getKey(GLFW_KEY_J))
	{
		currentDirection = LEFT;
		if (sprite->animation() != MOVE_LEFT)
			sprite->changeAnimation(MOVE_LEFT);
		posEnemy.x -= 2;
		if (map->collisionMoveLeft(posEnemy, glm::ivec2(SPRITE_SIZE, SPRITE_SIZE)))
		{
			posEnemy.x += 2;
			sprite->changeAnimation(STAND_LEFT);
		}
	}
	else if (Game::instance().getKey(GLFW_KEY_L))
	{
		currentDirection = RIGHT;
		if (sprite->animation() != MOVE_RIGHT)
			sprite->changeAnimation(MOVE_RIGHT);
		posEnemy.x += 2;
		if (map->collisionMoveRight(posEnemy, glm::ivec2(SPRITE_SIZE, SPRITE_SIZE)))
		{
			posEnemy.x -= 2;
			sprite->changeAnimation(STAND_RIGHT);
		}
	}
	else if (Game::instance().getKey(GLFW_KEY_K))
	{
		currentDirection = DOWN;
		if (sprite->animation() != MOVE_DOWN)
			sprite->changeAnimation(MOVE_DOWN);
		posEnemy.y += 2;  // Move down (increase Y)
		if (map->collisionMoveDown(posEnemy, glm::ivec2(SPRITE_SIZE, SPRITE_SIZE)))
		{
			posEnemy.y -= 2;  // Cancel movement
			sprite->changeAnimation(STAND_DOWN);
		}
	}
	else if (Game::instance().getKey(GLFW_KEY_I))
	{
		currentDirection = UP;
		if (sprite->animation() != MOVE_UP)
			sprite->changeAnimation(MOVE_UP);
		posEnemy.y -= 2;  // Move down (increase Y)
		if (map->collisionMoveUp(posEnemy, glm::ivec2(SPRITE_SIZE, SPRITE_SIZE)))
		{
			posEnemy.y += 2;  // Cancel movement
			sprite->changeAnimation(STAND_DOWN);
		}
	}
	else
	{
		switch (currentDirection) {
			case LEFT:
				sprite->changeAnimation(STAND_LEFT);
				break;
			case RIGHT:
				sprite->changeAnimation(STAND_RIGHT);
				break;
			case DOWN:
				sprite->changeAnimation(STAND_DOWN);
				break;
			case UP:
				sprite->changeAnimation(STAND_UP);
				break;
		}
	}
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));
}

void Enemy::render(const glm::mat4& view)
{
	sprite->render(view);
}

void Enemy::setTileMap(TileMap* tileMap)
{
	map = tileMap;
}

void Enemy::setPosition(const glm::vec2& pos)
{
	posEnemy = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));
}

void Enemy::setSpriteSheet(const string& spriteSheet)
{
	this->spriteSheet = spriteSheet;
	spritesheet.loadFromFile(this->spriteSheet, TEXTURE_PIXEL_FORMAT_RGBA);
}

void Enemy::initializeAnimations() 
{
	//DEFAULT ANIMATIONS. TO BE OVERRIDEN BY CHILD CLASSES
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

	sprite->changeAnimation(STAND_DOWN);
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));
}

void Enemy::initializeSprite(ShaderProgram& shaderProgram) {
	const glm::vec2 QUAD_SIZE = glm::vec2(16.f, 16.f);
	spritesheet.loadFromFile(this->spriteSheet, TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(QUAD_SIZE, glm::vec2(0.25f, 0.25f), &spritesheet, &shaderProgram);
}

bool Enemy::checkPlayerVisibility(const glm::vec2 playerPos)
{
	float distanceToPlayer = glm::distance(glm::vec2(posEnemy), playerPos);
	const float MAX_VISION_DISTANCE = 200.0f;

	bool playerVisible = false;
	if (distanceToPlayer <= MAX_VISION_DISTANCE) {
		glm::vec2 dirVec;
		switch (currentDirection) {
		case LEFT:  dirVec = glm::vec2(-1, 0); break;
		case RIGHT: dirVec = glm::vec2(1, 0); break;
		case UP:    dirVec = glm::vec2(0, -1); break;
		case DOWN:  dirVec = glm::vec2(0, 1); break;
		}
		glm::vec2 toPlayer = glm::normalize(playerPos - glm::vec2(posEnemy));
		float dot = glm::dot(dirVec, toPlayer);
		if (dot > 0.85f) { 
			playerVisible = map->hasLineOfSight(glm::vec2(posEnemy), playerPos);
		}
	}

	return playerVisible;
}

// Path following logic using the Pathfinder class
void Enemy::followPath(int deltaTime)
{
	if (currentPathIndex >= static_cast<int>(currentPath.size())) return;
	glm::vec2 enemyPos(posEnemy);

	//Skip close nodes
	while (currentPathIndex < static_cast<int>(currentPath.size())) {
		glm::vec2 targetPos = tileToWorld(currentPath[currentPathIndex]);
		float distance = glm::length(targetPos - enemyPos);

		if (distance < 4.0f) {
			++currentPathIndex;
			continue;
		}
		break;
	}

	if (currentPathIndex < static_cast<int>(currentPath.size())) {
		glm::vec2 targetPos = tileToWorld(currentPath[currentPathIndex]);
		glm::vec2 direction = targetPos - enemyPos;
		float distance = glm::length(direction);

		if (distance > 0.1f) { // MIN_MOVE_DISTANCE
			direction = glm::normalize(direction);
			float moveAmount = moveSpeed * cachedTileSize * (deltaTime / 1000.0f);
			posEnemy += direction * moveAmount;

			changeAnimationsForDirection(direction);
		}
	}
}

void Enemy::changeAnimationsForDirection(glm::vec2 direction)
{
	cout << "Base Enemy changeAnimationsForDirection called. This should be overridden." << endl;
}

// Common helpers
int Enemy::findClosestPathIndex(const std::vector<glm::ivec2>& path, const glm::vec2& worldPos) const
{
	if (path.empty()) return 0;
	const int startIdx = (currentPathIndex < static_cast<int>(path.size())) ? currentPathIndex : 0;
	float bestDist = std::numeric_limits<float>::max();
	int bestIdx = startIdx;

	for (int i = startIdx; i < static_cast<int>(path.size()); ++i) {
		float d = glm::length(glm::vec2(path[i].x * cachedTileSize, path[i].y * cachedTileSize) - worldPos);
		if (d < bestDist) {
			bestDist = d;
			bestIdx = i;
		}
	}

	if (bestIdx == startIdx && bestDist < cachedTileSize * 0.35f && bestIdx + 1 < static_cast<int>(path.size())) {
		return bestIdx + 1;
	}

	return bestIdx;
}

bool Enemy::followPathToTarget(int deltaTime, const glm::ivec2& targetTile)
{
	if (currentPath.empty() || currentPathIndex >= static_cast<int>(currentPath.size())) {
		return true; // Path completado
	}

	followPath(deltaTime);

	glm::vec2 targetPos = tileToWorld(targetTile);
	float distToTarget = glm::length(targetPos - posEnemy);

	if (distToTarget < cachedTileSize * 1.5f) {
		currentPath.clear();
		currentPathIndex = 0;
		return true;
	}

	return false;
}

int Enemy::findClosestNodeInPath(const std::vector<glm::ivec2>& path, const glm::vec2& worldPos, int startIdx) const
{
	if (path.empty()) return 0;

	float bestDist = std::numeric_limits<float>::max();
	int bestIdx = startIdx;

	for (int i = startIdx; i < static_cast<int>(path.size()); ++i) {
		float d = glm::length(glm::vec2(path[i].x * cachedTileSize, path[i].y * cachedTileSize) - worldPos);
		if (d < bestDist) {
			bestDist = d;
			bestIdx = i;
		}
	}

	if (bestIdx == startIdx && bestDist < cachedTileSize * 0.35f && bestIdx + 1 < static_cast<int>(path.size())) {
		return bestIdx + 1;
	}

	return bestIdx;
}