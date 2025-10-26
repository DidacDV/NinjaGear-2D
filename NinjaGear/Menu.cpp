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
}

void Menu::setMenuImage(const string& imagePath)
{
    menuImagePath = imagePath;
}

void Menu::setUpButtons() {
    if (type == MenuType::START) {
        addButton(glm::vec2(365, 360), glm::vec2(280, 65), "start");
		addButton(glm::vec2(365, 450), glm::vec2(280, 65), "settings");
    }
}

void Menu::addButton(const glm::vec2& pos, const glm::vec2& size, const string& action)
{
    Button btn;
    btn.position = pos;
    btn.size = size;
    btn.action = action;
    buttons.push_back(btn);

    cout << "Added button: " << action << " at (" << pos.x << ", " << pos.y
        << ") size (" << size.x << ", " << size.y << ")" << endl;
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