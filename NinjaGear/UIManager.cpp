#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "UIManager.h"
#include "Globals.h"
#include "UIManagerUtils.h"

UIManager::UIManager()
{
    health = 3.5f;
    maxHealth = 5.f;
	currentItemName = "";
    currentWeaponName = "";
    rank = 2;
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
    weaponTextures.clear();
    objectTextures.clear();
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

    initTextures();

    textRenderer = new Text("resources/NormalFont.ttf", FONT_SIZE, screenWidth, screenHeight * 0.15);
    calculateLayout();
    
    initQuad();
}

void UIManager::initTextures() {
    if (!fullHeartTexture.loadFromFile("images/ui/FullHeart.png", TEXTURE_PIXEL_FORMAT_RGBA)) {
        std::cout << "ERROR: Could not load star_filled.png" << std::endl;
    }
    if (!halfHeartTexture.loadFromFile("images/ui/HalfHeart.png", TEXTURE_PIXEL_FORMAT_RGBA)) {
        std::cout << "ERROR: Could not load star_empty.png" << std::endl;
    }
    if (!emptyHeartTexture.loadFromFile("images/ui/EmptyHeart.png", TEXTURE_PIXEL_FORMAT_RGBA)) {
        std::cout << "ERROR: Could not load star_empty.png" << std::endl;
    }

    rankTextures = vector<Texture>(maxRank);

    for (int i = 0; i < maxRank; i++) {
        string path = "images/ui/Rank" + std::to_string(i + 1) + ".png";
        if (!rankTextures[i].loadFromFile(path, TEXTURE_PIXEL_FORMAT_RGBA)) {
            std::cout << "ERROR: Could not load " << path << std::endl;
        }
    }

    if (!dialogBoxTexture.loadFromFile("images/ui/DialogBox.png", TEXTURE_PIXEL_FORMAT_RGBA)) {
        std::cout << "ERROR: Could not load DialogBox.png" << std::endl;
    }
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
    uiPositions.rank_value_pos = glm::vec2(MARGIN_LEFT + 150, MARGIN_TOP + LINE_HEIGHT);

    uiPositions.weapon_text_pos = glm::vec2(rightColumnX, MARGIN_TOP);
    uiPositions.weapon_value_pos = glm::vec2(rightColumnX + 95, MARGIN_TOP - 5);  // After text

    uiPositions.object_text_pos= glm::vec2(rightColumnX, MARGIN_TOP + LINE_HEIGHT);
    uiPositions.object_value_pos = glm::vec2(rightColumnX + 95, MARGIN_TOP + LINE_HEIGHT - 5);
}

//TODO: use player getter functions
void UIManager::update(int deltaTime, Player* player) {
    if (player != nullptr) {
        health = player->getHealth();
        rank = 1;

        Item* currentItem = player->getCurrentItem();
        if (currentItem != nullptr) {
            currentItemName = currentItem->getName();
            currentItemQuantity = player->getItemQuantity(currentItemName);
        }
        else {
            currentItemName = "";
        }

        Item* currentWeapon = player->getCurrentWeapon();
        if (currentWeapon != nullptr) {
            currentWeaponName = currentWeapon->getName();
        }
        else {
            currentWeaponName = "Punch"; 
        }
    }
    updateTemporaryMessages(deltaTime);
}

void UIManager::renderFixedText() {
    textRenderer->RenderText("LIFE", uiPositions.life_text_pos.x, uiPositions.life_text_pos.y, 0.8, glm::vec3(1.f));
    textRenderer->RenderText("NINJA  RANK", uiPositions.rank_text_pos.x, uiPositions.rank_text_pos.y, 0.8, glm::vec3(1.f));

    textRenderer->RenderText("WEAPON", uiPositions.weapon_text_pos.x, uiPositions.weapon_text_pos.y, 0.8, glm::vec3(1.f));
    textRenderer->RenderText("OBJECT", uiPositions.object_text_pos.x, uiPositions.object_text_pos.y, 0.8, glm::vec3(1.f));
}

void UIManager::render()
{
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 modelview;

    texProgram.use();
    setupViewport(0.15, 0.); //15% of the screen

    texProgram.setUniformMatrix4f("projection", projection);
    texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);

    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Full background panel
    texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
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
    renderRank(uiPositions.rank_value_pos, rank);
    glBindVertexArray(vao);
    texProgram.use();
    texProgram.setUniformMatrix4f("projection", projection);
    texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);

    if (!currentWeaponName.empty()) {
        string weaponLower = currentWeaponName;
        transform(weaponLower.begin(), weaponLower.end(), weaponLower.begin(), ::tolower);
        renderItem(uiPositions.weapon_value_pos, "images/weapons/", weaponLower, 1);
    }

    // Render current inventory item
    if (!currentItemName.empty()) {
        string itemLower = currentItemName;
        transform(itemLower.begin(), itemLower.end(), itemLower.begin(), ::tolower);
        renderItem(uiPositions.object_value_pos, "images/items/", itemLower, currentItemQuantity);
    }

    glBindVertexArray(0);
}

//curently used for text when picking up items. maybe dialog too?
void UIManager::renderGameOverlay()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    // Setup viewport for game area (85% of screen, top portion)
    int gameHeight = static_cast<int>(screenHeight * 0.85f);
    int gameY = static_cast<int>(screenHeight * 0.15f);  // Start above UI panel

    glViewport(0, gameY, screenWidth, gameHeight);

    // Create projection for game viewport
    glm::mat4 gameProjection = glm::ortho(0.f, static_cast<float>(screenWidth),
        static_cast<float>(gameHeight), 0.f);

    // Update text renderer projection temporarily
    textRenderer->UpdateScreenSize(screenWidth, gameHeight);

    // Render temporary messages
    renderTemporaryMessages();

    // Restore text renderer to UI dimensions
    textRenderer->UpdateScreenSize(screenWidth, screenHeight * 0.15f);
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
    glBindVertexArray(vao);
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
    glBindTexture(GL_TEXTURE_2D, 0);
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

void UIManager::renderRank(const glm::vec2& position, int currentRank) {
    if (currentRank < 0) currentRank = 0;
    if (currentRank >= maxRank) currentRank = maxRank - 1;


    renderPanel(glm::vec2(position.x, position.y - 10), glm::vec2(ICON_SIZE), &rankTextures[rank]);

    bool pulseEffect = true;
    UIManagerUtils::renderStyledText(textRenderer, ranks[currentRank].name, glm::vec2(position.x + ICON_TEXT_MARGIN, position.y - 5), 1.f, ranks[currentRank].baseColor, pulseEffect, ranks[currentRank].colorA, ranks[currentRank].colorB);
    glBindVertexArray(vao);
    texProgram.use();
}

void UIManager::renderItem(const glm::vec2& position, const string& basePath, const string& itemName, int quantity)
{
    bool isWeapon = basePath.find("weapons") != std::string::npos;
    Texture* tex = UIManagerUtils::findOrLoadItemTexture(basePath, itemName, isWeapon, weaponTextures, objectTextures);

    if (!tex) {
        std::cout << "Failed to load texture for item: " << itemName << std::endl;
        return;
	}

    //icon
    renderPanel(glm::vec2(position.x, position.y - 3), glm::vec2(ICON_SIZE, ICON_SIZE), tex); // - 3 --> center with label, TODO try with different weapons etc
    //label
    std::string itemNameUpper = UIManagerUtils::toUppercase(itemName);
    if (quantity > 1) {
        itemNameUpper += " X" + std::to_string(quantity);
    }
    textRenderer->RenderText(itemNameUpper, position.x + ICON_TEXT_MARGIN, position.y + 5, 0.8f, glm::vec3(1.f));

    glBindVertexArray(vao);
    texProgram.use();
}

void UIManager::renderTemporaryMessages()
{
    int gameHeight = static_cast<int>(screenHeight * 0.85f);
    glm::mat4 gameProjection = glm::ortho(0.f, static_cast<float>(screenWidth),
        static_cast<float>(gameHeight), 0.f);

    texProgram.use();
    texProgram.setUniformMatrix4f("projection", gameProjection);

    for (const auto& msg : temporaryMessages)
    {
        glm::vec3 color = msg.color;
        float alpha = msg.remainingTime / 500.0f;
        if (msg.remainingTime < 500) {
            color *= alpha;
        }

        showMessageAndDialog(msg, alpha, color);
    }
}

void UIManager::showMessageAndDialog(TemporaryMessage msg, float alpha, glm::vec3 color) {
    float paddingX = 8.0f;
    float paddingY = 4.0f;

    float estimatedWidth = msg.text.size() * FONT_SIZE * msg.scale * 0.9f;
    float estimatedHeight = FONT_SIZE * msg.scale * 1.4f;

    glm::vec2 boxPos = glm::vec2(msg.position.x - paddingX, msg.position.y - paddingY - 10);
    glm::vec2 boxSize = glm::vec2(estimatedWidth + paddingX * 1.1, estimatedHeight + paddingY * 2);

    glBindVertexArray(vao);
    renderPanel(boxPos, boxSize, &dialogBoxTexture, glm::vec4(1.0f, 1.0f, 1.0f, alpha));

    textRenderer->RenderText(msg.text, msg.position.x, msg.position.y, msg.scale, color);
}

void UIManager::showTemporaryMessage(const std::string& text, const glm::vec2& position,
    float scale, const glm::vec3& color, int durationMs)
{
    TemporaryMessage msg;
    msg.text = text;
    msg.position = position;
    msg.scale = scale;
    msg.color = color;
    msg.remainingTime = durationMs;
    msg.totalDuration = durationMs;

    temporaryMessages.push_back(msg);
}

//just to delete those who are expired
void UIManager::updateTemporaryMessages(int deltaTime) {
    for (int i = temporaryMessages.size() - 1; i >= 0; i--)
    {
        temporaryMessages[i].remainingTime -= deltaTime;
        //remove expired messages
        if (temporaryMessages[i].remainingTime <= 0)
        {
            temporaryMessages.erase(temporaryMessages.begin() + i);
        }
    }
}