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

	mouseX = 0;
	mouseY = 0;

	/*UI MANAGER*/
	this->uiManager = new UIManager();
	this->uiManager->init();
	/* PLAYER */
	this->player = new Player();
	this->player->setSpriteSheet("images/characters/ninja_dark/SpriteSheet.png");
	
	Menu* startMenu = new Menu(MenuType::START);
	startMenu->setMenuImage("images/StartMenu.png");
	Menu* settingsMenu = new Menu(MenuType::SETTINGS);
	settingsMenu->setMenuImage("images/SettingsMenu.png");

	/* LEVELS */
	// Outside layers
	vector<string> outside_layers = { 
		"levels/Outside_background.txt",
		"levels/Outside_decoration.txt"
	};
	// Enemy configurations for the jungle level
	// Usamos push_back con cast explícito para asegurarnos que MeleeEnemy* sea tratado como Enemy*
	vector<EnemyConfig> jungleEnemies;
	jungleEnemies.reserve(3);
	jungleEnemies.push_back(EnemyConfig{35, 10, "images/enemies/cyclope.png", EnemyType::RANGED });
	jungleEnemies.push_back(EnemyConfig{10, 5,  "images/enemies/cyclope.png", EnemyType::MELEE });
	jungleEnemies.push_back(EnemyConfig{5,  5,  "images/enemies/cyclope.png", EnemyType::BASE });

	Level* outside = new Level(outside_layers, player, 10, 10, jungleEnemies);

	addScene("outside", outside);
	addScene("startMenu", startMenu);
	addScene("settings", settingsMenu);
	setCurrentScene("startMenu");
}

bool Game::update(int deltaTime)
{
	if (currentScene != NULL)
		currentScene->update(deltaTime);

	if (uiManager != NULL)
		uiManager->update(deltaTime, player);

	return bPlay;
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render the current scene (level in top 90%)
	if (currentScene != NULL)
		currentScene->render();

	//only render UI if we're not in a menu
	Menu* menu = dynamic_cast<Menu*>(currentScene);
	if (menu == NULL && uiManager != NULL) {
		uiManager->render();
	}
}

void Game::keyPressed(int key)
{
	if (key == GLFW_KEY_ESCAPE) {
		// If in game, go back to menu
		Menu* menu = dynamic_cast<Menu*>(currentScene);
		if (menu == NULL) {
			setCurrentScene("startMenu");
		}
		else {
			if (menu->getType() == MenuType::SETTINGS) {
				setCurrentScene("startMenu");
			}
			else if (menu->getType() == MenuType::START) {
				bPlay = false; //exit if in menu
			}
		}
	}
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
	mouseX = x;
	mouseY = y;
}

void Game::mousePress(int button)
{
	cout << mouseX << " " << mouseY << endl;
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		// Check if we're in a menu and handle the click
		Menu* menu = dynamic_cast<Menu*>(currentScene);
		if (menu != NULL) {
			menu->handleClick(mouseX, mouseY);
		}
	}
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

glm::vec2 Game::getPlayerPosition() const {
	return player->getPosition();
}

