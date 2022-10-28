# Game Title Pending

*Author: Donovan Blake*

This game will be a basic rouge-like, with various [items](#items), [enemies](#enemies), and [NPCs](#towns) for the player to encounter. The player will have minimal [inventory](#inventory) space, encouraging specific planning and build crafting.

---

## Inventory 

The player's inventory is quite small. The player has one [clothing](#clothing) slot, one [consumable](#consumable) slot, and four general slots that may be used for anything other than clothing.

+ The number of general lots an item takes is dependent on its weight
+ Because clothing has its own slot, its weight does not carry into other slots
+ The inventory automatically sorts itself whenever an item is picked up or dropped

The player has four stats: health, strength, magicka and aura. These may be upgraded when the player reaches a [town](#towns).

+ Physical Attributes
    + Health is how much damage the player can take before they die
    + Strength increases melee damage
+ Magikal Attributes
    + Mana is how much spellcasting power the player has left
    + Aura increases most magical damage
        + [x] Spells
        + [x] Crystals
        + [ ] Scrolls
        + [ ] Secondary effects

---

## Items 

Possible Items include, but are not limited to,

+ [Clothing](#clothing)
+ [Auxiliary](#auxiliary)
+ [Weapons](#weapons)
+ [Consumables](#consumables)

### Clothing

Clothing may provide various benefits, and determines the playstyle.

| Type | Description |
| :--- |     :---    |
| Armor | Decreases damage taken (to a minimum of 1). |
| Cloak | Gives the player an additional consumable slot. |
| Robe | Increases mana pool. |

### Auxiliary Equipment

Auxiliary equipment also provides benefits, and enhances or morphs the playstyle.

+ Takes up at least one item slot, but cannot be used.
+ Only one may be equiped at a time.
    + *e.g., cannot have both a Bow and a Focus*

| Type | Description |
| :--- |     :---    |
| Shield | Provides additional armor. May reduce damage to 0. |
| Bow | Allows the use of arrows. |
| Focus | Decreases the cost of spells. |

### Weapons

Weapons are items that are used to attack an opponent. *\*Spells may also be beneficial*

| Type | Description |
| :--- |     :---    |
| Melee | The damage of a melee weapon is determined by its damage value plus the player's strength. |
| Quiver | A quiver may only be used if the player has a bow. The damage of a quiver is determined by its damage plus the bow's damage. |
| Scroll | A spell that costs no mana. The effect of a scroll is determined only by its value. |
| Spell | The effect of a spell is determined by its value plus the player's aura. |

### Consumables

Consumables will always take one slot and have no weight. When a consumable is used, it is removed from the player's inventory and leaves the slot empty.

| Type | Description |
| :--- |     :---    |
| Potion | A potion may have a boon or curse effect. Curses will be applied to the enemy. |
| Arrow | An arrow may only be used if the player has a bow. The bow's damage is added to the arrow. |
| Tool | The effect of a tool is determined by the player's strength. |
| Crystal | A crystal is a stored spell. The player's aura is added to the effect. |

---

## Enemies

Enemies will be expanded upon later in the development process.

---

## Towns

Towns are respites for the player, where they may recover, level up, and spend some gold on upgrades.

+ Recovery
    + 25% of helth is recovered
    + 100% of magicka is recovered
+ Level Up
    + Increase physical prowess
        + Maximum health by a displayed random amount
        + Strength by level or level+2
    OR
    + Increase magical power
        + Maximum magicka by a displayed random amount
        + Aura by level or level+2
+ Spend Gold at various Shops
    + Blacksmith
        + One Weapon
        + One Quiver
        + One Bow or Shield
        + A 1/3 chance for armor 
            + Armor is guaranteed on the 3rd fail
    + Spellmaster
        + Two Spells
        + A 1/2 chance for a Scroll
            + Scroll is guaranteed on the 2nd fail
        + A 1/3 chance for a Robe
            + Robe is guaranteed on the 3rd fail
    + Trader
        + One Potion
        + One Tool
        + One Arrow
        + One Crystal
        + A 1/3 chance for a Cloak
            + Cloak is guaranteed on the 3rd fail
    + Inn
        + Random price dependent on player level
        + Restores an additional 25% of player health
        + If the player cannot afford to stay, the innkeeper may decrease the price
+ Exit
    + If the player does not rest at the inn before they leave, they will be asked if they want to stay

---

## Notes on the Scope:

There are many things I opted not to do because of the time it would take to implement. On a project that will be worked on for a long period of time, I would be more certain to write clean, modular code with minimal side effects and plenty of data tracking.

### Features left out include, but are not limited to,

+ Dedicated data compiler
    + [x] Would give detailed information on syntax errors
    + [x] Would allow the game to load data faster (because it would be in binary)
    + [x] Would decrease size of data files
    + [x] Would keep the game from having to do data checks
    + [x] Would make partial data loading easier, decreasing RAM required
    + [ ] The size of this game renders this nearly unnecessary
    + [ ] Writing one would take too long
+ More common format for data (e.g. .json files)
    + [x] Would allow for reliable syntax highlighting
    + [x] Would allow for use of a library instead of a custom interpreter
    + [x] Would allow for less obtuse data scheme
    + [ ] Requires setting up and using a library
    + [ ] Frankly that's just too much effort if I'm not making a data compiler
+ Game saving/loading
    + [x] Would allow the user to pick up where they left off
    + [x] Would allow for testing up to higher levels more easily
    + [ ] The size of this game renders this nearly unnecessary
+ Event system
    + [x] Would allow for precise and detailed tracking
    + [x] Would allow for better error logs
    + [x] Would allow for a much wider feature set
    + [ ] Item/effect data would become much larger to accomomdate for the complexity
    + [ ] Writing one would take too long
+ GUI
    + [x] Would make the program more user friendly
    + [ ] Requires setting up and using a library
    + [ ] It would take too much time
    + [ ] Duh

> TL;DR
> This is a school project, not a money maker or a hobby.
> No need to spend my hours away on it.