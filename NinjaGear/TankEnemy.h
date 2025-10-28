#pragma once
#ifndef _TANK_ENEMY_INCLUDE
#define _TANK_ENEMY_INCLUDE

#include "MeleeEnemy.h"

class TankEnemy : public MeleeEnemy
{
public:
    TankEnemy()
    {
        // Tank stats: slower, tankier, hits harder, more cooldown between attacks
        moveSpeed = 0.8f;      
        health = 10;            
        attackDamage = 2;            

        attackCooldown = 2000; 
    }

    virtual ~TankEnemy() {}
};

#endif // _TANK_ENEMY_INCLUDE