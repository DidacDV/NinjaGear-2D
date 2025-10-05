#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include "Player.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 96
#define FALL_STEP 4


enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT
};


void Player::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram)
{
	const float FRAME_WIDTH = 1.0f / 4.0f;
	const float FRAME_HEIGHT = 1.0f / 7.0f;
	const glm::vec2 QUAD_SIZE = glm::vec2(16.f, 16.f);
	const glm::vec2 FRAME_NORMALIZED_SIZE = glm::vec2(0.25, 0.1428);


	bJumping = false;
	spritesheet.loadFromFile("images/characters/ninja_dark/SpriteSheet.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(QUAD_SIZE, glm::vec2(0.25f, 0.142857f), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(4);
	
		sprite->setAnimationSpeed(STAND_LEFT, 8);
		sprite->addKeyframe(STAND_LEFT, glm::vec2(2.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));


		sprite->setAnimationSpeed(STAND_RIGHT, 8);
		sprite->addKeyframe(STAND_RIGHT, glm::vec2(3.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));

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
		
	sprite->changeAnimation(0);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	
}

void Player::update(int deltaTime)
{
	sprite->update(deltaTime);
	if(Game::instance().getKey(GLFW_KEY_LEFT))
	{
		if(sprite->animation() != MOVE_LEFT)
			sprite->changeAnimation(MOVE_LEFT);
		/*posPlayer.x -= 2;
		if(map->collisionMoveLeft(posPlayer, glm::ivec2(32, 32)))
		{
			posPlayer.x += 2;
			sprite->changeAnimation(STAND_LEFT);
		}*/
	}
	else if(Game::instance().getKey(GLFW_KEY_RIGHT))
	{
		if(sprite->animation() != MOVE_RIGHT)
			sprite->changeAnimation(MOVE_RIGHT);
		/*posPlayer.x += 2;
		if(map->collisionMoveRight(posPlayer, glm::ivec2(32, 32)))
		{
			posPlayer.x -= 2;
			sprite->changeAnimation(STAND_RIGHT);
		}*/
	}
	else
	{
		if(sprite->animation() == MOVE_LEFT)
			sprite->changeAnimation(STAND_LEFT);
		else if(sprite->animation() == MOVE_RIGHT)
			sprite->changeAnimation(STAND_RIGHT);
	}
	
	if(bJumping)
	{
		jumpAngle += JUMP_ANGLE_STEP;
		if(jumpAngle == 180)
		{
			bJumping = false;
			posPlayer.y = startY;
		}
		else
		{
			posPlayer.y = int(startY - 96 * sin(3.14159f * jumpAngle / 180.f));
			if(jumpAngle > 90)
				bJumping = !map->collisionMoveDown(posPlayer, glm::ivec2(32, 32), &posPlayer.y);
		}
	}
	else
	{
		posPlayer.y += FALL_STEP;
		if(map->collisionMoveDown(posPlayer, glm::ivec2(32, 32), &posPlayer.y))
		{
			if(Game::instance().getKey(GLFW_KEY_UP))
			{
				bJumping = true;
				jumpAngle = 0;
				startY = posPlayer.y;
			}
		}
	}
	
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

void Player::render()
{
	sprite->render();
}

void Player::setTileMap(TileMap *tileMap)
{
	map = tileMap;
}

void Player::setPosition(const glm::vec2 &pos)
{
	posPlayer = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

void Player::setSpriteSheet(const string& spriteSheet)
{
	this->spriteSheet = spriteSheet;
}







