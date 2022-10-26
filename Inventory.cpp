#include "Inventory.h"

/* * * * * *
*	Item   *
* * * * * */

// Static Variables
util::NameMap<Item::Type>* Item::_typemap{};
util::NameArray<Item::TYPES_TOTAL> Item::typenames{};
// Type Methods
Item::Type Item::getType() const { return type; }
Item::Super Item::getSuper() const { return super; }
// Accessor Methods
int Item::getLevel() const { return level; }
const string& Item::getName() const { return name; }
int Item::getWeight() const { return weight; }
Effect Item::getEffect() const { return effect; }
Effect Item::getSpecial() const { return special; }
int Item::getMana() const { return mana; }
int Item::getPrice() const { return price; }
// String Methods
string Item::strEffect() const
{
	return getStr(flag_effects);
}
string Item::strPrice() const
{
	return getStr(flag_price);
}
string Item::strWeight() const
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
// Type Methods cont.
bool Item::isEmpty() const { return type == Empty; }
bool Item::isClothing() const { return super == Super::Clothing; }
bool Item::isAuxiliary() const { return super == Super::Auxiliary; }
bool Item::isWeapon() const { return super == Super::Weapon; }
bool Item::isConsumable() const { return super == Super::Consumable; }
// Mutator Methods
void Item::remove()
{
	*this = Item();
}
void Item::infuse(const Item& item)
{
	if (m_infused)
		throw std::overflow_error{ "Item already infused" };

	name = name + ' ' + item.getName();
	special = item.getSpecial();
	price += item.getPrice();

	m_infused = true;
}
// Operator Methods/Functions
Item& Item::operator=(const Item& other)
{
	type = other.type;
	super = other.super;
	level = other.level;
	weight = other.weight;
	name = other.name;
	effect.data = other.effect.data;
	effect.stacks = other.effect.stacks;
	mana = other.mana;
	price = other.price;
	m_infused = other.m_infused;

	return *this;
}
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
	switch (item.type)
	{
	case Item::Empty:
		item.super = Item::Super::Empty;
		break;
	case Item::Unarmed:
		item.super = Item::Super::Unarmed;
		break;
	case Item::Armor:
	case Item::Cloak:
	case Item::Robe:
		item.super = Item::Super::Clothing;
		break;
	case Item::Shield:
	case Item::Bow:
	case Item::Focus:
		item.super = Item::Super::Auxiliary;
		break;
	case Item::Melee:
	case Item::Quiver:
	case Item::Scroll:
	case Item::Spell:
		item.super = Item::Super::Weapon;
		break;
	case Item::Potion:
	case Item::Arrow:
	case Item::Tool:
	case Item::Crystal:
		item.super = Item::Super::Consumable;
		break;
	}

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


/* * * * * * * *
*	Inventory  *
* * * * * * * */

// Iterator Methods
Inventory::iterator Inventory::slot_begin() { return m_items.begin() + Slot1; }
Inventory::iterator Inventory::slot_end() { return m_items.begin() + Slot4 + 1; }
Inventory::const_iterator Inventory::slot_begin() const { return m_items.begin() + Slot1; }
Inventory::const_iterator Inventory::slot_end() const { return m_items.begin() + Slot4 + 1; }
Inventory::iterator Inventory::findgeneralempty()
{
	return std::find_if(slot_begin(), slot_end(), [](const Item& i) {return i.isEmpty(); });
}

// Check Methods
void Inventory::slotsverify() const
{
	for (int i{}; i < SLOTS_TOTAL; ++i)
	{
		if (m_items[i].isEmpty())
			continue;

		switch (i)
		{
		case SlotClothing:
			if (!m_items[i].isClothing())
				throw std::overflow_error{ "Clothing slot error" };
		case SlotConsumable:
		case SlotConsumableExtra:
			if (!m_items[i].isConsumable())
				throw std::overflow_error{ "Consumable slot error" };
		case Slot1:
		case Slot2:
		case Slot3:
		case Slot4:
			if (m_items[i].isClothing())
				throw std::overflow_error{ "General purpose slot error" };
		}
	}

	if (!m_extraconsumable && !m_items[SlotConsumableExtra].isEmpty())
		throw std::overflow_error{ "Extra consumable slot error" };
}
void Inventory::verify() const
{
	// Individual Slot Check
	slotsverify();

	// Auxiliary Check and Weight Sum
	bool hasaux{};
	int weightsum{};
	for (int i{}; i < SLOTS_TOTAL; ++i)
	{
		if (m_items[i].isAuxiliary())
		{
			if (!hasaux)
				hasaux = true;
			else
				throw std::overflow_error{ "More than one auxiliary equipment in inventory" };
		}

		weightsum += m_items[i].getWeight();
	}

	// Weight Check
	if (weightsum > MAX_WEIGHT)
		throw std::overflow_error{ "Too much being carried" };
}
void Inventory::sort()
{
	// Verify inventory is not broken
	verify();

	int start = Slot1;
	int aux{-1};
	int consumable{-1};

	// Get indices
	for (int i{ Slot1 }; i <= Slot4; ++i)
	{
		if (m_items[i].getType() == Item::Shield)
			aux = i;
		else if (m_items[i].isConsumable())
			consumable = i;
	}

	// Move a consumable to the consumable slot (if there's room)
	if (consumable > 0 && m_items[SlotConsumable].isEmpty())
		std::swap(m_items[SlotConsumable], m_items[consumable]);

	// Move aux to the first slot
	if (aux > Slot1)
	{
		std::swap(m_items[Slot1], m_items[aux]);
		start = Slot2;
	}

	// Sort the remaining items, heaviest first
	std::sort(m_items.begin() + start, m_items.begin() + Slot4 + 1,
		[](const Item& item1, const Item& item2) {
			return item1.getWeight() > item2.getWeight();
		});

	// If there is an available slot, and no unarmed weapon, add one
	for (int i{ start }; i <= Slot4; ++i)
	{
		if (m_items[i].getType() == Item::Unarmed)
			break;

		if (m_items[i].isEmpty())
			m_items[i] = *ItemBaseList.getdatabyname("Unarmed");
	}
}
int Inventory::generalSlotsAvailable() const
{
	int empty{ std::count_if(slot_begin(), slot_end(),
		[](const Item& i)
		{
			return i.isEmpty();
		}) };

	return MULTI_SLOTS - empty;
}
int Inventory::weightAvailable() const
{
	int sum{};
	for (const Item& i : m_items)
		sum += i.getWeight();
	return MAX_WEIGHT - sum;
}
bool Inventory::hasRoomFor(const Item& item) const
{
	bool hasroom{true};

	if (item.getWeight() > weightAvailable())
		hasroom = false;

	if (hasroom)
	{
		switch (item.getSuper())
		{
		case Item::Super::Clothing:
			if (!m_items[SlotClothing].isEmpty())
				hasroom = false;
			break;
		case Item::Super::Auxiliary:
			if (m_items[Slot1].isAuxiliary())
				hasroom = false;
			break;
		case Item::Super::Consumable:
			if (generalSlotsAvailable() == 0)
				hasroom = false;
			if (hasroom)
			{
				if (!m_extraconsumable && !m_items[SlotConsumable].isEmpty())
					hasroom = false;
				else if (m_extraconsumable && !m_items[SlotConsumableExtra].isEmpty())
					hasroom = false;
			}
			break;
		}
	}

	if (hasroom && !item.isClothing() && !item.isAuxiliary())
			hasroom = generalSlotsAvailable() >= item.getWeight();

	return hasroom;
}
bool Inventory::hasAuxiliary() const
{
	return m_items[Slot1].isAuxiliary();
}

// Accessor Methods
const Item& Inventory::getItem(int slot) const
{
	return m_items[slot];
}
const Item& Inventory::getClothing() { return m_items[SlotClothing]; }
const Item& Inventory::getConsumable(bool extra)
{
	if (!extra)
		return m_items[SlotConsumable];
	else
		return m_items[SlotConsumableExtra];
}
int Inventory::getGold() const { return m_gold; }

// Mutator Methods
void Inventory::equipItem(const Item& item)
{
	// Verify that there is room
	if(!hasRoomFor(item))
		throw std::range_error{ "Not enough room for the item, drop something first" };

	// Where is the room
	auto ptr{ m_items.end() };
	switch (item.getSuper())
	{
	case Item::Super::Clothing:
		ptr = m_items.begin() + SlotClothing;
		break;
	case Item::Super::Consumable:
		if (m_items[SlotConsumable].isEmpty())
			ptr = m_items.begin() + SlotConsumable;
		else if (m_extraconsumable && m_items[SlotConsumableExtra].isEmpty())
			ptr = m_items.begin() + SlotConsumableExtra;
		break;
	}

	// Room not found yet
	if (ptr == m_items.end())
		ptr = findgeneralempty();

	// Assign the item and sort the inventory
	*ptr = item;
	if (item.getType() == Item::Cloak)
	{
		m_extraconsumable = true;
		if (!m_tempconsumable.isEmpty())
		{
			equipItem(m_tempconsumable);
			m_tempconsumable = Item();
		}
	}
	sort();
}
void Inventory::useItem(int slot)
{
	if (m_items[slot].getType() == Item::Tool
		|| m_items[slot].getType() == Item::Crystal)
		dropItem(slot);
}
void Inventory::dropItem(int slot)
{
	// If there is an extra consumable that will be lost, keep it until the start of the next turn
	if (m_items[slot].getType() == Item::Cloak && !getConsumable(true).isEmpty())
	{
		m_extraconsumable = false;
		m_tempconsumable = getConsumable(true);
		m_items[SlotConsumableExtra] = Item();
	}
	m_items[slot].remove();
	sort();
}
void Inventory::addGold(int gold)
{
	if (gold < 0)
		throw std::invalid_argument{ "Cannot add negative gold. Use spendGold() instead." };
	m_gold += gold;
}
void Inventory::spendGold(int gold)
{
	if (gold < 0)
		throw std::invalid_argument{ "Cannot spend negatice gold. Use addGold() instead." };
	if (gold > m_gold)
		throw std::underflow_error{ "Not enough gold in inventory. Verify amount before calling spendGold()" };
	m_gold -= gold;
}
void Inventory::buyItem(const Item& item)
{
	spendGold(item.getPrice());
	equipItem(item);
}