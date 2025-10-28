#pragma once
#include "Enemy.h"
#include <vector>

class ShaderProgram;
class TileMap;
class ProjectileManager;
struct EnemyConfig;

class EnemyFactory {
public:
    static Enemy* create(
        const EnemyConfig& config,
        const glm::ivec2& screenPos,
        ShaderProgram& texProgram,
        TileMap* map,
        const vector<TileMap*>& maps,
        ProjectileManager* projectileManager);
};

