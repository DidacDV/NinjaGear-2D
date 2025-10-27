#pragma once
#ifndef _NULLUIMANAGER_INCLUDE
#define _NULLUIMANAGER_INCLUDE
#include "UIManager.h"

class NullUIManager : public UIManager
{
public:
    ~NullUIManager() override {}

    void init() override {}

    void update(int deltaTime, Player* player) override {};

    void render() override {};

    void renderGameOverlay() override {};

    void showTemporaryMessage(const std::string& text, const glm::vec2& position,
        float scale, const glm::vec3& color, int durationMs) override {
    };
    void showMessageAndDialog(TemporaryMessage msg, float alpha, glm::vec3 color) override {};

    NullUIManager() : UIManager() {}

private:
};

#endif