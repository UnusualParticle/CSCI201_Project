#pragma once

#include "Stats.h"
#include "Inventory.h"

class Actor
{
protected:
	string name{};
	StatBlock stats{};
public:
	Actor(const string& _name = "", const StatBlock& _stats = {}, const Inventory& _inventory = {});
	Inventory inventory{};

	const string& getName();

	// Stat Accessors
	int getArmor();
	int getAura();
	int getHealth();
	int getHealthMax();
	int getMana();
	int getManaMax();
	int getStrength();
};

struct EnemyData;
class Enemy : public Actor
{
public:
	Enemy(const string& _name = "", const StatBlock& _stats = {}, const Inventory& _inventory = {});
	const Item& useItem();
};

struct ActorData
{
	string name;
	StatBlock stats;
	Inventory inventory;

	Actor makeActor();
	Enemy makeEnemy();
};
std::ifstream& operator>>(std::ifstream& stream, ActorData& data);

inline util::DataVector<ActorData> PresetDataList{};
inline util::DataVector<ActorData> EnemyDataList{};