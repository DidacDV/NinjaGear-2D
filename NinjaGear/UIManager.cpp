#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "UIManager.h"
#include "Globals.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

UIManager::UIManager()
{
    health = 3.5f;
    maxHealth = 5.f;
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

    vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
    if (!vShader.isCompiled()) {
        std::cout << "UI Vertex Shader Error:\n" << vShader.log() << std::endl;
    }

    fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
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

    if (!fullHeartTexture.loadFromFile("images/ui/FullHeart.png", TEXTURE_PIXEL_FORMAT_RGBA)) {
        std::cout << "ERROR: Could not load star_filled.png" << std::endl;
    }
    if (!halfHeartTexture.loadFromFile("images/ui/HalfHeart.png", TEXTURE_PIXEL_FORMAT_RGBA)) {
        std::cout << "ERROR: Could not load star_empty.png" << std::endl;
    }
    if (!emptyHeartTexture.loadFromFile("images/ui/EmptyHeart.png", TEXTURE_PIXEL_FORMAT_RGBA)) {
        std::cout << "ERROR: Could not load star_empty.png" << std::endl;
    }


    textRenderer = new Text("resources/NormalFont.ttf", 24, screenWidth, screenHeight * 0.15);
    calculateLayout();
    
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

void UIManager::calculateLayout() {
    float rightColumnX = screenWidth - screenWidth / 3;
    float LINE_HEIGHT = screenHeight * 0.15f / 2;
    uiPositions.life_text_pos = glm::vec2(MARGIN_LEFT, MARGIN_TOP);
    uiPositions.life_value_pos = glm::vec2(MARGIN_LEFT + 70, MARGIN_TOP);  // After "LIFE" text

    uiPositions.rank_text_pos = glm::vec2(MARGIN_LEFT, MARGIN_TOP + LINE_HEIGHT);
    uiPositions.rank_value_pos = glm::vec2(MARGIN_LEFT + 70, MARGIN_TOP + LINE_HEIGHT);

    uiPositions.weapon_text_pos = glm::vec2(rightColumnX, MARGIN_TOP);
    uiPositions.weapon_value_pos = glm::vec2(rightColumnX + 110, MARGIN_TOP - 5);  // After text

    uiPositions.object_text_pos= glm::vec2(rightColumnX, MARGIN_TOP + LINE_HEIGHT);
    uiPositions.object_value_pos = glm::vec2(rightColumnX + 110, MARGIN_TOP + LINE_HEIGHT - 5);
}

void UIManager::update(int deltaTime, Player* player)
{
    //if (player != nullptr) {
    //    health = player->getHealth();
    //    maxHealth = player->getMaxHealth();
    //}
}

void UIManager::renderFixedText() {
    textRenderer->RenderText("LIFE", uiPositions.life_text_pos.x, uiPositions.life_text_pos.y, 0.8, glm::vec3(1.0f));
    textRenderer->RenderText("NINJA  RANK", uiPositions.rank_text_pos.x, uiPositions.rank_text_pos.y, 0.8, glm::vec3(1.0f));

    textRenderer->RenderText("WEAPON", uiPositions.weapon_text_pos.x, uiPositions.weapon_text_pos.y, 0.8, glm::vec3(1.0f));
    textRenderer->RenderText("OBJECT", uiPositions.object_text_pos.x, uiPositions.object_text_pos.y, 0.8, glm::vec3(1.0f));
}

void UIManager::render()
{
    glm::mat4 modelview;

    texProgram.use();
    setupViewport(0.15, 0.); //15% of the screen

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

    renderFixedText();

    glBindVertexArray(vao);
    texProgram.use();
    texProgram.setUniformMatrix4f("projection", projection);
    texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);

    renderHealth(uiPositions.life_value_pos, health, maxHealth);

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

void UIManager::renderPanel(const glm::vec2& position, const glm::vec2& size, Texture* texture, const glm::vec4& color)
{
    if (texture)
        texture->use();
    else
        glBindTexture(GL_TEXTURE_2D, 0);

    texProgram.setUniform4f("color", color.r, color.g, color.b, color.a);

    glm::mat4 modelview(1.0f);
    modelview = glm::translate(modelview, glm::vec3(position.x, position.y, 0.0f));
    modelview = glm::scale(modelview, glm::vec3(size.x, size.y, 1.0f));
    texProgram.setUniformMatrix4f("modelview", modelview);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void UIManager::renderHealth(const glm::vec2& position, float currentHealth, float maxHealth)
{
    float heartSize = 24.f;
    float heartSpacing = 25.0f;

    for (int i = 0; i < (int)maxHealth; ++i) {
        Texture* tex = nullptr;
        float heartValue = currentHealth - i;

        if (heartValue >= 1.0f) {
            tex = &fullHeartTexture;          
        }
        else if (heartValue >= 0.5f) {
            tex = &halfHeartTexture;      
        }
        else {
            tex = &emptyHeartTexture;     
        }

        glm::vec2 heartPos = position + glm::vec2(i * heartSpacing, -3); //-5 to center hearts with life 
        renderPanel(heartPos, glm::vec2(heartSize, heartSize), tex);
    }
}