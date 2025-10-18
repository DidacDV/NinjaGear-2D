#ifndef _UIMANAGER_INCLUDE
#define _UIMANAGER_INCLUDE

#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "Player.h"
#include "Text.h"

class UIManager
{
public:
    UIManager();
    ~UIManager();

    void init();
    void update(int deltaTime, Player* player);
    void render();

private:
    void setupViewport(float heightPercent, float yOffset);
    void renderPanel(const glm::vec2& position, const glm::vec2& size, Texture* texture = nullptr, const glm::vec4& color = glm::vec4(1.f));
    void initQuad();
    void renderFixedText();
    void calculateLayout();
	void renderHealth(const glm::vec2& position, float currentHealth, float maxHealth);

private:
    ShaderProgram texProgram;
    glm::mat4 projection;

    //UI data
    float health;
    float maxHealth;
    int currentWeapon;
    int rank;
    int currentObject;

    //Rendering
    GLuint vao;
    GLuint vbo;
    Texture fullHeartTexture;
    Texture halfHeartTexture;
    Texture emptyHeartTexture;
    int screenWidth;
    int screenHeight;

    Text* textRenderer;

    static constexpr float MARGIN_LEFT = 10.0f;
    static constexpr float MARGIN_RIGHT = 10.0f;
    static constexpr float MARGIN_TOP = 15.f;
    static constexpr float PANEL_HEIGHT = 40.0f;
    static constexpr float PANEL_PADDING = 5.0f;

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
};

#endif