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

    delete Item::_typemap;
    delete StatBlock::_namemap;
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

    return 0;
}
