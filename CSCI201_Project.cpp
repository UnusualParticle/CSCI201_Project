/*
* Project Name: Game Title Pending
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

    EffectDataList.loadFromFile("effects.txt");
    ItemBaseList.loadFromFile("itembases.txt");
    ItemModifierList.loadFromFile("itemmodifiers.txt");
    PlayerDataList.loadFromFile("playerpresets.txt");
    EnemyDataList.loadFromFile("enemies.txt");
    Town::load();

    delete Item::_typemap;
    delete StatBlock::_namemap;
}
void startGame(Actor& player)
{
    std::cout << "Starting a new game\n\nChoose your class:\n";
    int i{};
    for (auto ptr{ PlayerDataList.begin() }; ptr != PlayerDataList.end(); ++ptr)
        std::cout << ++i << ". " << ptr->name << '\n';
    i = util::promptint(": ");
    auto temp{ PlayerDataList.getdata(i) };
    std::cout << "You chose: " << temp.name << '\n';
    temp.name = util::promptstr("Enter a name for your character: ");
    player = temp.makeActor();
    std::cout << "\nWelcome, " << player.getName() << " you may begin your journey.\n\n";
}
void startBattle(Actor& player, int level)
{
    enum BattleState {
        PlayerTurn,
        EnemyTurn,
        Win,
        Lose
    } battlestate{};

    Enemy enemy{ EnemyDataList.getdatabyname("Imp")->makeEnemy() };
    while (battlestate != Win && battlestate != Lose)
    {
        if (battlestate == PlayerTurn)
        {
            // Display Stats
            std::cout << player.getName()
                << ":: Hp(" << player.getHealth() << '/' << player.getHealthMax()
                << ") Mp(" << player.getMana() << '/' << player.getManaMax()
                << ")\n\n";
            std::cout << enemy.getName()
                << ":: Hp(" << enemy.getHealth() << '/' << enemy.getHealthMax()
                << ") Mp(" << enemy.getMana() << '/' << enemy.getManaMax()
                << ")\n\n";


            const Item* viewing{};

            // Get valid options from inventory
            std::vector<Inventory::Slots> choices;
            for (int i{}; i < Inventory::SLOTS_TOTAL; ++i)
            {
                viewing = &player.inventory.getItem((Inventory::Slots)i);
                switch (viewing->getType())
                {
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
            std::cout << "Choose an action: \n";
            for (int i{}; i < choices.size(); ++i)
            {
                viewing = &player.inventory.getItem((Inventory::Slots)i);
                std::cout << '\t' << (i + 1) << ". " << viewing->getName();
                if (viewing->getType() == Item::Spell || viewing->getType() == Item::Crystal)
                    std::cout << " Mp: " << viewing->getMana() << '\n';
            }

            // Get user choice
            int opt{util::promptint(": ")-1};
            while (opt < 0 || choices.size() <= opt)
            {
                std::cout << "Not a valid option\n";
                int opt{ util::promptint(": ") - 1 };
            }

            // Use Item
            viewing = &player.inventory.getItem((Inventory::Slots)opt);
            Effect eff1{ viewing->getEffect() };
            Effect eff2{ viewing->getSpecial() };
            if (eff1.data->boon)
            {
                player;
                if (eff2.stacks)
                    player;
            }
            else
            {
                enemy;
                if (eff2.stacks)
                    enemy;
            }
            player.useItem(opt);

            if (enemy.getHealth())
                battlestate = EnemyTurn;
            else
                battlestate = Win;
        }
        else if (battlestate == EnemyTurn)
        {

        }
    }
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

    char opt;
    std::cout << "View Data [y,n]? ";
    std::cin >> opt;

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
        int playerlevel;
    } gamestate{};

    Actor player{};
    while (!gamestate.state == GameState::Dead)
    {
        switch (gamestate.state)
        {
        case GameState::Start:
            startGame(player);
            gamestate.state == GameState::Battle;
            break;
        case GameState::Battle:

        default:
            break;
        }
    }

    return 0;
}
