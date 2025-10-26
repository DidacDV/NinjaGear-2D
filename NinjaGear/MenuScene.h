#pragma once
#include "Scene.h"
#include "Sprite.h"

class MenuScene : public Scene
{
public:
    MenuScene();
    ~MenuScene();

    void init() override;    
    void update(int deltaTime) override;
    void render() override;  
    string menuImagePath;

private:
    Sprite* menuSprite;
    Texture menuTexture;
    bool imageLoaded;

};