#pragma once
#include "Projectile.h"
#include <vector>
#include <memory>

class ProjectileManager
{
public:
    void init(ShaderProgram* shaderProgram, TileMap* map);

    void spawnProjectile(const glm::vec2& startPos, const glm::vec2& direction,
        float speed, int damage, const string& spritePath, 
        bool isPlayerProjectile = false,
        const glm::vec2& sizeInSprite = glm::vec2(1.0f, 1.0f),
        int animationSpeed = 10,
        const vector<glm::vec2>& animationKeyframes = {}
    );

    void update(int deltaTime);
    void render(const glm::mat4& view);
    void clear();

    vector<Projectile*> getActiveProjectiles();

private:
    vector<std::unique_ptr<Projectile>> projectiles;
    ShaderProgram* shaderProgram;
    TileMap* map;

    // Object pooling for performance
    static const int MAX_PROJECTILES = 100;
    void removeInactiveProjectiles();
};