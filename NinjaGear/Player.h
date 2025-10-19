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
	enum PlayerAnims
	{
		STAND_LEFT, STAND_RIGHT, STAND_UP, STAND_DOWN,
		MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN,
		PUNCH_LEFT, PUNCH_RIGHT, PUNCH_UP, PUNCH_DOWN,
		DANCE
	};
	virtual void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram);
	virtual void update(int deltaTime);
	virtual void render(const glm::mat4& view = glm::mat4(1.0f));
	
	void setTileMaps(const vector<TileMap*>& tileMaps);
	void setPosition(const glm::vec2 &pos);
	void setSpriteSheet(const string& spriteSheet);
	glm::ivec2 getPosition() { return posPlayer; }
	glm::vec2 getPositionFloat() { return glm::vec2(posPlayer); }

	int getHealth() const { return health; }
	int getMaxHealth() const { return maxHealth; }
	bool isInvulnerable() const { return invulnerable; }
	void takeDamage(int damage);
	void heal(int amount);
	bool isAlive() const { return health > 0; }

	// Punching
	bool isPunching() const;
	bool justStartedPunching() const;
	int getFacingDirection() const; 
	glm::vec2 getPunchHitbox() const;

private:
	static const int INVULNERABILITY_DURATION = 1000;
	static const int DEFAULT_MAX_HEALTH = 100;

	int currentFacingDirection = STAND_DOWN; // Track last direction
	int lastFrameAnimation = -1;


	// Health management
	int health = 100;
	int maxHealth = 100;
	bool invulnerable = false;
	int invulnerabilityTimer = 0;

	// Collision detectors
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


