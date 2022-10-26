#pragma once

#include "Stats.h"
#include "Inventory.h"

class Actor
{
protected:
	string name{};
	StatBlock stats{};
	int level{};
	std::vector<Effect> effects{};
	void _addeffect(const Effect& e);
	void _levelup();
public:
	Actor(const string& _name = "", const StatBlock& _stats = {}, const Inventory& _inventory = {}, int level = 1);
	Inventory inventory{};
	using ChoiceList = std::vector<Inventory::Slots>;

	const string& getName() const;
	int getLevel() const;

	// Stat Accessors
	int getClothing() const;
	int getAura() const;
	int getHealth() const;
	int getHealthMax() const;
	int getMana() const;
	int getManaMax() const;
	int getStrength() const;

	// Basic Stat Modification
	void takeDamage(int n);
	void heal(int n);
	void levelPhysical(std::pair<int, int>);
	void levelMagikal(std::pair<int,int>);

	// Effect Methods
	void startBattle();
	void endBattle();
	void startTurn();
	void addEffect(const Effect& effect);
	int getStatModifier(StatBlock::Stats stat) const;

	// Inventory Methods
	std::pair<Effect, Effect> getItemEffects(int slot) const;
	void getChoices(ChoiceList& choices) const;
	string itemStr(int slot) const;
	void useItem(int slot, Actor& target);
};

class Enemy : public Actor
{
public:
	Enemy(const string& _name = "", const StatBlock& _stats = {}, const Inventory& _inventory = {}, int level = 1, int gold = 0);
	int taketurn() const;
};

struct ActorData
{
	string name;
	StatBlock stats;
	Inventory inventory;
	int level;

	static const int _basegold{ 5 };
	static const int _goldperlevel{ 2 };
	const string& getName() const;
	Actor makeActor() const;
	Enemy makeEnemy() const;
};
std::ifstream& operator>>(std::ifstream& stream, ActorData& data);

inline util::DataVector<ActorData> PlayerDataList{};
inline util::DataVector<ActorData> EnemyDataList{};
Enemy generateEnemy(int level);

class NPC
{
public:
	using SaleItems = std::array<Item, 3>;
private:
	string m_firstname;
	string m_name;
	string m_greeting;
	string m_shopname;

	SaleItems m_items;

	using namelist = std::vector<string>;
	static namelist s_firstnames;
	static namelist s_lastnames;
	static namelist s_greetings;
	static namelist s_offers;
public:
	NPC() = default;
	NPC(const string& shop);

	static void load();

	string firstname() const;
	string name() const;
	string greet() const;
	string shopname() const;

	const SaleItems& items() const;
	void removeItem(int slot);

	friend namelist Town::npc_lastnames();
};

class Town
{
public:
	enum Shop
	{
		Blacksmith,
		Spellmaster,
		Trader,
		Inn,
		SHOPS_TOTAL
	};
	enum Items
	{
		Weapon1,
		Weapon2,
		Armor,
		Spell1,
		Spell2,
		Scroll,
		Tool,
		Crystal,
		Extra,
		ITEMS_TOTAL
	};
	std::array<NPC, SHOPS_TOTAL> npcs;
private:
	string name{};
	
	std::array<Item, ITEMS_TOTAL> items;
	int roomprice{};

	static const int roomprice_low{ 5 };
	static const int roomprice_high{ 15 };
	static const int roomprice_level{ 3 };
	static const int target_armor{3};
	static int counter_armor;
	static std::vector<string> townsuffix;
	static std::vector<string> npc_lastnames();
	static void generatetownname(string& name);
public:
	Town(int level);
	~Town() = default;

	static void load();
	void reset();

	const string& getName() const;
	int getRoomPrice() const;
};