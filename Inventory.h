#pragma once

#include "Stats.h"
#include <array>

class Item
{
public:
	enum Type
	{
		Empty,
		Taken,
		Armor,
		Weapon,
		Shield,
		Consumable
	};
	static util::NameVector typenames;
private:
	Type type;
	int weight;
	string name;
	Effect effect;
	int magika;
	int price;
public:
	Type getType() const;
	int getWeight() const;
	const string& getName() const;
	Effect getEffect() const;
	int getMagika() const;
	int getPrice() const;

	Item& operator=(const Item& other);
};

struct Inventory
{
private:
	std::array<Item, 6> items;

	static util::NameVector slotnames;
public:
	void sort();
	int slotsAvailable();

	void equipArmor(Item armor);
	void equipItem(Item item);
	void equipConsumable(Item consumable);

	void useItem(int slot);
	void dropItem(int slot);
};