#pragma once
#include "Scene.h"
#include "Player.h"
#include "Enemy.h"
#include "ProjectileManager.h"

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
    ProjectileManager* getProjectileManager() { return &projectileManager; }


    private:
        //Player
        Player* player;
        int initPlayerX, initPlayerY;
        // Enemies
        vector<Enemy*> enemies;
        vector<EnemyConfig> enemyConfigs;
		// Projectiles
        ProjectileManager projectileManager;

        // Camera sector traking
        int currentSectorI = 0;  
        int currentSectorJ = 0;  
        int numSectorsI = 0;     
        int numSectorsJ = 0;     
        int sectorWidth = 0;     
        int sectorHeight = 0;
        float cameraOffsetX = 0.0f;
        float cameraOffsetY = 0.0f;

        // Combat manager
        static constexpr float PLAYER_SIZE = 16.0f;
        static constexpr float ENEMY_SIZE = 16.0f;
        static constexpr int ENEMY_CONTACT_DAMAGE = 10;
        void checkCombat();
        void handlePlayerAttack();
        //TODO enemy attacking void handleEnemyAttacks();

        // Helper for collision detection
        bool isColliding(const glm::vec2& pos1, const glm::vec2& size1,
            const glm::vec2& pos2, const glm::vec2& size2);

      

        void updateCameraSector();
        void calculateCameraOffset();
        void initializeEnemies();
};

