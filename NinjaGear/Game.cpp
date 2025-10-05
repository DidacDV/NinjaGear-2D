#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Game.h"


void Game::init(int screenWidth, int screenHeight)
{
	bPlay = true;
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	vector<string> level1Maps = { "levels/outisde_1.txt"};
	vector<string> level2Maps = { "levels/test.txt" };
	Scene* level1 = new Scene(level1Maps);
	Scene* level2 = new Scene(level2Maps);
	addScene("level1", level1);
	addScene("level2", level2);
	setCurrentScene("level1");
}

bool Game::update(int deltaTime)
{
	if (currentScene != NULL)
		currentScene->update(deltaTime);

	return bPlay;
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (currentScene != NULL)
		currentScene->render();
}

void Game::keyPressed(int key)
{
	if(key == GLFW_KEY_ESCAPE) // Escape code
		bPlay = false;
	keys[key] = true;
}

void Game::keyReleased(int key)
{
	keys[key] = false;
}

void Game::mouseMove(int x, int y)
{
}

void Game::mousePress(int button)
{
}

void Game::mouseRelease(int button)
{
}

bool Game::getKey(int key) const
{
	return keys[key];
}

void Game::addScene(const string& name, Scene* scene)
{
	levels[name] = scene;
}

void Game::setCurrentScene(const string& name)
{
	auto it = levels.find(name);
	if (it != levels.end()) {
		currentScene = it->second;
		currentScene->init(screenWidth, screenHeight);
	}
}

Scene* Game::getCurrentScene() const
{
	return currentScene;
}



