#include "Inventory.h"

/* * * * * *
*	Item   *
* * * * * */

// Static Variables
util::NameMap<Item::Type>* Item::_typemap{};
util::NameArray<Item::TYPES_TOTAL> Item::typenames{};
util::NameMap<Item::Type>* Item::_supermap{};
util::NameArray<(size_t)Item::Super::MAJOR_TOTAL> Item::supernames{};
// Type Methods
Item::Type Item::getType() const { return m_type; }
Item::Super Item::getSuper() const { return m_super; }
// Accessor Methods
int Item::getLevel() const { return m_level; }
const string& Item::getName() const { return m_name; }
int Item::getWeight() const { return m_weight; }
Effect Item::getEffect() const { return m_effect; }
Effect Item::getSpecial() const { return m_special; }
int Item::getMana() const { return m_mana; }
int Item::getPrice() const { return m_price; }
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
	if (isEmpty())
		return "None";

	std::ostringstream str{};
	str << '[' << typenames.getName(m_type) << "] " << m_name;
	if (m_mana)
		str << " mp(" << m_mana << ')';
	if (flags & flag_effects)
	{
		str << " [";
		if (m_effect.data)
			str << m_effect.data->name << ": " << m_effect.stacks;
		if (m_special.data)
			str << ", " << m_special.data->name << ": " << m_special.stacks;
		str << ']';
	}
	if(flags & flag_weight)
		str << " [Weight: " << m_weight << ']';
	if(flags & flag_price)
		str << " [" << m_price << " gp]";

	return str.str();
}
// Type Methods cont.
bool Item::isEmpty() const { return m_type == Empty; }
bool Item::isClothing() const { return m_super == Super::Clothing; }
bool Item::isAuxiliary() const { return m_super == Super::Auxiliary; }
bool Item::isWeapon() const { return m_super == Super::Weapon; }
bool Item::isConsumable() const { return m_super == Super::Consumable; }
// Use Check Methods
bool Item::usable() const { return m_usetype & flag_usable; }
bool Item::physical() const { return m_usetype & flag_physical; }
bool Item::magikal() const { return m_usetype & flag_magikal; }
bool Item::ranged() const { return m_usetype & flag_ranged; }
bool Item::infuseable() const { return m_usetype & flag_infuseable; }
// Mutator Methods
void Item::remove()
{
	*this = Item();
}
void Item::infuse(const Item& item)
{
	if (!infuseable())
		throw std::exception{ "Item does not have infuseable flag set" };
	if (item.getType() != Item::Scroll)
		throw std::exception{ "Infusion source must be a scroll" };

	m_name = m_name + ' ' + item.getName();
	m_special = item.getEffect();
	m_special.stacks = 1;
	m_price += item.getPrice();
}
// Operator Methods/Functions
Item& Item::operator=(const Item& other)
{
	m_name = other.m_name;
	m_level = other.m_level;
	m_type = other.m_type;
	m_super = other.m_super;
	m_usetype = other.m_usetype;

	m_weight = other.m_weight;
	m_mana = other.m_mana;
	m_price = other.m_price;

	m_effect = other.m_effect;
	m_special = other.m_special;

	return *this;
}
util::CSV& operator>>(util::CSV& csv, Item& item)
{
	string temp_type;
	string temp_effect;
	string temp_special;
	
	csv >> item.m_name >> item.m_level >> temp_type >> item.m_weight >> item.m_mana >> item.m_price
		>> temp_effect >> item.m_effect.stacks;
	item.m_type = Item::_typemap->getID(temp_type);

	switch (item.m_type)
	{
	case Item::Empty:
		item.m_super = Item::Super::Empty;
		break;
	case Item::Unarmed:
		item.m_usetype |= Item::flag_usable;
		item.m_super = Item::Super::Unarmed;
		break;
	case Item::Armor:
	case Item::Cloak:
	case Item::Robe:
		item.m_super = Item::Super::Clothing;
		break;
	case Item::Shield:
	case Item::Bow:
	case Item::Focus:
		item.m_super = Item::Super::Auxiliary;
		break;
	case Item::Melee:
	case Item::Quiver:
	case Item::Scroll:
		item.m_usetype |= Item::flag_infuseable;
	case Item::Spell:
		item.m_usetype |= Item::flag_usable;
		item.m_super = Item::Super::Weapon;
		break;
	case Item::Potion:
	case Item::Arrow:
	case Item::Tool:
	case Item::Crystal:
		item.m_usetype |= Item::flag_usable;
		item.m_super = Item::Super::Consumable;
		break;
	}
	switch (item.m_type)
	{
	case Item::Melee:
	case Item::Tool:
		item.m_usetype |= Item::flag_physical;
		break;
	case Item::Spell:
	case Item::Crystal:
		item.m_usetype |= Item::flag_magikal;
		break;
	case Item::Quiver:
	case Item::Arrow:
		item.m_usetype |= Item::flag_ranged;
		break;
	}

	try {
		item.m_effect = EffectDataList.getdatabyname(temp_effect)->make(item.m_effect.stacks);
	}
	catch (std::range_error& e)
	{
		throw std::range_error{ "ITEM: " + (string)e.what() + " at <" + item.m_name + '>' };
	}

	if (!csv.good())
		throw std::invalid_argument{"ITEM: csv not formatted correctly at <" + item.m_name + '>'};

	csv >> temp_special >> item.m_special.stacks;
	if (temp_special.size())
	{
		try {
			item.m_special = EffectDataList.getdatabyname(temp_special)->make(item.m_special.stacks);
		}
		catch (std::range_error& e)
		{
			throw std::range_error{ "EFFECT: " + (string)e.what() + " at <" + item.m_name + '>' };
		}
	}
	
	csv.endline();

	return csv;
}

std::pair<util::DataVector<Item>::const_iterator, util::DataVector<Item>::const_iterator> getItemRangeByLevel(int level)
{
	// Go just after the range of acceptable items
	auto last{ --std::find_if(ItemBaseList.begin(), ItemBaseList.end(), [&](const Item& item) {return item.getLevel() > level; }) };
	// Go to the start of the range of acceptable items, or just after
	auto rfirst{ --std::find_if(ItemBaseList.rbegin(), ItemBaseList.rend(), [&](const Item& item) {return item.getLevel() < level; }) };

	// Increase the level range up if there was nothing in range
	if (last == rfirst.base())
	{
		int target{ last->getLevel() };
		auto last{ --std::find_if(ItemBaseList.begin(), ItemBaseList.end(), [&](const Item& item) {return item.getLevel() > target; }) };
	}

	return {rfirst.base(), last};
}
Item generateItem(int level)
{
	if (util::randint(0, 5) == 0)
		level += 1;
	int maxlevel{ ItemBaseList.rbegin()->getLevel()};
	if (level > maxlevel)
		level = maxlevel;
	
	Item item{};
	auto range{ getItemRangeByLevel(level) };
	std::ranges::sample(range.first, range.second, &item, 1, util::randengine);

	return item;
}
Item generateItemByType(int level, Item::Type type)
{
	if (util::randint(0, 5) == 0)
		level += 1;
	int maxlevel{ ItemBaseList.rbegin()->getLevel() };
	if (level > maxlevel)
		level = maxlevel;

	// Get the range of items with the desired level
	auto range{ getItemRangeByLevel(level) };

	// Get the items in that range with the correct type
	using myvec = std::vector<Item*>;
	myvec items{};
	for (auto i{ range.first }; i != range.second; ++i)
	{
		if (i->getType() == type)
			items.push_back(i._Ptr);
	}

	// If that list has a size of 0, look outward for a correct type of item
	// NOTE:
	//       There are return statements in this block
	if (items.size() == 0)
	{
		bool outofoptions{};
		while (!outofoptions)
		{
			if (range.second != ItemBaseList.end())
			{
				if (range.second->getType() == type)
					return { *range.second };
				else
					++range.second;
			}
			if (range.first != ItemBaseList.begin())
			{
				--range.first;
				if (range.first->getType() == type)
					return { *range.first };
			}

			if (range.first == ItemBaseList.begin() && range.second == ItemBaseList.end())
				throw std::range_error{ "Could not find an item of the desired type" };
		}
	}

	// Select a random item from the list and return it
	//   Cannot use nullptr as an argument, needs to already have a value
	//   It is a pointer to a pointer, so it needs to be assigned to a pointer from the heap
	Item** ptr{ new Item* {} };
	std::ranges::sample(items.begin(), items.end(), ptr, 1, util::randengine);
	
	// Return a copy of the item
	Item item{ **ptr };
	delete ptr;				// Don't forget to delete it
	return item;
}

/* * * * * * * *
*	Inventory  *
* * * * * * * */
InvException::InvException(const char* str) : std::exception(str) 
{}
InvException::InvException(const string& str) : std::exception(str.c_str())
{}

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
				throw InvException{ "Clothing slot error" };
			break;
		case SlotConsumable:
		case SlotConsumableExtra:
			if (!m_items[i].isConsumable())
				throw InvException{ "Consumable slot error" };
			break;
		case Slot1:
		case Slot2:
		case Slot3:
		case Slot4:
			if (m_items[i].isClothing())
				throw InvException{ "General purpose slot error" };
			break;
		}
	}

	if (!m_extraconsumable && !m_items[SlotConsumableExtra].isEmpty())
		throw InvException{ "Extra consumable slot error" };
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
				throw InvException{ "More than one auxiliary equipment in inventory" };
		}

		weightsum += m_items[i].getWeight();
	}

	// Weight Check
	if (weightsum > MAX_WEIGHT)
		throw InvException{ "Too much being carried" };
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

	
}
int Inventory::generalSlotsAvailable() const
{
	int empty{ (int)std::count_if(slot_begin(), slot_end(),
		[](const Item& i)
		{
			return i.isEmpty();
		}) };

	return empty;
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
			if (!m_items[SlotConsumable].isEmpty()
				&& !(m_extraconsumable && m_items[SlotConsumableExtra].isEmpty()))
				hasroom = generalSlotsAvailable() > 0;
			break;
		}
	}

	if (hasroom && !item.isClothing() && !item.isConsumable())
			hasroom = generalSlotsAvailable() > 0;

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
const Item& Inventory::getClothing() const { return m_items[SlotClothing]; }
const Item& Inventory::getAuxiliary() const { return m_items[Slot1]; }
const Item& Inventory::getConsumable(bool extra) const
{
	if (!extra)
		return m_items[SlotConsumable];
	else
		return m_items[SlotConsumableExtra];
}
int Inventory::getGold() const { return m_gold; }

// Mutator Methods
void Inventory::addItem(const Item& item)
{
	// Verify that there is room
	if(!hasRoomFor(item))
		throw InvException{ "Not enough room for <"+item.getName()+">, drop something first" };

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

	// Room not found yet, pick a general slot
	if (ptr == m_items.end())
	{
		ptr = findgeneralempty();
		if (ptr == slot_end())
			throw InvException{ "Not a general slot" };
	}

	// Assign the item and sort the inventory
	*ptr = item;
	if (item.getType() == Item::Cloak)
	{
		m_extraconsumable = true;
		if (!m_tempconsumable.isEmpty())
		{
			addItem(m_tempconsumable);
			m_tempconsumable.remove();
		}
	}
	sort();
}
void Inventory::useItem(int slot)
{
	if (m_items[slot].isConsumable())
		dropItem(slot);
}
void Inventory::dropItem(int slot)
{
	// If there is an extra consumable that will be lost, keep it until the start of the next turn
	if (m_items[slot].getType() == Item::Cloak && !getConsumable(true).isEmpty())
	{
		m_extraconsumable = false;
		m_tempconsumable = getConsumable(true);
		m_items[SlotConsumableExtra].remove();
	}

	// Remove the item
	m_items[slot].remove();
	sort();
}
void Inventory::infuseItem(int slot, const Item& item)
{
	m_items[slot].infuse(item);
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