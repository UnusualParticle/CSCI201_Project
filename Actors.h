#pragma once

#include "Stats.h"
#include "Inventory.h"

class Actor
{
protected:
	string name{};
	StatBlock stats{};
	std::vector<Effect> effects{};
	void _addeffect(const Effect& e);
public:
	Actor(const string& _name = "", const StatBlock& _stats = {}, const Inventory& _inventory = {});
	Inventory inventory{};

	const string& getName();

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
	
	// Effect Methods
	void startBattle();
	void endBattle();
	void startTurn();
	void addEffect(const Effect& effect);
	int getStatModifier(StatBlock::Stats stat) const;

	std::pair<Effect, Effect> getItemEffects(int slot) const;
	void useItem(int slot);
};

class Enemy : public Actor
{
public:
	Enemy(const string& _name = "", const StatBlock& _stats = {}, const Inventory& _inventory = {});
	const Item& taketurn();
};

struct ActorData
{
	string name;
	StatBlock stats;
	Inventory inventory;

	const string& getName() const;
	Actor makeActor() const;
	Enemy makeEnemy() const;
};
std::ifstream& operator>>(std::ifstream& stream, ActorData& data);

inline util::DataVector<ActorData> PlayerDataList{};
inline util::DataVector<ActorData> EnemyDataList{};

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