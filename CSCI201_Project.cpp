/*
* Project Name: Game Title Pending
* Version: C++17
* Author: Donovan Blake
* Purpose: An RPG that runs in the console
*/

#include "Utility.h"
#include "Stats.h"
#include "Inventory.h"
#include "Actors.h"
#include <iostream>

void loadGameFiles()
{
    StatBlock::_namemap = new util::NameMap<StatBlock::Stats>("Stats");
    StatBlock::_namemap->exportToArray(StatBlock::statnames);
    if (!util::verifyNameArray(StatBlock::statnames))
        throw std::invalid_argument{ "Stat names array not full" };

    util::NameMap<size_t> slotnames("Slots");
    slotnames.exportToArray(Inventory::slotnames);
    if (!util::verifyNameArray(Inventory::slotnames))
        throw std::invalid_argument{ "Slot names array not full" };

    Item::_typemap = new util::NameMap<Item::Type>("Item Types");
    Item::_typemap->exportToArray(Item::typenames);
    if (!util::verifyNameArray(Item::typenames))
        throw std::invalid_argument{ "Item type names array not full" };

    // Load data lists
    EffectDataList.loadFromFile("effects.txt");
    ItemBaseList.loadFromFile("itembases.txt");
    // ItemBaseList.loadFromFile("defaults.txt");
    // ItemBaseList.loadFromFile("weapons.txt");
    // ItemBaseList.loadFromFile("tools.txt");
    // ItemBaseList.loadFromFile("spells.txt");
    // ItemBaseList.loadFromFile("crystals.txt");
    ItemModifierList.loadFromFile("itemmodifiers.txt");
    PlayerDataList.loadFromFile("playerpresets.txt");
    EnemyDataList.loadFromFile("enemies.txt");

    // Sort Lists
    std::sort(ItemBaseList.begin(), ItemBaseList.end(), [](const Item& first, const Item& last) {return first.getLevel() < last.getLevel(); });
    std::sort(EnemyDataList.begin(), EnemyDataList.end(), [](const ActorData& first, const ActorData& last) {return first.level < last.level; });

    Town::load();

    delete Item::_typemap;
    delete StatBlock::_namemap;
}
void debugGameFiles()
{
    char opt{ util::promptchar("View Data [y,n]? ") };

    if (opt == 'y')
    {
        std::cout << "== Stat Names ==\n";
        for (const auto& n : StatBlock::statnames)
            std::cout << n << '\n';
        std::cout << std::endl;

        std::cout << "== Slot Names ==\n";
        for (const auto& n : Inventory::slotnames)
            std::cout << n << '\n';
        std::cout << std::endl;

        std::cout << "== Item Type Names ==\n";
        for (const auto& n : Item::typenames)
            std::cout << n << '\n';
        std::cout << std::endl;

        std::cout << "== Effect Names ==\n";
        for (const auto& n : EffectDataList)
            std::cout << n.name << '\n';
        std::cout << std::endl;

        std::cout << "== Item Base Names ==\n";
        for (const auto& n : ItemBaseList)
            std::cout << n.getName() << '\n';
        std::cout << std::endl;

        std::cout << "== Item Modifier Names ==\n";
        for (const auto& n : ItemModifierList)
            std::cout << n.getName() << '\n';
        std::cout << std::endl;

        std::cout << "== Player Preset Names ==\n";
        for (const auto& n : PlayerDataList)
            std::cout << n.name << '\n';
        std::cout << std::endl;

        std::cout << "== Enemy Names ==\n";
        for (const auto& n : EnemyDataList)
            std::cout << n.name << '\n';
        std::cout << std::endl;
    }
}
void startGame(Actor& player)
{
    std::cout << "Starting a new game\n\nChoose your class:\n";
    int i{};
    for (auto ptr{ PlayerDataList.begin() }; ptr != PlayerDataList.end(); ++ptr)
        std::cout << ++i << ". " << ptr->name << '\n';
    i = util::promptchoice(1,i)-1;
    auto temp{ PlayerDataList.getdata(i) };
    std::cout << "You chose: " << temp.name << '\n';
    temp.name = util::promptstr("Enter a name for your character: ");
    player = temp.makeActor();
    std::cout << "\nWelcome, " << player.getName() << ". You begin your journey to defeat a god.\n\n";
}

struct BattleManager
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
    States state{};
    bool wait{};
    util::Timer tmr{};

    Actor* player{};
    Enemy* enemy{};

    int playerpre{};
    int enemypre{};

    bool pointersOK() const
    {
        if (player && enemy)
            return true;
        else
            throw std::exception{ "Battle Manager not set up" };
    }
    void startPlayerTurn()
    {
        recordHealth();
        player->startTurn();

        if (!checkDone())
        {
            state = PlayerTurn;
        }
    }
    void startEnemyTurn()
    {
        recordHealth();
        enemy->startTurn();

        if (!checkDone())
            state = EnemyTurn;
    }
    bool checkDone()
    {
        if (player->getHealth() < 0)
            state == Lose;
        else if (enemy->getHealth() < 0)
            state == Win;
        else
            return false;
        return true;
    }
public:
    void assign(Actor& _player, Enemy& _enemy)
    {
        player = &_player;
        enemy = &_enemy;
    }
    void recordHealth()
    {
        if (!pointersOK())
            throw std::exception{ "Manager was not set up" };

        playerpre = player->getHealth();
        enemypre = enemy->getHealth();
    }
    std::pair<int, int> healthChanges()
    {
        if (!pointersOK())
            throw std::exception{ "Manager was not set up" };

        std::pair<int, int> pair;
        if (playerpre != player->getHealth())
            pair.first = player->getHealth() - playerpre;
        if (enemypre < enemy->getHealth())
            pair.second = enemy->getHealth() - enemypre;

        return pair;
    }
    void next()
    {
        pointersOK();

        switch (state)
        {
        case Start:
            if (!wait)
            {
                tmr.start(700);
                wait = true;
            }
            else if(tmr.isDone())
            {
                startPlayerTurn();
                wait = false;
            }
            break;
        case PlayerTurn:
            if (!wait)
            {
                tmr.start(700);
                wait = true;
            }
            else if (tmr.isDone())
            {
                startEnemyTurn();
                wait = false;
            }
            break;
        case EnemyTurn:
            if (!wait)
            {
                tmr.start(700);
                wait = true;
            }
            else if (tmr.isDone())
            {
                startPlayerTurn();
                wait = false;
            }
            break;
        }
    }

    States getState() const
    { 
        if (wait)
            return Wait;
        else
            return state;
    }
    bool done() const
    {
        return state == Win || state == Lose;
    }
};
void startBattle(Actor& player)
{
    // Prepare Actors
    Enemy enemy{ generateEnemy(player.getLevel())};
    player.startBattle();
    enemy.startBattle();
    std::cout << "\n= You encounter a " << enemy.getName() << "!\n\n";

    // Prepare Manager
    BattleManager manager{};
    manager.assign(player, enemy);
    manager.next();

    // Run Battle
    while (!manager.done())
    {
        // Wait for the manager to be ready
        if (manager.getState() == BattleManager::Wait)
            continue;

        if (manager.getState() == BattleManager::PlayerTurn)
        {
            std::cout << "It is your turn.\n";

            // Display Stats
            std::cout << player.getName()
                << ": Hp(" << player.getHealth() << '/' << player.getHealthMax()
                << ") Mp(" << player.getMana() << '/' << player.getManaMax()
                << ")\n\n";
            std::cout << enemy.getName()
                << ": Hp(" << enemy.getHealth() << '/' << enemy.getHealthMax()
                << ") Mp(" << enemy.getMana() << '/' << enemy.getManaMax()
                << ")\n\n";

            // Get valid options from inventory
            std::vector<Inventory::Slots> choices;
            for (int i{}; i < Inventory::SLOTS_TOTAL; ++i)
            {
                switch (player.inventory.getItem(i).getType())
                {
                case Item::Unarmed:
                case Item::Weapon:
                case Item::Tool:
                case Item::Spell:
                case Item::Crystal:
                    choices.push_back((Inventory::Slots)i);
                    break;
                default:
                    break;
                }
            }

            // Display options to user
            bool choiceOK{};
            int opt{};
            std::cout << "Choose an action: \n";
            for (size_t i{}; i < choices.size(); ++i)
            {
                std::cout << '\t' << (i + 1) << ". " << player.itemStr(choices[i]) << '\n';
            }
            while (!choiceOK)
            {
                // Get user choice
                opt = choices[util::promptchoice(1, (int)choices.size()) - 1 ];

                // Verify choice
                if (player.inventory.getItem(opt).getMana() > player.getMana())
                    std::cout << "Not enough Mana!\n";
                else
                    choiceOK = true;
            }

            // Use Item
            manager.recordHealth();
            std::cout << "You use " << player.inventory.getItem(opt).getName() << "\n\n";
            player.useItem(opt, enemy);
            manager.displayHealthChanges();
        }
        else if (manager.getState() == BattleManager::EnemyTurn)
        {
            std::cout << "It is the " << enemy->getName() << "'s turn.\n";

            int slot{ enemy.taketurn() };

            manager.recordHealth();
            std::cout << "The " << enemy.getName() << " uses " << enemy.inventory.getItem(slot).getName() << "\n\n";
            enemy.useItem(slot, player);
            manager.displayHealthChanges();
        }

        manager.next();
    }

    if (manager.getState() == BattleManager::Win)
        std::cout << "\nYou have defeated the " << enemy.getName() << '\n';
    else
        std::cout << "\nYou died to the " << enemy.getName() << '\n';

    player.endBattle();
}

void visitTown(Actor& player)
{

}

int main()
{
    string err{};
    try
    {
        std::cout << "Loading Data...\n" << std::endl;
        loadGameFiles();
        std::cout << "No errors occured\n\n";
    }
    catch (std::invalid_argument& e)
    {
        err = e.what();
        std::cout << "== Error ==\n"
            << err << "\n\n";
    }

    if (false)
        debugGameFiles();

    struct GameState
    {
        enum State {
            Start,
            Battle,
            Town,
            Dead
        } state;

        const int until_min{ 3 };
        const int until_max{ 7 };
        int untiltown{};
    } gamestate{};

    Actor player{};
    util::Timer tmr{};
    while (!(gamestate.state == GameState::Dead))
    {
        switch (gamestate.state)
        {
        case GameState::Start:
            startGame(player);
            gamestate.state = GameState::Battle;
            tmr.start(1000);
            break;
        case GameState::Battle:
            if (tmr.isDone())
            {
                startBattle(player);
                if (player.getHealth() > 0)
                {
                    --gamestate.untiltown;
                    std::cout << "After defeating the enemy, you continue your journey\n== == == == == ==\n\n";
                    if (gamestate.untiltown == 0)
                        gamestate.state = GameState::Town;
                }
                else
                    gamestate.state = GameState::Dead;
            }
            break;
        case GameState::Town:
            if (tmr.isDone())
            {
                visitTown(player);
                gamestate.state = GameState::Battle;
            }
            break;
        default:
            break;
        }
    }

    return 0;
}
