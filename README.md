# Short Overview (for AzerothCore-Catalouge)

Name:   GO-Move (with Objscale integration)
What it does: This module allows to add/delete/move/copy/turn/scale GameObjects with an ingame Addon/GUI
Author:   Tralenor
Repository:   https://github.com/Tralenor/mod-gomove
Download:   https://github.com/Tralenor/mod-gomove/archive/refs/heads/main.zip
License:   MIT

# GO Move (with Objscale integration)

## Description

This module allows to use an ingame addon (the addon is provided in the repository) to add/delete/move/copy/turn GameObjects in a easier fashion. (Watch [Rochet2's original video](https://youtu.be/dqbUQrvIlcA?list=PL5CF7437D71BB0795) for an example/more information)
It can also be used to change the scaling/size of GameObjects but depends on the [Objscale Module](https://github.com/Tralenor/mod-objscale) to store these changes persistently. 

This is a port of the TC-Corepatch by [Rochet2](https://github.com/Rochet2) https://github.com/Rochet2/TrinityCore/tree/gomove_3.3.5/src/server/scripts/Custom/GOMove
Note that Rochet2's [GOCommand](https://github.com/Rochet2/GOCommand) is a LUA-Addon which provides essentially the same functionality as the Corepatch mentioned above and this module, but depends on a TrinityCore feature, the addon protocol, which is currently not supported in AzerothCore.
The addon protocol is part of [TrinityCore/TrinityCore#20074](https://github.com/TrinityCore/TrinityCore/pull/20074), if this change is imported to AzerothCore in the future, this module will be obsolete and I highly recommend to switch to the LUA-Addon only implementation.

## How to use ingame

Place the GOMove Addon folder (located in this repository under client-addon) in your client under ./Interface/Addon/
In the Char selection, make sure to activate the addon. 
If the Addon UI doesn't show automatically, you can reset (and thus open) it with: 

```
/gomove reset
```

Walk up to any GameObject - search for them by using the search button. Now all nearby GameObject should appear in your list.
All Objects on your list can now be selected/deselected and moved/turned/scaled/deleted etc. with the addon.
Have Fun while building some Stuff. 

### List of ingame commands

```
/gomove
/gomove help
/gomove invertselection
/gomove reset
```



### UI-Overview

![UI-Overview](/guide.jpg)

### Advanced Usage

Want to place objects with a spell (green targetting circle)?
- execute this to world database `INSERT INTO spell_script_names (spell_id, ScriptName) VALUES (27651, 'GOMove_spell_place');`
- learn spell `27651`
- Now you can put the entry of the object in Send input box and click Send. Then you can spawn the object with the spell. All objects spawned will be saved for the spell spawning as well (spawned from favourites list for example)


## Requirements

GO-Move requires:

- AzerothCore v4.0.0+
- For persistent saving of scaling (size changes) the [Objscale Module](https://github.com/Tralenor/mod-objscale) is needed. Thus its optional, but highly recommended.
  (Objscale dependends on two custom Hooks: OnCreatureSaveToDB and OnGameObjectSaveToDB - Which are provided in a Pullrequest for the main AC repo. Relevant PR: https://github.com/azerothcore/azerothcore-wotlk/pull/11246 (Relevant git branch can be found here: https://github.com/Tralenor/azerothcore-wotlk/tree/On-Creature/GameObject-SaveToDB-Hook ))


## Installation

```
1) Simply `git clone` the module under the `modules` directory of your AzerothCore source or copy paste it manually.
2) Import the SQL to the right Database (auth, world or characters).
3) Re-run cmake and launch a clean build of AzerothCore.
4) "install" LUA-Addon in the Client (see the "How to use ingame" Section for more Information)
```

## Credits

* [Tralenor](https://github.com/Tralenor) (author of the module)
* [Rochet2](https://github.com/Rochet2) (Original Author of GOMove)
* Original GOMove Idea by [Mordred](https://www.youtube.com/channel/UCi8ykEoD_0G2nHGc06ioqzA)
* AzerothCore: [repository](https://github.com/azerothcore) - [website](http://azerothcore.org/) - [discord chat community](https://discord.gg/PaqQRkd)
