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
    const int HIT_ANIMATION_DURATION = 375;

	State currentState = State::IDLE;

    enum class Phase {
        PHASE_ONE,
        PHASE_TWO,
        PHASE_THREE
	};
	Phase currentPhase = Phase::PHASE_ONE;

    bool isOnScreen(int cameraWidth, int cameraHeight, float margin = 50.0f) const;
    // Moving
    glm::ivec2 moveStartTile;
    glm::ivec2 moveEndTile;
    bool movingRight = true;
    bool movementInitialized = false;
    void initializeMovement();
    void updateMovement(int deltaTime);

	// Shooting
    int shootCooldownTimer = 0;
    int shootCooldown = 2000;
    void shootAtPlayer(const glm::vec2& playerPos);

	// Phase Management
    void updatePhaseParameters();
    int getIdleAnim() { return (currentPhase == Phase::PHASE_THREE) ? IDLE_PHASE3 : IDLE; }
    int getHitAnim() { return (currentPhase == Phase::PHASE_THREE) ? HIT_PHASE3 : HIT; }

};

