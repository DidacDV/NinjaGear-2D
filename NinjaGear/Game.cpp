#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Game.h"
#include "MeleeEnemy.h"
#include "RangedEnemy.h"
#include "MovingStatue.h"
#include <iostream>


void Game::init(int screenWidth, int screenHeight)
{
	bPlay = true;
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	/*UI MANAGER*/
	this->uiManager = new UIManager();
	this->uiManager->init();
	/* PLAYER */
	this->player = new Player();
	this->player->setSpriteSheet("images/characters/ninja_dark/SpriteSheet.png");
	
	vector<string> titlesScreenMaps = { "levels/base_menu.txt" };
	Scene* baseMenu = new Menu(titlesScreenMaps);
	/* LEVELS */

	/* ------------- */
	/*  JUNGLE LEVEL */
	/* ------------- */
	vector<string> jungle_layers = { 
		"levels/Outside_background.txt",
		"levels/Outside_decoration.txt"
	};

	vector<EnemyConfig> jungleEnemies;
	jungleEnemies.push_back(EnemyConfig{35, 10, "images/enemies/cyclope.png", static_cast<Enemy*>(new RangedEnemy())});
	jungleEnemies.push_back(EnemyConfig{10, 5,  "images/enemies/cyclope.png", static_cast<Enemy*>(new MeleeEnemy())});

	vector<MovingObjectConfig> jungleObjects;

	vector<MusicConfig> jungleMusic;
	jungleMusic.push_back(MusicConfig{ 0, 0, "sounds/village.wav" });
	jungleMusic.push_back(MusicConfig{ 2, 0, "sounds/village.wav" });
	jungleMusic.push_back(MusicConfig{ 2, 1, "sounds/punch.wav" });
	Level* Jungle1 = new Level(jungle_layers, player, 10, 10, jungleEnemies, jungleObjects, jungleMusic);

	addScene("Jungle1", Jungle1);

	/* ------------- */
	/* DUNGEON LEVEL */
	/* ------------- */
	vector<string> dungeon_layers = {
		"levels/dungeon/Dungeon_Background.txt",
		"levels/dungeon/Dungeon_walls.txt",
		"levels/dungeon/Dungeon_Overlayed.txt"
	};

	vector<EnemyConfig> dungeonEnemies;

	vector<MovingObjectConfig> dungeonObjects;
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
	vector<MusicConfig> dungeonMusic;
	Level* Dungeon = new Level(dungeon_layers, player, 17, 38, dungeonEnemies, dungeonObjects, dungeonMusic);
	addScene("dungeon", Dungeon);


	setCurrentScene("Jungle1");
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

	if (uiManager != NULL)
		uiManager->render();

	//// Render the menu UI (bottom 10%)
	//auto it = levels.find("menu");
	//if (it != levels.end()) {
	//	Scene* menuScene = it->second;
	//	if (menuScene != nullptr) {
	//		menuScene->render();
	//	}
	//}
}

void Game::keyPressed(int key)
{
	if (key == GLFW_KEY_ESCAPE) // Escape code
		bPlay = false;
	else if (key == GLFW_KEY_Z) {
		if(currentScene == levels["dungeon"]) setCurrentScene("Jungle1");
		else setCurrentScene("dungeon");
	}
	else if (key == GLFW_KEY_X) // Change sprite
		player->setSpriteSheet("images/characters/ninja_blue/SpriteSheet.png");
	else if (key == GLFW_KEY_G) 
		player->onPunchKeyPressed();
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

glm::vec2 Game::getPlayerPosition() const {
	return player->getPosition();
}



