/*
* Project Name: Game Title Pending
* Version: C++20
* Author: Donovan Blake
* Purpose: An RPG that runs in the console
*/


#include "Utility.h"
#include "Stats.h"
#include "Inventory.h"
#include "Actors.h"
#include "Stages.h"
#include <iostream>

void loadGameFiles(std::vector<string>& errlist)
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
    EffectDataList.loadFromFile("Data/effect.csv", errlist);
    ItemBaseList.loadFromFile("Data/item_default.csv", errlist);
    ItemBaseList.loadFromFile("Data/item_clothing.csv", errlist);
    ItemBaseList.loadFromFile("Data/item_auxiliary.csv", errlist);
    ItemBaseList.loadFromFile("Data/item_weapon.csv", errlist);
    ItemBaseList.loadFromFile("Data/item_consumable.csv", errlist);
    //ItemModifierList.loadFromFile("Data/item_infuse.csv", errlist);
    PlayerDataList.loadFromFile("Data/actor_player.csv", errlist);
    EnemyDataList.loadFromFile("Data/actor_enemy.csv", errlist);

    // Sort Lists
    std::sort(ItemBaseList.begin(), ItemBaseList.end(), [](const Item& first, const Item& last) {return first.getLevel() < last.getLevel(); });
    std::sort(EnemyDataList.begin(), EnemyDataList.end(), [](const ActorData& first, const ActorData& last) {return first.level < last.level; });

    NPC::load(errlist);
    Town::load(errlist);

    if (errlist.size())
        throw std::exception{ "CSV files bad" };

    delete Item::_typemap;
    delete Item::_supermap;
    delete StatBlock::_namemap;
}
bool debugGameFiles()
{
    std::cout << "What would you like to do?"
        << "\n\t1. Continue"
        << "\n\t2. View Data"
        << "\n\t3. Exit Program\n";
    int opt{ util::promptchoice(1,3)};

    if (opt == 3)
        return false;

    if (opt == 2)
    {
        std::cout << "== Stat Names ==\n";
        for (const auto& n : StatBlock::statnames)
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

    return true;
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

void printPlayerInfo(const Actor& player)
{
    std::cout << "\n== == == == == == == == == == == == == ==\n"
        << "| " << player.getName() << "  |  " << player.getLevel() << "  |  " << player.inventory.getGold() << " gp\n"
        << "|  -- -- -- -- -- -- -- -- -- -- -- -- --\n"
        << "| Health: " << player.getHealth() << '/' << player.getHealthMax() << "  |  Strength: " << player.getStrength() << '\n'
        << "|   Mana: " << player.getMana() << '/' << player.getManaMax() << "  |  Aura: " << player.getAura() << '\n'
        << "|  -- -- -- -- -- -- -- -- -- -- -- -- --\n";

    const Inventory& inv{ player.inventory };
    const int strflag{ Item::flag_effects | Item::flag_weight };

    std::cout << "|      Armor: ";
    if (inv.getClothing().isEmpty())
        std::cout << "None";
    else
        std::cout << inv.getClothing().getStr(strflag);

    bool hasaux{ inv.hasAuxiliary() };
    std::cout << "\n|  Auxiliary: ";
    if (hasaux)
        std::cout << inv.getAuxiliary().getStr(strflag);
    else
        std::cout << "None";

    for (int i{ Inventory::Slot1 + hasaux }; i <= Inventory::Slot4; ++i)
    {
        const Item& item{ inv.getItem(i) };
        std::cout << "\n|     Item "<< (i-Inventory::Slot1+1) << ": " << item.getStr(strflag);
    }

    std::cout << "\n| Consumable: ";
    if (inv.getConsumable().isEmpty())
        std::cout << "None";
    else
        std::cout << inv.getConsumable().getStr(strflag);

    if (inv.m_extraconsumable)
    {
        std::cout << "\n|      Extra: ";
        if (inv.getConsumable(true).isEmpty())
            std::cout << "None";
        else
            std::cout << inv.getConsumable(true).getStr(strflag);
    }

    std::cout << "\n== == == == == == == == == == == == == ==\n\n";
}
bool promptDropItem(Inventory& inv)
{
    int opt{};
    int max{ 0 };

    std::vector<int> choices{};
    for (int i{}; i < Inventory::SLOTS_TOTAL; ++i)
    {
        if (inv.getItem(i).isEmpty())
            continue;

        ++max;
        std::cout << "\n\t" << max << ": ";
        if (i == Inventory::SlotClothing)
            std::cout << "[Armor Slot] ";
        else if (i == Inventory::SlotConsumable || i == Inventory::SlotConsumableExtra)
            std::cout << "[Consumable Slot] ";
        std::cout << inv.getItem(i).getStr(Item::flag_effects & Item::flag_weight);
        choices.push_back(i);
    }
    ++max;
    std::cout << "\n\t" << max << ": Cancel\n";

    opt = util::promptchoice(1, max);

    if (opt < max)
    {
        inv.dropItem(choices[opt - 1]);
        return true;
    }
    else
        return false;
}
bool promptTakeItem(Inventory& inv, const Item& item)
{
    if (inv.hasRoomFor(item))
    {
        inv.addItem(item);
        return true;
    }
    else
    {
        bool exit{};
        while (!exit)
        {
            switch (item.getSuper())
            {
            case Item::Super::Clothing:
                if (!inv.getClothing().isEmpty())
                    std::cout << "You can only have one clothing at a time.\n";
                break;
            case Item::Super::Auxiliary:
                if (inv.hasAuxiliary())
                    std::cout << "You can only have one auxiliary at a time.\n";
                if (inv.generalSlotsAvailable() == 0)
                    std::cout << "You do not have a slot for this item.\n";
                break;
            case Item::Super::Consumable:
                if (inv.generalSlotsAvailable() == 0
                    && !inv.getConsumable().isEmpty())
                {
                    if (!inv.m_extraconsumable)
                        std::cout << "You do not have a slot for this item.\n";
                    else if(inv.m_extraconsumable && !inv.getConsumable(true).isEmpty())
                        std::cout << "You do not have a slot for this item.\n";
                }
                break;
            default:
                if (inv.generalSlotsAvailable() == 0)
                    std::cout << "You do not have a slot for this item.\n";
                break;
            }

            if (inv.weightAvailable() < item.getWeight())
                std::cout << "You do not have enough weight to carry this item. You have " << inv.weightAvailable() << ", you need " << item.getWeight() << '\n';

            std::cout << "Do you want to drop an item to make room?";
            if (promptDropItem(inv))
            {
                if (inv.hasRoomFor(item))
                {
                    inv.addItem(item);
                    return true;
                }
            }
            else
                exit = true;
        }
    }
    return false;
}

void displayBattleStats(const Actor& actor)
{
    std::cout << ' ' << actor.getName()
        << ": Hp(" << actor.getHealth() << '/' << actor.getHealthMax()
        << ") Mp(" << actor.getMana() << '/' << actor.getManaMax()
        << ") ";
    const std::vector<Effect>& effects{ actor.effects.getEffects() };
    if (effects.size())
    {
        for (const auto& e : effects)
            std::cout << e.data->getName() << '(' << e.stacks << ") ";
    }
    std::cout << "\n\n";
}
void displayHealthChanges(std::pair<int, int> pair, const string& enemyName)
{
    // Player health
    if (pair.first > 0)
        std::cout << "\tYou heal " << pair.first << " points.\n";
    else if (pair.first < 0)
        std::cout << "\tYou take " << -pair.first << " damage.\n";

    // Enemy health
    if (pair.second > 0)
        std::cout << "\tThe " << enemyName << " heals " << pair.second << " points.\n";
    else if (pair.second < 0)
        std::cout << "\tThe " << enemyName << " takes " << -pair.second << " damage.\n";
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
            std::cout << "\nIt is your turn.\n";

            displayHealthChanges(manager.healthChanges(), enemy.getName());

            // Display Stats
            displayBattleStats(player);
            displayBattleStats(enemy);

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
                const Item& item{ player.inventory.getItem(opt) };
                if (item.getMana() > player.getMana())
                    std::cout << "Not enough Mana!\n";
                else if (item.ranged() && player.inventory.getAuxiliary().getType() != Item::Bow)
                    std::cout << "You do not have a bow!\n";
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
            displayHealthChanges(manager.healthChanges(), enemy.getName());

            int slot{ enemy.taketurn() };

            manager.recordHealth();
            std::cout << "  The " << enemy.getName() << " uses " << enemy.inventory.getItem(slot).getName() << ".\n  ";
            enemy.useItem(slot, player);
            displayHealthChanges(manager.healthChanges(), enemy.getName());
        }

        manager.next();
    }

    player.endBattle();

    // Looting
    if (manager.getState() == BattleManager::Win)
    {
        int gold{ enemy.inventory.getGold() };
        std::cout << "\nYou have defeated the " << enemy.getName() << " and found " << gold << " gold pieces.\n";
        player.inventory.addGold(gold);

        if (util::randint(0, 1) == 0)
        {
            Item item{ generateItem(player.getLevel()) };
            std::cout << "You found an item: " << item.getStr() << ". Will you take it?";
            bool took{};
            if (util::promptyn())
            {
                if (promptTakeItem(player.inventory, item))
                {
                    std::cout << "You added the " << item.getName() << " to your inventory.\n\n";
                    took = true;
                }
            }
            if (!took)
                std::cout << "You chose not to take the item.\n\n";
        }
    }
    else
        std::cout << "\nYou died to the " << enemy.getName() << '\n';
}

int promptShopVisit(const Town& town)
{
    int opt{};

    for (int i{}; i < NPC::SHOPS_TOTAL; ++i)
    {
        std::cout << "\n\t" << (i + 1) << ". " << town.npcs[i].shopname();
    }
    std::cout << "\n\t" << (NPC::SHOPS_TOTAL + 1) << ". Leave the City\n";
    opt = util::promptchoice(1, NPC::SHOPS_TOTAL + 1);
    opt -= 1;

    switch (opt)
    {
    case NPC::Blacksmith:
        std::cout << "You make your way to the blacksmith, looking for a weapon or some armor.\n";
        break;
    case NPC::Spellmaster:
        std::cout << "You make your way to the spellmaster, looking for a powerfull spell.\n";
        break;
    case NPC::Trader:
        std::cout << "You make your way to the trader, hoping to find a useful tool.\n";
        break;
    case NPC::Inn:
        std::cout << "You make your way to the inn, looking for a good night's sleep and some breakfast.\n";
        break;
    case NPC::SHOPS_TOTAL:
        std::cout << "You make your way out of the town to continue on your journey.\n";
        break;
    }

    return (opt == NPC::SHOPS_TOTAL) ? TownManager::Location::Exit : opt + TownManager::Location::Blacksmith;
}
int promptShopItems(const NPC& npc)
{
    int opt{};
    const auto& items{ npc.items() };
    int max{ (int)items.size()+1 };

    for (int i{}; i < (int)items.size(); ++i)
    {
        std::cout << "\n\t" << i+1 << ": " << items[i].getStr();
    }
    std::cout << "\n\t" << max << ": Exit Shop\n";

    opt = util::promptchoice(1, max);
    return (opt != max) ? opt - 1 : -1;
}
void visitShop(Actor& player, NPC& npc)
{
    player.enterShop();
    std::cout << "\nYou enter " << npc.shopname() << ".\n";
    std::cout << npc.greet();
    
    int opt{promptShopItems(npc)};
    while (opt != -1)
    {
        const Item& item{ npc.items()[opt] };
        if (player.inventory.getGold() < item.getPrice())
            std::cout << "You do not have enough gold to buy this.\n\n";
        else if (promptTakeItem(player.inventory, item))
        {
            std::cout << "You bought the " << item.getName() << " for " << item.getPrice() << " gp. You have " << player.inventory.getGold() << "gp left.\n\n";
            player.inventory.spendGold(item.getPrice());
            npc.removeItem(opt);
        }
        std::cout << npc.name() << ": Is there anything else I can get for you?";
        opt = promptShopItems(npc);
    }

    std::cout << "You leave " << npc.shopname() << ".\n\n";
}
void sleep(Actor& player, const Town& town, int price = 0)
{
    if(price == 0)
        player.inventory.spendGold(town.getRoomPrice());
    else
        player.inventory.spendGold(price);
    std::cout << town.npcs[NPC::Inn].firstname() << Town::INN_STAY << player.getHealthMax() / 4 << " points.";
    player.changehealth(player.getHealthMax() / 4);
}
bool visitInn(Actor& player, const Town& town)
{
    player.enterShop();
    const NPC& npc{ town.npcs[NPC::Inn] };
    std::cout << "\nYou enter " << npc.shopname() << '\n';
    std::cout << npc.greet();

    std::cout << "\nWould like a room? I've got one available for " << town.getRoomPrice() << " gold if you want it.";
    int opt{ util::promptyn() };
    if (opt)
    {
        if (player.inventory.getGold() < town.getRoomPrice())
        {
            std::cout << "I'm afraid you don't have enough money.";
            opt = 0;
        }
        else
            sleep(player, town);
    }
    else
        std::cout << "Alright then. Come back if you change your mind.";

    return opt;
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
        player.levelMagikal(magk);
    }

    // Display New Stats
    tmr.start(500);
    while (!tmr.isDone());
    printPlayerInfo(player);
    
    // Explore the town
    NPC* npc{};
    bool hasslept{};
    while (manager.getLocation() != TownManager::Exit)
    {
        manager.update();
        if (manager.getLocation() == TownManager::Walk)
            continue;

        switch (manager.getLocation())
        {
        case TownManager::Enter:
            std::cout << "You make your way to the town square to see what shops there are.\n";
            manager.walkto(TownManager::Square);
            break;
        case TownManager::Square:
            std::cout << "There are a few shops that you could visit. Where would you like to go?";
            opt = promptShopVisit(town);
            manager.walkto((TownManager::Location)opt);
            break;
        case TownManager::Blacksmith:
            visitShop(player, town.npcs[NPC::Blacksmith]);
            manager.walkto(TownManager::Square);
            break;
        case TownManager::Spellmaster:
            visitShop(player, town.npcs[NPC::Spellmaster]);
            manager.walkto(TownManager::Square);
            break;
        case TownManager::Trader:
            visitShop(player, town.npcs[NPC::Trader]);
            manager.walkto(TownManager::Square);
            break;
        case TownManager::Inn:
            hasslept = visitInn(player, town);
            manager.walkto(TownManager::Square);
            break;
        }
    }

    // Ask the player if they want to stay at the inn, if they haven't already.
    if (!hasslept)
    {
        const NPC& innkeep{ town.npcs[NPC::Inn] };
        std::cout << "As you leave, " << innkeep.firstname() << " asks, "
            << "\"Are you sure you don't want to stay here at my inn for a night?\""
            << "\n\t1. Stay and rest (" << town.getRoomPrice() << " gp)"
            << "\n\t2. Continue on your journey\n";
        opt = util::promptchoice(1, 2);

        // If the player has low health and says no, there is a low chance for a cheaper room
        int price{ town.getRoomPrice() };
        if (opt == 2 && (player.getHealth()*10/player.getHealthMax())<5 && util::randint(0, 2) == 0)
        {
            price /= 2;
            std::cout << "The innkeeper insists, saying, \"You don't look very well... I could let you stay for half price, if that changes your mind.\""
                << "\n\t1. Stay and rest (" << price << " gp)"
                << "\n\t2. Continue on your journey\n";
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
                    std::cout << "The innkeep looks to the next customer as you are unable to pay for the room.";
            }
            else
                hasslept = true;

        }
        if (hasslept)
        {
            sleep(player, town, price);
        }
        std::cout << innkeep.firstname() << " bids you farewell.\n";
    }

    // Display New Stats
    tmr.start(500);
    while (!tmr.isDone());
    printPlayerInfo(player);
}

void printlevel(int level, int stage)
{
    std::cout << "\n== Level " << level << " : " << stage << " ==\n";
}
int main()
{
    std::vector<string> errlist{};
    try
    {
        std::cout << "Loading Data...\n" << std::endl;
        loadGameFiles(errlist);
        std::cout << "No errors occured\n\n";
    }
    catch (std::exception& e)
    {
        std::cout << "== Error List ==\n";
        for (const auto& e : errlist)
        {
            std::cout << e << '\n';
        }
        std::cout << '\n';
    }
    errlist.clear();

    if (!debugGameFiles())
        return 1;

    // Test
    if (false)
    {
        Item item{ generateItemByType(1, Item::Armor) };
        return 0;
    }

    struct GameState
    {
        enum State {
            Start,
            Battle,
            Town,
            Dead
        } state;

        const int until_min{ 4 };
        const int until_max{ 6 };
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
                printlevel(player.getLevel()+1, gamestate.stage);
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
