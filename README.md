<div align="center">
  <img src="NinjaGear/images/StartMenu.png" alt="Game Banner" width="800"/>
</div>

# Software Design Patterns & Principles - Ninja Gear

This document explains the software design patterns and architectural decisions used in the Ninja Gear game engine.
## Table of Contents

1. [Service Locator Pattern](#service-locator-pattern)
2. [Factory Pattern](#factory-pattern)
3. [Singleton Pattern](#singleton-pattern)
4. [Good Practices and Design Principles](#Good-Practices-and-Design-Principles)
---

## Service Locator Pattern

**Used in:** `ServiceLocator` class for audio and UI management

### Purpose
Provides global access to services without tight coupling between classes.

### Class Diagram

```mermaid
classDiagram
    class ServiceLocator {
        -Audio* audioService
        -UIManager* uiService
        +getAudio() Audio
        +getUI() UIManager
        +provide(Audio*) void
        +provide(UIManager*) void
    }
    
    class Level {
        +playMusic()
        +showMessage()
    }
    
    class Audio {
        +playMusic(file)
        +playSound(file)
    }
    
    class UIManager {
        +showTemporaryMessage()
        +render()
    }
    
    Level --> ServiceLocator
    ServiceLocator --> Audio
    ServiceLocator --> UIManager
```

### Implementation

```cpp
// ServiceLocator.h
class ServiceLocator {
private:
    static Audio* audioService;
    static UIManager* uiService;
    
public:
    static Audio& getAudio() { return *audioService; }
    static UIManager& getUI() { return *uiService; }
    
    static void provide(Audio* service) { audioService = service; }
    static void provide(UIManager* service) { uiService = service; }
};

// ServiceLocator.cpp
Audio* ServiceLocator::audioService = nullptr;
UIManager* ServiceLocator::uiService = nullptr;
```

### Usage Example

```cpp
// Level.cpp - Playing music
void Level::updateMusic() {
    auto it = sectorMusicMap.find({ currentSectorI, currentSectorJ });
    if (it != sectorMusicMap.end()) {
        const MusicConfig& config = it->second;
        if (config.musicFile != currentMusicFile) {
            currentMusicFile = config.musicFile;
            ServiceLocator::getAudio().playMusic(config.musicFile.c_str());
        }
    }
}

// Level.cpp - Showing UI message
void Level::itemPickUpEvent(int indexInVector) {
    Item* itemPicked = items[indexInVector];
    std::string pickupText = "PICKED UP " + itemPicked->getName() + "!";
    
    ServiceLocator::getUI().showTemporaryMessage(
        pickupText, 
        glm::vec2(320, 160), 
        1.0f, 
        glm::vec3(0.f, 0.f, 0.f), 
        2000
    );
}
```
---

## Factory Pattern

**Used in:** `EnemyFactory`, `MovingObjectFactory`

### Purpose
Centralizes object creation logic and provides a clean interface for creating different types of objects.

### Class Diagram

```mermaid
classDiagram
    class EnemyFactory {
        +create(EnemyConfig) Enemy*
    }
    
    class MovingObjectFactory {
        +create(MovingObjectConfig) MovingObject*
    }
    
    class Level {
        -vector~Enemy*~ enemies
        -vector~MovingObject*~ movingObjects
        +initializeEnemies()
        +initializeMovingObjects()
    }
    
    class Enemy {
        +init()
        +setPosition()
        +update()
    }
    
    class MovingObject {
        +init()
        +setPosition()
        +update()
    }
    
    Level ..> EnemyFactory : uses
    Level ..> MovingObjectFactory : uses
    EnemyFactory ..> Enemy : creates
    MovingObjectFactory ..> MovingObject : creates
```

### Implementation

```cpp
// EnemyFactory.h
class EnemyFactory {
public:
    static Enemy* create(
        const EnemyConfig& config,
        const glm::ivec2& screenSize,
        ShaderProgram& program,
        TileMap* map,
        const vector<TileMap*>& allMaps,
        ProjectileManager* projectileManager
    );
};

// EnemyFactory.cpp
Enemy* EnemyFactory::create(
    const EnemyConfig& config,
    const glm::ivec2& screenSize,
    ShaderProgram& program,
    TileMap* map,
    const vector<TileMap*>& allMaps,
    ProjectileManager* projectileManager
) {
    Enemy* enemy = new Enemy();
    
    enemy->init(screenSize, program);
    enemy->setPosition(glm::vec2(
        config.x * map->getTileSize(),
        config.y * map->getTileSize()
    ));
    enemy->setTileMaps(allMaps);
    enemy->setProjectileManager(projectileManager);
    
    // Configure enemy based on config
    enemy->setHealth(config.health);
    enemy->setDamage(config.damage);
    enemy->setBoss(config.isBoss);
    
    return enemy;
}
```

### Usage Example

```cpp
// Level.cpp
void Level::initializeEnemies() {
    for (const auto& config : enemyConfigs) {
        Enemy* enemy = EnemyFactory::create(
            config,
            glm::ivec2(GameConfig::SCREEN_X, GameConfig::SCREEN_Y),
            texProgram,
            maps[0],
            maps,
            &projectileManager
        );
        if (enemy) {
            enemies.push_back(enemy);
        }
    }
}

void Level::initializeMovingObjects() {
    for (size_t i = 0; i < movingObjectConfigs.size(); i++) {
        const MovingObjectConfig& config = movingObjectConfigs[i];
        
        MovingObject* obj = MovingObjectFactory::create(
            config,
            &movingObjectTextures[i],
            &texProgram,
            maps,
            glm::ivec2(GameConfig::SCREEN_X, GameConfig::SCREEN_Y)
        );
        
        if (obj) {
            movingObjects.push_back(obj);
        }
    }
}
```

---
## Singleton Pattern

**Used in:** `Game` class, `GameConfig`

### Purpose
Ensures a class has only one instance and provides global access to it.

### Class Diagram

```mermaid
classDiagram
    class Game {
        -static Game* instance
        -Game()
        +static instance() Game
        +init()
        +update()
        +render()
        +setCurrentScene(string)
        +victory()
    }
    
    class Level {
        +checkCombat()
    }
    
    Level ..> Game : accesses singleton
    
    note for Game "Private constructor\nStatic instance\nGlobal access point"
```

### Implementation

```cpp
// Game.h
class Game {
private:
    static Game* _instance;
    Game() {}  // Private constructor
    
public:
    static Game& instance() {
        if (_instance == nullptr) {
            _instance = new Game();
        }
        return *_instance;
    }
    
    void setCurrentScene(const string& sceneName);
    void victory();
};

// Game.cpp
Game* Game::_instance = nullptr;
```

### Usage Example

```cpp
// Level.cpp
void Level::handlePlayerAttack() {
    // ... combat logic ...
    
    if (!enemy->isAlive()) {
        if (enemy->isBoss()) {
            Game::instance().victory();  // Access singleton
        }
        delete enemy;
        player->increaseRank(1);
        it = enemies.erase(it);
        continue;
    }
}

void Level::checkTransitionTiles() {
    // ... tile checking ...
    
    for (const auto& transition : transitionTiles) {
        if (transition.tileId == tileId) {
            Game::instance().setCurrentScene(transition.targetScene);  // Access singleton
            player->setPosition(newPos);
            return;
        }
    }
}
```

---
# Good Practices and Design Principles

## 1. Polymorphism and Extensibility

**Used in:** `Enemy`, `MeleeEnemy`, `Player`, and `MovingObject` classes

The engine leverages **polymorphism** through virtual and pure virtual methods to allow flexible behavior for different entities (e.g., `MeleeEnemy`, `RangedEnemy`, `BossEnemy`).  
Each enemy class defines its own state machine and animation logic while sharing common functionality through the `Enemy` base class.

This supports the **Open/Closed Principle (OCP)** — new enemy types can be added without modifying existing code, only by extending existing interfaces.

```cpp
class Enemy {
public:
    virtual void update(int deltaTime) = 0;
    virtual void onDamageReceived() = 0;
};

class MeleeEnemy : public Enemy {
protected:
    void onDamageReceived() override {
        // Custom reaction for melee enemies
    }
};
```

---

## 2. DRY (Don't Repeat Yourself)

**Used in:** `Level`, `Player`, and `Enemy` systems

To reduce redundancy, common functionality is abstracted and reused:
- Reusable movement and collision logic via shared `TileMap` functions  
- Shared helpers for coordinate conversion (`tileToWorld`, `worldToTile`) across systems  
- Factories (`EnemyFactory`, `MovingObjectFactory`) centralize object creation  

This ensures consistency and maintainability.

```cpp
inline glm::vec2 tileToWorld(const glm::ivec2& tile) const {
    return glm::vec2(tile.x * cachedTileSize, tile.y * cachedTileSize);
}
```
---
## 3. Single Responsibility Principle (SRP)

Each system class is designed to do **one thing well**:
- `Pathfinder` → pathfinding and navigation  
- `ProjectileManager` → projectile lifecycle  
- `TileMap` → map rendering and collision checks  
- `Enemy` subclasses → specialized AI behaviors  

This simplifies debugging and allows independent scaling of systems.

---

## 4. Maintainability and Scalability

The engine’s modular structure allows new content (maps, items, enemies, weapons) to be added with minimal code changes:
- Enemy types are defined through configuration (`EnemyConfig`) and created by factories  
- Game scenes are loaded dynamically using `Game::instance().setCurrentScene()`  
- Shared managers (like `ProjectileManager` and `UIManager`) handle cross-system operations  
---
- `ProjectileManager` → projectile lifecycle  
- `TileMap` → map rendering and collision checks  
- `Enemy` subclasses → specialized AI behaviors  

This simplifies debugging and allows independent scaling of systems.

---

## 4. Maintainability and Scalability

The engine’s modular structure allows new content (maps, items, enemies, weapons) to be added with minimal code changes:
- Enemy types are defined through configuration (`EnemyConfig`) and created by factories  
- Game scenes are loaded dynamically using `Game::instance().setCurrentScene()`  
- Shared managers (like `ProjectileManager` and `UIManager`) handle cross-system operations  
---
