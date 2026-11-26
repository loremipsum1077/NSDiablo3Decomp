#include "ActorCommonData.h"
#include "Attributes/FastAttribConstants.h"
#include "Attributes/FastAttribute.h"
#include "GlobalSNO.h"
#include "Player.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../Server/PlayerList.h"
#include "../Server/Powers/PowerAPI.h"

// The 2.7.7 dark alchemy code reads its 30 node powers from a table at 0x7101901218 that the app-init callback
// 0x71004b6800 fills with GlobalSNOGet(GLOBALSNO_UNK_1599 + i) (P75_ItemPassive_DarkAlchemyMinor/Major_*). That
// callback is not decompiled, so the functions below call GlobalSNOGet directly (same values, different code).
static SNO sDarkAlchemyNodePower(int32 nNode)
{
    return GlobalSNOGet((GlobalSNO)(GLOBALSNO_UNK_1599 + nNode));
}

// Probably: dark alchemy "get unlocked node powers": appends the power SNO of every unlocked node (bits 0..29 of
// ATTRIBUTE_DARK_ALCHEMY_UNLOCKED_NODES) to arPowers. Caller: ActorCommonData.cpp.
// NON_MATCHING: target reads the node table at 0x7101901218 (see above); ours calls GlobalSNOGet per node
extern "C" void sub_71004b71b0(const ActorCommonData & tACD, blz::vector<SNO> & arPowers)
{
    uint32 uNodes = tACD.AttributesGetInt(ATTRIBUTE_DARK_ALCHEMY_UNLOCKED_NODES);
    if (uNodes == 0)
        return;
    for (int32 i = 0; i < 30; i++)
    {
        if (uNodes & (1u << i))
            arPowers.push_back(sDarkAlchemyNodePower(i));
    }
}


extern "C" BOOL sub_71004b72d0(const ActorCommonData & tACD)
{
    return (tACD.AttributesGetInt(ATTRIBUTE_DARK_ALCHEMY_UNLOCKED_NODES) & 0x1c000000) != 0;
}

extern "C" uint32 sub_71004b72f0(const ActorCommonData & tACD)
{
    return tACD.AttributesGetInt(ATTRIBUTE_DARK_ALCHEMY_UNLOCKED_NODES);
}

extern "C" BOOL sub_71004b7300(const ActorCommonData & tACD)
{
    uint32 nodes = sub_71004b72f0(tACD);
    size_t count = 0;
    while (nodes)
    {
        count += nodes & 1;
        nodes >>= 1;
    }
    return static_cast<uint32>(count) == 30;
}

extern "C" BOOL sub_71004b81d0(uint32 index)
{
    return index < 30;
}

extern "C" BOOL sub_71004b81e0(const ActorCommonData & tACD, uint32 index)
{
    if (!sub_71004b81d0(index))
        return FALSE;
    return (sub_71004b72f0(tACD) & (1u << index)) != 0;
}

extern "C" int32 sub_71004b8330(const ActorCommonData & tACD)
{
    uint32 nodes = sub_71004b72f0(tACD);
    size_t count = 0;
    while (nodes)
    {
        count += nodes & 1;
        nodes >>= 1;
    }

    int32 potions = sub_71004b81e0(tACD, 26) + sub_71004b81e0(tACD, 27) + sub_71004b81e0(tACD, 28);
    if (potions == 3)
        potions += sub_71004b81e0(tACD, 29);
    return count - potions;
}

extern "C" int32 sub_71004b83e0(uint32 index)
{
    if (index - 26u > 3u)
        return sub_71004b81d0(index);
    return 2;
}

// The four helpers below test the node's power as a buff on the actor first (PowerAPI_BuffFindOnACD with node
// power table entries 2, 0, 0 and 1), then the node bit in ATTRIBUTE_DARK_ALCHEMY_UNLOCKED_NODES.

// Probably: dark alchemy node 2 "no level requirement": returns 1 instead of nLevel when node 2 is active.
// Callers: ItemOwnerMeetsRequirements (Items.cpp), ActorInventory.cpp, UIHotbarButtonControl.cpp.
// NON_MATCHING: target loads the node power from the table at 0x7101901218; ours calls GlobalSNOGet
extern "C" int32 sub_71004b8900(const ActorCommonData * ptACD, int32 nLevel)
{
    if (ptACD == NULL)
        return nLevel;
    if (PowerAPI_BuffFindOnACD(sDarkAlchemyNodePower(2), -1, *ptACD, ACDID_INVALID, FALSE) != BUFFID_INVALID)
        return 1;
    return (ptACD->AttributesGetInt(ATTRIBUTE_DARK_ALCHEMY_UNLOCKED_NODES) & 4) ? 1 : nLevel;
}

// Probably: dark alchemy node 0 doubling of an integer (Player.cpp: streak timer tolerance in ticks).
// NON_MATCHING: target loads the node power from the table at 0x7101901218; ours calls GlobalSNOGet
extern "C" int32 sub_71004b8980(const ActorCommonData * ptACDPlayer, int32 nValue)
{
    if (ptACDPlayer == NULL)
        return nValue;
    if (PowerAPI_BuffFindOnACD(sDarkAlchemyNodePower(0), -1, *ptACDPlayer, ACDID_INVALID, FALSE) != BUFFID_INVALID)
        return nValue << 1;
    return nValue << (ptACDPlayer->AttributesGetInt(ATTRIBUTE_DARK_ALCHEMY_UNLOCKED_NODES) & 1);
}

// Probably: dark alchemy node 0 doubling of a float (SPlayer.cpp: massacre XP multiplier).
// NON_MATCHING: target loads the node power from the table at 0x7101901218; ours calls GlobalSNOGet
extern "C" float sub_71004b89f0(const ActorCommonData * ptACDPlayer, float flValue)
{
    if (ptACDPlayer == NULL)
        return flValue;
    if (PowerAPI_BuffFindOnACD(sDarkAlchemyNodePower(0), -1, *ptACDPlayer, ACDID_INVALID, FALSE) != BUFFID_INVALID)
        return flValue + flValue;
    return (ptACDPlayer->AttributesGetInt(ATTRIBUTE_DARK_ALCHEMY_UNLOCKED_NODES) & 1) ? flValue + flValue : flValue;
}

// Probably: dark alchemy node 1 active on the actor (Experience.cpp: rest XP refill).
// NON_MATCHING: target loads the node power from the table at 0x7101901218; ours calls GlobalSNOGet
extern "C" BOOL sub_71004b8ae0(const ActorCommonData * ptACD)
{
    if (ptACD == NULL)
        return FALSE;
    if (PowerAPI_BuffFindOnACD(sDarkAlchemyNodePower(1), -1, *ptACD, ACDID_INVALID, FALSE) != BUFFID_INVALID)
        return TRUE;
    return (ptACD->AttributesGetInt(ATTRIBUTE_DARK_ALCHEMY_UNLOCKED_NODES) >> 1) & 1;
}

extern "C" BOOL sub_71004b8b80(const ActorCommonData & first, const ActorCommonData & second)
{
    if (ACDGetLevelArea(second) != ACDGetLevelArea(first))
        return FALSE;
    WorldPlace firstPlace = first.GetPlace();
    WorldPlace secondPlace = second.GetPlace();
    if (firstPlace.idSWorld != secondPlace.idSWorld)
        return FALSE;
    float dx = secondPlace.wp.x - firstPlace.wp.x;
    float dy = secondPlace.wp.y - firstPlace.wp.y;
    return !(dx * dx + dy * dy > 225.0f);
}

// Probably: dark alchemy node 1 active on tACD or on any player local to it (BrainComponents.cpp: TRUE keeps rest XP
// on death).
// NON_MATCHING: target loads the node power from the table at 0x7101901218; ours calls GlobalSNOGet
extern "C" BOOL sub_71004b8d40(const ActorCommonData & tACD)
{
    if (PowerAPI_BuffFindOnACD(sDarkAlchemyNodePower(1), -1, tACD, ACDID_INVALID, FALSE) != BUFFID_INVALID)
        return TRUE;
    if (tACD.AttributesGetInt(ATTRIBUTE_DARK_ALCHEMY_UNLOCKED_NODES) & 2)
        return TRUE;

    Player * ptPlayer = PlayerGetByACD(tACD.id);
    if (ptPlayer == NULL)
        return FALSE;

    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetAllPlayersLocalToPlayer(listPlayers, ptPlayer);
    POS pos = listPlayers.GetHeadPosition();
    while (pos)
    {
        Player * ptLocalPlayer = listPlayers.GetNext(pos);
        const ActorCommonData * ptACD = ACDTryToGet(ptLocalPlayer->idPlayerACD);
        if (ptACD == NULL)
            continue;
        if (PowerAPI_BuffFindOnACD(sDarkAlchemyNodePower(1), -1, *ptACD, ACDID_INVALID, FALSE) != BUFFID_INVALID)
            return TRUE;
        if (ptACD->AttributesGetInt(ATTRIBUTE_DARK_ALCHEMY_UNLOCKED_NODES) & 2)
            return TRUE;
    }
    return FALSE;
}
