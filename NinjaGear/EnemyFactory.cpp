#include "EnemyFactory.h"
#include "MeleeEnemy.h"
#include "RangedEnemy.h"
#include "TankEnemy.h"
#include "Boss.h"

Enemy* EnemyFactory::create(
    const EnemyConfig& config,
    const glm::ivec2& screenPos,
    ShaderProgram& texProgram,
    TileMap* map,
    const std::vector<TileMap*>& maps,
    ProjectileManager* projectileManager)
{
    Enemy* enemy = nullptr;

    switch (config.type) {
    case EnemyType::MELEE:
        enemy = new MeleeEnemy();
        break;
    case EnemyType::RANGED:
        enemy = new RangedEnemy();
        break;
    case EnemyType::TANK:
        enemy = new TankEnemy();
        break;
    case EnemyType::BOSS:
        enemy = new Boss();
        break;
    default:
        return nullptr;
    }

    if (enemy) {
        enemy->init(screenPos, texProgram, map, config.spriteSheet, maps);
        enemy->setPatrolDistance(config.patrolDistance);
        enemy->setPosition(glm::ivec2(config.xTile * map->getTileSize(), config.yTile * map->getTileSize()));
        enemy->setProjectileManager(projectileManager);
    }

    return enemy;
}