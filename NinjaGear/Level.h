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
};

