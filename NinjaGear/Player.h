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
	virtual void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram);
	virtual void update(int deltaTime);
	virtual void render(const glm::mat4& view = glm::mat4(1.0f));
	
	void setTileMap(TileMap *tileMap);
	void setPosition(const glm::vec2 &pos);
	void setSpriteSheet(const string& spriteSheet);
	glm::ivec2 getPosition() { return posPlayer; }
	glm::vec2 getPositionFloat() { return glm::vec2(posPlayer); }

private:
	bool bJumping;
	glm::ivec2 tileMapDispl, posPlayer;
	int jumpAngle, startY;
	Texture spritesheet;
	Sprite *sprite;
	TileMap *map;
	string spriteSheet;
};


#endif // _PLAYER_INCLUDE


