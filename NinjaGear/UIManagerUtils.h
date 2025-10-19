#pragma once
#ifndef _UI_MANAGER_UTILS_INCLUDE
#define _UI_MANAGER_UTILS_INCLUDE

#include <string>
#include <map>
#include <glm/glm.hpp>
#include "Texture.h"
#include "Text.h"

class UIManagerUtils
{
public:
	static std::string toUppercase(const std::string& input);

	//interpolates between two colors to create a pulsing effect.
	static glm::vec3 pulseColor(const glm::vec3& colorA, const glm::vec3& colorB, float speed = 2.5f);

	//finds a texture in the cache or loads it from disk if not found.
	static Texture* findOrLoadItemTexture(const std::string& basePath, const std::string& itemName, bool isWeapon, std::map<std::string, Texture>& weaponTextures, std::map<std::string, Texture>& objectTextures);

	//renders styled text with optional outline and pulsing color.
	static void renderStyledText(Text* textRenderer, const std::string& text, const glm::vec2& pos, float scale, const glm::vec3& baseColor, bool pulse = false, const glm::vec3& colorA = glm::vec3(1.f), const glm::vec3& colorB = glm::vec3(1.f));
};

#endif
