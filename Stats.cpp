#include "Stats.h"

Effect EffectData::make(int stacks)
{
	return { this, stacks };
}
std::ifstream& operator>>(std::ifstream& stream, EffectData& data)
{
	// Look for an opening bracket
	stream.ignore('[');

	// Get the data
	string temp;
	util::getline(stream, data.name);
	util::getline(stream, temp);
	data.stat = (StatBlock::Stats)StatBlock::statnames.getID(temp);
	stream >> data.power;
	util::getline(stream, temp);
	if (temp == "boon")
		data.boon = true;
	else if (temp == "curse")
		data.boon = false;
	else
		stream.setstate(std::ios_base::failbit);

	stream.ignore(']');
}
