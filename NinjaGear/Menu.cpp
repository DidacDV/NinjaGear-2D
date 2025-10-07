#include "Menu.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#define CAMERA_WIDTH 480  
#define CAMERA_HEIGHT 320


Menu::Menu()
{
	Scene::Scene();
}

Menu::Menu(const vector<string>& tileMapFiles)
	: Scene(tileMapFiles)
{
}

Menu::~Menu()
{
	Scene::~Scene();
}

void Menu::init()
{
	Scene::init();
}

void Menu::update(int deltaTime)
{
	Scene::update(deltaTime);
}

void Menu::render()
{
	if (maps.empty()) {
		std::cout << "ERROR: Menu has no maps to render!" << std::endl;
		return;
	}
	glm::mat4 modelview;
	texProgram.use();
	setupViewport(0.1f, 0.0f);

	int mapHeightInPixels = maps[0]->mapSize.y * maps[0]->getTileSize();

	texProgram.setUniformMatrix4f("projection", projection);
	texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);

	glm::mat4 view = glm::mat4(1.0f);
	texProgram.setUniformMatrix4f("modelview", view);
	texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);

	for (unsigned int i = 0; i < maps.size(); i++)
		maps[i]->render();
}