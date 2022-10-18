#include "Stats.h"

namespace eff
{
	Effect EffectData::make(int stacks)
	{
		return { this, stacks };
	}
	std::ifstream& operator>>(std::ifstream& stream, EffectData& data)
	{
		std::getline(stream >> std::ws, data.name, ';');
		stream >> data.power;

	}

	void loadFromFile(const string& filename)
	{
		datalist.clear();
		std::ifstream file{ filename };

		while (file)
		{
			EffectData data;
			file >> data;
			data.id = datalist.size();
			datalist.push_back(data);
		}
	}

	EffectData& getdata(int id)
	{
		return datalist.at(id);
	}

	EffectData& getdata(const string& name)
	{
		auto ptr{ std::find(datalist.begin(), datalist.end(), [name](const EffectData& d) {return d.name == name; }) };
		if (ptr == datalist.end())
			throw std::invalid_argument("Not a valid effect");
		return *ptr;
	}
}