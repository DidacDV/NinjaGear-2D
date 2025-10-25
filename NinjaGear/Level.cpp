#include "Level.h"
#include "RangedEnemy.h"
#include "MeleeEnemy.h"
#include <iostream>

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
	int initPlayerX, int initPlayerY, const vector<EnemyConfig>& enemyConfigs)
	: Scene(tileMapFiles)
{
	this->player = player;
	this->initPlayerX = initPlayerX;
	this->initPlayerY = initPlayerY;
	this->enemyConfigs = enemyConfigs;
	this->uiManager = nullptr;

	//Initialize camera sector tracking variables
	currentSectorI = 0;
    currentSectorJ = 0;
    numSectorsI = 0;
    numSectorsJ = 0;
    sectorWidth = 0;
    sectorHeight = 0;
    cameraOffsetX = 0.0f;
    cameraOffsetY = 0.0f;
}

Level::~Level()
{
	Scene::~Scene();
	clearEnemies();
	clearItems();
	this->player = nullptr;
}

void Level::init() 
{
	Scene::init();
	//Projection matrix override
	projection = glm::ortho(0.f, float(CAMERA_WIDTH), float(CAMERA_HEIGHT), 0.f);
	
	// Initialize player
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), this->texProgram);
	player->setPosition(glm::vec2(this->initPlayerX * maps[0]->getTileSize(), this->initPlayerY * maps[0]->getTileSize()));
	player->setTileMaps(maps);

	//need to clear on each init to not duplicate enemies frames
	clearEnemies();
	initializeEnemies();

	clearItems();
	initializeItems();

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

	checkItemPickUp();

	updateCameraSector();
}

void Level::setUIManager(UIManager* uiManager)
{
	this->uiManager = uiManager;
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
	glm::mat4 modelview;
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
	// Player render - pass the view matrix
	player->render(view);
}

// Helper method to add enemies
void Level::addEnemy(const string& spriteSheet, int initX, int initY)
{
    Enemy* enemy = new Enemy();
    enemy->setSpriteSheet(spriteSheet);
    enemies.push_back(enemy);
}

void Level::initializeEnemies() {
	for (const auto& config : enemyConfigs) {
		Enemy* enemy = nullptr;
		switch (config.type) {
			case EnemyType::BASE:
				enemy = new Enemy();
				break;
			case EnemyType::MELEE:
				enemy = new MeleeEnemy();
				break;
			case EnemyType::RANGED:
				enemy = new RangedEnemy();
				break;
			default:
				enemy = new Enemy();
				break;
		}
		enemy->setSpriteSheet(config.spriteSheet);
		enemy->init(glm::ivec2(SCREEN_X, SCREEN_Y), this->texProgram, maps[0]);
		enemy->setPosition(glm::ivec2(config.xTile * maps[0]->getTileSize(), config.yTile * maps[0]->getTileSize()));
		enemies.push_back(enemy);
	}
}

void Level::initializeItems() {
	int tileSize = maps[0]->getTileSize();

	// Example: Create a medipack at tile position (5, 5)
	Texture* medpackTexture = new Texture();
	medpackTexture->loadFromFile("images/items/Medipack.png", TEXTURE_PIXEL_FORMAT_RGBA);

	Item* medipack = new Item(
		glm::vec2(tileSize, tileSize),       
		glm::vec2(1.0f, 1.0f),                
		medpackTexture,                      
		&this->texProgram,                    
		glm::ivec2(SCREEN_X, SCREEN_Y)
	);
	medipack->setItem("MEDIPACK", 1, "Restores 50 health points.", glm::vec2(25, 10), tileSize);

	Texture* rapierTexture = new Texture();
	rapierTexture->loadFromFile("images/items/Rapier.png", TEXTURE_PIXEL_FORMAT_RGBA);

	Item* rapier = new Item(
		glm::vec2(10, 15),       
		glm::vec2(1.0f, 1.0f),                
		rapierTexture,
		&this->texProgram,                    
		glm::ivec2(SCREEN_X, SCREEN_Y)
	);
	rapier->setItem("RAPIER", 1, "Medium range weapon.", glm::vec2(15, 10), tileSize);


	items.push_back(medipack);
	items.push_back(rapier);
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
			//remove item from the level

			//TODO -> UPDATE PLAYER TO GIVE HIM THIS ITEM (and so the UI will be updated too)
			break;
		}
	}
}

void Level::itemPickUpEvent(int indexInVector) {
	Item* itemPicked = items[indexInVector];

	if (uiManager != nullptr) {
		std::string pickupText = "PICKED UP " + itemPicked->getName() + "!";
		glm::vec2 messagePos(320, 160);
		glm::vec3 messageColor(0.f, .0f, 0.f);  
		uiManager->showTemporaryMessage(pickupText, messagePos, 1.0f, messageColor, 2000); // 2 seconds
	}

	//TODO -> player pick up X
	delete items[indexInVector];
	items.erase(items.begin() + indexInVector);
}

bool Level::checkColission(glm::vec2& pos1, glm::vec2& pos2, int size1, int size2) {
	bool collisionX = pos1.x + size1 > pos2.x &&
		pos2.x + size2 > pos1.x;

	bool collisionY = pos1.y + size1 > pos2.y &&
		pos2.y + size2 > pos1.y;

	return collisionX && collisionY;
}