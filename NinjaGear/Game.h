#ifndef _GAME_INCLUDE
#define _GAME_INCLUDE


#include <GLFW/glfw3.h>
#include "Scene.h"
#include "Level.h"
#include "Menu.h"
#include "UIManager.h"
#include <map>
#include <string>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 320

// Game is a singleton (a class with a single instance) that represents our whole application

class Game
{

private:
	Game() {}
	
public:
	static Game &instance()
	{
		static Game G;
	
		return G;
	}
	void init(int screenHeight, int screenWidth);
	bool update(int deltaTime);
	void render();
	
	// Input callback methods
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMove(int x, int y);
	void mousePress(int button);
	void mouseRelease(int button);

	bool getKey(int key) const;

	//Level (scene) management
	void addScene(const string& name, Scene* scene);
	void setCurrentScene(const string& name);
	Scene* getCurrentScene() const;
	void victory();
	//Player management
	Player* getPlayer() const;
	glm::vec2 getPlayerPosition() const;
	Player* player;

private:
	bool bPlay; // Continue to play game?
	bool keys[GLFW_KEY_LAST+1]; // Store key states so that 
							    // we can have access at any time
	map<string, Scene*> levels;
	Scene* currentScene;
	int mouseX, mouseY;
};


#endif // _GAME_INCLUDE


