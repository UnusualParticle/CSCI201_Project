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
void Actor::changehealth(int n, bool ignoreArmor)
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

	if (item.physical())
		pair.first.stacks += getStrength();
	if (item.magikal())
		pair.first.stacks += getAura();
	if (item.ranged())
	{
		const auto& aux{ inventory.getAuxiliary() };
		if (aux.getType() == Item::Bow)
			pair.first.stacks += aux.getEffect().stacks;
		else
			throw std::exception{ "Cannot use ranged weapons without a bow" };
	}

	return pair;
}
int Actor::getItemMana(int slot) const
{
	int mana{ inventory.getItem(slot).getMana() - getStatModifier(StatBlock::Focus) };
	return (mana < 0) ? 0 : mana;
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
	const Item& aux{ inventory.getItem(Inventory::Slot1) };
	std::ostringstream str{};

	int stacks{ item.getEffect().stacks };
	int cost{ getItemMana(slot) };

	if (item.physical())
		stacks += stats.strength;
	if (item.magikal())
		stacks += stats.aura;
	if (item.ranged())
	{
		if (aux.getType() != Item::Bow)
			stacks = 0;
		else
			stacks += aux.getEffect().stacks;
	}

	str << item.getName() << ' ' << item.getEffect().data->name << '(' << stacks << ')';
	if (item.getSpecial().stacks > 0)
		str << ' ' << item.getSpecial().data->name << '(' << item.getEffect().stacks << ')';

	if (cost != 0)
		str << " Mp(" << -cost << ')';

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
	
	changemana(getItemMana(slot));
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
		effects.addEffect(armor.getEffect());
		if(armor.getSpecial().stacks > 0)
			effects.addEffect(armor.getSpecial());
	}

	if (inventory.hasAuxiliary())
	{
		const auto& aux{ inventory.getAuxiliary() };
		effects.addEffect(aux.getEffect());
		if (aux.getSpecial().stacks > 0)
			effects.addEffect(aux.getSpecial());
	}

	// If there is an available slot, and no unarmed weapon, add one
	bool hasunarmed{};
	for (int i{ Inventory::Slot1 }; i <= Inventory::Slot4; ++i)
	{
		if (inventory.getItem(i).getType() == Item::Unarmed)
		{
			hasunarmed = true;
			break;
		}
	}
	if (!hasunarmed && inventory.generalSlotsAvailable() > 0)
		inventory.addItem(*ItemBaseList.getdatabyname("Unarmed"));
}
void Actor::endBattle()
{
	// If the player didn't die, make sure they aren't dead
	//	I know this allows for a potential exploit (armor with a max health enchantment can keep giving you more health)
	//	I think the exploit is interesting so I am leaving it
	if (getHealth() > 0)
	{
		effects.clear();
		if (getHealth() < 0)
			stats.health = 1;
	}

	// Remove any unarmed weapons
	for (int i{ Inventory::Slot1 }; i <= Inventory::Slot4; ++i)
	{
		auto& item{ inventory.getItem(i) };
		if (item.getType() == Item::Unarmed)
			inventory.dropItem(i);
	}
}
void Actor::startTurn()
{
	for (const Effect& e : effects.getEffects())
	{
		if (e.data->power < 0)
			continue;

		switch (e.data->stat)
		{
		case StatBlock::Health:
			if (e.data->boon)
				changehealth(e.data->power);
			else
				changehealth(-(e.data->power), e.data->ignore);
			break;
		case StatBlock::Mana:
			if (e.data->boon)
				changemana(e.data->power);
			else
				changemana(-(e.data->power));
			break;
		}
	}
	effects.update();
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
				changehealth(e.stacks);
			else
				changehealth(-(e.stacks), e.data->ignore);
			break;
		case StatBlock::Mana:
			if (e.data->boon)
				changemana(e.stacks);
			else
				changemana(-(e.stacks));
			break;
		}
	}
	else
	{
		effects.addEffect(e);
	}
}
int Actor::getStatModifier(StatBlock::Stats stat) const
{
	return effects.getStat(stat);
}

// Enemy Methods
Enemy::Enemy(const string& _name, const StatBlock& _stats, const Inventory& _inventory, int _level, int _gold)
	: Actor(_name, _stats, _inventory, _level)
{
	inventory.addGold(_gold);
}
void Enemy::getchoices(ChoiceList& choices) const
{
	for (int i{}; i < Inventory::SLOTS_TOTAL; ++i)
	{
		if (inventory.getItem(i).usable() && inventory.getItem(i).getType() != Item::Unarmed)
			choices.push_back((Inventory::Slots)i);
	}
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
		if (getItemMana(choice) <= getMana())
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
	stream >> data.stats.healthMax
		>> data.stats.strength
		>> data.stats.manaMax
		>> data.stats.aura;
	data.stats.health = data.stats.healthMax;
	data.stats.mana = data.stats.manaMax;

	// Get Inventory until the closing bracket is found
	stream >> std::ws;
	while (stream.peek() != ']')
	{
		util::getline(stream, temp);
		data.inventory.addItem(*ItemBaseList.getdatabyname(temp));
		stream >> std::ws;
	}

	return stream;
}
util::CSV& operator>>(util::CSV& csv, ActorData& data)
{
	csv >> data.name >> data.level
		>> data.stats.healthMax
		>> data.stats.strength
		>> data.stats.manaMax
		>> data.stats.aura;
	data.stats.health = data.stats.healthMax;
	data.stats.mana = data.stats.manaMax;

	if (!csv.good())
		throw std::invalid_argument{ "ACTOR: csv not formatted correctly at <" + data.name + '>' };

	for (int i{}; i < Inventory::SLOTS_TOTAL; ++i)
	{
		string temp{};
		csv >> temp;
		if (temp.size())
		{
			try {
				const Item& item{ *ItemBaseList.getdatabyname(temp) };
				data.inventory.addItem(item);
			}
			catch (std::range_error& e)
			{
				throw std::range_error{ "ACTOR: " + (string)e.what() + " at <" + data.name + '>' };
			}
			catch (InvException& e)
			{
				throw std::range_error{ "ACTOR: " + (string)e.what() + " at <" + data.name + '>' };
			}
		}
	}

	csv.endline();

	return csv;
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
void NPC::load(std::vector<string>& errlist)
{
	auto getlist{
		[&](const string& filename, std::vector<string>& list, std::vector<string>& errlist) {
			std::ifstream file{"Data/" + filename};
			string str;
			while (file)
			{
				std::getline(file, str);
				list.push_back(str);
			}
			if (!list.size())
				errlist.push_back("NPC: Data/"+filename+" is empty");
		}
	};

	getlist("npc_firstname.txt", s_firstnames, errlist);
	getlist("npc_lastname.txt", s_lastnames, errlist);
	getlist("npc_greeting.txt", s_greetings, errlist);
	getlist("npc_offer.txt", s_offers, errlist);
	getlist("npc_shop.txt", s_shops, errlist);
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
void Town::load(std::vector<string>& errlist)
{
	std::ifstream file;
	string str;

	file.open("Data/town_suffix.txt");
	while (file)
	{
		std::getline(file, str);
		townsuffix.push_back(str);
	}
	file.close();

	if (!townsuffix.size())
		errlist.push_back("TOWN: suffix list empty");
}

const string& Town::getName() const { return name; }
int Town::getRoomPrice() const { return roomprice; }