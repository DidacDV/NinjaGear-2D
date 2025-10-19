#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Game.h"
#include "MeleeEnemy.h"
#include "RangedEnemy.h"
#include <iostream>


void Game::init(int screenWidth, int screenHeight)
{
	bPlay = true;
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	/* PLAYER */
	this->player = new Player();
	this->player->setSpriteSheet("images/characters/ninja_dark/SpriteSheet.png");
	
	vector<string> titlesScreenMaps = { "levels/base_menu.txt" };
	Scene* baseMenu = new Menu(titlesScreenMaps);
	/* LEVELS */

	// Jungle level layers
	vector<string> jungle_layers = { 
		"levels/Outside_background.txt",
		"levels/Outside_decoration.txt"
	};
	// Enemy configurations for the jungle level
	// Usamos push_back con cast explícito para asegurarnos que MeleeEnemy* sea tratado como Enemy*
	vector<EnemyConfig> jungleEnemies;
	jungleEnemies.reserve(3);
	jungleEnemies.push_back(EnemyConfig{35, 10, "images/enemies/cyclope.png", static_cast<Enemy*>(new RangedEnemy())});
	jungleEnemies.push_back(EnemyConfig{10, 5,  "images/enemies/cyclope.png", static_cast<Enemy*>(new MeleeEnemy())});
	jungleEnemies.push_back(EnemyConfig{5,  5,  "images/enemies/cyclope.png", static_cast<Enemy*>(new Enemy())});

	Level* Jungle1 = new Level(jungle_layers, player, 10, 10, jungleEnemies);

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

	// Render the current scene (level in top 90%)
	if (currentScene != NULL)
		currentScene->render();

	// Render the menu UI (bottom 10%)
	auto it = levels.find("menu");
	if (it != levels.end()) {
		Scene* menuScene = it->second;
		if (menuScene != nullptr) {
			menuScene->render();
		}
	}
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

	auto it2 = levels.find("menu");
	if (it2 != levels.end()) {
		it2->second->init();
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

glm::vec2 Game::getPlayerPosition() const {
	return player->getPosition();
}



