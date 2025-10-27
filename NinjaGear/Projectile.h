#pragma once
#include "Sprite.h"
#include "TileMap.h"
#include "GeometryUtils.h"
#include <glm/glm.hpp>

class Projectile
{
public:
    void init(const glm::vec2& startPos, const glm::vec2& direction,
        float speed, ShaderProgram& shaderProgram,
        const string& spritePath, TileMap* map);

    void update(int deltaTime);
    void render(const glm::mat4& view);

    // Getters
    glm::vec2 getPosition() const { return position; }
    glm::vec2 getSize() const { return size; }
    bool isActive() const { return active; }
    int getDamage() const { return damage; }


    void deactivate() { active = false; }
    void setDamage(int dmg) { damage = dmg; }
    void setLifetime(int ms) { maxLifetime = ms; }

    void setIsPlayerProjectile(bool isPlayer) { isPlayerProjectile = isPlayer; }
    bool getIsPlayerProjectile() const { return isPlayerProjectile; }
private:
    // Position & movement
    glm::vec2 position;
    glm::vec2 direction;
    float speed;
    glm::vec2 size = glm::vec2(8.0f, 8.0f);

    // Lifetime
    bool active = true;
    int lifetime = 0;
    int maxLifetime = 5000;

    // Combat
    int damage = 10;

    // Rendering
    Sprite* sprite;
    Texture spritesheet;

    // Collision
    TileMap* map;
    bool checkWallCollision() const;
    bool isPlayerProjectile = false;

};
