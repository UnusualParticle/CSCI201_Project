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

Enemy::Enemy(const string& _name, const StatBlock& _stats, const Inventory& _inventory)
	: Actor(_name, _stats, _inventory)
{}
const Item& Enemy::useItem()
{
	return inventory.getConsumable();
}
Actor ActorData::makeActor()
{
	return { name, stats, inventory };
}
Enemy ActorData::makeEnemy()
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