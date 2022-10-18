#pragma once

#include "Utility.h"

struct StatBlock
{
	enum Stats
	{
		Armor,
		Aura,
		Health,
		Magika,
		Strength
	};
	static util::NameVector statnames;

	int armor;
	int aura;
	int health;
	int magika;
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

	Effect make(int stacks);
};

using EffectDataList = util::DataVector<EffectData>;