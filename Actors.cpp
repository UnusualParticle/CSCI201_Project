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
void Actor::changehealth(int n)
{
	if (n > 0)
	{
		stats.health += n;
		if (stats.health > stats.healthMax)
			stats.health = stats.healthMax;
	}
	else if (n < 0)
	{
		n += getArmor();
		if (n > -1)
			n = -1;
		stats.health += n;
		if (getHealth() < 0)
			stats.health += 0 - getHealth();
	}
}
void Actor::changemana(int n)
{
	if (n > 0)
	{
		stats.mana += n;
		if (stats.mana > stats.manaMax)
			stats.mana = stats.manaMax;
	}
	else if (n < 0)
	{
		stats.mana += n;
		if (getMana() < 0)
			stats.mana += 0 - getMana();
	}
}
void Actor::_levelup()
{
	changehealth(stats.healthMax / 4);
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
// Encounter Methods
void Actor::_startencounter()
{
	inventory.sort();
}
void Actor::startBattle()
{
	_startencounter();

	if (!inventory.getClothing().isEmpty())
	{
		const auto& armor{ inventory.getClothing() };
		m_effects.addEffect(armor.getEffect());
		if(armor.getSpecial().stacks > 0)
			m_effects.addEffect(armor.getSpecial());
	}

	if (inventory.hasAuxiliary())
	{
		const auto& aux{ inventory.getAuxiliary() };
		m_effects.addEffect(aux.getEffect());
		if (aux.getSpecial().stacks > 0)
			m_effects.addEffect(aux.getSpecial());
	}
}
void Actor::endBattle()
{
	if (getHealth() > 0)
	{
		m_effects.clear();
		if (getHealth() < 0)
			stats.health = 1;
	}
}
void Actor::startTurn()
{
	for (const Effect& e : m_effects.getEffects())
	{
		if (e.data->power < 0)
			continue;

		switch (e.data->stat)
		{
		case StatBlock::Health:
			if (e.data->boon)
				changehealth(e.data->power);
			else
				changehealth(-(e.data->power));
			break;
		case StatBlock::Mana:
			if (e.data->boon)
				changemana(e.data->power);
			else
				changemana(-(e.data->power));
			break;
		}
	}
	m_effects.update();
}
void Actor::enterShop()
{
	_startencounter();
}
// Effect Methods
void Actor::addEffect(const Effect& e)
{
	if (e.data->power == -1)
	{
		switch (e.data->stat)
		{
		case StatBlock::Health:
			if (e.data->boon)
				changehealth(e.data->power);
			else
				changehealth(-(e.data->power));
			break;
		case StatBlock::Mana:
			if (e.data->boon)
				changemana(e.data->power);
			else
				changemana(-(e.data->power));
			break;
		}
	}
	else
	{
		m_effects.addEffect(e);
	}
}
int Actor::getStatModifier(StatBlock::Stats stat) const
{
	return m_effects.getStat(stat);
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
	data.inventory.addItem(*ItemBaseList.getdatabyname(temp));
	util::getline(stream, temp);
	data.inventory.addItem(*ItemBaseList.getdatabyname(temp));
	util::getline(stream, temp);
	data.inventory.addItem(*ItemBaseList.getdatabyname(temp));
	util::getline(stream, temp);
	data.inventory.addItem(*ItemBaseList.getdatabyname(temp));
	util::getline(stream, temp);
	data.inventory.addItem(*ItemBaseList.getdatabyname(temp));
	util::getline(stream, temp);
	data.inventory.addItem(*ItemBaseList.getdatabyname(temp));

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
	auto last{ std::find_if(EnemyDataList.begin(), EnemyDataList.end(), [&](const ActorData& data) {return data.level > highlevel; }) };
	// Go to the start of the range of acceptable enemies, or just after
	auto rfirst{ std::find_if(EnemyDataList.rbegin(), EnemyDataList.rend(), [&](const ActorData& data) {return data.level < lowlevel; }) };

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

util::Counter NPC::c_armor{ 3 };
util::Counter NPC::c_scroll{ 2 };
util::Counter NPC::c_robe{ 3 };
util::Counter NPC::c_cloak{ 3 };
std::vector<string> NPC::s_firstnames{};
std::vector<string> NPC::s_lastnames{};
std::vector<string> NPC::s_greetings{};
std::vector<string> NPC::s_offers{};
std::vector<string> NPC::s_shops{};

NPC::NPC(Shop shop, int level)
{
	// Generate Names
	string fname{};
	string lname{};
	string greeting{};

	int index_first{ util::randint(0,s_firstnames.size() - 1) };
	fname = s_firstnames[index_first];
	int index_last{ util::randint(0,s_lastnames.size() - 1) };
	lname = s_lastnames[index_last];

	int index_greet{ util::randint(0,s_greetings.size() - 1) };
	int index_offer{ util::randint(0,s_offers.size() - 1) };
	greeting = s_greetings[index_greet] + ' ' + s_offers[index_offer];

	m_firstname = fname;
	m_name = fname + ' ' + lname;
	m_greeting = m_name + ": " + greeting;
	if (util::randint(0, 2))
		m_shopname = lname + s_shops[shop];
	else
		m_shopname = fname + s_shops[shop];

	// Generate Items
	Item::Type t1{};
	Item::Type t2{};
	Item::Type t3{};
	Item::Type t4{};
	Item::Type t5{};

	switch (shop)
	{
	case Shop::Blacksmith:
		t1 = Item::Melee;
		t2 = Item::Quiver;
		if (util::randint(0, 1))
			t3 = Item::Shield;
		else
			t3 = Item::Bow;
		++c_armor;
		if (c_armor.done() || util::randint(0, 2) == 0)
		{
			t4 = Item::Armor;
			c_armor.reset();
		}
		break;
	case Shop::Spellmaster:
		t1 = Item::Spell;
		t2 = Item::Spell;
		++c_scroll;
		if (c_scroll.done() || util::randint(0, 1))
		{
			t3 = Item::Scroll;
			c_scroll.reset();
		}
		++c_robe;
		if (c_robe.done() || util::randint(0, 2) == 0)
		{
			t4 = Item::Robe;
			c_robe.reset();
		}
		break;
	case Shop::Trader:
		t1 = Item::Potion;
		t2 = Item::Tool;
		t3 = Item::Arrow;
		t4 = Item::Crystal;
		++c_cloak;
		if (c_cloak.done() || util::randint(0, 2) == 0)
		{
			t5 = Item::Cloak;
			c_cloak.reset();
		}
		break;
	}

	if (t1 != Item::Empty)
		m_items.push_back(generateItemByType(level, t1));
	if (t2 != Item::Empty)
		m_items.push_back(generateItemByType(level, t2));
	if (t3 != Item::Empty)
		m_items.push_back(generateItemByType(level, t3));
	if (t4 != Item::Empty)
		m_items.push_back(generateItemByType(level, t4));
	if (t5 != Item::Empty)
		m_items.push_back(generateItemByType(level, t5));
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

	file.open("town_shop.txt");
	while (file)
	{
		std::getline(file, str);
		s_shops.push_back(str);
	}
	file.close();
}
string NPC::firstname() const { return m_firstname; }
string NPC::name() const { return m_name; }
string NPC::greet() const { return m_greeting; }
string NPC::shopname() const { return m_shopname; }
const NPC::SaleItems& NPC::items() const { return m_items; }
void NPC::removeItem(int slot) { m_items.erase(m_items.begin() + slot); }

const string Town::INN_STAY{ " leads you to a spare room and you sleep soundly through the night, and eat a hearty breakfast the next morning.\n You heal " };
std::vector<string> Town::townsuffix{};

void Town::generatetownname(string& name)
{
	int index_name{ util::randint(0,NPC::s_lastnames.size() - 1)};
	int index_suffix{ util::randint(0,townsuffix.size() - 1) };
	name = NPC::s_lastnames[index_name] + townsuffix[index_suffix];
}
Town::Town(int level)
{
	generatetownname(name);

	// Generate NPCs
	npcs[NPC::Blacksmith] = { NPC::Blacksmith, level};
	npcs[NPC::Spellmaster] = { NPC::Spellmaster, level };
	npcs[NPC::Trader] = { NPC::Trader, level };
	npcs[NPC::Inn] = { NPC::Inn, level };

	// Generate a random room cost
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

const string& Town::getName() const { return name; }
int Town::getRoomPrice() const { return roomprice; }