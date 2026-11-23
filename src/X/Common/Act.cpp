// Original file: X/Common/Act.cpp

#include "Act.h"
#include "../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../Sigma/Memory/SigmaMemory.h"
#include "../Server/SGame.h"
#include "Definitions/ActDefinition.h"
#include "Definitions/QuestRangeDefinition.h"
#include "Game.h"
#include "GameCommonData.h"
#include "GameSwig.h"
#include "GameTemplate.h"
#include "Shell/CmdLine.h"
#include <new>

// Act.h:13-19 in the DWARF; repeated here as in the other files that use them (Act.h is a subset).
struct CachedWaypoint {  // size 0x2c
    Act eSourceAct;  // 0x00
    WaypointInfo tInfo;  // 0x04
};
typedef blz::vector<CachedWaypoint, blz::allocator<CachedWaypoint> > CachedWaypointArray;

struct ActGlobals {  // size 0x18
    CachedWaypointArray arWaypoints;  // 0x00
};

// Declared here because their headers do not carry them yet.
const CmdLineParams * CmdLineGetParams();  // X/Common/Shell/CmdLine.cpp
Act ActEnumFromActIndex(const int32 nIndex);  // GameCommonData.cpp:380
BOOL QuestAreAllBountiesCompleteForAct(Act eAct);  // X/Common/Quests/Quests.cpp:3098
void ActCacheWaypoints(const Act eAct);
const CachedWaypointArray & ActGetCachedWaypointArray();









uint8 arrBonusActEntries[20] = { 0, 2, 1, 4, 3, 2, 4, 0, 1, 3, 4, 1, 2, 0, 3, 1, 0, 4, 2, 3 };







void ActsInitializeForGame()
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();


    XMemoryAllocatorInterface * ptMemInterface = AppGetCurrentMemoryInterface();

    ActGlobals * ptActGlobals = new (SigmaMemoryNew(sizeof(ActGlobals), 4, ptMemInterface, TRUE)) ActGlobals;

    tGCD.ptActGlobals = ptActGlobals;

    ActCacheWaypoints(GameGetCurrentAct());
}



void ActsDisposeForGame()
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();


    XMemoryAllocatorInterface * ptMemInterface = AppGetCurrentMemoryInterface();
    ActGlobals * ptActGlobals = tGCD.ptActGlobals;
    ptActGlobals->~ActGlobals();

    SigmaMemoryFree(ptActGlobals, ptMemInterface);

    tGCD.ptActGlobals = NULL;
}



void ActCacheWaypoints(const Act eAct)
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();

    ActGlobals * ptActGlobals = tGCD.ptActGlobals;
    if (ptActGlobals == NULL)
    {
        return;
    }

    ptActGlobals->arWaypoints.clear();

    if (eAct == ACT_OPEN_WORLD)
    {
        Act eActs[5] = { ACT_1, ACT_2, ACT_3, ACT_4, ACT_5 };
        for (int32 nActIndex = 0; nActIndex < 5; nActIndex++)
        {
            SNO snoAct = ActSNOFromActEnum(eActs[nActIndex]); ScopedActDefinitionLockR pdefAct(snoAct);


            for (int32 i = 0; i < 100; i++)
            {
                const WaypointInfo & tInfo = pdefAct->aWaypointInfo[i];

                if (tInfo.snoWorld != SNO_NONE && tInfo.snoLevelArea != SNO_NONE &&
                    (tInfo.dwFlags & 2))
                {

                    CachedWaypoint tWaypoint;
                    tWaypoint.eSourceAct = eActs[nActIndex];
                    tWaypoint.tInfo = tInfo;
                    ptActGlobals->arWaypoints.push_back(tWaypoint);
                }
            }
        }
    }
    else
    {
        SNO snoAct = ActSNOFromActEnum(eAct);
        if (snoAct != SNO_NONE)
        {
            ScopedActDefinitionLockR pdefAct(snoAct);

            for (int32 i = 0; i < 100; i++)
            {
                const WaypointInfo & tInfo = pdefAct->aWaypointInfo[i];

                if (tInfo.snoWorld != SNO_NONE && tInfo.snoLevelArea != SNO_NONE &&
                    (tInfo.dwFlags & 1))
                {

                    CachedWaypoint tWaypoint;
                    tWaypoint.eSourceAct = eAct;
                    tWaypoint.tInfo = tInfo;
                    ptActGlobals->arWaypoints.push_back(tWaypoint);
                }
            }
        }
    }
}



const CachedWaypointArray & ActGetCachedWaypointArray()
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();

    return tGCD.ptActGlobals->arWaypoints;
}




int32 ActGetWaypointByLevelArea(const SNO snoLevelArea,
                                BOOL bCheckActivationState)

{
    const CachedWaypointArray & tWaypoints = ActGetCachedWaypointArray();
    int32 nCount = (int32)tWaypoints.size();


    for (int32 i = 0; i < nCount; i++)
    {
        const CachedWaypoint & tWaypoint = tWaypoints[i];
        if (tWaypoint.tInfo.snoLevelArea == snoLevelArea)
        {
            if (GameIsOpenWorld() && !(tWaypoint.tInfo.dwFlags & 2))
                continue;
            if (GameIsStoryMode() && !(tWaypoint.tInfo.dwFlags & 1))
                continue;
            if (bCheckActivationState && GameActiveGameCommonDataIsServer() && !SGameGlobalsGet().WaypointIsActive(i))
                continue;
            return i;
        }
    }
















    return -1;
}


SNO ActGetLevelAreaByWaypointIndex(int32 nIndex)

{
    const CachedWaypointArray & tWaypoints = ActGetCachedWaypointArray();
    int32 nCount = (int32)tWaypoints.size();
    if (nIndex >= 0 && nIndex < nCount)
    {
        return tWaypoints[nIndex].tInfo.snoLevelArea;
    }

    return SNO_NONE;
}




Act ActGetWaypointActByLevelArea(const SNO snoLevelArea)

{
    if (snoLevelArea == SNO_NONE)
        return ACT_INVALID;

    const CachedWaypointArray & tWaypoints = ActGetCachedWaypointArray();
    size_t nCount = tWaypoints.size();

    for (uint32 i = 0; i < nCount; i++)
    {

        const CachedWaypoint & tWaypoint = tWaypoints[i];
        if (tWaypoint.tInfo.snoLevelArea == snoLevelArea)
            return tWaypoint.eSourceAct;
    }


    return ACT_INVALID;
}


























void ActGetCurrentStartLocation(ResolvedPortalDestination & tDest)

{
    tDest.snoWorld = SNO_NONE; tDest.uEntranceGUID = (uint32)-1;


    Act eAct = GameGetCurrentAct();
    SNO snoAct = ActSNOFromActEnum(eAct);
    if (snoAct == SNO_NONE)
        return;

    ScopedActDefinitionLockR pdefAct(snoAct);

    if (GameIsOpenWorld())
    {

        Act eOpenWorldAct = GameGetOpenWorldModeAct();

        if (eOpenWorldAct == ACT_INVALID)
        {

            if (GameGetActiveGameCommonData().tGameSyncedData.eOpenWorldDefaultAct != ACT_INVALID)
            {
                eOpenWorldAct = GameGetActiveGameCommonData().tGameSyncedData.eOpenWorldDefaultAct;
            }
        }

        if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        {
            tDest = pdefAct->aStartLocOverride[4].tStartLocation; return;
        }


        switch (eOpenWorldAct)
        {


        case ACT_1:
            tDest = pdefAct->aStartLocOverride[0].tStartLocation; return;

        case ACT_2:
            tDest = pdefAct->aStartLocOverride[1].tStartLocation; return;

        case ACT_3:
        case ACT_4:
            tDest = pdefAct->aStartLocOverride[2].tStartLocation; return;

        case ACT_5:
            tDest = pdefAct->aStartLocOverride[3].tStartLocation; return;
        }
    }



    for (int32 i = 0; i < MAX_START_LOCATION_OVERRIDES; i++)
    {
        const ActStartLocOverride & tOverride = pdefAct->aStartLocOverride[i];

        if (tOverride.snoQuestRange != SNO_NONE && tOverride.tStartLocation.snoWorld != SNO_NONE &&
            QuestRangeEvaluate(tOverride.snoQuestRange, NULL))
        {
            tDest = tOverride.tStartLocation; return;
        }
    }






    if (pdefAct->tStartLocDefault.snoWorld != SNO_NONE) tDest = pdefAct->tStartLocDefault;
}








































SNO ActGetCurrentHubWorldSNO()

{
    Act eAct = GameGetCurrentAct();
    SNO snoAct = ActSNOFromActEnum(eAct);
    if (snoAct != SNO_NONE)
    {
        ScopedActDefinitionLockR pdefAct(snoAct);
        for (int32 i = 0; i < MAX_START_LOCATION_OVERRIDES; i++)
        {
            const ActStartLocOverride & tOverride = pdefAct->aStartLocOverride[i];
            if (tOverride.snoQuestRange != SNO_NONE &&
                tOverride.tStartLocation.snoWorld != SNO_NONE)
            {
                if (QuestRangeEvaluate(tOverride.snoQuestRange, NULL))
                {
                    if (tOverride.snoHubWorld != SNO_NONE)
                        return tOverride.snoHubWorld;
                }
            }
        }
        if (pdefAct->tStartLocDefault.snoWorld != SNO_NONE) return pdefAct->tStartLocDefault.snoWorld;
    }
    return SNO_NONE;
}























































int32 ActGetNumQuests(const ActDefinition * ptActDef)
{
    return ptActDef->serQuests.dwSize / sizeof(ActQuestInfo);
}















































































uint32 sGetDefualtActIndex()
{

    Blizzard::Time::Timestamp tStartTime = Blizzard::Time::GetTimestamp();
    Blizzard::Time::TimeRec tTimeRecord;
    Blizzard::Time::BreakTime(tStartTime, tTimeRecord);

    int32 nHours = tTimeRecord.dayOfYear * 24 + tTimeRecord.hour;
    return arrBonusActEntries[nHours % (sizeof(arrBonusActEntries) / sizeof(arrBonusActEntries[0]))];
}





Act ActGetOpenWorldDefaultAct()

{
    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        return ACT_5;
    }









    return ActEnumFromActIndex(sGetDefualtActIndex());
}































BOOL ActAllBountiesComplete()

{
    if (!GameIsOpenWorld())
    {
        return FALSE;
    }

    for (uint nAct = 0; nAct < 5; nAct++)
    {
        if (!QuestAreAllBountiesCompleteForAct(ActEnumFromActIndex(nAct)))
        {
            return FALSE;
        }
    }

    return TRUE;
}
