#include "Boss.h"
#include <iostream>
#include "ServiceLocator.h"

Boss::Boss() : Enemy()
{
    health = 100;
    attackDamage = 15;
    moveSpeed = 3.0f;
    enemySize = glm::vec2(50.f, 50.f);
    sizeInSpritesheet = glm::vec2(0.2f, 0.25f);
}
void Boss::initializeAnimations()
{
    cachedTileSize = static_cast<float>(map->getTileSize());
    const float FRAME_WIDTH = 1.0f / 5.0f;
    const float FRAME_HEIGHT = 1.0f / 4.0f;
    sprite->setNumberAnimations(4);

    // IDLE
    sprite->setAnimationSpeed(IDLE, 8);
    sprite->addKeyframe(IDLE, glm::vec2(0.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));
    sprite->addKeyframe(IDLE, glm::vec2(1.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));
    sprite->addKeyframe(IDLE, glm::vec2(2.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));
    sprite->addKeyframe(IDLE, glm::vec2(3.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));
    sprite->addKeyframe(IDLE, glm::vec2(4.0f * FRAME_WIDTH, 0.0f * FRAME_HEIGHT));

    // HIT
    sprite->setAnimationSpeed(HIT, 8);
    sprite->addKeyframe(HIT, glm::vec2(0.0f * FRAME_WIDTH, 1.0f * FRAME_HEIGHT));
    sprite->addKeyframe(HIT, glm::vec2(1.0f * FRAME_WIDTH, 1.0f * FRAME_HEIGHT));
    sprite->addKeyframe(HIT, glm::vec2(2.0f * FRAME_WIDTH, 1.0f * FRAME_HEIGHT));

    // PHASE 3 ANIMATIONS
    sprite->setAnimationSpeed(IDLE_PHASE3, 8);
    sprite->addKeyframe(IDLE_PHASE3, glm::vec2(0.0f * FRAME_WIDTH, 2.0f * FRAME_HEIGHT));
    sprite->addKeyframe(IDLE_PHASE3, glm::vec2(1.0f * FRAME_WIDTH, 2.0f * FRAME_HEIGHT));
    sprite->addKeyframe(IDLE_PHASE3, glm::vec2(2.0f * FRAME_WIDTH, 2.0f * FRAME_HEIGHT));
    sprite->addKeyframe(IDLE_PHASE3, glm::vec2(3.0f * FRAME_WIDTH, 2.0f * FRAME_HEIGHT));
    sprite->addKeyframe(IDLE_PHASE3, glm::vec2(4.0f * FRAME_WIDTH, 2.0f * FRAME_HEIGHT));

    sprite->setAnimationSpeed(HIT_PHASE3, 8);
    sprite->addKeyframe(HIT_PHASE3, glm::vec2(0.0f * FRAME_WIDTH, 3.0f * FRAME_HEIGHT));
    sprite->addKeyframe(HIT_PHASE3, glm::vec2(1.0f * FRAME_WIDTH, 3.0f * FRAME_HEIGHT));
    sprite->addKeyframe(HIT_PHASE3, glm::vec2(2.0f * FRAME_WIDTH, 3.0f * FRAME_HEIGHT));


    sprite->changeAnimation(getIdleAnim());
    sprite->setPosition(glm::vec2(float(tileMapDispl.x + posEnemy.x), float(tileMapDispl.y + posEnemy.y)));
}

void Boss::updateStateMachine(int deltaTime) {
    const int CAMERA_WIDTH = 320;
    const int CAMERA_HEIGHT = 320;
    if (!isOnScreen(CAMERA_WIDTH, CAMERA_HEIGHT)) {

        return;
    }
    // PHASE MANAGEMENT
    float healthPercent = static_cast<float>(health) / 300.0f;

    if (currentPhase == Phase::PHASE_ONE && healthPercent <= 0.66f) {
        currentPhase = Phase::PHASE_TWO;
        std::string pickupText = "I AM TIRED OF YOU! GO DIE! ";
        glm::vec2 messagePos(320, 160);
        glm::vec3 messageColor(0.f, 0.f, 0.f);
        ServiceLocator::getUI().showTemporaryMessage(pickupText, messagePos, 1.0f, messageColor, 2000);

        updatePhaseParameters();
        cout << "Boss entered Phase 2!" << endl;
    }
    else if (currentPhase == Phase::PHASE_TWO && healthPercent <= 0.33f) {
        currentPhase = Phase::PHASE_THREE;
        updatePhaseParameters();
        cout << "Boss entered Phase 3!" << endl;
    }

    if (hitAnimationTimer > 0)
    {
        hitAnimationTimer -= deltaTime;
        if (hitAnimationTimer <= 0)
        {
            sprite->changeAnimation(getIdleAnim());
        }
        else
        {
            return;
        }
    }


    switch (currentState) {
    case State::IDLE:
        if (!movementInitialized) {
            initializeMovement();
        }
        currentState = State::MOVING;
        break;

    case State::MOVING:
        updateMovement(deltaTime);
        break;

    case State::SHOOTING:
        // Reserved for future more complex shooting patterns
        break;
    }
}

void Boss::updatePhaseParameters()
{
    // Update boss parameters based on current phase
    switch (currentPhase) {
    case Phase::PHASE_ONE:
        moveSpeed = 3.0f;
        shootCooldown = 2000;
        attackDamage = 15;
        break;
    case Phase::PHASE_TWO:
        moveSpeed = 4.5f;
        shootCooldown = 1500;
        attackDamage = 20;
        cout << "Boss is faster and shoots more frequently!" << endl;
        break;
    case Phase::PHASE_THREE:
        moveSpeed = 6.0f;
        shootCooldown = 1000;
        attackDamage = 25;
        cout << "Boss enters rage mode!" << endl;
        break;
    }
}


// Movement
void Boss::initializeMovement()
{
    glm::ivec2 currentTile = getEnemyTile();

    // Set horizontal movement range (e.g., 8 tiles left and right)
    moveStartTile = glm::ivec2(
        std::max(0, currentTile.x - 4),
        currentTile.y
    );

    moveEndTile = glm::ivec2(
        std::min(map->width() - 1, currentTile.x + 4),
        currentTile.y
    );

    movementInitialized = true;
}

void Boss::updateMovement(int deltaTime)
{
    glm::ivec2 targetTile = movingRight ? moveEndTile : moveStartTile;
    glm::vec2 targetPos = tileToWorld(targetTile);
    glm::vec2 currentPos(posEnemy);

    glm::vec2 direction = targetPos - currentPos;
    float distanceToTarget = glm::length(direction);

    // Check if reached target
    if (distanceToTarget < cachedTileSize * 0.5f) {
        movingRight = !movingRight;
        currentDirection = movingRight ? RIGHT : LEFT;
        return;
    }

    // Move towards target
    if (distanceToTarget > 0.1f) {
        direction = glm::normalize(direction);
        float moveAmount = moveSpeed * cachedTileSize * (deltaTime / 1000.0f);
        posEnemy += direction * moveAmount;
    }

    // Shoot while moving
    if (shootCooldownTimer <= 0) {
        glm::vec2 playerPos = Game::instance().getPlayerPosition();
        shootAtPlayer(playerPos);
        shootCooldownTimer = shootCooldown;
    }
    else shootCooldownTimer -= deltaTime;
}


// Shooting
void Boss::shootAtPlayer(const glm::vec2& playerPos)
{
    if (!projectileManager) return;
    cout << "about to shoot!" << endl;
    glm::vec2 direction = glm::normalize(playerPos - posEnemy);
    float speed = 100.0f;
    int damage = 1;

    vector<glm::vec2> projectileKeyframes = {
    glm::vec2(0.0f, 0.0f),
    glm::vec2(0.25f, 0.0f),
    glm::vec2(0.50f, 0.0f),
    glm::vec2(0.75f, 0.0f)
    };

    projectileManager->spawnProjectile(
        posEnemy,
        direction,
        speed,
        damage,
        "images/projectiles/fireball.png",
        false,
        glm::vec2(0.25f, 1.0f),
        10,
        projectileKeyframes
    );
}


void Boss::changeAnimationsForDirection(glm::vec2 direction)
{
    // Simple implementation - boss primarily moves horizontally

}

bool Boss::isInAttackState() const
{
    return currentState == State::MOVING || currentState == State::SHOOTING;
}


void Boss::onDamageReceived()
{
    sprite->changeAnimation(getHitAnim());
    hitAnimationTimer = HIT_ANIMATION_DURATION;
}

bool Boss::isOnScreen(int cameraWidth, int cameraHeight, float margin) const
{
    glm::vec2 worldPos = glm::vec2(tileMapDispl.x + posEnemy.x,
        tileMapDispl.y + posEnemy.y);
    glm::vec2 screenPos = worldPos + cameraOffset;

    return (screenPos.x > -margin && screenPos.x < cameraWidth + margin &&
        screenPos.y > -margin && screenPos.y < cameraHeight + margin);
}