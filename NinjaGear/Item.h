#pragma once
#ifndef ITEM_H
#define ITEM_H


#include "Sprite.h"

class Item : public Sprite
{
private:

public:
	Item(const glm::vec2& itemSize, const glm::vec2& sizeInSpritesheet,
		Texture* spriteTexture, ShaderProgram* program, glm::vec2 tileMapDispl);
	Item(); //default constructor
	virtual ~Item();
	virtual void render(const glm::mat4& view);

	string getName();
	int getQuantity() const;
	string getDescription();
	glm::vec2 getPosition();
	bool getIsWeapon() const { return isWeapon; }

	void setItem(string name, int quantity, string description, glm::vec2 position, bool isWeapon, int tileSize);
	void setName(const string& name);
	void setQuantity(int quantity);
	void setDescription(const string& description);

private:
	string name;
	int quantity;
	string description;
	glm::vec2 position;
	bool isWeapon;
};

#endif