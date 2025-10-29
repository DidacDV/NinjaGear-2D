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
	
	Menu* startMenu = new Menu(MenuType::START, "sounds/music/ninja.mp3");
	startMenu->setMenuImage("images/StartMenu.png");

	Menu* settingsMenu = new Menu(MenuType::SETTINGS);
	settingsMenu->setMenuImage("images/SettingsMenu.jpg");

	Menu* deathMenu = new Menu(MenuType::DEATH, "sounds/music/defeat.mp3");
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
	jungleEnemies.push_back(EnemyConfig{32, 13, "images/enemies/miniFlame.png", EnemyType ::MELEE, 4});
	jungleEnemies.push_back(EnemyConfig{54, 25, "images/enemies/miniFlame.png", EnemyType ::MELEE, 5});
	jungleEnemies.push_back(EnemyConfig{56, 46, "images/enemies/miniFlame.png", EnemyType ::MELEE, 4});
	jungleEnemies.push_back(EnemyConfig{56, 49, "images/enemies/miniFlame.png", EnemyType ::MELEE, 0});
	jungleEnemies.push_back(EnemyConfig{45, 67, "images/enemies/miniFlame.png", EnemyType ::MELEE, 1});
	jungleEnemies.push_back(EnemyConfig{33, 4, "images/enemies/larvaTank.png", EnemyType ::TANK, 3});
	jungleEnemies.push_back(EnemyConfig{35, 4, "images/enemies/larvaTank.png", EnemyType ::TANK, 3});
	jungleEnemies.push_back(EnemyConfig{55, 30, "images/enemies/larvaTank.png", EnemyType ::TANK, 1});
	jungleEnemies.push_back(EnemyConfig{54, 11, "images/enemies/larvaTank.png", EnemyType ::TANK, 4});
	jungleEnemies.push_back(EnemyConfig{54, 55, "images/enemies/larvaTank.png", EnemyType ::TANK, 4});
	jungleEnemies.push_back(EnemyConfig{54, 7,  "images/enemies/miniFlame.png", EnemyType::MELEE, 2});

	vector<MovingObjectConfig> jungleObjects;

	vector<MusicConfig> jungleMusic;
	jungleMusic.push_back(MusicConfig{ 0, 0, "sounds/music/jungle.mp3" });
	jungleMusic.push_back(MusicConfig{ 2, 0, "sounds/music/jungle.mp3" });
	jungleMusic.push_back(MusicConfig{ 2, 1, "sounds/music/desert.mp3" });
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
	dungeonEnemies.push_back(EnemyConfig{ 13, 25, "images/enemies/eye.png", EnemyType::RANGED, 1 });
	dungeonEnemies.push_back(EnemyConfig{ 17, 18, "images/enemies/eye.png", EnemyType::RANGED, 3 });
	dungeonEnemies.push_back(EnemyConfig{ 13, 12, "images/enemies/eye.png", EnemyType::RANGED, 3 });
	dungeonEnemies.push_back(EnemyConfig{ 6, 8, "images/enemies/larvaTank.png", EnemyType::TANK, 2 });
	dungeonEnemies.push_back(EnemyConfig{ 9, 14, "images/enemies/larvaTank.png", EnemyType::TANK, 2 });
	dungeonEnemies.push_back(EnemyConfig{ 6, 19, "images/enemies/larvaTank.png", EnemyType::TANK, 2 });
	dungeonEnemies.push_back(EnemyConfig{ 24, 13, "images/enemies/larvaTank.png", EnemyType::TANK, 2 });
	//pre-pre-boss lvl
	dungeonEnemies.push_back(EnemyConfig{ 38, 70, "images/enemies/miniFlame.png", EnemyType::MELEE, 7 });
	dungeonEnemies.push_back(EnemyConfig{ 32, 76, "images/enemies/larvaTank.png", EnemyType::TANK, 1 });
	dungeonEnemies.push_back(EnemyConfig{ 29, 76, "images/enemies/larvaTank.png", EnemyType::TANK, 1 });
	dungeonEnemies.push_back(EnemyConfig{ 29, 65, "images/enemies/eye.png", EnemyType::RANGED, 5 });
	dungeonEnemies.push_back(EnemyConfig{ 30, 65, "images/enemies/eye.png", EnemyType::RANGED, 5 });
	//pre-boss lvl
	dungeonEnemies.push_back(EnemyConfig{ 4, 69, "images/enemies/miniFlameBlue.png", EnemyType::MELEE, 7 });
	dungeonEnemies.push_back(EnemyConfig{ 6, 69, "images/enemies/larvaTankBlue.png", EnemyType::TANK, 7 });
	dungeonEnemies.push_back(EnemyConfig{ 15, 69, "images/enemies/miniFlameBlue.png", EnemyType::MELEE, 7 });
	dungeonEnemies.push_back(EnemyConfig{ 13, 69, "images/enemies/larvaTankBlue.png", EnemyType::TANK, 7 });


	dungeonEnemies.push_back(EnemyConfig{ 9, 41,  "images/boss/flame.png", EnemyType::BOSS });

	vector<MovingObjectConfig> dungeonObjects;
	dungeonObjects.push_back(MovingObjectConfig{
		glm::vec2(336.0f, 720.0f),
		glm::vec2(592.0f, 720.0f),
		500.0f,
		MovingObjectType::MOVING_STATUE,
		"images/statue.png",
		glm::vec2(32.f, 47.f),
		glm::vec2(1.0f, 1.0f),
		true,
		1,
		});
	dungeonObjects.push_back(MovingObjectConfig{
		glm::vec2(592.0f, 784.0f),
		glm::vec2(336.0f, 784.0f),
		500.0f,
		MovingObjectType::MOVING_STATUE,
		"images/statue.png",
		glm::vec2(32.f, 47.f),
		glm::vec2(1.0f, 1.0f),
		true,
		1,
		});
	dungeonObjects.push_back(MovingObjectConfig{
		glm::vec2(336.0f, 848.0f),
		glm::vec2(592.0f, 848.0f),
		500.0f,
		MovingObjectType::MOVING_STATUE,
		"images/statue.png",
		glm::vec2(32.f, 47.f),
		glm::vec2(1.0f, 1.0f),
		true,
		1,
		});
	dungeonObjects.push_back(MovingObjectConfig{
		glm::vec2(48.0f, 816.0f),
		glm::vec2(240.0f, 816.0f),
		500.0f,
		MovingObjectType::MOVING_STATUE,
		"images/statue.png",
		glm::vec2(32.f, 47.f),
		glm::vec2(1.0f, 1.0f),
		true,
		1
		});
	dungeonObjects.push_back(MovingObjectConfig{
		glm::vec2(240.0f, 752.0f),
		glm::vec2(48.0f, 752.0f),
		500.0f,
		MovingObjectType::MOVING_STATUE,
		"images/statue.png",
		glm::vec2(32.f, 47.f),
		glm::vec2(1.0f, 1.0f),
		true,
		1
		});

	// SPIKES - alternating trap pattern
	createSpikeTraps(dungeonObjects, 25.0f, 10.0f, 10);
	createSpikeTraps(dungeonObjects, 25.0f, 12.0f, 10);
	createSpikeTraps(dungeonObjects, 25.0f, 14.0f, 10);
	createSpikeTraps(dungeonObjects, 25.0f, 16.0f, 10);
	//laberynth like lvl
	createSpikeTraps(dungeonObjects, 24.0f, 27.0f, 1);
	createSpikeTraps(dungeonObjects, 26.0f, 27.0f, 1);
	createSpikeTraps(dungeonObjects, 28.0f, 27.0f, 1);

	createSpikeTraps(dungeonObjects, 32.0f, 29.0f, 1);
	createSpikeTraps(dungeonObjects, 34.0f, 29.0f, 1);
	createSpikeTraps(dungeonObjects, 36.0f, 29.0f, 1);

	createSpikeTraps(dungeonObjects, 32.0f, 34.0f, 1);
	createSpikeTraps(dungeonObjects, 34.0f, 34.0f, 1);
	createSpikeTraps(dungeonObjects, 36.0f, 34.0f, 1);

	createSpikeTraps(dungeonObjects, 24.0f, 32.0f, 1);
	createSpikeTraps(dungeonObjects, 26.0f, 32.0f, 1);
	createSpikeTraps(dungeonObjects, 28.0f, 32.0f, 1);

	//pre-boss lvl
	createSpikeTraps(dungeonObjects, 7.0f, 61.0f, 6);


	vector<MusicConfig> dungeonMusic;
	dungeonMusic.push_back(MusicConfig{ 0, 0, "sounds/music/dungeon.mp3" });
	dungeonMusic.push_back(MusicConfig{ 0, 1, "sounds/music/dungeon.mp3" });
	dungeonMusic.push_back(MusicConfig{ 0, 2, "sounds/music/boss1.wav" });
	dungeonMusic.push_back(MusicConfig{ 0, 3, "sounds/music/dungeon.mp3" });

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
	else if (key == GLFW_KEY_K) {
		if (currentScene == levels["dungeon"]) setCurrentScene("dungeon");
		else {
			setCurrentScene("dungeon");
			ServiceLocator::getUI().showTemporaryMessage("INTERIOR TP ACTIVATED",
				glm::vec2(GameConfig::CENTER_X + 200, GameConfig::CENTER_Y - 200), 1.f, glm::vec3(0.f, 0.f, 0.f), 1500);
		}
		ServiceLocator::getAudio().playSound("sounds/teleport.wav");
	}
	else if (key == GLFW_KEY_Z)
		player->onPunchKeyPressed();
	//items cheat
	else if (key == GLFW_KEY_I) {
		player->giveAllItems();
		ServiceLocator::getUI().showTemporaryMessage("ALL ITEMS CHEAT ACTIVATED",
			glm::vec2(GameConfig::CENTER_X + 200, GameConfig::CENTER_Y - 200), 1.f, glm::vec3(0.f, 0.f, 0.f), 1500);
	}
	//heal cheat
	else if (key == GLFW_KEY_H) {
		player->setHealth(player->getMaxHealth());
		int randomHit = (rand() % 8) + 1;
		std::string soundPath = "sounds/Cure" + std::to_string(randomHit) + ".wav";
		ServiceLocator::getAudio().playSound(soundPath.c_str());
		ServiceLocator::getUI().showTemporaryMessage("FULL HEAL ACTIVATED",
			glm::vec2(GameConfig::CENTER_X + 200, GameConfig::CENTER_Y - 200), 1.f, glm::vec3(0.f, 0.f, 0.f), 1500);
	}
	//god mode cheat
	else if (key == GLFW_KEY_G) {
		player->toggleGodMode();
		string message = player->isGodModeOn() ? "GOD MODE ACTIVATED" : "GOD MODE DEACTIVATED";
		ServiceLocator::getAudio().playSound("sounds/god_mode.mp3");
		ServiceLocator::getUI().showTemporaryMessage(message,
			glm::vec2(GameConfig::CENTER_X + 200, GameConfig::CENTER_Y - 200), 1.f, glm::vec3(0.f, 0.f, 0.f), 1500);
	}
	//boss tp cheat
	else if (key == GLFW_KEY_B) {
		setCurrentScene("dungeon");
		int tileSize = 16.0f;
		int targetTileX = 10; 
		int targetTileY = 51;

		glm::vec2 newPos(targetTileX * tileSize, targetTileY * tileSize);
		player->setPosition(newPos);
		ServiceLocator::getAudio().playSound("sounds/teleport.wav");
		ServiceLocator::getUI().showTemporaryMessage("BOSS TP ACTIVATED",
			glm::vec2(GameConfig::CENTER_X + 200, GameConfig::CENTER_Y - 200), 1.f, glm::vec3(0.f, 0.f, 0.f), 1500);
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
		currentScene->playMusic();
	}
}

void Game::victory() {
	ServiceLocator::getAudio().stopAllSounds();
	ServiceLocator::getAudio().playSound("sounds/victory.mp3");
	setCurrentScene("win");
	ServiceLocator::getAudio().playMusic("sounds/music/victory.mp3");
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

void Game::createSpikeTraps(vector<MovingObjectConfig>& objects, float startTileX, float tileY, int count)
{
	constexpr float TILE_SIZE = 16.0f;
	constexpr int SPIKE_IDLE_DURATION_MS = 500;
	constexpr int SPIKE_ACTIVE_DURATION_MS = 500;
	constexpr int SPIKE_TRANSITION_DURATION_MS = 500;
	const glm::vec2 SPIKE_SIZE(16.0f, 16.0f);
	const glm::vec2 SPIKE_ANCHOR(0.5f, 1.0f);

	const float startX = startTileX * TILE_SIZE;
	const float fixedY = tileY * TILE_SIZE;
	const int fullCycle = SPIKE_IDLE_DURATION_MS + SPIKE_TRANSITION_DURATION_MS +
		SPIKE_ACTIVE_DURATION_MS + SPIKE_TRANSITION_DURATION_MS;
	const int halfCycleDuration = fullCycle / 2;

	objects.reserve(objects.size() + count);

	for (int i = 0; i < count; ++i)
	{
		const float xPosition = startX + static_cast<float>(i) * TILE_SIZE;
		const int timerOffset = ((i / 2) % 2 == 0) ? 0 : halfCycleDuration;

		objects.push_back(MovingObjectConfig{
			glm::vec2(xPosition, fixedY),
			glm::vec2(xPosition, fixedY),
			0.0f,
			MovingObjectType::SPIKE_TRAP,
			"images/spikes.png",
			SPIKE_SIZE,
			SPIKE_ANCHOR,
			true,
			1,
			SPIKE_IDLE_DURATION_MS,
			SPIKE_ACTIVE_DURATION_MS,
			timerOffset
			});
	}
}