#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include "Player.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 96
#define FALL_STEP 4
#define SPRITE_SIZE 16

enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, STAND_UP, STAND_DOWN,
	MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN,
	PUNCH_LEFT, PUNCH_RIGHT, PUNCH_UP, PUNCH_DOWN,
	DANCE
};


void Player::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram)
{
	health = 3.0f;
	maxHealth = 5.0f;

	itemQuantities.clear();
	itemInventory.clear();
	weaponInventory.clear();

	initDefaultWeapon();
	const float FRAME_WIDTH = 1.0f / 4.0f;
	const float FRAME_HEIGHT = 1.0f / 7.0f;
	const glm::vec2 QUAD_SIZE = glm::vec2(16.f, 16.f);
	const glm::vec2 FRAME_NORMALIZED_SIZE = glm::vec2(0.25, 0.1428);

	bJumping = false;
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
	sprite->update(deltaTime);
	//for debounce
	static bool cKeyPressed = false;
	static bool xKeyPressed = false;
	static bool vKeyPressed = false;

	if (Game::instance().getKey(GLFW_KEY_C)) {
		if (!cKeyPressed) {
			cycleItem();
			cKeyPressed = true;
		}
	}
	else {
		cKeyPressed = false;
	}
	if (Game::instance().getKey(GLFW_KEY_X)) {
		if (!xKeyPressed) {
			useCurrentItem();
			xKeyPressed = true;
		}
	}
	else {
		xKeyPressed = false;
	}
	if (Game::instance().getKey(GLFW_KEY_V)) {
		if (!vKeyPressed) {
			cycleWeapon();
			vKeyPressed = true;
		}
	}
	else {
		vKeyPressed = false;
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
		if (sprite->animation() == MOVE_LEFT || sprite->animation() == STAND_LEFT)
			sprite->changeAnimation(PUNCH_LEFT);
		else if (sprite->animation() == MOVE_RIGHT || sprite->animation() == STAND_RIGHT)
			sprite->changeAnimation(PUNCH_RIGHT);
		else if (sprite->animation() == MOVE_UP || sprite->animation() == STAND_UP)
			sprite->changeAnimation(PUNCH_UP);
		else if (sprite->animation() == MOVE_DOWN || sprite->animation() == STAND_DOWN)
			sprite->changeAnimation(PUNCH_DOWN);
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

void Player::heal(float amount) {
	health += amount;
	if (health > maxHealth) health = maxHealth;
}

//ITEMS & WEAPONS MANAGEMENT
void Player::initDefaultWeapon() {
	//doesn't need a texture since it's never rendered in the map
	Item* punchWeapon = new Item();
	punchWeapon->setItem("PUNCH", 1, "Your fists", glm::vec2(0, 0), true, 16);

	weaponInventory.push_back(punchWeapon);
	currentWeaponIndex = 0;
	equippedWeapon = "PUNCH";

	cout << "Initialized default weapon: PUNCH" << endl;
}


void Player::addItem(Item* item) {
	if (item == nullptr) return;
	if (item->getIsWeapon()) {
		weaponInventory.push_back(item);
		//auto-switch to newly picked weapon
		currentWeaponIndex = weaponInventory.size() - 1;
		equippedWeapon = item->getName();

		cout << "Added WEAPON " << item->getName() << " to weapon inventory" << endl;
		cout << "Weapon inventory size: " << weaponInventory.size() << endl;
		cout << "Switched to: " << equippedWeapon << endl;
	}
	else {
		string name = item->getName();
		itemQuantities[name]++;
		if (itemQuantities[name] == 1) {
			itemInventory.push_back(item);
			if (itemInventory.size() == 1) {
				currentItemIndex = 0;
			}
		}
	}
}

int Player::getItemQuantity(const std::string& itemName) const {
	auto it = itemQuantities.find(itemName);
	if (it != itemQuantities.end()) return it->second;
	return 0;
}

void Player::cycleItem() {
	if (itemInventory.empty()) return;

	currentItemIndex++;
	if (currentItemIndex >= itemInventory.size()) {
		currentItemIndex = 0;  //loop back to first item
	}
}

void Player::cycleWeapon() {
	if (weaponInventory.empty()) return;

	currentWeaponIndex++;
	if (currentWeaponIndex >= weaponInventory.size()) {
		currentWeaponIndex = 0;  //loop back to first item
	}
}

Item* Player::getCurrentWeapon() const {
	if (weaponInventory.empty()) return nullptr;
	return weaponInventory[currentWeaponIndex];
}

Item* Player::getCurrentItem() const {
	if (itemInventory.empty()) return nullptr;
	return itemInventory[currentItemIndex];
}

void Player::useCurrentItem() {
	if (itemInventory.empty()) {
		cout << "No items to use!" << endl;
		return;
	}

	Item* item = itemInventory[currentItemIndex];
	string itemName = item->getName();

	if (itemName == "MEDIPACK") {
		if (health == maxHealth)
			return;
		heal(1.f);
		cout << "Used MEDIPACK! Health: " << health << "/" << maxHealth << endl;
		itemQuantities[itemName]--;

		if (itemQuantities[itemName] <= 0) {
			itemQuantities.erase(itemName);
			delete item;
			itemInventory.erase(itemInventory.begin() + currentItemIndex);
		}

		if (currentItemIndex >= itemInventory.size() && !itemInventory.empty()) {
			currentItemIndex = 0;
		}
	}
	else {
		cout << "Cannot use item: " << itemName << endl;
	}
}