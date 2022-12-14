#ifndef STATS_H
#define STATS_H

#include "Utility.h"

struct StatBlock
{
	enum Stats
	{
		Armor,		// Take Decreased Damage
		Health,
		HealthMax,
		Strength,	// Deal Increased PHysical Damage
		Mana,
		ManaMax,
		Aura,		// Deal Increased Magikal Damage
		Focus,		// Spend Decreased Mana
		STATS_TOTAL
	};
	// namemap is temprorary, for loading purposes ONLY
	static util::NameMap<Stats>* _namemap;
	static util::NameArray<STATS_TOTAL> statnames;

	int armor;
	int health;
	int healthMax;
	int strength;
	int mana;
	int manaMax;
	int aura;
	int focus;
};

struct EffectData;
struct Effect
{
	const EffectData* data;
	int stacks;
};
struct EffectData
{
	string name;
	StatBlock::Stats stat;
	int power;
	bool boon;
	bool ignore;
	size_t id;
	string description{};

	const string& getName() const;
	Effect make(int stacks) const;
};
std::ifstream& operator>>(std::ifstream& stream, EffectData& data);
util::CSV& operator>>(util::CSV& csv, EffectData& data);

class EffectList
{
private:
	std::vector<Effect> v{};
public:
	const std::vector<Effect>& getEffects() const;
	int getStat(StatBlock::Stats) const;
	void addEffect(const Effect&);
	void update();
	void clear();
};

inline util::DataVector<EffectData> EffectDataList;

#endif