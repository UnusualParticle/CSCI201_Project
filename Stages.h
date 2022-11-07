#pragma once

#include "Utility.h"
#include "Inventory.h"
#include "Actors.h"

class BattleManager
{
public:
    enum States {
        Start,
        Wait,
        PlayerTurn,
        EnemyTurn,
        Win,
        Lose
    };
private:
    BattleManager() = delete;

    States state{};
    bool wait{};
    util::Timer tmr{};

    Actor* player{};
    Enemy* enemy{};

    int playerpre{};
    int enemypre{};

    bool pointersOK() const;
    void startPlayerTurn();
    void startEnemyTurn();
    bool checkDone();
public:
    BattleManager(Actor& _player, Enemy& _enemy);
    // Assign the player and enemy to the manager
    void assign(Actor& _player, Enemy& _enemy);

    // Record the current health for player and the enemy
    void recordHealth();
    // Get the health change for the player and the enemy
    std::pair<int, int> healthChanges() const;
    // Request to begin the next turn
    void next();
    // Check if the next turn can begin
    void update();

    // Get the current state or if it is waiting
    States getState() const;
    // Check if the battle is over
    bool done() const;
};

class TownManager
{
public:
    enum Location {
        Enter,
        Walk,
        Square,
        Blacksmith,
        Spellmaster,
        Trader,
        Inn,
        Exit
    };

private:
    TownManager() = delete;

    Location location{};
    bool walk{};
    util::Timer tmr{};

    Actor* player{};
    Town* town{};

    static const int health_low{ 2 };
    static const int health_mid{ 5 };
    static const int health_high{ 6 };
    static const int str_low{ 1 };
    static const int str_high{ 2 };
    static const int mana_low{ 1 };
    static const int mana_mid{ 3 };
    static const int mana_high{ 3 };
    static const int aur_low{ 0 };
    static const int aur_high{ 1 };
    bool pointersOK() const;
public:
    TownManager(Actor& _player, Town& _town);
    // Assign the player and enemy to the manager
    void assign(Actor& _player, Town& _town);

    std::pair<int, int> physicallevel();
    std::pair<int, int> magikallevel();

    void walkto(Location);
    void update();
    Location getLocation() const;
};