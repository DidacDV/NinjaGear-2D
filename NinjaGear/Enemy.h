#pragma once

#include "Sprite.h"
#include "TileMap.h"
#include "Game.h"
#include "Pathfinder.h"
#include "ProjectileManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <string>

enum Direction { LEFT, RIGHT, UP, DOWN };

enum class EnemyType {
	BASE,
	MELEE,
	RANGED,
	TANK,
	BOSS
};

struct EnemyConfig {
	int xTile;
	int yTile;
	string spriteSheet;
	EnemyType type;
	int patrolDistance = 5;
};

class Enemy
{
public:
	virtual ~Enemy() = default;
	void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, TileMap* tileMap, const string& spritesheet);
	virtual void update(int deltaTime);
	virtual void render(const glm::mat4& view = glm::mat4(1.0f));

	void setTileMap(TileMap* tileMap);
	void setPosition(const glm::vec2& pos);
	void setSpriteSheet(const string& spriteSheet);
	glm::vec2 getPosition() const { return posEnemy; };
	void setProjectileManager(ProjectileManager* pm) { projectileManager = pm; };

	virtual void takeDamage(int damage);
	virtual int getHealth() const { return health; }
	virtual bool isAlive() const { return health > 0; }
	virtual int getDamage() const { return attackDamage; }
	virtual glm::vec2 getEnemySize() const { return enemySize; }

	virtual bool isInAttackState() const { return false; }
	virtual bool canDealDamage() const { return false; }
	virtual void onDamageDealt();
	virtual bool isBoss() const { return false; }
	void setCameraOffset(const glm::vec2& offset) { cameraOffset = offset; }

	void setPatrolDistance(int distance) {
		patrolDistance = distance;
	}
protected:
	// Child class methods
	virtual void initializeAnimations() = 0;
	virtual void updateStateMachine(int deltaTime) = 0;
	virtual void changeAnimationsForDirection(glm::vec2 direction) = 0;
    glm::vec2 cameraOffset = glm::vec2(0.0f, 0.0f);
	// Attributes
	int health = 8;
	int attackDamage = 1;
	glm::vec2 enemySize = glm::vec2(16.f, 16.f);
	glm::ivec2 tileMapDispl;
	glm::vec2 posEnemy; 
	Direction currentDirection;
	int patrolDistance;
	// Visuals
	Texture spritesheet;
	Sprite* sprite;
	glm::vec2 sizeInSpritesheet = glm::vec2(0.25f, 0.25f);
	virtual void onDamageReceived() = 0;

	// Tilemap
	TileMap* map;
	bool checkPlayerVisibility(const glm::vec2 playerPos);
	float cachedTileSize;

	// Attacking 
	int attackCooldownTimer = 0;
	int attackCooldown = 1000;
	float attackSpeed = 150.0f;
	ProjectileManager* projectileManager = nullptr;
	
	// Path following
	std::vector<glm::ivec2> currentPath;
	int currentPathIndex = 0;
	int pathUpdateTimer = 0;
	float moveSpeed = 5.0f; // tiles per second
	glm::ivec2 lastTargetTile;
	void followPath(int deltaTime);

	// Helpers
	int findClosestPathIndex(const std::vector<glm::ivec2>& path, const glm::vec2& worldPos) const;
	bool followPathToTarget(int deltaTime, const glm::ivec2& targetTile);
	int findClosestNodeInPath(const std::vector<glm::ivec2>& path, const glm::vec2& worldPos, int startIdx = 0) const;

	//Inline helpers
	inline glm::ivec2 getEnemyTile() const {
		return Pathfinder::instance().worldToTile(glm::vec2(posEnemy), map->getTileSize());
	}

	inline glm::ivec2 getPlayerTile(const glm::vec2& playerPos) const {
		return Pathfinder::instance().worldToTile(playerPos, map->getTileSize());
	}

	inline glm::vec2 tileToWorld(const glm::ivec2& tile) const {
		return glm::vec2(tile.x * cachedTileSize, tile.y * cachedTileSize);
	}
private:
	void initializeSprite(ShaderProgram& shaderProgram, const string& spritesheetPath);
};

