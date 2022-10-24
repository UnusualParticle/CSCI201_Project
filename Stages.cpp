#include "Stages.h"


BattleManager::BattleManager(Actor& _player, Enemy& _enemy)
{
    assign(_player, _enemy);
}
void BattleManager::assign(Actor& _player, Enemy& _enemy)
{
    player = &_player;
    enemy = &_enemy;
}
bool BattleManager::pointersOK() const
{
    if (player && enemy)
        return true;
    else
        throw std::exception{ "Battle Manager not set up" };
}
void BattleManager::startPlayerTurn()
{
    recordHealth();
    player->startTurn();

    if (!checkDone())
    {
        state = PlayerTurn;
    }
}
void BattleManager::startEnemyTurn()
{
    recordHealth();
    enemy->startTurn();

    if (!checkDone())
        state = EnemyTurn;
}
bool BattleManager::checkDone()
{
    if (player->getHealth() <= 0)
        state = Lose;
    else if (enemy->getHealth() <= 0)
        state = Win;
    else
        return false;
    return true;
}

void BattleManager::recordHealth()
{
    if (!pointersOK())
        throw std::exception{ "Manager was not set up" };

    playerpre = player->getHealth();
    enemypre = enemy->getHealth();
}
std::pair<int, int> BattleManager::healthChanges() const
{
    if (!pointersOK())
        throw std::exception{ "Manager was not set up" };

    std::pair<int, int> pair;
    pair.first = player->getHealth() - playerpre;
    pair.second = enemy->getHealth() - enemypre;

    return pair;
}
void BattleManager::next()
{
    pointersOK();

    // Check if the battle is over
    if (checkDone())
        return;

    // Only continue if the manager is not waiting
    if (!wait)
    {
        switch (state)
        {
        case Start:
            tmr.start(700);
            wait = true;
            break;
        case PlayerTurn:
            tmr.start(700);
            wait = true;
            break;
        case EnemyTurn:
            tmr.start(700);
            wait = true;
            break;
        }
    }
}
void BattleManager::update()
{
    // Do nothing if the battle is over
    if (checkDone())
        return;

    // Only update when the manager is done waiting
    if (wait && tmr.isDone())
    {
        switch (state)
        {
        case Start:
            startPlayerTurn();
            wait = false;
            break;
        case PlayerTurn:
            startEnemyTurn();
            wait = false;
            break;
        case EnemyTurn:
            startPlayerTurn();
            wait = false;
            break;
        }
    }
}

BattleManager::States BattleManager::getState() const
{
    if (wait)
        return Wait;
    else
        return state;
}
bool BattleManager::done() const
{
    return state == Win || state == Lose;
}

TownManager::TownManager(Actor& _player, Town& _town)
{
    assign(_player, _town);
}
void TownManager::assign(Actor& _player, Town& _town)
{
    player = &_player;
    town = &_town;
}
bool TownManager::pointersOK() const
{
    if (player && town)
        return true;
    else
        throw std::exception{ "Town Manager not set up" };
}

std::pair<int, int> TownManager::physicallevel()
{
    int base{ util::randint(health_low, health_high) };
    int strength{ player->getLevel() };
    if (base <= health_mid)
        strength += 4;
    base += health_level * player->getLevel();
    return { base, strength };
}
std::pair<int, int> TownManager::magikallevel()
{
    int base{ util::randint(mana_low, mana_high) };
    int aura{ player->getLevel() };
    if (base <= mana_mid)
        aura += 2;
    base += mana_level * player->getLevel();
    return { base, aura };
}

void TownManager::walkto(Location l)
{
    if (location == l)
        return;

    tmr.start(2000);
    walk = true;
    location = l;
}
void TownManager::update()
{
    if (walk && tmr.isDone())
    {
        walk = false;
    }
}
TownManager::Location TownManager::getLocation() const
{
    if (walk)
        return Walk;
    else
        return location;
}