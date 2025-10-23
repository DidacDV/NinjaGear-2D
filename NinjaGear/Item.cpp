#include "Item.h"

Item::Item() : Sprite()
{
	name = "none";
	quantity = 1;
	description = "no desc";
}	

Item::Item(const glm::vec2& itemSize, const glm::vec2& sizeInSpritesheet,
	Texture* spriteTexture, ShaderProgram* program)
	: Sprite(itemSize, sizeInSpritesheet, spriteTexture, program)
{
	name = "set item";
	quantity = 1;
	description = "set desc";
}
Item::~Item()
{
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
