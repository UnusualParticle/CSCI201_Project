#include "Actors.h"

Actor::Actor(const string& _name, const StatBlock& _stats, const Inventory& _inventory, int _level)
	: name(_name), stats(_stats), inventory(_inventory), level(_level)
{}
const string& Actor::getName() const
{
	return name;
}
int Actor::getLevel() const { return level; }
// Stat Accessors
int Actor::getArmor() const
{
	return stats.armor + getStatModifier(StatBlock::Armor);
}
int Actor::getAura() const
{
	return stats.aura + getStatModifier(StatBlock::Aura);
}
int Actor::getHealth() const
{
	return stats.health + getStatModifier(StatBlock::HealthMax);
}
int Actor::getHealthMax() const
{
	return stats.healthMax + getStatModifier(StatBlock::HealthMax);
}
int Actor::getMana() const
{
	return stats.mana + getStatModifier(StatBlock::ManaMax);
}
int Actor::getManaMax() const
{
	return stats.manaMax + getStatModifier(StatBlock::ManaMax);
}
int Actor::getStrength() const
{
	return stats.strength + getStatModifier(StatBlock::Strength);
}
// Basic Stat Modification
void Actor::takeDamage(int n)
{
	n -= getArmor();
	if (n < 1)
		n = 1;
	stats.health -= n;
}
void Actor::heal(int n)
{
	stats.health += n;
	if (stats.health > stats.healthMax)
		stats.health = stats.healthMax;
}
void Actor::_levelup()
{
	heal(stats.healthMax / 4);
	stats.mana = stats.manaMax;
	++level;
}
void Actor::levelPhysical(int maxhealth, int strength)
{
	stats.health += maxhealth;
	stats.healthMax += maxhealth;
	stats.strength += strength;
	_levelup();
}
void Actor::levelMagikal(int maxmana, int aura)
{
	stats.mana += maxmana;
	stats.manaMax += maxmana;
	stats.aura += aura;
	_levelup();
}
// Item Methods
std::pair<Effect, Effect> Actor::getItemEffects(int slot) const
{
	std::pair<Effect, Effect> pair{};
	const Item& item{ inventory.getItem(slot) };
	pair.first = item.getEffect();
	pair.second = item.getSpecial();
	
	switch (item.getType())
	{
	case Item::Unarmed:
	case Item::Weapon:
	case Item::Tool:
		pair.first.stacks += getStrength();
		break;
	case Item::Spell:
	case Item::Crystal:
		pair.first.stacks += getAura();
		break;
	}

	return pair;
}
string Actor::itemStr(int slot) const
{
	auto& item{ inventory.getItem(slot) };
	std::ostringstream str{};

	int stacks{ item.getEffect().stacks };
	switch (item.getType())
	{
	case Item::Unarmed:
	case Item::Weapon:
	case Item::Tool:
		stacks += stats.strength;
		break;
	case Item::Spell:
	case Item::Crystal:
		stacks += stats.aura;
		break;
	}

	str << item.getName() << ' ' << item.getEffect().data->name << '(' << stacks << ')';
	if (item.getSpecial().stacks > 0)
		str << ' ' << item.getSpecial().data->name << '(' << item.getEffect().stacks << ')';

	if (item.getMana())
		str << " Mp(" << item.getMana() << ')';

	return str.str();
}
void Actor::useItem(int slot, Actor& enemy)
{
	const auto pair{ getItemEffects(slot) };

	if (pair.first.data->boon)
	{
		addEffect(pair.first);
		if (pair.second.stacks)
			addEffect(pair.second);
	}
	else
	{
		enemy.addEffect(pair.first);
		if (pair.second.stacks)
			enemy.addEffect(pair.second);
	}
	
	stats.mana -= inventory.getItem(slot).getMana();
	inventory.useItem(slot);
}
// Effect Methods
void Actor::startBattle()
{
	inventory.sort();

	if (inventory.getArmor().getEffect().stacks > 0)
	{
		const auto& armor{ inventory.getArmor() };
		_addeffect(armor.getEffect());
		if(armor.getSpecial().stacks > 0)
			_addeffect(armor.getSpecial());
	}

	auto shieldpos{ inventory.hasShield() };
	if (shieldpos != Inventory::SLOTS_TOTAL)
	{
		const auto& shield{ inventory.getItem(shieldpos) };
		_addeffect(shield.getEffect());
		if (shield.getSpecial().stacks > 0)
			_addeffect(shield.getSpecial());
	}
}
void Actor::endBattle()
{
	if (getHealth() > 0)
	{
		effects.clear();
		if (getHealth() < 0)
			stats.health = 1;
	}
}
void Actor::startTurn()
{
	for (Effect& e : effects)
	{
		if (e.data->stat == StatBlock::Health)
			takeDamage(e.data->power);
		else if (e.data->stat == StatBlock::Mana)
			stats.aura -= e.data->power;

		if (e.stacks > 0)
		{
			--e.stacks;
		}
	}
	auto discard{ std::remove_if(effects.begin(), effects.end(), [](const Effect& e) { return e.stacks == 0; }) };
}
void Actor::_addeffect(const Effect& effect)
{
	auto ptr{ std::find_if(effects.begin(), effects.end(), [effect](const Effect& e) {return e.data->id == effect.data->id; }) };
	if (ptr == effects.end())
		effects.push_back(effect);
	else if (ptr->stacks < effect.stacks)
		ptr->stacks = effect.stacks;
}
void Actor::addEffect(const Effect& effect)
{
	if (effect.data->power == -1)
	{
		if (effect.data->stat == StatBlock::Health)
			takeDamage(effect.stacks);
		else if (effect.data->stat == StatBlock::Mana)
			stats.aura -= effect.stacks;
		else
			_addeffect(effect);
	}
	else
	{
		_addeffect(effect);
	}
}
int Actor::getStatModifier(StatBlock::Stats stat) const
{
	int value{};
	for (const auto& e : effects)
	{
		if (e.data->stat == stat)
		{
			if (e.data->power == -1)
				value += e.stacks;
			else
				value += e.data->power;
		}
	}
	return value;
}

// Enemy Methods
Enemy::Enemy(const string& _name, const StatBlock& _stats, const Inventory& _inventory, int _level)
	: Actor(_name, _stats, _inventory, _level)
{}
int Enemy::taketurn() const
{
	return Inventory::Slot1;
}

// Actor Data Methods
const string& ActorData::getName() const { return name; }
Actor ActorData::makeActor() const
{
	return { name, stats, inventory, level };
}
Enemy ActorData::makeEnemy() const
{
	return { name, stats, inventory, level };
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
	stream >> data.level;

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
	data.inventory.equipArmor(*ItemBaseList.getdatabyname(temp));
	util::getline(stream, temp);
	data.inventory.equipItem(*ItemBaseList.getdatabyname(temp));
	util::getline(stream, temp);
	data.inventory.equipItem(*ItemBaseList.getdatabyname(temp));
	util::getline(stream, temp);
	data.inventory.equipItem(*ItemBaseList.getdatabyname(temp));
	util::getline(stream, temp);
	data.inventory.equipItem(*ItemBaseList.getdatabyname(temp));
	util::getline(stream, temp);
	data.inventory.equipConsumable(*ItemBaseList.getdatabyname(temp));

	// Look for a closing bracket
	stream.ignore(util::STREAMMAX, ']');

	return stream;
}

// Generates a random enemy from two levels below up to one level above
Enemy generateEnemy(int level)
{
	int maxlevel{ EnemyDataList.rbegin()->level };

	int lowlevel{ (level > 2) ? level - 2 : 1 };
	int highlevel{ (level > maxlevel - 1) ? maxlevel : level + 1 };
	auto first{ std::find_if(EnemyDataList.begin(), EnemyDataList.end(), [&](const ActorData& data) {return data.level >= lowlevel; }) };
	auto rlast{ std::find_if(EnemyDataList.rbegin(), EnemyDataList.rend(), [&](const ActorData& data) {return data.level <= highlevel; }) };
	
	ActorData* data{new ActorData()};
	std::ranges::sample(first, rlast.base(), data, 1, util::randengine);
	Enemy enemy{ data->makeEnemy() };
	delete data;
	
	return enemy;
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