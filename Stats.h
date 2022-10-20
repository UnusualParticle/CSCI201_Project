#pragma once

#include "Utility.h"

struct StatBlock
{
	enum Stats
	{
		Armor,
		Aura,
		Health,
		HealthMax,
		Mana,
		ManaMax,
		Strength,
		STATS_TOTAL
	};
	// namemap is temprorary, for loading purposes ONLY
	static util::NameMap<Stats>* _namemap;
	static util::NameArray<STATS_TOTAL> statnames;

	int armor;
	int aura;
	int health;
	int healthMax;
	int mana;
	int manaMax;
	int strength;
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
	size_t id;

	const string& getName() const;
	Effect make(int stacks) const;
};
std::ifstream& operator>>(std::ifstream& stream, EffectData& data);

inline util::DataVector<EffectData> EffectDataList;