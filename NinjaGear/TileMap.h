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
								5400, 5500, 2918,
								//SCREEN 3
								5390, 5490, 4693, 4694 ,4695, 4793, 4794, 4795, 4893, 4894, 4895, 4993, 4994, 4995, 5093,  5094,
								5095, 2811, 2812, 2813, 5729,  5829, 5929, 5731, 5831, 593, 5924, 5925, 6024, 6025, 5666, 5667, 5766, 5767,
								//SCREEN 4
								5130, 5030, 5926, 688, 689, 5128, 5133, 6026, 789, 788, 5661, 5660, 5673, 5674, 5760, 5761, 5132,  5029,
								691, 690, 791, 790, 4648, 4649, 4650, 4748, 4750, 4848, 4850, 4949, 4949, 4950, 6021, 5131, 5027, 5028,
								5127, 5128, 5720,  5721, 5820, 5821, 5920, 5921, 5726, 5826, 4427, 4428, 4429, 4430, 4327, 4328, 4329,
								4330, 4227, 4228, 4229, 4230,
								//SCREEN 5
								5027, 5156, 5157, 5557, 5556, 5657, 5656, 5757, 5756, 5634, 5734, 4962, 4963, 4964, 5062, 5063, 5064,
								5065, 5162,  5163,  5164, 5262, 5263, 5264, 5265, 4625, 4626, 4956 , 4957, 5056, 5057, 5658, 5659, 5758,
								5759, 6241, 6231, 5550, 5551,  5650,  5651, 5719, 5819, 5919, 388, 389, 288, 289, 5128,
								// SCREEN 6
								5070, 5071, 5072, 5073, 5074, 5075, 5171, 5172, 5173, 5174, 5175, 5170, 5270, 5271, 5272, 5273, 5274,
								5275, 5370, 5371, 5372, 5470, 5471, 5472, 5570, 5571, 5572, 5671, 5048, 5049, 5148, 5149, 5258, 5259,
								5260, 5261, 5358, 5359, 5360, 5361, 5562, 5563, 5458, 5459, 5460, 5461, 5663, 5558, 5560,5106, 5006,
								3001, 2802, 2802, 2900, 2901, 2802,
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


