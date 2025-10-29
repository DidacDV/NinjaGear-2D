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

void Enemy::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, TileMap* tileMap, const string& spritesheet)
{
	//Template that every enemy will follow. 
	initializeSprite(shaderProgram, spritesheet);
	map = tileMap;
	cachedTileSize = static_cast<float>(map->getTileSize());
	tileMapDispl = tileMapPos;
	currentDirection = DOWN;
	initializeAnimations();
}

void Enemy::update(int deltaTime)
{
	sprite->update(deltaTime);
	if (attackCooldownTimer > 0) attackCooldownTimer -= deltaTime;
	updateStateMachine(deltaTime);
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
	spritesheet.loadFromFile(spriteSheet, TEXTURE_PIXEL_FORMAT_RGBA);
}

void Enemy::initializeSprite(ShaderProgram& shaderProgram, const string& spritesheetPath) {
	spritesheet.loadFromFile(spritesheetPath, TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(enemySize, sizeInSpritesheet, &spritesheet, &shaderProgram);
}

bool Enemy::checkPlayerVisibility(const glm::vec2 playerPos)
{
	float distanceToPlayer = glm::distance(glm::vec2(posEnemy), playerPos);
	const float MAX_VISION_DISTANCE = 150.f;

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
			playerVisible = TileMap::hasLineOfSight(glm::vec2(posEnemy), playerPos, map);
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

// Combat management

void Enemy::takeDamage(int damage)
{
	
	health -= damage;
	if (health < 0) {
		health = 0;
	}

	//TODO visuals
	onDamageReceived();
}

// Common attacking behaviour
void Enemy::onDamageDealt()
{
	attackCooldownTimer = attackCooldown;
}

