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

	const string& getName() const;
	int getLevel() const;

	// Stat Accessors
	int getArmor() const;
	int getAura() const;
	int getHealth() const;
	int getHealthMax() const;
	int getMana() const;
	int getManaMax() const;
	int getStrength() const;

	// Basic Stat Modification
	void takeDamage(int n);
	void heal(int n);
	void levelPhysical(int maxhealth, int strength);
	void levelMagikal(int maxmana, int aura);

	// Effect Methods
	void startBattle();
	void endBattle();
	void startTurn();
	void addEffect(const Effect& effect);
	int getStatModifier(StatBlock::Stats stat) const;

	// Inventory Methods
	std::pair<Effect, Effect> getItemEffects(int slot) const;
	string itemStr(int slot) const;
	void useItem(int slot, Actor& target);
};

class Enemy : public Actor
{
public:
	Enemy(const string& _name = "", const StatBlock& _stats = {}, const Inventory& _inventory = {}, int level = 1);
	int taketurn() const;
};

struct ActorData
{
	string name;
	StatBlock stats;
	Inventory inventory;
	int level;

	const string& getName() const;
	Actor makeActor() const;
	Enemy makeEnemy() const;
};
std::ifstream& operator>>(std::ifstream& stream, ActorData& data);

inline util::DataVector<ActorData> PlayerDataList{};
inline util::DataVector<ActorData> EnemyDataList{};
Enemy generateEnemy(int level);

class Town
{
private:
	string armorer{};
	string toolmaster{};
	string spellmaster{};
	string trader{};
	string innowner{};

	Item armor{};
	Item tool1{};
	Item tool2{};
	Item spell1{};
	Item spell2{};
	Item consumable{};
	int roomprice{};

	static const int roomprice_low{ 5 };
	static const int roomprice_high{ 20 };
	static const int target_armor{3};
	static int counter_armor;
	static std::vector<string> firstnames;
	static std::vector<string> lastnames;
	static std::vector<string> greetings;
	static void generatename(string& name);
public:
	Town();
	~Town() = default;

	static void load();

	void reset();
};