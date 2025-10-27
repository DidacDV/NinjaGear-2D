#ifndef _UIMANAGER_INCLUDE
#define _UIMANAGER_INCLUDE

#include <glm/glm.hpp>
#include <cctype>
#include <algorithm>
#include <string>
#include "ShaderProgram.h"
#include "Player.h"
#include "Text.h"

struct TemporaryMessage {
    std::string text;
    glm::vec2 position;
    float scale;
    glm::vec3 color;
    int remainingTime;  
    int totalDuration;  
};

int const FONT_SIZE = 24;

class UIManager
{
public:
    UIManager();
    virtual ~UIManager();

    virtual void init();
    virtual void update(int deltaTime, Player* player);
    virtual void render();
    virtual void renderGameOverlay();

    virtual void showTemporaryMessage(const std::string& text, const glm::vec2& position,
        float scale, const glm::vec3& color, int durationMs);
    virtual void showMessageAndDialog(TemporaryMessage msg, float alpha, glm::vec3 color);

private:
    void setupViewport(float heightPercent, float yOffset);
    void renderPanel(const glm::vec2& position, const glm::vec2& size, Texture* texture = nullptr, const glm::vec4& color = glm::vec4(1.f));
    void initQuad();
    void renderFixedText();
    void calculateLayout();
    void initTextures();
	void renderHealth(const glm::vec2& position, float currentHealth, float maxHealth);
    void renderRank(const glm::vec2& position, int currentRank);
	void renderItem(const glm::vec2& position, const string& weaponName, const string& basepath, int quantity); //used for both objects and weapons

    void renderTemporaryMessages();
    void updateTemporaryMessages(int deltaTime);

private:
    ShaderProgram texProgram;
    glm::mat4 projection;

    vector<TemporaryMessage> temporaryMessages;

    //UI data
    float health;
    float maxHealth;
    string currentItemName;
	int currentItemQuantity;
    string currentWeaponName;
    int rank; //goes from 0 (worst) to 6 (best)
    int maxRank = 6;

    //Rendering
    GLuint vao;
    GLuint vbo;
    Texture fullHeartTexture;
    Texture halfHeartTexture;
    Texture emptyHeartTexture;
    Texture dialogBoxTexture;
    std::vector<Texture> rankTextures;
    int screenWidth;
    int screenHeight;

    Text* textRenderer;

    static constexpr float MARGIN_LEFT = 20.0f;
    static constexpr float MARGIN_RIGHT = 20.0f;
    static constexpr float MARGIN_TOP = 15.f;
    static constexpr float PANEL_HEIGHT = 40.0f;
    static constexpr float PANEL_PADDING = 5.0f;

    static constexpr float ICON_SIZE = 28.0f;
    static constexpr float ICON_TEXT_MARGIN = 35.f;

    struct UIPositions {
        glm::vec2 life_text_pos;
        glm::vec2 life_value_pos;
        glm::vec2 rank_text_pos;
        glm::vec2 rank_value_pos;
        glm::vec2 weapon_text_pos;
        glm::vec2 weapon_value_pos;
        glm::vec2 object_text_pos;
        glm::vec2 object_value_pos;
    } uiPositions;

    struct RankInfo {
        std::string name;
        glm::vec3 baseColor; 
        glm::vec3 colorA;   
        glm::vec3 colorB;  
    };

    std::vector<RankInfo> ranks = {
        {"NOVICE",     glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(1.0f, 1.0f, 1.0f)}, // gray -> white 
        {"APPRENTICE", glm::vec3(0.2f, 0.8f, 1.0f), glm::vec3(0.0f, 0.5f, 1.0f), glm::vec3(0.0f, 1.0f, 1.0f)}, // cyan
        {"WARRIOR",    glm::vec3(0.9f, 0.5f, 0.2f), glm::vec3(1.0f, 0.3f, 0.0f), glm::vec3(1.0f, 0.7f, 0.2f)}, // orange->red 
        {"ELITE",      glm::vec3(0.6f, 0.3f, 1.0f), glm::vec3(0.4f, 0.0f, 1.0f), glm::vec3(0.8f, 0.5f, 1.0f)}, // deep purple -> bright purple
        {"MASTER",     glm::vec3(1.0f, 0.85f, 0.3f), glm::vec3(1.0f, 0.7f, 0.0f), glm::vec3(1.0f, 1.0f, 0.5f)}, // gold 
        {"LEGEND",     glm::vec3(1.0f, 0.2f, 0.2f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.5f, 0.2f)}  // bright red 
    };

    map<std::string, Texture> weaponTextures;
    map<std::string, Texture> objectTextures;
};

#endif