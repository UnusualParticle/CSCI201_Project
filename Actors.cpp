#include "Actors.h"

Actor::Actor(const string& _name, const StatBlock& _stats, const Inventory& _inventory)
	: name(_name), stats(_stats), inventory(_inventory)
{}
const string& Actor::getName()
{
	return name;
}
// Stat Accessors
int Actor::getArmor()
{
	return stats.armor + inventory.getArmor().getEffect().stacks;
}
int Actor::getAura()
{
	return stats.aura;
}
int Actor::getHealth()
{
	return stats.health;
}
int Actor::getHealthMax()
{
	return stats.healthMax;
}
int Actor::getMana()
{
	return stats.mana;
}
int Actor::getManaMax()
{
	return stats.manaMax;
}
int Actor::getStrength()
{
	return stats.strength;
}
void Actor::useItem(int slot)
{
	stats.mana -= inventory.getItem(slot).getMana();
	inventory.useItem(slot);
}

Enemy::Enemy(const string& _name, const StatBlock& _stats, const Inventory& _inventory)
	: Actor(_name, _stats, _inventory)
{}
const Item& Enemy::taketurn()
{
	return inventory.getItem(Inventory::Slot1);
}
Actor ActorData::makeActor() const
{
	return { name, stats, inventory };
}
Enemy ActorData::makeEnemy() const
{
	return { name, stats, inventory };
}

std::ifstream& operator>>(std::ifstream& stream, ActorData& data)
{
	// Look for an opening bracket
	stream.ignore(util::STREAMMAX, '[');
	if (!stream.good())
		return stream;

	// Get the data
	string temp;
	util::getline(stream, data.name);
	
	// Get StatBlock
	stream >> data.stats.armor
		>> data.stats.aura
		>> data.stats.healthMax
		>> data.stats.manaMax
		>> data.stats.strength;
	data.stats.health = data.stats.healthMax;
	data.stats.mana = data.stats.manaMax;

	// Get Inventory
	util::getline(stream, temp);
	

	// Look for a closing bracket
	stream.ignore(util::STREAMMAX, ']');

	return stream;
}

int Town::counter_armor{};
std::vector<string> Town::firstnames{};
std::vector<string> Town::lastnames{};
std::vector<string> Town::greetings{};
void Town::generatename(string& name)
{
	int index_first{ util::randint(0,firstnames.size() - 1) };
	int index_last{ util::randint(0,firstnames.size() - 1) };
	name = firstnames[index_first] + ' ' + lastnames[index_last];
}
const Item& getitembyname(const string& name)
{
	auto ptr{ std::find_if(ItemBaseList.begin(), ItemBaseList.end(), [name](const Item& data) {return data.getName() == name; })};
	if (ptr == ItemBaseList.end())
		throw std::invalid_argument{ "Name <" + name + "> is not in items" };
	return *ptr;
}
Town::Town()
{
	bool addarmor{ (bool)util::randint(0,1) };
	if (addarmor || counter_armor == target_armor)
	{
		counter_armor = 0;
		armor = getitembyname("Leather Armor");
		generatename(armorer);
	}
	else
		++counter_armor;

	tool1 = getitembyname("Rusty Sword");
	tool2 = getitembyname("Small Buckler");
	generatename(toolmaster);

	spell1 = getitembyname("Flame");
	spell2 = getitembyname("Flame");
	generatename(spellmaster);

	consumable = getitembyname("Scroll of Burning");
	generatename(trader);

	roomprice = util::randint(roomprice_low, roomprice_high);
	generatename(innowner);
}
void Town::load()
{
	std::ifstream file;
	string str;

	file.open("firstnames.txt");
	while (file)
	{
		util::getline(file, str);
		firstnames.push_back(str);
	}
	file.close();

	file.open("lastnames.txt");
	while (file)
	{
		util::getline(file, str);
		lastnames.push_back(str);
	}
	file.close();

	file.open("greetings.txt");
	while (file)
	{
		util::getline(file, str);
		greetings.push_back(str);
	}
	file.close();
}
void Town::reset()
{
	counter_armor = 0;
}