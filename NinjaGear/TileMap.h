#ifndef _TILE_MAP_INCLUDE
#define _TILE_MAP_INCLUDE


#include <glm/glm.hpp>
#include "Texture.h"
#include "ShaderProgram.h"
#include <unordered_set>
#include "GeometryUtils.h"

// Class Tilemap is capable of loading a tile map from a text file in a very
// simple format (see level01.txt for an example). With this information
// it builds a single VBO that contains all tiles. As a result the render
// method draws the whole map independently of what is visible.

//right now, only SCREEN 1  & SCREEN 2 tiles have been added (which can appear on other screens ofc) 
const int BLOCKED_TILES[] = { 
								711, 712, 811, 911, 912, 713, 813, 102, 100, 2233, 2234, 2235, 2335, 2435, 2434, 2433, 2333, 200, 201,
								5588, 5589, 5688, 5689, 51, 52, 151, 152, 5387, 5083, 5585, 5685, 5786, 5787, 5686, 5687, 5586, 5587,
								5785, 5595, 5596, 5597, 5598, 5495, 5496, 5497, 5498, 5396, 5397, 5398, 5295, 5296, 5297, 5298, 5196, 5197, 5683, 5684, 5583, 5584, 937, 938,
								5283, 5284, 5384,
								5180, 5200, 5182, 5181, 4981, 4887, 4888, 4889, 4789, 4788,
								4787, 4979, 4980, 5079, 5080, 4679, 4680, 4681, 4682, 4782, 3012, 2812, 4781, 4879, 4880, 4881, 4882, 3011, 4779, 2910,
								5400, 5500, 2918
							  //199, 198, 176, 155, 154, 178, 182, 198, 200 //JUNGLE
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
	bool hasLineOfSight(const glm::vec2& from, const glm::vec2& to) const;
	//NEW FUNCTIONS
	void addBlockedTiles(const vector<int> tilesToBlock, int count);

	glm::ivec2 mapSize;

	int width() const { return mapSize.x; }
	int height() const { return mapSize.y; }
	bool isTileBlocked(int x, int y) const;
private:
	bool loadLevel(const string &levelFile);
	void prepareArrays(const glm::vec2 &minCoords, ShaderProgram &program);

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


