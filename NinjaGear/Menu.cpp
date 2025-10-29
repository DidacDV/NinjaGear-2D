#include "Menu.h"
#include "Game.h"
#include <iostream>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include "ServiceLocator.h"

Menu::Menu(MenuType menuType, const string& musicFile) : MenuScene()
{
    menuSprite = NULL;
    imageLoaded = false;
    type = menuType;
    music = musicFile;
    currentMusicFile = musicFile;
}

Menu::~Menu()
{
    if (menuSprite != NULL)
        delete menuSprite;
}

void Menu::init()
{
    MenuScene::menuImagePath = this->menuImagePath;
    // Start music
    if (music != "") ServiceLocator::getAudio().playMusic(music.c_str());
    setUpButtons();
    MenuScene::init();
}

MenuType Menu::getType()
{
    return type;
}

void Menu::update(int deltaTime)
{
	MenuScene::update(deltaTime);
}

void Menu::render()
{
    MenuScene::render();
    if (type == MenuType::DEATH || type == MenuType::CREDITS) {
        if (Game::instance().getKey(GLFW_KEY_ESCAPE)) {
            Game::instance().setCurrentScene("menu");
        }
    }
}

void Menu::setMenuImage(const string& imagePath)
{
    menuImagePath = imagePath;
}

void Menu::setUpButtons() {
    if (type == MenuType::START) {
        addButton(glm::vec2(0.38, 0.68f), glm::vec2(0.29f, 0.125f), "start"); //0.38 -> 365/960, where 360 was original position on my 1920width screen
        addButton(glm::vec2(0.38, 0.835f), glm::vec2(0.29f, 0.125f), "settings");
    }
    if (type == MenuType::DEATH) {
        addButton(glm::vec2(0.325, 0.73), glm::vec2(0.38f, 0.116f), "start");
    }
    if (type == MenuType::CREDITS) {
        addButton(glm::vec2(0.328, 0.78), glm::vec2(0.36f, 0.12f), "start");
    }
}

void Menu::addButton(const glm::vec2& relativePos, const glm::vec2& relativeSize, const string& action)
{
    Button btn;

    btn.position = glm::vec2(relativePos.x * globalScreenWidth,
        relativePos.y * globalScreenHeight);
    btn.size = glm::vec2(relativeSize.x * globalScreenWidth,
        relativeSize.y * globalScreenHeight);
    btn.action = action;

    buttons.push_back(btn);

    std::cout << "Added button: " << action << " at (" << btn.position.x << ", " << btn.position.y
        << ") size (" << btn.size.x << ", " << btn.size.y << ")" << std::endl;
}

string Menu::getClickedButton(int mouseX, int mouseY)
{
    for (const Button& btn : buttons) {
        //check if mouse click is within button bounds
        if (mouseX >= btn.position.x &&
            mouseX <= btn.position.x + btn.size.x &&
            mouseY >= btn.position.y &&
            mouseY <= btn.position.y + btn.size.y) {
            return btn.action;
        }
    }
    return ""; //no btn
}

void Menu::handleClick(int mouseX, int mouseY)
{
    string action = getClickedButton(mouseX, mouseY);

    if (!action.empty()) {
        cout << "Button clicked: " << action << endl;

        if (action == "start") {
            Game::instance().setCurrentScene("outside");
        }
        else if (action == "settings") {
            Game::instance().setCurrentScene("settings");
        }
        else if (action == "back") {
            Game::instance().setCurrentScene("menu");
        }
        else if (action == "exit") {
            
        }
    }
}