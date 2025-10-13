#pragma once

#include "Sprite.h"
#include "TileMap.h"
#include "Game.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <string>

enum Direction { LEFT, RIGHT, UP, DOWN };

class Enemy
{
public:
	virtual ~Enemy() = default;
	void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram);
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
private:
	void initializeSprite(ShaderProgram& shaderProgram);
};

