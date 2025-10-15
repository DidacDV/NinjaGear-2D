#ifndef _UIMANAGER_INCLUDE
#define _UIMANAGER_INCLUDE

#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "Player.h"

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
    void renderPanel(const glm::vec2& position, const glm::vec2& size);
    void initQuad();

private:
    ShaderProgram texProgram;
    glm::mat4 projection;

    //UI data
    int health;
    int maxHealth;
    int currentWeapon;
    int rank;
    int currentObject;

    //Rendering
    GLuint vao;
    GLuint vbo;
    int screenWidth;
    int screenHeight;
};

#endif