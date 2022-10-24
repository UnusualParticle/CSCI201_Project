## Notes on the Scope:

There are many things I opted not to do because of the time it would take to implement. On a project that will be worked on for a long period of time, I would be more certain to write clean, modular code with minimal side effects and plenty of data tracking.

### Features left out include, but are not limited to,

+ Dedicated data compiler
    + [x] Would give detailed information on syntax errors
    + [x] Would allow the game to load data faster (because it would be in binary)
    + [x] Would decrease size of data files
    + [x] Would keep the game from having to do data checks
    + [ ] The size of this game renders this nearly unnecessary
    + [ ] Writing one would take too long
+ More common format for data (e.g. .json or .ini files)
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