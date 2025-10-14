#pragma once

#include "Sprite.h"
#include "TileMap.h"
#include "Game.h"
#include "Pathfinder.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <string>

enum Direction { LEFT, RIGHT, UP, DOWN };

class Enemy
{
public:
	virtual ~Enemy() = default;
	virtual void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, TileMap* tileMap);
	virtual void update(int deltaTime);
	virtual void render(const glm::mat4& view = glm::mat4(1.0f));

	void setTileMap(TileMap* tileMap);
	void setPosition(const glm::vec2& pos);
	void setSpriteSheet(const string& spriteSheet);
	glm::vec2 getPosition() { return posEnemy; }

protected:
	virtual void initializeAnimations();
	bool bJumping;
	glm::ivec2 tileMapDispl;
	glm::vec2 posEnemy; 
	int jumpAngle, startY;
	Texture spritesheet;
	Sprite* sprite;
	TileMap* map;
	string spriteSheet;
	Direction currentDirection;
	bool checkPlayerVisibility(const glm::vec2 playerPos);
	float cachedTileSize;

	// Path following
	std::vector<glm::ivec2> currentPath;
	int currentPathIndex = 0;
	int pathUpdateTimer = 0;
	float moveSpeed = 5.0f; // tiles per second
	float moveProgress = 0.0f;
	glm::ivec2 lastTargetTile;
	void followPath(int deltaTime);
	virtual void changeAnimationsForDirection(glm::vec2 direction);

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
	void initializeSprite(ShaderProgram& shaderProgram);
};

