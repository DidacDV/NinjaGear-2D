#ifndef _INTERPOLATED_TILE_MAP_INCLUDE
#define _INTERPOLATED_TILE_MAP_INCLUDE

#include "TileMap.h"
#include <vector>

class InterpolatedTileMap {
public:
    static TileMap* createComposite(const std::vector<TileMap*>& layers);

private:
    static bool isAnyTileBlocked(int x, int y, const std::vector<TileMap*>& layers);
};

#endif