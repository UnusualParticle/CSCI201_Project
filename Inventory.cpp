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

	return *this;
}
util::NameArray<Item::TYPES_TOTAL> Item::typenames{};

void Inventory::sort()
{
	// Verify inventory is not broken
	if (items[Armor].getType() != Item::Empty
		|| items[Armor].getType() != Item::Armor)
		throw std::overflow_error{ "Armor slot has something other than armor" };

	if (items[Consumable].getType() != Item::Empty
		|| items[Consumable].getType() != Item::Consumable)
		throw std::overflow_error{ "Consumable slot has something other than consumable" };

	int shield{ -1 };
	for (int i{ Slot1 }; i <= Slot4; ++i)
	{
		if (items[i].getType() == Item::Armor)
			throw std::overflow_error{ "Item slot has armor" };

		if (items[i].getType() == Item::Shield)
		{
			if (shield >= Slot1)
				throw std::overflow_error{ "Inventory has two shields" };
			else
				shield = i;
		}
	}

	// Move the shield to the first slot
	int start = Slot1;
	if (shield > Slot1)
	{
		std::swap(items[Slot1], items[shield]);
		start = Slot2;
	}

	// Sort the remaining items, heaviest first
	std::sort(items.begin() + start, items.begin() + Slot4 + 1,
		[](const Item& item1, const Item& item2) {
			return item1.getWeight() > item2.getWeight();
		});


}
const Item& Inventory::getArmor() { return items[Armor]; }
const Item& Inventory::getConsumable() { return items[Consumable]; }
int Inventory::slotsAvailable()
{
	return std::count_if(items.begin(), items.end(), [](const Item& i) { return i.getType() == Item::Empty; });
}
void Inventory::equipArmor(const Item& armor)
{ 
	if (armor.getType() != Item::Armor)
		throw std::invalid_argument{ "Cannot put non armor in the armor slot" };
	items[0] = armor;
}
void Inventory::equipItem(const Item& item)
{
	if (item.getType() == Item::Armor)
		throw std::invalid_argument{ "Cannot put armor in a non-armor slot" };
	if (item.getWeight() > slotsAvailable())
		throw std::range_error{ "Not enough slots for the item" };

}
util::NameArray<Inventory::SLOTS_TOTAL> Inventory::slotnames{};