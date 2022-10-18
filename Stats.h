#pragma once

#include "Utility.h"

struct StatBlock
{
	enum Stats
	{
		Health,
		Strength,
		Magika,
		Aura
	};
	static util::NameVector names;

	int health;
	int strength;
	int magika;
	int aura;
	int armor;
};

namespace eff
{
	struct EffectData;
	struct Effect
	{
		EffectData* data;
		int stacks;
	};
	struct EffectData
	{
		string name;
		StatBlock::Stats stat;
		int power;
		bool buff;
		size_t id;

		Effect make(int stacks);
	};

	std::vector<EffectData> datalist;

	void loadFromFile(const string& filename);

	EffectData& getdata(int id);
	EffectData& getdata(const string& name);
}