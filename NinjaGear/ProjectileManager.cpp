#include "ProjectileManager.h"

void ProjectileManager::init(ShaderProgram* shaderProgram, TileMap* map)
{
    this->shaderProgram = shaderProgram;
    this->map = map;
}

void ProjectileManager::spawnProjectile(const glm::vec2& startPos,
    const glm::vec2& direction,
    float speed, int damage,
    const string& spritePath,
    const glm::vec2& sizeInSprite,
    int animationSpeed,
    const vector<glm::vec2>& animationKeyframes
    )
{
    if (projectiles.size() >= MAX_PROJECTILES) {
        removeInactiveProjectiles();
    }

    auto projectile = std::make_unique<Projectile>();
    projectile->init(startPos, direction, speed, *shaderProgram, spritePath,map, sizeInSprite, animationSpeed, animationKeyframes);
    projectile->setDamage(damage);
    projectiles.push_back(std::move(projectile));
}

void ProjectileManager::spawnProjectile(const glm::vec2& startPos,
    const glm::vec2& direction,
    float speed, int damage,
    const string& spritePath,
    bool isPlayerProjectile)  
{
    if (projectiles.size() >= MAX_PROJECTILES) {
        removeInactiveProjectiles();
    }

    auto projectile = std::make_unique<Projectile>();
    projectile->init(startPos, direction, speed, *shaderProgram, spritePath, map);
    projectile->setDamage(damage);
    projectile->setIsPlayerProjectile(isPlayerProjectile); 
    projectiles.push_back(std::move(projectile));
}

void ProjectileManager::update(int deltaTime)
{
    for (auto& projectile : projectiles) projectile->update(deltaTime);
    removeInactiveProjectiles();
}

void ProjectileManager::render(const glm::mat4& view)
{
    for (auto& projectile : projectiles) projectile->render(view);
}

std::vector<Projectile*> ProjectileManager::getActiveProjectiles()
{
    vector<Projectile*> active;
    for (auto& projectile : projectiles) {
        if (projectile->isActive()) {
            active.push_back(projectile.get());
        }
    }
    return active;
}

void ProjectileManager::removeInactiveProjectiles()
{
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
            [](const std::unique_ptr<Projectile>& p) {
                return !p->isActive();
            }),
        projectiles.end()
    );
}

void ProjectileManager::clear()
{
    projectiles.clear();
}