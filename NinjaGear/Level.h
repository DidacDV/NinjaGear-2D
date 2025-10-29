#pragma once
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <glm/glm.hpp>
#include "Scene.h"
#include "Player.h"
#include "Enemy.h"
#include "MovingObject.h"
#include "ProjectileManager.h"
#include "Item.h"
#include "UIManager.h"
#include "GameConfig.h"

class Enemy;
class MovingStatue;
class MovingObject;
struct EnemyConfig;

using namespace std;

struct MusicConfig {
    int sectorI;           
    int sectorJ;           
    string musicFile;      
};

enum class LevelType { OUTSIDE, DUNGEON };

struct ItemData {
    std::string name;
    int quantity;
    std::string description;
    glm::vec2 pos;
    Texture* texture;
};

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

    private:
        struct TransitionTile {
            int tileId;
            std::string targetScene;
            int targetTileX;
            int targetTileY;
        };

        std::vector<TransitionTile> transitionTiles;
        // Moving objects
        vector<MovingObject*> movingObjects;
        vector<MovingObjectConfig> movingObjectConfigs;
        vector<Texture> movingObjectTextures;
        TileMap* navigationMap;
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

		vector<Item*> items;
        // Camera sector traking
        int currentSectorI = 0;  
        int currentSectorJ = 0;  
        int numSectorsI = 0;     
        int numSectorsJ = 0;     
        int sectorWidth = 0;      
        int sectorHeight = 0;
        float cameraOffsetX = 0.0f;
        float cameraOffsetY = 0.0f;
        bool introMessagesDisplayed = false;
        int introMessageDelayTimer = 500;
        // Combat manager
        static constexpr float PLAYER_SIZE = GameConfig::PLAYER_SIZE;
        static constexpr float ENEMY_SIZE = GameConfig::ENEMY_SIZE;
        static constexpr int ENEMY_CONTACT_DAMAGE = 10;
        void checkCombat();
        void handlePlayerAttack();
        //TODO enemy attacking void handleEnemyAttacks();

        // Helper for collision detection
        bool isColliding(const glm::vec2& pos1, const glm::vec2& size1,
            const glm::vec2& pos2, const glm::vec2& size2);

        void checkTransitionTiles();
        void initializeTransitionTiles();

        void checkMovingObjectCollisions();
        
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

        // Ambient
        struct Particle {
            Sprite* sprite;
            glm::vec2 position;
            glm::vec2 velocity;
            float lifetime;
            float maxLifetime;
            float scale;
            int animationFrame;
        };

        std::vector<Particle> particles;
        Texture particleTexture;
        float particleSpawnTimer;
        float particleSpawnInterval; 

        void initializeParticleSystem();
        void updateParticles(int deltaTime);
        void spawnParticle();
        void renderParticles();
};

