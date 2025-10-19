#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE


#include "Sprite.h"
#include "TileMap.h"
#include <glm/gtc/matrix_transform.hpp>
#include <string>

// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.


class Player
{

public:
	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram);
	void update(int deltaTime);
	void render(const glm::mat4& view = glm::mat4(1.0f));
	
	void setTileMaps(const vector<TileMap*>& tileMaps);
	void setPosition(const glm::vec2 &pos);
	void setSpriteSheet(const string& spriteSheet);
	glm::ivec2 getPosition() { return posPlayer; }

private:
	bool collisionMoveLeft(const glm::ivec2& pos, const glm::ivec2& size) const;
	bool collisionMoveRight(const glm::ivec2& pos, const glm::ivec2& size) const;
	bool collisionMoveDown(const glm::ivec2& pos, const glm::ivec2& size) const;
	bool collisionMoveUp(const glm::ivec2& pos, const glm::ivec2& size) const;

	bool bJumping;
	glm::ivec2 tileMapDispl, posPlayer;
	int jumpAngle, startY;
	Texture spritesheet;
	Sprite *sprite;
	vector<TileMap*> maps;
	string spriteSheet;
};


#endif // _PLAYER_INCLUDE


