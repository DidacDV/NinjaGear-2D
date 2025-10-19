#include "UIManagerUtils.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <algorithm>
#include <cctype>

std::string UIManagerUtils::toUppercase(const std::string& input)
{
    std::string output = input;
    std::transform(output.begin(), output.end(), output.begin(),
        [](unsigned char c) { return std::toupper(c); });
    return output;
}

glm::vec3 UIManagerUtils::pulseColor(const glm::vec3& colorA, const glm::vec3& colorB, float speed)
{
    float time = glfwGetTime();
    float t = 0.5f + 0.5f * sin(time * speed);
    return glm::mix(colorA, colorB, t);
}

Texture* UIManagerUtils::findOrLoadItemTexture(
    const std::string& basePath,
    const std::string& itemName,
    bool isWeapon,
    std::map<std::string, Texture>& weaponTextures,
    std::map<std::string, Texture>& objectTextures)
{
    auto& texMap = isWeapon ? weaponTextures : objectTextures;

    // Try cache first
    auto it = texMap.find(itemName);
    if (it != texMap.end())
        return &it->second;

    // Load new texture
    std::string path = basePath + itemName + ".png";
    Texture newTex;
    if (!newTex.loadFromFile(path, TEXTURE_PIXEL_FORMAT_RGBA)) {
        std::cout << "ERROR: Could not load " << path << std::endl;
        return nullptr;
    }

    texMap[itemName] = std::move(newTex);
    std::cout << "Loaded new texture: " << path << std::endl;

    return &texMap[itemName];
}

void UIManagerUtils::renderStyledText(
    Text* textRenderer,
    const std::string& text,
    const glm::vec2& pos,
    float scale,
    const glm::vec3& baseColor,
    bool pulse,
    const glm::vec3& colorA,
    const glm::vec3& colorB)
{
    if (!textRenderer) return;

    glm::vec3 finalColor = baseColor;
    if (pulse)
        finalColor = pulseColor(colorA, colorB);

    float offset = 2.0f;
    glm::vec3 outlineColor(1.f); // white outline for readability

    // Outline rendering (4 directions)
    textRenderer->RenderText(text, pos.x - offset, pos.y, scale, outlineColor);
    textRenderer->RenderText(text, pos.x + offset, pos.y, scale, outlineColor);
    textRenderer->RenderText(text, pos.x, pos.y - offset, scale, outlineColor);
    textRenderer->RenderText(text, pos.x, pos.y + offset, scale, outlineColor);

    // Main colored text
    textRenderer->RenderText(text, pos.x, pos.y, scale, finalColor);
}
