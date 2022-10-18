#include "Inventory.h"

Item::Type Item::getType() const { return type; }
const string& Item::getName() const { return name; }
int Item::getWeight() const { return weight; }
Effect Item::getEffect() const { return effect; }
int Item::getMagika() const { return magika; }
int Item::getPrice() const { return price; }
Item& Item::operator=(const Item& other)
{
	type = other.type;
	weight = other.weight;
	name = other.name;
	effect.data = other.effect.data;
	effect.stacks = other.effect.stacks;
	magika = other.magika;
	price = other.price;
}

int Inventory::slotsAvailable()
{
	return std::count_if(items.begin(), items.end(), [](const Item& i) {i.getType() == Item::Empty; });
}
void Inventory::equipArmor(Item armor)
{ 
	if (armor.getType() != Item::Armor)
		throw std::invalid_argument{ "Cannot put non armor in the armor slot" };
	items[0] = armor;
}
void Inventory::equipItem(Item item)
{
	if (item.getType() == Item::Armor)
		throw std::invalid_argument{ "Cannot put armor in a non-armor slot" };
	if (item.getWeight() > slotsAvailable())
		throw std::range_error{ "Not enough slots for the item" };

}