#include "Level.h"
#include "MovingStatue.h" 
#include "RangedEnemy.h"
#include "MeleeEnemy.h"
#include <iostream>
#include "Projectile.h"
#include "ServiceLocator.h"

#define SCREEN_X 0
#define SCREEN_Y 0

#define CAMERA_WIDTH 320  
#define CAMERA_HEIGHT 320

Level::Level()
{
	Scene::Scene();
	this->player = nullptr;
	this->initPlayerX = 0;
	this->initPlayerY = 0;
	this->uiManager = nullptr;
}

Level::Level(const vector<string>& tileMapFiles, Player* player, 
	int initPlayerX, int initPlayerY, const vector<EnemyConfig>& enemyConfigs, const vector<MovingObjectConfig>& objectConfigs, const vector<MusicConfig>& musicConfigs, LevelType type)
	: Scene(tileMapFiles)
{
	this->player = player;
	this->initPlayerX = initPlayerX;
	this->initPlayerY = initPlayerY;
	this->enemyConfigs = enemyConfigs;
	this->movingObjectConfigs = objectConfigs;
	this->musicConfigs = musicConfigs;
	this->type = type;
	this->init();
	//Initialize enemies
	initializeEnemies();
	initializeItems();
	//Initialize moving objects
	initializeMovingObjects();
	this->uiManager = nullptr;
}

Level::~Level()
{
	for (MovingObject* obj : movingObjects) {
		if (obj != nullptr) {
			delete obj;
		}
	}
	movingObjects.clear();
	clearEnemies();
	clearItems();
	this->player = nullptr;
}

void Level::reStartLevel() {
	clearItems();
	clearEnemies();
	clearProjectiles();
	initializeEnemies();
	initializeItems();
	initializeMusic();
	introMessagesDisplayed = false;
	introMessageDelayTimer = 500;
}

void Level::init() 
{	
	Scene::init();
	reStartLevel();
	// Initialize projectile manager
	projectileManager.init(&texProgram, maps[0]);

	//Projection matrix override
	projection = glm::ortho(0.f, float(CAMERA_WIDTH), float(CAMERA_HEIGHT), 0.f);
	
	// Initialize player
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), this->texProgram);
	player->setPosition(glm::vec2(this->initPlayerX * maps[0]->getTileSize(), this->initPlayerY * maps[0]->getTileSize()));
	player->setTileMaps(maps);

	player->setProjectileManager(&projectileManager);


	int mapWidth = maps[0]->mapSize.x * maps[0]->getTileSize();
	int mapHeight = maps[0]->mapSize.y * maps[0]->getTileSize();

	numSectorsI = mapWidth / CAMERA_WIDTH;
	numSectorsJ = mapHeight / CAMERA_HEIGHT;

	if (mapWidth % CAMERA_WIDTH != 0) numSectorsI++;
	if (mapHeight % CAMERA_HEIGHT != 0) numSectorsJ++;

	sectorWidth = CAMERA_WIDTH;
	sectorHeight = CAMERA_HEIGHT;

	glm::vec2 playerPos = player->getPosition();
	currentSectorI = static_cast<int>(playerPos.x) / sectorWidth;
	currentSectorJ = static_cast<int>(playerPos.y) / sectorHeight;

	currentSectorI = glm::clamp(currentSectorI, 0, numSectorsI - 1);
	currentSectorJ = glm::clamp(currentSectorJ, 0, numSectorsJ - 1);

	calculateCameraOffset();
}

void Level::update(int deltaTime)
{
	Scene::update(deltaTime);
	player->update(deltaTime);
	for (unsigned int i = 0; i < enemies.size(); i++) enemies[i]->update(deltaTime);
	projectileManager.update(deltaTime);
	checkCombat();
	for (MovingObject* obj : movingObjects) {
		if (obj != nullptr) {
			obj->setCameraOffset(glm::vec2(cameraOffsetX, cameraOffsetY));
			obj->update(deltaTime);
		}
	}

	checkItemPickUp();
	updateCameraSector();

	if (uiManager && !introMessagesDisplayed)
	{
		introMessageDelayTimer -= deltaTime;
		if (introMessageDelayTimer <= 0)
		{
			uiManager->showTemporaryMessage("HELLO SOLID NINJA, KILL ALL ENEMIES",
				glm::vec2(320, 160), 1.f, glm::vec3(0.f, 0.f, 0.f), 4000);

			uiManager->showTemporaryMessage("AND WE WON'T KILL YOU...",
				glm::vec2(320, 200), 1.f, glm::vec3(1.f, 0.f, 0.f), 5000);

			introMessagesDisplayed = true;
		}
	}
}

void Level::setUIManager(UIManager* uiManager)
{
	this->uiManager = uiManager;
}

void Level::updateCameraSector()
{
	glm::vec2 playerPos = player->getPosition();
	int newSectorI = static_cast<int>(playerPos.x) / sectorWidth;
	int newSectorJ = static_cast<int>(playerPos.y) / sectorHeight;
	newSectorI = glm::clamp(newSectorI, 0, numSectorsI - 1);
	newSectorJ = glm::clamp(newSectorJ, 0, numSectorsJ - 1);

	// Check if sector changed
	if (newSectorI != currentSectorI || newSectorJ != currentSectorJ)
	{
		currentSectorI = newSectorI;
		currentSectorJ = newSectorJ;
		calculateCameraOffset();
		updateMusic();
	}
}

void Level::updateMusic()
{
	auto it = sectorMusicMap.find({ currentSectorI, currentSectorJ });

	if (it != sectorMusicMap.end()) {
		const MusicConfig& config = it->second;
		if (config.musicFile != currentMusicFile) {
			currentMusicFile = config.musicFile;
			ServiceLocator::getAudio().playMusic(config.musicFile.c_str());
		}
	}
}

void Level::calculateCameraOffset()
{
	// Calculate the offset based on current sector
	cameraOffsetX = -static_cast<float>(currentSectorI * sectorWidth);
	cameraOffsetY = -static_cast<float>(currentSectorJ * sectorHeight);
}

void Level::render()
{
	texProgram.use();
	setupViewport(0.85f, 0.15f);

	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);

	// Create view matrix with camera offset
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(cameraOffsetX, cameraOffsetY, 0.0f));

	// For tilemap, set modelview directly (if tilemap doesn't need local transform)
	texProgram.setUniformMatrix4f("modelview", view);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);

	// Map render
	for (unsigned int i = 0; i < maps.size(); i++)
		maps[i]->render();

	for (unsigned int i = 0; i < items.size(); i++)
		items[i]->render(view);

	// Render all enemies
	for (unsigned int i = 0; i < enemies.size(); i++) enemies[i]->render(view);
	projectileManager.render(view);
	// Player render - pass the view matrix
	for (MovingObject* obj : movingObjects) {
		if (obj != nullptr) {
			obj->render(view);
		}
	}
	player->render(view);
}

void Level::initializeEnemies() {
	for (const auto& config : enemyConfigs) {
		Enemy* enemy = nullptr;
		switch (config.type) {
			case EnemyType::BASE:
				enemy = nullptr;
				break;
			case EnemyType::MELEE:
				enemy = new MeleeEnemy();
				break;
			case EnemyType::RANGED:
				enemy = new RangedEnemy();
				break;
			default:
				enemy = nullptr;
				break;
		}
		enemy->init(glm::ivec2(SCREEN_X, SCREEN_Y), this->texProgram, maps[0], config.spriteSheet);
		enemy->setPosition(glm::ivec2(config.xTile * maps[0]->getTileSize(), config.yTile * maps[0]->getTileSize()));
		enemy->setProjectileManager(&projectileManager);
		enemies.push_back(enemy);
	}
}

void Level::initializeMovingObjects()
{
	movingObjectTextures.resize(movingObjectConfigs.size());

	for (size_t i = 0; i < movingObjectConfigs.size(); i++) {
		const MovingObjectConfig& config = movingObjectConfigs[i];
		MovingObject* obj = nullptr; // Initialize the pointer

		// Load texture first, as it's needed for the constructor
		bool loaded = movingObjectTextures[i].loadFromFile(config.spriteSheet, TEXTURE_PIXEL_FORMAT_RGBA);
		if (!loaded) {
			std::cout << "ERROR: Failed to load moving object texture: " << config.spriteSheet << std::endl;
			continue;
		}

		movingObjectTextures[i].setMinFilter(GL_NEAREST);
		movingObjectTextures[i].setMagFilter(GL_NEAREST);

		switch (config.type) {
		case MovingObjectType::MOVING_STATUE:
			obj = new MovingStatue(config.spriteSize, config.texCoordSize,
				&movingObjectTextures[i], &texProgram);
			break;
			
		default:
			std::cout << "ERROR: Unknown MovingObject type in config." << std::endl;
			continue; 
		}

		if (obj == nullptr) continue;

		obj->setMovementPath(config.startPos, config.endPos, config.speed);

		obj->setNumberAnimations(1);
		obj->setAnimationSpeed(0, 8);
		obj->addKeyframe(0, glm::vec2(0.0f, 0.0f));
		obj->changeAnimation(0);

		obj->setTileMapPosition(glm::ivec2(config.startPos.x, config.startPos.y),
			glm::ivec2(SCREEN_X, SCREEN_Y));
		obj->setTileMaps(maps);

		movingObjects.push_back(obj);
	}
}

void Level::initializeMusic()
{
	// Build a map for quick sector -> music lookup
	for (const MusicConfig& config : musicConfigs) {
		sectorMusicMap[{config.sectorI, config.sectorJ}] = config;
	}

	// Play initial music for starting sector
	updateMusic();
}

void Level::checkCombat()
{
	if (!player->isAlive()) return;

	// Check enemy collision with player (contact damage)
	for (auto& enemy : enemies) {
		glm::vec2 playerSize(PLAYER_SIZE, PLAYER_SIZE);
		glm::vec2 enemySize(ENEMY_SIZE, ENEMY_SIZE);
		if (isColliding(player->getPositionFloat(), playerSize,
			enemy->getPosition(), enemySize)) {

			// Only deal damage if enemy is ready to attack
			if (enemy->canDealDamage()) {
				player->takeDamage(enemy->getDamage());
				enemy->onDamageDealt(); // Reset cooldown
			}
		}
	}
	for (Projectile* projectile : projectileManager.getActiveProjectiles()) {
		if (projectile->getIsPlayerProjectile()) {
			// PLAYER projectiles hit ENEMIES
			glm::vec2 enemySize(ENEMY_SIZE, ENEMY_SIZE);

			for (auto it = enemies.begin(); it != enemies.end(); ) {
				Enemy* enemy = *it;

				if (isColliding(projectile->getPosition(), projectile->getSize(),
					enemy->getPosition(), enemySize)) {
					enemy->takeDamage(projectile->getDamage());
					projectile->deactivate();

					if (!enemy->isAlive()) {
						delete enemy;
						player->increaseRank(1);
						it = enemies.erase(it);
						continue;
					}
				}
				++it;
			}
		}
		else {
			// ENEMY projectiles hit PLAYER
			glm::vec2 playerSize(PLAYER_SIZE, PLAYER_SIZE);

			if (isColliding(player->getPositionFloat(), playerSize,
				projectile->getPosition(), projectile->getSize())) {
				player->takeDamage(projectile->getDamage());
				projectile->deactivate();
			}
		}
	}
	handlePlayerAttack();
}

bool Level::isColliding(const glm::vec2& pos1, const glm::vec2& size1,
	const glm::vec2& pos2, const glm::vec2& size2)
{
	// AABB (Axis-Aligned Bounding Box) collision detection
	return (pos1.x < pos2.x + size2.x &&
		pos1.x + size1.x > pos2.x &&
		pos1.y < pos2.y + size2.y &&
		pos1.y + size1.y > pos2.y);
}

void Level::handlePlayerAttack()
{
	if (!player->justStartedPunching()) return;

	const float PUNCH_REACH = 20.0f;
	const int PUNCH_DAMAGE = 1.f;

	glm::vec2 punchPos = player->getPunchHitbox();

	glm::vec2 punchSize(PLAYER_SIZE + PUNCH_REACH, PLAYER_SIZE + PUNCH_REACH);
	glm::vec2 enemySize(ENEMY_SIZE, ENEMY_SIZE);


	for (auto it = enemies.begin(); it != enemies.end(); ) {
		Enemy* enemy = *it;

		if (isColliding(punchPos, punchSize,
			enemy->getPosition(), enemySize)) {
			enemy->takeDamage(PUNCH_DAMAGE);

			if (!enemy->isAlive()) {
				delete enemy;
				player->increaseRank(1);
				it = enemies.erase(it);
				continue;
			}
		}
		++it;
	}
}

void Level::initializeItems() {
	int tileSize = maps[0]->getTileSize();

	initializeObjects(tileSize);
	initializeWeapons(tileSize);
}

void Level::initializeObjects(int tileSize) {
	if (type == LevelType::OUTSIDE) {
		Texture* medpackTexture = new Texture();
		medpackTexture->loadFromFile("images/items/Medipack.png", TEXTURE_PIXEL_FORMAT_RGBA);

		Item* medipack = new Item(
			glm::vec2(tileSize - 4, tileSize - 4),
			glm::vec2(1.0f, 1.0f),
			medpackTexture,
			&this->texProgram,
			glm::ivec2(SCREEN_X, SCREEN_Y)
		);
		medipack->setItem("MEDIPACK", 1, "Restores 50 health points.", glm::vec2(25, 10), false, tileSize);

		Item* medipack2 = new Item(
			glm::vec2(tileSize - 4, tileSize - 4),
			glm::vec2(1.0f, 1.0f),
			medpackTexture,  // Share same texture                     
			&this->texProgram,
			glm::ivec2(SCREEN_X, SCREEN_Y)
		);
		medipack2->setItem("MEDIPACK", 1, "Restores 50 health points.", glm::vec2(30, 15), false, tileSize);

		Texture* speedPotionTexture = new Texture();
		speedPotionTexture->loadFromFile("images/items/SPEED POTION.png", TEXTURE_PIXEL_FORMAT_RGBA);
		Item* speedPotion = new Item(
			glm::vec2(tileSize - 3, tileSize - 3),
			glm::vec2(1.0f, 1.0f),
			speedPotionTexture,
			&this->texProgram,
			glm::ivec2(SCREEN_X, SCREEN_Y)
		);
		speedPotion->setItem("SPEED POTION", 1, "Increases speed for 10 seconds.", glm::vec2(6, 10), false, tileSize);

		Texture* arrowTexture = new Texture();
		arrowTexture->loadFromFile("images/items/Arrow.png", TEXTURE_PIXEL_FORMAT_RGBA);
		Item* arrow = new Item(
			glm::vec2(tileSize - 3, tileSize - 3),
			glm::vec2(1.0f, 1.0f),
			arrowTexture,
			&this->texProgram,
			glm::ivec2(SCREEN_X, SCREEN_Y)
		);
		arrow->setItem("ARROW", 1, "Projectile for bow", glm::vec2(6, 5), false, tileSize);
		Item* arrow2 = new Item(
			glm::vec2(tileSize - 3, tileSize - 3),
			glm::vec2(1.0f, 1.0f),
			arrowTexture,
			&this->texProgram,
			glm::ivec2(SCREEN_X, SCREEN_Y)
		);
		arrow2->setItem("ARROW", 1, "Projectile for bow", glm::vec2(6, 8), false, tileSize);

		items.push_back(medipack);
		items.push_back(medipack2);
		items.push_back(speedPotion);
		items.push_back(arrow);
		items.push_back(arrow2);
	}
}

void Level::initializeWeapons(int tileSize) {
	if (type == LevelType::OUTSIDE) {
		Texture* bowTexture = new Texture();
		bowTexture->loadFromFile("images/items/Bow.png", TEXTURE_PIXEL_FORMAT_RGBA);

		Item* bow = new Item(
			glm::vec2(16, 16),
			glm::vec2(1.0f, 1.0f),
			bowTexture,
			&this->texProgram,
			glm::ivec2(SCREEN_X, SCREEN_Y)
		);
		bow->setItem("BOW", 1, "Long range weapon.", glm::vec2(15, 10), true, tileSize);
		items.push_back(bow);
	}
}

void Level::clearProjectiles() {
	projectileManager.clear();
}

void Level::clearEnemies() {
	for (unsigned int i = 0; i < enemies.size(); i++)
	{
		if (enemies[i] != nullptr)
		{
			delete enemies[i];
			enemies[i] = nullptr;
		}
	}
	enemies.clear();
}

void Level::clearItems() {
	for (unsigned int i = 0; i < items.size(); i++)
	{
		if (items[i] != nullptr)
		{
			delete items[i];
			items[i] = nullptr;
		}
	}
	items.clear();
}


void Level::checkItemPickUp() {
	glm::vec2 playerPos = player->getPositionFloat();
	for (int i = 0; i < items.size(); i++)
	{
		glm::vec2 itemPos = items[i]->getPosition();
		if (checkColission(playerPos, itemPos, 16, 16)) 
		{
			std::cout << "Item picked up at position: (" << itemPos.x << ", " << itemPos.y << ")\n";
			itemPickUpEvent(i);
			break;
		}
	}
}

void Level::itemPickUpEvent(int indexInVector) {
	Item* itemPicked = items[indexInVector];

	bool isFirstOfKind = (player->getItemQuantity(itemPicked->getName()) == 0);

	player->addItem(itemPicked);

	if (uiManager != nullptr) {
		std::string pickupText = "PICKED UP " + itemPicked->getName() + "!";
		glm::vec2 messagePos(320, 160);
		glm::vec3 messageColor(0.f, 0.f, 0.f);
		uiManager->showTemporaryMessage(pickupText, messagePos, 1.0f, messageColor, 2000);
	}

	//only delete if itss not the first (Player keeps first instance)
	if (!isFirstOfKind && !itemPicked->getIsWeapon()) {
		delete itemPicked;
	}

	items.erase(items.begin() + indexInVector);
}

bool Level::checkColission(glm::vec2& pos1, glm::vec2& pos2, int size1, int size2) {
	bool collisionX = pos1.x + size1 > pos2.x &&
		pos2.x + size2 > pos1.x;

	bool collisionY = pos1.y + size1 > pos2.y &&
		pos2.y + size2 > pos1.y;

	return collisionX && collisionY;
}
