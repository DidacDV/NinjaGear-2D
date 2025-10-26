#pragma once
#include "Sprite.h"
#include "TileMap.h"
class MovingObject :
    public Sprite
{
public:
    static MovingObject* createMovingObject(const glm::vec2& quadSize,
        const glm::vec2& sizeInSpritesheet,
        Texture* spritesheet,
        ShaderProgram* program);

    MovingObject(const glm::vec2& quadSize,
        const glm::vec2& sizeInSpritesheet,
        Texture* spritesheet,
        ShaderProgram* program);

    virtual void update(int deltaTime) override;

    virtual void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram,
        const glm::vec2& spriteSize, const glm::vec2& texCoordSize, Texture* texture);

    virtual void setMovementPath(const glm::vec2& startPos, const glm::vec2& endPos, float speed);

    virtual void updateMovementPattern(int deltaTime);

    void setTileMapPosition(const glm::ivec2& tilePos, const glm::ivec2& tileMapDispl);
    void setTileMaps(const std::vector<TileMap*>& tileMaps);


    glm::vec2 getPosition() const { return objectPosition; }
    void setVelocity(const glm::vec2& vel) { velocity = vel; }
    glm::vec2 getVelocity() const { return velocity; }

    void setCameraOffset(const glm::vec2& offset) { cameraOffset = offset; }

protected:
    // Collision detection helpers
    bool collisionMoveLeft(const glm::ivec2& pos, const glm::ivec2& size) const;
    bool collisionMoveRight(const glm::ivec2& pos, const glm::ivec2& size) const;
    bool collisionMoveDown(const glm::ivec2& pos, const glm::ivec2& size) const;
    bool collisionMoveUp(const glm::ivec2& pos, const glm::ivec2& size) const;
    bool isOnScreen(int cameraWidth, int cameraHeight, float margin = 50.0f) const;
        
	void playSoundIfOnScreen(const std::string& soundFile) const;
    glm::vec2 objectPosition;
    glm::vec2 velocity;
    glm::ivec2 tileMapDispl;
    std::vector<TileMap*> maps;
    glm::vec2 cameraOffset;
};