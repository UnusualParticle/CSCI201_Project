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
    if (util::verifyNameArray(StatBlock::statnames))
        throw std::invalid_argument{ "Stat names array not full" };

    util::NameMap<size_t> slotnames("Slots");
    slotnames.exportToArray(Inventory::slotnames);
    if (util::verifyNameArray(Inventory::slotnames))
        throw std::invalid_argument{ "Slot names array not full" };

    util::NameMap<Item::Type> itemtypes("Item Types");
    itemtypes.exportToArray(Item::typenames);
    if (util::verifyNameArray(Item::typenames))
        throw std::invalid_argument{ "Item type names array not full" };

    EffectDataList.loadFromFile("effects.txt");

    delete StatBlock::_namemap;
}

int main()
{
    try
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

        for (const auto& n : EffectDataList)
            std::cout << n.name << '\n';
        std::cout << '\n' << std::endl;
    }
    catch (std::invalid_argument& e)
    {
        std::cout << e.what() << "\n\n";
    }

}
