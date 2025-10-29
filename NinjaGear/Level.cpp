#include "Level.h"
#include <iostream>
#include "Projectile.h"
#include "ServiceLocator.h"
#include "MovingObjectFactory.h"
#include "EnemyFactory.h"

#include "GameConfig.h"
#include "InterpolatedTileMap.h"

Level::Level()
{
	Scene::Scene();
	this->player = nullptr;
	this->initPlayerX = 0;
	this->initPlayerY = 0;
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

	initializeEnemies();
	initializeItems();
	initializeMovingObjects();
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
	introMessageDelayTimer = 500;
	introMessagesDisplayed = false;
}

void Level::init() 
{	
	Scene::init();
	navigationMap = InterpolatedTileMap::createComposite(maps);
	reStartLevel();
	initializeTransitionTiles();
	// Initialize projectile manager
	projectileManager.init(&texProgram, navigationMap);

	//Projection matrix override
	projection = glm::ortho(0.f, float(GameConfig::CAMERA_WIDTH), float(GameConfig::CAMERA_HEIGHT), 0.f);
	
	// Initialize player
	if(!player->isInitialized() || type != LevelType::DUNGEON) 
		player->init(glm::ivec2(GameConfig::SCREEN_X, GameConfig::SCREEN_Y), this->texProgram);
	player->setPosition(glm::vec2(this->initPlayerX * maps[0]->getTileSize(), this->initPlayerY * maps[0]->getTileSize()));
	player->setTileMaps(maps);

	player->setProjectileManager(&projectileManager);


	int mapWidth = maps[0]->mapSize.x * maps[0]->getTileSize();
	int mapHeight = maps[0]->mapSize.y * maps[0]->getTileSize();

	numSectorsI = mapWidth / GameConfig::CAMERA_WIDTH;
	numSectorsJ = mapHeight / GameConfig::CAMERA_HEIGHT;

	if (mapWidth % GameConfig::CAMERA_WIDTH != 0) numSectorsI++;
	if (mapHeight % GameConfig::CAMERA_HEIGHT != 0) numSectorsJ++;

	sectorWidth = GameConfig::CAMERA_WIDTH;
	sectorHeight = GameConfig::CAMERA_HEIGHT;

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

	checkTransitionTiles();

	for (unsigned int i = 0; i < enemies.size(); i++) {
		enemies[i]->setCameraOffset(glm::vec2(cameraOffsetX, cameraOffsetY));
		enemies[i]->update(deltaTime);
	}
	projectileManager.update(deltaTime);
	checkCombat();
	for (MovingObject* obj : movingObjects) {
		if (obj != nullptr) {
			obj->setCameraOffset(glm::vec2(cameraOffsetX, cameraOffsetY));
			obj->update(deltaTime);
		}
	}

	checkMovingObjectCollisions();
	checkItemPickUp();
	updateCameraSector();

	if (!introMessagesDisplayed && type == LevelType::OUTSIDE)
	{
		introMessageDelayTimer -= deltaTime;
		if (introMessageDelayTimer <= 0)
		{
			ServiceLocator::getUI().showTemporaryMessage(
				"MISSION START: SHADOW OPERATIVE – 'SILENT EDGE'",
				glm::vec2(GameConfig::CENTER_X - 240, GameConfig::CENTER_Y - 260),
				1.f, glm::vec3(0.f, 0.f, 0.f), 5000);

			ServiceLocator::getUI().showTemporaryMessage(
				"INFILTRATE THE FORTRESS - NEUTRALIZE ALL ENEMY UNITS.",
				glm::vec2(GameConfig::CENTER_X - 240, GameConfig::CENTER_Y - 220),
				1.f, glm::vec3(0.f, 0.f, 0.f), 6000);

			ServiceLocator::getUI().showTemporaryMessage(
				"OBJECTIVE: REACH THE COMMAND CHAMBER AND ELIMINATE THE FLAME LORD.",
				glm::vec2(GameConfig::CENTER_X - 240, GameConfig::CENTER_Y - 180),
				1.f, glm::vec3(1.f, 0.f, 0.f), 7000);

			introMessagesDisplayed = true;
		}
	}
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

	int uiHeight = static_cast<int>(GameConfig::WINDOW_HEIGHT * 0.15f);
	int gameHeight = static_cast<int>(GameConfig::WINDOW_HEIGHT * 0.85f);

	// Center the game area horizontally
	int viewportX = (GameConfig::WINDOW_WIDTH - GameConfig::GAME_WIDTH) / 2;

	int viewportY = uiHeight;
	glViewport(viewportX, viewportY, GameConfig::GAME_WIDTH, gameHeight);



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
		Enemy* enemy = EnemyFactory::create(
			config,
			glm::ivec2(GameConfig::SCREEN_X, GameConfig::SCREEN_Y),
			texProgram,
			navigationMap,
			&projectileManager
		);
		if (enemy) {
			enemies.push_back(enemy);
		}
	}
}

void Level::initializeMovingObjects()
{
	movingObjectTextures.resize(movingObjectConfigs.size());

	for (size_t i = 0; i < movingObjectConfigs.size(); i++) {
		const MovingObjectConfig& config = movingObjectConfigs[i];

		if (!movingObjectTextures[i].loadFromFile(config.spriteSheet, TEXTURE_PIXEL_FORMAT_RGBA)) {
			std::cout << "ERROR: Failed to load texture: " << config.spriteSheet << std::endl;
			continue;
		}

		movingObjectTextures[i].setMinFilter(GL_NEAREST);
		movingObjectTextures[i].setMagFilter(GL_NEAREST);

		MovingObject* obj = MovingObjectFactory::create(
			config,
			&movingObjectTextures[i],
			&texProgram,
			maps,
			glm::ivec2(GameConfig::SCREEN_X, GameConfig::SCREEN_Y)
		);

		if (obj) {
			movingObjects.push_back(obj);
		}
	}
}

void Level::initializeMusic()
{
	for (const MusicConfig& config : musicConfigs) {
		sectorMusicMap[{config.sectorI, config.sectorJ}] = config;
	}
	updateMusic();
}

void Level::checkCombat()
{
	if (!player->isAlive()) return;

	// Check enemy collision with player (contact damage)
	for (auto& enemy : enemies) {
		glm::vec2 playerSize(PLAYER_SIZE, PLAYER_SIZE);
		glm::vec2 enemySize = enemy->getEnemySize();
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
	return (pos1.x < pos2.x + size2.x && pos1.x + size1.x > pos2.x &&
		pos1.y < pos2.y + size2.y && pos1.y + size1.y > pos2.y);
}

void Level::handlePlayerAttack()
{
	if (!player->justStartedPunching()) return;

	const float PUNCH_REACH = 20.0f;
	const int PUNCH_DAMAGE = 1.f;

	glm::vec2 punchPos = player->getPunchHitbox();

	glm::vec2 punchSize(PLAYER_SIZE + PUNCH_REACH, PLAYER_SIZE + PUNCH_REACH);


	for (auto it = enemies.begin(); it != enemies.end(); ) {
		Enemy* enemy = *it;
		glm::vec2 enemySize = enemy->getEnemySize();
		if (isColliding(punchPos, punchSize,
			enemy->getPosition(), enemySize)) {
			enemy->takeDamage(PUNCH_DAMAGE);

			if (!enemy->isAlive()) {
				if (enemy->isBoss()) Game::instance().victory();
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
	auto createItem = [&](Texture* texture, const string& name, int quantity,
		const string& description, const glm::vec2& position, bool isWeapon = false) -> Item* {
			Item* item = new Item(
				glm::vec2(tileSize - 3, tileSize - 3),
				glm::vec2(1.0f, 1.0f),
				texture,
				&this->texProgram,
				glm::ivec2(GameConfig::SCREEN_X, GameConfig::SCREEN_Y)
			);
			item->setItem(name, quantity, description, position, isWeapon, tileSize);
			return item;
		};

	// --- Load textures once ---
	auto* medpackTexture = new Texture();
	medpackTexture->loadFromFile("images/items/Medipack.png", TEXTURE_PIXEL_FORMAT_RGBA);

	auto* speedPotionTexture = new Texture();
	speedPotionTexture->loadFromFile("images/items/SPEED POTION.png", TEXTURE_PIXEL_FORMAT_RGBA);

	auto* arrowTexture = new Texture();
	arrowTexture->loadFromFile("images/items/Arrow.png", TEXTURE_PIXEL_FORMAT_RGBA);
	vector<ItemData> itemData;
	if (type == LevelType::OUTSIDE) {
		itemData = {
			//Medipacks
			{ "MEDIPACK", 1, "Restores 1 health points.", {34, 2}, medpackTexture },
			{ "MEDIPACK", 1, "Restores 1 health points.", {58, 7}, medpackTexture },
			{ "MEDIPACK", 1, "Restores 1 health points.", {57, 31}, medpackTexture },
			{ "MEDIPACK", 1, "Restores 1 health points.", {57, 45}, medpackTexture },
			{ "MEDIPACK", 1, "Restores 1 health points.", {57, 46}, medpackTexture },
			{ "MEDIPACK", 1, "Restores 1 health points.", {43, 67}, medpackTexture },

			//Speed potions
			{ "SPEED POTION", 1, "Increases speed for 3 seconds.", {58, 13}, speedPotionTexture },
			{ "SPEED POTION", 1, "Increases speed for 3 seconds.", {56, 55}, speedPotionTexture },

			//Arrows
			{ "ARROW", 3, "Projectile for bow", {17, 10}, arrowTexture },
			{ "ARROW", 5, "Projectile for bow", {33, 16}, arrowTexture },
			{ "ARROW", 5, "Projectile for bow", {45, 2}, arrowTexture },
			{ "ARROW", 5, "Projectile for bow", {58, 8}, arrowTexture },
			{ "ARROW", 3, "Projectile for bow", {58, 27}, arrowTexture },
			{ "ARROW", 3, "Projectile for bow", {58, 26}, arrowTexture },
			{ "ARROW", 2, "Projectile for bow", {56, 56}, arrowTexture },
			{ "ARROW", 2, "Projectile for bow", {43, 68}, arrowTexture },
		};
	}
	else {
		itemData = {
			{ "MEDIPACK", 1, "Restores 1 health points.", {23, 13}, medpackTexture },
			{ "MEDIPACK", 1, "Restores 1 health points.", {27, 27}, medpackTexture },
			{ "MEDIPACK", 1, "Restores 1 health points.", {30, 71}, medpackTexture },
			{ "MEDIPACK", 1, "Restores 1 health points.", {4, 69}, medpackTexture },
			{ "MEDIPACK", 1, "Restores 1 health points.", {15, 69}, medpackTexture },

			{ "SPEED POTION", 1, "Increases speed for 3 seconds.", {25, 32}, speedPotionTexture },
			{ "SPEED POTION", 1, "Increases speed for 3 seconds.", {29, 71}, speedPotionTexture },

			{ "ARROW", 5, "Projectile for bow", {17, 10}, arrowTexture },
			{ "ARROW", 5, "Projectile for bow", {35, 29}, arrowTexture },
			{ "ARROW", 5, "Projectile for bow", {33, 34}, arrowTexture },
			{ "ARROW", 5, "Projectile for bow", {5, 69}, arrowTexture },
			{ "ARROW", 5, "Projectile for bow", {16, 69}, arrowTexture },
		};
	}

	for (const auto& data : itemData) {
		items.push_back(createItem(data.texture, data.name, data.quantity, data.description, data.pos));
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
			glm::ivec2(GameConfig::SCREEN_X, GameConfig::SCREEN_Y)
		);
		bow->setItem("BOW", 1, "Long range weapon.", glm::vec2(16, 10), true, tileSize);
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

	std::string pickupText = "PICKED UP " + itemPicked->getName() + "!";
	glm::vec2 messagePos(GameConfig::CENTER_X + 200, GameConfig::CENTER_Y - 200);
	glm::vec3 messageColor(0.f, 0.f, 0.f);
	ServiceLocator::getUI().clearAllTemporaryMessages();
	ServiceLocator::getUI().showTemporaryMessage(pickupText, messagePos, 1.0f, messageColor, 2000);

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

void Level::initializeTransitionTiles()
{
	// Clear any existing transitions
	transitionTiles.clear();

	transitionTiles.push_back({ 5002, "dungeon", 17, 38 });
	transitionTiles.push_back({ 5102, "outside", 10, 10 });
}

void Level::checkTransitionTiles()
{
	glm::vec2 playerPos = player->getPosition();
	int tileSize = maps[0]->getTileSize();

	int playerTileX = static_cast<int>(playerPos.x) / tileSize;
	int playerTileY = static_cast<int>(playerPos.y) / tileSize;

	for (unsigned int layer = 0; layer < maps.size(); layer++) {
		int tileId = maps[layer]->getTileAt(playerTileX, playerTileY);

		// Check if this tile triggers a transition
		for (const auto& transition : transitionTiles) {
			if (transition.tileId == tileId) {
				Game::instance().setCurrentScene(transition.targetScene);

				glm::vec2 newPos(transition.targetTileX * tileSize,
					transition.targetTileY * tileSize);
				player->setPosition(newPos);
				return; // Exit after triggering transition
			}
		}
	}
}

void Level::checkMovingObjectCollisions()
{
    if (!player->isAlive()) return;
    
    glm::vec2 playerPos = player->getPositionFloat();
    glm::vec2 playerSize(PLAYER_SIZE, PLAYER_SIZE);
    
    for (MovingObject* obj : movingObjects) {
        if (obj == nullptr) continue;
        
        glm::vec2 objPos = obj->getPosition();
        glm::vec2 objSize = obj->getCollisionSize();
        
        if (isColliding(playerPos, playerSize, objPos, objSize)) {
            if (obj->getDealsDamage()) {
                player->takeDamage(obj->getDamageAmount());
                break;
            }
        }
    }
}
