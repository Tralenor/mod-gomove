/*
GOMove By Rochet2
http://rochet2.github.io/
Port as AC-Module by Tralenor
Original idea by Mordred
Objscale Integration partly from Magnus

This File provides the commands used by the LUA-Addon
*/

#include "GOMove.h"
#include <math.h>
#include <set>
#include <sstream>
#include <string>
#include "Chat.h"
#include "GameObject.h"
#include "Language.h"
#include "Map.h"
#include "Object.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Player.h"
#include "Config.h"


class GOMove_commandscript : public CommandScript
{
public:
    GOMove_commandscript() : CommandScript("GOMove_commandscript")
    {
    }

    enum commandIDs
    {
        TEST,
        SELECTNEAR,
        DELET,
        X,
        Y,
        Z,
        O,
        GROUND,
        FLOOR,
        RESPAWN,
        GOTO,
        FACE,

        SPAWN,
        NORTH,
        EAST,
        SOUTH,
        WEST,
        NORTHEAST,
        NORTHWEST,
        SOUTHEAST,
        SOUTHWEST,
        UP,
        DOWN,
        LEFT,
        RIGHT,
        SIZEMINUS, //needs Objscale
        SIZEPLUS, //needs Objscale
        PHASE,
        SELECTALLNEAR,
        SPAWNSPELL,
    };

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> GOMoveCommandTable =
        {
            {"gomove", SEC_ADMINISTRATOR,false, &GOMove_Command, ""}
        };
        return GOMoveCommandTable;
    }

    static bool GOMove_Command(ChatHandler* handler, char const* args)
    {
        if (!args)
            return false;

        char* ID_t = strtok((char*) args, " ");
        if (!ID_t)
            return false;
        uint32 ID = static_cast<uint32>(atol(ID_t));

        char*  cLowguid = strtok(nullptr, " ");
        uint32 lowguid  = 0;
        if (cLowguid)
            lowguid = static_cast<uint32>(atol(cLowguid));

        char*  ARG_t = strtok(nullptr, " ");
        uint32 ARG   = 0;
        if (ARG_t)
            ARG = static_cast<uint32>(atol(ARG_t));

        WorldSession* session = handler->GetSession();
        if (!session)
            return false;
        Player* player = session->GetPlayer();


        if (ID < SPAWN) // no args
        {
            if (ID >= DELET && ID <= GOTO) // has target (needs retrieve gameobject)
            {
                GameObject* target = GOMove::GetGameObject(player, lowguid);
                if (!target)
                    ChatHandler(player->GetSession()).PSendSysMessage("Object GUID: %u not found.", lowguid);
                else
                {
                    float x, y, z, o;
                    target->GetPosition(x, y, z, o);
                    uint32 p = target->GetPhaseMask();
                    switch (ID)
                    {
                    case DELET:
                    {
                        GOMove::DeleteGameObject(player, target, lowguid);
                        GOMove::SendRemove(player, lowguid);
                        break; 
                    }
                    case X: GOMove::MoveGameObject(player, player->GetPositionX(), y, z, o, p, lowguid);       break;
                    case Y: GOMove::MoveGameObject(player, x, player->GetPositionY(), z, o, p, lowguid);       break;
                    case Z: GOMove::MoveGameObject(player, x, y, player->GetPositionZ(), o, p, lowguid);       break;
                    case O: GOMove::MoveGameObject(player, x, y, z, player->GetOrientation(), p, lowguid);     break;
                    case RESPAWN: GOMove::SpawnGameObject(player, x, y, z, o, p, target->GetEntry());          break;
                    case GOTO:
                    {
                        // skip if is in flight
                        if (player->IsInFlight())
                        {
                            ChatHandler(player->GetSession()).PSendSysMessage("Try Again after you finished your flight.");
                            break;
                        }
                        else
                        {
                            player->SaveRecallPosition(); 
                            player->TeleportTo(target->GetMapId(), x, y, z, o);
                        }
                    } break;
                    case GROUND:
                    {
                        float ground = target->GetMap()->GetHeight(target->GetPhaseMask(), x, y, MAX_HEIGHT);
                        if (ground != INVALID_HEIGHT)
                            GOMove::MoveGameObject(player, x, y, ground, o, p, lowguid);
                    } break;
                    case FLOOR:
                    {
                        float floor = target->GetMap()->GetHeight(target->GetPhaseMask(), x, y, z);
                        if (floor != INVALID_HEIGHT)
                            GOMove::MoveGameObject(player, x, y, floor, o, p, lowguid);
                    } break;
                    }
                }
            }
            else
            {
                switch (ID)
                {
                case TEST:
                {
                    session->SendAreaTriggerMessage("%s", player->GetName().c_str());
                    break;
                }
                case FACE:
                {
                    float const piper2 = float(M_PI) / 2.0f;
                    float const multi = player->GetOrientation() / piper2;
                    float const multi_int = floor(multi);
                    float const new_ori = (multi - multi_int > 0.5f) ? (multi_int + 1)*piper2 : multi_int*piper2;
                    player->SetFacingTo(new_ori);
                } break;
                case SELECTNEAR:
                {
                    GameObject* object = handler->GetNearbyGameObject();
                    if (!object)
                        ChatHandler(player->GetSession()).PSendSysMessage("No objects found");
                    else
                    {
                        GOMove::SendAdd(player, object->GetSpawnId());
                        session->SendAreaTriggerMessage("Selected %s", object->GetName().c_str());
                    }
                } break;
                }
            }
        }
        else if (ARG && ID >= SPAWN)
        {
            if (ID >= NORTH && ID <= PHASE)
            {
                GameObject* target = GOMove::GetGameObject(player, lowguid);
                if (!target)
                    ChatHandler(player->GetSession()).PSendSysMessage("Object GUID: %u not found", lowguid);
                else
                {
                    float x, y, z, o;
                    target->GetPosition(x, y, z, o);
                    uint32 p = target->GetPhaseMask();
                    float  originalScale = target->GetObjectScale();
                    float  scaleModifier = ((float) ARG) / 100;
                    
                    switch (ID)
                    {
                    case NORTH: GOMove::MoveGameObject(player, x + ((float)ARG / 100), y, z, o, p, lowguid);                            break;
                    case EAST: GOMove::MoveGameObject(player, x, y - ((float)ARG / 100), z, o, p, lowguid);                             break;
                    case SOUTH: GOMove::MoveGameObject(player, x - ((float)ARG / 100), y, z, o, p, lowguid);                            break;
                    case WEST: GOMove::MoveGameObject(player, x, y + ((float)ARG / 100), z, o, p, lowguid);                             break;
                    case NORTHEAST: GOMove::MoveGameObject(player, x + ((float)ARG / 100), y - ((float)ARG / 100), z, o, p, lowguid);   break;
                    case SOUTHEAST: GOMove::MoveGameObject(player, x - ((float)ARG / 100), y - ((float)ARG / 100), z, o, p, lowguid);   break;
                    case SOUTHWEST: GOMove::MoveGameObject(player, x - ((float)ARG / 100), y + ((float)ARG / 100), z, o, p, lowguid);   break;
                    case NORTHWEST: GOMove::MoveGameObject(player, x + ((float)ARG / 100), y + ((float)ARG / 100), z, o, p, lowguid);   break;
                    case UP: GOMove::MoveGameObject(player, x, y, z + ((float)ARG / 100), o, p, lowguid);                               break;
                    case DOWN: GOMove::MoveGameObject(player, x, y, z - ((float)ARG / 100), o, p, lowguid);                             break;
                    case RIGHT: GOMove::MoveGameObject(player, x, y, z, o - ((float)ARG / 100), p, lowguid);                            break;
                    case LEFT: GOMove::MoveGameObject(player, x, y, z, o + ((float)ARG / 100), p, lowguid);                             break;
                    case SIZEMINUS:
                    {

                        GOMove::ResizeGameObject(player, originalScale - scaleModifier, p, lowguid);
                        break;
                    }
                    case SIZEPLUS:
                    {
                        GOMove::ResizeGameObject(player, originalScale + scaleModifier, p, lowguid);
                        break;
                    }
                    case PHASE: GOMove::MoveGameObject(player, x, y, z, o, ARG, lowguid);                                               break;
                    }
                }
            }
            else
            {
                switch (ID)
                {
                case SPAWN: 
                {
                    if (GOMove::SpawnGameObject(player, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation(), player->GetPhaseMaskForSpawn(), ARG))
                        GOMove::Store.SpawnQueAdd(player->GetGUID(), ARG);
                } break;
                case SPAWNSPELL:
                {
                    GOMove::Store.SpawnQueAdd(player->GetGUID(), ARG);
                } break;
                case SELECTALLNEAR:
                {
                    for (GameObject const * go : GOMove::GetNearbyGameObjects(player, static_cast<float>(ARG)))
                        GOMove::SendAdd(player, go->GetSpawnId());
                } break;
                }
            }
        }
        else
        {
            return false;
        }
        return true;
    }
};

// possible spells:
// 27651, 897

class GOMove_spell_place : public SpellScriptLoader
{
public:
    GOMove_spell_place() : SpellScriptLoader("GOMove_spell_place") { }

    class GOMove_spellscript : public SpellScript
    {
        PrepareSpellScript(GOMove_spellscript);

        void HandleAfterCast()
        {
            if (!GetCaster())
                return;
            Player* player = GetCaster()->ToPlayer();
            if (!player)
                return;
            const WorldLocation* summonPos = GetExplTargetDest();
            if (!summonPos)
                return;
            if (uint32 entry = GOMove::Store.SpawnQueGet(player->GetGUID()))
                GOMove::SpawnGameObject(player, summonPos->GetPositionX(), summonPos->GetPositionY(), summonPos->GetPositionZ(), player->GetOrientation(), player->GetPhaseMaskForSpawn(), entry);
        }

        void Register() override
        {
            AfterCast += SpellCastFn(GOMove_spellscript::HandleAfterCast);
        }
    };

    SpellScript* GetSpellScript() const override
    {
        return new GOMove_spellscript();
    }
};

class GOMove_player_track : public PlayerScript
{
public:
    GOMove_player_track() : PlayerScript("GOMove_player_track") { }

    void OnLogout(Player* player) override
    {
        GOMove::Store.SpawnQueRem(player->GetGUID());
    }
};

bool GOMoveAnnounceModule;
class GOMove_conf : public WorldScript
{
public:
    GOMove_conf() : WorldScript("GOMove_conf") { }

    void OnBeforeConfigLoad(bool /*reload*/) override
    {
        GOMoveAnnounceModule = sConfigMgr->GetOption<bool>("GOMove.Announce", 1);
    }
};


class GoMoveAnnounce : public PlayerScript
{
public:

    GoMoveAnnounce() : PlayerScript("GoMoveAnnounce") {}

    void OnLogin(Player* player)
    {
        // Announce Module
        if (GOMoveAnnounceModule)
        {
            ChatHandler(player->GetSession()).SendSysMessage("This server is running the |cff4CFF00GO-Move |rmodule");
        }
    }
};


void AddGOMoveScripts()
{
    new GOMove_commandscript();
    new GOMove_spell_place();
    new GOMove_player_track();
    new GoMoveAnnounce();

}




