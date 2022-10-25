#include "Inventory.h"

Item::Type Item::getType() const { return type; }
int Item::getLevel() const { return level; }
const string& Item::getName() const { return name; }
int Item::getWeight() const { return weight; }
Effect Item::getEffect() const { return effect; }
Effect Item::getSpecial() const { return special; }
int Item::getMana() const { return mana; }
int Item::getPrice() const { return price; }
string Item::getEffectStr() const
{
	return getStr(flag_effects);
}
string Item::getPriceStr() const
{
	return getStr(flag_price);
}
string Item::getWeightStr() const
{
	return getStr(flag_weight);
}
string Item::getStr(int flags) const
{
	std::ostringstream str{};
	str << name;
	if (flags & flag_effects)
	{
		str << " [" << effect.data->name << ": " << effect.stacks;
		if (special.stacks)
			str << ", " << special.data->name << ": " << special.stacks;
		str << ']';
	}
	if(flags & flag_weight)
		str << '[' << weight << " slots]";
	if(flags & flag_price)
		str << '[' << price << " gp]";

	return str.str();
}
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
bool Item::isEmpty() const { return type == Empty; }
bool Item::isConsumable() const { return type == Tool || type == Crystal; }
void Item::remove()
{
	type = Empty;
	weight = 0;
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
	stream >> item.level;

	util::getline(stream, str);
	item.type = Item::_typemap->getID(str);

	stream >> item.weight;
	stream >> item.mana;
	stream >> item.price;

	util::getline(stream, str);
	stream >> num;
	item.effect = EffectDataList.getdatabyname(str)->make(num);

	// Look for a closing bracket
	stream.ignore(util::STREAMMAX, ']');

	return stream;
}

const Inventory::iterator Inventory::slot_begin() { return items.begin() + Slot1; }
const Inventory::iterator Inventory::slot_end() { return items.begin() + Slot4 + 1; }
Inventory::iterator Inventory::findempty()
{
	return std::find_if(slot_begin(), slot_end(), [](const Item& i) {return i.isEmpty(); });
}

void Inventory::verify() const
{
	// Armor Check
	if (items[SlotArmor].getType() != Item::Empty
		&& items[SlotArmor].getType() != Item::Armor)
		throw std::overflow_error{ "Armor slot has something other than armor" };

	// Consumable Check
	if (items[SlotConsumable].getType() != Item::Empty
		&& items[SlotConsumable].isConsumable())
		throw std::overflow_error{ "Consumable slot has something other than consumable" };

	// Shield and Weight Check
	bool hasshield{};
	int weightsum{};
	for (int i{}; i < SLOTS_TOTAL; ++i)
	{
		if (items[i].getType() == Item::Shield)
		{
			if (hasshield)
				throw std::overflow_error{ "More than one shield in inventory" };
			else
				hasshield = true;
		}

		if (i != SlotArmor && items[i].getType() == Item::Armor)
			throw std::overflow_error{ "Armor is in a non-armor slot" };

		weightsum += items[i].getWeight();
	}

	if (weightsum > MAX_WEIGHT)
		throw std::overflow_error{ "Too much being carried" };
}
void Inventory::sort()
{
	if (m_sorted)
		return;

	// Verify inventory is not broken
	verify();

	int start = Slot1;
	int shield{-1};
	int consumable{-1};

	// Get indices
	for (int i{ Slot1 }; i <= Slot4; ++i)
	{
		if (items[i].getType() == Item::Shield)
			shield = i;
		else if (items[i].isConsumable())
			consumable = i;
	}

	// Move a consumable to the consumable slot (if there's room)
	if (consumable > 0 && items[SlotConsumable].isEmpty())
		std::swap(items[SlotConsumable], items[consumable]);

	// Move shield to the first slot
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

	// If there is an available slot, and no unarmed weapon, add one
	for (int i{ start }; i <= Slot4; ++i)
	{
		if (items[i].getType() == Item::Unarmed)
			break;

		if (items[i].getType() == Item::Empty)
			items[i] = *ItemBaseList.getdatabyname("Unarmed");
	}

	m_sorted = true;
}
int Inventory::slotsAvailable() const
{
	if (!m_sorted)
		throw std::range_error{ "Inventory not sorted" };

	int empty{ std::count_if(items.begin() + Slot1, items.begin() + Slot4 + 1,
		[](const Item& i)
		{
			return i.isEmpty();
		}) };

	return MULTI_SLOTS - empty;
}
int Inventory::weightAvailable() const
{
	int sum{};
	for (const Item& i : items)
		sum += i.getWeight();
	return MAX_WEIGHT - sum;
}
bool Inventory::hasRoomFor(const Item& item) const
{
	if (!m_sorted)
		throw std::range_error{ "Inventory not sorted" };

	bool hasroom{};
	switch (item.getType())
	{
	case Item::Armor:
		if(items[SlotArmor].isEmpty())
			hasroom = true;
		break;
	case Item::Shield:
		if (items[Slot1].getType() != Item::Shield
			&& slotsAvailable() >= item.getWeight())
			hasroom = true;
		break;
	case Item::Tool:
	case Item::Crystal:
		if (items[SlotConsumable].isEmpty())
			hasroom = true;
		break;
	}

	// Could have used a fallthrough in the switch,
	//   but those can be hard to read.
	// In the interest of consistency, chose to also
	//   avoid using a return in the switch.

	if (!hasroom && item.getType() != Item::Armor && item.getType() != Item::Shield)
		hasroom = slotsAvailable() && weightAvailable() >= item.getWeight();

	return hasroom;
}
bool Inventory::hasShield() const
{
	if (!m_sorted)
		throw std::range_error{ "Inventory not sorted" };

	return items[i].getType() == Item::Shield;
}

const Item& Inventory::getItem(int slot) const
{
	return items[slot];
}
const Item& Inventory::getArmor() { return items[SlotArmor]; }
const Item& Inventory::getConsumable() { return items[SlotConsumable]; }
void Inventory::equipItem(const Item& item)
{
	// Verify that there is room
	if(!hasRoomFor(item))
		throw std::range_error{ "Not enough room for the item, drop something first" };

	// Where is the room
	auto ptr{ items.end() };
	switch (item.getType())
	{
	case Item::Armor:
		ptr = items.begin() + SlotArmor;
		break;
	case Item::Tool:
	case Item::Crystal:
		if (items[SlotConsumable].isEmpty())
			ptr = items.begin() + SlotConsumable;
		break;
	}

	// Room not found yet
	if (ptr == items.end())
		ptr = findempty();

	// Assign the item and sort the inventory
	*ptr = item;
	sort();
}
void Inventory::useItem(int slot)
{
	if (items[slot].getType() == Item::Tool
		|| items[slot].getType() == Item::Crystal)
		dropItem(slot);
}
void Inventory::dropItem(int slot)
{
	items[slot].remove();
	sort();
}
int Inventory::getGold() const { return gold; }
void Inventory::addGold(int _gold)
{
	if (_gold < 0)
		throw std::invalid_argument{ "Cannot add negative gold. Use spendGold() instead." };
	gold += _gold;
}
void Inventory::spendGold(int _gold)
{
	if (_gold > gold)
		throw std::underflow_error{ "Not enough gold in inventory. Verify amount before calling spendGold()" };
	gold -= _gold;
}
void Inventory::buyItem(const Item& item)
{
	spendGold(item.getPrice());
	equipItem(item);
}
util::NameArray<Inventory::SLOTS_TOTAL> Inventory::slotnames{};