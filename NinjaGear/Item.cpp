#include "Item.h"

Item::Item() : Sprite()
{
	name = "none";
	quantity = 1;
	description = "no desc";
}	

Item::Item(const glm::vec2& itemSize, const glm::vec2& sizeInSpritesheet,
	Texture* spriteTexture, ShaderProgram* program, glm::vec2 tileMapDispl)
	: Sprite(itemSize, sizeInSpritesheet, spriteTexture, program)
{
	name = "set item";
	quantity = 1;
	description = "set desc";
}
Item::~Item()
{
}

void Item::setItem(string name, int quantity, string description, glm::vec2 position, int tileSize) {
	this->name = name;
	this->quantity = quantity;
	this->description = description;

	this->position = glm::vec2(position.x * tileSize, position.y * tileSize);
	this->setPosition(glm::vec2(position.x * tileSize, position.y * tileSize));
}

string Item::getName()
{
	return name;
}

int Item::getQuantity() const
{
	return quantity;
}

string Item::getDescription()
{
	return description;
}

glm::vec2 Item::getPosition()
{
	return position;
}

void Item::setName(const string& itemName)
{
	name = itemName;
}

void Item::setQuantity(int qty)
{
	quantity = qty;
}

void Item::setDescription(const string& desc)
{
	description = desc;
}

void Item::render(const glm::mat4& view)
{
	Sprite::render(view);
}


//void Item::setPosition(const glm::vec2& pos)
//{
//	sprite->setPosition(glm::vec2(float(tileMapDispl.x + pos.x), float(tileMapDispl.y + pos.y)));
//}