#include "Level.h"

#define SCREEN_X 0
#define SCREEN_Y 0

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
}

Level::~Level()
{
	Scene::~Scene();
}

void Level::init() 
{
	Scene::init();
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), this->texProgram);
	player->setPosition(glm::vec2(this->initPlayerX * maps[0]->getTileSize(), this->initPlayerY * maps[0]->getTileSize()));
	player->setTileMap(maps[0]);
}

void Level::update(int deltaTime)
{
	Scene::update(deltaTime);
	player->update(deltaTime);
}

void Level::render()
{
	Scene::render();
	player->render();
}
