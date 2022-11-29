# Halfive Design notes

#### SPDX identifier: COIL-1.0
#### Copyright Nomagno 2022

***
### Missing suitable (re)implementations
The h5rpg/h5rpg.h header, for simulating turn-based combat, is awaiting an implementation. It is meant to be an easter egg (SIMPLE)


The h5t.h and h5net.h headers, to tokenize Halfive Track and Halfive Network .h5doc files, respectively, are awaiting an implementation (TEDIOUS).

### Missing suitable interfaces and implementations

Rendering facilities, physics, collision detection, configuration, UI, etc. (HARD!)

The h5ecs.h header, to create, delete, update, and manage *E*ntities, *C*omponents, and *S*ystems, is awaiting an implementation (TRICKY)

The h5vi.h header needs a big library with auxiliary software rendering functions such as blitting,
bitmap text rendering, arbirary shapes, line rendering, quadratic bezier curve rendering, etcetera:  h5render.h (TEDIOUS)

The Halfive Pixel format needs a library with utilities for converting to/from the BMP image format, at the very least: h5pix.h (MEDIUM DIFFICULTY)


***
### General design notes about the game
#### Halfive is meant to be a racing game engine as well as a game that isn't inherently constrained to any type of gameplay, however not making a flagship physics engine implementation and base game to go with it would be simply disingenous.
#### A few overarching notes about code architecturing, game design, atmosphere and car handling are clear:

- **The game takes place in a modern human society**. It does not, by default, feature death machines, a complete in-universe disregard for safety, or any other messed up stuff like that.
- **The cars are not magical**. Cars are propulsed by two long electromagnets on their bottom, and the handling reflects it. They levitate, run dangerously close to the floor on ocassion, and without electronic assistance handling is extremely unstable at low speeds.
- **Tracks are distinct**. Tracks are made out of superconductors for the highest-tier elite racing series, and simple metals for the 'karting' equivalent everyday ones.
- **Computers are vital to the game**. The entire point of H5VM (Halfive Virtual machine) is to provide a reasonably human-friendly, easily-abstractable but purposefully inefficient microcontroller framework. Car sensors, displays, and everything else is memory mapped. Most of the cars (specially the elite-tier ones) are incredibly inconvenient to handle without a properly configured computer to assist you, to change the handling, to stabilize them, to show information about the rivals etcetera.
- **There is no escaping strategy**. The cars are electrical, have a battery level that needs to
be managed, the magnets wear down due to accidental or purposeful introduction of thermal elements
and dust to the tracks. Just as in real car racing, you might have car types where the time spent in
pits is negligible, and others where the strategic importance of a well-timed undercut is of note.
- **Competition is important**. While some series and rulesets and specific moments in specific
races might have luck elements, the reality is that you mostly will not get automatically boosted to keep up with your rivals
- **This project is useless if it can't be forked**. The entire point of this game is to have an engine where you know where to change things, where it's clear how to do something, no matter how complex. The Halfive project, the Universal Racing Game, has failed if it needs to be wholly redesigned to adapt to a forker's needs.
- **However, reimplementation is vital**. It is humbly acknowledged that this game is not well optimized, that it relies on staying small to run fast, and that its architecturing philosophy is not pragmatic but rather in search of reaching distilled perfection. All symbols, all functions, all features, all interfaces, everything, should be easy to rewrite to run faster, to redesign to suit a specific system, to scale up, down, right, or left.
- **Double buffer things**. While this obviously introduces a tiny bit of overhead and increases the memory footprint, at least adding a compile-time option to make the game state double buffered on systems where multithreading might be supported in the future would be nice.
