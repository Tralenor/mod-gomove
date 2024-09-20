/*
GOMove By Rochet2
http://rochet2.github.io/
Port as AC-Module and slight rework by Tralenor
Original idea by Mordred
Objscale Integration partly from Magnus
*/

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
#include "WorldPacket.h"
#include "GOMove.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "ObjectAccessor.h"
#include "DBCStores.h"
#include "objscale.h"


GameObjectStore GOMove::Store;


void GOMove::SendAddonMessage(Player * player, const char * msg)
{
    if (!player || !msg)
        return;

    char buf[256];
    snprintf(buf, 256, "GOMOVE\t%s", msg);

    // copy paste addon message packet
    WorldPacket data; // Needs a custom built packet since TC doesnt send guid
    uint32 messageLength = static_cast<uint32>(std::strlen(buf) + 1);
    uint64      objGuid       = player->GetGUID().GetRawValue();
    data.Initialize(SMSG_MESSAGECHAT, 100);
    data << uint8(CHAT_MSG_SYSTEM);
    data << int32(LANG_ADDON);
    data << static_cast<uint64>(objGuid);
    data << uint32(0);
    data << static_cast<uint64>(objGuid);
    data << uint32(messageLength);
    data << buf;
    data << uint8(0);
    player->GetSession()->SendPacket(&data);
}

GameObject * GOMove::GetGameObject(Player * player, ObjectGuid::LowType lowguid)
{
    return ChatHandler(player->GetSession()).GetObjectFromPlayerMapByDbGuid(lowguid);
}

void GOMove::SendAdd(Player * player, ObjectGuid::LowType lowguid)
{
    GameObjectData const* data = sObjectMgr->GetGameObjectData(lowguid);
    if (!data)
        return;
    GameObjectTemplate const* temp = sObjectMgr->GetGameObjectTemplate(data->id);
    if (!temp)
        return;
    char msg[256];
    snprintf(msg, 256, "ADD|%u|%s|%u", lowguid, temp->name.c_str(), data->id);
    SendAddonMessage(player, msg);
}

void GOMove::SendRemove(Player * player, ObjectGuid::LowType lowguid)
{
    char msg[256];
    snprintf(msg, 256, "REMOVE|%u||0", lowguid);

    SendAddonMessage(player, msg);
}

void GOMove::DeleteGameObject(Player* player, GameObject* object, ObjectGuid::LowType guidLow)
{
    if (!player)
    {
        return;
    }

    ChatHandler handler = ChatHandler(player->GetSession());

    if (!object)
    {
        handler.PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, guidLow);
        handler.SetSentErrorMessage(true);
        return;
    }


    ObjectGuid ownerGuid = object->GetOwnerGUID();
    if (ownerGuid)
    {
        Unit* owner = ObjectAccessor::GetUnit(*player, ownerGuid);
        if (!owner || !ownerGuid.IsPlayer())
        {
            handler.PSendSysMessage(LANG_COMMAND_DELOBJREFERCREATURE, ownerGuid.GetCounter(), object->GetSpawnId());
            handler.SetSentErrorMessage(true);
            return;
        }

        owner->RemoveGameObject(object, false);
    }

    object->SetRespawnTime(0); // not save respawn time
    object->Delete();
    object->DeleteFromDB();

    handler.PSendSysMessage(LANG_COMMAND_DELOBJMESSAGE, object->GetSpawnId());



}

GameObject * GOMove::SpawnGameObject(Player* player, float x, float y, float z, float o, uint32 p, uint32 entry)
{

    if (!player || !entry)
        return nullptr;
    ChatHandler handler = ChatHandler(player->GetSession());
    
    if (!Acore::IsValidMapCoord(player->GetMapId(), x, y, z))
        return nullptr;
    
    
    
    // copy paste .gob add command
    GameObjectTemplate const* objectInfo = sObjectMgr->GetGameObjectTemplate(entry);
    if (!objectInfo)
    {
        handler.PSendSysMessage(LANG_GAMEOBJECT_NOT_EXIST, entry);
        handler.SetSentErrorMessage(true);
        return nullptr;
    }

    if (objectInfo->displayId && !sGameObjectDisplayInfoStore.LookupEntry(objectInfo->displayId))
    {
        handler.PSendSysMessage(LANG_GAMEOBJECT_HAVE_INVALID_DATA, entry);
        handler.SetSentErrorMessage(true);
        return nullptr;
    }

    Map* map = player->GetMap();

    GameObject* object = new GameObject();
    ObjectGuid::LowType guidLow = map->GenerateLowGuid<HighGuid::GameObject>();

    if (!object->Create(guidLow, objectInfo->entry, map, player->GetPhaseMaskForSpawn(), x, y, z, o,G3D::Quat(), 0, GO_STATE_READY))
    {
        delete object;
        return nullptr;
    }

    // fill the gameobject data and save to the db
    object->SaveToDB(map->GetId(), (1 << map->GetSpawnMode()), p);
    guidLow = object->GetSpawnId();

    // delete the old object and do a clean load from DB with a fresh new GameObject instance.
    // this is required to avoid weird behavior and memory leaks
    delete object;

    object = new GameObject(); 
    // this will generate a new guid if the object is in an instance
    if (!object->LoadGameObjectFromDB(guidLow, map))
    {
        delete object;
        return nullptr;
    }

    /// @todo is it really necessary to add both the real and DB table guid here ?
    sObjectMgr->AddGameobjectToGrid(guidLow, sObjectMgr->GetGameObjectData(guidLow));

    if (object)
        SendAdd(player, guidLow);
    return object;
}

GameObject* GOMove::MoveGameObject(Player* player, float x, float y, float z, float o, uint32 p, ObjectGuid::LowType guidLow)
{

    if (!player)
        return nullptr;
    ChatHandler handler = ChatHandler(player->GetSession());

    GameObject* object = ChatHandler(player->GetSession()).GetObjectFromPlayerMapByDbGuid(guidLow);
    if (!object)
    {
        handler.PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, guidLow);
        handler.SetSentErrorMessage(true);
    }

    Map* map = object->GetMap();
    if (!Acore::IsValidMapCoord(object->GetMapId(), x, y, z))
    {
        handler.PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, object->GetMapId());
        handler.SetSentErrorMessage(true);
        return nullptr;
    }
    
    // copy paste .gob move command
    // copy paste .gob turn command

    Position pos;
    pos.Relocate(x, y, z);


    pos.SetOrientation(o);
    object->Relocate(pos);

    
    // update which cell has this gameobject registered for loading
    sObjectMgr->RemoveGameobjectFromGrid(guidLow, object->GetGameObjectData());
    object->SaveToDB();
    sObjectMgr->AddGameobjectToGrid(guidLow, object->GetGameObjectData());

    // Generate a completely new spawn with new guid
    // 3.3.5a client caches recently deleted objects and brings them back to life
    // when CreateObject block for this guid is received again
    // however it entirely skips parsing that block and only uses already known location
    object->Delete();

    object = new GameObject();
    if (!object->LoadGameObjectFromDB(guidLow, map))
    {
        delete object;
        return nullptr;
    }

    // copy paste from .gob phase command
    object->SetPhaseMask(p, true);
    object->SaveToDB();

    return object;
}
GameObject* GOMove::ResizeGameObject(Player* player, float scale, uint32 p, ObjectGuid::LowType lowguid)
{
    if (!player)
        return nullptr;
    GameObject* object = ChatHandler(player->GetSession()).GetObjectFromPlayerMapByDbGuid(lowguid);
    if (!object)
    {
        SendRemove(player, lowguid);
        return nullptr;
    }
    Map* map = object->GetMap();

    object->CustomData.Set("scale", new Objscale(scale));
    object->SetObjectScale(scale);

    object->DestroyForNearbyPlayers();
    object->UpdateObjectVisibility();
    object->SaveToDB();

    // Generate a completely new spawn with new guid
    // 3.3.5a client caches recently deleted objects and brings them back to life
    // when CreateObject block for this guid is received again
    // however it entirely skips parsing that block and only uses already known location
    object->Delete();

    object = new GameObject();
    if (!object->LoadGameObjectFromDB(lowguid, map))
    {
        delete object;
        SendRemove(player, lowguid);
        return nullptr;
    }

    // copy paste from .gob phase command
    object->SetPhaseMask(p, true);
    object->SaveToDB();

    return object;
}

void GameObjectStore::SpawnQueAdd(ObjectGuid const & guid, uint32 entry)
{
    WriteGuard lock(_objectsToSpawnLock);
    objectsToSpawn[guid] = entry;
}

void GameObjectStore::SpawnQueRem(ObjectGuid const & guid)
{
    WriteGuard lock(_objectsToSpawnLock);
    objectsToSpawn.erase(guid);
}

uint32 GameObjectStore::SpawnQueGet(ObjectGuid const & guid)
{
    WriteGuard lock(_objectsToSpawnLock);
    auto it = objectsToSpawn.find(guid);
    if (it != objectsToSpawn.end())
        return it->second;
    return 0;
}

std::list<GameObject*> GOMove::GetNearbyGameObjects(Player* player, float range)
{
    float x, y, z;
    player->GetPosition(x, y, z);

    std::list<GameObject*> objects;
    Acore::GameObjectInRangeCheck check(x, y, z, range);
    Acore::GameObjectListSearcher<Acore::GameObjectInRangeCheck> searcher(player, objects, check);
    Cell::VisitGridObjects(player, searcher, SIZE_OF_GRIDS);
    return objects;
}
