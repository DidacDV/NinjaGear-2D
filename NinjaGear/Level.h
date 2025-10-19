#pragma once
#include "Scene.h"
#include "Player.h"
#include "Enemy.h"

class Enemy;

struct EnemyConfig {
    int xTile;
    int yTile;
    string spriteSheet;
    Enemy* enemyInstance;
};

class Level :
    public Scene
{
    public:
    Level();
    Level(const vector<string>& tileMapFiles, Player* player, int initPlayerX, int initPlayerY, const vector<EnemyConfig>& enemyConfigs);
    ~Level();

    void init() override;
    void update(int deltaTime) override;
	void render() override;
	// Enemy management
    void addEnemy(const string& spriteSheet, int initX, int initY);

    private:
        //Player
        Player* player;
        int initPlayerX, initPlayerY;
        // Enemies
        vector<Enemy*> enemies;
        vector<EnemyConfig> enemyConfigs;
        // Camera sector traking
        int currentSectorI;  
        int currentSectorJ;  
        int numSectorsI;     
        int numSectorsJ;     
        int sectorWidth;     
        int sectorHeight;

        // Combat manager
        static constexpr float PLAYER_SIZE = 16.0f;
        static constexpr float ENEMY_SIZE = 16.0f;
        static constexpr int ENEMY_CONTACT_DAMAGE = 10;
        void checkCombat(int deltaTime);
        void handlePlayerAttack();
        //TODO enemy attacking void handleEnemyAttacks();

        // Helper for collision detection
        bool isColliding(const glm::vec2& pos1, const glm::vec2& size1,
            const glm::vec2& pos2, const glm::vec2& size2);

        float cameraOffsetX;
        float cameraOffsetY;

        void updateCameraSector();
        void calculateCameraOffset();
        void initializeEnemies();
};

