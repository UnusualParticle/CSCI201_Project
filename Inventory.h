#pragma once

#include "Stats.h"
#include <array>

class Item
{
public:
	enum Type
	{
		Empty,
		Modifier,
		Armor,
		Shield,
		Unarmed,
		Weapon,
		Tool,
		Spell,
		Crystal,
		TYPES_TOTAL
	};
	static util::NameMap<Type>* _typemap;
	static util::NameArray<TYPES_TOTAL> typenames;
private:
	string name{};
	int level{};
	Type type{};
	int weight{};
	Effect effect{};
	Effect special{};
	int mana{};
	int price{};
public:
	Type getType() const;
	int getWeight() const;
	const string& getName() const;
	Effect getEffect() const;
	Effect getSpecial() const;
	int getMana() const;
	int getPrice() const;

	Item& operator=(const Item& other);
	friend std::ifstream& operator>>(std::ifstream& stream, Item& item);
};
std::ifstream& operator>>(std::ifstream& stream, Item& item);
inline util::DataVector<Item> ItemBaseList{};
inline util::DataVector<Item> ItemModifierList{};

struct Inventory
{
public:
	enum Slots
	{
		SlotArmor,
		Slot1,
		Slot2,
		Slot3,
		Slot4,
		SlotConsumable,
		SLOTS_TOTAL
	};
	static util::NameArray<SLOTS_TOTAL> slotnames;
private:
	std::array<Item, SLOTS_TOTAL> items;

	std::array<Item, SLOTS_TOTAL>::iterator findtype(Item::Type type);
	using const_iterator = std::array<Item, SLOTS_TOTAL>::const_iterator;
public:
	void sort();
	int slotsAvailable();

	const Item& getItem(int slot) const;
	const Item& getArmor();
	Slots hasShield() const;
	const Item& getConsumable();

	void equipArmor(const Item& armor);
	void equipItem(const Item& item);
	void equipConsumable(const Item& consumable);

	void useItem(int slot);
	void dropItem(int slot);
};