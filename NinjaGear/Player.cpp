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

enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, STAND_UP, STAND_DOWN,
	MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN,
	PUNCH_LEFT, PUNCH_RIGHT, PUNCH_UP, PUNCH_DOWN,
	DANCE,
	BOW_ATTACK_LEFT, BOW_ATTACK_RIGHT, BOW_ATTACK_UP, BOW_ATTACK_DOWN
};


void Player::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram)
{
	initialized = true;
	rank = 0;
	baseSpeed = 2.0f;
	godMode = false;
	health = 5.0f;
	maxHealth = 5.0f;
	useRedSprite = false;
	invulnerable = false;
	invulnerabilityTimer = 0;

	activeBuffs.clear();
	showAura = false;
	itemQuantities.clear();
	itemInventory.clear();
	weaponInventory.clear();

	initDefaultWeapon();
	const float FRAME_WIDTH = 1.0f / 4.0f;
	const float FRAME_HEIGHT = 1.0f / 7.0f;
	const glm::vec2 QUAD_SIZE = glm::vec2(16.f, 16.f);
  
	spritesheet.loadFromFile(this->spriteSheet, TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(QUAD_SIZE, glm::vec2(0.25f, 0.142857f), &spritesheet, &shaderProgram);

	redSpritesheet.loadFromFile("images/characters/ninja_damage/Spritesheet.png", TEXTURE_PIXEL_FORMAT_RGBA);
	redSprite = Sprite::createSprite(QUAD_SIZE, glm::vec2(0.25f, 0.142857f), &redSpritesheet, &shaderProgram);

	setUpPlayerAnimation(sprite);
	setUpPlayerAnimation(redSprite);

		
	sprite->changeAnimation(STAND_DOWN); 
	redSprite->changeAnimation(STAND_DOWN); 
	
	setUpBowSprite(shaderProgram);

	setUpAuraSprites(shaderProgram);
	
	
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	
}

void Player::setUpPlayerAnimation(Sprite* spr) {
	const float FRAME_WIDTH = 1.0f / 4.0f;
	const float FRAME_HEIGHT = 1.0f / 7.0f;
	const glm::vec2 QUAD_SIZE = glm::vec2(16.f, 16.f);
	spr->setNumberAnimations(13);
	// STANDING ANIMATIONS
	spr->setAnimationSpeed(STAND_DOWN, 8);
	spr->addKeyframe(STAND_DOWN, glm::vec2(0.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));

	spr->setAnimationSpeed(STAND_UP, 8);
	spr->addKeyframe(STAND_UP, glm::vec2(1.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));

	spr->setAnimationSpeed(STAND_LEFT, 8);
	spr->addKeyframe(STAND_LEFT, glm::vec2(2.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));


	spr->setAnimationSpeed(STAND_RIGHT, 8);
	spr->addKeyframe(STAND_RIGHT, glm::vec2(3.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));


	//MOVING ANIMATIONS
	spr->setAnimationSpeed(MOVE_DOWN, 8);
	spr->addKeyframe(MOVE_DOWN, glm::vec2(0.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));
	spr->addKeyframe(MOVE_DOWN, glm::vec2(0.0f * FRAME_WIDTH, 1.0f * FRAME_HEIGHT));
	spr->addKeyframe(MOVE_DOWN, glm::vec2(0.0f * FRAME_WIDTH, 2.0f * FRAME_HEIGHT));
	spr->addKeyframe(MOVE_DOWN, glm::vec2(0.0f * FRAME_WIDTH, 3.0f * FRAME_HEIGHT));

	spr->setAnimationSpeed(MOVE_UP, 8);
	spr->addKeyframe(MOVE_UP, glm::vec2(1.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));
	spr->addKeyframe(MOVE_UP, glm::vec2(1.0f * FRAME_WIDTH, 1.0f * FRAME_HEIGHT));
	spr->addKeyframe(MOVE_UP, glm::vec2(1.0f * FRAME_WIDTH, 2.0f * FRAME_HEIGHT));
	spr->addKeyframe(MOVE_UP, glm::vec2(1.0f * FRAME_WIDTH, 3.0f * FRAME_HEIGHT));


	spr->setAnimationSpeed(MOVE_LEFT, 8);
	spr->addKeyframe(MOVE_LEFT, glm::vec2(2.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));
	spr->addKeyframe(MOVE_LEFT, glm::vec2(2.0f * FRAME_WIDTH, 1.0f * FRAME_HEIGHT));
	spr->addKeyframe(MOVE_LEFT, glm::vec2(2.0f * FRAME_WIDTH, 2.0f * FRAME_HEIGHT));
	spr->addKeyframe(MOVE_LEFT, glm::vec2(2.0f * FRAME_WIDTH, 3.0f * FRAME_HEIGHT));



	spr->setAnimationSpeed(MOVE_RIGHT, 8);
	spr->addKeyframe(MOVE_RIGHT, glm::vec2(3.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));
	spr->addKeyframe(MOVE_RIGHT, glm::vec2(3.0f * FRAME_WIDTH, 1.0f * FRAME_HEIGHT));
	spr->addKeyframe(MOVE_RIGHT, glm::vec2(3.0f * FRAME_WIDTH, 2.0f * FRAME_HEIGHT));
	spr->addKeyframe(MOVE_RIGHT, glm::vec2(3.0f * FRAME_WIDTH, 3.0f * FRAME_HEIGHT));
	// DANCE
	spr->setAnimationSpeed(DANCE, 6);
	spr->addKeyframe(DANCE, glm::vec2(0.0f * FRAME_WIDTH, 6.0f * FRAME_HEIGHT));
	spr->addKeyframe(DANCE, glm::vec2(1.0f * FRAME_WIDTH, 6.0f * FRAME_HEIGHT));
	spr->addKeyframe(DANCE, glm::vec2(2.0f * FRAME_WIDTH, 5.0f * FRAME_HEIGHT));
	spr->addKeyframe(DANCE, glm::vec2(2.0f * FRAME_WIDTH, 6.0f * FRAME_HEIGHT));
	spr->addKeyframe(DANCE, glm::vec2(3.0f * FRAME_WIDTH, 5.0f * FRAME_HEIGHT));
	spr->addKeyframe(DANCE, glm::vec2(3.0f * FRAME_WIDTH, 6.0f * FRAME_HEIGHT));


	// PUNCHING
	spr->setAnimationSpeed(PUNCH_LEFT, 6);
	spr->addKeyframe(PUNCH_LEFT, glm::vec2(2.0f * FRAME_WIDTH, 4.0f * FRAME_HEIGHT)); // PUNCH
	spr->addKeyframe(PUNCH_LEFT, glm::vec2(2.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT)); // STAND

	spr->setAnimationSpeed(PUNCH_RIGHT, 6);
	spr->addKeyframe(PUNCH_RIGHT, glm::vec2(3.0f * FRAME_WIDTH, 4.0f * FRAME_HEIGHT));
	spr->addKeyframe(PUNCH_RIGHT, glm::vec2(3.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));

	spr->setAnimationSpeed(PUNCH_UP, 6);
	spr->addKeyframe(PUNCH_UP, glm::vec2(1.0f * FRAME_WIDTH, 4.0f * FRAME_HEIGHT));
	spr->addKeyframe(PUNCH_UP, glm::vec2(0.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));

	spr->setAnimationSpeed(PUNCH_DOWN, 6);
	spr->addKeyframe(PUNCH_DOWN, glm::vec2(0.0f * FRAME_WIDTH, 4.0f * FRAME_HEIGHT));
	spr->addKeyframe(PUNCH_DOWN, glm::vec2(0.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));
}

void Player::setUpBowSprite(ShaderProgram& shaderProgram) {
	bowSpritesheet.loadFromFile("images/characters/ninja_dark/PlayerBowSpritesheet.png", TEXTURE_PIXEL_FORMAT_RGBA);
	bowSprite = Sprite::createSprite(glm::vec2(16.f, 22.f), glm::vec2(0.25f, 1.0f), &bowSpritesheet, &shaderProgram);
	bowSprite->setNumberAnimations(4);  // Only 4 attack directions

	bowSprite->setAnimationSpeed(0, 6); // DOWN
	bowSprite->addKeyframe(0, glm::vec2(0.0f * 0.25f, 0.0f));

	bowSprite->setAnimationSpeed(1, 6); // UP
	bowSprite->addKeyframe(1, glm::vec2(1.0f * 0.25f, 0.0f));

	bowSprite->setAnimationSpeed(2, 6); // LEFT
	bowSprite->addKeyframe(2, glm::vec2(2.0f * 0.25f, 0.0f));

	bowSprite->setAnimationSpeed(3, 6); // RIGHT
	bowSprite->addKeyframe(3, glm::vec2(3.0f * 0.25f, 0.0f));

	bowSprite->changeAnimation(0);
}

void Player::setUpAuraSprites(ShaderProgram& shaderProgram) {
	if (!auraTexture.loadFromFile("images/aura/LightingAura.png", TEXTURE_PIXEL_FORMAT_RGBA)) {
		std::cout << "Failed to load aura texture!" << std::endl;
	}

	auraSprite = Sprite::createSprite(glm::ivec2(20, 28), glm::vec2(1.0f / 8.0f, 1.0f), &auraTexture, &shaderProgram);
	auraSprite->setNumberAnimations(1);
	auraSprite->setAnimationSpeed(0, 12);
	for (int i = 0; i < 8; ++i)
		auraSprite->addKeyframe(0, glm::vec2(i / 8.0f, 0.f));

	auraSprite->changeAnimation(0);
	auraSprite->setPosition(glm::vec2(posPlayer.x - 3, posPlayer.y - 5));
}

void Player::update(int deltaTime)
{
	lastFrameAnimation = sprite->animation();
	sprite->update(deltaTime);

	if (redSprite && redSprite->animation() != sprite->animation()) {
		redSprite->changeAnimation(sprite->animation());
	}
	if (redSprite) {
		redSprite->update(deltaTime);
	}

	if (invulnerable) {
		invulnerabilityTimer -= deltaTime;
		static int flashTimer = 0;
		flashTimer += deltaTime;
		if (flashTimer >= 100) {
			useRedSprite = !useRedSprite;
			flashTimer = 0;
		}

		if (invulnerabilityTimer <= 0) {
			invulnerable = false;
			invulnerabilityTimer = 0;
			useRedSprite = false;
		}
}
	
	float moveSpeed = baseSpeed;

	checkBuffsState(deltaTime);
	auto it = activeBuffs.find(BuffType::SPEED);
	if (it != activeBuffs.end()) {
		moveSpeed *= it->second.multiplier;
	}

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
	if (Game::instance().getKey(GLFW_KEY_LEFT) || Game::instance().getKey(GLFW_KEY_A))
	{
		if (sprite->animation() != MOVE_LEFT)
			sprite->changeAnimation(MOVE_LEFT);
		posPlayer.x -= moveSpeed;
		if (collisionMoveLeft(posPlayer, glm::ivec2(SPRITE_SIZE, SPRITE_SIZE)))
		{
			posPlayer.x += moveSpeed;
			sprite->changeAnimation(STAND_LEFT);
		}
	}
	else if (Game::instance().getKey(GLFW_KEY_RIGHT) || Game::instance().getKey(GLFW_KEY_D))
	{
		if (sprite->animation() != MOVE_RIGHT)
			sprite->changeAnimation(MOVE_RIGHT);
		posPlayer.x += moveSpeed;
		if (collisionMoveRight(posPlayer, glm::ivec2(SPRITE_SIZE, SPRITE_SIZE)))
		{
			posPlayer.x -= moveSpeed;
			sprite->changeAnimation(STAND_RIGHT);
		}
	}
	else if (Game::instance().getKey(GLFW_KEY_DOWN) || Game::instance().getKey(GLFW_KEY_S))
	{
		if (sprite->animation() != MOVE_DOWN)
			sprite->changeAnimation(MOVE_DOWN);
		posPlayer.y += moveSpeed;
		if (collisionMoveDown(posPlayer, glm::ivec2(SPRITE_SIZE, SPRITE_SIZE)))
		{
			posPlayer.y -= moveSpeed;
			sprite->changeAnimation(STAND_DOWN);
		}
	}
	else if (Game::instance().getKey(GLFW_KEY_UP) || Game::instance().getKey(GLFW_KEY_W))
	{
		if (sprite->animation() != MOVE_UP)
			sprite->changeAnimation(MOVE_UP);
		posPlayer.y -= moveSpeed;
		if (collisionMoveUp(posPlayer, glm::ivec2(SPRITE_SIZE, SPRITE_SIZE)))
		{
			posPlayer.y += moveSpeed;
			sprite->changeAnimation(STAND_DOWN);
		}
	}
	else if (Game::instance().getKey(GLFW_KEY_Y))  // DANCE
	{
		if (sprite->animation() != DANCE)
			sprite->changeAnimation(DANCE);
	}
	else if (Game::instance().getKey(GLFW_KEY_Z))  // ATTACK
	{
		Item* weapon = getCurrentWeapon();
		bool usingBow = (weapon != nullptr && weapon->getName() == "BOW");

		if (usingBow) {
			if (!lastFrameWasAttackingWithBow) {
				shootBowProjectile();
			}
			lastFrameWasAttackingWithBow = true;
			if (sprite->animation() == MOVE_LEFT || sprite->animation() == STAND_LEFT)
				bowSprite->changeAnimation(2); // LEFT
			else if (sprite->animation() == MOVE_RIGHT || sprite->animation() == STAND_RIGHT)
				bowSprite->changeAnimation(3); // RIGHT
			else if (sprite->animation() == MOVE_UP || sprite->animation() == STAND_UP)
				bowSprite->changeAnimation(1); // UP
			else
				bowSprite->changeAnimation(0); // DOWN
		}
		else {
			lastFrameWasAttackingWithBow = false;
			// Use punch animations (original code)
			if (sprite->animation() == MOVE_LEFT || sprite->animation() == STAND_LEFT)
				sprite->changeAnimation(PUNCH_LEFT);
			else if (sprite->animation() == MOVE_RIGHT || sprite->animation() == STAND_RIGHT)
				sprite->changeAnimation(PUNCH_RIGHT);
			else if (sprite->animation() == MOVE_UP || sprite->animation() == STAND_UP)
				sprite->changeAnimation(PUNCH_UP);
			else if (sprite->animation() == MOVE_DOWN || sprite->animation() == STAND_DOWN)
				sprite->changeAnimation(PUNCH_DOWN);
		}
	}
	else
	{
		lastFrameWasAttackingWithBow = false;
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
	if (redSprite) {
		redSprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	}

	if (bowSprite) {
		bowSprite->update(deltaTime);
		bowSprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
	}

}

void Player::render(const glm::mat4& view)
{

	if (showAura) {
		auraSprite->render(view);
	}

	Item* weapon = getCurrentWeapon();
	bool isAttackingWithBow = (weapon != nullptr && weapon->getName() == "BOW" &&
		Game::instance().getKey(GLFW_KEY_Z) && !Game::instance().getKey(GLFW_KEY_DOWN) && !Game::instance().getKey(GLFW_KEY_UP)
		&& !Game::instance().getKey(GLFW_KEY_RIGHT) && !Game::instance().getKey(GLFW_KEY_LEFT));

	if (isAttackingWithBow) {
		bowSprite->render(view);
	}
	else {
		if (useRedSprite && redSprite) {
			redSprite->render(view);
		}
		else {
			sprite->render(view);
		}
	}
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
	if (isGodModeOn())
		return;
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
	useRedSprite = true;
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
	//ServiceLocator::getAudio().playSound("sounds/punch.wav");
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
		if (itemQuantities.find(name) == itemQuantities.end()) {
			itemInventory.push_back(item);
			if (itemInventory.size() == 1) {
				currentItemIndex = 0;
			}
		}
		itemQuantities[name] += item->getQuantity();
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
		consumeItem(item, itemName);
	}

	if (itemName == "SPEED POTION") {
		applyBuff(BuffType::SPEED, 3, 2.f);
		consumeItem(item, itemName);
	}

	else {
		cout << "Cannot use item: " << itemName << endl;
	}
}

void Player::consumeItem(Item* item, string& itemName) {
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

//buffs
void Player::applyBuff(BuffType type, float duration, float multiplier)
{
	activeBuffs[type] = { type, duration, duration, multiplier };
	std::cout << "Applied buff: " << (int)type << " for " << duration << "s" << std::endl;
}

void Player::checkBuffsState(int deltaTime) {
	float deltaSec = deltaTime / 1000.0f;

	for (auto it = activeBuffs.begin(); it != activeBuffs.end(); )
	{
		it->second.remainingTime -= deltaSec;
		if (it->second.remainingTime <= 0.0f)
		{
			std::cout << "Buff expired: " << (int)it->second.type << std::endl;
			it = activeBuffs.erase(it); //remove expired buff
		}
		else ++it;
	}

	if (activeBuffs.count(BuffType::SPEED)) {
		showAura = true;
		auraSprite->update(deltaTime);
		auraSprite->setPosition(glm::vec2(posPlayer.x - 3, posPlayer.y - 5));
	}
	else {
		showAura = false;
	}
}
//bow attacking
void Player::shootBowProjectile()
{
	if (!projectileManager) return;

	if (getItemQuantity("ARROW") <= 0) {
		cout << "No arrows!" << endl;
		return;
	}

	glm::vec2 direction(0.0f);

	//determine direction based on current animation
	if (sprite->animation() == MOVE_LEFT || sprite->animation() == STAND_LEFT) {
		direction = glm::vec2(-1, 0);
	}
	else if (sprite->animation() == MOVE_RIGHT || sprite->animation() == STAND_RIGHT) {
		direction = glm::vec2(1, 0);
	}
	else if (sprite->animation() == MOVE_UP || sprite->animation() == STAND_UP) {
		direction = glm::vec2(0, -1);
	}
	else {
		direction = glm::vec2(0, 1); // DOWN
	}

	float speed = 100.f;
	int damage = 3;

	//SPAWN PROJECTILE FIRST
	projectileManager->spawnProjectile(
		posPlayer,
		direction,
		speed,
		damage,
		"images/projectiles/Arrow.png", 
		true
	);

	//consume arrow
	string arrowName = "ARROW";
	itemQuantities[arrowName]--;

	if (itemQuantities[arrowName] <= 0) {
		itemQuantities.erase(arrowName);

		for (auto it = itemInventory.begin(); it != itemInventory.end(); ++it) {
			if ((*it)->getName() == arrowName) {
				delete* it;
				itemInventory.erase(it);

				if (currentItemIndex >= itemInventory.size() && !itemInventory.empty()) {
					currentItemIndex = 0;
				}
				break;
			}
		}
	}

	cout << "Shot arrow! Remaining: " << getItemQuantity("ARROW") << endl;
}

void Player::giveAllItems() {
	cout << "=== GIVING ALL ITEMS ===" << endl;

	for (auto& item : itemInventory) {
		delete item;
	}
	itemInventory.clear();
	itemQuantities.clear();

	//MEDIPACK
	Item* medipack = new Item();
	medipack->setItem("MEDIPACK", 1, "Restores 50 health points.", glm::vec2(0, 0), false, 16);
	itemInventory.push_back(medipack);
	itemQuantities["MEDIPACK"] = 10;

	//SPEED POTION
	Item* speedPotion = new Item();
	speedPotion->setItem("SPEED POTION", 1, "Increases speed for 10 seconds.", glm::vec2(0, 0), false, 16);
	itemInventory.push_back(speedPotion);
	itemQuantities["SPEED POTION"] = 10;

	//ARROW
	Item* arrow = new Item();
	arrow->setItem("ARROW", 1, "Projectile for bow", glm::vec2(0, 0), false, 16);
	itemInventory.push_back(arrow);
	itemQuantities["ARROW"] = 40;

	//BOW weapon
	Item* bow = new Item();
	bow->setItem("BOW", 1, "Long range weapon.", glm::vec2(0, 0), true, 16);
	weaponInventory.push_back(bow);


	if (!itemInventory.empty()) currentItemIndex = 0;

	cout << "Given: MEDIPACK, SPEED POTION, ARROW (x5), BOW" << endl;
	cout << "Total items: " << itemInventory.size() << endl;
	cout << "Total weapons: " << weaponInventory.size() << endl;
}