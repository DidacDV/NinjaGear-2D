#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"


#define SCREEN_X 0
#define SCREEN_Y 0

#define INIT_PLAYER_X_TILES 4
#define INIT_PLAYER_Y_TILES 19


Scene::Scene()
{
	player = NULL;
	mapFiles = {
		"levels/outisde_1.txt",
		"levels/test.txt"
	};
}

Scene::Scene(const vector<string>& tileMapFiles)
{
	player = NULL;
	mapFiles = tileMapFiles;
}

Scene::~Scene()
{
	if(player != NULL)
		delete player;
	for (unsigned int i = 0; i < maps.size(); i++)
	{
		if (maps[i] != NULL)
			delete maps[i];
	}
	maps.clear();
}


void Scene::init()
{
	initShaders();
	loadTileMaps();

	if (maps.empty()) {
		cout << "ERROR: No tilemaps loaded!" << endl;
		return;
	}

	player = new Player();
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * maps[0]->getTileSize(), INIT_PLAYER_Y_TILES * maps[0]->getTileSize()));
	player->setTileMap(maps[0]);
	projection = glm::ortho(0.f, float(SCREEN_WIDTH), float(SCREEN_HEIGHT), 0.f);
	currentTime = 0.0f;
}

void Scene::update(int deltaTime)
{
	currentTime += deltaTime;
	player->update(deltaTime);
}

void Scene::render()
{
	glm::mat4 modelview;
	texProgram.use();
	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
	modelview = glm::mat4(1.0f);
	texProgram.setUniformMatrix4f("modelview", modelview);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);
	for (unsigned int i = 0; i < maps.size(); i++)maps[i]->render();
	player->render();
}

void Scene::initShaders()
{
	Shader vShader, fShader;

	vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
	if(!vShader.isCompiled())
	{
		cout << "Vertex Shader Error" << endl;
		cout << "" << vShader.log() << endl << endl;
	}
	fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
	if(!fShader.isCompiled())
	{
		cout << "Fragment Shader Error" << endl;
		cout << "" << fShader.log() << endl << endl;
	}
	texProgram.init();
	texProgram.addShader(vShader);
	texProgram.addShader(fShader);
	texProgram.link();
	if(!texProgram.isLinked())
	{
		cout << "Shader Linking Error" << endl;
		cout << "" << texProgram.log() << endl << endl;
	}
	texProgram.bindFragmentOutput("outColor");
	vShader.free();
	fShader.free();
}

void Scene::loadTileMaps()
{
	for (const string& file : mapFiles) {
		TileMap* map = TileMap::createTileMap(file, glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
		if (map != NULL) {
			maps.push_back(map);
			cout << "Loaded: " << file << endl;
		}
		else {
			cout << "Failed to load: " << file << endl;
		}
	}
}



