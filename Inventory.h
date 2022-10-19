#pragma once

#include "Stats.h"
#include <array>

class Item
{
public:
	enum Type
	{
		Empty,
		Full,
		Armor,
		Tool,
		Shield,
		Consumable,
		TYPES_TOTAL
	};
	static util::NameArray<TYPES_TOTAL> typenames;
private:
	Type type;
	int weight;
	string name;
	Effect effect;
	int mana;
	int price;
public:
	Type getType() const;
	int getWeight() const;
	const string& getName() const;
	Effect getEffect() const;
	int getMana() const;
	int getPrice() const;

	Item& operator=(const Item& other);
};

struct Inventory
{
public:
	enum Slots
	{
		Armor,
		Slot1,
		Slot2,
		Slot3,
		Slot4,
		Consumable,
		SLOTS_TOTAL
	};
	static util::NameArray<SLOTS_TOTAL> slotnames;
private:
	std::array<Item, SLOTS_TOTAL> items;
public:
	void sort();
	int slotsAvailable();

	const Item& getArmor();
	const Item& getConsumable();

	void equipArmor(const Item& armor);
	void equipItem(const Item& item);
	void equipConsumable(const Item& consumable);

	void useItem(int slot);
	void dropItem(int slot);
};