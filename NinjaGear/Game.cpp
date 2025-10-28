#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Game.h"
#include "MeleeEnemy.h"
#include "RangedEnemy.h"
#include "MovingStatue.h"
#include "ServiceLocator.h"
#include <iostream>


void Game::init(int screenWidth, int screenHeight)
{
	bPlay = true;
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	mouseX = 0;
	mouseY = 0;

	/*UI MANAGER, same instance used in multiple classes*/ //todo singleton?
	UIManager* GLOBAL_UI_MANAGER = new UIManager();
	GLOBAL_UI_MANAGER->init();
	ServiceLocator::provide(GLOBAL_UI_MANAGER);
	/* PLAYER */
	this->player = new Player();
	this->player->setSpriteSheet("images/characters/ninja_dark/SpriteSheet.png");
	
	Menu* startMenu = new Menu(MenuType::START);
	startMenu->setMenuImage("images/StartMenu.png");

	Menu* settingsMenu = new Menu(MenuType::SETTINGS);
	settingsMenu->setMenuImage("images/SettingsMenu.png");

	Menu* deathMenu = new Menu(MenuType::DEATH);
	deathMenu->setMenuImage("images/DeathMenu.png");

	Menu* winMenu = new Menu(MenuType::CREDITS);
	winMenu->setMenuImage("images/WinMenu.png");
	

	addScene("menu", startMenu);
	addScene("settings",settingsMenu);
	addScene("death",deathMenu);
	addScene("win", winMenu);


	/* ------------- */
	/*  JUNGLE LEVEL */
	/* ------------- */
	vector<string> jungle_layers = { 
		"levels/Outside_background.txt",
		"levels/Outside_decoration.txt"
	};

	vector<EnemyConfig> jungleEnemies;
	jungleEnemies.push_back(EnemyConfig{35, 8, "images/enemies/miniFlame.png", EnemyType ::MELEE});
	//jungleEnemies.push_back(EnemyConfig{15, 10, "images/enemies/larvaTank.png", EnemyType ::TANK});
//	jungleEnemies.push_back(EnemyConfig{ 10, 5,  "images/enemies/cyclope.png", EnemyType::RANGED });

	vector<MovingObjectConfig> jungleObjects;

	vector<MusicConfig> jungleMusic;
	//jungleMusic.push_back(MusicConfig{ 0, 0, "sounds/village.wav" });
	//jungleMusic.push_back(MusicConfig{ 2, 0, "sounds/village.wav" });
	//jungleMusic.push_back(MusicConfig{ 2, 1, "sounds/punch.wav" });
	Level* Jungle1 = new Level(jungle_layers, player, 10, 10, jungleEnemies, jungleObjects, jungleMusic, LevelType::OUTSIDE);
	addScene("outside", Jungle1);

	/* ------------- */
	/* DUNGEON LEVEL */
	/* ------------- */
	vector<string> dungeon_layers = {
		"levels/dungeon/Dungeon_Background.txt",
		"levels/dungeon/Dungeon_walls.txt",
		"levels/dungeon/Dungeon_Overlayed.txt"
	};

	vector<EnemyConfig> dungeonEnemies;
	dungeonEnemies.push_back(EnemyConfig{ 10, 41,  "images/boss/flame.png", EnemyType::BOSS });

	vector<MovingObjectConfig> dungeonObjects;
	// AT ROOM
	dungeonObjects.push_back(MovingObjectConfig{
		glm::vec2(368.0f, 720.0f),            // startPos
		glm::vec2(576.0f, 720.0f),           // endPos
		500.0f,                              // speed
		MovingObjectType::MOVING_STATUE,
		"images/statue.png",				 // spriteSheet
		glm::vec2(32.f, 47.f),				 // Actual statue size
		glm::vec2(1.0f, 1.0f),
	});
	dungeonObjects.push_back(MovingObjectConfig{
		glm::vec2(576.0f, 784.0f),           // endPos
		glm::vec2(368.0f, 784.0f),            // startPos
		500.0f,                              // speed
		MovingObjectType::MOVING_STATUE,
		"images/statue.png",				 // spriteSheet
		glm::vec2(32.f, 47.f),				 // Actual statue size
		glm::vec2(1.0f, 1.0f),
	});
	dungeonObjects.push_back(MovingObjectConfig{
		glm::vec2(368.0f, 848.0f),            // startPos
		glm::vec2(576.0f, 848.0f),           // endPos
		500.0f,                              // speed
		MovingObjectType::MOVING_STATUE,
		"images/statue.png",				 // spriteSheet
		glm::vec2(32.f, 47.f),				 // Actual statue size
		glm::vec2(1.0f, 1.0f),
	});
	// AT BOSS ROOM
	dungeonObjects.push_back(MovingObjectConfig{
		glm::vec2(48.0f, 800.0f),            // startPos
		glm::vec2(112.0f, 800.0f),           // endPos
		500.0f,                              // speed
		MovingObjectType::MOVING_STATUE,
		"images/statue.png",				 // spriteSheet
		glm::vec2(32.f, 47.f),				 // Actual statue size
		glm::vec2(1.0f, 1.0f),
		});
	dungeonObjects.push_back(MovingObjectConfig{
		glm::vec2(48.0f, 896.0f),            // startPos
		glm::vec2(112.0f, 896.0f),           // endPos
		500.0f,                              // speed
		MovingObjectType::MOVING_STATUE,
		"images/statue.png",				 // spriteSheet
		glm::vec2(32.f, 47.f),				 // Actual statue size
		glm::vec2(1.0f, 1.0f),
		});
	vector<MusicConfig> dungeonMusic;
	Level* Dungeon = new Level(dungeon_layers, player, 17, 38, dungeonEnemies, dungeonObjects, dungeonMusic, LevelType::DUNGEON);
	addScene("dungeon", Dungeon);


	setCurrentScene("menu");
}

bool Game::update(int deltaTime)
{
	if (player->getHealth() == 0) {
		setCurrentScene("death");

		player->setHealth(player->getMaxHealth());
	}

	if (currentScene != NULL)
		currentScene->update(deltaTime);

	ServiceLocator::getUI().update(deltaTime, player);

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
	if (menu == NULL) {
		ServiceLocator::getUI().render();
		ServiceLocator::getUI().renderGameOverlay();
	}
}

void Game::keyPressed(int key)
{
	if (key == GLFW_KEY_ESCAPE) {
		// If in game, go back to menu
		Menu* menu = dynamic_cast<Menu*>(currentScene);
		if (menu == NULL) {
			setCurrentScene("menu");
		}
		else {
			if (menu->getType() == MenuType::SETTINGS) {
				setCurrentScene("menu");
			}
			else if (menu->getType() == MenuType::START) {
				bPlay = false; //exit if in menu
			}
		}
	}
	//interior tp cheat
	//interior tp cheat
	else if (key == GLFW_KEY_K) {
		if (currentScene == levels["dungeon"]) setCurrentScene("Jungle1");
		else {
			setCurrentScene("dungeon");
			ServiceLocator::getUI().showTemporaryMessage("INTERIOR TP ACTIVATED",
				glm::vec2(320, 40), 1.f, glm::vec3(0.f, 0.f, 0.f), 1500);
		}
	}
	else if (key == GLFW_KEY_Z)
		player->onPunchKeyPressed();
	//items cheat
	else if (key == GLFW_KEY_I) {
		player->giveAllItems();
		ServiceLocator::getUI().showTemporaryMessage("ALL ITEMS CHEAT ACTIVATED",
			glm::vec2(320, 40), 1.f, glm::vec3(0.f, 0.f, 0.f), 1500);
	}
	//heal cheat
	else if (key == GLFW_KEY_H) {
		player->setHealth(player->getMaxHealth());
		ServiceLocator::getUI().showTemporaryMessage("FULL HEAL ACTIVATED",
			glm::vec2(320, 40), 1.f, glm::vec3(0.f, 0.f, 0.f), 1500);
	}
	//god mode cheat
	else if (key == GLFW_KEY_G) {
		player->toggleGodMode();
		string message = player->isGodModeOn() ? "GOD MODE ACTIVATED" : "GOD MODE DEACTIVATED";
		ServiceLocator::getUI().showTemporaryMessage(message,
			glm::vec2(320, 40), 1.f, glm::vec3(0.f, 0.f, 0.f), 1500);
	}
	//boss tp cheat
	else if (key == GLFW_KEY_B) {
		setCurrentScene("dungeon");
		int tileSize = 16.0f;
		int targetTileX = 10; // Change to the tile X coordinate you want
		int targetTileY = 51; // Change to the tile Y coordinate you want

		glm::vec2 newPos(targetTileX * tileSize, targetTileY * tileSize);
		player->setPosition(newPos);
		ServiceLocator::getUI().showTemporaryMessage("BOSS TP ACTIVATED",
			glm::vec2(320, 40), 1.f, glm::vec3(0.f, 0.f, 0.f), 1500);
	}
		


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

void Game::victory() {
	setCurrentScene("win");
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

