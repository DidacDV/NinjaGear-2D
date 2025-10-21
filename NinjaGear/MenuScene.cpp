#include "MenuScene.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;

MenuScene::MenuScene()
{
    menuSprite = nullptr;
    imageLoaded = false;
}

MenuScene::~MenuScene()
{
    if (menuSprite != nullptr)
        delete menuSprite;
}

void MenuScene::init()
{
    initShaders();

    if (!menuImagePath.empty() && !imageLoaded) {
        menuTexture.loadFromFile(menuImagePath, TEXTURE_PIXEL_FORMAT_RGBA);
        menuTexture.setMinFilter(GL_NEAREST);
        menuTexture.setMagFilter(GL_NEAREST);
        menuSprite = Sprite::createSprite(glm::ivec2(globalScreenWidth / 2, globalScreenHeight / 2),
            glm::vec2(1.0f, 1.0f),
            &menuTexture,
            &texProgram);

        if (menuSprite != NULL) {
            menuSprite->setPosition(glm::vec2(0, 0));
            imageLoaded = true;
            cout << "Menu image loaded successfully: " << menuImagePath << endl;
        }
    }
}

void MenuScene::update(int deltaTime)
{
    currentTime += deltaTime;
}

void MenuScene::render()
{

    resetViewportFullScreen();

    glm::mat4 modelview;
    texProgram.use();

    glm::mat4 projection = glm::ortho(0.f, float(globalScreenWidth / 2),
        float(globalScreenHeight / 2), 0.f);

    texProgram.setUniformMatrix4f("projection", projection);
    texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
    modelview = glm::mat4(1.0f);
    texProgram.setUniformMatrix4f("modelview", modelview);
    texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);

    if (menuSprite != NULL && imageLoaded) {
        menuSprite->render();
    }
}