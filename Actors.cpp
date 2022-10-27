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
int Actor::getClothing() const
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
	n -= getClothing();
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
void Actor::levelPhysical(std::pair<int, int> p)
{
	stats.health += p.first;
	stats.healthMax += p.first;
	stats.strength += p.second;
	_levelup();
}
void Actor::levelMagikal(std::pair<int, int> p)
{
	stats.mana += p.first;
	stats.manaMax += p.first;
	stats.aura += p.second;
	_levelup();
}
// Inventory Methods
std::pair<Effect, Effect> Actor::getItemEffects(int slot) const
{
	std::pair<Effect, Effect> pair{};
	const Item& item{ inventory.getItem(slot) };
	pair.first = item.getEffect();
	pair.second = item.getSpecial();
	
	switch (item.getType())
	{
	case Item::Unarmed:
	case Item::Melee:
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
void Actor::getChoices(ChoiceList& choices) const
{
	for (int i{}; i < Inventory::SLOTS_TOTAL; ++i)
	{
		if (inventory.getItem(i).usable())
			choices.push_back((Inventory::Slots)i);
	}
}
string Actor::itemStr(int slot) const
{
	auto& item{ inventory.getItem(slot) };
	std::ostringstream str{};

	int stacks{ item.getEffect().stacks };

	if(item.physical())
		stacks += stats.strength;
	if(item.magikal())
		stacks += stats.aura;

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

	if (inventory.getClothing().getEffect().stacks > 0)
	{
		const auto& armor{ inventory.getClothing() };
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
Enemy::Enemy(const string& _name, const StatBlock& _stats, const Inventory& _inventory, int _level, int _gold)
	: Actor(_name, _stats, _inventory, _level)
{
	inventory.addGold(_gold);
}
int Enemy::taketurn() const
{
	ChoiceList choices;
	getChoices(choices);
	int max = choices.size() - 1;
	bool choiceOK{};
	int choice;
	while (!choiceOK)
	{
		choice = util::randint(0, max);
		if (inventory.getItem(choices[choice]).getMana() <= getMana())
			choiceOK = true;
	}
	return choices[choice];
}

// Actor Data Methods
const string& ActorData::getName() const { return name; }
Actor ActorData::makeActor() const
{
	return { name, stats, inventory, level };
}
Enemy ActorData::makeEnemy() const
{
	int gold{ _basegold + (level * _goldperlevel) };
	return { name, stats, inventory, level, gold};
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
	data.inventory.addItem(*ItemBaseList.getdatabyname(temp));
	util::getline(stream, temp);
	data.inventory.addItem(*ItemBaseList.getdatabyname(temp));
	util::getline(stream, temp);
	data.inventory.addItem(*ItemBaseList.getdatabyname(temp));
	util::getline(stream, temp);
	data.inventory.addItem(*ItemBaseList.getdatabyname(temp));
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

	// Go just after the range of acceptable enemies
	auto last{ std::find_if(EnemyDataList.begin(), EnemyDataList.end(), [&](const ActorData& data) {return data.level > lowlevel; }) };
	// Go to the start of the range of acceptable enemies, or just after
	auto rfirst{ std::find_if(EnemyDataList.rbegin(), EnemyDataList.rend(), [&](const ActorData& data) {return data.level < highlevel; }) };

	// Increase the level range down if there was nothing in range
	if (last == rfirst.base())
	{
		int target{ ++rfirst->level };
		auto rfirst{ std::find_if(EnemyDataList.rbegin(), EnemyDataList.rend(), [&](const ActorData& data) {return data.level < target; }) };
	}

	ActorData* data{new ActorData()};
	std::ranges::sample(rfirst.base(), last, data, 1, util::randengine);
	Enemy enemy{ data->makeEnemy() };
	delete data;
	
	return enemy;
}

std::vector<string> NPC::s_firstnames{};
std::vector<string> NPC::s_lastnames{};
std::vector<string> NPC::s_greetings{};
std::vector<string> NPC::s_offers{};

NPC::NPC(const string& shop)
{
	string fname{};
	string lname{};
	string greeting{};

	int index_first{ util::randint(0,s_firstnames.size() - 1) };
	fname = s_firstnames[index_first];
	int index_last{ util::randint(0,s_lastnames.size() - 1) };
	lname = s_firstnames[index_first];

	int index_greet{ util::randint(0,s_greetings.size() - 1) };
	int index_offer{ util::randint(0,s_offers.size() - 1) };
	greeting = s_greetings[index_greet] + ' ' + s_offers[index_offer];

	m_firstname = fname;
	m_name = fname + ' ' + lname;
	m_greeting = m_name + ": " + greeting;
	if (util::randint(0, 2))
		m_shopname = lname + shop;
	else
		m_shopname = fname + shop;
}
void NPC::load()
{
	std::ifstream file;
	string str;

	file.open("town_firstname.txt");
	while (file)
	{
		std::getline(file, str);
		s_firstnames.push_back(str);
	}
	file.close();

	file.open("town_lastname.txt");
	while (file)
	{
		std::getline(file, str);
		s_lastnames.push_back(str);
	}
	file.close();

	file.open("town_greeting.txt");
	while (file)
	{
		std::getline(file, str);
		s_greetings.push_back(str);
	}
	file.close();

	file.open("town_offer.txt");
	while (file)
	{
		std::getline(file, str);
		s_offers.push_back(str);
	}
	file.close();
}
string NPC::firstname() const { return m_firstname; }
string NPC::name() const { return m_name; }
string NPC::greet() const { return m_greeting; }
string NPC::shopname() const { return m_shopname; }
const NPC::SaleItems& NPC::items() const { return m_items; }
void NPC::removeItem(int slot) { m_items[slot].remove(); }

int Town::counter_armor{};
std::vector<string> Town::townsuffix{};

void Town::generatetownname(string& name)
{
	int index_name{ util::randint(0,npc_lastnames().size() - 1)};
	int index_suffix{ util::randint(0,townsuffix.size() - 1) };
	name = npc_lastnames()[index_name] + townsuffix[index_suffix];
}
const Item& getitembyname(const string& name)
{
	auto ptr{ std::find_if(ItemBaseList.begin(), ItemBaseList.end(), [name](const Item& data) {return data.getName() == name; }) };
	if (ptr == ItemBaseList.end())
		throw std::invalid_argument{ "Name <" + name + "> is not in items" };
	return *ptr;
}
Town::Town(int level)
{
	generatetownname(name);

	// Generate a black smith with a chance for armor
	generatenpc(npcs[Blacksmith]);
	npcs[Blacksmith].shopname += "'s Blacksmith";
	items[Weapon1] = getitembyname("Rusty Sword");
	items[Weapon2] = getitembyname("Small Buckler");
	bool addarmor{ (bool)util::randint(0,1) };
	if (addarmor || counter_armor == target_armor)
	{
		counter_armor = 0;
		items[Armor] = getitembyname("Leather Armor");
	}
	else
		++counter_armor;

	// Generate a spell master with a chance for a scroll
	generatenpc(npcs[Spellmaster]);
	npcs[Spellmaster].shopname += "'s Spells";
	items[Spell1] = getitembyname("Flame");
	items[Spell2] = getitembyname("Flame");
	if (util::randint(0, 1) == 1)
		items[Scroll] = getitembyname("Scroll of Burning");

	// Generate a trader
	generatenpc(npcs[Trader]);
	npcs[Trader].shopname += " Trader";
	items[Tool] = getitembyname("Small Healing Potion");
	items[Crystal] = getitembyname("Crystal of Burning");
	if (util::randint(0, 1) == 1)
		items[Extra] = getitembyname("Small Mana Potion");

	// Generate an inn owner with a random room cost
	generatenpc(npcs[Inn]);
	npcs[Inn].shopname += " Inn";
	roomprice = util::randint(roomprice_low + (level * roomprice_level), roomprice_high + (level * roomprice_level));
}
void Town::load()
{
	std::ifstream file;
	string str;

	file.open("town_suffix.txt");
	while (file)
	{
		std::getline(file, str);
		townsuffix.push_back(str);
	}
	file.close();
}
void Town::reset()
{
	counter_armor = 0;
}

const string& Town::getName() const { return name; }
int Town::getRoomPrice() const { return roomprice; }