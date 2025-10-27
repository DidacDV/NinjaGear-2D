#pragma once
#include "Scene.h"
#include "Player.h"
#include "Enemy.h"
#include "ProjectileManager.h"
#include <map>
#include <utility>

#include "Item.h"
#include "UIManager.h"
class Enemy;
class MovingStatue;
class MovingObject;

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


enum class MovingObjectType {
    MOVING_STATUE,
    // MOVING_PLATFORM, // Add other types here
};

struct MovingObjectConfig {
    glm::vec2 startPos;
    glm::vec2 endPos;
    float speed;
    MovingObjectType type;
    string spriteSheet;
    glm::vec2 spriteSize;
    glm::vec2 texCoordSize;
};

struct MusicConfig {
    int sectorI;           // Sector column
    int sectorJ;           // Sector row
    string musicFile;      // Path to music file
};

enum class LevelType { OUTSIDE, DUNGEON };


class Level :
    public Scene
{
    public:
    Level();
    Level(const vector<string>& tileMapFiles, Player* player, int initPlayerX, 
        int initPlayerY, const vector<EnemyConfig>& enemyConfigs, 
        const vector<MovingObjectConfig>& objectConfigs, const vector<MusicConfig>& musicConfigs, LevelType type);
    ~Level();

    void init() override;
    void reStartLevel();
    void update(int deltaTime) override;
	void render() override;
    ProjectileManager* getProjectileManager() { return &projectileManager; };


    void setUIManager(UIManager* uiManager);

    private:
        // Moving objects
        vector<MovingObject*> movingObjects;
        vector<MovingObjectConfig> movingObjectConfigs;
        vector<Texture> movingObjectTextures;

        //Player
        Player* player;
        int initPlayerX, initPlayerY;
        // Enemies
        vector<Enemy*> enemies;
        vector<EnemyConfig> enemyConfigs;
		    // Projectiles
        ProjectileManager projectileManager;
        //Music
        vector<MusicConfig> musicConfigs;
        map<pair<int, int>, MusicConfig> sectorMusicMap;  
        string currentMusicFile;

		    vector<Item*> items;
        //UIManager
        UIManager* uiManager;
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
        void initializeMovingObjects();
        void initializeMusic();       
        void updateMusic();
        void clearEnemies();
        void initializeItems();
		void initializeObjects(int tileSize);
		void initializeWeapons(int tileSize);
        void clearItems();
        void checkItemPickUp();
        bool checkColission(glm::vec2& pos1, glm::vec2& pos2, int size1, int size2);
        void itemPickUpEvent(int indexInVector);
        void clearProjectiles();
        LevelType type;
};

