#include "Stats.h"

util::NameMap<StatBlock::Stats>* StatBlock::_namemap{};
util::NameArray<StatBlock::STATS_TOTAL> StatBlock::statnames{};

const string& EffectData::getName() const { return name; }
Effect EffectData::make(int stacks) const
{
	return { this, stacks };
}
std::ifstream& operator>>(std::ifstream& stream, EffectData& data)
{
	// Look for an opening bracket
	stream.ignore(util::STREAMMAX, '[');
	if (!stream.good())
		return stream;

	// Get the data
	string temp;
	util::getline(stream, data.name);
	util::getline(stream, temp);
	data.stat = StatBlock::_namemap->getID(temp);
	stream >> data.power;
	util::getline(stream, temp);
	if (temp == "boon")
		data.boon = true;
	else if (temp == "curse")
		data.boon = false;
	else
		stream.setstate(std::ios_base::failbit);
	data.id = EffectDataList.size();

	// Look for a closing bracket
	stream.ignore(util::STREAMMAX, ']');

	return stream;
}

const std::vector<Effect>& EffectList::getEffects() const { return v; }
int EffectList::getStat(StatBlock::Stats s) const
{
	int value{};
	for (const auto& e : v)
	{
		if (e.data->stat == s)
		{
			if (e.data->power > 0)
				value += e.data->power;
			else
				value += e.stacks;
		}
	}
	return value;
}
void EffectList::addEffect(const Effect& e)
{
	if (e.data->power == -2)
	{
		// Add to duplicate
		auto ptr{ std::find_if(v.begin(), v.end(), [e](const Effect& e2) {return e.data->id == e2.data->id; }) };
		if (ptr == v.end())
			v.push_back(e);
		else
			ptr->stacks += e.stacks;
	}
	else if (e.data->power == -1)
	{
		// Not supposed to happen, should be handled immediately by actor
	}
	else
	{
		// Absolutely allow duplicates. I want the player to feel the power!! (and also the pain!!)
		v.push_back(e);
	}
}
void EffectList::update()
{
	for (auto& e : v)
	{
		if (e.data->power > 0)
			--e.stacks;
	}
	auto discard{ std::remove_if(v.begin(), v.end(), [](const Effect& e) { return e.stacks == 0; }) };
}
void EffectList::clear() { v.clear(); }