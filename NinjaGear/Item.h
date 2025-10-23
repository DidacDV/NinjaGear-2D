#pragma once
#ifndef ITEM_H
#define ITEM_H


#include "Sprite.h"

class Item : public Sprite
{
private:

public:
	Item(const glm::vec2& itemSize, const glm::vec2& sizeInSpritesheet,
		Texture* spriteTexture, ShaderProgram* program);
	Item(); //default constructor
	virtual ~Item();
	virtual void render(const glm::mat4& view);

	string getName();
	int getQuantity() const;
	string getDescription();

	void setName(const string& name);
	void setQuantity(int quantity);
	void setDescription(const string& description);

private:
	string name;
	int quantity;
	string description;
};

#endif