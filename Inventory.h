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
		Unarmed,
		Armor,
		Shield,
		Weapon,
		Tool,
		Spell,
		Crystal,
		TYPES_TOTAL
	};
	static util::NameMap<Type>* _typemap;
	static util::NameArray<TYPES_TOTAL> typenames;

	static const int flag_effects{ 1 };
	static const int flag_weight{ 2 };
	static const int flag_price{ 4 };
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
	int getLevel() const;
	int getWeight() const;
	const string& getName() const;
	Effect getEffect() const;
	Effect getSpecial() const;
	int getMana() const;
	int getPrice() const;

	string getEffectStr() const;
	string getPriceStr() const;
	string getWeightStr() const;
	string getStr(int flags = flag_effects | flag_weight | flag_effects) const;

	bool isEmpty() const;
	bool isConsumable() const;
	void remove();

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
	static const int MULTI_SLOTS{ 4 };
	static const int MAX_WEIGHT{ SLOTS_TOTAL };
private:
	bool m_sorted{};
	std::array<Item, SLOTS_TOTAL> items;
	using iterator = std::array<Item, SLOTS_TOTAL>::iterator;
	using const_iterator = std::array<Item, SLOTS_TOTAL>::const_iterator;

	const iterator slot_begin();
	const iterator slot_end();
	iterator findempty();
	void verify() const;

	int gold{};
public:
	void sort();
	int slotsAvailable() const;
	int weightAvailable() const;
	bool hasRoomFor(const Item& item) const;
	bool hasShield() const;

	const Item& getItem(int slot) const;
	const Item& getArmor();
	const Item& getConsumable();

	void equipItem(const Item& item);

	void useItem(int slot);
	void dropItem(int slot);

	int getGold() const;
	void addGold(int _gold);
	void spendGold(int _gold);
	void buyItem(const Item&);
};