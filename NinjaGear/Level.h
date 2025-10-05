#pragma once
#include "Scene.h"
class Level :
    public Scene
{
    public:
    Level();
    Level(const vector<string>& tileMapFiles, Player* player, int initPlayerX, int initPlayerY);
    ~Level();

    void init() override;
    void update(int deltaTime) override;
	void render() override;
    

    private:
        Player* player;
        int initPlayerX, initPlayerY;

};

