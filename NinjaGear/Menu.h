#ifndef _MENU_INCLUDE
#define _MENU_INCLUDE

#include "Texture.h"
#include "Sprite.h"
#include "MenuScene.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>

using namespace std;

struct Button {
    glm::vec2 position;  //top - left
    glm::vec2 size;      //width and height
    string action;       //action id
};

enum class MenuType {
    START,
    SETTINGS,
    DEATH,
    CREDITS
};

class Menu : public MenuScene
{
public:
    Menu(MenuType type = MenuType::START);
    ~Menu();

    void init();
    void update(int deltaTime);
    void render();
    
    MenuType getType();

    void handleClick(int mouseX, int mouseY);
    void setUpButtons();
    void addButton(const glm::vec2& pos, const glm::vec2& size, const string& action);

    void setMenuImage(const string& imagePath);

    //get action of clicked buttton
    string getClickedButton(int mouseX, int mouseY);

private:
    Texture menuTexture;
    Sprite* menuSprite;
    vector<Button> buttons;
    string menuImagePath;
    bool imageLoaded;
    MenuType type;
};

#endif