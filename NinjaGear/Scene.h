#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"


// Scene contains all the entities of our game.
// It is responsible for updating and render them.


class Scene
{

public:
	Scene();
	Scene(const vector<string>& tileMapFiles, Player* player);
	~Scene();

	void init();
	void update(int deltaTime);
	void render();

private:
	void initShaders();
	void loadTileMaps();

private:
	vector<string> mapFiles;
	vector<TileMap*> maps;
	Player *player;
	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;

};


#endif // _SCENE_INCLUDE

