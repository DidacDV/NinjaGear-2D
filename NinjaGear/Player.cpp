#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include "Player.h"
#include "Game.h"
#include "ServiceLocator.h"

#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 96
#define FALL_STEP 4
#define SPRITE_SIZE 16

void Player::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram)
{
	const float FRAME_WIDTH = 1.0f / 4.0f;
	const float FRAME_HEIGHT = 1.0f / 7.0f;
	const glm::vec2 QUAD_SIZE = glm::vec2(16.f, 16.f);

	spritesheet.loadFromFile(this->spriteSheet, TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(QUAD_SIZE, glm::vec2(0.25f, 0.142857f), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(13);
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
		// DANCE
		sprite->setAnimationSpeed(DANCE, 6);
		sprite->addKeyframe(DANCE, glm::vec2(0.0f * FRAME_WIDTH, 6.0f * FRAME_HEIGHT));
		sprite->addKeyframe(DANCE, glm::vec2(1.0f * FRAME_WIDTH, 6.0f * FRAME_HEIGHT));
		sprite->addKeyframe(DANCE, glm::vec2(2.0f * FRAME_WIDTH, 5.0f * FRAME_HEIGHT));
		sprite->addKeyframe(DANCE, glm::vec2(2.0f * FRAME_WIDTH, 6.0f * FRAME_HEIGHT));
		sprite->addKeyframe(DANCE, glm::vec2(3.0f * FRAME_WIDTH, 5.0f * FRAME_HEIGHT));
		sprite->addKeyframe(DANCE, glm::vec2(3.0f * FRAME_WIDTH, 6.0f * FRAME_HEIGHT));


		// PUNCHING
		sprite->setAnimationSpeed(PUNCH_LEFT, 6);
		sprite->addKeyframe(PUNCH_LEFT, glm::vec2(2.0f * FRAME_WIDTH, 4.0f * FRAME_HEIGHT)); // PUNCH
		sprite->addKeyframe(PUNCH_LEFT, glm::vec2(2.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT)); // STAND

		sprite->setAnimationSpeed(PUNCH_RIGHT, 6);
		sprite->addKeyframe(PUNCH_RIGHT, glm::vec2(3.0f * FRAME_WIDTH, 4.0f * FRAME_HEIGHT));
		sprite->addKeyframe(PUNCH_RIGHT, glm::vec2(3.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));

		sprite->setAnimationSpeed(PUNCH_UP, 6);
		sprite->addKeyframe(PUNCH_UP, glm::vec2(1.0f * FRAME_WIDTH, 4.0f * FRAME_HEIGHT));
		sprite->addKeyframe(PUNCH_UP, glm::vec2(0.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));

		sprite->setAnimationSpeed(PUNCH_DOWN, 6);
		sprite->addKeyframe(PUNCH_DOWN, glm::vec2(0.0f * FRAME_WIDTH, 4.0f * FRAME_HEIGHT));
		sprite->addKeyframe(PUNCH_DOWN, glm::vec2(0.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));

		
	sprite->changeAnimation(STAND_DOWN); // Cambia a una animación que tenga keyframes válidos
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	
}

void Player::update(int deltaTime)
{
	lastFrameAnimation = sprite->animation();
	sprite->update(deltaTime);

	if (invulnerable) {
		invulnerabilityTimer -= deltaTime;
		if (invulnerabilityTimer <= 0) {
			invulnerable = false;
			invulnerabilityTimer = 0;
		}
	}
	
	if (Game::instance().getKey(GLFW_KEY_LEFT))
	{
		if (sprite->animation() != MOVE_LEFT)
			sprite->changeAnimation(MOVE_LEFT);
		posPlayer.x -= 2;
		if (collisionMoveLeft(posPlayer, glm::ivec2(SPRITE_SIZE, SPRITE_SIZE)))
		{
			posPlayer.x += 2;
			sprite->changeAnimation(STAND_LEFT);
		}
	}
	else if (Game::instance().getKey(GLFW_KEY_RIGHT))
	{
		if (sprite->animation() != MOVE_RIGHT)
			sprite->changeAnimation(MOVE_RIGHT);
		posPlayer.x += 2;
		if (collisionMoveRight(posPlayer, glm::ivec2(SPRITE_SIZE, SPRITE_SIZE)))
		{
			posPlayer.x -= 2;
			sprite->changeAnimation(STAND_RIGHT);
		}
	}
	else if (Game::instance().getKey(GLFW_KEY_DOWN))
	{
		if (sprite->animation() != MOVE_DOWN)
			sprite->changeAnimation(MOVE_DOWN);
		posPlayer.y += 2;
		if (collisionMoveDown(posPlayer, glm::ivec2(SPRITE_SIZE, SPRITE_SIZE)))
		{
			posPlayer.y -= 2;
			sprite->changeAnimation(STAND_DOWN);
		}
	}
	else if (Game::instance().getKey(GLFW_KEY_UP))
	{
		if (sprite->animation() != MOVE_UP)
			sprite->changeAnimation(MOVE_UP);
		posPlayer.y -= 2;
		if (collisionMoveUp(posPlayer, glm::ivec2(SPRITE_SIZE, SPRITE_SIZE)))
		{
			posPlayer.y += 2;
			sprite->changeAnimation(STAND_DOWN);
		}
	}
	else if (Game::instance().getKey(GLFW_KEY_Y))  // DANCE
	{
		if (sprite->animation() != DANCE)
			sprite->changeAnimation(DANCE);
	}
	else if (Game::instance().getKey(GLFW_KEY_G))  // PUNCH
	{
		if (sprite->animation() == MOVE_LEFT || sprite->animation() == STAND_LEFT) {
			sprite->changeAnimation(PUNCH_LEFT);
			currentFacingDirection = STAND_LEFT;
		}
		else if (sprite->animation() == MOVE_RIGHT || sprite->animation() == STAND_RIGHT) {
			sprite->changeAnimation(PUNCH_RIGHT);
			currentFacingDirection = STAND_RIGHT;
		}
		else if (sprite->animation() == MOVE_UP || sprite->animation() == STAND_UP) {
			sprite->changeAnimation(PUNCH_UP);
			currentFacingDirection = STAND_UP;
		}
		else if (sprite->animation() == MOVE_DOWN || sprite->animation() == STAND_DOWN) {
			sprite->changeAnimation(PUNCH_DOWN);
			currentFacingDirection = STAND_DOWN;
		}
	}
	else
	{
		if(sprite->animation() == MOVE_LEFT || sprite->animation() == PUNCH_LEFT)
			sprite->changeAnimation(STAND_LEFT);
		else if(sprite->animation() == MOVE_RIGHT || sprite->animation() == PUNCH_RIGHT)
			sprite->changeAnimation(STAND_RIGHT);
		else if(sprite->animation() == MOVE_DOWN || sprite->animation() == PUNCH_DOWN)
			sprite->changeAnimation(STAND_DOWN);
		else if(sprite->animation() == MOVE_UP || sprite->animation() == PUNCH_UP)
			sprite->changeAnimation(STAND_UP);
		else if(sprite->animation() == DANCE)
			sprite->changeAnimation(STAND_DOWN);
	}
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

void Player::render(const glm::mat4& view)
{
	sprite->render(view);
}

void Player::setTileMaps(const vector<TileMap*>& tileMaps)
{
	maps = tileMaps;
}

void Player::setPosition(const glm::vec2 &pos)
{
	posPlayer = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

void Player::setSpriteSheet(const string& spriteSheet)
{
	this->spriteSheet = spriteSheet;
	spritesheet.loadFromFile(this->spriteSheet, TEXTURE_PIXEL_FORMAT_RGBA);
}

bool Player::collisionMoveLeft(const glm::ivec2& pos, const glm::ivec2& size) const
{
	for (TileMap* map : maps) {
		if (map->collisionMoveLeft(pos, size))
			return true;
	}
	return false;
}

bool Player::collisionMoveRight(const glm::ivec2& pos, const glm::ivec2& size) const
{
	for (TileMap* map : maps) {
		if (map->collisionMoveRight(pos, size))
			return true;
	}
	return false;
}

bool Player::collisionMoveDown(const glm::ivec2& pos, const glm::ivec2& size) const
{
	for (TileMap* map : maps) {
		if (map->collisionMoveDown(pos, size))
			return true;
	}
	return false;
}

bool Player::collisionMoveUp(const glm::ivec2& pos, const glm::ivec2& size) const
{
	for (TileMap* map : maps) {
		if (map->collisionMoveUp(pos, size))
			return true;
	}
	return false;
}

// Health management

void Player::takeDamage(int damage)
{
	if (invulnerable || health <= 0) {
		return;
	}

	health -= damage;
	if (health < 0) {
		health = 0;
	}

	invulnerable = true;
	invulnerabilityTimer = 1000; 

	//TODO visuals

}

void Player::heal(int amount)
{
	// Can't heal if dead
	if (health <= 0) {
		return;
	}

	health += amount;

	// Clamp to max health
	if (health > maxHealth) {
		health = maxHealth;
	}

	// Optional: Visual/audio feedback
	// - Play heal sound
	// - Show heal particle effect
}

// Punching

bool Player::isPunching() const
{
	int anim = sprite->animation();
	return (anim >= PUNCH_LEFT && anim <= PUNCH_DOWN);
}

bool Player::justStartedPunching() const
{
	int currentAnim = sprite->animation();
	bool isPunchAnim = (currentAnim >= PUNCH_LEFT && currentAnim <= PUNCH_DOWN);
	bool wasPunchAnim = (lastFrameAnimation >= PUNCH_LEFT && lastFrameAnimation <= PUNCH_DOWN);

	return isPunchAnim && !wasPunchAnim;
}

int Player::getFacingDirection() const
{
	return currentFacingDirection;
}

glm::vec2 Player::getPunchHitbox() const
{
	const float PUNCH_REACH = 20.0f;
	glm::vec2 hitbox = glm::vec2(posPlayer);


	switch (currentFacingDirection) {
	case STAND_LEFT:
		hitbox.x -= PUNCH_REACH; 
		break;
	case STAND_RIGHT:
		break;
	case STAND_UP:
		hitbox.y -= PUNCH_REACH; 
		break;
	case STAND_DOWN:
		break;
	}

	return hitbox;
}

void Player::increaseRank(const int& increase) {
	cout << "increased rank by" << increase << endl;
	rank += increase;
}

void Player::onPunchKeyPressed()
{
	ServiceLocator::getAudio().playSound("sounds/punch.wav");
}