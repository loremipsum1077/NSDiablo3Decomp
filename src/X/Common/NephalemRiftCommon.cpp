// Original file: X/Common/NephalemRiftCommon.cpp

#include "NephalemRiftCommon.h"

#include "ActorCommonData.h"
#include "ActorInventory.h"
#include "Attributes/FastAttribConstantsSwig.h"
#include "Attributes/FastAttribute.h"
#include "Definitions/GameBalanceDefinition.h"
#include "Game.h"
#include "GameBalance.h"
#include "GameCommonData.h"
#include "GameSwig.h"
#include "GlobalSNO.h"
#include "ItemsSwig.h"
#include "Player.h"
#include "../Client/CGame.h"
#include "../Sigma/Math/MathGeneral.h"

namespace NephalemRiftManager {  // X/Server/System/SNephalemRift.h
int32 RiftPlayerCount();
}

namespace NephalemRift {









inline BOOL IsRiftKey(const GBHandle & gbHandle)
{


    if (!GBIDExists(gbHandle))
    {
        return FALSE;
    }




    if (gbHandle.eType != GB_ITEMS)
    {
        return FALSE;
    }



    GBID gbidLootRunKey = StringToGBID("GreaterLootRunKey");


    return gbHandle == GBHandle(GB_ITEMS, gbidLootRunKey) ? TRUE : FALSE;
}





BOOL IsRiftKey(GBID gbidItem)
{

    GBHandle keyHandle(GB_ITEMS, gbidItem);
    return IsRiftKey(keyHandle);
}



BOOL IsRiftKey(ACDID idACDItem)
{

    ActorCommonData * ptItem = ACDTryToGet(idACDItem);
    if (ptItem == NULL)
    {
        return FALSE;
    }

    GBHandle hGBItem = ptItem->GetGBHandle();

    GBID gbidLootRunKey = StringToGBID("GreaterLootRunKey");
    GBHandle gbHandle;
    gbHandle.eType = GB_ITEMS;
    gbHandle.gbid = gbidLootRunKey;




    if (hGBItem == gbHandle)
    {
        return TRUE;
    }
    return FALSE;
}


int32 GetKeyEntryCost(int32 nRiftTier)
{
    return nRiftTier >= 0 ? 1 : 0;
}











ACDID GetKeyForGreaterRift(Player * ptPlayer, int32 & nCount)
{

    if (ptPlayer == NULL)
    {
        return ACDID_INVALID;
    }


    ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
    if (ptACD == NULL)
    {
        return ACDID_INVALID;
    }


    ACDID idACDItem = ACDID_INVALID;
    InventorySlotType eSlotsToCheckA[2] = { SLOT_PLAYER_BACKPACK, SLOT_NONE };





    InventorySlotType eSlotsToCheckB[2] = { SLOT_PLAYER_SHARED_STASH, SLOT_NONE };






    GBID gbidLootRunKey = StringToGBID("GreaterLootRunKey");
    GBHandle hGBItem;
    hGBItem.Set(GB_ITEMS, gbidLootRunKey);
    if (!GBIDExists(hGBItem))
    {
        return ACDID_INVALID;
    }


    ACDInventoryGetItem(*ptACD, hGBItem, eSlotsToCheckA, idACDItem, nCount, FALSE);
    if (nCount <= 0)
    {
        ACDInventoryGetItem(*ptACD, hGBItem, eSlotsToCheckB, idACDItem, nCount, FALSE);
    }


    return idACDItem;
}


extern "C" int32 sub_710051abb0(Player * ptPlayer)
{
    ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
    return ptACD ? static_cast<int32>(ptACD->GetCurrencyAmount(20)) : 0;
}

extern "C" BOOL sub_710051ac90(Player * ptPlayer)
{
    ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
    return ptACD ? ptACD->ModifyCurrencyAmount(-1, 20, GMR_PURCHASE, TRUE) : FALSE;
}

BOOL HasKeyForGreaterRift(Player * ptPlayer)
{
    return sub_710051abb0(ptPlayer) > 0;
}



BOOL IsTieredRiftActive()
{

    return GameGetActiveGameCommonData().tGameSyncedData.nOpenLootRunLevel >= 0;
}


int32 GetActiveTieredRiftLevel()
{

    return GameGetActiveGameCommonData().tGameSyncedData.nOpenLootRunLevel;
}


int32 GetMaxTieredRiftLevel()
{
    int32 nMaxTieredRiftLevel = 0;

    ScopedGameBalanceDefinitionLockR ptGameBalDef(GlobalSNOGet(GLOBALSNO_GB_TIERED_LOOT_RUN_LEVELS));
    if (ptGameBalDef)
    {

        nMaxTieredRiftLevel = ptGameBalDef->tTieredLootRunLevelTable.serTieredLootRunLevels.dwSize / sizeof(TieredLootRunLevel);


        nMaxTieredRiftLevel = Math::Pin<int32, int32>(nMaxTieredRiftLevel, GREATERRIFTLEVEL_MINIMUM, GREATERRIFTLEVEL_MAXIMUM);
    }
    return nMaxTieredRiftLevel;
}




















BOOL PlayerHasNoRiftDeaths(const Player * ptPlayer)
{

    if (ptPlayer != NULL)
    {

        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);

        if (ptACDPlayer != NULL && IsTieredRiftActive())
        {

            int32 nDeathCount = ptACDPlayer->AttributesGetInt(ATTRIBUTE_TIERED_LOOT_RUN_DEATH_COUNT);
            if (nDeathCount <= 0)
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}


int32 GetParticipatingPlayerCount()
{

    if (GameActiveGameCommonDataIsServer())
    {

        return NephalemRiftManager::RiftPlayerCount();
    }




    if (GameActiveGameCommonDataIsClient())
    {

        return CGameGlobalsGet().nDungeonFinderParticipatingPlayerCount;
    }


    return 0;
}



uint32 GetStartRiftDelaySeconds()
{
    return 3;
}


uint64 GetEmpoweredRiftCost(int32 nRiftLevel)
{

    ScopedGameBalanceDefinitionLockR pdefGB(GlobalSNOGet(GLOBALSNO_GB_TIERED_LOOT_RUN_LEVELS));
    const TieredLootRunLevel & tTieredLootRunLevel = pdefGB->tTieredLootRunLevelTable.ptTieredLootRunLevels[nRiftLevel];

    return tTieredLootRunLevel.tLootRunValues.nEmpowerCost;
}

extern "C" BOOL sub_710051b100();

extern "C" BOOL sub_710051b060()
{
    if (!IsTieredRiftActive() || PlayersGetCount() > 1)
        return FALSE;
    if (IsTieredRiftActive() && sub_710051b100())
        return FALSE;
    return TRUE;
}

extern "C" BOOL sub_710051b100()
{
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACD && FastAttribGetValueInt(ptACD->idFastAttrib, ATTRIBUTE_TIERED_LOOT_RUN_REWARD_CHOICE_STATE) == RIFT_GEM_UPGRADE)
            return TRUE;
    }
    return FALSE;
}

}  // namespace NephalemRift
