#include "Level.h"
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
}

Level::Level(const vector<string>& tileMapFiles, Player* player, int initPlayerX, int initPlayerY)
	: Scene(tileMapFiles)
{
	this->player = player;
	this->initPlayerX = initPlayerX;
	this->initPlayerY = initPlayerY;

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
}

void Level::init() 
{
	Scene::init();
	//Projection matrix override
	projection = glm::ortho(0.f, float(CAMERA_WIDTH), float(CAMERA_HEIGHT), 0.f);
	
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
}

void Level::update(int deltaTime)
{
	Scene::update(deltaTime);
	player->update(deltaTime);
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

	// Player render - pass the view matrix
	player->render(view);
}
