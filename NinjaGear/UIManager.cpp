#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "UIManager.h"
#include "Globals.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

UIManager::UIManager()
{
    health = 0;
    maxHealth = 100;
    currentWeapon = 0;
    rank = 0;
    currentObject = 0;
    vao = 0;
    vbo = 0;
}

UIManager::~UIManager()
{
    if (vao != 0)
        glDeleteVertexArrays(1, &vao);
    if (vbo != 0)
        glDeleteBuffers(1, &vbo);
}

void UIManager::init()
{
    screenWidth = globalScreenWidth / 2;
    screenHeight = globalScreenHeight / 2;

    Shader vShader, fShader;

    vShader.initFromFile(VERTEX_SHADER, "shaders/simple.vert");
    if (!vShader.isCompiled()) {
        std::cout << "UI Vertex Shader Error:\n" << vShader.log() << std::endl;
    }

    fShader.initFromFile(FRAGMENT_SHADER, "shaders/simple.frag");
    if (!fShader.isCompiled()) {
        std::cout << "UI Fragment Shader Error:\n" << fShader.log() << std::endl;
    }

    texProgram.init();
    texProgram.addShader(vShader);
    texProgram.addShader(fShader);
    texProgram.link();

    if (!texProgram.isLinked()) {
        std::cout << "UI Shader Linking Error:\n" << texProgram.log() << std::endl;
    }

    texProgram.bindFragmentOutput("outColor");

    vShader.free();
    fShader.free();

    initQuad();
}

void UIManager::initQuad()
{
    float vertices[] = {
        0.0f, 1.0f,  0.0f, 1.0f,
        1.0f, 1.0f,  1.0f, 1.0f,
        1.0f, 0.0f,  1.0f, 0.0f,

        0.0f, 1.0f,  0.0f, 1.0f,
        1.0f, 0.0f,  1.0f, 0.0f,
        0.0f, 0.0f,  0.0f, 0.0f
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

void UIManager::update(int deltaTime, Player* player)
{
    //if (player != nullptr) {
    //    health = player->getHealth();
    //    maxHealth = player->getMaxHealth();
    //}
}

void UIManager::render()
{
    glm::mat4 modelview;

    texProgram.use();
    setupViewport(0.1, 0.);

    texProgram.setUniformMatrix4f("projection", projection);
    texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);

    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Full background panel
    texProgram.setUniform4f("color", 0.0f, 0.0f, 0.0f, 1.0f);
    modelview = glm::mat4(1.0f);
    modelview = glm::scale(modelview, glm::vec3(screenWidth, screenHeight * 0.15f, 1.0f));
    texProgram.setUniformMatrix4f("modelview", modelview);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // HEALTH PANEL border
    texProgram.setUniform4f("color", 0.2f, 0.2f, 0.2f, 1.0f);
    renderPanel(glm::vec2(10, 10), glm::vec2(200, 40));

    // HEALTH BAR (red fill)
    float healthPercent = (maxHealth > 0) ? (float)health / (float)maxHealth : 0.0f;
    texProgram.setUniform4f("color", 0.8f, 0.1f, 0.1f, 1.0f);
    renderPanel(glm::vec2(15, 15), glm::vec2(190 * 0.1, 30));

    // CURRENT WEAPON
    texProgram.setUniform4f("color", 0.2f, 0.2f, 0.2f, 1.0f);
    renderPanel(glm::vec2(220, 10), glm::vec2(100, 40));

    // RANK panel
    texProgram.setUniform4f("color", 0.2f, 0.2f, 0.2f, 1.0f);
    renderPanel(glm::vec2(330, 10), glm::vec2(80, 40));

    // CURRENT OBJECT panel
    texProgram.setUniform4f("color", 0.2f, 0.2f, 0.2f, 1.0f);
    renderPanel(glm::vec2(420, 10), glm::vec2(100, 40));

    glBindVertexArray(0);
}

void UIManager::setupViewport(float heightPercent, float yOffsetPercent)
{
    int sectionHeight = static_cast<int>(screenHeight * heightPercent);
    int sectionY = static_cast<int>(screenHeight * yOffsetPercent);

    glViewport(0, sectionY, screenWidth, sectionHeight);
    projection = glm::ortho(0.f, static_cast<float>(screenWidth),
        static_cast<float>(sectionHeight), 0.f);
}

void UIManager::renderPanel(const glm::vec2& position, const glm::vec2& size)
{
    glm::mat4 modelview = glm::mat4(1.0f);
    modelview = glm::translate(modelview, glm::vec3(position.x, position.y, 0.0f));
    modelview = glm::scale(modelview, glm::vec3(size.x, size.y, 1.0f));

    texProgram.setUniformMatrix4f("modelview", modelview);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}