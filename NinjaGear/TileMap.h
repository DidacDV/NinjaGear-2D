#ifndef _TILE_MAP_INCLUDE
#define _TILE_MAP_INCLUDE


#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"
#include <unordered_set>

// Class Tilemap is capable of loading a tile map from a text file in a very
// simple format (see level01.txt for an example). With this information
// it builds a single VBO that contains all tiles. As a result the render
// method draws the whole map independently of what is visible.

const int BLOCKED_TILES[] = { 
							  0, 1, 22, 44, 45,       // DESERT
							  //199, 198, 176, 155, 154, 178, 182, 198, 200 //JUNGLE
							  - 1
							};

class TileMap
{
	static constexpr int SCREEN_WIDTH_TILES = 20;
	static constexpr int SCREEN_HEIGHT_TILES = 20;

private:
	TileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program);

public:
	// Tile maps can only be created inside an OpenGL context
	static TileMap *createTileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program);

	~TileMap();

	void render() const;
	void free();
	
	int getTileSize() const { return tileSize; }

	bool collisionMoveLeft(const glm::ivec2 &pos, const glm::ivec2 &size) const;
	bool collisionMoveRight(const glm::ivec2 &pos, const glm::ivec2 &size) const;
	bool collisionMoveDown(const glm::ivec2 &pos, const glm::ivec2 &size) const;
	bool collisionMoveUp(const glm::ivec2& pos, const glm::ivec2& size) const;
	//NEW FUNCTIONS
	void addBlockedTiles(const vector<int> tilesToBlock, int count);

	glm::ivec2 mapSize;

	int width() const { return mapSize.x; }
	int height() const { return mapSize.y; }

private:
	bool loadLevel(const string &levelFile);
	void prepareArrays(const glm::vec2 &minCoords, ShaderProgram &program);
	bool isTileBlocked(int x, int y) const;

private:
	GLuint vao;
	GLuint vbo;
	GLint posLocation, texCoordLocation;
	int nTiles;
	glm::ivec2 position, tilesheetSize;
	int tileSize, blockSize;
	Texture tilesheet;
	glm::vec2 tileTexSize;
	int *map;
	std::unordered_set<int> blockedTiles;

};


#endif // _TILE_MAP_INCLUDE


