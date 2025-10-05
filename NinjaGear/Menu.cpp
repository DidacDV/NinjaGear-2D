#include "Menu.h"
#include <iostream>


Menu::Menu()
{
	Scene::Scene();
}

Menu::Menu(const vector<string>& tileMapFiles)
	: Scene(tileMapFiles)
{
}

Menu::~Menu()
{
	Scene::~Scene();
}

void Menu::init()
{
	Scene::init();
}

void Menu::update(int deltaTime)
{
	Scene::update(deltaTime);
}

void Menu::render()
{
	Scene::render();
}