#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Game.h"


void Game::init(int screenWidth, int screenHeight)
{
	bPlay = true;
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	//Create the player
	this->player = new Player();
	this->player->setSpriteSheet("images/characters/ninja_dark/SpriteSheet.png");
	//Create the levels (scenes) and add them to the game
	vector<string> titlesScreenMaps = { "levels/base_menu.txt" };
	Scene* baseMenu = new Menu(titlesScreenMaps);

	vector<string> jungle_layers = { 
		"levels/Jungle_background.txt",
		"levels/Jungle_vegetation.txt"
	};
	Scene* Jungle1 = new Level(jungle_layers, player, 10, 10);

	addScene("Jungle1", Jungle1);
	addScene("menu", baseMenu);
	setCurrentScene("Jungle1");
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
	if (key == GLFW_KEY_ESCAPE) // Escape code
		bPlay = false;
	else if (key == GLFW_KEY_Z) // Change menu
		setCurrentScene("menu");
	else if (key == GLFW_KEY_X) // Change sprite
		player->setSpriteSheet("images/characters/ninja_blue/SpriteSheet.png");
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
		currentScene->init();
	}
}

Scene* Game::getCurrentScene() const
{
	return currentScene;
}

Player* Game::getPlayer() const
{
	return player;
}



