#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"
#include "ServiceLocator.h"

Scene::Scene()
{
	this->currentTime = 0.0f;
}

Scene::Scene(const vector<string>& tileMapFiles)
{
	cout << "detecting tilemaps" << endl;
	this->currentTime = 0.0f;
	mapFiles = tileMapFiles;
}

Scene::~Scene()
{
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

	// Calculate projection based on map size
	int mapWidthInPixels = maps[0]->mapSize.x * maps[0]->getTileSize();
	int mapHeightInPixels = maps[0]->mapSize.y * maps[0]->getTileSize();
	projection = glm::ortho(0.f, float(mapWidthInPixels), float(mapHeightInPixels), 0.f);
	currentTime = 0.0f;
}

void Scene::update(int deltaTime)
{
	currentTime += deltaTime;
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
		TileMap* map = TileMap::createTileMap(file, glm::vec2(GameConfig::SCREEN_X, GameConfig::SCREEN_Y), texProgram);
		if (map != NULL) {
			maps.push_back(map);
			cout << "Loaded: " << file << endl;
		}
		else {
			cout << "Failed to load: " << file << endl;
		}
	}
}

void Scene::setupViewport(float heightPercent, float yOffsetPercent) {
	int actualWindowWidth = globalScreenWidth;
	int actualWindowHeight = globalScreenHeight;
	int sectionHeight = static_cast<int>(actualWindowHeight * heightPercent);
	int sectionY = static_cast<int>(actualWindowHeight * yOffsetPercent);
	glViewport(0, sectionY, actualWindowWidth, sectionHeight);
}


void Scene::resetViewportFullScreen() {
	glViewport(0, 0, globalScreenWidth, globalScreenHeight);
}

void Scene::playMusic() {
	musicPlaying = true;
	if(currentMusicFile !="" ) ServiceLocator::getAudio().playMusic(currentMusicFile.c_str());
}

void Scene::stopMusic() {
	musicPlaying = false;
	ServiceLocator::getAudio().playMusic("");
}