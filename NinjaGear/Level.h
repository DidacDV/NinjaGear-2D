#pragma once
#include "Scene.h"
#include "Player.h"
#include "Enemy.h"

class Enemy;

enum class EnemyType {
    BASE,
    MELEE,
    RANGED
};

struct EnemyConfig {
    int xTile;
    int yTile;
    string spriteSheet;
    EnemyType type;
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

        float cameraOffsetX;
        float cameraOffsetY;

        void updateCameraSector();
        void calculateCameraOffset();
        void initializeEnemies();
        void clearEnemies();
};

