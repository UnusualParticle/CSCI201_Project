/*
* Project Name: Game Title Pending
* Author: Donovan Blake
* Purpose: An RPG that runs in the console
*/

#include "Utility.h"
#include "Stats.h"
#include "Inventory.h"
#include <iostream>

void loadGameFiles()
{
    StatBlock::_namemap = new util::NameMap<StatBlock::Stats>("Stats");
    StatBlock::_namemap->exportToArray(StatBlock::statnames);
    
    util::NameMap<size_t> slotnames("Slots");
    slotnames.exportToArray(Inventory::slotnames);
    
    util::NameMap<Item::Type> itemtypes("Item Types");
    itemtypes.exportToArray(Item::typenames);

    EffectDataList.loadFromFile("effects.txt");

    delete StatBlock::_namemap;
}

int main()
{
    loadGameFiles();

    for (const auto& n : StatBlock::statnames)
        std::cout << n << '\n';
    std::cout << '\n' << std::endl;

    for (const auto& n : Inventory::slotnames)
        std::cout << n << '\n';
    std::cout << '\n' << std::endl;

    for (const auto& n : Item::typenames)
        std::cout << n << '\n';
    std::cout << '\n' << std::endl;
}
