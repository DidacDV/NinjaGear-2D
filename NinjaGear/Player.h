#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE


#include "Sprite.h"
#include "TileMap.h"
#include "Item.h"
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.

enum BuffType {
	SPEED,
	STRENGTH
};

struct ActiveBuff {
	BuffType type;
	float duration;      // total duration in seconds
	float remainingTime; // remaining time in seconds
	float multiplier;    
};

class Player
{

public:
	enum PlayerAnims
	{
		STAND_LEFT, STAND_RIGHT, STAND_UP, STAND_DOWN,
		MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN,
		PUNCH_LEFT, PUNCH_RIGHT, PUNCH_UP, PUNCH_DOWN,
		DANCE
	};
	virtual void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram);
	virtual void update(int deltaTime);
	virtual void render(const glm::mat4& view = glm::mat4(1.0f));

	void setTileMaps(const vector<TileMap*>& tileMaps);
	void setPosition(const glm::vec2 &pos);
	void setSpriteSheet(const string& spriteSheet);
	glm::ivec2 getPosition() const { return posPlayer; }
	glm::vec2 getPositionFloat() const { return glm::vec2(posPlayer); }

	bool isInvulnerable() const { return invulnerable; }
	void takeDamage(int damage);
	bool isAlive() const { return health > 0; }
	int getRank() const { return rank; }
	void increaseRank(const int& increase);

	// Punching
	bool isPunching() const;
	bool justStartedPunching() const;
	int getFacingDirection() const; 
	glm::vec2 getPunchHitbox() const;
	void onPunchKeyPressed();

	//both items and weapons
	void addItem(Item* item);
	void initDefaultWeapon();

	void cycleItem();
	void useCurrentItem();
	void cycleWeapon();
	Item* getCurrentItem() const;

	Item* getCurrentWeapon() const;

	float getHealth() const { return health; }
	float getMaxHealth() const { return maxHealth; }
	void setHealth(float h) { health = h; }
	void heal(float amount);

	bool hasItems() const { return !itemInventory.empty(); }
	bool hasWeapons() const { return !weaponInventory.empty(); }
	int getItemQuantity(const std::string& itemName) const;

private:

	int currentFacingDirection = STAND_DOWN; // Track last direction
	int lastFrameAnimation = -1;

	// Attributes
	bool invulnerable = false;
	int invulnerabilityTimer = 0;
	int rank = 0;
	bool isPunchKeyHeld = false;

	// Collision detectors
	bool collisionMoveLeft(const glm::ivec2& pos, const glm::ivec2& size) const;
	bool collisionMoveRight(const glm::ivec2& pos, const glm::ivec2& size) const;
	bool collisionMoveDown(const glm::ivec2& pos, const glm::ivec2& size) const;
	bool collisionMoveUp(const glm::ivec2& pos, const glm::ivec2& size) const;

	float baseSpeed = 2.0f;
	glm::ivec2 tileMapDispl, posPlayer;
	Texture spritesheet;
	Sprite *sprite;
	vector<TileMap*> maps;
	string spriteSheet;

	//items
	vector<Item*> itemInventory;
	//weapons only
	vector<Item*> weaponInventory;

	std::unordered_map<std::string, int> itemQuantities;
	int currentItemIndex;
	int currentWeaponIndex;
	float health;
	float maxHealth;
	string equippedWeapon;

	void consumeItem(Item* item, string& itemName);

	//buffs
	std::unordered_map<BuffType, ActiveBuff> activeBuffs;
	void applyBuff(BuffType type, float duration, float multiplier);
	void checkBuffsState(int deltaTime);
	//aura
	Sprite* auraSprite = nullptr;
	Texture auraTexture;
	bool showAura = false;
	int auraFrameTime = 80; //ms
	void setUpAuraSprites(ShaderProgram& shaderProgram);

	//bow
	void setUpBowSprite(ShaderProgram& shaderProgram);
	Texture bowSpritesheet;
	Sprite* bowSprite = nullptr;
	bool isBowEquipped = false;
public:
	~Player() {
		for (Item* item : itemInventory) delete item;
		for (Item* weapon : weaponInventory) delete weapon;
		itemInventory.clear();
		weaponInventory.clear();
	}
};



#endif // _PLAYER_INCLUDE


