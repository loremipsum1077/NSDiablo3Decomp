// Original file: X/Server/AI/AIStates.cpp
// Assembled by build/try/r2-aistates/asm.py: functions sit on their original lines (asserts embed __LINE__).

#include "AIStates.h"
#include "ActorAI.h"
#include "BrainComponents.h"
#include "../Actors/Actors.h"
#include "../Movement/SActorMovement.h"
#include "../SGame.h"
#include "../SGameManager.h"
#include "../Definitions/AIDefinition.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/GameCommonData.h"
#include "../../Common/Game.h"
#include "../../Common/Target.h"
#include "../../Common/Definitions/GameBalanceDefinition.h"
#include "../../Common/Attributes/FastAttribute.h"
#include "../../Common/Nav/NavigationMesh.h"
#include "../../Common/Nav/NavCellPath.h"
#include "../../Common/Movement/ActorMovement.h"
#include "../../Common/Definitions/PowerDefinition.h"
#include "../../Common/Definitions/MonsterDefinition.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/Powers/PowersCommon.h"
#include "../../Common/Powers/SpellEffectCommon.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/TagMaps/PowerTags.h"
#include "../../Common/TagMaps/MonsterTags.h"
#include "../../Common/GameTemplate.h"
#include "../System/LevelArea.h"
#include "../SNPC.h"
#include "../Powers/PowerAPI.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Sigma/Random.h"
#include "../../Sigma/DataStructures/XPooledList.h"
#include "../../Sigma/DataStructures/XPooledMap.h"
#include "../../../BlizzardCore/Include/Blizzard/Lock.h"
#include "../../Common/World.h"
#include "../../Common/ActorCommonDataSwig.h"
#include "../../Common/AnimPlayer.h"
#include "../../Common/GlobalSNO.h"
#include "../../Common/WorldCommonCollision.h"
#include "../../Sigma/Math/Sphere.h"
#include <math.h>
#include <float.h>
#include "../../Common/Nav/NavZoneIntersect.h"
// Declared here until their own headers are adopted (X/Server/AI/BrainAPI.h, X/Server/AI/ActorAI.h).
BOOL AIHasNodePath(ActorID idActor);  // ActorAI.cpp
BOOL AIIsTargetOnPath(ActorID idActor, const WorldPlace & tPlace, float flDist);  // ActorAI.cpp
BOOL AIIsDerailedFromPath(ActorID idActor, float flDist);  // ActorAI.cpp
float BrainAPIActorGetAwarenessScanRadius(ACDID idACD, WorldPlace & placeCenter);  // BrainAPI.cpp



















static const float TARGET_NEARBY_MIN_DISTANCE = 7.0f; static const float TARGET_NEARBY_MAX_DISTANCE = 12.0f;  // original lines 71-72
static const float TARGET_FAR_MIN_DISTANCE = 20.0f; static const float TARGET_FAR_MAX_DISTANCE = 30.0f;  // original lines 73-74
static const float PLAYER_FOLLOW_TARGET_MAX_DISTANCE = 80.0f; static const uint32 VALID_BLOCK_TICKS = 60;  // original lines 75-76
enum AIStateFlags {
    AISTATE_FLAG_INACTIVE = 0,
    AISTATE_FLAG_COUNT = 1,
};

enum {
    MAX_TARGET_MAP_PAIRS = 256,
    MAX_ENTRY_DATA = 32,
    MAX_MERGED_AI_POWER_ENTRIES = 32,
    MAX_MERGED_DEFS = 64,
};

struct EntryData {  // size 0x18
    AIEntry * ptEntry;  // 0x00
    ACDID idACDBroadcastSource;  // 0x08
    ACDID idACDCastLocation;  // 0x0c
    uint32 uWeight;  // 0x10
};

typedef XListMemoryPool<EntryData> TEntryDataPool;
typedef XPooledList<EntryData, const EntryData &> TEntryDataList;
typedef XPooledMap<int, int, Target, Target, 16> TargetMap;
typedef XMapMemoryPool<int, Target> TargetMapPool;

struct AIStateGameGlobals {  // size 0x1788
    TargetMapPool poolTargetMap;  // 0x00
    TEntryDataPool poolEntryData;  // 0x30
    Blizzard::Lock::Mutex tPoolMutex;  // 0x60
    AIEntry arTempAIEntries[32];  // 0x80
    int32 nCurrTempAIEntry;  // 0x1780
};

class CTargetCache;
// Condition functions not written yet; the table at line 4492 refers to them.
// Lines 106-127 and 198-203: declarations for functions of this file whose callees have no adopted header yet (repacked so that the CTargetCache class sits on its original lines 128-160).
#include "../SGameConstants.h"
#include "../../Sigma/MemoryPools.h"
#include "../../Common/Player.h"
#include "../../Common/Items.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../PlayerList.h"
typedef BOOL (*AIConditionFunc)(Actor &, AICondition &, CTargetCache &, EntryData &); void BrainAPIGetRunAwayPlace(ACDID idACDFearTarget, const Target & tTargetFearSource, WorldPlace & placeDest, float flDistanceMin, float flDistanceMax); /* BrainComponents.cpp:2249 */ BOOL PowerComputeDestination(SNO snoPower, const ActorMovement & tMovement, const WorldPlace & placeSource, const WorldPlace & placeRolloutSource, WorldPlace & placeDest, int32 nFlags, float flDistance); /* Powers.cpp */ BOOL WorldIsRotated(const SNO snoWorld); /* World.cpp:775 */ static BOOL sCondTargetWithinFacing(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData); static BOOL sCondTargetInRangeOfOwner(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData);
static BOOL sCondNoPathToTarget(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData); static BOOL sCondRunAwayDistanceFromTargetAtleast(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData); static BOOL sCondHasNoDirectPathToTarget(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData); static BOOL sCondHasDirectPathToTarget(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData); static BOOL sCondDirectPathToWithinRangeOfTarget(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData); static BOOL sCondTargetIsUpAgainstWall(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData); static AIStateInstanceData * sGetInstanceData(ActorID idActor);
static AIStateInstanceData * sGetInstanceData(Actor & tActor); void ACDsIntersectSphere(const WorldPlace & tPlace, const WorldDistance wdRadius, const ACDID idACDIgnore, XPooledDataIDList & listACDs, uint32 dwFlags); /* X/Common/ActorCommonData.cpp */ BOOL PowerActorCanResurrect(ActorID idActor); /* X/Server/ActorIsAffected.cpp:366 */ BOOL ACDCanAttackTarget(const ActorCommonData & tACD, const Target & tTarget, AutoPowerDefinitionR pdefPower, uint32 * pdwFailureReasons); /* X/Server/AI/ActorAI.cpp:1116 */ ACDID PlayerGetFirstPet(ACDID idACDPlayer, PetType eType, int32 & nPos); /* X/Common/Player.cpp:2589 */ ACDID PlayerGetNextPet(ACDID idACDPlayer, PetType eType, int32 & nPos); /* X/Common/Player.cpp:2600 */ BOOL EscortCanBePulledThroughWarp(const ActorCommonData & tACDEscort, const ActorCommonData & tACDPlayer); /* X/Common/Player.cpp:3256 */
BOOL ACDWarpToACD(ActorCommonData & tACDToWarp, const ActorCommonData & tACDTarget, const PlayerWarpReason eReason, float flMinWarpRadius); /* X/Server/AI/ActorAI.cpp:1802 */ int32 SPlayerAddPet(ACDID idACDPlayer, PetType eType, ACDID idACDPet, BOOL fSendNow, int32 nMaxCount, float (*pCompareFunc)(ACDID)); /* X/Server/SPlayer.cpp:2987 */ ACDID BrainAPIActorGetClosestTarget(ActorID idActor, SNO snoPower, BOOL bTargetFriendlies, BOOL (*pfnFilterCallback)(ACDID, ACDID)); /* X/Server/AI/BrainComponents.cpp:2542 */ ACDID BrainAPIChoosePrimaryTarget(ActorCommonData & tACD, BOOL bActiveEnemiesOnly); /* X/Server/AI/BrainComponents.cpp:3667 */ void TargetEvaluateCustom(Target & tTarget, ActorID idActor, SNO snoPower); /* X/Server/AI/TargetEvaluation.cpp:401 */ WorldPlace AIStateGetRandomSpotBehind(ACDID idACD, ActorCommonData * ptACDTarget, float flDistMin, float flDistMax); /* line 1449 */
WorldPlace AIStateGetRandomLOSPlace(ACDID idACD, float flDistMin, float flDistMax); /* line 1519 */ ACDID BrainAPIActorGetClosestPlusAllyTarget(ActorID idActor); /* X/Server/AI/BrainComponents.cpp:2569 */ ACDID BrainAPIActorGetHighestThreatTarget(ActorID idActor, BOOL bActiveEnemiesOnly); /* X/Server/AI/BrainComponents.cpp:2594 */ ACDID BrainAPIActorGetSwitchOnAttackTarget(ActorID idActor); ACDID BrainAPIActorGetClosestFriendlyLeaderTarget(ActorID idActor); /* X/Server/AI/BrainComponents.cpp:2637, :2586 */ BOOL ItemIsNeverTrash(GBID gbidItem); ItemQualityLevel ItemGetDisplayedQualityLevel(const ActorCommonData & tACD); BOOL ItemIsEquippableByPlayerOrHireling(const ActorCommonData & tACDItem); /* X/Common/Items.cpp:11171, :2208, :9448 */ BOOL ActorInventoryAllowedToPickupItem(const ActorCommonData & tACDPlayer, const ActorCommonData & tACDItem); /* X/Common/ActorInventory.cpp:13140 */
BOOL ACDCanHaveEnemies(Team eTeam); /* X/Common/ActorCommonData.cpp:2056 */ BOOL sBrainAPITargetFilterInCombat(ACDID, ACDID idACDTarget); BOOL sBrainAPITargetFilterSameTeam(ACDID idACDSource, ACDID idACDTarget); WorldPlace sGetPlaceCenterOfPack(ACDID idACD); /* lines 1625, 1633, 1590 */ typedef int32 AIDecisionType; /* X/Common/AI/ActorAIStatesSwig.h */ BOOL AIStateIsActive(ActorID idActor); /* line 6058 */ void AIStateClearOverridePower(ActorID idActor); /* line 6101 */ void PowerHelper_AddCooldown(ActorCommonData & tACD, SNO snoPower, uint32 uCooldownTicks, BOOL bReapplyPassivesWhenFinished); /* X/Server/Powers/PowerHelper.cpp:1498 */ void BrainAPISetTarget(ActorID idActor, ActorID idActorTarget); /* X/Server/AI/BrainComponents.cpp */ void BrainAPIWarnPackAboutTarget(ActorID idActor, ActorID idActorTarget); /* X/Server/AI/BrainComponents.cpp */
BOOL PowerIsCoolDownDone(const ActorCommonData & tACD, SNO snoPower); /* X/Server/Powers/Powers.cpp */ void ActorAISetMovementStyle(ActorID idActor, AIStateType eStateType, GBID gbidMovementStyle); /* X/Server/AI/ActorAI.cpp */






class CTargetCache  // size 0x618
{
public:
    CTargetCache(ActorID idActor)
    {

        for (int32 i = 0; i < AISTATE_TARGET_COUNT; i++)
        {
            m_Target[i].eType = TT_NONE;
            m_Target[i].idACDTarget = ACDID_INVALID;
        }
        m_dwCachedFlags = 0;
        m_idACD = ACDFindByActor(idActor);
        m_Target[AISTATE_TARGET_SELF].eType = TT_ACTOR;
        m_Target[AISTATE_TARGET_SELF].idACDTarget = m_idACD;

        AIStateGameGlobals * pGlobals = SGameManager::GameGet().ptAIStateGameGlobals;
        mapPowerTargets.SetPool(&pGlobals->poolTargetMap);
    }

    const Target & GetTarget(AIStateTargetType eTargetType, EntryData & tEntryData);

    Target m_Target[AISTATE_TARGET_COUNT];  // 0x00
    TargetMap mapPowerTargets;  // 0x560
    uint64 m_dwCachedFlags;  // 0x608
    ACDID m_idACD;  // 0x610
private:
    static const int32 MAX_TARGET_TYPES = 64;
};





void AIStateInitializeForGame()
{
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    AIStateGameGlobals *& ptAIStateGameGlobals = SGameManager::GameGet().ptAIStateGameGlobals;
    ptAIStateGameGlobals = (AIStateGameGlobals *)SigmaMemoryNew(sizeof(AIStateGameGlobals), 4, ptMemoryInterface, 1);

    ptAIStateGameGlobals->poolTargetMap.Initialize();
    ptAIStateGameGlobals->poolTargetMap.SetMemoryInterface(ptMemoryInterface);
    ptAIStateGameGlobals->poolTargetMap.SetBlockCount_Tracked(MAX_TARGET_MAP_PAIRS);

    ptAIStateGameGlobals->poolEntryData.Initialize();
    ptAIStateGameGlobals->poolEntryData.SetMemoryInterface(ptMemoryInterface);
    ptAIStateGameGlobals->poolEntryData.SetBlockCount_Tracked(MAX_ENTRY_DATA);

    ptAIStateGameGlobals->tPoolMutex.Create();
    ptAIStateGameGlobals->nCurrTempAIEntry = 0;
}






void AIStateDisposeForGame()
{
    XMemoryAllocatorInterface * ptMemoryInterface = AppGetCurrentMemoryInterface();
    AIStateGameGlobals *& ptAIStateGameGlobals = SGameManager::GameGet().ptAIStateGameGlobals;
    if (ptAIStateGameGlobals != NULL)
    {
        ptAIStateGameGlobals->poolEntryData.Dispose();
        ptAIStateGameGlobals->poolTargetMap.Dispose();
        ptAIStateGameGlobals->tPoolMutex.Destroy();
        SigmaMemoryFree(ptAIStateGameGlobals, ptMemoryInterface);
        ptAIStateGameGlobals = NULL;
    }
}






static AIEntry * sGetTempAIEntry()
{

    AIStateGameGlobals * ptGlobals = SGameManager::GameGet().ptAIStateGameGlobals;
    AIEntry * ptEntry = NULL;

    if (ptGlobals->nCurrTempAIEntry < MAX_MERGED_AI_POWER_ENTRIES)
    {
        ptEntry = &ptGlobals->arTempAIEntries[ptGlobals->nCurrTempAIEntry];
        ptGlobals->nCurrTempAIEntry++;
    }
    return ptEntry;
}





static ACDID sGetClosestWreckableTarget(
    ActorID idActor)
{
    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_BIT))
    {
        return ACDID_INVALID;
    }

    ACDID idACDWinner = ACDID_INVALID;
    ACDID idACD = ACDFindByActor(idActor);
    if (idACD != ACDID_INVALID)
    {
        WorldPlace placeCenter;
        float flTargetScanRadius = BrainAPIActorGetAwarenessScanRadius(idACD, placeCenter);
        XPooledDataIDList listACDsInRadius(MemoryPoolsGetDATAIDListPool());
        ACDsIntersectSphere(placeCenter, flTargetScanRadius, idACD, listACDsInRadius, 7);

        float flDistWinnerSqr = FLT_MAX;
        POS pos = listACDsInRadius.GetHeadPosition();
        while (pos)
        {
            ACDID idACDBody = (ACDID)listACDsInRadius.GetNext(pos);
            ActorCommonData & tACDBody = ACDGet(idACDBody);
            if (tACDBody.IsWreckableObject() && tACDBody.IsAlive())
            {
                float flDistSqr = ACDGetDistanceSquared(ACDGet(idACD), tACDBody, FALSE);
                if (flDistSqr < flDistWinnerSqr)
                {
                    flDistWinnerSqr = flDistSqr;
                    idACDWinner = idACDBody;
                }
            }
        }
    }

    return idACDWinner;
}











































static ACDID sGetClosestCorpseTarget(
    ActorID idActor)
{
    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_BIT))
    {
        return ACDID_INVALID;
    }

    ACDID idACDWinner = ACDID_INVALID;
    ACDID idACD = ACDFindByActor(idActor);
    if (idACD != ACDID_INVALID)
    {
        WorldPlace placeCenter;
        float flTargetScanRadius = BrainAPIActorGetAwarenessScanRadius(idACD, placeCenter);
        XPooledDataIDList listACDsInRadius(MemoryPoolsGetDATAIDListPool());
        ACDsIntersectSphere(placeCenter, flTargetScanRadius, idACD, listACDsInRadius, 7);

        float flDistWinnerSqr = FLT_MAX;
        POS pos = listACDsInRadius.GetHeadPosition();
        while (pos)
        {
            ACDID idACDBody = (ACDID)listACDsInRadius.GetNext(pos);
            ActorCommonData & tACDBody = ACDGet(idACDBody);
            ActorID idCorpse = ActorID(ACDGetOwner(idACDBody));
            if (tACDBody.IsCorpse() || PowerActorCanResurrect(idCorpse))
            {
                float flDistSqr = ACDGetDistanceSquared(ACDGet(idACD), tACDBody, FALSE);
                if (flDistSqr < flDistWinnerSqr)
                {
                    flDistWinnerSqr = flDistSqr;
                    idACDWinner = idACDBody;
                }
            }
        }
    }

    return idACDWinner;
}



static ACDID sGetClosestCorpseOfSameFamilyTarget(
    ActorID idActor,
    BOOL bNonEliteOnly)
{
    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_BIT))
    {
        return ACDID_INVALID;
    }

    ACDID idACDWinner = ACDID_INVALID;
    ACDID idACD = ACDFindByActor(idActor);
    if (idACD != ACDID_INVALID)
    {
        ActorCommonData & tACD = ACDGet(idACD);

        WorldPlace placeCenter;
        float flTargetScanRadius = BrainAPIActorGetAwarenessScanRadius(idACD, placeCenter);
        XPooledDataIDList listACDsInRadius(MemoryPoolsGetDATAIDListPool());
        ACDsIntersectSphere(placeCenter, flTargetScanRadius, idACD, listACDsInRadius, 7);

        float flDistWinnerSqr = FLT_MAX;
        POS pos = listACDsInRadius.GetHeadPosition();
        while (pos)
        {
            ACDID idACDBody = (ACDID)listACDsInRadius.GetNext(pos);
            ActorCommonData & tACDBody = ACDGet(idACDBody);
            ActorID idCorpse = ActorID(ACDGetOwner(idACDBody));
            if (tACDBody.IsMonster() && tACD.IsMonster() &&
                MonsterGetMonsterFamily(SNOName(ACTOR_SNO, tACDBody.snoActor)) == MonsterGetMonsterFamily(SNOName(ACTOR_SNO, tACD.snoActor)))
            {
                if (tACDBody.IsCorpse() || PowerActorCanResurrect(idCorpse))
                {
                    if (!bNonEliteOnly || !tACDBody.IsElite())
                    {
                        float flDistSqr = ACDGetDistanceSquared(tACD, tACDBody, FALSE);
                        if (flDistSqr < flDistWinnerSqr)
                        {
                            flDistWinnerSqr = flDistSqr;
                            idACDWinner = idACDBody;
                        }
                    }
                }
            }
        }
    }

    return idACDWinner;
}



static ACDID sGetClosestInPack(
    ActorID idActor)
{
    ACDID idACDWinner = ACDID_INVALID;
    ACDID idACD = ACDFindByActor(idActor);
    if (idACD == ACDID_INVALID) return idACDWinner;

    ActorCommonData & tACD = ACDGet(idACD);

    ACDPack * ptACDPack = ACDPackTryToGet(tACD.idACDPack);
    if (!ptACDPack)
    {
        return idACDWinner;
    }

    const ACDIDListPooled * ptMembers = ptACDPack->GetMemberList();

    float flDistWinnerSqr = FLT_MAX;

    POS pos = ptMembers->GetHeadPosition();
    while (pos)
    {
        ACDID idACDPackMember = (ACDID)ptMembers->GetNext(pos);

        if (idACDPackMember == idACD)
        {
            continue;
        }

        ActorCommonData * ptACDPackMember = ACDTryToGet(idACDPackMember);
        if (ptACDPackMember && ptACDPackMember->IsAlive())
        {
            float flDistSqr = ACDGetDistanceSquared(tACD, *ptACDPackMember, FALSE);
            if (flDistSqr < flDistWinnerSqr)
            {
                flDistWinnerSqr = flDistSqr;
                idACDWinner = idACDPackMember;
            }
        }
    }

    return idACDWinner;
}




static ACDID sGetClosestUndeadTarget(
    ActorID idActor)
{
    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_BIT))
    {
        return ACDID_INVALID;
    }

    ACDID idACDWinner = ACDID_INVALID;
    ACDID idACD = ACDFindByActor(idActor);
    if (idACD != ACDID_INVALID)
    {
        WorldPlace placeCenter;
        float flTargetScanRadius = BrainAPIActorGetAwarenessScanRadius(idACD, placeCenter);
        XPooledDataIDList listACDsInRadius(MemoryPoolsGetDATAIDListPool());
        ACDsIntersectSphere(placeCenter, flTargetScanRadius, idACD, listACDsInRadius, 7);

        float flDistWinnerSqr = FLT_MAX;
        POS pos = listACDsInRadius.GetHeadPosition();
        ActorCommonData & tACD = ACDGet(idACD);
        while (pos)
        {
            ACDID idACDMonster = (ACDID)listACDsInRadius.GetNext(pos);
            ActorCommonData & tACDMonster = ACDGet(idACDMonster);
            if (tACDMonster.IsUndeadMonster())
            {
                float flDistSqr = ACDGetDistanceSquared(tACD, tACDMonster, FALSE);
                if (flDistSqr < flDistWinnerSqr)
                {
                    flDistWinnerSqr = flDistSqr;
                    idACDWinner = idACDMonster;
                }
            }
        }
    }

    return idACDWinner;
}



static ACDID sGetClosestGoldTarget(
    ActorID idActor)
{
    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_BIT))
    {
        return ACDID_INVALID;
    }

    ACDID idACDWinner = ACDID_INVALID;
    ACDID idACD = ACDFindByActor(idActor);
    if (idACD != ACDID_INVALID)
    {
        WorldPlace placeCenter;
        float flTargetScanRadius = BrainAPIActorGetAwarenessScanRadius(idACD, placeCenter);
        XPooledDataIDList listACDsInRadius(MemoryPoolsGetDATAIDListPool());
        ACDsIntersectSphere(placeCenter, flTargetScanRadius, idACD, listACDsInRadius, 7);

        float flDistWinnerSqr = FLT_MAX;
        ActorCommonData & tACD = ACDGet(idACD);
        Player * ptControllingPlayer = ACDGetControllingPlayer(idACD);
        ActorCommonData * ptACDPlayer = ptControllingPlayer ? ACDTryToGet(ptControllingPlayer->idPlayerACD) : NULL;

        POS pos = listACDsInRadius.GetHeadPosition();
        while (pos)
        {
            ACDID idACDItem = (ACDID)listACDsInRadius.GetNext(pos);
            ActorCommonData & tACDItem = ACDGet(idACDItem);
            if (tACDItem.IsItem() && ItemGetItemTypeGBID(tACDItem.hGB.gbid) == GlobalGBIDGet(GLOBALGBID_ITEMTYPE_GOLD))
            {
                if (ptACDPlayer && !ActorInventoryAllowedToPickupItem(*ptACDPlayer, tACDItem))
                {
                    continue;
                }

                float flDistSqr = ACDGetDistanceSquared(tACD, tACDItem, FALSE);
                if (flDistSqr < flDistWinnerSqr)
                {
                    flDistWinnerSqr = flDistSqr;
                    idACDWinner = idACDItem;
                }
            }
        }
    }

    return idACDWinner;
}




static ACDID sGetClosestNewLegendaryTarget(
    ActorID idActor)
{
    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_BIT))
    {
        return ACDID_INVALID;
    }

    ACDID idACDWinner = ACDID_INVALID;
    ACDID idACD = ACDFindByActor(idActor);
    if (idACD != ACDID_INVALID)
    {
        WorldPlace placeCenter;
        float flTargetScanRadius = BrainAPIActorGetAwarenessScanRadius(idACD, placeCenter);
        XPooledDataIDList listACDsInRadius(MemoryPoolsGetDATAIDListPool());
        ACDsIntersectSphere(placeCenter, flTargetScanRadius, idACD, listACDsInRadius, 7);

        float flDistWinnerSqr = FLT_MAX;
        ActorCommonData & tACD = ACDGet(idACD);
        Player * ptControllingPlayer = ACDGetControllingPlayer(idACD);
        ActorCommonData * ptACDPlayer = ptControllingPlayer ? ACDTryToGet(ptControllingPlayer->idPlayerACD) : NULL;

        POS pos = listACDsInRadius.GetHeadPosition();
        while (pos)
        {
            ACDID idACDItem = (ACDID)listACDsInRadius.GetNext(pos);
            ActorCommonData & tACDItem = ACDGet(idACDItem);
            if (tACDItem.IsItem() && ItemIsLegendary(tACDItem.hGB.gbid))
            {
                if (ptACDPlayer && !ActorInventoryAllowedToPickupItem(*ptACDPlayer, tACDItem))
                {
                    continue;
                }

                float flDistSqr = ACDGetDistanceSquared(tACD, tACDItem, FALSE);
                if (flDistSqr < flDistWinnerSqr)
                {
                    flDistWinnerSqr = flDistSqr;
                    idACDWinner = idACDItem;
                }
            }
        }
    }

    return idACDWinner;
}



static ACDID sGetClosestTrashItemTarget(
    ActorID idActor, ItemQualityLevel eQualityMin, ItemQualityLevel eQualityMax)
{
    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_BIT))
    {
        return ACDID_INVALID;
    }

    ACDID idACDWinner = ACDID_INVALID;
    ACDID idACD = ACDFindByActor(idActor);
    if (idACD != ACDID_INVALID)
    {
        WorldPlace placeCenter;
        float flTargetScanRadius = BrainAPIActorGetAwarenessScanRadius(idACD, placeCenter);
        XPooledDataIDList listACDsInRadius(MemoryPoolsGetDATAIDListPool());
        ACDsIntersectSphere(placeCenter, flTargetScanRadius, idACD, listACDsInRadius, 7);

        float flDistWinnerSqr = FLT_MAX;
        ActorCommonData & tACD = ACDGet(idACD);
        Player * ptControllingPlayer = ACDGetControllingPlayer(idACD);
        ActorCommonData * ptACDPlayer = ptControllingPlayer ? ACDTryToGet(ptControllingPlayer->idPlayerACD) : NULL;

        POS pos = listACDsInRadius.GetHeadPosition();
        while (pos)
        {
            ACDID idACDItem = (ACDID)listACDsInRadius.GetNext(pos);
            ActorCommonData & tACDItem = ACDGet(idACDItem);
            if (tACDItem.IsItem())
            {
                ItemQualityLevel eQuality = ItemGetDisplayedQualityLevel(tACDItem);
                if (eQuality <= eQualityMax && eQuality >= eQualityMin &&
                    ItemIsEquippableByPlayerOrHireling(tACDItem))
                {

                    if (ItemIsNeverTrash(tACDItem.hGB.gbid))
                    {
                        continue;
                    }





                    if (ptACDPlayer && (tACDItem.GetAssignedHeroID() == OnlineService::INVALID_HERO_ID || !ActorInventoryAllowedToPickupItem(*ptACDPlayer, tACDItem)))
                    {
                        continue;
                    }


                    float flDistSqr = ACDGetDistanceSquared(tACD, tACDItem, FALSE);
                    if (flDistSqr < flDistWinnerSqr)
                    {
                        flDistWinnerSqr = flDistSqr;
                        idACDWinner = idACDItem;
                    }
                }
            }
        }
    }

    return idACDWinner;
}



static ACDID sGetClosestResplendentChestTarget(
    ActorID idActor)
{
    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_BIT))
    {
        return ACDID_INVALID;
    }

    ACDID idACDWinner = ACDID_INVALID;
    ACDID idACD = ACDFindByActor(idActor);
    if (idACD != ACDID_INVALID)
    {
        WorldPlace placeCenter;
        float flTargetScanRadius = BrainAPIActorGetAwarenessScanRadius(idACD, placeCenter);
        XPooledDataIDList listACDsInRadius(MemoryPoolsGetDATAIDListPool());
        ACDsIntersectSphere(placeCenter, flTargetScanRadius, idACD, listACDsInRadius, 7);

        float flDistWinnerSqr = FLT_MAX;
        ActorCommonData & tACD = ACDGet(idACD);
        Player * ptControllingPlayer = ACDGetControllingPlayer(idACD);
        ActorCommonData * ptACDPlayer = ptControllingPlayer ? ACDTryToGet(ptControllingPlayer->idPlayerACD) : NULL;

        POS pos = listACDsInRadius.GetHeadPosition();
        while (pos)
        {
            ACDID idACDChest = (ACDID)listACDsInRadius.GetNext(pos);
            ActorCommonData & tACDChest = ACDGet(idACDChest);
            if (tACDChest.IsChest() &&
                (!ptACDPlayer || tACDChest.IsOperatableGizmo(ptACDPlayer->id, FALSE)))
            {

                ScopedActorDefinitionLockR pdefActor(tACDChest.snoActor);
                SNO snoTreasureClass = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_GIZMO_TREASURE_CLASS);

                if (snoTreasureClass == GlobalSNOGet(GLOBALSNO_TREASURECLASS_RESPLENDENT_CHEST))
                {
                    float flDistSqr = ACDGetDistanceSquared(tACD, tACDChest, FALSE);
                    if (flDistSqr < flDistWinnerSqr)
                    {
                        flDistWinnerSqr = flDistSqr;
                        idACDWinner = idACDChest;
                    }
                }
            }
        }
    }

    return idACDWinner;
}




static ACDID sGetClosestEliteTarget(
    ActorID idActor)
{
    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_BIT))
    {
        return ACDID_INVALID;
    }

    ACDID idACDWinner = ACDID_INVALID;
    ACDID idACD = ACDFindByActor(idActor);
    if (idACD != ACDID_INVALID)
    {
        WorldPlace placeCenter;
        float flTargetScanRadius = BrainAPIActorGetAwarenessScanRadius(idACD, placeCenter);
        XPooledDataIDList listACDsInRadius(MemoryPoolsGetDATAIDListPool());
        ACDsIntersectSphere(placeCenter, flTargetScanRadius, idACD, listACDsInRadius, 7);

        float flDistWinnerSqr = FLT_MAX;
        ActorCommonData & tACD = ACDGet(idACD);

        POS pos = listACDsInRadius.GetHeadPosition();
        while (pos)
        {
            ACDID idACDElite = (ACDID)listACDsInRadius.GetNext(pos);
            ActorCommonData & tACDElite = ACDGet(idACDElite);
            if (tACDElite.IsElite() && tACD.IsAlive() && !ACDIsFriendly(tACD, tACDElite, TRUE))
            {
                float flDistSqr = ACDGetDistanceSquared(tACD, tACDElite, FALSE);
                if (flDistSqr < flDistWinnerSqr)
                {
                    flDistWinnerSqr = flDistSqr;
                    idACDWinner = idACDElite;
                }
            }
        }
    }

    return idACDWinner;
}



static ACDID sGetClosestPet(
    ActorID idActor)
{
    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_BIT) ||
        SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_TO_PLAYERS_BIT) ||
        sgtAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_FLYTHROUGH_BIT))
    {
        return ACDID_INVALID;
    }


    ACDID idACD = ACDFindByActor(idActor);
    if (idACD == ACDID_INVALID)
    {
        return ACDID_INVALID;
    }

    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
    {
        return ACDID_INVALID;
    }

    if (ptACD->IsCharmed())
    {
        return ACDID_INVALID;
    }

    WorldPlace placeCenter;
    float flTargetScanRadius = BrainAPIActorGetAwarenessScanRadius(idACD, placeCenter);

    ACDID idACDWinner = ACDID_INVALID;
    float flDistWinner = FLT_MAX;
    BOOL bWinnerAlive = FALSE;


    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listPlayers, -1, FALSE, -1);

    POS pos = listPlayers.GetHeadPosition();
    WorldPlace placeACD = ptACD->GetPlace();
    while (pos)
    {
        const Player * ptPlayer = listPlayers.GetNext(pos);


        if (ptPlayer->idPlayerACD == idACD || ptPlayer->idPlayerACD == ACDID_INVALID)
        {
            continue;
        }

        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (!ptACDPlayer)
        {
            continue;
        }


        WorldPlace placeACDPlayer = ptACDPlayer->GetPlace();
        if (placeACD.idSWorld != placeACDPlayer.idSWorld)
        {
            continue;
        }

        int32 nPos;
        ACDID idACDPet = PlayerGetFirstPet(ptACDPlayer->id, -1, nPos);

        XPooledDataIDList listPets(MemoryPoolsGetDATAIDListPool());

        while (idACDPet != ACDID_INVALID)
        {

            ActorCommonData * ptACDPet = ACDTryToGet(idACDPet);
            if (ptACDPet)
            {
                BOOL bCurAlive = ptACDPet->IsAlive();
                WorldPlace placeACDPet = ptACDPet->GetPlace();
                if (placeACD.idSWorld == placeACDPet.idSWorld)
                {
                    float flDist = WorldPlaceDistance(placeACD, placeACDPet);
                    if (flDist > flTargetScanRadius)
                    {
                        idACDPet = PlayerGetNextPet(ptACDPlayer->id, -1, nPos);
                        continue;
                    }


                    float flCustomWeight = ptACDPet->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_CUSTOM_TARGET_WEIGHT, ptACD->snoActor));
                    flDist -= flCustomWeight;

                    if ((bWinnerAlive == bCurAlive && flDist < flDistWinner) || (!bWinnerAlive && bCurAlive))
                    {
                        idACDWinner = ptACDPet->id;
                        flDistWinner = flDist;
                        bWinnerAlive = bCurAlive;
                    }

                }
            }

            idACDPet = PlayerGetNextPet(ptACDPlayer->id, -1, nPos);
        }
    }

    return idACDWinner;
}



static ACDID sGetTauntTarget(
    ActorID idActor)
{
    ACDID idACD = ACDFindByActor(idActor);
    if (idACD != ACDID_INVALID)
    {
        WorldPlace placeCenter;
        float flTargetScanRadius = BrainAPIActorGetAwarenessScanRadius(idACD, placeCenter);

        XPooledDataIDList listACDsInRadius(MemoryPoolsGetDATAIDListPool());
        ACDsIntersectSphere(placeCenter, flTargetScanRadius, ACDID_INVALID, listACDsInRadius, 7);


        ActorCommonData & tACD = ACDGet(idACD);
        POS posBuff = tACD.listBuffs.GetTailPosition();
        while (posBuff)
        {
            BuffID idBuff = tACD.listBuffs.GetPrev(posBuff);

            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff && !ptBuff->bDeleted)
            {
                ACDID idACDBuffTauntTarget = (ACDID)FastAttribGetValueInt(ptBuff->idFastAttrib, FastAttribKey(ATTRIBUTE_TAUNT_TARGET_ACDID));
                if (idACDBuffTauntTarget != ACDID_INVALID)
                {

                    POS pos = listACDsInRadius.GetHeadPosition();
                    while (pos)
                    {
                        ACDID idACDCurrent = (ACDID)listACDsInRadius.GetNext(pos);
                        ActorCommonData & tACDCurrent = ACDGet(idACDCurrent);
                        if (idACDCurrent == idACDBuffTauntTarget && (tACDCurrent.IsAlive() || tACDCurrent.AttributesGetInt(FastAttribKey(ATTRIBUTE_IS_POWER_PROXY))))
                        {
                            return idACDBuffTauntTarget;
                        }
                    }
                }
            }
        }
    }

    return ACDID_INVALID;
}



static ACDID sGetCharmSource(
    ActorID idActor)
{
    ACDID idACD = ACDFindByActor(idActor);
    if (idACD != ACDID_INVALID)
    {
        WorldPlace placeCenter;
        float flTargetScanRadius = BrainAPIActorGetAwarenessScanRadius(idACD, placeCenter);

        XPooledDataIDList listACDsInRadius(MemoryPoolsGetDATAIDListPool());
        ACDsIntersectSphere(placeCenter, flTargetScanRadius, ACDID_INVALID, listACDsInRadius, 7);


        ActorCommonData & tACD = ACDGet(idACD);
        POS posBuff = tACD.listBuffs.GetTailPosition();
        while (posBuff)
        {
            BuffID idBuff = tACD.listBuffs.GetPrev(posBuff);

            PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuff);
            if (ptBuff && !ptBuff->bDeleted)
            {
                ACDID idACDBuffCharmSource = (ACDID)FastAttribGetValueInt(ptBuff->idFastAttrib, FastAttribKey(ATTRIBUTE_CHARM_SOURCE_ACDID));
                if (idACDBuffCharmSource != ACDID_INVALID)
                {

                    POS pos = listACDsInRadius.GetHeadPosition();
                    while (pos)
                    {
                        ACDID idACDCurrent = (ACDID)listACDsInRadius.GetNext(pos);
                        ActorCommonData & tACDCurrent = ACDGet(idACDCurrent);
                        if (idACDCurrent == idACDBuffCharmSource && tACDCurrent.IsAlive())
                        {
                            return idACDBuffCharmSource;
                        }
                    }
                }
            }
        }
    }

    return ACDID_INVALID;
}



static ACDID sGetClosestPowerTarget(
    ActorID idActor,
    SNO snoPower,
    float flSearchRadius)
{
    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_BIT))
    {
        return ACDID_INVALID;
    }

    ACDID idACDWinner = ACDID_INVALID;
    ACDID idACD = ACDFindByActor(idActor);
    if (idACD != ACDID_INVALID)
    {
        WorldPlace placeCenter;
        float flTargetScanRadius = BrainAPIActorGetAwarenessScanRadius(idACD, placeCenter);

        if (flSearchRadius > 0.0f)
        {
            flTargetScanRadius = flSearchRadius;
        }

        XPooledDataIDList listACDsInRadius(MemoryPoolsGetDATAIDListPool());
        ACDsIntersectSphere(placeCenter, flTargetScanRadius, idACD, listACDsInRadius, 7);

        float flDistWinnerSqr = FLT_MAX;
        POS pos = listACDsInRadius.GetHeadPosition();
        ActorCommonData & tACD = ACDGet(idACD);
        while (pos)
        {
            ACDID idACDMonster = (ACDID)listACDsInRadius.GetNext(pos);
            Target tTarget;
            tTarget.InitializeFromACD(idACDMonster, FALSE, FALSE);
            if (ACDCanAttackTarget(tACD, tTarget, AutoPowerDefinitionR(snoPower), NULL))
            {
                float flDistSqr = ACDGetDistanceSquared(tACD, ACDGet(idACDMonster), FALSE);
                if (flDistSqr < flDistWinnerSqr)
                {
                    flDistWinnerSqr = flDistSqr;
                    idACDWinner = idACDMonster;
                }
            }
        }
    }

    return idACDWinner;
}



static ACDID sGetClosestPlayer(
    ActorCommonData & tACD,
    float flDistToCheck)
{
    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_BIT) ||
        SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_TO_PLAYERS_BIT) ||
        sgtAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_FLYTHROUGH_BIT))
    {
        return ACDID_INVALID;
    }

    ACDID idACDWinner = ACDID_INVALID;

    if (tACD.IsCharmed())
    {
        return idACDWinner;
    }

    WorldPlace placeCenter;
    float flTargetScanRadius = BrainAPIActorGetAwarenessScanRadius(tACD.id, placeCenter);
    // flDistToCheck folds away in the only (inlined) copy; this use is modelled on sGetClosestPowerTarget (line 959).
    if (flDistToCheck > 0.0f)
    {
        flTargetScanRadius = flDistToCheck;
    }

    float flDistWinner = FLT_MAX;
    BOOL bWinnerAlive = FALSE;

    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listPlayers, -1, FALSE, -1);

    POS pos = listPlayers.GetHeadPosition();
    WorldPlace placeACD = tACD.GetPlace();
    while (pos)
    {
        const Player * ptPlayer = listPlayers.GetNext(pos);


        if (ptPlayer->idPlayerACD == ACDID_INVALID || ptPlayer->idPlayerACD == tACD.id)
        {
            continue;
        }

        ActorCommonData * ptACDCurrent = ACDTryToGet(ptPlayer->idPlayerACD);
        if (!ptACDCurrent)
        {
            continue;
        }

        BOOL bCurAlive = ptACDCurrent->IsAlive();
        WorldPlace placeACDCurrent = ptACDCurrent->GetPlace();
        if (placeACD.idSWorld == placeACDCurrent.idSWorld)
        {
            float flDist = WorldPlaceDistance(placeACD, placeACDCurrent);
            if (flDist > flTargetScanRadius)
            {
                continue;
            }


            float flCustomWeight = ptACDCurrent->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_CUSTOM_TARGET_WEIGHT, tACD.snoActor));
            flDist -= flCustomWeight;

            if ((bWinnerAlive == bCurAlive && flDist < flDistWinner) || (!bWinnerAlive && bCurAlive))
            {
                idACDWinner = ptPlayer->idPlayerACD;
                flDistWinner = flDist;
                bWinnerAlive = bCurAlive;
            }
        }
    }

    return idACDWinner;
}



static ACDID sGetFarthestPlayer(
    ActorCommonData & tACD,
    float flDistToCheck)
{
    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_BIT) ||
        SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_TO_PLAYERS_BIT) ||
        sgtAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_FLYTHROUGH_BIT))
    {
        return ACDID_INVALID;
    }

    ACDID idACDWinner = ACDID_INVALID;

    if (tACD.IsCharmed())
    {
        return idACDWinner;
    }

    WorldPlace placeCenter;
    float flTargetScanRadius = BrainAPIActorGetAwarenessScanRadius(tACD.id, placeCenter);
    // flDistToCheck folds away in the only (inlined) copy; this use is modelled on sGetClosestPowerTarget (line 959).
    if (flDistToCheck > 0.0f)
    {
        flTargetScanRadius = flDistToCheck;
    }

    float flDistWinner = FLT_MAX;
    BOOL bWinnerAlive = FALSE;

    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listPlayers, -1, FALSE, -1);

    POS pos = listPlayers.GetHeadPosition();
    WorldPlace placeACD = tACD.GetPlace();
    while (pos)
    {
        const Player * ptPlayer = listPlayers.GetNext(pos);


        if (ptPlayer->idPlayerACD == ACDID_INVALID || ptPlayer->idPlayerACD == tACD.id)
        {
            continue;
        }

        ActorCommonData * ptACDCurrent = ACDTryToGet(ptPlayer->idPlayerACD);
        if (!ptACDCurrent)
        {
            continue;
        }

        BOOL bCurAlive = ptACDCurrent->IsAlive();
        WorldPlace placeACDCurrent = ptACDCurrent->GetPlace();
        if (placeACD.idSWorld == placeACDCurrent.idSWorld)
        {
            float flDist = WorldPlaceDistance(placeACD, placeACDCurrent);
            if (flDist > flTargetScanRadius)
            {
                continue;
            }


            float flCustomWeight = ptACDCurrent->AttributesGetFloat(MakeAttribKey(ATTRIBUTE_CUSTOM_TARGET_WEIGHT, tACD.snoActor));
            flDist -= flCustomWeight;

            if ((bWinnerAlive == bCurAlive && flDist > flDistWinner) || (!bWinnerAlive && bCurAlive))
            {
                idACDWinner = ptPlayer->idPlayerACD;
                flDistWinner = flDist;
                bWinnerAlive = bCurAlive;
            }
        }
    }

    return idACDWinner;
}



static ACDID sGetClosestPlayerInCombatWithPlayer(
    ActorCommonData & tACD,
    float flDistToCheck)
{
    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_BIT) ||
        SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_TO_PLAYERS_BIT) ||
        sgtAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_FLYTHROUGH_BIT))
    {
        return ACDID_INVALID;
    }

    ACDID idACDWinner = ACDID_INVALID;

    if (tACD.IsCharmed())
    {
        return idACDWinner;
    }

    WorldPlace placeCenter;
    float flTargetScanRadius = BrainAPIActorGetAwarenessScanRadius(tACD.id, placeCenter);
    // flDistToCheck folds away in the only (inlined) copy; this use is modelled on sGetClosestPowerTarget (line 959).
    if (flDistToCheck > 0.0f)
    {
        flTargetScanRadius = flDistToCheck;
    }

    float flDistWinner = FLT_MAX;

    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listPlayers, -1, FALSE, -1);

    POS pos = listPlayers.GetHeadPosition();
    WorldPlace placeACD = tACD.GetPlace();
    while (pos)
    {
        const Player * ptPlayer = listPlayers.GetNext(pos);


        if (ptPlayer->idPlayerACD == ACDID_INVALID || ptPlayer->idPlayerACD == tACD.id)
        {
            continue;
        }

        ActorCommonData * ptACDCurrent = ACDTryToGet(ptPlayer->idPlayerACD);
        if (!ptACDCurrent)
        {
            continue;
        }

        WorldPlace placeACDCurrent = ptACDCurrent->GetPlace();
        if (placeACD.idSWorld == placeACDCurrent.idSWorld)
        {
            float flDist = WorldPlaceDistance(placeACD, placeACDCurrent);
            if (flDist > flTargetScanRadius)
            {
                continue;
            }

            if (ptACDCurrent->IsAlive() &&
                ACDIsHostile(*ptACDCurrent, tACD, FALSE) &&
                ptACDCurrent->AttributesGetInt(FastAttribKey(ATTRIBUTE_IN_COMBAT_WITH_PLAYER)))
            {
                if (flDist < flDistWinner)
                {
                    idACDWinner = ptPlayer->idPlayerACD;
                    flDistWinner = flDist;
                }
            }
        }
    }

    return idACDWinner;
}



static ACDID sGetClosestNonPlayerControlledEnemy(
    ActorID idActor,
    float flDistToCheck)
{
    if (SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_BIT))
    {
        return ACDID_INVALID;
    }

    ACDID idACDWinner = ACDID_INVALID;
    ACDID idACD = ACDFindByActor(idActor);
    if (idACD != ACDID_INVALID)
    {
        WorldPlace placeCenter;
        float flTargetScanRadius = BrainAPIActorGetAwarenessScanRadius(idACD, placeCenter);
        // flDistToCheck folds away in the only (inlined) copy; this use is modelled on sGetClosestPowerTarget (line 959).
        if (flDistToCheck > 0.0f)
        {
            flTargetScanRadius = flDistToCheck;
        }

        XPooledDataIDList listACDsInRadius(MemoryPoolsGetDATAIDListPool());
        ACDsIntersectSphere(placeCenter, flTargetScanRadius, ACDID_INVALID, listACDsInRadius, 7);

        float flDistWinner = FLT_MAX;

        POS pos = listACDsInRadius.GetHeadPosition();
        ActorCommonData & tACD = ACDGet(idACD);
        WorldPlace placeACD = tACD.GetPlace();
        while (pos)
        {
            ACDID idACDCurrent = (ACDID)listACDsInRadius.GetNext(pos);
            if (idACDCurrent == idACD)
            {
                continue;
            }

            if (!ACDIsPlayer(idACDCurrent) && !ACDGetControllingPlayer(idACDCurrent))
            {
                ActorCommonData & tACDCurrent = ACDGet(idACDCurrent);
                WorldPlace placeACDCurrent = tACDCurrent.GetPlace();
                if (placeACD.idSWorld == placeACDCurrent.idSWorld)
                {
                    if (tACDCurrent.IsAlive() && ACDIsHostile(tACDCurrent, tACD, TRUE))
                    {
                        float flDist = WorldPlaceDistance(placeACD, placeACDCurrent);

                        if (flDist < flDistWinner)
                        {
                            flDistWinner = flDist;
                            idACDWinner = idACDCurrent;
                        }
                    }
                }
            }
        }
    }

    return idACDWinner;
}



static ACDID sGetClosestPlayerToFollow(
    ActorID idActor,
    float flDistToCheck,
    BOOL bAllowedToWarpWorld)
{
    ACDID idACDWinner = ACDID_INVALID;

    ACDID idACD = ACDFindByActor(idActor);
    if (idACD != ACDID_INVALID)
    {
        ActorCommonData & tACD = ACDGet(idACD);
        WorldPlace placeACD = tACD.GetPlace();

        float flDistWinner = FLT_MAX;
        float flMaxPrefDist = flDistToCheck;
        // Lines 1302-1307 are not recoverable: the inlined copy only shows flMaxPrefDist == 80.0f.





        BOOL bWinnerInSameWorld = FALSE;
        BOOL bWinnerAtPrefDist = FALSE;
        BOOL bWinnerAlive = FALSE;


        for (Player * ptPlayer = PlayerGetFirstInGame();
             ptPlayer;
             ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            ACDID idACDCurrent = ptPlayer->idPlayerACD;
            ActorCommonData & tACDCurrent = ACDGet(idACDCurrent);

            WorldPlace placeACDCurrent = tACDCurrent.GetPlace();
            BOOL bCurInSameWorld = (placeACD.idSWorld == placeACDCurrent.idSWorld);


            if (!bCurInSameWorld)
            {
                if (!bAllowedToWarpWorld || !EscortCanBePulledThroughWarp(tACD, tACDCurrent))
                {
                    continue;
                }
            }

            float flDist = bCurInSameWorld ? WorldPlaceDistance(placeACD, placeACDCurrent) : 0.0f;
            BOOL bCurAtPrefDist = bCurInSameWorld && flDist < flMaxPrefDist;

            BOOL bCurAlive = tACDCurrent.IsAlive();








            if ((!bWinnerInSameWorld && bCurInSameWorld) ||
                (bWinnerInSameWorld == bCurInSameWorld &&
                 ((!bWinnerAtPrefDist && bCurAtPrefDist) ||
                  (bWinnerAtPrefDist == bCurAtPrefDist &&
                   ((!bWinnerAlive && bCurAlive) ||
                    (bWinnerAlive == bCurAlive && flDist < flDistWinner))))))
            {
                idACDWinner = idACDCurrent;
                flDistWinner = flDist;
                bWinnerInSameWorld = bCurInSameWorld;
                bWinnerAtPrefDist = bCurAtPrefDist;
                bWinnerAlive = bCurAlive;
            }
        }
    }

    return idACDWinner;
}





static ACDID sGetFollowTarget(
    ActorID idActor)
{
    ACDID idACDFollowTarget = ACDID_INVALID;

    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD)
    {
        WorldPlace placeACD = ptACD->GetPlace();
        idACDFollowTarget = (ACDID)ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_FOLLOW_TARGET_ACDID));
        AIFollowType eFollowType = (AIFollowType)ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_FOLLOW_TARGET_TYPE));

        BOOL bValidTarget = FALSE;
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDFollowTarget);
        if (ptACDTarget)
        {
            if (ptACDTarget->IsAlive())
            {
                if (eFollowType == FOLLOW_TYPE_SWITCH_IF_OUT_OF_RANGE) {

                    WorldPlace placeTarget = ptACDTarget->GetPlace();
                    if (placeACD.idSWorld == placeTarget.idSWorld)
                    {
                        bValidTarget = TRUE;
                    }
                }
                else if (eFollowType == FOLLOW_TYPE_SWITCH_IF_INVALID_ONLY)
                {
                    bValidTarget = TRUE;
                }
            }
        }





        if (!bValidTarget)
        {
            PetType ePetType = (PetType)ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_PET_TYPE));
            BOOL bAllowedToWarpWorld = (ePetType == PETTYPE_FOLLOWER);

            idACDFollowTarget = sGetClosestPlayerToFollow(idActor, PLAYER_FOLLOW_TARGET_MAX_DISTANCE, bAllowedToWarpWorld);

            if (ePetType == PETTYPE_FOLLOWER)
            {


                Player * ptPlayerOwner = ptACD->GetPetOwnerPlayer();
                if (ptPlayerOwner && ptPlayerOwner->idPlayerACD != idACDFollowTarget)
                {

                    PlayerRemovePet(*ptPlayerOwner, idACD, TRUE);
                }

                if (idACDFollowTarget != ACDID_INVALID)
                {


                    ActorCommonData & tACDFollowTarget = ACDGet(idACDFollowTarget);
                    WorldPlace placeTarget = tACDFollowTarget.GetPlace();
                    if (placeACD.idSWorld != placeTarget.idSWorld)
                    {
                        ACDWarpToACD(*ptACD, tACDFollowTarget, WARP_UNKNOWN, 40.0f);
                    }

                    if (!ptPlayerOwner || ptPlayerOwner->idPlayerACD != idACDFollowTarget)
                    {

                        SPlayerAddPet(idACDFollowTarget, PETTYPE_FOLLOWER, idACD, TRUE, -1, NULL);
                    }
                }
            }

            ptACD->AttributesSetInt(FastAttribKey(ATTRIBUTE_FOLLOW_TARGET_ACDID), idACDFollowTarget);
        }
    }

    return idACDFollowTarget;
}
void ACDGetNavMeshHandleWithPlace(const ActorCommonData & tACD, NavMeshHandle & tNavMeshHandle, WorldPlace & tPlace); BOOL NavMeshIsPointOnHandle(const NavMeshHandle & tNavMeshHandle, const Vector3D & wp); void NavMeshRollDataInitialize(NavMeshRollData & tRollData); BOOL NavMeshRollMarble(NavMeshRollData & tRollData); BOOL NavMeshCallBackPassabilityTypeCanEnter(void * pCallbackData, const NavMeshHandle & tNavMeshHandle); void NavMeshGetCenterWorldSpace(const NavMeshHandle & tNavMeshHandle, Vector3D & wpCenter);  // ActorCommonData.cpp:4668, NavigationMesh.cpp:676, :1937, :2000, :3383, :958
WorldPlace AIStateGetRandomSpotBehind(
    ACDID idACD,
    ActorCommonData * ptACDTarget,
    float flDistMin,
    float flDistMax)
{
    WorldPlace wpCenter;

    if (!ptACDTarget)
    {
        return cgplaceNull;
    }

    NavMeshHandle tNavMeshHandleCenter;

    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
    {
        return ptACDTarget->GetPlace();
    }

    wpCenter = ptACD->GetPlace();

    ACDGetNavMeshHandleWithPlace(*ptACD, tNavMeshHandleCenter, wpCenter);

    if (!NavMeshIsPointOnHandle(tNavMeshHandleCenter, wpCenter.wp))
    {
        return wpCenter;
    }

    Vector3D vDirection;
    Vector3D vStart = wpCenter.wp;
    Vector3D vTarget = ptACDTarget->GetPlace().wp;

    vDirection = vTarget - vStart;
    if (vDirection.MagnitudeSquared() > EPSILON)
    {
        vDirection.Normalize();
    }
    else
    {
        vDirection = -cgwvXAxis;
        Vector3D::Rotate(cgwvZAxis, ptACD->GetOrientation().ToYaw(), vDirection);
    }

    vDirection *= RandRangeFloat_Internal(SGameGlobalsGet().rng, flDistMin, flDistMax);
    vTarget += vDirection;

    NavMeshRollData tRollData;
    NavMeshRollDataInitialize(tRollData);
    tRollData.pCallbackFunction = NavMeshCallBackPassabilityTypeCanEnter;
    tRollData.pCallbackData = (void *)1;
    tRollData.tNavMeshHandleStartCenter = tNavMeshHandleCenter;
    tRollData.wpStart = wpCenter.wp;
    tRollData.wpDest = vTarget;
    tRollData.dwMarbleRollFlags = 1;
    tRollData.tPassability.snoActor = ptACD->snoActor;

    WorldPlace placeDest = wpCenter;
    NavMeshRollMarble(tRollData);

    NavMeshGetCenterWorldSpace(tRollData.tNavMeshHandleEndCenter, placeDest.wp);

    return placeDest;
}
BOOL NavCellHandleFromWorldPlace(const WorldPlace & tPlace, NavCellHandle & hNavCell);  // X/Common/Nav/NavZone.cpp:730
BOOL WorldCollSweptCircle(SWorldID idSWorld, const NavCellHandle & hNavCellStart, const Circle & wcStart, const Vector2D & wpDest, const WorldCollParams & tWorldCollParams, uint32 uCollTestFlags, uint32 uACDCollNarrowFlags, ACDID idACDIgnore, WorldCollResult & tResult);  // X/Common/WorldCommonCollision.cpp:147
BOOL ScenesCommonNavMeshNodeFromPlace(const WorldPlace & tPlace, NavMeshHandle * ptNavMeshHandle, const BOOL fCanActivateScenes, const BOOL fProcessInactive); BOOL NavMeshGetHeightAndNormal(const NavMeshHandle & tNavMeshHandle, const Vector3D & wp, const BOOL fPinToSquareBounds, WorldDistance & flHeight, Vector3D & wvNormal);  // X/Common/ScenesCommon.cpp, X/Common/Nav/NavigationMesh.cpp
static const int32 TARGET_RANDOM_POINT_NUM_TRIES = 3;
static const float TARGET_RANDOM_POINT_ANGLE_INC = FULL_CIRCLE / TARGET_RANDOM_POINT_NUM_TRIES;
WorldPlace AIStateGetRandomLOSPlace(ACDID idACD, float flDistMin, float flDistMax)
{
    ActorCommonData & tACD = ACDGet(idACD);
    WorldPlace placeStart = tACD.GetPlace();

    NavCellHandle tNavCellHandleStart;
    if (!NavCellHandleFromWorldPlace(placeStart, tNavCellHandleStart))
    {
        return placeStart;
    }

    Circle circleStart;
    circleStart.Set(placeStart.wp.As2D(), tACD.GetRadius());

    const ACDCollSettings & tACDCollSettings = tACD.GetCollSettings();
    WorldCollParams tCollParams(tACDCollSettings);
    tCollParams.uObjectNoOverlapFlags &= ~0xb;

    Angle angYaw = RandRangeFloat_Internal(SGameGlobalsGet().rng, 0.0f, FULL_CIRCLE);
    Vector2D vPosWinner = circleStart.wpCenter;
    float flWinningDistSqr = 0.0f;
    float flDistMinSqr = flDistMin * flDistMin;
    for (int32 nTry = 0; nTry < TARGET_RANDOM_POINT_NUM_TRIES; nTry++)
    {
        Vector2D vTarget(Math::Cos(angYaw), Math::Sin(angYaw));
        vTarget *= RandRangeFloat_Internal(SGameGlobalsGet().rng, flDistMin, flDistMax);
        vTarget += circleStart.wpCenter;

        WorldCollResult tCollResult;
        if (!WorldCollSweptCircle(placeStart.idSWorld, tNavCellHandleStart, circleStart, vTarget, tCollParams,
                                  3,
                                  1,
                                  tACD.id, tCollResult))
        {
            vPosWinner = tCollResult.wpEnd.As2D();
            break;
        }
        else
        {

            float flDistSqr = Vector2D::DistanceSquared(circleStart.wpCenter, tCollResult.wpContact.As2D());
            if (flDistSqr >= flDistMinSqr)
            {

                vPosWinner = tCollResult.wpEnd.As2D();
                break;
            }
            if (flDistSqr > flWinningDistSqr)
            {
                flWinningDistSqr = flDistSqr;

                vPosWinner = tCollResult.wpEnd.As2D();
            }
        }


        angYaw += TARGET_RANDOM_POINT_ANGLE_INC;
    }

    placeStart.wp.Set(vPosWinner.x, vPosWinner.y, 0.0f);

    NavMeshHandle tNavMeshHandle;
    Vector3D vNormal;
    ScenesCommonNavMeshNodeFromPlace(placeStart, &tNavMeshHandle, FALSE, FALSE);
    NavMeshGetHeightAndNormal(tNavMeshHandle, placeStart.wp, FALSE, placeStart.wp.z, vNormal);

    return placeStart;
}



WorldPlace sGetPlaceCenterOfPack(ACDID idACD)
{
    ActorCommonData & tACD = ACDGet(idACD);
    WorldPlace placeCenter = tACD.GetPlace();

    ACDPack * ptACDPack = ACDPackTryToGet(tACD.idACDPack);
    if (ptACDPack)
    {



        int32 nPackMembersCount = 1;
        const ACDIDListPooled * plistACDPack = ptACDPack->GetMemberList();
        POS pos = plistACDPack->GetHeadPosition();
        while (pos)
        {
            ACDID idACDPackMember = (ACDID)plistACDPack->GetNext(pos);
            ActorCommonData * ptACDPackMember = ACDTryToGet(idACDPackMember);
            if (idACDPackMember != idACD && ptACDPackMember && ptACDPackMember->IsAlive())
            {
                WorldPlace placePackMember = ptACDPackMember->GetPlace();
                if (placePackMember.idSWorld == placeCenter.idSWorld)
                {
                    nPackMembersCount++;
                    placeCenter.wp = placeCenter.wp * ((float)(nPackMembersCount - 1) / (float)nPackMembersCount) +
                                     placePackMember.wp * (1.0f / (float)nPackMembersCount);
                }
            }
        }
    }
    return placeCenter;
}



BOOL sBrainAPITargetFilterInCombat(ACDID, ACDID idACDTarget)
{
    ActorCommonData & tAcdTarget = ACDGet(idACDTarget);
    return tAcdTarget.IsInCombat();
}



BOOL sBrainAPITargetFilterSameTeam(ACDID idACDSource, ACDID idACDTarget)
{
    ActorCommonData & tACDSource = ACDGet(idACDSource);
    ActorCommonData & tACDTarget = ACDGet(idACDTarget);
    return tACDSource.GetTeam() == tACDTarget.GetTeam();
}








// NON_MATCHING: 2.7.7 logic (cases 46-48) reproduced; 18804 vs 18824 bytes, register allocation and block order differ
const Target & CTargetCache::GetTarget(
    AIStateTargetType eTargetType,
    EntryData & tEntryData)
{


    SNO snoPower = tEntryData.ptEntry->snoPower;

    if (eTargetType < 0 || eTargetType >= AISTATE_TARGET_COUNT)
    {
        return cgTargetNone;
    }

    if (m_dwCachedFlags & ((uint64)1 << eTargetType))
    {
        if (eTargetType != AISTATE_TARGET_CLOSEST_POWER_TARGET && eTargetType != AISTATE_TARGET_CUSTOM_POWER_TARGET)
        {

            return m_Target[eTargetType];
        }




        Target & tTarget = m_Target[eTargetType];
        if (mapPowerTargets.Lookup(snoPower, tTarget))
        {
            return tTarget;
        }
    }


    if (eTargetType == AISTATE_TARGET_SELF)
    {
        return m_Target[AISTATE_TARGET_SELF];
    }

    ActorID idActor = ActorID(ACDGetOwner(m_idACD));
    ActorCommonData & tACD = ACDGet(m_idACD);
    // The binary tests two bit masks (no update / needs enemies); which group was listed explicitly and the label order
    // inside each group are not recoverable. Lines 1689-1747 are laid out to put the two calls on their lines.
    switch (eTargetType)
    {
    case AISTATE_TARGET_PRIMARY_ENEMY:
    case AISTATE_TARGET_OWNER_CLOSEST_ENEMY:
    case AISTATE_TARGET_CLOSEST_ENEMY_IN_COMBAT:
    case AISTATE_TARGET_RANDOM_BEHIND_ENEMY:
    case AISTATE_TARGET_CLOSEST_ENEMY:
    case AISTATE_TARGET_SWITCH_ON_ATTACK:
    case AISTATE_TARGET_PRIMARY_ACTIVE_ENEMY:
    case AISTATE_TARGET_FRONT_OF_PRIMARY_ENEMY:
        if (!ACDCanHaveEnemies(tACD.GetTeam()))
        {
            return cgTargetNone;
        }


    default:





        BrainAPIUpdateTargets(idActor, FALSE);
        break;

    case AISTATE_TARGET_CLOSEST_PLAYER:
    case AISTATE_TARGET_RANDOM_NEARBY_GROUND:
    case AISTATE_TARGET_PLAYER_TO_FOLLOW:
    case AISTATE_TARGET_CLOSEST_CORPSE:
    case AISTATE_TARGET_CLOSEST_UNDEAD:
    case AISTATE_TARGET_CLOSEST_POWER_TARGET:
    case AISTATE_TARGET_GUARD_OBJECT:
    case AISTATE_TARGET_OWNER:
    case AISTATE_TARGET_TALK_TARGET:
    case AISTATE_TARGET_BROADCASTER:
    case AISTATE_TARGET_RANDOM_FAR_GROUND:
    case AISTATE_TARGET_CLOSEST_GOLD:
    case AISTATE_TARGET_CLOSEST_PLUS_ALLY:
    case AISTATE_TARGET_HIGHEST_THREAT:
    case AISTATE_TARGET_CLOSEST_WRECKABLE:
    case AISTATE_TARGET_WRECKABLE_BLOCKER:
    case AISTATE_TARGET_TETHER_POINT:
    case AISTATE_TARGET_TAUNT_TARGET:
    case AISTATE_TARGET_CHARM_SOURCE:
    case AISTATE_TARGET_CLOSEST_CORPSE_OF_SAME_FAMILY:
    case AISTATE_TARGET_CLOSEST_CORPSE_OF_SAME_FAMILY_NON_ELITE:
    case AISTATE_TARGET_CLOSEST_IN_PACK:
    case AISTATE_TARGET_CENTER_OF_PACK_GROUND:
    case AISTATE_TARGET_CLOSEST_NEW_LEGENDARY:
    case AISTATE_TARGET_CLOSEST_RESPLENDENT_CHEST:
    case AISTATE_TARGET_CLOSEST_ELITE:
    case AISTATE_TARGET_CLOSEST_PLAYER_IN_COMBAT_WITH_PLAYER:
    case AISTATE_TARGET_CLOSEST_NON_PLAYER_CONTROLLED_ENEMY:
    case AISTATE_TARGET_FRONT_OF_SELF:
    case AISTATE_TARGET_CLOSEST_TRASH_ITEM:
    case AISTATE_TARGET_CLOSEST_PET:
    case AISTATE_TARGET_FARTHEST_PLAYER:
    case AISTATE_TARGET_CLOSEST_MAGIC_ITEM:
    case AISTATE_TARGET_CLOSEST_RARE_ITEM:
    case AISTATE_TARGET_CLOSEST_DEATHS_BREATH:
        break;
    }

    m_Target[eTargetType].eType = TT_NONE;
    switch (eTargetType)
    {
    case AISTATE_TARGET_PRIMARY_ENEMY:
    case AISTATE_TARGET_PRIMARY_ACTIVE_ENEMY:
    case AISTATE_TARGET_CLOSEST_ENEMY:
    {
        ACDID idACDForcedEnemy = (ACDID)tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_FORCED_ENEMY_ACDID));
        ActorCommonData * ptACDForcedEnemy = ACDTryToGet(idACDForcedEnemy);
        if (ptACDForcedEnemy && ptACDForcedEnemy->IsAlive() && ptACDForcedEnemy->IsTargetable())
        {
            m_Target[eTargetType].idACDTarget = idACDForcedEnemy;
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        else
        {
            switch (eTargetType) {
            case AISTATE_TARGET_PRIMARY_ENEMY:
                m_Target[eTargetType].idACDTarget = BrainAPIChoosePrimaryTarget(tACD, FALSE);
                break;

            case AISTATE_TARGET_PRIMARY_ACTIVE_ENEMY:
                m_Target[eTargetType].idACDTarget = BrainAPIChoosePrimaryTarget(tACD, TRUE);
                break;

            default:
                m_Target[eTargetType].idACDTarget = BrainAPIActorGetClosestTarget(idActor, SNO_NONE, FALSE, NULL);
            }
            if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
            {
                m_Target[eTargetType].eType = TT_ACTOR;
            }
        }
    }
    break;
    case AISTATE_TARGET_CLOSEST_FRIEND:

        m_Target[eTargetType].idACDTarget = BrainAPIActorGetClosestTarget(idActor, SNO_NONE, TRUE, NULL);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        break;


    case AISTATE_TARGET_CLOSEST_WRECKABLE:
        m_Target[eTargetType].idACDTarget = sGetClosestWreckableTarget(idActor);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        break;


    case AISTATE_TARGET_CLOSEST_FRIENDLY_LEADER:
        m_Target[eTargetType].idACDTarget = BrainAPIActorGetClosestFriendlyLeaderTarget(idActor);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        break;

    case AISTATE_TARGET_CLOSEST_CORPSE:
        m_Target[eTargetType].idACDTarget = sGetClosestCorpseTarget(idActor);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        break;

    case AISTATE_TARGET_CLOSEST_CORPSE_OF_SAME_FAMILY:
        m_Target[eTargetType].idACDTarget = sGetClosestCorpseOfSameFamilyTarget(idActor, FALSE);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        break;

    case AISTATE_TARGET_CLOSEST_CORPSE_OF_SAME_FAMILY_NON_ELITE:
        m_Target[eTargetType].idACDTarget = sGetClosestCorpseOfSameFamilyTarget(idActor, TRUE);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        break;

    case AISTATE_TARGET_CLOSEST_IN_PACK:
        m_Target[eTargetType].idACDTarget = sGetClosestInPack(idActor);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        break;

    case AISTATE_TARGET_CLOSEST_UNDEAD:
        m_Target[eTargetType].idACDTarget = sGetClosestUndeadTarget(idActor);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        break;

    case AISTATE_TARGET_CLOSEST_POWER_TARGET:
    {
        Target tTarget;
        tTarget.Clear();
        tTarget.idACDTarget = sGetClosestPowerTarget(idActor, snoPower, 0.0f);
        if (tTarget.idACDTarget != ACDID_INVALID)
        {
            tTarget.eType = TT_ACTOR;
        }
        mapPowerTargets.SetAt_Tracked(snoPower, tTarget);
    }
    break;
    case AISTATE_TARGET_CUSTOM_POWER_TARGET:
    {
        Target tTarget;
        TargetEvaluateCustom(tTarget, idActor, snoPower);
        mapPowerTargets.SetAt_Tracked(snoPower, tTarget);
    }
    break;
    case AISTATE_TARGET_OWNER_CLOSEST_ENEMY:
    {
        ACDID idACDOwner = ACDGetPetOwner(m_idACD);
        ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);

        if (ptACDOwner &&
            ptACDOwner->IsAlive())
        {
            ActorID idActorOwner = ActorID(ACDGetOwner(idACDOwner));

            if (ACDIsPlayer(idACDOwner))
            {
                m_Target[eTargetType].idACDTarget = sGetClosestPowerTarget(idActorOwner, snoPower, TARGET_FAR_MAX_DISTANCE);
            }
            else
            {
                m_Target[eTargetType].idACDTarget = BrainAPIActorGetClosestTarget(idActorOwner, SNO_NONE, FALSE, NULL);
            }
        }
        else
        {





            m_Target[eTargetType].idACDTarget = BrainAPIActorGetClosestTarget(idActor, SNO_NONE, FALSE, NULL);
        }

        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
    }
    break;
    case AISTATE_TARGET_CLOSEST_PLAYER:
        m_Target[eTargetType].idACDTarget = sGetClosestPlayer(tACD, 0.0f);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        break;

    case AISTATE_TARGET_CLOSEST_PLAYER_IN_COMBAT_WITH_PLAYER:
        m_Target[eTargetType].idACDTarget = sGetClosestPlayerInCombatWithPlayer(tACD, 0.0f);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        break;

    case AISTATE_TARGET_CLOSEST_NON_PLAYER_CONTROLLED_ENEMY:
        m_Target[eTargetType].idACDTarget = sGetClosestNonPlayerControlledEnemy(idActor, 0.0f);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        break;


    case AISTATE_TARGET_RANDOM_BEHIND_ENEMY:
    {
        m_Target[eTargetType].eType = TT_POINT;
        ACDID idACDEnemy;
        ACDID idACDForcedEnemy = (ACDID)tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_FORCED_ENEMY_ACDID));
        ActorCommonData * ptACDForcedEnemy = ACDTryToGet(idACDForcedEnemy);
        if (ptACDForcedEnemy && ptACDForcedEnemy->IsAlive() && ptACDForcedEnemy->IsTargetable())
        {
            idACDEnemy = idACDForcedEnemy;
        }
        else
        {
            idACDEnemy = BrainAPIActorGetClosestTarget(idActor, SNO_NONE, FALSE, NULL);
        }

        ActorCommonData * ptACDEnemy = ACDTryToGet(idACDEnemy);
        if (!ptACDEnemy)
        {
            m_Target[eTargetType].tPlace = AIStateGetRandomLOSPlace(m_idACD, 10.0f, 15.0f);
        }
        else
        {
            m_Target[eTargetType].tPlace = AIStateGetRandomSpotBehind(m_idACD, ptACDEnemy, 10.0f, 15.0f);
        }
    }
    break;

    case AISTATE_TARGET_RANDOM_NEARBY_GROUND:
        m_Target[eTargetType].eType = TT_POINT;
        m_Target[eTargetType].tPlace = AIStateGetRandomLOSPlace(m_idACD, TARGET_NEARBY_MIN_DISTANCE, TARGET_NEARBY_MAX_DISTANCE);
        break;

    case AISTATE_TARGET_RANDOM_FAR_GROUND:
        m_Target[eTargetType].eType = TT_POINT;
        m_Target[eTargetType].tPlace = AIStateGetRandomLOSPlace(m_idACD, TARGET_FAR_MIN_DISTANCE, TARGET_FAR_MAX_DISTANCE);
        break;



    case AISTATE_TARGET_PLAYER_TO_FOLLOW:
    {
        Player * ptOwner = tACD.GetPetOwnerPlayer();
        PetType ePetType = (PetType)tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_PET_TYPE));
        if (ptOwner && ePetType != PETTYPE_FOLLOWER)
        {
            m_Target[eTargetType].idACDTarget = ptOwner->idPlayerACD;
        }
        else
        {
            m_Target[eTargetType].idACDTarget = sGetFollowTarget(idActor);
        }

        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
    }
    break;

    case AISTATE_TARGET_GUARD_OBJECT:
    {
        ACDID idACDGuardObject = (ACDID)tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_GUARD_OBJECT_ACDID));
        if (ACDTryToGet(idACDGuardObject))
        {
            m_Target[eTargetType].idACDTarget = idACDGuardObject;
            m_Target[eTargetType].eType = TT_ACTOR;
        }
    }
    break;


    case AISTATE_TARGET_OWNER:
        m_Target[eTargetType].idACDTarget = ACDGetPetOwner(m_idACD);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        break;


    case AISTATE_TARGET_TALK_TARGET:
    {
        ACDID idACDTarget = ACDID_INVALID;
        ACDNetworkName idANNTarget = (ACDNetworkName)tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_NPC_TALK_TARGET_ANN));
        if (idANNTarget != -1)
        {
            idACDTarget = ACDFromANN(idANNTarget);
        }
        else
        {
            ACDNetworkName _idANNTarget = (ACDNetworkName)tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_NPC_CONV_TARGET_ANN));
            if (_idANNTarget != -1)
            {
                idACDTarget = ACDFromANN(_idANNTarget);
            }
        }

        m_Target[eTargetType].idACDTarget = idACDTarget;
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
    }
    break;


    case AISTATE_TARGET_BROADCASTER:
        if (ACDTryToGet(tEntryData.idACDBroadcastSource))
        {
            m_Target[eTargetType].InitializeFromACD(tEntryData.idACDBroadcastSource, FALSE, FALSE);
        }
        else
        {
            m_Target[eTargetType].Clear();
        }
        break;


    case AISTATE_TARGET_CLOSEST_ENEMY_IN_COMBAT:
    {
        ACDID idACDOwner = ACDGetPetOwner(m_idACD);
        ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);

        if (ptACDOwner &&
            ptACDOwner->IsAlive())
        {
            ActorID idActorOwner = ActorID(ACDGetOwner(idACDOwner));

            m_Target[eTargetType].idACDTarget = BrainAPIActorGetClosestTarget(idActorOwner, SNO_NONE, FALSE, sBrainAPITargetFilterInCombat);
            if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
            {
                m_Target[eTargetType].eType = TT_ACTOR;
            }
        }
        else
        {
            m_Target[eTargetType].idACDTarget = BrainAPIActorGetClosestTarget(idActor, SNO_NONE, FALSE, sBrainAPITargetFilterInCombat);
            if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
            {
                m_Target[eTargetType].eType = TT_ACTOR;
            }
        }
    }
    break;
    case AISTATE_TARGET_CLOSEST_GOLD:
        m_Target[eTargetType].idACDTarget = sGetClosestGoldTarget(idActor);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        break;
    case AISTATE_TARGET_CLOSEST_NEW_LEGENDARY:
        m_Target[eTargetType].idACDTarget = sGetClosestNewLegendaryTarget(idActor);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        break;
    case AISTATE_TARGET_CLOSEST_TRASH_ITEM:
        m_Target[eTargetType].idACDTarget = sGetClosestTrashItemTarget(idActor, IQL_INFERIOR, IQL_SUPERIOR);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        break;
    case AISTATE_TARGET_CLOSEST_MAGIC_ITEM:
        m_Target[eTargetType].idACDTarget = sGetClosestTrashItemTarget(idActor, IQL_MAGIC_AFFIX_START, IQL_MAGIC_AFFIX_END);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        break;
    case AISTATE_TARGET_CLOSEST_RARE_ITEM:
        m_Target[eTargetType].idACDTarget = sGetClosestTrashItemTarget(idActor, IQL_RARE_AFFIX_START, IQL_RARE_AFFIX_END);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        break;
    case AISTATE_TARGET_CLOSEST_DEATHS_BREATH:
    {
        GBID gbidDeathsBreath = GlobalGBIDGet(GLOBALGBID_ITEM_DEATHS_BREATH);
        ACDID idACDWinner = ACDID_INVALID;
        if (!(SGameGlobalsGet().dwFlags & (1 << SGAMEGLOBALS_AI_BLIND_BIT)))
        {
            ACDID idACDSelf = ACDFindByActor(idActor);
            if (idACDSelf != ACDID_INVALID)
            {
                WorldPlace placeCenter;
                float flTargetScanRadius = BrainAPIActorGetAwarenessScanRadius(idACDSelf, placeCenter);
                XPooledDataIDList listACDsInRadius(MemoryPoolsGetDATAIDListPool());
                ACDsIntersectSphere(placeCenter, flTargetScanRadius, idACDSelf, listACDsInRadius, 7);

                float flDistWinnerSqr = FLT_MAX;
                ActorCommonData & tACDSelf = ACDGet(idACDSelf);
                Player * ptControllingPlayer = ACDGetControllingPlayer(idACDSelf);
                ActorCommonData * ptACDPlayer = ptControllingPlayer ? ACDTryToGet(ptControllingPlayer->idPlayerACD) : NULL;

                POS pos = listACDsInRadius.GetHeadPosition();
                while (pos)
                {
                    ACDID idACDItem = (ACDID)listACDsInRadius.GetNext(pos);
                    ActorCommonData & tACDItem = ACDGet(idACDItem);
                    if (tACDItem.IsItem() && tACDItem.hGB.gbid == gbidDeathsBreath)
                    {
                        if (ptACDPlayer && (tACDItem.GetAssignedHeroID() == OnlineService::INVALID_HERO_ID || !ActorInventoryAllowedToPickupItem(*ptACDPlayer, tACDItem)))
                        {
                            continue;
                        }

                        float flDistSqr = ACDGetDistanceSquared(tACDSelf, tACDItem, FALSE);
                        if (flDistSqr < flDistWinnerSqr)
                        {
                            flDistWinnerSqr = flDistSqr;
                            idACDWinner = idACDItem;
                        }
                    }
                }
            }
        }
        m_Target[eTargetType].idACDTarget = idACDWinner;
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
    }
    break;
    case AISTATE_TARGET_CLOSEST_RESPLENDENT_CHEST:
        m_Target[eTargetType].idACDTarget = sGetClosestResplendentChestTarget(idActor);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        break;
    case AISTATE_TARGET_CLOSEST_ELITE:
        m_Target[eTargetType].idACDTarget = sGetClosestEliteTarget(idActor);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        break;
    case AISTATE_TARGET_CLOSEST_PLUS_ALLY:
        m_Target[eTargetType].idACDTarget = BrainAPIActorGetClosestPlusAllyTarget(idActor);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        break;
    case AISTATE_TARGET_HIGHEST_THREAT:
        m_Target[eTargetType].idACDTarget = BrainAPIActorGetHighestThreatTarget(idActor, FALSE);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        break;

    case AISTATE_TARGET_SWITCH_ON_ATTACK:
        m_Target[eTargetType].idACDTarget = BrainAPIActorGetSwitchOnAttackTarget(idActor);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        break;
    case AISTATE_TARGET_CLOSEST_SAME_TEAM:
        m_Target[eTargetType].idACDTarget = BrainAPIActorGetClosestTarget(idActor, SNO_NONE, TRUE, sBrainAPITargetFilterSameTeam);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        break;

    case AISTATE_TARGET_WRECKABLE_BLOCKER:
    {
        uint32 uNow = GameGetServerTime();
        uint32 uBlockedTime = (uint32)tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_LAST_BLOCKED_TIME));
        if (uNow >= uBlockedTime)
        {
            uint32 uTicksSinceBlock = uNow - uBlockedTime;
            if (uTicksSinceBlock <= VALID_BLOCK_TICKS)
            {
                ACDID idACDBlocker = (ACDID)tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_LAST_BLOCKED_ACDID));
                ActorCommonData * ptACDBlocker = ACDTryToGet(idACDBlocker);


                if (ptACDBlocker && ptACDBlocker->IsWreckableObject())
                {
                    m_Target[eTargetType].idACDTarget = idACDBlocker;
                    m_Target[eTargetType].eType = TT_ACTOR;
                }
            }
        }
    }
    break;


    case AISTATE_TARGET_TETHER_POINT:
    {
        ActorAI & tAI = ActorGetAI(idActor);
        m_Target[eTargetType].tPlace = tAI.tAITetherData.placeTetherPoint;
        m_Target[eTargetType].eType = TT_POINT;
    }
    break;


    case AISTATE_TARGET_TAUNT_TARGET:
        m_Target[eTargetType].idACDTarget = sGetTauntTarget(idActor);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
            m_Target[eTargetType].bForcedActorTarget = TRUE;
        }
        break;



    case AISTATE_TARGET_CHARM_SOURCE:
        m_Target[eTargetType].idACDTarget = sGetCharmSource(idActor);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        break;


    case AISTATE_TARGET_CENTER_OF_PACK_GROUND:
        m_Target[eTargetType].eType = TT_POINT;
        m_Target[eTargetType].tPlace = sGetPlaceCenterOfPack(m_idACD);
        break;

    case AISTATE_TARGET_FRONT_OF_PRIMARY_ENEMY:
    {
        ACDID idACDEnemy = (ACDID)tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_FORCED_ENEMY_ACDID));
        ActorCommonData * ptACDEnemy = ACDTryToGet(idACDEnemy);
        if (!ptACDEnemy || !ptACDEnemy->IsAlive() || !ptACDEnemy->IsTargetable())
        {
            idACDEnemy = BrainAPIChoosePrimaryTarget(tACD, FALSE);
            ptACDEnemy = ACDTryToGet(idACDEnemy);
        }

        m_Target[eTargetType].eType = TT_POINT;
        if (ptACDEnemy)
        {
            m_Target[eTargetType].tPlace = AIStateGetRandomSpotBehind(m_idACD, ptACDEnemy, -5.0f, -5.0f);
        }
        else
        {
            m_Target[eTargetType].tPlace = tACD.GetPlace();
        }
    }
    break;


    case AISTATE_TARGET_FRONT_OF_SELF:
        m_Target[eTargetType].eType = TT_POINT;
        m_Target[eTargetType].tPlace = AIStateGetRandomSpotBehind(m_idACD, &tACD, -12.0f, -12.0f);
        break;


    case AISTATE_TARGET_FRONT_OF_CUSTOM_POWER_TARGET:
    {
        Target tTarget;
        if (!mapPowerTargets.Lookup(snoPower, tTarget))
        {
            TargetEvaluateCustom(tTarget, idActor, snoPower);
        }

        ActorCommonData * ptACDTarget = ACDTryToGet(tTarget.GetACD());
        if (!ptACDTarget)
        {
            tTarget.Clear();
        }
        else
        {
            m_Target[eTargetType].eType = TT_POINT;
            m_Target[eTargetType].tPlace = AIStateGetRandomSpotBehind(m_idACD, ptACDTarget, -5.0f, -5.0f);
        }
    }
    break;


    case AISTATE_TARGET_CLOSEST_PET:
        m_Target[eTargetType].idACDTarget = sGetClosestPet(idActor);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        break;



    case AISTATE_TARGET_FARTHEST_PLAYER:
        m_Target[eTargetType].idACDTarget = sGetFarthestPlayer(tACD, 0.0f);
        if (m_Target[eTargetType].idACDTarget != ACDID_INVALID)
        {
            m_Target[eTargetType].eType = TT_ACTOR;
        }
        break;

    default:
#line 2381
        XASSERT(FALSE);
        break;
    }

    if (eTargetType != AISTATE_TARGET_BROADCASTER)
    {
        m_dwCachedFlags |= ((uint64)1 << eTargetType);
    }

    if (eTargetType == AISTATE_TARGET_CUSTOM_POWER_TARGET || eTargetType == AISTATE_TARGET_CLOSEST_POWER_TARGET)
    {





        Target & tTarget = m_Target[eTargetType];
        mapPowerTargets.Lookup(snoPower, tTarget);
    }
    return m_Target[eTargetType];
}



void sGetConditionRange(ACDID idACD, const AICondition & tCondition, const Target & tTarget, float & flRangeMin, float & flRangeMax)
{
    flRangeMax = tCondition.flConditionParm[1];
    if (idACD != ACDID_INVALID && flRangeMax < 1e-06f)
    {
        ActorCommonData * ptACD = ACDTryToGet(idACD);
        if (ptACD != NULL)
        {
            WorldPlace placeCenter;
            flRangeMax = BrainAPIActorGetAwarenessScanRadius(idACD, placeCenter);
        }
    }
    flRangeMin = Math::Pin(tCondition.flConditionParm[0], 0.0f, flRangeMax);
    if (tTarget.IsTargetingACD())
    {
        ActorCommonData * ptACDTarget = ACDTryToGet(tTarget.idACDTarget);
        if (ptACDTarget != NULL)
            flRangeMax += ptACDTarget->GetRadius();
    }
}


















static BOOL sCondTargetInRange(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    ACDID idACD = tActor.idACD;
    WorldPlace wpPlace = ACDGet(idACD).GetPlace();
    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone())
        return FALSE;
    if (!tTarget.IsInWorld())
        return FALSE;
    WorldPlace wpTargetPlace;
    tTarget.GetPlace(wpTargetPlace);
    if (wpPlace.idSWorld != wpTargetPlace.idSWorld)
        return FALSE;
    float flRangeMin;
    float flRangeMax;
    sGetConditionRange(idACD, tCondition, tTarget, flRangeMin, flRangeMax);
    float flDistSqr = Vector3D::DistanceSquared(wpPlace.wp, wpTargetPlace.wp);
    if (flDistSqr >= flRangeMin * flRangeMin && flDistSqr < flRangeMax * flRangeMax)
        return TRUE;
    return FALSE;
}

















static BOOL sCondTargetNotInRange(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    return !sCondTargetInRange(tActor, tCondition, TargetCache, tEntryData);
}





static BOOL sCondRunAwayDistanceFromTargetAtleast(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    ACDID idACD = tActor.idACD;

    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone())
        return FALSE;


    float flRangeMin, flRangeMax;
    sGetConditionRange(idACD, tCondition, tTarget, flRangeMin, flRangeMax);

    const ActorCommonData & tACD = ACDGet(idACD);

    WorldPlace wpTargetPlace;
    BrainAPIGetRunAwayPlace(idACD, tTarget, wpTargetPlace, flRangeMin, flRangeMin);

    const ActorMovement * ptACM = tACD.GetActorMovement();
    if (ptACM == NULL)
        return FALSE;

    PowerComputeDestination(tEntryData.ptEntry->snoPower, *ptACM, wpTargetPlace, tACD.GetPlace(), wpTargetPlace, 0, 10.0f);


    float flDistSqr = Vector3D::DistanceSquared(tACD.GetPlace().wp, wpTargetPlace.wp);
    return flDistSqr >= flRangeMin * flRangeMin;
}














static BOOL sCondTargetInRangeOfOwner(
    Actor & tActor,
    AICondition & tCondition,
    CTargetCache & TargetCache,
    EntryData & tEntryData)
{
    ACDID idACD = tActor.idACD;

    ACDID idACDOwner = ACDGetPetOwner(idACD);
    ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
    if (ptACDOwner == NULL)
        return FALSE;



    WorldPlace wpPlace = ptACDOwner->GetPlace();
    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone())
        return FALSE;
    if (!tTarget.IsInWorld())
        return FALSE;

    WorldPlace wpTargetPlace;
    tTarget.GetPlace(wpTargetPlace);
    if (wpPlace.idSWorld != wpTargetPlace.idSWorld)
        return FALSE;



    float flRangeMin, flRangeMax;
    sGetConditionRange(idACD, tCondition, tTarget, flRangeMin, flRangeMax);


    const float cgflOneOverRoot2 = 0.70710678f;
    Vector3D wvOffset = (cgwvXAxis + cgwvYAxis) * cgflOneOverRoot2;
    Team eTeam = ptACDOwner->GetTeam();

    if (WorldIsRotated(WorldCommonGetWorldSNO(wpPlace.idSWorld)))
    {

        wvOffset = cgwvXAxis;
    }
    if (GameIsParamFlagSet(GPF_REVERSE_CAMERA_FOR_TEAM_1))
    {
        if (eTeam != TEAM_PLAYER_1)
        {
            wvOffset = -wvOffset;
        }
    }
    else
    {

        if (eTeam >= TEAM_PLAYER_FIRST && eTeam <= TEAM_PLAYER_LAST)
        {
            wvOffset = -wvOffset;
        }
    }


    wvOffset *= flRangeMax * 0.25f;
    wpPlace.wp += wvOffset;

    float flDistSqr = Vector3D::DistanceSquared(wpPlace.wp, wpTargetPlace.wp);
    if (flDistSqr >= flRangeMin * flRangeMin &&
        flDistSqr < flRangeMax * flRangeMax)
        return TRUE;



    return FALSE;
}


static BOOL sCondTargetNotInRangeOfOwner(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    ACDID idACD = tActor.idACD;
    ACDID idACDOwner = ACDGetPetOwner(idACD);
    ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
    if (ptACDOwner == NULL)
        return FALSE;
    return !sCondTargetInRangeOfOwner(tActor, tCondition, TargetCache, tEntryData);
}










static BOOL sCondTargetInRangeOfGuardObject(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    ACDID idACD = tActor.idACD;
    ActorCommonData & tACD = ACDGet(idACD);
    ACDID idACDGuardObject = (ACDID)tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_GUARD_OBJECT_ACDID));
    ActorCommonData * ptACDGuardObject = ACDTryToGet(idACDGuardObject);
    if (ptACDGuardObject == NULL)
        return FALSE;
    if (!ptACDGuardObject->IsInWorld())
        return FALSE;
    WorldPlace wpPlace = ptACDGuardObject->GetPlace();
    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone())
        return FALSE;
    if (!tTarget.IsInWorld())
        return FALSE;
    WorldPlace wpTargetPlace;
    BOOL bGotPlace = tTarget.GetPlace(wpTargetPlace);
    if (!bGotPlace)
        return FALSE;
    if (wpPlace.idSWorld != wpTargetPlace.idSWorld)
        return FALSE;
    if (!isfinite(wpPlace.wp.x) || !isfinite(wpPlace.wp.y) || !isfinite(wpPlace.wp.z) ||
        !isfinite(wpTargetPlace.wp.x) || !isfinite(wpTargetPlace.wp.y) || !isfinite(wpTargetPlace.wp.z))
        return FALSE;
    float flRangeMin;
    float flRangeMax;
    sGetConditionRange(idACD, tCondition, tTarget, flRangeMin, flRangeMax);
    float flDistSqr = Vector3D::DistanceSquared(wpPlace.wp, wpTargetPlace.wp);
    if (flDistSqr >= flRangeMin * flRangeMin && flDistSqr < flRangeMax * flRangeMax)
        return TRUE;
    return FALSE;
}


































static BOOL sCondTargetNotInRangeOfGuardObject(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    ActorCommonData & tACD = ACDGet(tActor.idACD);
    ACDID idACDGuardObject = (ACDID)tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_GUARD_OBJECT_ACDID));
    ActorCommonData * ptACDGuardObject = ACDTryToGet(idACDGuardObject);
    if (ptACDGuardObject == NULL)
        return FALSE;
    return !sCondTargetInRangeOfGuardObject(tActor, tCondition, TargetCache, tEntryData);
}











static BOOL sCondTargetHealthPercentInRange(
    Actor & tActor,
    AICondition & tCondition,
    CTargetCache & TargetCache,
    EntryData & tEntryData)
{


    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone())
    {
        return FALSE;
    }

    ACDID idACDTarget = tTarget.idACDTarget;
    if (idACDTarget == ACDID_INVALID)
    {
        return FALSE;
    }

    ActorCommonData & tACDTarget = ACDGet(idACDTarget);
    float flHPCurrent = tACDTarget.GetHitPointsCurrent();
    float flHPMaxTotal = tACDTarget.GetHitPointsMaxTotal();
    if (flHPMaxTotal <= EPSILON)
    {
        return FALSE;
    }

    float flMinHP = tCondition.flConditionParm[0];
    float flMaxHP = tCondition.flConditionParm[1];

    if (flMaxHP < flMinHP)
    {
        Math::Swap(flMinHP, flMaxHP);
    }

    float flHPPercent = flHPCurrent / flHPMaxTotal;
    return (flHPPercent >= flMinHP && flHPPercent <= flMaxHP);
}


static BOOL sCondTargetResourcePercentInRange(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone())
        return FALSE;
    ACDID idACDTarget = tTarget.idACDTarget;
    if (idACDTarget == ACDID_INVALID)
        return FALSE;
    ActorCommonData & tACDTarget = ACDGet(idACDTarget);
    float flManaCur = tACDTarget.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_CUR, tACDTarget.GetPrimaryResourceType());
    float flManaMax = tACDTarget.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_MAX_TOTAL, tACDTarget.GetPrimaryResourceType());
    if (flManaMax <= 1e-06f)
        return FALSE;
    float flManaPercent = flManaCur / flManaMax;
    return flManaPercent >= tCondition.flConditionParm[0] && flManaPercent <= tCondition.flConditionParm[1];
}


















static BOOL sCondChance(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    float flRandomUnitValue = RandFloat_Internal(SGameGlobalsGet().rng);
    return flRandomUnitValue < tCondition.flConditionParm[0];
}










static BOOL sCondHasTarget(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    return !tTarget.IsNone();
}








static BOOL sCondHasSkill(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    ACDID idACD = tActor.idACD;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL)
        return FALSE;
    SNO snoSkill = tEntryData.ptEntry->snoPower;
    return ptACD->AttributesGetInt(MakeAttribKey(ATTRIBUTE_SKILL_TOTAL, snoSkill)) > 0;
}












static BOOL sCondBuffNotActive(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone())
        return FALSE;
    ACDID idACDTarget = tTarget.GetACD();
    if (idACDTarget == ACDID_INVALID)
        return FALSE;
    ActorCommonData & tACD = ACDGet(idACDTarget);
    SNO snoPower = tEntryData.ptEntry->snoPower;
    if (tCondition.snoPower != SNO_NONE)
        snoPower = tCondition.snoPower;
    return PowerAPI_BuffFindOnACD(snoPower, tACD, ACDID_INVALID) == BUFFID_INVALID;
}

















static BOOL sCondBuffIsActive(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone())
        return FALSE;
    ACDID idACDTarget = tTarget.GetACD();
    if (idACDTarget == ACDID_INVALID)
        return FALSE;
    ActorCommonData & tACD = ACDGet(idACDTarget);
    SNO snoPower = tEntryData.ptEntry->snoPower;
    if (tCondition.snoPower != SNO_NONE)
        snoPower = tCondition.snoPower;
    return PowerAPI_BuffFindOnACD(snoPower, tACD, ACDID_INVALID) != BUFFID_INVALID;
}

















static BOOL sCondNoEnemyTimeInRange(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    uint32 dwTicks = BrainAPIGetTimeSinceEnemyTarget(tActor.id);
    float flSeconds = dwTicks * (1.0f / 60.0f);
    float flMin = Math::Max(0.0f, tCondition.flConditionParm[0]);
    float flMax = tCondition.flConditionParm[1];
    if (flMax <= 0.0f)
        flMax = FLT_MAX;
    flMax = Math::Max(flMin, flMax);
    return flSeconds >= flMin && flSeconds <= flMax;
}














static BOOL sCondLastAllyDeathTime(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    ActorAI * ptAI = tActor.ptAI;
    if (ptAI != NULL)
    {
        float flTime = tCondition.flConditionParm[0];
        uint32 dwNow = GameGetServerTime();
        uint32 dwTicks = Math::FloatToInt(Math::Max(0.0f, flTime) * 60.0f);
        if (ptAI->dwLastAllyDeathTime != 0 && ptAI->dwLastAllyDeathTime <= dwNow)
        {
            uint32 dwTimeSinceDeath = dwNow - ptAI->dwLastAllyDeathTime;
            if (dwTimeSinceDeath <= dwTicks)
                return TRUE;
        }
    }
    return FALSE;
}













static BOOL sCondLastAllyWarnTime(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    ActorAI * ptAI = tActor.ptAI;
    if (ptAI != NULL)
    {
        float flTime = tCondition.flConditionParm[0];
        uint32 dwNow = GameGetServerTime();
        uint32 dwTicks = Math::FloatToInt(Math::Max(0.0f, flTime) * 60.0f);
        if (ptAI->dwLastAllyWarnTime != 0 && ptAI->dwLastAllyWarnTime <= dwNow)
        {
            uint32 dwTimeSinceWarned = dwNow - ptAI->dwLastAllyWarnTime;
            if (dwTimeSinceWarned <= dwTicks)
                return TRUE;
        }
    }
    return FALSE;
}













static BOOL sCondLastAttackReceivedTime(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    ActorAI * ptAI = tActor.ptAI;
    if (ptAI != NULL)
    {
        float flTime = tCondition.flConditionParm[0];
        uint32 dwNow = GameGetServerTime();
        uint32 dwTicks = Math::FloatToInt(Math::Max(0.0f, flTime) * 60.0f);
        if (ptAI->dwLastDamageTime != 0 && ptAI->dwLastDamageTime <= dwNow)
        {
            uint32 dwTimeSinceDamage = dwNow - ptAI->dwLastDamageTime;
            if (dwTimeSinceDamage <= dwTicks)
                return TRUE;
        }
    }
    return FALSE;
}













static BOOL sCondLastAttackPerformedTime(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    ActorAI * ptAI = tActor.ptAI;
    if (ptAI != NULL)
    {
        float flTime = tCondition.flConditionParm[0];
        uint32 dwNow = GameGetServerTime();
        uint32 dwTicks = Math::FloatToInt(Math::Max(0.0f, flTime) * 60.0f);
        if (ptAI->dwLastAttackTime != 0 && ptAI->dwLastAttackTime <= dwNow)
        {
            uint32 dwTimeSinceAttack = dwNow - ptAI->dwLastAttackTime;
            if (dwTimeSinceAttack <= dwTicks)
                return TRUE;
        }
    }
    return FALSE;
}













static BOOL sCondTimeInCurrentState(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    ActorAI * ptAI = tActor.ptAI;
    if (ptAI != NULL)
    {
        AIStateInstanceData * ptInstData = sGetInstanceData(tActor);
        if (ptInstData != NULL)
        {
            float flTime = Math::Max(0.0f, tCondition.flConditionParm[0]);
            uint32 dwNow = GameGetServerTime();
            uint32 dwTicks = Math::FloatToInt(flTime * 60.0f);
            if (dwNow - ptInstData->dwStateEntryTime > dwTicks)
                return TRUE;
        }
    }
    return FALSE;
}
















static BOOL sCondTargetOnPathInRange(
    Actor & tActor,
    AICondition & tCondition,
    CTargetCache & TargetCache,
    EntryData & tEntryData)
{
    ActorAI * ptAI = tActor.ptAI;
    if (ptAI == NULL)
        return FALSE;
    if (AIHasNodePath(tActor.id))
    {



        ACDID idACD = tActor.idACD;
        ActorCommonData * ptACD = ACDTryToGet(idACD);
        if (ptACD == NULL)
            return FALSE;
        if (!ptACD->IsAlive())
            return FALSE;


        const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
        if (tTarget.IsNone() || !tTarget.IsInWorld())
            return FALSE;



        WorldPlace placeTarget;
        tTarget.GetPlace(placeTarget);
        BOOL bOnPath = AIIsTargetOnPath(tActor.id, placeTarget, tCondition.flConditionParm[0]);
        if (bOnPath)
            return TRUE;
    }



    return FALSE;
}


static BOOL sCondDerailedFromPath(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    return AIIsDerailedFromPath(tActor.id, tCondition.flConditionParm[0]);
}










static BOOL sCondTargetIsNormalMonster(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone())
        return FALSE;
    ACDID idACDTarget = tTarget.idACDTarget;
    if (idACDTarget == ACDID_INVALID)
        return FALSE;
    ActorCommonData & tACDTarget = ACDGet(idACDTarget);
    if (!tACDTarget.IsNormalMonster())
        return FALSE;
    if (ACDIsSummoned(idACDTarget))
    {
        ACDID idACDSummoner = ACDGetSummoner(idACDTarget);
        ActorCommonData * ptACDSummoner = ACDTryToGet(idACDSummoner);
        if (ptACDSummoner != NULL && !ptACDSummoner->IsNormalMonster())
            return FALSE;
    }
    return TRUE;
}




















static BOOL sCondTargetIsNotLargeMonster(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone())
        return FALSE;
    ACDID idACDTarget = tTarget.idACDTarget;
    if (idACDTarget == ACDID_INVALID)
        return FALSE;
    return !ACDGet(idACDTarget).IsLargeMonster();
}



















static BOOL sCondTargetIsNotWreckable(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone())
        return FALSE;
    ACDID idACDTarget = tTarget.idACDTarget;
    if (idACDTarget == ACDID_INVALID)
        return FALSE;
    return !ACDGet(idACDTarget).IsWreckableObject();
}



















static BOOL sCondHasAttachedActor(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    ACDID idACD = tActor.idACD;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL)
        return FALSE;
    return ptACD->HasAttachedACD() != FALSE;
}


















static BOOL sCondTargetIsOverStaticFlag(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData, int32 nFlag)
{
    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone())
        return FALSE;
    ACDID idACDTarget = tTarget.idACDTarget;
    if (idACDTarget == ACDID_INVALID)
        return FALSE;
    ActorCommonData & tACDTarget = ACDGet(idACDTarget);
    NavMeshHandle tNavMeshHandle;
    ACDGetNavMeshHandle(tACDTarget, tNavMeshHandle);
    if (NavMeshHandleIsNone(tNavMeshHandle))
        return FALSE;
    return NavMeshHandleGetStaticFlag(tNavMeshHandle, nFlag);
}



















static BOOL sCondTargetIsOverWalkableArea(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    return sCondTargetIsOverStaticFlag(tActor, tCondition, TargetCache, tEntryData, 0);
}






static BOOL sCondTargetIsNotOverWalkableArea(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    return !sCondTargetIsOverStaticFlag(tActor, tCondition, TargetCache, tEntryData, 0);
}






static BOOL sCondTargetIsOverSpecial0(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    return sCondTargetIsOverStaticFlag(tActor, tCondition, TargetCache, tEntryData, 7);
}






static BOOL sCondTargetIsNotOverSpecial0(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    return !sCondTargetIsOverStaticFlag(tActor, tCondition, TargetCache, tEntryData, 7);
}






static BOOL sCondTargetIsOverSpecial1(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    return sCondTargetIsOverStaticFlag(tActor, tCondition, TargetCache, tEntryData, 8);
}






static BOOL sCondTargetIsNotOverSpecial1(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    return !sCondTargetIsOverStaticFlag(tActor, tCondition, TargetCache, tEntryData, 8);
}






static BOOL sCondTargetIsInHelperArea(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone())
        return FALSE;
    ACDID idACDTarget = tTarget.GetACD();
    if (idACDTarget == ACDID_INVALID)
        return FALSE;
    ACDID idACD = tActor.idACD;
    ActorCommonData & tACDTarget = ACDGet(idACDTarget);
    LevelAreaID idLevelArea = ACDGetLevelAreaID(tACDTarget);
    SNO snoLevelArea = LevelAreaGetSNO(idLevelArea);
    ActorCommonData & tACD = ACDGet(idACD);
    return HelperIsLevelAreaAllowed(tACD, snoLevelArea);
}















static BOOL sCondTargetIsRooted(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone())
        return FALSE;
    ACDID idACDTarget = tTarget.GetACD();
    if (idACDTarget == ACDID_INVALID)
        return FALSE;
    return ACDGet(idACDTarget).IsRooted() || ACDGet(idACDTarget).IsImmobilized();
}














static BOOL sCondTargetIsNotRooted(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    return !sCondTargetIsRooted(tActor, tCondition, TargetCache, tEntryData);
}






static BOOL sCondSummonCountLessThan(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    ACDID idACD = tActor.idACD;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL)
        return FALSE;
    int32 nCount = ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_SUMMON_COUNT));
    return nCount < Math::FloatToInt(tCondition.flConditionParm[0]);
}

















static BOOL sCondTotalSummonCountNotReached(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    ACDID idACD = tActor.idACD;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD != NULL)
    {
        int32 nMaxTotal;
        if (PowerContactTagGetInt(AutoPowerDefinitionR(tEntryData.ptEntry->snoPower), TAG_POWER_SPELL_FUNC_0, 0) == SPELL_UNIQUE_MONSTER_GENERIC_SUMMON)
        {
            SNO snoMonster = ActorGetMonsterSNO(tActor.snoActor);
            ScopedMonsterDefinitionLockR pdefMonster(snoMonster);
            nMaxTotal = TagMapGetInt(pdefMonster->hTagMap, TAG_MONSTER_GENERIC_SUMMON_MAX_TOTAL_SUMMONS);
        }
        else
        {
            nMaxTotal = PowerAPI_GetSummonMaxTotal(*ptACD, tEntryData.ptEntry->snoPower);
        }
        int32 nCount = ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_SUMMON_COUNT));
        if (nCount < nMaxTotal)
            return TRUE;
    }
    return FALSE;
}



















static BOOL sCondConcurrentSummonCountNotReached(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    ACDID idACD = tActor.idACD;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD != NULL)
    {
        WorldPlace placeCenter;
        float flTargetScanRadius = BrainAPIActorGetAwarenessScanRadius(idACD, placeCenter);
        if (tEntryData.ptEntry->snoPower != SNO_NONE)
        {
            int32 nMaxConcurrent;
            if (PowerContactTagGetInt(AutoPowerDefinitionR(tEntryData.ptEntry->snoPower), TAG_POWER_SPELL_FUNC_0, 0) == SPELL_UNIQUE_MONSTER_GENERIC_SUMMON)
            {
                SNO snoMonster = ActorGetMonsterSNO(tActor.snoActor);
                ScopedMonsterDefinitionLockR pdefMonster(snoMonster);
                nMaxConcurrent = TagMapGetInt(pdefMonster->hTagMap, TAG_MONSTER_GENERIC_SUMMON_MAX_CONCURRENT_SUMMONS);
            }
            else
            {
                nMaxConcurrent = PowerAPI_GetSummonMaxConcurrent(*ptACD, tEntryData.ptEntry->snoPower);
            }
            int32 nExistingUnits = ACDCountNearbySummonedUnits(*ptACD, flTargetScanRadius);
            if (nExistingUnits < nMaxConcurrent)
                return TRUE;
        }
    }
    return FALSE;
}






















static BOOL sCondDoesNotHaveAttackerSlotReserved(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone())
        return FALSE;
    ACDID idACDTarget = tTarget.GetACD();
    if (idACDTarget == ACDID_INVALID)
        return FALSE;
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    ActorID idTarget = ptACDTarget ? (ActorID)ptACDTarget->idOwner : ACTORID_INVALID;
    Actor * ptTarget = ActorTryToGet(idTarget);
    if (ptTarget == NULL)
        return FALSE;
    return !BrainAPIHasAttackerSlotReservedForTarget(tActor.id, idTarget);
}















static BOOL sIsPathDistanceInRange(
    ActorCommonData * ptACD,
    ActorCommonData * ptACDSource,
    AICondition & tCondition,
    CTargetCache & TargetCache,
    EntryData & tEntryData,
    BOOL bPathRequired)
{
    ActorMovement * ptMove = ptACDSource->GetActorMovement();
    if (ptMove == NULL)
        return FALSE;



    WorldPlace wpPlace = ptACDSource->GetPlace();
    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone() || !tTarget.IsInWorld())
        return FALSE;



    WorldPlace wpTargetPlace;
    tTarget.GetPlace(wpTargetPlace);

    if (wpPlace.idSWorld != wpTargetPlace.idSWorld)
        return FALSE;



    float flRangeMin, flRangeMax;
    sGetConditionRange(ptACD->id, tCondition, tTarget, flRangeMin, flRangeMax);

    PathPoint2DList listPathPoints;
    listPathPoints.SetPool(NavCellPathGetPointPool());

    PathResult eResult = ptMove->FindPath(wpTargetPlace, listPathPoints, 1, 0, NULL);
    if (eResult == PATH_RESULT_FOUND)
    {
        POS pos = listPathPoints.GetHeadPosition();
        float flDist = 0.0f;

        Vector2D vLastPoint = wpPlace.wp.As2D();
        BOOL bPathValid = TRUE;
        while (pos)
        {
            Vector2D & vPoint = listPathPoints.GetNext(pos);
            Vector2D vDelta = vPoint - vLastPoint;
            flDist += vDelta.Magnitude();
            vLastPoint = vPoint;

            if (flDist >= flRangeMax)
            {
                bPathValid = FALSE;
                break;
            }
        }

        if (bPathValid && flDist >= flRangeMin && flDist < flRangeMax)
            return TRUE;
        return FALSE;
    }

    if (bPathRequired && eResult != PATH_RESULT_NOT_NECESSARY)
        return FALSE;

    float flDistSqr = Vector3D::DistanceSquared(wpPlace.wp, wpTargetPlace.wp);
    if (flDistSqr >= flRangeMin * flRangeMin &&
        flDistSqr < flRangeMax * flRangeMax)
        return TRUE;



    return FALSE;

}


static BOOL sCondTargetHasPath(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    ACDID idACD = tActor.idACD;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL)
        return FALSE;
    return sIsPathDistanceInRange(ptACD, ptACD, tCondition, TargetCache, tEntryData, TRUE);
}









static BOOL sCondTargetPathDistanceInRange(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    ACDID idACD = tActor.idACD;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL)
        return FALSE;
    return sIsPathDistanceInRange(ptACD, ptACD, tCondition, TargetCache, tEntryData, FALSE);
}










static BOOL sCondTargetPathDistanceNotInRange(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    ACDID idACD = tActor.idACD;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL)
        return FALSE;
    return !sIsPathDistanceInRange(ptACD, ptACD, tCondition, TargetCache, tEntryData, FALSE);
}









static BOOL sCondTargetPathDistanceFromOwner(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    ACDID idACD = tActor.idACD;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL)
        return FALSE;
    ACDID idACDOwner = ACDGetPetOwner(idACD);
    ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
    if (ptACDOwner == NULL)
        return FALSE;
    return sIsPathDistanceInRange(ptACD, ptACDOwner, tCondition, TargetCache, tEntryData, FALSE);
}












static BOOL sCondTargetIsBurrowed(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone())
        return FALSE;
    ACDID idACDTarget = tTarget.GetACD();
    if (idACDTarget == ACDID_INVALID)
        return FALSE;
    return ACDGet(idACDTarget).IsBurrowed();
}














static BOOL sCondTargetIsNotBurrowed(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    return !sCondTargetIsBurrowed(tActor, tCondition, TargetCache, tEntryData);
}






static BOOL sCondTargetIsStealthed(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone())
        return FALSE;
    ACDID idACDTarget = tTarget.GetACD();
    if (idACDTarget == ACDID_INVALID)
        return FALSE;
    return ACDGet(idACDTarget).IsStealthed();
}














static BOOL sCondTargetIsNotStealthed(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    return !sCondTargetIsStealthed(tActor, tCondition, TargetCache, tEntryData);
}






static BOOL sCondTargetIsEnraged(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone())
        return FALSE;
    ACDID idACDTarget = tTarget.GetACD();
    if (idACDTarget == ACDID_INVALID)
        return FALSE;
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (ptACDTarget == NULL)
        return FALSE;
    return ptACDTarget->IsEnraged();
}












static BOOL sCondTargetIsNotEnraged(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    return !sCondTargetIsEnraged(tActor, tCondition, TargetCache, tEntryData);
}






static BOOL sCondTargetHasAttribute(AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData, const Attrib eAttrib)
{
    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone())
        return FALSE;
    ACDID idACDTarget = tTarget.GetACD();
    if (idACDTarget == ACDID_INVALID)
        return FALSE;
    ActorCommonData & tACD = ACDGet(idACDTarget);
    return tACD.AttributesGetInt(FastAttribKey(eAttrib)) != 0;
}












static BOOL sCondTargetIsInSpecialState(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    return sCondTargetHasAttribute(tCondition, TargetCache, tEntryData, ATTRIBUTE_AI_IN_SPECIAL_STATE);
}








static BOOL sCondTargetIsNotInSpecialState(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    return !sCondTargetIsInSpecialState(tActor, tCondition, TargetCache, tEntryData);
}






static BOOL sCondTargetPlayedSpawnAnim(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    return sCondTargetHasAttribute(tCondition, TargetCache, tEntryData, ATTRIBUTE_AI_USED_SPAWN_ANIM);
}








static BOOL sCondDoesNotHaveTarget(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    return !sCondHasTarget(tActor, tCondition, TargetCache, tEntryData);
}






static BOOL sCondRecentlyKilledLastTargetInRange(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (!tTarget.IsNone() && sCondTargetInRange(tActor, tCondition, TargetCache, tEntryData))
        return FALSE;
    ACDID idACD = tActor.idACD;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL)
        return FALSE;
    uint32 dwLastKillTime = ptACD->AttributesGetInt(FastAttribKey(ATTRIBUTE_LAST_ACD_KILLED_TIME));
    return GameGetServerTime() - dwLastKillTime < 60;
}

BOOL NavCellHandleFromWorldPlace(const WorldPlace & tPlace, NavCellHandle & hNavCell);  // X/Common/Nav/NavZone.cpp:730
BOOL WorldCollSearchForLocation(const WorldPlace & place, uint16 uStaticFlags, uint32 uDynamicFlags, float flRadius, float flMinDist, float flMaxDist, ACDID idACDIgnore,
                                Vector3D & vFoundLoc, NavCellHandle & hNavCellFound, Team eTeam, uint16 uFlags, ACDID idACDIgnore2);  // X/Common/WorldCommonCollision.cpp
BOOL WorldCollSweptCircle(SWorldID idSWorld, const NavCellHandle & hNavCellStart, const Circle & wcStart, const Vector2D & wpDest, const WorldCollParams & tWorldCollParams,
                          uint32 uCollTestFlags, uint32 uACDCollNarrowFlags, ACDID idACDIgnore, WorldCollResult & tResult);  // X/Common/WorldCommonCollision.cpp:147







static BOOL sCondHasDirectPathToTarget(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    if (tActor.ptSActorMovement == NULL)
        return FALSE;

    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone() || !tTarget.IsInWorld())
        return FALSE;

    WorldPlace tPlaceDest;
    tTarget.GetPlace(tPlaceDest);
    WorldPlace tPlaceStart;
    float flRadius;
    tActor.ptSActorMovement->GetPlaceAndRadius(tPlaceStart, flRadius, FALSE);
    if (tPlaceDest.idSWorld != tPlaceStart.idSWorld)
        return FALSE;
    if (WorldTryGetCommonData(tPlaceDest.idSWorld) == NULL || WorldTryGetCommonData(tPlaceStart.idSWorld) == NULL)
        return FALSE;

    Circle circleStart;
    circleStart.Set(tPlaceStart.wp.x, tPlaceStart.wp.y, flRadius);

    ACDCollSettings tCollSettingsDuring;
    ACDCollSettings tCollSettingsDest;
    tActor.ptSActorMovement->GetCollSettings(tEntryData.ptEntry->snoPower, POWER_PASSABILITY_DURING, tCollSettingsDuring);
    tActor.ptSActorMovement->GetCollSettings(tEntryData.ptEntry->snoPower, POWER_PASSABILITY_DESTINATION, tCollSettingsDest);

    Vector3D vFoundLoc;
    NavCellHandle hNavCellTarget;
    NavCellHandle hNavCellStart;
    NavCellHandleFromWorldPlace(tPlaceStart, hNavCellStart);

    BOOL bFoundDest = WorldCollSearchForLocation(tPlaceDest, tCollSettingsDest.uStaticOneRequiredFlags, tCollSettingsDest.uDynamicFlagsNoOverlap,
                                                 flRadius, 0.0f, 10.0f, tActor.idACD, vFoundLoc, hNavCellTarget, tCollSettingsDest.eTeam, 0, ACDID_INVALID);
    if (!bFoundDest)
        return FALSE;

    WorldCollResult tResultDuring;
    WorldCollSweptCircle(tPlaceDest.idSWorld, hNavCellStart, circleStart, vFoundLoc.As2D(),
                         WorldCollParams(tCollSettingsDuring), 3, 1, tActor.idACD, tResultDuring);
    return !tResultDuring.bCollision;
}
































static BOOL sCondHasNoDirectPathToTarget(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    return !sCondHasDirectPathToTarget(tActor, tCondition, TargetCache, tEntryData);
}






static BOOL sCondDirectPathToWithinRangeOfTarget(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    if (tActor.ptSActorMovement == NULL)
        return FALSE;

    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone() || !tTarget.IsInWorld())
        return FALSE;

    WorldPlace tPlaceDest;
    tTarget.GetPlace(tPlaceDest);
    WorldPlace tPlaceStart;
    float flRadius;
    tActor.ptSActorMovement->GetPlaceAndRadius(tPlaceStart, flRadius, FALSE);
    if (tPlaceDest.idSWorld != tPlaceStart.idSWorld)
        return FALSE;
    if (WorldTryGetCommonData(tPlaceDest.idSWorld) == NULL || WorldTryGetCommonData(tPlaceStart.idSWorld) == NULL)
        return FALSE;

    Circle circleStart;
    circleStart.Set(tPlaceStart.wp.x, tPlaceStart.wp.y, flRadius);

    ACDCollSettings tCollSettingsDuring;
    tActor.ptSActorMovement->GetCollSettings(tEntryData.ptEntry->snoPower, POWER_PASSABILITY_DURING, tCollSettingsDuring);

    NavCellHandle hNavCellStart;
    NavCellHandleFromWorldPlace(tPlaceStart, hNavCellStart);

    WorldCollResult tResultDuring;
    WorldCollSweptCircle(tPlaceDest.idSWorld, hNavCellStart, circleStart, tPlaceDest.wp.As2D(),
                         WorldCollParams(tCollSettingsDuring), 3, 1, tActor.idACD, tResultDuring);
    if (!tResultDuring.bCollision)
        return TRUE;

    float flMaxRange = tCondition.flConditionParm[0];
    float flDistSq = Vector2D::DistanceSquared(tResultDuring.wpEnd.As2D(), tPlaceDest.wp.As2D());
    return flDistSq < flMaxRange * flMaxRange;
}






























static BOOL sCondFailedDynamicPath(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    if (tActor.ptSActorMovement != NULL && tActor.ptSActorMovement->HasFailedDynamicPath())
        return TRUE;
    return FALSE;
}













static BOOL sCondTargetSpawnedByInactiveMarker(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    return sCondTargetHasAttribute(tCondition, TargetCache, tEntryData, ATTRIBUTE_SPAWNED_BY_INACTIVE_MARKER);
}








static BOOL sCondTargetWithinFacing(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{

    ACDID idACD = tActor.idACD;
    ActorCommonData & tACD = ACDGet(idACD);
    WorldPlace wpPlace = tACD.GetPlace();

    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone())
        return FALSE;
    if (!tTarget.IsInWorld())
        return FALSE;

    WorldPlace wpTargetPlace;
    tTarget.GetPlace(wpTargetPlace);
    if (wpPlace.idSWorld != wpTargetPlace.idSWorld)
        return FALSE;



    Vector3D wvMeToTarget = wpTargetPlace.wp - wpPlace.wp;
    wvMeToTarget.Normalize();
    Vector3D wvFacing = tACD.GetForward();


    Quaternion qRotOffset;
    qRotOffset.SetRotation(cgwvZAxis, Math::DegToAngle(tCondition.flConditionParm[1]));
    wvFacing = qRotOffset.Rotate(wvFacing);



    float fHalfAngleCos = Math::Cos(Math::DegToAngle(tCondition.flConditionParm[0]));
    float fDotProduct = Vector3D::DotProduct(wvMeToTarget, wvFacing);
    return fDotProduct >= fHalfAngleCos;
}











static BOOL sCondNoPathToTarget(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{

    ACDID idACD = tActor.idACD;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (ptACD == NULL)
        return FALSE;



    ActorMovement * ptMove = ptACD->GetActorMovement();
    if (ptMove == NULL)
        return FALSE;



    WorldPlace wpTargetPlace;
    Target tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.eType == TT_INVALID || tTarget.eType == TT_NONE)
        return FALSE;
    if (!tTarget.IsInWorld())
        return FALSE;
    tTarget.GetPlace(wpTargetPlace);



    PathPoint2DList listPathPoints;
    listPathPoints.SetPool(NavCellPathGetPointPool());

    ACDCollSettings tCollSettings;
    ptMove->GetCollSettings(tCollSettings);

    PathResult eResult = ptMove->FindPath(wpTargetPlace, listPathPoints, tCollSettings.uStaticOneRequiredFlags, tCollSettings.uStaticPreventFlags, NULL);
    return eResult == PATH_RESULT_NOT_FOUND;
}







static BOOL sCondTargetIsActive(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    WorldPlace wpPlace = ACDGet(tActor.idACD).GetPlace();
    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone())
        return FALSE;
    if (!tTarget.IsInWorld())
        return FALSE;
    ACDID idACDTarget = tTarget.GetACD();
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (ptACDTarget == NULL)
        return FALSE;
    return !BrainAPIActorAIIsInactive(*ptACDTarget);
}













static BOOL sCondTargetIsNotActive(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    return !sCondTargetIsActive(tActor, tCondition, TargetCache, tEntryData);
}






static int32 sGetPackAliveCount(ACDID idACD)
{
    int32 nPackMembersCount = 1;
    ActorCommonData & tACD = ACDGet(idACD);
    ACDPack * ptACDPack = ACDPackTryToGet(tACD.idACDPack);
    if (ptACDPack != NULL)
    {
        const ACDIDListPooled * plistACDPack = ptACDPack->GetMemberList();
        POS pos = plistACDPack->GetHeadPosition();
        while (pos)
        {
            ACDID idACDPackMember = (ACDID)plistACDPack->GetNext(pos);
            ActorCommonData * ptACDPackMember = ACDTryToGet(idACDPackMember);
            if (idACDPackMember != idACD && ptACDPackMember->IsAlive())
                nPackMembersCount++;
        }
    }
    return nPackMembersCount;
}







static BOOL sCondPackAliveCountGreaterThan(Actor & tActor, AICondition & tCondition, CTargetCache &, EntryData &)
{
    return sGetPackAliveCount(tActor.idACD) > Math::FloatToInt(tCondition.flConditionParm[0]);
}






static BOOL sCondPackAliveCountLessThan(Actor & tActor, AICondition & tCondition, CTargetCache &, EntryData &)
{
    return sGetPackAliveCount(tActor.idACD) < Math::FloatToInt(tCondition.flConditionParm[0]);
}






static BOOL sCondTargetIsAheadOnPath(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    ActorCommonData & tACD = ACDGet(tActor.idACD);
    WorldPlace wpPlace = tACD.GetPlace();
    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone())
        return FALSE;
    if (!tTarget.IsInWorld())
        return FALSE;
    ACDID idACD = tActor.idACD;
    WorldPlace wpTargetPlace;
    tTarget.GetPlace(wpTargetPlace);
    if (wpPlace.idSWorld != wpTargetPlace.idSWorld)
        return FALSE;
    float flDist = tCondition.flConditionParm[0];
    return AITargetIsAheadOnPath(tActor.id, wpTargetPlace, flDist);
}












static BOOL sCondTargetCanBeImmobilized(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    WorldPlace wpPlace = ACDGet(tActor.idACD).GetPlace();
    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone())
        return FALSE;
    if (!tTarget.IsInWorld())
        return FALSE;
    ACDID idACDTarget = tTarget.GetACD();
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (ptACDTarget == NULL)
        return FALSE;
    return ptACDTarget->CanBeImmobilized();
}













static BOOL sCondGameIsStoryMode(Actor &, AICondition &, CTargetCache &, EntryData &)
{
    return GameIsStoryMode();
}






static BOOL sCondGameIsAventureMode(Actor &, AICondition &, CTargetCache &, EntryData &)
{
    return GameIsAdventureMode();
}






static BOOL sCondMonsterPowerIsAtLeast(Actor &, AICondition & tCondition, CTargetCache &, EntryData &)
{
    return GameGetHandicapLevel() >= tCondition.flConditionParm[0];
}






static BOOL sCondMonsterPowerIsLessThan(Actor &, AICondition & tCondition, CTargetCache &, EntryData &)
{
    return GameGetHandicapLevel() < tCondition.flConditionParm[0];
}







static BOOL sCondTargetOnWalkableNotBurrowedOrStealthed(Actor & tActor, AICondition & tCondition, CTargetCache & tTargetCache, EntryData & tEntryData)
{
    return !sCondTargetIsBurrowed(tActor, tCondition, tTargetCache, tEntryData) &&
           !sCondTargetIsStealthed(tActor, tCondition, tTargetCache, tEntryData) &&
           sCondTargetIsOverWalkableArea(tActor, tCondition, tTargetCache, tEntryData);
}






static int32 sGetTargetActorLevel(AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    int32 nLevel = 1;
    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (!tTarget.IsNone() && tTarget.IsInWorld())
    {
        ACDID idACDTarget = tTarget.GetACD();
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget != NULL)
            nLevel = ptACDTarget->AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));
    }
    return nLevel;
}








static BOOL sCondTargetLevelIsAtLeast(Actor &, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    int32 nLevel = sGetTargetActorLevel(tCondition, TargetCache, tEntryData);
    return nLevel >= Math::FloatRoundToInt(tCondition.flConditionParm[0]);
}






static BOOL sCondTargetLevelIsLessThan(Actor &, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    int32 nLevel = sGetTargetActorLevel(tCondition, TargetCache, tEntryData);
    return nLevel < Math::FloatRoundToInt(tCondition.flConditionParm[0]);
}






static BOOL sCondTimeSinceEnteredAggressive(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    ActorAI * ptAI = tActor.ptAI;
    if (ptAI == NULL)
        return FALSE;
    AIStateInstanceData * ptInstData = sGetInstanceData(tActor);
    if (ptInstData == NULL)
        return FALSE;
    if (ptInstData->dwAggressiveStartTime == 0)
        return FALSE;
    float flTime = Math::Max(0.0f, tCondition.flConditionParm[0]);
    uint32 dwTicks = Math::FloatToInt(flTime * 60.0f);
    uint32 dwNow = GameGetServerTime();
    return dwNow >= ptInstData->dwAggressiveStartTime + dwTicks;
}



















static BOOL sCondIsInChina(Actor &, AICondition &, CTargetCache &, EntryData &)
{
    return FALSE;
}











static BOOL sCondTargetIsUpAgainstWall(
    Actor & tActor,
    AICondition & tCondition,
    CTargetCache & TargetCache,
    EntryData & tEntryData)
{
    ActorCommonData * ptACDSelf = ACDTryToGet(tActor.idACD);


    if (tCondition.flConditionParm[0] <= EPSILON)
    {
        return FALSE;
    }


    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone() || !tTarget.IsInWorld())
    {
        return FALSE;
    }


    WorldPlace wpTarget;
    if (!tTarget.GetPlace(wpTarget) || wpTarget.idSWorld != ptACDSelf->GetPlace().idSWorld)
    {
        return FALSE;
    }


    ActorCommonData * ptACDPassability = ACDTryToGet(tTarget.GetACD());
    if (!ptACDPassability)
    {
        ptACDPassability = ACDTryToGet(tActor.idACD);
    }
    if (!ptACDPassability)
    {
        return FALSE;
    }

    ActorID idActorPassability = (ActorID)ACDGetOwner(ptACDPassability->id);
    Actor * ptActorPassability = ActorTryToGet(idActorPassability);
    if (!ptActorPassability)
    {
        return FALSE;
    }


    ACDCollSettings tCollSettings;
    ptActorPassability->ptSActorMovement->GetCollSettings(tCollSettings);
    tCollSettings.uDynamicFlagsNoOverlap = 0;


    NavCellHandle hNavCellStart;
    NavCellHandleFromWorldPlace(wpTarget, hNavCellStart);

    Circle circleStart;
    circleStart.Set(wpTarget.wp.x, wpTarget.wp.y, ptActorPassability->ptSActorMovement->GetRadius(FALSE));


    Vector3D vBackDir;
    if (tTarget.IsTargetingACD())
    {

        vBackDir = ptACDPassability->GetWorldLocation().transform.q.Rotate(-cgwvXAxis);
    }
    else
    {

        vBackDir = wpTarget.wp - ptACDPassability->GetPlace().wp;
        vBackDir.Normalize();
    }


    Vector3D wpEnd = wpTarget.wp + vBackDir * tCondition.flConditionParm[0];


    WorldCollResult tResultDuring;
    WorldCollSweptCircle(wpTarget.idSWorld,
                         hNavCellStart,
                         circleStart,
                         wpEnd.As2D(),
                         WorldCollParams(tCollSettings),
                         (1 << NAV_ZONE_COLL_ROUNDED_CORNERS) | (1 << NAV_ZONE_COLL_IGNORE_COLLISION_IF_MOVING_AWAY),
                         1 << ACD_CNF_IGNORE_OBJECTS_IF_MOVING_AWAY,
                         ptActorPassability->idACD,
                         tResultDuring);

    return tResultDuring.bCollision;
}


static BOOL sCondTargetIsNotInRift(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone() || !tTarget.IsInWorld())
        return FALSE;
    ACDID idACDTarget = tTarget.GetACD();
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (ptACDTarget == NULL)
        return FALSE;
    BOOL bInRift = ACDIsInLootRun(*ptACDTarget) || ACDIsInTieredLootRun(*ptACDTarget);
    return !bInRift;
}














static BOOL sCondTargetIsNotInBossFight(Actor & tActor, AICondition & tCondition, CTargetCache & TargetCache, EntryData & tEntryData)
{
    const Target & tTarget = TargetCache.GetTarget(tCondition.eConditionTargetType, tEntryData);
    if (tTarget.IsNone() || !tTarget.IsInWorld())
        return FALSE;
    ACDID idACDTarget = tTarget.GetACD();
    if (idACDTarget == ACDID_INVALID)
        return FALSE;
    ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
    if (ptACDTarget == NULL)
        return FALSE;
    return ptACDTarget->AttributesGetInt(FastAttribKey(ATTRIBUTE_IN_BOSS_ENCOUNTER)) == 0;
}














AIConditionFunc sgConditionFuncs[80] = {
    sCondTargetInRange,
    sCondTargetNotInRange,
    sCondTargetHealthPercentInRange,
    sCondChance,
    sCondHasTarget,
    sCondHasSkill,
    sCondNoEnemyTimeInRange,
    sCondLastAllyDeathTime,
    sCondLastAllyWarnTime,
    sCondLastAttackReceivedTime,
    sCondLastAttackPerformedTime,
    sCondTimeInCurrentState,
    sCondTargetOnPathInRange,
    sCondDerailedFromPath,
    sCondTargetIsNormalMonster,
    sCondTargetIsNotLargeMonster,
    sCondTargetIsOverWalkableArea,
    sCondTargetIsNotOverWalkableArea,
    sCondTargetIsInHelperArea,
    sCondTargetIsRooted,
    sCondTargetIsNotRooted,
    sCondSummonCountLessThan,
    sCondTotalSummonCountNotReached,
    sCondConcurrentSummonCountNotReached,
    sCondTargetIsOverSpecial0,
    sCondTargetIsNotOverSpecial0,
    sCondTargetIsOverSpecial1,
    sCondTargetIsNotOverSpecial1,
    sCondTargetPathDistanceInRange,
    sCondTargetPathDistanceNotInRange,
    sCondTargetIsBurrowed,
    sCondTargetIsNotBurrowed,
    sCondTargetIsStealthed,
    sCondTargetIsNotStealthed,
    sCondTargetIsInSpecialState,
    sCondTargetWithinFacing,
    sCondTargetIsNotInSpecialState,
    sCondTargetResourcePercentInRange,
    sCondTargetInRangeOfOwner,
    sCondTargetNotInRangeOfOwner,
    sCondBuffNotActive,
    sCondBuffIsActive,
    sCondTargetPathDistanceFromOwner,
    NULL,
    sCondTargetIsEnraged,
    sCondTargetIsNotEnraged,
    sCondNoPathToTarget,
    sCondTargetIsNotWreckable,
    sCondHasAttachedActor,
    sCondTargetInRangeOfGuardObject,
    sCondTargetNotInRangeOfGuardObject,
    sCondRunAwayDistanceFromTargetAtleast,
    sCondTargetHasPath,
    sCondTargetPlayedSpawnAnim,
    sCondTargetSpawnedByInactiveMarker,
    sCondHasNoDirectPathToTarget,
    sCondHasDirectPathToTarget,
    sCondRecentlyKilledLastTargetInRange,
    sCondDoesNotHaveTarget,
    sCondDirectPathToWithinRangeOfTarget,
    sCondFailedDynamicPath,
    sCondDoesNotHaveAttackerSlotReserved,
    sCondTargetIsActive,
    sCondTargetIsNotActive,
    sCondPackAliveCountGreaterThan,
    sCondPackAliveCountLessThan,
    sCondTargetIsAheadOnPath,
    sCondTargetCanBeImmobilized,
    sCondGameIsStoryMode,
    sCondGameIsAventureMode,
    sCondTargetIsUpAgainstWall,
    sCondMonsterPowerIsAtLeast,
    sCondMonsterPowerIsLessThan,
    sCondTargetOnWalkableNotBurrowedOrStealthed,
    sCondTargetLevelIsAtLeast,
    sCondTargetLevelIsLessThan,
    sCondTimeSinceEnteredAggressive,
    sCondIsInChina,
    sCondTargetIsNotInRift,
    sCondTargetIsNotInBossFight,
};




static AIStateInstanceData * sGetInstanceData(ActorID idActor)
{
    ActorAI * ptAI = ActorTryToGetAI(idActor);
    if (ptAI == NULL)
        return NULL;
    return ptAI->ptAIStateInstData;
}







static AIStateInstanceData * sGetInstanceData(Actor & tActor)
{
    ActorAI * ptAI = tActor.ptAI;
    if (ptAI == NULL)
        return NULL;
    return ptAI->ptAIStateInstData;
}






inline uint32 sGetCoolDownKey(AIStateType eStateType, AIEntry & tEntry, BOOL bCoolUp)
{
    // A fixed cooldown key from the data wins over the generated one.
    if (!bCoolUp && tEntry.eCoolDown != AICOOLDOWN_NONE)
        return tEntry.eCoolDown;

    // Generate a key from the state, the entry's priority and a hash of the entry's contents.
    uint32 uStateType = eStateType + 1;
    uStateType &= 0xf;

    uint32 uStateSwitch = tEntry.eStateSwitch + 1;
    uStateSwitch <<= 4;
    uStateSwitch &= 0xf0;

    uint32 uPriority = tEntry.dwPriority;
    uPriority &= 0x7f;
    uPriority <<= 1;
    if (bCoolUp)
        uPriority |= 1;

    uint32 uHack = tEntry.dwCoolUpMin + tEntry.dwCoolUpDelta + tEntry.dwCoolDownMin + tEntry.dwCoolDownDelta + tEntry.dwWeight;
    for (int32 i = 0; i < 5; i++)
        uHack += tEntry.arCondition[i].eCondition;
    uHack += tEntry.snoPower;

    uint32 uKey = uStateType | uStateSwitch | (uPriority << 8) | (uHack << 16);
    return uKey;
}














uint32 sGetCoolDown(AIStateInstanceData & tInstData, AIEntry & tEntry)
{
    uint32 uCoolDown = 0;
    uint32 uCoolDownKey = sGetCoolDownKey(tInstData.eCurrState, tEntry, FALSE);
    tInstData.mapCoolDown.Lookup(uCoolDownKey, uCoolDown);
    return uCoolDown;
}






uint32 sGetCoolUp(AIStateInstanceData & tInstData, AIEntry & tEntry)
{
    uint32 uCoolUp = 0;
    uint32 uCoolUpKey = sGetCoolDownKey(tInstData.eCurrState, tEntry, TRUE);
    tInstData.mapCoolDown.Lookup(uCoolUpKey, uCoolUp);
    return uCoolUp;
}






inline void sSetCoolDown(AIStateInstanceData & tInstData, uint32 uCoolDownKey, uint32 uCoolDown)
{
    uint32 uNow = GameGetServerTime();
    tInstData.mapCoolDown.SetAt_Tracked(uCoolDownKey, uNow + uCoolDown);
}







SNO AIStateGetSNO(ActorID idActor, AIStateType eStateType)
{
    if (eStateType >= 0 && eStateType < AISTATE_TYPE_COUNT)
    {
        AIStateInstanceData * ptInstData = sGetInstanceData(idActor);
        if (ptInstData != NULL)
        {
            SNO snoAIState = ptInstData->arAIStateTable[eStateType];
            if (snoAIState != SNO_NONE && SNOFileExists(AISTATE_SNO, snoAIState))
                return snoAIState;
        }
    }
    return SNO_NONE;
}

















static SNO sGetCurrentAIStateSNO(ActorID idActor)
{

    AIStateInstanceData * ptInstData = sGetInstanceData(idActor);
    if (ptInstData)
    {
        return AIStateGetSNO(idActor, ptInstData->eCurrState);
    }
    return SNO_NONE;
}




static BOOL sHasAIState(Actor & tActor, AIStateType eStateType)
{


    if (eStateType >= 0 && eStateType < AISTATE_TYPE_COUNT)
    {
        AIStateInstanceData * ptInstData = sGetInstanceData(tActor);
        if (ptInstData)
        {
            SNO snoAIState = ptInstData->arAIStateTable[eStateType];
            if (snoAIState != SNO_NONE && SNOFileExists(AISTATE_SNO, snoAIState))
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}




static Target sGetPowerTarget(
    EntryData & tEntryData,
    CTargetCache & TargetCache)
{
    Target tTarget;
    tTarget.Clear();


    AIEntry & tEntry = *tEntryData.ptEntry;

    if (tEntry.snoPower != SNO_NONE)
    {
        if (tEntry.ePowerTargetType != AISTATE_TARGET_NONE)
        {

            tTarget = TargetCache.GetTarget(tEntry.ePowerTargetType, tEntryData);
        }
        else






        if (PowerWantsActorTarget(AutoPowerDefinitionR(tEntry.snoPower)))
        {
            if (PowerCastTargetAllies(AutoPowerDefinitionR(tEntry.snoPower)))
            {
                tTarget = TargetCache.GetTarget(AISTATE_TARGET_CLOSEST_FRIEND, tEntryData);
            }
            else
            {
                tTarget = TargetCache.GetTarget(AISTATE_TARGET_PRIMARY_ENEMY, tEntryData);
            }
        }
    }


    return tTarget;
}


static BOOL sEvaluateEntry(
    Actor & tActor,
    EntryData & tEntryData,
    CTargetCache & TargetCache,
    AIDecisionType eDecisionType)
{


    if (!tEntryData.ptEntry)
    {
        return FALSE;
    }

    AIEntry & tEntry = *tEntryData.ptEntry;


    if ((tEntry.snoPower == SNO_NONE && !sHasAIState(tActor, tEntry.eStateSwitch)) ||
        tEntry.dwWeight == 0)
    {
        return FALSE;
    }

    AIStateInstanceData * ptInstData = sGetInstanceData(tActor);



    uint32 dwNow = GameGetServerTime();

    uint32 uCoolUp = sGetCoolUp(*ptInstData, tEntry);
    if (uCoolUp > 0)
    {
        uint32 dwTimeSinceStart = dwNow - ptInstData->dwStateEntryTime;
        if (dwTimeSinceStart < uCoolUp)
        {
            return FALSE;
        }
    }


    uint32 uCoolDown = sGetCoolDown(*ptInstData, tEntry);
    if (dwNow < uCoolDown)
    {
        return FALSE;
    }

    ACDID idACD = tActor.idACD;
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
    {
        return FALSE;
    }


    for (int32 nCondition = 0; nCondition < AISTATE_MAX_CONDITIONS_PER_ENTRY; nCondition++)
    {
        AICondition & tCondition = tEntry.arCondition[nCondition];

        if (tCondition.eCondition != AISTATE_CONDITION_NONE)
        {
            AIConditionFunc pFunc = sgConditionFuncs[tCondition.eCondition];
            if (!pFunc ||
                !pFunc(tActor, tCondition, TargetCache, tEntryData))
            {
                return FALSE;
            }
        }
    }


    if (tEntry.snoPower != SNO_NONE)
    {
        if (!PowerIsCoolDownDone(*ptACD, tEntry.snoPower))
        {
            return FALSE;
        }

        ScopedPowerDefinitionLockR pdefPower(tEntry.snoPower);

        Target tTarget = sGetPowerTarget(tEntryData, TargetCache);
        uint32 dwFailureReasons = PowerFailureReasons(*ptACD, AutoPowerDefinitionR(pdefPower), &tTarget, TRUE, FALSE);
        if (!ACDCanAttackTarget(*ptACD, tTarget, AutoPowerDefinitionR(pdefPower), &dwFailureReasons))
        {
            return FALSE;
        }

        if (!PowerCanUseNow(*ptACD, AutoPowerDefinitionR(pdefPower), &tTarget, &dwFailureReasons))
        {
            return FALSE;
        }
        if (!(tTarget.eType == TT_ACTOR && tTarget.idACDTarget != ACDID_INVALID) &&
            PowerWantsActorTarget(AutoPowerDefinitionR(pdefPower)))
        {
            return FALSE;
        }
    }

    return TRUE;
}



static void sUpdateMergeList(AIStateInstanceData & tInstData)
{

    uint32 uNow = GameGetServerTime();

    POS pos = tInstData.listMergeData.GetHeadPosition();
    while (pos)
    {
        POS posLast = pos;
        AIMergeData & tMergeData = tInstData.listMergeData.GetNext(pos);
        if (uNow > tMergeData.uEndTime && tMergeData.uEndTime != 0)
        {

            tInstData.listMergeData.RemoveAt(posLast);
        }
    }
}
typedef int32 AIDecisionType;  // X/Common/AI/ActorAIStatesSwig.h
BOOL AIStateChooseSkill(ActorID idActor, BrainAttackSettings & tAttackSettings, AIDecisionType eDecisionType);  // line 5104

BOOL AIStateChooseSkill(ActorID idActor, BrainAttackSettings & tAttackSettings)
{
    return AIStateChooseSkill(idActor, tAttackSettings, AIDECISION_TYPE_ACTION_UPDATE);
}





static void sAddPowerEntry(
    TEntryDataList & listEntryData,
    SNO snoPower,
    ACDID idACDBroadcastSource,
    ACDID idACDCastLocation,
    uint32 uPriority)
{
    AIEntry * ptNewEntry = sGetTempAIEntry();
    if (ptNewEntry)
    {
        SigmaMemoryClear(ptNewEntry, sizeof(AIEntry));
        ptNewEntry->dwPriority = uPriority;
        ptNewEntry->dwWeight = 100;
        for (int32 i = 0; i < AISTATE_MAX_CONDITIONS_PER_ENTRY; i++)
        {
            ptNewEntry->arCondition[i].eCondition = AISTATE_CONDITION_NONE;
            ptNewEntry->arCondition[i].eConditionTargetType = AISTATE_TARGET_NONE;
            ptNewEntry->arCondition[i].snoPower = SNO_NONE;
        }
        ptNewEntry->snoPower = snoPower;
        ptNewEntry->gbidMovementStyle = GBID_NONE;
        ptNewEntry->ePowerTargetType = AISTATE_TARGET_BROADCASTER;
        ptNewEntry->eCoolDown = AICOOLDOWN_NONE;
        ptNewEntry->eStateSwitch = AISTATE_TYPE_NONE;

        EntryData tEntryData;
        tEntryData.ptEntry = ptNewEntry;
        tEntryData.idACDBroadcastSource = idACDBroadcastSource;
        tEntryData.idACDCastLocation = idACDCastLocation;
        tEntryData.uWeight = 0;

        POS pos = listEntryData.GetHeadPosition();
        while (pos)
        {
            POS posInsert = pos;
            EntryData & tCurrEntryData = listEntryData.GetNext(pos);
            if (tCurrEntryData.ptEntry->dwPriority <= ptNewEntry->dwPriority)
            {
                listEntryData.InsertBefore_Tracked(posInsert, tEntryData);
                return;
            }
        }

        listEntryData.AddTail_Tracked(tEntryData);
    }
}









static void sAddEntries(
    TEntryDataList & listEntryData,
    const AIDecision & tDecision,
    ACDID idACDBroadcastSource)
{
    POS pos = listEntryData.GetHeadPosition();
    for (int32 i = 0; i < tDecision.nEntryCount; i++)
    {
        EntryData tEntryData;
        tEntryData.ptEntry = &tDecision.arEntries[i];
        tEntryData.idACDBroadcastSource = idACDBroadcastSource;
        tEntryData.uWeight = 0;

        BOOL bAdded = FALSE;
        while (pos)
        {
            POS posInsert = pos;
            EntryData & tCurrEntryData = listEntryData.GetNext(pos);
            if (tCurrEntryData.ptEntry->dwPriority <= tEntryData.ptEntry->dwPriority)
            {
                listEntryData.InsertBefore_Tracked(posInsert, tEntryData);
                pos = posInsert;
                bAdded = TRUE;
                break;
            }
        }
        if (!bAdded)
        {

            listEntryData.AddTail_Tracked(tEntryData);
        }
    }
}



static void sAddMergeEntries(
    AIStateGameGlobals * pGlobals,
    ActorID idActor,
    AIStateInstanceData * ptInstData,
    TEntryDataList & listEntryData)
{
    pGlobals->nCurrTempAIEntry = 0;

    const AIStateDefinition * arDef[MAX_MERGED_DEFS];
    int32 nNumDefs = 0;
    POS pos;



    pos = ptInstData->listMergeData.GetHeadPosition();
    while (pos)
    {
        AIMergeData & tMergeData = ptInstData->listMergeData.GetNext(pos);
        if (tMergeData.snoName.snoGroup == AISTATE_SNO)
        {
            const AIStateDefinition * ptDef = (const AIStateDefinition *)gptGroupDefAIState->LockR(tMergeData.snoName.snoHandle, FALSE);
            if (ptDef)
            {
                sAddEntries(listEntryData, ptDef->arDecision[AIDECISION_TYPE_ACTION_UPDATE], tMergeData.idACDSource);

                arDef[nNumDefs] = ptDef;
                nNumDefs++;
                if (nNumDefs >= MAX_MERGED_DEFS)
                {
                    break;
                }
            }
        }
        else if (tMergeData.snoName.snoGroup == POWER_SNO)
        {






            sAddPowerEntry(listEntryData, tMergeData.snoName.snoHandle, tMergeData.idACDSource, tMergeData.idACDCastLocation, tMergeData.uPowerPriority);
        }
    }


    for (int32 i = 0; i < nNumDefs; i++)
    {
        gptGroupDefAIState->UnlockR((void *&)arDef[i]);
    }
}



static void sTryAlertPack(Actor & tActor, AIStateType eNewState, const Target & tTarget)
{



    ACDID idACD = ACDFindByActor(tActor.id);
    ActorCommonData * ptACD = ACDTryToGet(idACD);


    if (ptACD && (ptACD->eMonsterRarity == MR_CHAMPION || ptACD->eMonsterRarity == MR_RARE || ptACD->eMonsterRarity == MR_RARE_MINION))
    {


        AIStateInstanceData * ptInstData = sGetInstanceData(tActor.id);
        if (ptInstData)
        {

            if (ptInstData->eCurrState == AISTATE_TYPE_IDLE && eNewState != AISTATE_TYPE_NONE && eNewState != AISTATE_TYPE_IDLE)
            {


                ACDID idACDTarget = tTarget.GetACD();
                ActorID idActorTarget = ActorID(ACDGetOwner(idACDTarget));
                if (idActorTarget != ACTORID_INVALID)
                {
                    BrainAPIWarnPackAboutTarget(tActor.id, idActorTarget);
                }
            }
        }
    }
}




BOOL AIStateChooseSkill(
    ActorID idActor,
    BrainAttackSettings & tAttackSettings,
    AIDecisionType eDecisionType)
{

    BOOL bStateSwitch = FALSE;





    Actor * ptActor = ActorTryToGet(idActor);
    if (!ptActor || !AIStateIsActive(idActor))
    {
        return FALSE;
    }

    AIStateInstanceData * ptInstData = sGetInstanceData(idActor);


    SNO snoAIState = sGetCurrentAIStateSNO(idActor);
    ScopedAIStateDefinitionLockR pdefAIState(snoAIState);
    if (!pdefAIState)
    {
        return FALSE;
    }










    sUpdateMergeList(*ptInstData);


    const AIDecision & tDecision = pdefAIState->arDecision[eDecisionType];
    tAttackSettings.tTarget = cgTargetNone;
    tAttackSettings.snoPower = SNO_NONE;

    CTargetCache TargetCache(idActor);

    if (eDecisionType == AIDECISION_TYPE_ACTION_UPDATE &&
        ptInstData->snoOverridePower != SNO_NONE)
    {
        tAttackSettings.snoPower = ptInstData->snoOverridePower;
        tAttackSettings.tTarget.InitializeFromACD(ptInstData->idACDOverrideTarget, FALSE, FALSE);
        tAttackSettings.dwAttackWarmUp = 0;
        tAttackSettings.dwAttackWarmUpDelta = 0;

        ActorID idActorTarget = ActorID(ACDGetOwner(tAttackSettings.tTarget.idACDTarget));
        BrainAPISetTarget(idActor, idActorTarget);
        ptInstData->snoLastPower = ptInstData->snoOverridePower;
        ptInstData->idACDLastTarget = ptInstData->idACDOverrideTarget;
        AIStateClearOverridePower(idActor);
        return FALSE;
    }



    AIStateGameGlobals * pGlobals = SGameManager::GameGet().ptAIStateGameGlobals;

    Blizzard::Lock::ScopedMutex lock(pGlobals->tPoolMutex);

    TEntryDataList listEntryData;
    listEntryData.SetPool(&pGlobals->poolEntryData);
    sAddEntries(listEntryData, tDecision, ACDID_INVALID);



    if (eDecisionType == AIDECISION_TYPE_ACTION_UPDATE &&
        ptInstData->eCurrState != AISTATE_TYPE_SETUP)
    {
        sAddMergeEntries(pGlobals, idActor, ptInstData, listEntryData);
    }


    uint32 dwCurrentPriority = (uint32)-1;
    uint32 dwTotalWeight = 0;



    POS pos = listEntryData.GetHeadPosition();
    while (pos)
    {
        EntryData & tEntryData = listEntryData.GetNext(pos);
        AIEntry & tEntry = *tEntryData.ptEntry;


        if (tEntry.dwPriority < dwCurrentPriority)
        {
            if (dwTotalWeight > 0)
            {
                break;
            }

            dwCurrentPriority = tEntry.dwPriority;
        }







        if (sEvaluateEntry(*ptActor, tEntryData, TargetCache, eDecisionType))
        {

            tEntryData.uWeight = tEntry.dwWeight;
            dwTotalWeight += tEntry.dwWeight;
        }
        else
        {
            tEntryData.uWeight = 0;
        }
    }





    if (dwTotalWeight > 0)
    {
        uint32 dwRandomValue = Rand_Internal(SGameGlobalsGet().rng, dwTotalWeight - 1);
        uint32 dwCurrentWeight = 0;
        POS pos = listEntryData.GetHeadPosition();
        EntryData * ptWinningEntryData = NULL;
        while (pos)
        {
            EntryData & tEntryData = listEntryData.GetNext(pos);
            if (tEntryData.uWeight > 0)
            {
                dwCurrentWeight += tEntryData.uWeight;
                if (dwRandomValue <= dwCurrentWeight)
                {
                    ptWinningEntryData = &tEntryData;
                    break;
                }
            }
        }



        AIEntry & tWinningEntry = *ptWinningEntryData->ptEntry;
        if (tWinningEntry.snoPower != SNO_NONE)
        {
            tAttackSettings.snoPower = tWinningEntry.snoPower;
            tAttackSettings.tTarget = sGetPowerTarget(*ptWinningEntryData, TargetCache);
            tAttackSettings.idACDCastLocation = ptWinningEntryData->idACDCastLocation;


            ActorID idActorTarget = (tAttackSettings.tTarget.eType == TT_ACTOR) ? ActorID(ACDGetOwner(tAttackSettings.tTarget.idACDTarget)) : ACTORID_INVALID;



            BrainAPISetTarget(idActor, idActorTarget);

            ACDID idACD = ACDFindByActor(idActor);
            ActorCommonData * ptACD = ACDTryToGet(idACD);
            if (ptACD)
            {

                uint32 uPackCooldown = (uint32)(PowerGetFormulaValue(ptACD, AutoPowerDefinitionR(tWinningEntry.snoPower), TAG_POWER_AI_PACK_COOLDOWN, -1, -1, 0) * 60.0f);
                if (uPackCooldown > 0)
                {
                    ACDPack * ptACDPack = ACDPackTryToGet(ptACD->idACDPack);
                    if (ptACDPack)
                    {
                        const ACDIDListPooled * ptPackMembers = ptACDPack->GetMemberList();
                        POS pos2 = ptPackMembers->GetHeadPosition();
                        while (pos2)
                        {
                            ACDID idACDPackMember = (ACDID)ptPackMembers->GetNext(pos2);
                            ActorCommonData * ptACDPackMember = ACDTryToGet(idACDPackMember);
                            if (ptACDPackMember && ptACD->id != idACDPackMember)
                            {
                                PowerHelper_AddCooldown(*ptACDPackMember, tWinningEntry.snoPower, uPackCooldown, FALSE);
                            }
                        }
                    }
                }
            }
        }

        ptInstData->snoLastPower = tWinningEntry.snoPower;
        ptInstData->idACDLastTarget = tAttackSettings.tTarget.idACDTarget;

        if (sHasAIState(*ptActor, tWinningEntry.eStateSwitch))
        {
            bStateSwitch = TRUE;
        }


        if (tWinningEntry.dwCoolDownMin + tWinningEntry.dwCoolDownDelta != 0)
        {
            tAttackSettings.uAICoolDownTicks = RandRange_Internal(SGameGlobalsGet().rng, tWinningEntry.dwCoolDownMin, tWinningEntry.dwCoolDownDelta);
            tAttackSettings.uAICoolDownKey = sGetCoolDownKey(ptInstData->eCurrState, tWinningEntry, FALSE);

            if (tAttackSettings.snoPower == SNO_NONE && bStateSwitch)
            {

                AIStateActivateCooldown(idActor, tAttackSettings.uAICoolDownKey, tAttackSettings.uAICoolDownTicks, NULL);
            }
        }
        else
        {

            tAttackSettings.uAICoolDownKey = 0;
            tAttackSettings.uAICoolDownTicks = 0;
        }
        if (bStateSwitch)
        {
            sTryAlertPack(*ptActor, tWinningEntry.eStateSwitch, TargetCache.GetTarget(AISTATE_TARGET_PRIMARY_ENEMY, *ptWinningEntryData));
            AIStateSetState(idActor, tWinningEntry.eStateSwitch, FALSE);
        }

        ActorAISetMovementStyle(idActor, ptInstData->eCurrState, tWinningEntry.gbidMovementStyle);
    }
    else
    {
        ptInstData->snoLastPower = SNO_NONE;
        ptInstData->idACDLastTarget = ACDID_INVALID;
    }

    return bStateSwitch;
}



void sSetCoolUp(AIStateInstanceData & tInstData, AIStateType eStateType, AIEntry & tEntry, uint32 uCoolUp)
{
    uint32 uCoolUpKey = sGetCoolDownKey(eStateType, tEntry, TRUE);
    tInstData.mapCoolDown.SetAt_Tracked(uCoolUpKey, uCoolUp);
}







BOOL sRecalcTimerValues(
    ActorID idActor,
    AIStateInstanceData * ptInstData,
    AIStateType eStateType,
    AIStateType ePrevState)
{



    SNO snoAIState = AIStateGetSNO(idActor, eStateType);
    ScopedAIStateDefinitionLockR pdefAIState(snoAIState);
    if (!pdefAIState)
    {
        return FALSE;
    }







    AIStateGameGlobals * pGlobals = SGameManager::GameGet().ptAIStateGameGlobals;

    Blizzard::Lock::ScopedMutex lock(pGlobals->tPoolMutex);

    TEntryDataList listEntryData;
    listEntryData.SetPool(&pGlobals->poolEntryData);

    const AIDecision & tDecision = pdefAIState->arDecision[AIDECISION_TYPE_ACTION_UPDATE];
    sAddEntries(listEntryData, tDecision, ACDID_INVALID);

    for (int32 nDecision = 0; nDecision < AIDECISION_TYPE_COUNT; nDecision++)
    {
        if (nDecision == AIDECISION_TYPE_ACTION_UPDATE)
        {
            POS pos = listEntryData.GetHeadPosition();
            while (pos)
            {
                EntryData & tEntryData = listEntryData.GetNext(pos);
                AIEntry & tEntry = *tEntryData.ptEntry;
                uint32 uCoolUp = RandRange_Internal(SGameGlobalsGet().rng, tEntry.dwCoolUpMin, tEntry.dwCoolUpDelta);
                sSetCoolUp(*ptInstData, eStateType, tEntry, uCoolUp);
            }
        }
        else
        {
            const AIDecision & tDecision2 = pdefAIState->arDecision[nDecision];
            for (int32 i = 0; i < tDecision2.nEntryCount; i++)
            {
                AIEntry & tEntry = tDecision2.arEntries[i];
                uint32 uCoolUp = RandRange_Internal(SGameGlobalsGet().rng, tEntry.dwCoolUpMin, tEntry.dwCoolUpDelta);
                sSetCoolUp(*ptInstData, eStateType, tEntry, uCoolUp);
            }
        }
    }




    listEntryData.RemoveAll();
    sAddMergeEntries(pGlobals, (ActorID)0, ptInstData, listEntryData);

    if (listEntryData.Count() > 0)
    {
        BOOL bResetMergedTimers = ePrevState == AISTATE_TYPE_NONE || ((ePrevState == AISTATE_TYPE_IDLE) != (eStateType == AISTATE_TYPE_IDLE));
        uint32 dwNow = GameGetServerTime();

        POS pos = listEntryData.GetHeadPosition();
        while (pos)
        {
            EntryData & tEntryData = listEntryData.GetNext(pos);
            AIEntry & tEntry = *tEntryData.ptEntry;
            uint32 uCoolUp;

            if (bResetMergedTimers)
            {
                uCoolUp = RandRange_Internal(SGameGlobalsGet().rng, tEntry.dwCoolUpMin, tEntry.dwCoolUpDelta);
            }
            else
            {

                uCoolUp = sGetCoolUp(*ptInstData, tEntry);


                uint32 dwTimeSinceStart = dwNow - ptInstData->dwStateEntryTime;
                uCoolUp = uCoolUp > dwTimeSinceStart ? uCoolUp - dwTimeSinceStart : 0;


                uint32 uCoolDown = sGetCoolDown(*ptInstData, tEntry);
                uCoolDown = uCoolDown > dwNow ? uCoolDown - dwNow : 0;
                uint32 uCoolDownKey = sGetCoolDownKey(eStateType, tEntry, FALSE);
                sSetCoolDown(*ptInstData, uCoolDownKey, uCoolDown);
            }
            sSetCoolUp(*ptInstData, eStateType, tEntry, uCoolUp);
        }
    }

    return TRUE;
}



static void sSetIdleAnim(ActorID idActor, uint32 dwAnimTag, uint32 dwTransitionAnimTag)
{

    ACDID idACD = ActorGet(idActor)->idACD;
    if (idACD != ACDID_INVALID)
    {
        ActorCommonData & tACD = ACDGet(idACD);
        if (AnimPlayerAnimExists(tACD, dwAnimTag))
            BrainStanceSetIdleAnimation(idActor, dwAnimTag);
        if (AnimPlayerAnimExists(tACD, dwTransitionAnimTag))
            BrainAPIDoPlayAnimation(idActor, dwTransitionAnimTag, (uint32)-1, FALSE, FALSE, 0.0f, FALSE, 1.0f);
    }
}













static ActorAnimationTag sGetIdleAnim(ActorID idActor, AIStateType eStateType)
{

    ActorAnimationTag eTag = TAG_ACTOR_ANIMATION_NEUTRAL;
    if (eStateType == AISTATE_TYPE_SETUP || eStateType == AISTATE_TYPE_IDLE)
    {
        eTag = TAG_ACTOR_ANIMATION_UNALERT_NEUTRAL;


        ACDID idACD = ActorGet(idActor)->idACD;
        ActorCommonData * ptACD = ACDTryToGet(idACD);
        if (ptACD)
            ptACD->SetCombatStatus(FALSE);
    }
    return eTag;
}







BOOL AIStateSetState(ActorID idActor, AIStateType eStateType, BOOL bForceAnimSet)
{
    if (eStateType >= 0 && eStateType < AISTATE_TYPE_COUNT)
    {
        AIStateInstanceData * ptInstData = sGetInstanceData(idActor);
        if (ptInstData && ptInstData->arAIStateTable[eStateType] != SNO_NONE)
        {

            AIStateType ePrevState = ptInstData->eCurrState;
            if (!sRecalcTimerValues(idActor, ptInstData, eStateType, ePrevState))
                return FALSE;

            ptInstData->dwStateEntryTime = GameGetServerTime();
            ptInstData->eCurrState = eStateType;

            ActorAnimationTag ePrevIdleAnim = sGetIdleAnim(idActor, ePrevState);
            ActorAnimationTag eNewIdleAnim = sGetIdleAnim(idActor, eStateType);
            if (bForceAnimSet || ePrevIdleAnim != eNewIdleAnim)
            {
                uint32 dwTransitionAnimTag;
                if (ePrevIdleAnim == TAG_ACTOR_ANIMATION_NEUTRAL && eNewIdleAnim == TAG_ACTOR_ANIMATION_UNALERT_NEUTRAL)
                    dwTransitionAnimTag = TAG_ACTOR_ANIMATION_ALERT_TO_UNALERT;
                else if (ePrevIdleAnim == TAG_ACTOR_ANIMATION_UNALERT_NEUTRAL && eNewIdleAnim == TAG_ACTOR_ANIMATION_NEUTRAL)
                    dwTransitionAnimTag = TAG_ACTOR_ANIMATION_UNALERT_TO_ALERT;
                else
                    dwTransitionAnimTag = (uint32)-1;
                sSetIdleAnim(idActor, eNewIdleAnim, dwTransitionAnimTag);
            }

            if (eStateType >= AISTATE_TYPE_AGGRESSIVE && eStateType != AISTATE_TYPE_SETUP && ptInstData->dwAggressiveStartTime == 0)
                ptInstData->dwAggressiveStartTime = ptInstData->dwStateEntryTime;

            BrainConditionSetCondition(idActor, BRAIN_CONDITION_ABORTED);
            BrainAPIDoIdle(idActor);
            return TRUE;
        }
    }
    return FALSE;
}




















BOOL AIStateSetStateData(ActorID idActor, AIStateType eStateType, SNO snoAIState)
{
    if (eStateType >= 0 && eStateType < AISTATE_TYPE_COUNT)
    {
        AIStateInstanceData * ptInstData = sGetInstanceData(idActor);
        if (ptInstData != NULL)
        {
            ptInstData->arAIStateTable[eStateType] = snoAIState;
            if (snoAIState != SNO_NONE && sRecalcTimerValues(idActor, ptInstData, eStateType, AISTATE_TYPE_NONE))
                return TRUE;
        }
    }
    return FALSE;
}

BOOL ACDIsFollower(const ActorCommonData & tACD);  // ActorCommonData.cpp:8206
void PlayerRemovePet(ACDID idACDPlayer, ACDID idACDPet, BOOL bDisplayChatMessage);  // Player.cpp


























static void sEscortBuffRemoved(PowerAPIBuffInstance * ptBuff, BuffRemovalReason, void * ptData)
{

    ActorCommonData * ptACD = ACDTryToGet(ptBuff->idACDAffected);
    if (ptACD)
    {


        uint32 uDynamicFlags = (uint32)(uintptr_t)ptData;
        ptACD->tCollSettings.uDynamicMyFlags = uDynamicFlags;
        ptACD->ModifiedMark(ACDMODIFIED_COLL_FLAGS);
    }
}








BOOL AIStateSetStateData(ActorID idActor, AIBehaviorType eBehaviorType)
{


    BOOL bSuccess = FALSE;

    ACDID idACD = ACDFindByActor(idActor);
    ActorCommonData * ptACD = ACDTryToGet(idACD);
    AIStateInstanceData * ptInstData = sGetInstanceData(idActor);
    if (ptInstData == NULL || ptACD == NULL)
        return bSuccess;

    SNO snoEscortingBuff = GlobalSNOGet(GLOBALSNO_POWERS_ESCORTING_BUFF);
    if (snoEscortingBuff != SNO_NONE)
    {
        if (eBehaviorType == AIBEHAVIOR_TYPE_ESCORT_FOLLOW || eBehaviorType == AIBEHAVIOR_TYPE_ESCORT_LEAD)
        {
            PowerAPIBuffStartData tBuffStartData(POWERID_INVALID);
            tBuffStartData.bInfinite = TRUE;
            tBuffStartData.snoPower = snoEscortingBuff;
            tBuffStartData.eStackType = 1;
            FastAttribSetValueInt(tBuffStartData.idFastAttrib, ATTRIBUTE_NPC_IS_ESCORTING, TRUE);
            uint32 uDynamicFlags = ptACD->tCollSettings.uDynamicMyFlags;
            if (uDynamicFlags & 1)
            {
                tBuffStartData.pRemovalEffect = sEscortBuffRemoved;
                tBuffStartData.pRemovalEffectData = (void *)(uintptr_t)uDynamicFlags;
                ptACD->tCollSettings.uDynamicMyFlags = uDynamicFlags & ~1;
                ptACD->ModifiedMark(ACDMODIFIED_COLL_FLAGS);
            }
            PowerAPI_BuffAddToACD(tBuffStartData, *ptACD);
        }
        else
        {
            BuffID idBuff = PowerAPI_BuffFindOnACD(snoEscortingBuff, *ptACD, ACDID_INVALID);
            if (idBuff != BUFFID_INVALID)
                PowerAPI_BuffFlagForDeletion(idBuff, FALSE, TRUE, 0, TRUE);
            ptACD->AttributesSetInt(FastAttribKey(ATTRIBUTE_FOLLOW_TARGET_ACDID), ACDID_INVALID);
            if (ACDIsFollower(*ptACD))
            {
                ACDID idACDOwner = ACDGetPetOwner(idACD);
                if (idACDOwner != ACDID_INVALID)
                    PlayerRemovePet(idACDOwner, idACD, TRUE);
            }
        }
    }

    SNO snoMonster = ptACD->GetMonsterSNO();
    if (snoMonster != SNO_NONE)
    {
        const MonsterDefinition * ptDefMonster = (const MonsterDefinition *)gptGroupDefMonster->LockR(snoMonster, FALSE);
        if (ptDefMonster)
        {
            SNO snoBehavior = ptDefMonster->arAIBehaviors[eBehaviorType];
            if (snoBehavior != SNO_NONE)
            {
                const AIBehaviorDefinition * pdefBehavior = (const AIBehaviorDefinition *)gptGroupDefAIBehavior->LockRFast(snoBehavior);
                if (pdefBehavior && pdefBehavior->arAIState[AISTATE_TYPE_IDLE] != SNO_NONE)
                {
                    for (int32 i = 0; i < AISTATE_TYPE_COUNT; i++)
                        AIStateSetStateData(idActor, i, pdefBehavior->arAIState[i]);

                    if (ptInstData->eBehaviorType != eBehaviorType)
                    {
                        sSetIdleAnim(idActor, sGetIdleAnim(idActor, AISTATE_TYPE_IDLE), (uint32)-1);
                        ptInstData->eBehaviorType = eBehaviorType;
                        if (pdefBehavior->arAIState[ptInstData->eCurrState] == SNO_NONE)
                            AIStateSetState(idActor, AISTATE_TYPE_IDLE, FALSE);
                    }
                    bSuccess = TRUE;
                }
            }
            gptGroupDefMonster->UnlockR((void *&)ptDefMonster);
        }
    }
    return bSuccess;
}

















































BOOL AIStateSetMovementStyle(ActorID idActor, AIStateType eStateType, GBID gbidMovementStyle)
{
    if (eStateType >= 0 && eStateType < AISTATE_TYPE_COUNT)
    {
        AIStateInstanceData * ptInstData = sGetInstanceData(idActor);
        if (ptInstData != NULL)
            ptInstData->arAIMovementStyles[eStateType] = gbidMovementStyle;
    }
    return FALSE;
}


// AIStateFillDebugString (line 5769, 0xac8 bytes in the DWARF) was removed by the linker (DW_AT_low_pc 0), so its body is
// not in the image. Only the part the DWARF records is reconstructed here: the locals and inlined calls below sit on
// their recorded lines; the CRefString formatting of strMsg (strTemp, strTitle, strPower, strState, strOverride,
// strMerge, lines 5807-5940) is not recoverable and is left out. The two calls on lines 5874-5875 are inferred from
// the locals shared with AIStateChooseSkill (pGlobals, lock, listEntryData, tDecision); this second caller is what
// keeps sAddMergeEntries out of line in the original.
BOOL AIStateFillDebugString(
    ActorID idActor,
    char * strMsg,
    int32 nMaxStringLength)
{






    ACDID idACD = ACDFindByActor(idActor);





    ActorCommonData * ptACD = ACDTryToGet(idACD);
    if (!ptACD)
    {
        return FALSE;
    }

    AIStateInstanceData * ptInstData = sGetInstanceData(idActor);
    if (!ptInstData)
    {
        return FALSE;
    }

    SNO snoAIState = sGetCurrentAIStateSNO(idActor);
    ScopedAIStateDefinitionLockR pdefAIState(snoAIState);
    if (!pdefAIState)
    {
        return FALSE;
    }































































    AIStateGameGlobals * pGlobals = SGameManager::GameGet().ptAIStateGameGlobals;

    Blizzard::Lock::ScopedMutex lock(pGlobals->tPoolMutex);

    TEntryDataList listEntryData;
    listEntryData.SetPool(&pGlobals->poolEntryData);

    const AIDecision & tDecision = pdefAIState->arDecision[AIDECISION_TYPE_ACTION_UPDATE];
    sAddEntries(listEntryData, tDecision, ACDID_INVALID);
    sAddMergeEntries(pGlobals, idActor, ptInstData, listEntryData);

































































    return listEntryData.Count() > 0;






}






































BOOL AIStateOnHitReaction(const ACDID idACD)
{
    return FALSE;
}







































AIStateType AIStateGetCurrState(ActorID idActor)
{
    AIStateType eState = AISTATE_TYPE_NONE;
    AIStateInstanceData * ptInstData = sGetInstanceData(idActor);
    if (ptInstData != NULL)
        eState = ptInstData->eCurrState;
    return eState;
}







void AIStateSetActive(ActorID idActor, BOOL bActive)
{
    AIStateInstanceData * ptInstData = sGetInstanceData(idActor);
    if (ptInstData != NULL)
    {
        if (!bActive)
            ptInstData->dwFlags |= (1 << AISTATE_FLAG_INACTIVE);
        else
            ptInstData->dwFlags &= ~(1 << AISTATE_FLAG_INACTIVE);
    }
}


BOOL AIStateIsActive(ActorID idActor)
{

    AIStateInstanceData * ptInstData = sGetInstanceData(idActor);
    if (ptInstData != NULL)
    {
        return (ptInstData->dwFlags & (1 << AISTATE_FLAG_INACTIVE)) == 0;
    }
    return FALSE;
}




AIBehaviorType AIStateGetBehaviorType(ActorID idActor)
{
    AIBehaviorType eBehaviorType = AIBEHAVIOR_TYPE_NONE;
    AIStateInstanceData * ptInstData = sGetInstanceData(idActor);
    if (ptInstData != NULL)
        eBehaviorType = ptInstData->eBehaviorType;
    return eBehaviorType;
}






void AIStateSetOverridePower(ActorID idActor, SNO snoPower, ACDID idACDTarget)
{
    AIStateInstanceData * ptInstData = sGetInstanceData(idActor);
    if (ptInstData != NULL)
    {
        ptInstData->snoOverridePower = snoPower;
        ptInstData->idACDOverrideTarget = idACDTarget;
    }
}






void AIStateClearOverridePower(ActorID idActor)
{
    AIStateInstanceData * ptInstData = sGetInstanceData(idActor);
    if (ptInstData != NULL)
    {
        ptInstData->snoOverridePower = SNO_NONE;
        ptInstData->idACDOverrideTarget = ACDID_INVALID;
    }
}




static void sAddToMergeList(ActorID idActor, SNOName & snoName, uint32 uLifetime, ACDID idACDSource, ACDID idACDCastLocation, uint32 uPowerPriority, BuffID idBuff)
{
    AIStateInstanceData * ptInstData = sGetInstanceData(idActor);
    if (ptInstData == NULL)
        return;

    uint32 uNow = GameGetServerTime();
    uint32 uEndTime = 0;
    if (uLifetime != 0)
        uEndTime = uNow + uLifetime;

    // Refresh an existing merge from the same source.
    POS pos = ptInstData->listMergeData.GetHeadPosition();
    while (pos)
    {
        AIMergeData & tMergeData = ptInstData->listMergeData.GetNext(pos);
        if (tMergeData.idACDSource == idACDSource && snoName == tMergeData.snoName && tMergeData.idACDCastLocation == idACDCastLocation)
        {
            tMergeData.uEndTime = uEndTime;
            return;
        }
    }

    AIMergeData tMergeData;
    SigmaMemoryClear(&tMergeData, sizeof(tMergeData));
    tMergeData.snoName = snoName;
    tMergeData.uPowerPriority = uPowerPriority;
    tMergeData.uEndTime = uEndTime;
    tMergeData.idACDSource = idACDSource;
    tMergeData.idACDCastLocation = idACDCastLocation;
    tMergeData.idBuff = idBuff;
    ptInstData->listMergeData.AddTail_Tracked(tMergeData);

    if (tMergeData.snoName.snoGroup == AISTATE_SNO && tMergeData.snoName.snoHandle != SNO_NONE)
    {
        ScopedAIStateDefinitionLockR pdefAIState(tMergeData.snoName.snoHandle);
        const AIDecision & tDecision = pdefAIState->arDecision[AIDECISION_TYPE_ACTION_UPDATE];
        for (int32 i = 0; i < tDecision.nEntryCount; i++)
        {
            for (int32 nStateType = 0; nStateType < AISTATE_TYPE_COUNT; nStateType++)
            {
                AIEntry & tEntry = tDecision.arEntries[i];
                uint32 uCoolUp = RandRange_Internal(SGameGlobalsGet().rng, tEntry.dwCoolUpMin, tEntry.dwCoolUpDelta);
                sSetCoolUp(*ptInstData, nStateType, tEntry, uCoolUp);
            }
        }
    }
}

































void AIStateMerge(ActorID idActor, SNO snoAIState, uint32 uLifetime, ACDID idACDSource)
{
    SNOName snoName(AISTATE_SNO, snoAIState);
    sAddToMergeList(idActor, snoName, uLifetime, idACDSource, ACDID_INVALID, 0, -1);
}







void AIStateMerge(ActorID idActor, SNO snoAIState, uint32 uLifetime, BuffID idBuff)
{
    SNOName snoName(AISTATE_SNO, snoAIState);
    sAddToMergeList(idActor, snoName, uLifetime, ACDID_INVALID, ACDID_INVALID, 0, idBuff);
}







void AIStateMergeBroadcastPower(ActorID idActor, SNO snoPower, uint32 uLifetime, ACDID idACDSource, ACDID idACDCastLocation, uint32 uPriority)
{
    SNOName snoName(POWER_SNO, snoPower);
    sAddToMergeList(idActor, snoName, uLifetime, idACDSource, idACDCastLocation, uPriority, -1);
}









BOOL AIStateHasExistingMerge(ActorID idActor, ACDID idACDSourceIgnore)
{
    AIStateInstanceData * ptInstData = sGetInstanceData(idActor);
    if (ptInstData != NULL)
    {
        uint32 uNow = GameGetServerTime();
        POS pos = ptInstData->listMergeData.GetHeadPosition();
        while (pos)
        {
            AIMergeData & tMergeData = ptInstData->listMergeData.GetNext(pos);
            if (tMergeData.idACDSource != ACDID_INVALID && tMergeData.idACDSource != idACDSourceIgnore)
            {
                if (tMergeData.uEndTime >= uNow || tMergeData.uEndTime == 0)
                    return TRUE;
            }
        }
    }
    return FALSE;
}







void AIStateRemoveMergesFromBuff(ActorID idActor, BuffID idBuff)
{
    AIStateInstanceData * ptInstData = sGetInstanceData(idActor);
    if (ptInstData != NULL)
    {
        POS pos = ptInstData->listMergeData.GetHeadPosition();
        while (pos)
        {
            POS posLast = pos;
            AIMergeData & tMergeData = ptInstData->listMergeData.GetNext(pos);
            if (tMergeData.idBuff == idBuff)
                ptInstData->listMergeData.RemoveAt(posLast);
        }
    }
}







void AIStateActivateCooldown(Power & tPower)
{
    ActorCommonData * ptACD = ACDTryToGet(tPower.idACD);
    if (ptACD)
        AIStateActivateCooldown((ActorID)ACDGetOwner(tPower.idACD), tPower.uAICoolDownKey, tPower.uAICoolDownTicks, ptACD);
}







void AIStateActivateCooldown(ActorID idActor, uint32 uAICoolDownKey, uint32 uAICoolDownTicks, ActorCommonData * ptACD)
{
    if (uAICoolDownTicks > 0)
    {
        if (ptACD == NULL)
        {
            Actor * ptActor = ActorTryToGet(idActor);
            if (ptActor)
                ptACD = ACDTryToGet(ptActor->idACD);
        }

        float flCooldownRedux = ptACD ? ptACD->AttributesGetFloat(ATTRIBUTE_AI_COOLDOWN_REDUCTION_PERCENT) : 0.0f;
        if (flCooldownRedux != 0.0f)
        {
            float flCooldownTicks = (float)uAICoolDownTicks * Math::Pin(1.0f - flCooldownRedux, 0.1f, 2.0f);
            uAICoolDownTicks = (uint32)flCooldownTicks;
        }

        AIStateInstanceData * ptInstData = sGetInstanceData(idActor);
        if (ptInstData != NULL)
            sSetCoolDown(*ptInstData, uAICoolDownKey, uAICoolDownTicks);
    }
}










BOOL AIStateIsKeyOnCooldown(ActorID idActor, uint32 uAICooldownKey)
{
    AIStateInstanceData * ptInstData = sGetInstanceData(idActor);
    if (ptInstData != NULL)
    {
        uint32 uCoolDown = 0;
        ptInstData->mapCoolDown.Lookup(uAICooldownKey, uCoolDown);
        if (uCoolDown > GameGetServerTime())
            return TRUE;
    }
    return FALSE;
}

