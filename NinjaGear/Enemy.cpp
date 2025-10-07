#include "Enemy.h"
#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include "Game.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 96
#define FALL_STEP 4
#define SPRITE_SIZE 16

enum EnemyAnims
{
	STAND_LEFT, STAND_RIGHT, STAND_UP, STAND_DOWN,
	MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN,
};

enum Direction { LEFT, RIGHT, UP, DOWN };
Direction currentDirection = DOWN;


void Enemy::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram)
{
	const float FRAME_WIDTH = 1.0f / 4.0f;
	const float FRAME_HEIGHT = 1.0f / 4.0f;
	const glm::vec2 QUAD_SIZE = glm::vec2(16.f, 16.f);
	bJumping = false;
	spritesheet.loadFromFile(this->spriteSheet, TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(QUAD_SIZE, glm::vec2(0.25f, 0.25f), &spritesheet, &shaderProgram);
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

	sprite->changeAnimation(STAND_DOWN); // Cambia a una animación que tenga keyframes válidos
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));

}

void Enemy::update(int deltaTime)
{
	sprite->update(deltaTime);

	glm::vec2 playerPos = Game::instance().getPlayerPosition();

	if (checkPlayerVisibility(playerPos)) cout << "I see you!" << endl;
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
		// Cambia la animación de pie según la dirección actual
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
		if (dot > 0.99f) { 
			playerVisible = map->hasLineOfSight(glm::vec2(posEnemy), playerPos);
		}
	}

	return playerVisible;
}


