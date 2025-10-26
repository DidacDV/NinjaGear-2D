#include "Level.h"
#include "MovingStatue.h" 
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
}

Level::Level(const vector<string>& tileMapFiles, Player* player, 
	int initPlayerX, int initPlayerY, const vector<EnemyConfig>& enemyConfigs, const vector<MovingObjectConfig>& objectConfigs, const vector<MusicConfig>& musicConfigs)
	: Scene(tileMapFiles)
{
	this->player = player;
	this->initPlayerX = initPlayerX;
	this->initPlayerY = initPlayerY;
	this->enemyConfigs = enemyConfigs;
	this->movingObjectConfigs = objectConfigs;
	this->musicConfigs = musicConfigs;
	this->init();
	//Initialize enemies
	initializeEnemies();
	//Initialize moving objects
	initializeMovingObjects();
}

Level::~Level()
{
	for (unsigned int i = 0; i < enemies.size(); i++)
	{
		if (enemies[i] != nullptr)
		{
			delete enemies[i];
			enemies[i] = nullptr;
		}
	}
	enemies.clear();
	for (MovingObject* obj : movingObjects) {
		if (obj != nullptr) {
			delete obj;
		}
	}
	movingObjects.clear();
}

void Level::init() 
{	
	Scene::init();
	// Initialize projectile manager
	projectileManager.init(&texProgram, maps[0]);

	//Projection matrix override
	projection = glm::ortho(0.f, float(CAMERA_WIDTH), float(CAMERA_HEIGHT), 0.f);
	
	// Initialize player
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), this->texProgram);
	player->setPosition(glm::vec2(this->initPlayerX * maps[0]->getTileSize(), this->initPlayerY * maps[0]->getTileSize()));
	player->setTileMaps(maps);

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
	initializeMusic();
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
	updateCameraSector();
}

void Level::updateCameraSector()
{
	glm::vec2 playerPos = player->getPosition();

	// Calculate which sector the player is currently in
	int newSectorI = static_cast<int>(playerPos.x) / sectorWidth;
	int newSectorJ = static_cast<int>(playerPos.y) / sectorHeight;

	// Clamp to valid sector range
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
		Enemy* enemy = config.enemyInstance;
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

	//Check projectile collisions
	for (Projectile* projectile : projectileManager.getActiveProjectiles()) {
		glm::vec2 playerSize(PLAYER_SIZE, PLAYER_SIZE);

		if (isColliding(player->getPositionFloat(), playerSize,
			projectile->getPosition(), projectile->getSize())) {
			player->takeDamage(projectile->getDamage());
			projectile->deactivate();
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
	const int PUNCH_DAMAGE = 25;

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

