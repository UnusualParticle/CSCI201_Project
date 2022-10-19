#include "Stats.h"

util::NameMap<StatBlock::Stats>* StatBlock::_namemap{};
util::NameArray<StatBlock::STATS_TOTAL> StatBlock::statnames{};

Effect EffectData::make(int stacks)
{
	return { this, stacks };
}
std::ifstream& operator>>(std::ifstream& stream, EffectData& data)
{
	// Look for an opening bracket
	stream.ignore('[');
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

	// Look for a closing bracket
	stream.ignore(']');

	return stream;
}

