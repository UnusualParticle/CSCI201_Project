#ifndef INVENTORY_H
#define INVENTORY_H

#include "Stats.h"
#include <array>

class Item
{
public:
	enum Type
	{
		Empty,
		Unarmed,

		Armor,
		Robe,
		Cloak,

		Shield,
		Focus,
		Bow,

		Melee,
		Spell,
		Quiver,
		Scroll,

		Tool,
		Crystal,
		Arrow,
		Potion,

		TYPES_TOTAL
	};
	enum class Super
	{
		Empty,
		Unarmed,
		Clothing,
		Auxiliary,
		Weapon,
		Consumable,
		MAJOR_TOTAL
	};
	static util::NameMap<Type>* _typemap;
	static util::NameMap<Type>* _supermap;
	static util::NameArray<TYPES_TOTAL> typenames;
	static util::NameArray<(size_t)Super::MAJOR_TOTAL> supernames;

	// String method flags
	static const int flag_effects{ 1 };
	static const int flag_weight{ 2 };
	static const int flag_price{ 4 };

	// Use type flags
	static const int flag_usable{ 1 };
	static const int flag_physical{ 2 };
	static const int flag_magikal{ 4 };
	static const int flag_ranged{ 8 };
	static const int flag_infuseable{ 16 };
private:
	Type m_type{};
	Super m_super{};
	int m_usetype{};

	string m_name{};
	int m_level{};
	int m_weight{};
	Effect m_effect{};
	Effect m_special{};
	int m_mana{};
	int m_price{};
public:
	// Type Methods
	Type getType() const;
	Super getSuper() const;

	// Accessor Methods
	const string& getName() const;
	int getLevel() const;
	int getWeight() const;
	Effect getEffect() const;
	Effect getSpecial() const;
	int getMana() const;
	int getPrice() const;

	// String Methods
	string strEffect() const;
	string strPrice() const;
	string strWeight() const;
	string getStr(int flags = flag_effects | flag_weight | flag_price) const;

	// Type Methods cont.
	bool isEmpty() const;
	bool isClothing() const;
	bool isAuxiliary() const;
	bool isWeapon() const;
	bool isConsumable() const;

	// Use Check Methods
	bool usable() const;
	bool physical() const;
	bool magikal() const;
	bool ranged() const;
	bool infuseable() const;

	// Mutator Methods
	void remove();
	void infuse(const Item& item);

	// Operator Methods/Functions
	Item& operator=(const Item& other);
	friend std::ifstream& operator>>(std::ifstream& stream, Item& item);
	friend util::CSV& operator>>(util::CSV& csv, Item& item);
};
std::ifstream& operator>>(std::ifstream& stream, Item& item);
util::CSV& operator>>(util::CSV& csv, Item& item);
inline util::DataVector<Item> ItemBaseList{};
inline util::DataVector<Item> ItemModifierList{};

// Generates a random item of the player's level, or a small chance one level above
Item generateItem(int level);
// Generates a random item (of a specific type) of the player's level, or a small chance one level above
Item generateItemByType(int level, Item::Type type);

class InvException : public std::exception
{
public:
	InvException(const char* str);
	InvException(const string& str);
};

struct Inventory
{
public:
	enum Slots
	{
		SlotClothing,
		Slot1,
		Slot2,
		Slot3,
		Slot4,
		SlotConsumable,
		SlotConsumableExtra,
		SLOTS_TOTAL
	};
	static const int MULTI_SLOTS{ 4 };
	static const int MAX_WEIGHT{ 7 };

	bool m_extraconsumable{};
	Item m_tempconsumable{};
private:
	std::array<Item, SLOTS_TOTAL> m_items;
	using iterator = std::array<Item, SLOTS_TOTAL>::iterator;
	using const_iterator = std::array<Item, SLOTS_TOTAL>::const_iterator;

	iterator slot_begin();
	iterator slot_end();
	const_iterator slot_begin() const;
	const_iterator slot_end() const;

	iterator findgeneralempty();
	void slotsverify() const;
	void verify() const;

	int m_gold{};
public:
	// Check Methods
	void sort();
	int generalSlotsAvailable() const;
	int weightAvailable() const;
	bool hasRoomFor(const Item& item) const;
	bool hasAuxiliary() const;

	// Accessor Methods
	const Item& getItem(int slot) const;
	const Item& getClothing() const;
	const Item& getAuxiliary() const;
	const Item& getConsumable(bool extra = false) const;
	int getGold() const;

	// Mutator Methods
	void addItem(const Item& item);
	void useItem(int slot);
	void dropItem(int slot);
	void infuseItem(int slot, const Item& item);
	void addGold(int gold);
	void spendGold(int gold);
};

#endif