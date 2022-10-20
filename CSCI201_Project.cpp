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
    i = util::promptchoice(1,i)-1;
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
    player.startBattle();
    enemy.startBattle();
    std::cout << "\n= You encounter a " << enemy.getName() << "!\n\n";
    
    while (battlestate != Win && battlestate != Lose)
    {
        if (battlestate == PlayerTurn)
        {
            player.startTurn();

            // Display Stats
            std::cout << player.getName()
                << ": Hp(" << player.getHealth() << '/' << player.getHealthMax()
                << ") Mp(" << player.getMana() << '/' << player.getManaMax()
                << ")\n\n";
            std::cout << enemy.getName()
                << ": Hp(" << enemy.getHealth() << '/' << enemy.getHealthMax()
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
            std::cout << "Choose an action: \n";
            for (size_t i{}; i < choices.size(); ++i)
            {
                viewing = &player.inventory.getItem(choices[i]);
                std::cout << '\t' << (i + 1) << ". " << viewing->getName();
                if (viewing->getType() == Item::Spell || viewing->getType() == Item::Crystal)
                    std::cout << " Mp: " << viewing->getMana() << '\n';
                else
                    std::cout << '\n';
            }

            // Get user choice
            int opt{ choices[ util::promptchoice(1, (int)choices.size()) - 1] };

            std::cout << "You use " << player.inventory.getItem(opt).getName() << "\n\n";

            // Use Item
            auto pair = player.getItemEffects(opt);
            viewing = &player.inventory.getItem(opt);
            Effect& eff1{ pair.first };
            Effect& eff2{ pair.second };
            if (eff1.data->boon)
            {
                player.addEffect(eff1);
                if (eff2.stacks)
                    player.addEffect(eff2);
            }
            else
            {
                enemy.addEffect(eff1);
                if (eff2.stacks)
                    enemy.addEffect(eff2);
            }
            player.useItem(opt);


            // Update Game State
            if (player.getHealth() < 1)
                battlestate = Lose;
            else if (enemy.getHealth() < 1)
                battlestate = Win;
            else
                battlestate = EnemyTurn;
        }
        else if (battlestate == EnemyTurn)
        {
            battlestate = PlayerTurn;
        }
    }

    if (battlestate == Win)
        std::cout << "\nYou have defeated the " << enemy.getName() << '\n';
    else
        std::cout << "\nYou died to the " << enemy.getName() << '\n';

    player.endBattle();
}
void visitTown(Actor& player, int level)
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
                startBattle(player, gamestate.playerlevel);
                if (player.getHealth() > 0)
                {
                    --gamestate.untiltown;
                    std::cout << "== == == ==\n\nAfter defeating the enemy, you continue your journey\n\n";
                    if (gamestate.untiltown == 0)
                        gamestate.state = GameState::Town;
                }
                else
                    gamestate.state = GameState::Dead;
                tmr.start(1000);
            }
            break;
        case GameState::Town:
            if (tmr.isDone())
            {
                visitTown(player, gamestate.playerlevel);
                gamestate.state = GameState::Battle;
            }
            break;
        default:
            break;
        }
    }

    return 0;
}
