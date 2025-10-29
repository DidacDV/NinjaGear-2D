#pragma once
#include "Enemy.h"
class Boss :
    public Enemy
{
public:
    Boss();
    enum Anims {
		IDLE, HIT, IDLE_PHASE3, HIT_PHASE3
    };
    bool isInAttackState() const override;
    bool isBoss() const override { return true; }
protected:
    void initializeAnimations() override;
    void changeAnimationsForDirection(glm::vec2 direction) override;
	void updateStateMachine(int deltaTime) override;
    void onDamageReceived() override;

private:
    enum class State {
        IDLE,
        MOVING,
        SHOOTING
    };
    int hitAnimationTimer; 
    int lastHitTimer;
    const int HIT_ANIMATION_DURATION = 100;
    const int HIT_COOLDOWN = 500;

	State currentState = State::IDLE;

    enum class Phase {
        PHASE_ONE,
        PHASE_TWO,
        PHASE_THREE,
        PHASE_FOUR
	};
	Phase currentPhase = Phase::PHASE_ONE;

    bool isOnScreen(int cameraWidth, int cameraHeight, float margin = 0.0f) const;
    // Moving
    glm::ivec2 moveStartTile;
    glm::ivec2 moveEndTile;
    bool movingRight = true;
    bool movementInitialized = false;
    void initializeMovement(bool vertical = false);
    void updateMovement(int deltaTime);

	// Shooting
    int shootCooldownTimer = 0;
    int shootCooldown = 2000;
    void shootAtPlayer(const glm::vec2& playerPos);

	// Phase Management
    void updatePhaseParameters();
    int getIdleAnim() { return (currentPhase == Phase::PHASE_FOUR) ? IDLE_PHASE3 : IDLE; }
    int getHitAnim() { return (currentPhase == Phase::PHASE_FOUR) ? HIT_PHASE3 : HIT; }

};

