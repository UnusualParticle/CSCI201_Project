#include "Inventory.h"

Item::Type Item::getType() const { return type; }
const string& Item::getName() const { return name; }
int Item::getWeight() const { return weight; }
Effect Item::getEffect() const { return effect; }
Effect Item::getSpecial() const { return special; }
int Item::getMana() const { return mana; }
int Item::getPrice() const { return price; }
Item& Item::operator=(const Item& other)
{
	type = other.type;
	weight = other.weight;
	name = other.name;
	effect.data = other.effect.data;
	effect.stacks = other.effect.stacks;
	mana = other.mana;
	price = other.price;

	return *this;
}
util::NameMap<Item::Type>* Item::_typemap{};
util::NameArray<Item::TYPES_TOTAL> Item::typenames{};

std::ifstream& operator>>(std::ifstream& stream, Item& item)
{
	// Look for an opening bracket
	stream.ignore(util::STREAMMAX, '[');
	if (!stream.good())
		return stream;

	// Get the data
	string str;
	int num;

	util::getline(stream, item.name);
	
	util::getline(stream, str);
	item.type = Item::_typemap->getID(str);

	stream >> item.weight;

	util::getline(stream, str);
	stream >> num;
	item.effect = EffectDataList.getdatabyname(str)->make(num);

	stream >> item.mana;
	stream >> item.price;

	// Look for a closing bracket
	stream.ignore(util::STREAMMAX, ']');

	return stream;
}

void Inventory::sort()
{
	// Verify inventory is not broken
	if (items[SlotArmor].getType() != Item::Empty
		&& items[SlotArmor].getType() != Item::Armor)
		throw std::overflow_error{ "Armor slot has something other than armor" };

	if (items[SlotConsumable].getType() != Item::Empty
		&& items[SlotConsumable].getType() != Item::Tool
		&& items[SlotConsumable].getType() != Item::Crystal)
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

	// If there is an available slot, add an unarmed weapon
	if (slotsAvailable() > 0)
	{
		auto ptr{ findtype(Item::Empty) };
		*ptr = *ItemBaseList.getdatabyname("Unarmed");
	}
}
std::array<Item, Inventory::SLOTS_TOTAL>::iterator Inventory::findtype(Item::Type type)
{
	return std::find_if(items.begin(), items.end(), [type](const Item& i) {return i.getType() == type; });
}

const Item& Inventory::getItem(int slot)
{
	return items[slot];
}
const Item& Inventory::getArmor() { return items[SlotArmor]; }
const Item& Inventory::getConsumable() { return items[SlotConsumable]; }
int Inventory::slotsAvailable()
{
	int sum{};
	for (const auto& i : items)
		sum += i.getWeight();
	return sum;
}
void Inventory::equipArmor(const Item& armor)
{ 
	if (armor.getType() != Item::Armor)
		throw std::invalid_argument{ "Cannot put non armor in the armor slot" };
	items[SlotArmor] = armor;
}
void Inventory::equipItem(const Item& item)
{
	if (item.getType() == Item::Armor)
		throw std::invalid_argument{ "Cannot put armor in a non-armor slot" };
	if (item.getType() == Item::Shield && findtype(Item::Shield) != items.end())
	{
		if (findtype(Item::Shield) != items.end())
			throw std::range_error{ "Cannot have more than one shield equipped" };
	}
	if (item.getWeight() > slotsAvailable())
		throw std::range_error{ "Not enough slots for the item" };

	auto ptr{ findtype(Item::Empty)};
	*ptr = item;
}
void Inventory::equipConsumable(const Item& tool)
{
	if (tool.getType() != Item::Tool
		&& tool.getType() != Item::Crystal)
		throw std::invalid_argument{ "Cannot put non consumable in the consumable slot" };
	items[SlotConsumable] = tool;
}
void Inventory::useItem(int slot)
{
	if (items[slot].getType() == Item::Tool
		|| items[slot].getType() == Item::Crystal)
		dropItem(slot);
}
void Inventory::dropItem(int slot)
{
	items[slot] = Item{};
}
util::NameArray<Inventory::SLOTS_TOTAL> Inventory::slotnames{};