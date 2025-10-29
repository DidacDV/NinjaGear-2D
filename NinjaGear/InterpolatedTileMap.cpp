#include "InterpolatedTileMap.h"
#include <fstream>
#include <sstream>

TileMap* InterpolatedTileMap::createComposite(const std::vector<TileMap*>& layers) {
    if (layers.empty() || !layers[0]) {
        return nullptr;
    }

    TileMap* baseLayer = layers[0];

    std::string tempFile = "temp_composite_map.txt";
    std::ofstream file(tempFile);

    file << "TILEMAP\n";
    file << baseLayer->mapSize.x << " " << baseLayer->mapSize.y << "\n";
    file << baseLayer->getTileSize() << " " << baseLayer->getTileSize() << "\n";
    file << "images/bricks.png\n";
    file << "40 40\n";

    for (int y = 0; y < baseLayer->mapSize.y; y++) {
        for (int x = 0; x < baseLayer->mapSize.x; x++) {
            int tileValue = -1;

            if (isAnyTileBlocked(x, y, layers)) {
                for (TileMap* layer : layers) {
                    if (layer && layer->isTileBlocked(x, y)) {
                        tileValue = layer->getTileAt(x, y);
                        break;
                    }
                }
            }
            else {
                for (int i = layers.size() - 1; i >= 0; i--) {
                    if (layers[i]) {
                        int tile = layers[i]->getTileAt(x, y);
                        if (tile >= 0) {
                            tileValue = tile;
                            break;
                        }
                    }
                }
            }

            file << tileValue;
            if (x < baseLayer->mapSize.x - 1) {
                file << " ";
            }
        }
        file << "\n";
    }

    file.close();

    ShaderProgram tempProgram;
    TileMap* composite = TileMap::createTileMap(tempFile, glm::vec2(0, 0), tempProgram);

    std::remove(tempFile.c_str());

    return composite;
}

bool InterpolatedTileMap::isAnyTileBlocked(int x, int y, const std::vector<TileMap*>& layers) {
    for (TileMap* layer : layers) {
        if (layer && layer->isTileBlocked(x, y)) {
            return true;
        }
    }
    return false;
}