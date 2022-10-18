/*
* Project Name: Game Title Pending
* Author: Donovan Blake
* Purpose: An RPG that runs in the console
*/

#include <iostream>
#include <map>

class NameMap
{
private:

};

struct StatBlock
{
    enum Stats
    {
        Health,
        Strength,
        Magika,
        Aura
    };


    int health;
    int strength;
    int magika;
    int aura;
};

int main()
{
    std::cout << "Hello World!\n";
}
