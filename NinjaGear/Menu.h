#pragma once
#include "Scene.h"

class Menu : public Scene
{
public:
    Menu();
    Menu(const vector<string>& tileMapFiles);
    ~Menu();

    void init() override;
    void update(int deltaTime) override;
    void render() override;
};

