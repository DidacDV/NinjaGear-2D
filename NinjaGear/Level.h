#pragma once
#include "Scene.h"
#include "Player.h"
#include "Enemy.h"
#include "Item.h"
#include "UIManager.h"
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

    void setUIManager(UIManager* uiManager);

    private:
        //Player
        Player* player;
        int initPlayerX, initPlayerY;
        // Enemies
        vector<Enemy*> enemies;
        vector<EnemyConfig> enemyConfigs;
		vector<Item*> items;
        //UIManager
        UIManager* uiManager;
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
        void initializeItems();
		void initializeObjects(int tileSize);
		void initializeWeapons(int tileSize);
        void clearItems();
        void checkItemPickUp();
        bool checkColission(glm::vec2& pos1, glm::vec2& pos2, int size1, int size2);
        void itemPickUpEvent(int indexInVector);
};

