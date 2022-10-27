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
#include "Stages.h"
#include <iostream>

void loadGameFiles()
{
    StatBlock::_namemap = new util::NameMap<StatBlock::Stats>("Stats");
    StatBlock::_namemap->exportToArray(StatBlock::statnames);
    if (!util::verifyNameArray(StatBlock::statnames))
        throw std::invalid_argument{ "Stat names array not full" };

    Item::_typemap = new util::NameMap<Item::Type>("Item Types");
    Item::_typemap->exportToArray(Item::typenames);
    if (!util::verifyNameArray(Item::typenames))
        throw std::invalid_argument{ "Item type names array not full" };

    Item::_supermap = new util::NameMap<Item::Type>("Item Super Types");
    Item::_supermap->exportToArray(Item::supernames);
    if (!util::verifyNameArray(Item::supernames))
        throw std::invalid_argument{ "Item super type names array not full" };

    // Load data lists
    EffectDataList.loadFromFile("effects.txt");
    ItemBaseList.loadFromFile("item_default.txt");
    ItemBaseList.loadFromFile("item_clothing.txt");
    ItemBaseList.loadFromFile("item_auxiliary.txt");
    ItemBaseList.loadFromFile("item_weapon.txt");
    ItemBaseList.loadFromFile("item_consumable.txt");
    ItemModifierList.loadFromFile("itemmodifiers.txt");
    PlayerDataList.loadFromFile("playerpresets.txt");
    EnemyDataList.loadFromFile("enemies.txt");

    // Sort Lists
    std::sort(ItemBaseList.begin(), ItemBaseList.end(), [](const Item& first, const Item& last) {return first.getLevel() < last.getLevel(); });
    std::sort(EnemyDataList.begin(), EnemyDataList.end(), [](const ActorData& first, const ActorData& last) {return first.level < last.level; });

    Town::load();

    delete Item::_typemap;
    delete Item::_supermap;
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

void displayHealthChanges(std::pair<int, int> pair, const string& enemyName)
{
    // Player health
    if (pair.first > 0)
        std::cout << "You heal " << pair.first << " points.\n";
    else if (pair.first < 0)
        std::cout << "You take " << -pair.first << " damage.\n";

    // Enemy health
    if (pair.second > 0)
        std::cout << "The " << enemyName << " heals " << pair.second << " points.\n";
    else if (pair.second < 0)
        std::cout << "The " << enemyName << " takes " << -pair.second << " damage.\n";
}
void startBattle(Actor& player)
{
    // Prepare Actors
    Enemy enemy{ generateEnemy(player.getLevel())};
    player.startBattle();
    enemy.startBattle();
    std::cout << "= You encounter a " << enemy.getName() << "!\n";

    // Prepare Manager
    BattleManager manager{player, enemy};
    manager.next();

    // Run Battle
    while (!manager.done())
    {
        // Wait for the manager to be ready
        manager.update();
        if (manager.getState() == BattleManager::Wait)
            continue;

        if (manager.getState() == BattleManager::PlayerTurn)
        {
            std::cout << "\nIt is your turn.\n\n";

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
            Actor::ChoiceList choices;
            player.getChoices(choices);

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
            std::cout << "  You use " << player.inventory.getItem(opt).getName() << ".\n  ";
            player.useItem(opt, enemy);
            displayHealthChanges(manager.healthChanges(), enemy.getName());
        }
        else if (manager.getState() == BattleManager::EnemyTurn)
        {
            std::cout << "\nIt is the " << enemy.getName() << "'s turn.\n";

            int slot{ enemy.taketurn() };

            manager.recordHealth();
            std::cout << "  The " << enemy.getName() << " uses " << enemy.inventory.getItem(slot).getName() << ".\n  ";
            enemy.useItem(slot, player);
            displayHealthChanges(manager.healthChanges(), enemy.getName());
        }

        manager.next();
    }

    if (manager.getState() == BattleManager::Win)
    {
        int gold{ enemy.inventory.getGold() };
        std::cout << "\nYou have defeated the " << enemy.getName() << " and found " << gold << " gold pieces.\n";
        player.inventory.addGold(gold);
    }
    else
        std::cout << "\nYou died to the " << enemy.getName() << '\n';

    player.endBattle();
}

void visitTown(Actor& player)
{
    // Prepare town
    Town town{ player.getLevel() };
    std::cout << "= Visiting " << town.getName() << "\n\n";

    // Prepare Manager
    TownManager manager(player, town);
    util::Timer tmr{};

    // Level Up
    auto phys{ manager.physicallevel() };
    auto magk{ manager.magikallevel() };
    std::cout << "As you enter the town you feel a sense of comfort wash over you.\n"
        << "Which would you like to increase?\n"
        << "\t1. Max Health +" << phys.first << ", Strength +" << phys.second << '\n'
        << "\t2. Max Mana +" << magk.first << ", Aura +" << magk.second << '\n';
    int opt{ util::promptchoice(1, 2) };
    if (opt == 1)
    {
        std::cout << "After resting, you feel stronger.\n\n";
        player.levelPhysical(phys);
    }
    else
    {
        std::cout << "After resting, you feel relaxed.\n\n";
        player.levelPhysical(magk);
    }

    // Display New Stats
    tmr.start(500);
    while (!tmr.isDone());
    std::cout << "Your new stats:"
        << "\n\tHealth: " << player.getHealth() << '/' << player.getHealthMax()
        << "\n\tStrength: " << player.getStrength()
        << "\n\tMana: " << player.getMana() << '/' << player.getManaMax()
        << "\n\tAura: " << player.getAura();
    
    // Explore the town
    const Town::NPC* npc{};
    Town::SaleItems items{};
    bool hasslept{};
    static const string INN_STAY{ " leads you to a spare room and you sleep soundly through the night, and eat a hearty breakfast the next morning.\n You heal " };
    while (manager.getLocation() != TownManager::Exit)
    {
        manager.update();
        if (manager.getLocation() == TownManager::Walk)
            continue;

        switch (manager.getLocation())
        {
        case TownManager::Enter:
            std::cout << "You make your way to the town square to see what shops there are.";
            manager.walkto(TownManager::Square);
            break;
        case TownManager::Square:
            std::cout << "There are a few shops that you could visit. Where would you like to go? [" << player.inventory.getGold() << "gp]";
            for (int i{}; i < Town::SHOPS_TOTAL; ++i)
            {
                std::cout << "\n\t" << (i + 1) << ". " << town.getNPC(i).shopname;
            }
            std::cout << "\n\t" << (Town::SHOPS_TOTAL+1) << ". Leave the City\n";
            opt = util::promptchoice(1, Town::SHOPS_TOTAL+1);
            switch (opt)
            {
            case Town::Blacksmith:
                std::cout << "You make your way to the blacksmith, looking for a weapon or some armor.";
                manager.walkto(TownManager::Blacksmith);
                break;
            case Town::Spellmaster:
                std::cout << "You make your way to the spellmaster, looking for a powerfull spell.";
                manager.walkto(TownManager::Spellmaster);
                break;
            case Town::Trader:
                std::cout << "You make your way to the trader, hoping to find a useful tool.";
                manager.walkto(TownManager::Trader);
                break;
            case Town::Inn:
                std::cout << "You make your way to the inn, looking for a good night's sleep and some breakfast.";
                manager.walkto(TownManager::Inn);
                break;
            case Town::SHOPS_TOTAL:
                std::cout << "You make your way out of the city to continue on your journey.";
                manager.walkto(TownManager::Exit);
                break;
            }
            break;
        case TownManager::Blacksmith:
            manager.visitBlacksmith();
            std::cout << "You exit " << npc->shopname << "\n\n";
            manager.walkto(TownManager::Square);
        }
    }

    // Ask the player if they want to stay at the inn, if they haven't already.
    if (!hasslept)
    {
        const Town::NPC& innkeep{ town.getNPC(Town::Inn) };
        std::cout << "As you leave, " << innkeep.firstname << " asks, "
            << "\"Are you sure you don't want to stay here at my Inn for a night?\""
            << "\n\t1. Stay and rest (" << town.getRoomPrice() << " gp)"
            << "\n\t2. Continue on your journey";
        opt = util::promptchoice(1, 2);

        // If the player has low health and says no, there is a low chance for a cheaper room
        int price{ town.getRoomPrice() };
        if (opt == 2 && (player.getHealth()*10/player.getHealthMax())<5 && util::randint(0, 2) == 0)
        {
            price /= 2;
            std::cout << "The innkeeper insists, saying, \"You don't look very well... I could let you stay for half price, if that changes your mind.\""
                << "\n\t1. Stay and rest (" << price << " gp)"
                << "\n\t2. Continue on your journey";
            opt = util::promptchoice(1, 2);
            if(opt == 1)
                std::cout << "The innkeeper lets out a sigh of relief.\n";
        }
        if (opt == 1)
        {
            // The price will come down a bit if the player cannot afford it
            int pgold = player.inventory.getGold();
            if (pgold < price)
            {
                std::cout << "You rummage through your bags, but you only come up with " << player.inventory.getGold() << ".\n";
                price = price - price / 8;
                if (pgold >= price)
                {
                    std::cout << "The innkeep says \"That'll be fine,\" and accepts " << price << " instead.\n";
                    hasslept = true;
                }
                else
                    std::cout << "The innkeep sighs as you are unable to pay for the room.";
            }
            else
                hasslept = true;

        }
        if (hasslept)
        {
            player.inventory.spendGold(price);
            std::cout << innkeep.firstname << INN_STAY << player.getHealthMax() / 4 << " points.\n";
            player.heal(player.getHealthMax() / 4);
        }
        std::cout << innkeep.firstname << " bids you farewell.";
    }

    // Display New Stats
    tmr.start(500);
    while (!tmr.isDone());
    std::cout << "Your new stats:"
        << "\n\tArmor: " << player.getClothing()
        << "\n\tHealth: " << player.getHealth() << '/' << player.getHealthMax()
        << "\n\tStrength: " << player.getStrength()
        << "\n\tMana: " << player.getMana() << '/' << player.getManaMax()
        << "\n\tAura: " << player.getAura();
}

void printlevel(int level, int stage)
{
    std::cout << "\n== Level " << level << " : " << stage << " ==\n";
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

    if (true)
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
        int stage{};
    } gamestate{};

    Actor player{};
    util::Timer tmr{};
    while (!(gamestate.state == GameState::Dead))
    {
        switch (gamestate.state)
        {
        case GameState::Start:
            printlevel(player.getLevel(), gamestate.stage);
            startGame(player);
            gamestate.untiltown = util::randint(gamestate.until_min, gamestate.until_max);
            gamestate.state = GameState::Battle;
            ++gamestate.stage;
            tmr.start(1000);
            break;
        case GameState::Battle:
            if (tmr.isDone())
            {
                printlevel(player.getLevel(), gamestate.stage);
                startBattle(player);
                if (player.getHealth() > 0)
                {
                    std::cout << "After defeating the enemy, you continue your journey\n== == == == == ==\n\n";
                    if (gamestate.stage == gamestate.untiltown)
                        gamestate.state = GameState::Town;
                }
                else
                    gamestate.state = GameState::Dead;
                ++gamestate.stage;
                tmr.start(1000);
            }
            break;
        case GameState::Town:
            if (tmr.isDone())
            {
                gamestate.stage = 0;
                printlevel(player.getLevel(), gamestate.stage);
                visitTown(player);
                gamestate.untiltown = util::randint(gamestate.until_min, gamestate.until_max);
                gamestate.state = GameState::Battle;
                ++gamestate.stage;
                tmr.start(1000);
            }
            break;
        default:
            break;
        }
    }

    return 0;
}
