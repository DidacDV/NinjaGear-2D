#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "TileMap.h"


using namespace std;


TileMap *TileMap::createTileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program)
{
	TileMap *map = new TileMap(levelFile, minCoords, program);
	map->blockedTiles = unordered_set<int>(std::begin(BLOCKED_TILES), std::end(BLOCKED_TILES));
	return map;
}


TileMap::TileMap(const string &levelFile, const glm::vec2 &minCoords, ShaderProgram &program)
{
	loadLevel(levelFile);
	prepareArrays(minCoords, program);
}

TileMap::~TileMap()
{
	if(map != NULL)
		delete[] map;
}


void TileMap::render() const
{
	glEnable(GL_TEXTURE_2D);
	tilesheet.use();
	glBindVertexArray(vao);
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texCoordLocation);
	glDrawArrays(GL_TRIANGLES, 0, 6 * nTiles);
	glDisable(GL_TEXTURE_2D);
}

void TileMap::free()
{
	glDeleteBuffers(1, &vbo);
}

bool TileMap::loadLevel(const string &levelFile)
{
	ifstream fin;
	string line, tilesheetFile;
	stringstream sstream;

	fin.open(levelFile.c_str());
	if(!fin.is_open())
		return false;
	getline(fin, line);
	if(line.compare(0, 7, "TILEMAP") != 0)
		return false;
	getline(fin, line);
	sstream.str(line);
	sstream >> mapSize.x >> mapSize.y;
	sstream.clear();
	getline(fin, line);
	sstream.str(line);
	sstream >> tileSize >> blockSize;
	sstream.clear();
	getline(fin, line);
	sstream.str(line);
	sstream >> tilesheetFile;
	tilesheet.loadFromFile(tilesheetFile, TEXTURE_PIXEL_FORMAT_RGBA);
	tilesheet.setWrapS(GL_CLAMP_TO_EDGE);
	tilesheet.setWrapT(GL_CLAMP_TO_EDGE);
	tilesheet.setMinFilter(GL_NEAREST);
	tilesheet.setMagFilter(GL_NEAREST);
	sstream.clear();
	getline(fin, line);
	sstream.str(line);
	sstream >> tilesheetSize.x >> tilesheetSize.y;
	tileTexSize = glm::vec2(1.f / tilesheetSize.x, 1.f / tilesheetSize.y);
	
	map = new int[mapSize.x * mapSize.y];
	for (int j = 0; j < mapSize.y; j++)
	{
		for (int i = 0; i < mapSize.x; i++)
		{
			fin >> map[j * mapSize.x + i];
		}
	}
	fin.close();
	
	return true;
}

void TileMap::prepareArrays(const glm::vec2& minCoords, ShaderProgram& program)
{
	int tile;
	glm::vec2 posTile, texCoordTile[2];
	vector<float> vertices;

	nTiles = 0;

	for (int j = 0; j < mapSize.y; j++)
	{
		for (int i = 0; i < mapSize.x; i++)
		{
			tile = map[j * mapSize.x + i];
			if (tile < 0) continue;          //empty tile if -1
			if (tile >= tilesheetSize.x * tilesheetSize.y) continue;

			nTiles++;

			posTile = glm::vec2(minCoords.x + i * tileSize, minCoords.y + j * tileSize);

			//0 based with padding to delete green lines
			texCoordTile[0] = glm::vec2(
				float(tile % tilesheetSize.x) / tilesheetSize.x,
				float(tile / tilesheetSize.x) / tilesheetSize.y
			);
			texCoordTile[1] = texCoordTile[0] + tileTexSize;

			//first tri
			vertices.insert(vertices.end(), {
				posTile.x, posTile.y, texCoordTile[0].x, texCoordTile[0].y,
				posTile.x + blockSize, posTile.y, texCoordTile[1].x, texCoordTile[0].y,
				posTile.x + blockSize, posTile.y + blockSize, texCoordTile[1].x, texCoordTile[1].y,

				//second tri
				posTile.x, posTile.y, texCoordTile[0].x, texCoordTile[0].y,
				posTile.x + blockSize, posTile.y + blockSize, texCoordTile[1].x, texCoordTile[1].y,
				posTile.x, posTile.y + blockSize, texCoordTile[0].x, texCoordTile[1].y
				});
		}
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
	posLocation = program.bindVertexAttribute("position", 2, 4 * sizeof(float), 0);
	texCoordLocation = program.bindVertexAttribute("texCoord", 2, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

bool TileMap::isTileBlocked(int x, int y) const
{
	int tileId = map[y * mapSize.x + x];
	return blockedTiles.find(tileId) != blockedTiles.end();
}

void TileMap::addBlockedTiles(const vector<int> tilesToBlock, int count) {
	for (int i = 0; i < count; ++i) {
		blockedTiles.insert(tilesToBlock[i]);
	}
}

// Collision tests for axis aligned bounding boxes.
// Method collisionMoveDown also corrects Y coordinate if the box is
// already intersecting a tile below.

bool TileMap::collisionMoveLeft(const glm::ivec2& pos, const glm::ivec2& size) const
{
	int x = pos.x / tileSize;
	int y0 = pos.y / tileSize;
	int y1 = (pos.y + size.y - 1) / tileSize;

	for (int y = y0; y <= y1; y++)
	{
		if (isTileBlocked(x, y))
			return true;
	}

	return false;
}

bool TileMap::collisionMoveRight(const glm::ivec2& pos, const glm::ivec2& size) const
{
	int x = pos.x / tileSize;

	int y0 = pos.y / tileSize;
	int y1 = (pos.y + size.y - 1) / tileSize;

	for (int y = y0; y <= y1; y++)
	{
		if (isTileBlocked(x, y))
			return true;
	}

	return false;
}

bool TileMap::collisionMoveDown(const glm::ivec2& pos, const glm::ivec2& size) const
{
	int x0 = pos.x / tileSize;
	int x1 = (pos.x + size.x - 1) / tileSize;
	int y = (pos.y + size.y - 1) / tileSize;

	for (int x = x0; x <= x1; x++)
	{
		if (isTileBlocked(x, y))
			return true;
	}
	return false;
}

bool TileMap::collisionMoveUp(const glm::ivec2& pos, const glm::ivec2& size) const
{
	int x0 = pos.x / tileSize;
	int x1 = (pos.x + size.x - 1) / tileSize;
	int y = pos.y / tileSize;

	for (int x = x0; x <= x1; x++)
	{
		if (isTileBlocked(x, y))
			return true;
	}

	return false;
}