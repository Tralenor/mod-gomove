# GO Move

## Description

This module allows to use an ingame addon (the addon is provided in the repository) to move/copy/turn gameobjects in a simple way. (See GIF for an example)
If used alongside of the [Objscale Module](https://github.com/Tralenor/mod-objscale) it can also be used to change the size of GameObjects.

This is a port of the TC-Corepatch by [Rochet2](https://github.com/Rochet2) https://github.com/Rochet2/TrinityCore/tree/gomove_3.3.5/src/server/scripts/Custom/GOMove
Note that Rochet2's [GOCommand](https://github.com/Rochet2/GOCommand) is a LUA-Addon which provides essentially the same functionallity as the corepatch mentioned above and this module, but is dependend on a Trinitycore feature, the addon protocol, which is currently not supported in AzerothCore.
The addon protocol is part of [TrinityCore/TrinityCore#20074](https://github.com/TrinityCore/TrinityCore/pull/20074), if this change is imported to AzerothCore in the future, this module will be obsolete and I highly recommend to switch to the LUA-Addon only implementation.

## How to use ingame

Place the GOMove Addon folder (located in this repository under client-addon) in your client under ./Interface/Addon/
In the Char selection, make sure to activate the addon. 
If the Addon UI doesn't show automatically, you can open it with: 

```
/gomove
```

Walk up to any GameObject - search for them by using the search button. Now all nearby GameObject should appear in your list.
All Objects on your list can now be selected/deselected and moved/turned/scaled with the addon.
Have Fun while building some Stuff. 




## Requirements

My_new_module requires:

- AzerothCore v4.0.0+
- For persistent saving of scaling (size changes) the Objscale Module [Objscale Module](https://github.com/Tralenor/mod-objscale) is needed. (Objscale dependends on two custom Hooks: OnCreatureSaveToDB and OnGameObjectSaveToDB - Which are provided in a Pullrequest for the main AC repo. Relevant PR: https://github.com/azerothcore/azerothcore-wotlk/pull/11246 (Relevant git branch can be found here: https://github.com/Tralenor/azerothcore-wotlk/tree/On-Creature/GameObject-SaveToDB-Hook ))


## Installation

```
1) Simply `git clone` the module under the `modules` directory of your AzerothCore source or copy paste it manually.
2) Import the SQL to the right Database (auth, world or characters).
3) Re-run cmake and launch a clean build of AzerothCore.
4) "install" LUA-Addon in the Client (see How to use ingame Section for more Information)
```

## Edit the module's configuration (optional)

If you need to change the module configuration, go to your server configuration directory (where your `worldserver` or `worldserver.exe` is), copy `my_module.conf.dist` to `my_module.conf` and edit that new file.


## Credits

* [Tralenor](https://github.com/Tralenor) (author of the module)
* [Rochet2](https://github.com/Rochet2) (Original Author of GOMove)
* Original GOMove Idea by Mordred
* AzerothCore: [repository](https://github.com/azerothcore) - [website](http://azerothcore.org/) - [discord chat community](https://discord.gg/PaqQRkd)
