#pragma once

#include "Sprite.h"
#include "TileMap.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <string>

class Enemy
{
public:
	void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram);
	void update(int deltaTime);
	void render(const glm::mat4& view = glm::mat4(1.0f));

	void setTileMap(TileMap* tileMap);
	void setPosition(const glm::vec2& pos);
	void setSpriteSheet(const string& spriteSheet);
	glm::ivec2 getPosition() { return posEnemy; }

private:
	bool bJumping;
	glm::ivec2 tileMapDispl, posEnemy;
	int jumpAngle, startY;
	Texture spritesheet;
	Sprite* sprite;
	TileMap* map;
	string spriteSheet;
	bool checkPlayerVisibility(const glm::vec2 playerPos);
};

