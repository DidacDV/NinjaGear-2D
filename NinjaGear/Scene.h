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
	Scene(const vector<string>& tileMapFiles);
	virtual ~Scene();

	virtual void init();
	virtual void update(int deltaTime);
	virtual void render();

protected:
	void initShaders();
	void loadTileMaps();
	vector<string> mapFiles;
	vector<TileMap*> maps;
	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;

};


#endif // _SCENE_INCLUDE

