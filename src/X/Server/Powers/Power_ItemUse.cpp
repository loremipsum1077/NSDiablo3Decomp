// Original file: X/Server/Powers/Power_ItemUse.cpp
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/Achievements/AchievementDefines.h"
#include "../../Common/ActorCommonData.h"
#include "../../Common/ActorCommonDataSwig.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/Definitions/ActorDefinition.h"
#include "../../Common/Definitions/MonsterDefinition.h"
#include "../../Common/Game.h"
#include "../../Common/Marker/Marker.h"
#include "../../Common/Networking/GameMessage.h"
#include "../../Common/Player.h"
#include "../PlayerList.h"
#include "../SGame.h"
#include "../SScene.h"
#include "../../Common/TagMaps/Tags.h"
#include "../../Common/TagMaps/ActorTags.h"
#include "../../Common/Target.h"
#include "../../Common/Items.h"
#include "../../Sigma/Math/MathGeneral.h"
#include "../../Common/Attributes/FastAttribConstantsSwig.h"
#include "../../Common/GlobalSNO.h"
#include "../AI/AIStates.h"
#include "../Actors/Actors.h"
#include "../Achievements/AchievementModifierEvaluationLogic.h"
#include "../Crafting/SItemCrafting.h"
#include "PowerAPI.h"
#include "Powers.h"

// ItemUseResult is declared in X/Server/Powers/Power_ItemUse.h:20 (DWARF enum), a header that is not in src/.
enum ItemUseResult {
    ITEM_USE_RESULT_INVALID = -1,
    ITEM_USE_RESULT_CONSUMED = 0,
    ITEM_USE_RESULT_OK = 1,
    ITEM_USE_RESULT_CANT_USE = 2,
};

SNO ItemGetUsePower(const GBID gbidItem);  // Items.cpp:2434
ItemUseResult ItemUse(const PowerID idPower, const ACDID idACDUser, const ACDID idACDItemToUse, const Target * ptTarget);  // Power_ItemUse.cpp:1836
void sSummonMonsterForShrineOrScrollOfCallingGuts(ActorCommonData & tACDPlayer);  // Power_ItemUse.cpp:1098
void SACDInventoryEvaluateItemDyeAchievements(Achievement::EvaluationData * ptEvaluationData);  // ActorInventory.cpp:12580
#include "PowerHelper.h"
#include "../SCosmeticItems.h"
#include "../SGameStats.h"
#include "../../Common/ActorInventory.h"
GameError ItemCanBeUsed(const ACDID idACDPlayer, const ActorCommonData & tACDItemToUse, const Target * ptTarget);  // Items.cpp:2594
void SQuestsOnItemUsed(ACDID idACDPlayer, ACDID idACDItem);  // SQuests.cpp:4657
BOOL SItemCrafting_TryUnlockSecondaryTransmogs(GBID gbidTransmog, Player & tPlayer);  // SItemCrafting.cpp:386
void SItemCrafting_LearnRecipe(const ACDID idACDPlayer, const GBID gbidRecipe);  // SItemCrafting.cpp:2048
namespace Tutorial { BOOL SendTutorialToClient(SNO snoTutorial, Player * ptPlayer, const char * szUIAnchorName, const ACDID idACDTarget); }  // Tutorials.cpp:215
BOOL ACDIsInTownPortalArea(const ActorCommonData & tACD);  // ActorCommonData.cpp:4446
SNO ItemUseGetScrollPower(const ActorCommonData & tACDItem);  // Items.cpp:2543
#include "../SWorld.h"
#include "../Quests/SQuests.h"
#include "../../Common/Definitions/LevelAreaDefinition.h"
BOOL SItemConsumableAddSockets(ActorCommonData & tACDItem, ActorCommonData & tACDOwner, const ActorCommonData & tACDItemToUse);  // SItems.cpp:1355
BOOL ACDInSameWorld(const ActorCommonData & tACD1, const ActorCommonData & tACD2);  // ActorCommonData.cpp:2503
#include "../SPlayer.h"
ACDID TownPortalFindLocationMarkerACD(BOOL bCausedByLocalPlayer);  // TownPortal.cpp:110
void ActorChangeWorld(const ActorID idActor, const WorldPlace & tPlace, const Quaternion & q);  // Actors.cpp:6093
BOOL STeamGetStartLocation(const SWorldID idSWorld, const Team eTeam, WorldPlace & tPlace);  // TeamCommon.cpp:543
BOOL SPlayerFindValidWalkableSpotNearPlayer(const ActorCommonData & tACDPlayer, WorldPlace & placeSpot);  // SPlayer.cpp:12152
ActorID GizmoAddBySNO(int32 nLevel, ActorAddStruct & tActorAdd);  // Actors.cpp:5803
enum { GB_INVALID = -1 };  // X/Common/Definitions/GameBalanceDefinition.h:34 (kept local)
void SPortalSpecifierInitialize(Actor * ptActor, const ResolvedPortalDestination & tDest);  // SPortal.cpp:154
#include "../Loot/Loot.h"
#include "../../Common/Definitions/TreasureClassDefinition.h"
void FlippiesCreateOnActor(const ActorCommonData & tACDSpawningActor, const ACDID idACDAssigned, const LootSpecifierArray & arLootSpecifiersToSpawn, const uint32 dwFlippyFlags, uint32 dwGameEventNumber, float flCameraBiasDist, const ACDID idACDKiller, const LooteeInfo * ptLootee, const Vector3D * pvForcedPositionBias, const LootContext eLootContext, XPooledDataIDList * plistSpawnedItemACDs);  // Flippies.cpp:958
#include "../../Sigma/MemoryPools.h"
#include "../../Common/Nav/NavigationMesh.h"
#include "../../Common/CombatLog.h"
#include "../Actors/ActorEffect.h"
#include "../../Common/Definitions/GlobalDefinition.h"
#include "../../Common/Actors/ActorEffectCommon.h"
enum { FLOATING_NUMBER_TYPE_NEPHALEM_GLORY_GAINED = 37, FLOATING_NUMBER_TYPE_NEPHALEM_GLORY_EXTRA_TIME = 38 };  // X/Common/Player.h:352 (kept local)
void SGameSendDungeonFinderProgressToPlayer(Player & tPlayer);  /* SGame.cpp:8491 */  void SPlayerRecordHealthGlobePickedUp(const ACDID idACDPlayer, const float flAmountHealed);  /* SPlayer.cpp:4729 */  void PowerAPI_RecomputeHitpointHealTarget(ActorCommonData * ptACD);  // PowerAPI.cpp:7366
#include "../LuaScript/ScriptEvent.h"
#include "../../Common/GameConstants.h"
static void sDrinkHealth(
    PowerID idPower,
    ACDID idACDSource,
    ActorCommonData & tACDUser,
    const float flAmount,
    const int32 nDuration,
    const uint32 eHealType)
{
    ACDPlayEffect(tACDUser.id, EFFECT_HEAL, 0, ACDID_INVALID);

    int32 nHealDuration = nDuration;


    float flFasterHealingPercent = tACDUser.AttributesGetFloat(ATTRIBUTE_FASTER_HEALING_PERCENT);
    if (flFasterHealingPercent > EPSILON)
    {
        nHealDuration = Math::FloatToInt((float)nDuration / (flFasterHealingPercent + 1.0f));
    }

    float flHealSeconds = nHealDuration * OO_TICKS_PER_SECOND;
    if (flHealSeconds < EPSILON || tACDUser.IsPet())
    {
        tACDUser.Heal(flAmount, TRUE);
    }
    else
    {
        float flRegenPerSecond = flAmount / flHealSeconds;

        PowerAPIBuffStartData tBuffStartData(idPower);
        tBuffStartData.eStackType = BUFF_STACKTYPE_ALWAYS;
        tBuffStartData.dwDurationMin = nDuration;
        tBuffStartData.nMaxStackCount = 128;
        tBuffStartData.bPositiveEffect = true;
        tBuffStartData.bUndispellable = true;
        FastAttribSetValueFloat(tBuffStartData.idFastAttrib, ATTRIBUTE_HITPOINTS_REGEN_PER_SECOND_HEALTH_GLOBE, flRegenPerSecond);
        PowerAPI_BuffAddToACD(tBuffStartData, tACDUser);

        PowerAPI_RecomputeHitpointHealTarget(&tACDUser);
    }

    CombatLog::LogEvent(
        CombatLog::LOG_EVENT_HEALING_DONE,
        idACDSource,
        tACDUser.id,
        PowerGetSNO(idPower),
        flAmount,
        0.0f,
        eHealType);
}




static ItemUseResult sUseHealthPotion(
    const ACDID idACDUser,
    const ACDID idACDItem,
    const PowerID idPower)
{
    ActorCommonData & tACDUser = ACDGet(idACDUser);
    ActorCommonData & tACDItem = ACDGet(idACDItem);

    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());


    float flAlliesAffectedPercent = tACDUser.AttributesGetFloat(ATTRIBUTE_HEALTH_POTION_AFFECTS_ALLIES_PERCENT);
    if (flAlliesAffectedPercent > 0.0f)
    {

        NavPlace tNavPlace;
        ACDGetNavMeshHandleWithPlace(tACDUser, tNavPlace.tNavMeshHandle, tNavPlace.tPlace);

        PowerAPIChooseTargets tChooseTargets;
        PowerHelper_ChooseTargetsInit(idPower, tChooseTargets);
        PowerHelper_ChooseTargetsInitGeometrySphere(tNavPlace.tPlace, 50.0f, tChooseTargets.tGeometry);
        tChooseTargets.dwFlags |= (1 << CTF_DONT_PERFORM_LOS_CHECK);
        PowerAPI_DoFindAllPossibleTargets(tChooseTargets, listTargets);
    }
    else
    {

        listTargets.AddTail_Tracked(idACDUser);
    }


    const GameBalanceDefinition * pdefGB;
    ItemDefinition * ptItemDef = (ItemDefinition *)GBLockR(tACDItem.hGB, pdefGB, TRUE);
    ScopedActorDefinitionLockR pdefActor(ptItemDef->snoActor);
    SNO snoPower = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_POWERUP_POWER);
    GBUnlockR(pdefGB);

    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
        if (!ACDIsPlayer(idACDTarget))
        {
            continue;
        }

        ActorCommonData & tACDTarget = ACDGet(idACDTarget);
        if (!ACDIsFriendly(tACDUser, tACDTarget, FALSE))
        {
            continue;
        }

        // Inference: the snoPower test is hoisted out of the loop (cmn at 170, loop unswitched), so it is part of this condition.
        if (snoPower != SNO_NONE && SNOFileExists(POWER_SNO, snoPower))
        {
            PowerStartData tPowerStartData;
            PowerStartDataInit(tPowerStartData);
            tPowerStartData.idACD = idACDTarget;
            tPowerStartData.idACDAlternateSource = idACDTarget;
            tPowerStartData.snoPower = snoPower;
            tPowerStartData.ptExtraPowerStartData = (void *)(intptr_t)tACDItem.id;
            tPowerStartData.tTarget.Clear();
            PowerStartEx(tPowerStartData, -1, FALSE, FALSE);
        }

        ScopedGlobalDefinitionLockR pdefGlobal;
        float flHealHPPercent = pdefGlobal->flHealthPotionHealPercent + tACDTarget.AttributesGetFloat(ATTRIBUTE_HEALTH_POTION_BONUS_HEAL_PERCENT);
        float flHealHP = flHealHPPercent * tACDTarget.GetHitPointsMaxTotal();
        flHealHP += tACDTarget.AttributesGetFloat(ATTRIBUTE_HEALTH_GLOBE_BONUS_HEALTH);


        if (idACDTarget != idACDUser)
        {
            flHealHP *= flAlliesAffectedPercent;
        }

        ACDPlayEffect(idACDTarget, EFFECT_HEAL, 0, ACDID_INVALID);

        tACDTarget.Heal(flHealHP, TRUE);

        CombatLog::LogEvent(
            CombatLog::LOG_EVENT_HEALING_DONE,
            idACDUser,
            idACDTarget,
            PowerGetSNO(idPower),
            flHealHP,
            0.0f,
            HEAL_TYPE_POTION);
    }

    BOOL bBottomless = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_ITEM_IS_BOTTOMLESS);
    if (!bBottomless && !GameTypeIsPvP() && !tACDUser.IsDueling())
    {
        return ITEM_USE_RESULT_CONSUMED;
    }


    return ITEM_USE_RESULT_OK;

}


ACDID sAddEffectProxy(
    const WorldPlace & placeSpawn,
    float flDuration)
{
    ACDID idACDProxy = ACDID_INVALID;
    SNO snoProxyActor = GlobalSNOGet(GLOBALSNO_ACTOR_GENERIC_PROXY);
    if (snoProxyActor != SNO_NONE)
    {
        ActorAddStruct tActorAdd;
        tActorAdd.snoActor = snoProxyActor;
        // Inference: eTeam 0 is TEAM_SCENERY (str wzr at +0x58); flags 0x4d5 = bits 0,2,4,6,7,10.
        tActorAdd.eFastAttribType = FAT_SIMPLE;
        tActorAdd.eTeam = TEAM_SCENERY;
        tActorAdd.tPlace = placeSpawn;
        tActorAdd.qOrientation = cgqIdentity;
        tActorAdd.flScale = 1.0f;
        tActorAdd.tSNOName = SNOName(ACTOR_SNO, tActorAdd.snoActor);
        tActorAdd.dwActorAddFlags = (1 << ACTORADD_BRAINLESS_BIT) | (1 << ACTORADD_DOESNT_WALK_BIT) | (1 << ACTORADD_EVEN_WHEN_STUCK) |

                                    (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION) | (1 << ACTORADD_DOESNT_DEACTIVATE);

        ActorID idActorProxy = ActorAdd(tActorAdd);
        if (idActorProxy == ACTORID_INVALID)
        {
            return ACDID_INVALID;
        }

        idACDProxy = ACDFindByActor(idActorProxy);
        if (idACDProxy != ACDID_INVALID)
        {
            ActorCommonData & tACDProxy = ACDGet(idACDProxy);
            tACDProxy.AttributesSetInt(ATTRIBUTE_DROPS_NO_LOOT, 1);
            tACDProxy.AttributesSetInt(ATTRIBUTE_GRANTS_NO_XP, 1);
            tACDProxy.AttributesSetInt(ATTRIBUTE_INVULNERABLE, 1);
            ActorFlagSet(idActorProxy, ACTOR_DOESNT_DEACTIVATE_BIT, TRUE);
            tACDProxy.AttributesSetInt(ATTRIBUTE_REMOVES_BODY_ON_DEATH, 1);
            tACDProxy.AttributesSetInt(ATTRIBUTE_IS_POWER_PROXY, 1);

            uint32 dwDuration = Math::FloatToInt(flDuration * 60.0f);

            ActorDeleteSoon(idActorProxy, dwDuration);
        }
    }

    return idACDProxy;
}




static ItemUseResult sUseHealthGlyph(
    PowerID idPower,
    const ACDID idACDUser,
    const ACDID idACDItem)
{
    if (idACDUser == ACDID_INVALID)
    {
        return ITEM_USE_RESULT_INVALID;
    }

    ActorCommonData & tACDItem = ACDGet(idACDItem);
    ActorCommonData & tACDUser = ACDGet(idACDUser);
    float flHealSeconds = tACDItem.AttributesGetInt(ATTRIBUTE_HITPOINTS_GRANTED_DURATION) * OO_TICKS_PER_SECOND;
    float flBaseHealHP = tACDItem.AttributesGetFloat(ATTRIBUTE_HITPOINTS_GRANTED);
    float flPercentHeal = tACDItem.AttributesGetFloat(ATTRIBUTE_HITPOINTS_PERCENT);


    NavPlace tNavPlace;
    ACDGetNavMeshHandleWithPlace(tACDUser, tNavPlace.tNavMeshHandle, tNavPlace.tPlace);

    PowerAPIChooseTargets tChooseTargets;
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerHelper_ChooseTargetsInit(idPower, tChooseTargets);
    PowerHelper_ChooseTargetsInitGeometrySphere(tNavPlace.tPlace, 50.0f, tChooseTargets.tGeometry);
    tChooseTargets.dwFlags |= (1 << CTF_DONT_PERFORM_LOS_CHECK);
    PowerAPI_DoFindAllPossibleTargets(tChooseTargets, listTargets);

    ScopedGlobalDefinitionLockR pdefGlobal;

    float flFollowerPercent = pdefGlobal->ptServerData->flFollowerHealthGlobeBonusPercent;

    Player * ptPlayerHealing = ACDGetControllingPlayer(idACDUser);

    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);

        ActorCommonData & tACDTarget = ACDGet(idACDTarget);

        if (ACDAffectedByHealthGlobes(tACDTarget) &&
            ACDIsFriendly(tACDTarget, tACDUser, FALSE))
        {

            if (flHealSeconds > EPSILON)
            {
                float flPercentPerSecond = flPercentHeal / flHealSeconds;
                float flBonusPercentPerSecond = tACDTarget.AttributesGetFloat(ATTRIBUTE_BONUS_HEALTH_PERCENT_PER_SECOND_FROM_GLOBES_TOTAL);
                flPercentPerSecond += flBonusPercentPerSecond;
                if (flPercentPerSecond > EPSILON)
                {
                    flHealSeconds = flPercentHeal / flPercentPerSecond;
                }
            }


            if (GameTypeIsPvP())
            {
                flHealSeconds = 0.0f;
            }

            float flHealHP = flBaseHealHP;
            flHealHP += tACDTarget.AttributesGetFloat(ATTRIBUTE_HEALTH_GLOBE_BONUS_HEALTH);


            if (flPercentHeal > EPSILON)
            {

                float flTotalHealth = tACDTarget.AttributesGetFloat(ATTRIBUTE_HITPOINTS_MAX_TOTAL);
                flHealHP += flPercentHeal * flTotalHealth * 0.01f;

                float flIncreasedHealthFromGlobesPercent = tACDTarget.AttributesGetFloat(ATTRIBUTE_INCREASED_HEALTH_FROM_GLOBES_PERCENT_TOTAL);
                if (ACDIsHireling(tACDTarget))
                {
                    flIncreasedHealthFromGlobesPercent += flFollowerPercent;
                }

                flHealHP *= (flIncreasedHealthFromGlobesPercent + 1.0f);

                Player * ptPlayer = ACDGetControllingPlayer(idACDTarget);
                if (ptPlayer)
                {
                    if (ACDIsPlayer(idACDTarget))
                    {
                        SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_POWER_HEALING_RECEIVED, PowerGetSNO(idPower), -1, flHealHP);
                        SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_HEALING_RECEIVED_TOTAL, 0, 0, flHealHP);
                    }

                    if (ptPlayerHealing)
                    {
                        SGameStatsPlayerCounterIncrement(ptPlayerHealing, STAT_COUNTER_PLAYER_POWER_HEALING_DONE, PowerGetSNO(idPower), -1, flHealHP);
                        SGameStatsPlayerCounterIncrement(ptPlayerHealing, STAT_COUNTER_PLAYER_HEALING_DONE_TOTAL, 0, 0, flHealHP);
                    }
                }

                SPlayerRecordHealthGlobePickedUp(idACDTarget, flHealHP);


                float flManaPercent = tACDTarget.AttributesGetFloat(ATTRIBUTE_MANA_GAINED_FROM_GLOBES_PERCENT);
                float flMana = tACDTarget.AttributesGetFloat(ATTRIBUTE_MANA_GAINED_FROM_GLOBES);
                if (flManaPercent > EPSILON || flMana > EPSILON)
                {
                    float flManaMaxTotal = tACDTarget.ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_EFFECTIVE_MAX, RESOURCE_TYPE_MANA);
                    float flHealMana = flMana + flManaPercent * flManaMaxTotal;
                    if (flHealMana > EPSILON)
                    {
                        tACDTarget.AddResource(RESOURCE_TYPE_MANA, flHealMana, FALSE);
                    }
                }
            }

            int32 nHealTicks = Math::FloatToInt(flHealSeconds * TICKS_PER_SECOND);
            sDrinkHealth(idPower, idACDUser, tACDTarget, flHealHP, nHealTicks, HEAL_TYPE_HEALTH_GLOBE);

            ScriptEventOnActorHealed(tACDTarget.id, idACDUser, flHealHP);

            SNO snoEffect = GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_HEALTH_GLOBE_PICKED_UP);
            if (snoEffect != SNO_NONE)
            {
                if (tACDItem.IsInWorld())
                {
                    ACDID idACDProxy = sAddEffectProxy(tACDItem.GetPlace(), 0.5f);
                    if (idACDProxy != ACDID_INVALID)
                    {
                        ACDPlayEffectGroupACDToACD(snoEffect, idACDProxy, tACDTarget.id);
                    }
                }
            }


            if (idACDTarget != idACDUser)
            {
                PowerAPI_BuffApplyOnItemPickUpEffects(idACDTarget, idACDItem, PICKUP_ACTION_GLYPH);
            }
        }
    }

    return ITEM_USE_RESULT_CONSUMED;
}



















static ItemUseResult sUsePowerUpGlyph(
    PowerID idPower,
    const ACDID idACDUser,
    const ActorCommonData & tACDItem)
{


    const GameBalanceDefinition * pdefGB;
    ItemDefinition * ptItemDef = (ItemDefinition *)GBLockR(tACDItem.hGB, pdefGB, TRUE);
    ScopedActorDefinitionLockR pdefActor(ptItemDef->snoActor);
    SNO snoPower = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_POWERUP_POWER);
    GBUnlockR(pdefGB);

    if (snoPower == SNO_NONE)
    {
        return ITEM_USE_RESULT_INVALID;
    }

    if (!SNOFileExists(POWER_SNO, snoPower) || idACDUser == ACDID_INVALID)
    {
        return ITEM_USE_RESULT_INVALID;
    }






    PowerStartData tPowerStartData;
    PowerStartDataInit(tPowerStartData);
    tPowerStartData.idACD = idACDUser;
    tPowerStartData.idACDAlternateSource = idACDUser;
    tPowerStartData.snoPower = snoPower;
    tPowerStartData.ptExtraPowerStartData = (void *)(intptr_t)tACDItem.id;
    tPowerStartData.tTarget.Clear();
    PowerStartEx(tPowerStartData, -1, FALSE, FALSE);
    return ITEM_USE_RESULT_CONSUMED;
}



void sSendLootRunProgressGlyphPickupToPlayer(
    Player & tPlayer,
    float flProgress)
{
    FloatDataMessage tPickupMessage;
    tPickupMessage.flData = flProgress;

    PlayerList listDestPlayers(PlayerListGetElementPool());
    listDestPlayers.AddTail_Tracked(&tPlayer);

    MessageSendToClient(listDestPlayers, SMSG_DUNGEON_FINDER_PROGRESS_GLYPH_PICK_UP, &tPickupMessage, sizeof(tPickupMessage));
}



static ItemUseResult sUseLootRunProgressGlyph(
    const PowerID idPower,
    const ACDID idACDUser,
    const ACDID idACDItem)
{


    ActorCommonData * ptACDUser = ACDTryToGet(idACDUser);
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);

    SNO snoPickupEffect = GlobalSNOGet(GameIsWeeklyChallenge() ? GLOBALSNO_EFFECT_GROUP_CHALLENGE_RIFT_ORB_PICKUP : GLOBALSNO_EFFECT_GROUP_TIERED_LOOT_RUN_ORB_PICKUP);
    SNO snoAttractorEffect = GlobalSNOGet(GameIsWeeklyChallenge() ? GLOBALSNO_EFFECT_GROUP_CHALLENGE_RIFT_ORB_PICKUP_ATTRACTOR : GLOBALSNO_EFFECT_GROUP_TIERED_LOOT_RUN_ORB_PICKUP_ATTRACTOR);
    if (ptACDItem && ptACDItem->hGB.gbid == StringToGBID("p1_normal_rifts_Orb"))
    {
        snoPickupEffect = GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_NORMAL_LOOT_RUN_ORB_PICKUP);
        snoAttractorEffect = GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_NORMAL_LOOT_RUN_ORB_PICKUP_ATTRACTOR);
    }

    ACDPlayEffect(idACDUser, EFFECT_PLAY_EFFECT_GROUP, snoPickupEffect, ACDID_INVALID);

    if (snoAttractorEffect != SNO_NONE)
    {
        ActorCommonData * ptScopedACDItem = ACDTryToGet(idACDItem);
        if (ptScopedACDItem && ptScopedACDItem->IsInWorld())
        {
            ACDID idACDProxy = sAddEffectProxy(ptScopedACDItem->GetPlace(), 0.5f);
            if (idACDProxy != ACDID_INVALID)
            {
                ACDPlayEffectGroupACDToACD(snoAttractorEffect, idACDProxy, idACDUser);
            }
        }
    }

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    ScopedGlobalDefinitionLockR pdefGlobals;
    float flMaxXPPercent = pdefGlobals->flLootRunXPPercentToComplete;
    if (ptACDUser && ACDIsInTieredLootRun(*ptACDUser))
    {
        flMaxXPPercent = pdefGlobals->flTieredLootRunXPPercentToComplete;
    }

    const uint32 nLevels = 3;

    uint32 nCurrentLevel = Math::FloatToInt(tSGameGlobals.tDungeonFinderData.flDungeonFinderProgress * nLevels / flMaxXPPercent);
    nCurrentLevel = Math::Min(nCurrentLevel, nLevels);

    float flProgressBonus = pdefGlobals->ptServerData->flLootRunProgressGlobeXPScalar;


    if (GameIsWeeklyChallenge())
    {
        float flAdditionalScalar = tSGameGlobals.tChallengeRift.tDungeonFinderOverrides.flOrbValueMultiplier;

        if (flAdditionalScalar > 1.0f)
        {
            flProgressBonus *= flAdditionalScalar;
        }
    }


    tSGameGlobals.tDungeonFinderData.flDungeonFinderProgress += flProgressBonus;

    uint32 nNewLevel = Math::FloatToInt(tSGameGlobals.tDungeonFinderData.flDungeonFinderProgress * nLevels / flMaxXPPercent);
    nNewLevel = Math::Min(nNewLevel, nLevels);

    PlayerList listPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listPlayers, -1, FALSE, -1);
    POS pos = listPlayers.GetHeadPosition();
    while (pos)
    {
        Player * ptPlayer = listPlayers.GetNext(pos);
        SGameSendDungeonFinderProgressToPlayer(*ptPlayer);
        sSendLootRunProgressGlyphPickupToPlayer(*ptPlayer, tSGameGlobals.tDungeonFinderData.flDungeonFinderProgress);
    }

    if (nNewLevel != nCurrentLevel)
    {
        ScriptEventOnUserEvent("LootRun_BonusBarBuff", idACDUser, nNewLevel);

        if (nNewLevel == nLevels)
        {
            ScriptEventOnUserEvent("LRALLMONSTERSKILLED", idACDUser, 0);
            ACDID idACDBoss = ACDID_INVALID;  // Inference: declared on 580 (DWARF) but the spawn call is on 583; with no user the TryToGet below folds away.
            if (ptACDUser)
            {
                idACDBoss = SWorldDungeonFinderSpawnBoss(ptACDUser->GetPlace());
            }

            ActorCommonData * ptACDBoss = ACDTryToGet(idACDBoss);
            if (ptACDBoss)
            {
                ptACDBoss->AttributesSetInt(ATTRIBUTE_IS_LOOT_RUN_BOSS, 1);
                ptACDBoss->AttributesSetInt(ATTRIBUTE_NEVER_DEACTIVATES, 1);

                tSGameGlobals.tDungeonFinderData.snoBossSpawned = ptACDBoss->snoActor;
            }
        }
    }


    return ITEM_USE_RESULT_CONSUMED;
}



static ItemUseResult sUseArcaneGlyph(
    const PowerID idPower,
    const ACDID idACDUser,
    const ACDID idACDItem)
{
    ActorCommonData * ptACDUser = ACDTryToGet(idACDUser);
    ActorCommonData * ptACDItem = ACDTryToGet(idACDItem);
    if (ptACDUser == NULL || ptACDItem == NULL)
    {
        return ITEM_USE_RESULT_INVALID;
    }

    float flArcanePercentReturn = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_0);
    float flSpiritPercentReturn = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_1);
    float flHatredPercentReturn = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_2);
    float flDisciplinePercentReturn = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_3);
    float flManaPercentReturn = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_4);
    float flWrathPercentReturn = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_5);
    float flFuryPercentReturn = PowerGetFormulaValue(idPower, TAG_POWER_SCRIPT_FORMULA_6);

    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());


    ACDID idACDOwner = (ACDID)ptACDItem->AttributesGetInt(ATTRIBUTE_SPAWNED_BY_ACDID);
    if (idACDOwner != ACDID_INVALID)
    {
        ActorCommonData * ptACDOwner = ACDTryToGet(idACDOwner);
        if (ptACDOwner &&
            ptACDOwner->IsAlive() &&
            ptACDOwner->IsInSWorldID(ptACDItem->GetWorldLocation().idSWorld))
        {
            listTargets.AddTail_Tracked(ptACDOwner->id);
        }
    }
    else
    {

        NavPlace tNavPlace;
        ACDGetNavMeshHandleWithPlace(*ptACDUser, tNavPlace.tNavMeshHandle, tNavPlace.tPlace);

        PowerAPIChooseTargets tChooseTargets;
        PowerHelper_ChooseTargetsInit(idPower, tChooseTargets);
        PowerHelper_ChooseTargetsInitGeometrySphere(tNavPlace.tPlace, 50.0f, tChooseTargets.tGeometry);
        tChooseTargets.dwFlags |= (1 << CTF_DONT_PERFORM_LOS_CHECK);
        PowerAPI_DoFindAllPossibleTargets(tChooseTargets, listTargets);
    }

    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget && ACDIsPlayer(idACDTarget))
        {
            ResourceType ePrimaryResourceType = ptACDTarget->GetPrimaryResourceType();
            ResourceType eSecondaryResourceType = ptACDTarget->GetSecondaryResourceType();

            float flPrimaryResourceMax = ptACDTarget->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_MAX_TOTAL, ePrimaryResourceType);
            float flSecondaryResourceMax = ptACDTarget->ResourceAttributeGetFloat(ATTRIBUTE_RESOURCE_MAX_TOTAL, eSecondaryResourceType);

            switch (PlayerGetPlayerClass(ptACDTarget))
            {
            case PLAYER_CLASS_WIZARD:
                ptACDTarget->AddResource(ePrimaryResourceType, flArcanePercentReturn * flPrimaryResourceMax, FALSE);
                break;
            case PLAYER_CLASS_MONK:
                ptACDTarget->AddResource(ePrimaryResourceType, flSpiritPercentReturn * flPrimaryResourceMax, FALSE);
                break;
            case PLAYER_CLASS_DEMONHUNTER:
                ptACDTarget->AddResource(ePrimaryResourceType, flHatredPercentReturn * flPrimaryResourceMax, FALSE);
                ptACDTarget->AddResource(eSecondaryResourceType, flDisciplinePercentReturn * flSecondaryResourceMax, FALSE);
                break;
            case PLAYER_CLASS_WITCHDOCTOR:
                ptACDTarget->AddResource(ePrimaryResourceType, flManaPercentReturn * flPrimaryResourceMax, FALSE);
                break;
            case PLAYER_CLASS_CRUSADER:
                ptACDTarget->AddResource(ePrimaryResourceType, flWrathPercentReturn * flPrimaryResourceMax, FALSE);
                break;
            case PLAYER_CLASS_BARBARIAN:
                ptACDTarget->AddResource(ePrimaryResourceType, flFuryPercentReturn * flPrimaryResourceMax, FALSE);
                break;
            case PLAYER_CLASS_NECROMANCER:  // Inference: jump-table entries 5 and 6 share one block (wrath * primary max) whose rows fold to the switch line 663.
                ptACDTarget->AddResource(ePrimaryResourceType, flWrathPercentReturn * flPrimaryResourceMax, FALSE);
                break;
            }
        }
    }

    return ITEM_USE_RESULT_CONSUMED;
}







static ItemUseResult sUseCollectorsEditionBuff(
    PowerID idPower,
    const ACDID idACDUser,
    const ActorCommonData & tACDItem)
{


    const GameBalanceDefinition * pdefGB;
    ItemDefinition * ptItemDef = (ItemDefinition *)GBLockR(tACDItem.hGB, pdefGB, TRUE);
    ScopedActorDefinitionLockR pdefActor(ptItemDef->snoActor);
    SNO snoPower = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_POWERUP_POWER);
    GBUnlockR(pdefGB);
    // Inference: every exit leaves w23 = 1 (OK) after the actor unlock at 737.
    if (snoPower == SNO_NONE)
    {
        return ITEM_USE_RESULT_OK;
    }

    if (!SNOFileExists(POWER_SNO, snoPower) || idACDUser == ACDID_INVALID)
    {
        return ITEM_USE_RESULT_OK;
    }






    PowerStartData tPowerStartData;
    PowerStartDataInit(tPowerStartData);
    tPowerStartData.idACD = idACDUser;
    tPowerStartData.idACDAlternateSource = idACDUser;
    tPowerStartData.snoPower = snoPower;
    tPowerStartData.ptExtraPowerStartData = (void *)(intptr_t)tACDItem.id;
    tPowerStartData.tTarget.Clear();
    PowerStartEx(tPowerStartData, -1, FALSE, FALSE);
    return ITEM_USE_RESULT_OK;
}



static ItemUseResult sUseTownPortalStone(
    const ACDID idACDUser)
{
    ActorCommonData * ptACDUser = ACDTryToGet(idACDUser);
    if (ptACDUser == NULL || !ACDIsPlayer(idACDUser))
    {
        return ITEM_USE_RESULT_CANT_USE;
    }

    if (!GameIsParamFlagSet(GPF_ALLOW_TOWN_PORTALS))
    {
        return ITEM_USE_RESULT_CANT_USE;
    }
    // Inference: the null test shares line 755 with the declaration (cbz x0 at 755, nothing on 756-760).
    Player * ptPlayer = ACDGetControllingPlayer(idACDUser); if (ptPlayer == NULL) { return ITEM_USE_RESULT_CANT_USE; }





    if (!ACDIsInTownPortalArea(*ptACDUser))
    {
        return ITEM_USE_RESULT_CANT_USE;
    }

    return ITEM_USE_RESULT_OK;
}



static ItemUseResult sUseHearthPortalScroll(
    const ACDID idACDUser)
{
    Player * ptPlayer = ACDGetControllingPlayer(idACDUser);
    if (!ptPlayer)
    {
        return ITEM_USE_RESULT_CANT_USE;
    }

    ActorCommonData & tACDUser = ACDGet(idACDUser);
    if (tACDUser.IsInCombat())
    {
        return ITEM_USE_RESULT_CANT_USE;
    }

    ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
    const WorldLocation & tLocation = tACDPlayer.GetWorldLocation();
    WorldPlace tPlace;

    if (GameIsParamFlagSet(GPF_ALLOW_TOWN_PORTALS))
    {
        ACDID idLocationACD = TownPortalFindLocationMarkerACD(SPlayerIsLocal(*ptPlayer));
        if (idLocationACD == ACDID_INVALID)
        {
            return ITEM_USE_RESULT_CANT_USE;
        }


        ActorCommonData & tACDLocation = ACDGet(idLocationACD);
        NavMeshHandle tNavMeshHandleExit;
        ACDGetNavMeshHandle(tACDLocation, tNavMeshHandleExit);
        if (!FindSpawnLocation(
                tACDUser.snoActor, tNavMeshHandleExit, 0, 0, tPlace, tNavMeshHandleExit))
        {
            return ITEM_USE_RESULT_CANT_USE;
        }
        // Inference: the spawn-location call spans 802-803 (argument load at 803); the owner/orientation/warp statements are rows 815-818.







        ActorID idActorUser = (ActorID)ACDGetOwner(idACDUser);
        Quaternion qOrientation = tACDPlayer.GetOrientation();
        ActorChangeWorld(idActorUser, tPlace, qOrientation);
        SPlayerWarped(tACDUser, WARP_HEARTH_PORTAL, NULL, ACDID_INVALID, 0.0f, TRUE);
        return ITEM_USE_RESULT_CONSUMED;
    }

    // Inference: ptActor (830) is a function-scope local in the DWARF, so the town-portal branch above returns and this path follows it.
    if (!STeamGetStartLocation(tLocation.idSWorld, ptPlayer->eTeam, tPlace))
    {
        return ITEM_USE_RESULT_CANT_USE;
    }



    const Actor * ptActor = ActorTryToGet((ActorID)ACDGetOwner(ptPlayer->idPlayerACD));
    if (!ptActor)
    {
        return ITEM_USE_RESULT_CANT_USE;
    }

    ActorSnapToPosition(ptActor->id, tPlace, TRUE, TRUE);
    SPlayerWarped(tACDPlayer, WARP_HEARTH_PORTAL, NULL, ACDID_INVALID, 0.0f, TRUE);
    return ITEM_USE_RESULT_CONSUMED;
}




static ItemUseResult sUseDye(
    const ACDID idACDUser,
    const ACDID idACDSrcItem,
    const ACDID idACDTarget,
    BOOL bReusable)
{
    ActorCommonData & tACDUser = ACDGet(idACDUser);
    ActorCommonData & tACDSrcItem = ACDGet(idACDSrcItem);
    ActorCommonData & tACDTarget = ACDGet(idACDTarget);
    // Inference: three ACDGet locals on lines 850-852 in this order (inlined ACDGet call sites at 850, 851, 852 in the DWARF).
    if (!ItemIsDyeable(tACDTarget))
    {
        return ITEM_USE_RESULT_CANT_USE;
    }

    // Inference: DWARF puts eDyeType at 860 and the inlined Math::Pin at 861, so it is clamped in a second statement; the bounds 0 and MAX_DYE_VALUE - 1 (22) come from the cmp #0x15 / csel #0x16 / csel wzr in the assembly.
    DyeType eDyeType = tACDSrcItem.GetDyeType();
    eDyeType = Math::Pin(eDyeType, 0, MAX_DYE_VALUE - 1);

    if (!SItemApplyDye(tACDTarget, tACDUser, eDyeType))
    {
        return ITEM_USE_RESULT_CANT_USE;
    }




    // Inference: SItemCrafting.cpp:569-584 has the same achievement block (SetPlayer, GetPlayer check, SetACD, EvaluateEvent ACH_EVENT_DYE_ITEM, SACDInventoryEvaluateItemDyeAchievements); here the operand is the source item's gbid (ldr [item + 0x3c]) and the ACD is ACDTryToGet(idACDTarget) (inlined ACDTryToGet at 878). sUseDye is static and called from ItemUse (not written), so it is not emitted yet; it was verified identical (all 592 bytes, reloc-masked) with a temporary external caller that has been removed.
    {
        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(PlayerGetByACD(idACDUser));

        if (tEvaluationData.GetPlayer())
        {
            tEvaluationData.SetACD(ACDTryToGet(idACDTarget));

            tEvaluationData.EvaluateEvent(
                tEvaluationData.GetPlayer(),
                Achievement::ACH_EVENT_DYE_ITEM,
                tACDSrcItem.hGB.gbid,
                1, (uint32)sub_71007b5e70());

            SACDInventoryEvaluateItemDyeAchievements(&tEvaluationData);
        }
    }
    // Inference: ItemUseResult is OK(1) when bReusable else CONSUMED(0), from the assembly's cset on bReusable; no instruction of its own.
    return bReusable ? ITEM_USE_RESULT_OK : ITEM_USE_RESULT_CONSUMED;







}



static ItemUseResult sUseConsumableAddSockets(
    const ACDID idACDUser,
    const ACDID idACDTarget,
    const ActorCommonData & tACDItemToUse)
{
    ActorCommonData & tACDUser = ACDGet(idACDUser);
    ActorCommonData & tACDTarget = ACDGet(idACDTarget);

    if (SItemConsumableAddSockets(tACDTarget, tACDUser, tACDItemToUse))
    {
        return ITEM_USE_RESULT_CONSUMED;
    }

    return ITEM_USE_RESULT_CANT_USE;
}


























































































































BOOL CanSpawnActorByScrollOfCalling(
    SNO snoActor)
{
    if (snoActor != SNO_NONE)
    {
        // Inference (no instructions of their own): both early 'return FALSE' paths and the nested ifs; DWARF has no result local, and the line table shows b.eq at 1042, 1046 and 1053 feeding one merged return.
        ScopedActorDefinitionLockR pdefActor(snoActor);
        if (pdefActor->eType != ACTORTYPE_MONSTER)
        {
            return FALSE;
        }
// Inference: bCantSpawn (decl line 1052) is tested at 1053 with an early return, per the rows 1052/1053 and no result local.

        BOOL bCantSpawn = TagMapGetInt(pdefActor->hTagMap, TAG_ACTOR_CANNOT_BE_SPAWNED_BY_SCROLL_OF_CALLING);
        if (bCantSpawn)
        {
            return FALSE;
        }


        ScopedMonsterDefinitionLockR pdefMonster(pdefActor->snoMonster);
        return !(pdefMonster->dwFlags & (1 << MONSTERDEF_IS_UNIQUE_BIT))








            // Inference: the return expression continues on line 1070 (line-table rows at 1060 and 1070); flag bits UNIQUE(0) and BOSS(5) from the tst/bic on dwFlags.
            && !(pdefMonster->dwFlags & (1 << MONSTERDEF_IS_BOSS_BIT));
    }

    return FALSE;
}

static SNO sFindMonsterForPlayersLevelArea(
    ActorCommonData & tACDPlayer)
{
    SNOArray listActors;
    blz::vector<int> listAllowedMonsterRarities;
    listAllowedMonsterRarities.push_back(MR_CHAMPION);
    listAllowedMonsterRarities.push_back(MR_RARE);
    ActorFindMonsterForPlayersLevelArea(tACDPlayer, listActors, listAllowedMonsterRarities);

    int32 nCount = listActors.GetArraySize();
    if (nCount == 0)
    {
        return SNO_NONE;
    }

    int32 nIndex = RandRange_Internal(SGameGlobalsGet().rng, 0, nCount - 1);
    return listActors[nIndex];
}




void sSummonMonsterForShrineOrScrollOfCallingGuts(
    ActorCommonData & tACDPlayer)
{
    SNO snoActor = sFindMonsterForPlayersLevelArea(tACDPlayer);
    if (snoActor == SNO_NONE)
    {

        snoActor = GlobalSNOGet(GLOBALSNO_ACTOR_SUMMONING_SCROLL_FALLBACK);
    }

    ActorAddStruct tActorAdd;
    tActorAdd.snoActor = snoActor;
    tActorAdd.tSNOName.Set(ACTOR_SNO, snoActor);
    tActorAdd.eMonsterRarity = MR_RARE;
    tActorAdd.eTeam = TEAM_MONSTER_0;
    tActorAdd.tMarkersName = MarkerStringHash("SummonMonster");
    tActorAdd.bIsSummon = TRUE;





    ActorFindAddLocation(tACDPlayer, 4, tActorAdd);

    ActorID idActorMonster;
    if (SSceneTryToGet(tActorAdd.tNavMeshHandle.idSScene))
    {
        idActorMonster = GBAdd(tActorAdd);
    }

}



void PowerSummonMonsterForShrineOrScrollOfCalling(
    PowerID idPower,
    void *)
{
    ActorCommonData * ptACD = PowerGetOwner(idPower);
    if (ptACD != NULL)
    {
        sSummonMonsterForShrineOrScrollOfCallingGuts(*ptACD);
    }
}



static ItemUseResult sSummonMonster(
    ActorCommonData & tACDPlayer)
{
    sSummonMonsterForShrineOrScrollOfCallingGuts(tACDPlayer);
    return ITEM_USE_RESULT_CONSUMED;
}
























static ItemUseResult sUseScroll(
    const PowerID idPower,
    const ACDID idACDUser,
    const ACDID idACDItem)
{
    ActorCommonData & tACD = ACDGet(idACDItem);
    const GameBalanceDefinition * pdefGB;
    ItemDefinition * ptItemDef = (ItemDefinition *)GBLockR(tACD.hGB, pdefGB, TRUE);


    PowerAPIBuffStartData tBuffStartData(idPower);
    tBuffStartData.bInfinite = false;
    tBuffStartData.dwDurationMin = 1200;
    tBuffStartData.eStackType = BUFF_STACKTYPE_NEVER;
    tBuffStartData.snoPower = GlobalSNOGet(GLOBALSNO_POWERS_SCROLL_BUFF);

    for (int32 i = 0; i < (int32)(ptItemDef->serAttributes_Console.dwSize / sizeof(AttributeSpecifier)); i++)
    {
        AttributeSpecifier & tSpecifier = ptItemDef->ptAttributes[i];
        if (tSpecifier.eAttribute != ATTRIBUTE_NONE)  // Inference: both compares (-1 and 724) carry line 1194, so the duration test below is nested in this one and SimplifyCFG made a switch of the pair.
        {
            if (tSpecifier.eAttribute == ATTRIBUTE_SCROLL_DURATION)
            {
                tBuffStartData.dwDurationMin = (uint32)(tACD.AttributesGetFloat(ATTRIBUTE_SCROLL_DURATION) * 60.0f);
            }
            if (tSpecifier.eAttribute != ATTRIBUTE_ITEM_BUFF_ICON)
            {




                FastAttribKey tKey = MakeAttribKey(tSpecifier.eAttribute, tSpecifier.nParam);
                AttributeDataType eDataType = KeyGetDataType(tKey);
                switch (eDataType) {  // Inference: the float, int and fatal paths are rows 1210, 1214 and 1218, so the brace shares the switch line.
                case ATTRIBUTEDT_FLOAT:
                    FastAttribSetValueFloat(tBuffStartData.idFastAttrib, tKey, tACD.AttributesGetFloat(tKey));
                    break;

                case ATTRIBUTEDT_INT:
                    FastAttribSetValueInt(tBuffStartData.idFastAttrib, tKey, tACD.AttributesGetInt(tKey));
                    break;

                default:
                    DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
                    break;
                }
            }
        }
    }
    PowerAPI_BuffAddToACD(tBuffStartData, ACDGet(idACDUser));

    GBUnlockR(pdefGB);

    return ITEM_USE_RESULT_CONSUMED;
}



static ItemUseResult sLearnRecipe(
    ACDID idACDUser,
    ActorCommonData & tACDItem)
{
    ScopedItemDefinitionLockR pdefItem(tACDItem.hGB.gbid);
    for (int32 i = 0; i < (int32)(sizeof(pdefItem->gbidCraftingRecipesToGrant) / sizeof(pdefItem->gbidCraftingRecipesToGrant[0])); i++)
    {
        if (pdefItem->gbidCraftingRecipesToGrant[i] != GBID_NONE)  // Inference: loop unrolled over the 10 entries (ldr [x0, #0x11c] .. [x0, #0x140]); test on 1240, call on 1242, unlock (dtor) on 1246.
        {
            SItemCrafting_LearnRecipe(idACDUser, pdefItem->gbidCraftingRecipesToGrant[i]);
        }
    }
    return ITEM_USE_RESULT_CONSUMED;
}




static void sSendCosmeticLearnedTutorial(
    ACDID idPlayer)
{
    Player * ptPlayer = PlayerGetByACD(idPlayer);
    Tutorial::SendTutorialToClient(GlobalSNOGet(GLOBALSNO_TUTORIAL_LEARNED_COSMETIC), ptPlayer, "Root.NormalLayer.BattleNetFooter_main.LayoutRoot.ButtonContainer.PersonalizationButton", ACDID_INVALID);
}




static ItemUseResult sLearnWings(
    ACDID idACDUser,
    ActorCommonData & tACDItem)
{
    ScopedItemDefinitionLockR pdefItem(tACDItem.hGB.gbid);
    // Inference: type test on 1267 and learn call on 1269 per the line table; nested ifs with one CANT_USE return (w23 = 2 is set once at 1265).
    if (pdefItem->gbidItemType == GlobalGBIDGet(GLOBALGBID_ITEMTYPE_COSMETIC_WINGS))
    {
        if (SCosmeticItems_LearnCosmeticItem(idACDUser, tACDItem.hGB.gbid, COSMETIC_ITEM_WINGS, false))
        {
            sSendCosmeticLearnedTutorial(idACDUser);
            return ITEM_USE_RESULT_CONSUMED;
        }
    }

    return ITEM_USE_RESULT_CANT_USE;
}



static ItemUseResult sLearnPennant(
    ACDID idACDUser,
    ActorCommonData & tACDItem)
{
    ScopedItemDefinitionLockR pdefItem(tACDItem.hGB.gbid);

    if (pdefItem->gbidItemType == GlobalGBIDGet(GLOBALGBID_ITEMTYPE_COSMETIC_PENNANT))
    {
        if (SCosmeticItems_LearnCosmeticItem(idACDUser, tACDItem.hGB.gbid, COSMETIC_ITEM_PENNANT, false))
        {
            sSendCosmeticLearnedTutorial(idACDUser);
            return ITEM_USE_RESULT_CONSUMED;
        }
    }

    return ITEM_USE_RESULT_CANT_USE;
}



static ItemUseResult sLearnPet(
    ACDID idACDUser,
    ActorCommonData & tACDItem)
{
    ScopedItemDefinitionLockR pdefItem(tACDItem.hGB.gbid);

    if (pdefItem->gbidItemType == GlobalGBIDGet(GLOBALGBID_ITEMTYPE_COSMETIC_PET))
    {
        if (SCosmeticItems_LearnPet(idACDUser, tACDItem.hGB.gbid, false))
        {
            sSendCosmeticLearnedTutorial(idACDUser);
            return ITEM_USE_RESULT_CONSUMED;
        }
    }

    return ITEM_USE_RESULT_CANT_USE;
}



static ItemUseResult sLearnPortraitFrame(
    ACDID idACDUser,
    ActorCommonData & tACDItem)
{
    ScopedItemDefinitionLockR pdefItem(tACDItem.hGB.gbid);
    // Inference: no tutorial here; the learn call (1329) result alone selects CONSUMED (w23 = 0) or CANT_USE.
    if (pdefItem->gbidItemType == GlobalGBIDGet(GLOBALGBID_ITEMTYPE_COSMETIC_PORTRAIT_FRAME))
    {
        if (SCosmeticItems_LearnCosmeticItem(idACDUser, tACDItem.hGB.gbid, COSMETIC_ITEM_FRAME, false))
        {
            return ITEM_USE_RESULT_CONSUMED;
        }
    }

    return ITEM_USE_RESULT_CANT_USE;
}








































































static ItemUseResult sUnlockTransmogs(
    ACDID idACDUser,
    ActorCommonData & tACDItem)
{
    Player * ptPlayer = PlayerGetByACD(idACDUser);
    if (ptPlayer == NULL)
    {
        return ITEM_USE_RESULT_CANT_USE;
    }

    SItemCrafting_TryUnlockSecondaryTransmogs(tACDItem.hGB.gbid, *ptPlayer);
    return ITEM_USE_RESULT_CONSUMED;
}




static ItemUseResult sUseTreasureBag(
    ACDID idACDUser,
    ActorCommonData & tACDItem)
{
    Player * ptPlayer = PlayerGetByACD(idACDUser);
    if (ptPlayer == NULL)
    {
        return ITEM_USE_RESULT_CANT_USE;
    }

    ActorCommonData & tACDPlayer = ACDGet(idACDUser);

    ScopedItemDefinitionLockR pdefItem(tACDItem.hGB.gbid);
    SNO snoTreasureClass = pdefItem->snoSalvageTreasureClassNormal;

    LooteeInfo tLootee;
    tLootee.InitializeFromACD(tACDPlayer, ACDID_INVALID);
    tLootee.tDebugName = tACDItem.tName;
    tLootee.hGBDebug = tACDItem.GetGBHandle();
    tLootee.snoTreasureClass = snoTreasureClass;
    tLootee.tLooteeParams.nLevel = Math::Max(tACDItem.AttributesGetInt(ATTRIBUTE_ITEM_LEGENDARY_ITEM_LEVEL_OVERRIDE), 1);
    tLootee.tLooteeParams.bSuppressMagicAndGoldFind = TRUE;
    tLootee.tLooteeParams.bSuppressUnluckyBonus = TRUE;  // Inference: one 8-byte store of 1,1 to +0x1c/+0x20 attributed to 1447 (merged with this line).
    tLootee.tLooteeParams.bSuppressTieredRiftBonuses = TRUE;
    tLootee.tLooteeParams.fIsTreasureBag = TRUE;
    tLootee.tLooteeParams.bDisableAncientDrops = (tACDItem.AttributesGetInt(ATTRIBUTE_POST_2_1_2_DROP) == 0);
    tLootee.tLooteeParams.bDisablePrimalAncientDrops = (tACDItem.AttributesGetInt(ATTRIBUTE_POST_2_5_0_DROP) == 0);
    tLootee.tLooteeParams.bDisableNewCraftingDrops = (tACDItem.AttributesGetInt(ATTRIBUTE_POST_2_3_0_DROP) == 0);

    ScopedTreasureClassDefinitionLockR pdefTreasureClass(snoTreasureClass);
    if (pdefTreasureClass->nForcedDropLevel >= 1)
    {
        tLootee.tLooteeParams.nLevel = pdefTreasureClass->nForcedDropLevel;
    }

    tLootee.tLooteeParams.nAncientChanceHandicapLevelOverride = pdefTreasureClass->nAncientChanceHandicapLevelOverride;


    if (!(pdefItem->dwFlags & (1 << ITEM_TORMENT_ONLY_BIT)))
    {
        tLootee.tLooteeParams.bDisableTormentDrops = TRUE;
    }






    LootSpecifierArray arLootSpecifiersToSpawn;
    BOOL fResult = LootGetLootSpecifiers(tLootee, ptPlayer->idPlayerACD, arLootSpecifiersToSpawn, NULL, FALSE, 1, NULL);
    if (fResult)
    {



        LootModifyBeforeSpawning(arLootSpecifiersToSpawn, tACDPlayer, ACDID_INVALID);

        FlippiesCreateOnActor(tACDPlayer, ptPlayer->idPlayerACD, arLootSpecifiersToSpawn, 0, SGameGetEventNumber(), 0.0f, ACDID_INVALID, &tLootee, NULL, LC_TREASUREBAG, NULL);
    }
    return ITEM_USE_RESULT_CONSUMED;
}





static ItemUseResult sReadPageOfFate(
    const ActorCommonData & tACDPlayer,
    const ActorCommonData & tACDItem)
{
    WorldPlace placeSpot;
    BOOL bFoundSpot = SPlayerFindValidWalkableSpotNearPlayer(tACDPlayer, placeSpot);
    if (bFoundSpot)
    {
        ScopedItemDefinitionLockR pdefItem(tACDItem.hGB.gbid);
        ActorAddStruct tActorAdd;
        tActorAdd.dwActorAddFlags = (1 << ACTORADD_DOESNT_WALK_BIT) | (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);



        tActorAdd.tPlace = placeSpot;
        tActorAdd.qOrientation.FromYaw(0.0f);
        tActorAdd.eTeam = TEAM_SCENERY;
        tActorAdd.hGB.eType = GB_INVALID; tActorAdd.hGB.gbid = GBID_NONE;

        tActorAdd.tMarkersName = MarkerStringHash("PageOfFatePortal");
        tActorAdd.nLevel = pdefItem->nItemLevel;
        tActorAdd.snoActor = pdefItem->snoActorPageOfFatePortal;
        if (tActorAdd.snoActor != SNO_NONE)
        {
            ActorID idActorPortal = GizmoAddBySNO(pdefItem->nItemLevel, tActorAdd);
            Actor * ptActorPortal = ActorTryToGet(idActorPortal);
            if (ptActorPortal)
            {
                ACDID idACDPortal = ACDFindByActor(idActorPortal);
                ActorCommonData * ptACDPortal = ACDTryToGet(idACDPortal);
                if (ptACDPortal)
                {
                    ptACDPortal->AttributesSetInt(ATTRIBUTE_PAGE_OF_FATE_ITEM, tACDItem.hGB.gbid);

                    ResolvedPortalDestination tResolvedDest;
                    ActorResolvePortalDestination(ptActorPortal, tResolvedDest);
                    SPortalSpecifierInitialize(ptActorPortal, tResolvedDest); return ITEM_USE_RESULT_CONSUMED;  // Inference: the CONSUMED flag (w22 = 1) is on 1528 and the block's destructors on 1534.
                }
            }
        }



    }

    return ITEM_USE_RESULT_CANT_USE;
}


static ItemUseResult sUsePortalScroll(
    const ActorCommonData & tACDPlayer,
    const ActorCommonData & tACDItem)
{
    WorldPlace placeSpot;
    BOOL bFoundSpot = SPlayerFindValidWalkableSpotNearPlayer(tACDPlayer, placeSpot);
    if (bFoundSpot)
    {
        ScopedItemDefinitionLockR pdefItem(tACDItem.hGB.gbid);
        ActorAddStruct tActorAdd;
        tActorAdd.dwActorAddFlags = (1 << ACTORADD_DOESNT_WALK_BIT) | (1 << ACTORADD_EVEN_WHEN_STUCK) | (1 << ACTORADD_DONT_PERTURB) | (1 << ACTORADD_DONT_SEARCH_FOR_LOCATION);



        tActorAdd.tPlace = placeSpot;
        tActorAdd.qOrientation.FromYaw(0.0f);
        tActorAdd.eTeam = TEAM_SCENERY;
        tActorAdd.hGB.eType = GB_INVALID; tActorAdd.hGB.gbid = GBID_NONE;

        tActorAdd.tMarkersName = MarkerStringHash("ScrollPortal");
        tActorAdd.snoActor = ActorDefinitionGetTagMapValueSNO(TAG_ACTOR_ITEM_SPAWNED_ACTOR, tACDItem.snoActor);

        if (tActorAdd.snoActor != SNO_NONE)
        {
            ActorID idActorPortal = GizmoAddBySNO(pdefItem->nItemLevel, tActorAdd);
            Actor * ptActorPortal = ActorTryToGet(idActorPortal); if (ptActorPortal != NULL) { return ITEM_USE_RESULT_CONSUMED; }  // Inference: the cbz and the CONSUMED flag (w21 = 1) are both on 1566 and the block's destructors on 1572.
        }




    }

    return ITEM_USE_RESULT_CANT_USE;
}



static ItemUseResult sUseShrineGlyph(
    const ACDID idACDUser,
    PowerID idPower,
    const ActorCommonData & tACDItem)
{
    const GameBalanceDefinition * pdefGB;
    ItemDefinition * ptItemDef = (ItemDefinition *)GBLockR(tACDItem.hGB, pdefGB, TRUE);
    ScopedActorDefinitionLockR pdefActor(ptItemDef->snoActor);
    SNO snoPower = TagMapGetSNO(pdefActor->hTagMap, TAG_ACTOR_POWERUP_POWER);
    GBUnlockR(pdefGB);

    if (snoPower == SNO_NONE)
    {
        return ITEM_USE_RESULT_INVALID;
    }

    if (!SNOFileExists(POWER_SNO, snoPower) || idACDUser == ACDID_INVALID)
    {
        return ITEM_USE_RESULT_INVALID;
    }








    ActorCommonData & tACDUser = ACDGet(idACDUser);


    NavPlace tNavPlace;
    ACDGetNavMeshHandleWithPlace(tACDUser, tNavPlace.tNavMeshHandle, tNavPlace.tPlace);

    PowerAPIChooseTargets tChooseTargets;
    XPooledDataIDList listTargets(MemoryPoolsGetDATAIDListPool());
    PowerHelper_ChooseTargetsInit(idPower, tChooseTargets);
    PowerHelper_ChooseTargetsInitGeometrySphere(tNavPlace.tPlace, 50.0f, tChooseTargets.tGeometry);
    tChooseTargets.dwFlags |= (1 << CTF_DONT_PERFORM_LOS_CHECK);
    PowerAPI_DoFindAllPossibleTargets(tChooseTargets, listTargets);

    POS pos = listTargets.GetHeadPosition();
    while (pos)
    {
        ACDID idACDTarget = (ACDID)listTargets.GetNext(pos);

        ActorCommonData & tACDTarget = ACDGet(idACDTarget);

        if (ACDIsPlayer(idACDTarget) &&
            ACDIsFriendly(tACDTarget, tACDUser, FALSE) &&
            tACDTarget.IsAlive())
        {

            SNO snoNephalemGlory = GlobalSNOGet(GLOBALSNO_POWERS_NEPHALEM_GLORY_X1);
            Player * ptPlayer = PlayerGetByACD(idACDTarget);
            BuffID idBuffNephalemGlory = PowerAPI_BuffFindOnACD(snoNephalemGlory, &tACDTarget, ACDID_INVALID);
            BOOL bFirstUse = (idBuffNephalemGlory == BUFFID_INVALID);


            PowerStartData tPowerStartData;
            PowerStartDataInit(tPowerStartData);
            tPowerStartData.idACD = idACDTarget;
            tPowerStartData.idACDAlternateSource = idACDTarget;
            tPowerStartData.snoPower = snoPower;
            tPowerStartData.ptExtraPowerStartData = (void *)(intptr_t)tACDItem.id;
            tPowerStartData.tTarget.Clear();
            PowerStartEx(tPowerStartData, -1, FALSE, FALSE);


            if (idACDTarget != idACDUser)
            {
                PowerAPI_BuffApplyOnItemPickUpEffects(idACDTarget, tACDItem.id, PICKUP_ACTION_GLYPH);
            }

            SNO snoEffect = GlobalSNOGet(GLOBALSNO_EFFECT_GROUP_POWERUP_GLOBE_PICKED_UP);
            if (snoEffect != SNO_NONE)
            {
                if (tACDItem.IsInWorld())
                {
                    ACDID idACDProxy = sAddEffectProxy(tACDItem.GetPlace(), 0.5f);
                    if (idACDProxy != ACDID_INVALID)
                    {
                        ACDPlayEffectGroupACDToACD(snoEffect, idACDProxy, tACDTarget.id);
                    }
                }
            }


            if (ptPlayer->bIsPrimaryPlayerServerOnly)
            {
                idBuffNephalemGlory = PowerAPI_BuffFindOnACD(snoNephalemGlory, &tACDTarget, ACDID_INVALID);
                if (idBuffNephalemGlory != BUFFID_INVALID)
                {
                    FloatingNumberMessage tMessage;
                    tMessage.flDamage = 0.0f;
                    if (bFirstUse)
                    {
                        tMessage.eType = FLOATING_NUMBER_TYPE_NEPHALEM_GLORY_GAINED;
                    }
                    else
                    {
                        tMessage.eType = FLOATING_NUMBER_TYPE_NEPHALEM_GLORY_EXTRA_TIME;
                        PowerAPIBuffInstance * ptBuff = PowerAPI_BuffTryToGet(idBuffNephalemGlory);
                        if (ptBuff)
                        {
                            tMessage.flDamage = PowerGetFormulaValue(ptBuff->idPower, TAG_POWER_SCRIPT_FORMULA_27);
                        }
                    }
                    tMessage.annTarget = ANNFromACD(idACDTarget);
                    PlayerList listDestPlayers(PlayerListGetElementPool());
                    listDestPlayers.AddTail_Tracked(PlayerGetByACD(idACDTarget));
                    MessageSendToClient(listDestPlayers, SMSG_FLOATING_NUMBER, &tMessage, sizeof(tMessage));
                }
            }
        }
    }







    return ITEM_USE_RESULT_CONSUMED;
}


static ItemUseResult sUsePowerScroll(
    const ACDID idACDUser,
    const ActorCommonData & tACDItem)
{
    SNO snoPower = ItemUseGetScrollPower(tACDItem);
    if (snoPower == SNO_NONE)
    {
        return ITEM_USE_RESULT_INVALID;
    }

    if (!SNOFileExists(POWER_SNO, snoPower) || idACDUser == ACDID_INVALID)
    {
        return ITEM_USE_RESULT_INVALID;
    }






    PowerStartData tPowerStartData;
    PowerStartDataInit(tPowerStartData);
    tPowerStartData.idACD = idACDUser;
    tPowerStartData.idACDAlternateSource = tACDItem.id;
    tPowerStartData.snoPower = snoPower;
    tPowerStartData.ptExtraPowerStartData = (void *)(intptr_t)tACDItem.id;
    tPowerStartData.tTarget.Clear();
    PowerStartEx(tPowerStartData, -1, FALSE, FALSE);
    return ITEM_USE_RESULT_OK;
}





static ItemUseResult sUseHoradricReagent(
    const ACDID idACDUser,
    const ActorCommonData & tACDItem)
{
    // Inference: body follows ActorGetHQCursedRealmActIndex (ScriptUtil.cpp:14744) for the world/act checks; IsInWorld at 1746, SWorldTryToGet at 1752, flag test 1754, IsValid 1755.
    if (!tACDItem.IsInWorld())
    {
        return ITEM_USE_RESULT_INVALID;
    }


    SWorld * ptSWorld = SWorldTryToGet(tACDItem.GetSWorldID());
    if (!ptSWorld ||
        !(ptSWorld->dwFlags & (1 << FLAG_SWORLD_FROM_DEATH_PORTAL)) ||
        !ptSWorld->tDynamicReturnLocation.IsValid())
    {
        return ITEM_USE_RESULT_INVALID;
    }


    int32 nCursedRealmAct = -1;
    Act eCursedRealmAct = LevelAreaGetAct(ptSWorld->tDynamicReturnLocation.snoDestLevelArea);
    switch (eCursedRealmAct)
    {
    case ACT_1:
        nCursedRealmAct = 1;
        break;
    case ACT_2:
        nCursedRealmAct = 2;
        break;
    case ACT_3:
        nCursedRealmAct = 3;
        break;
    case ACT_4:
        nCursedRealmAct = 4;
        break;
    case ACT_5:
        nCursedRealmAct = 5;
        break;
    }

    if (nCursedRealmAct == -1)
    {
        return ITEM_USE_RESULT_INVALID;
    }












    GBID gbidDroppedReagent = GlobalGBIDGet(GLOBALGBID_HQ_CURSED_REALM_DROPPED_REAGENT);
    BOOL bIsDroppedReagent = (tACDItem.hGB.gbid == gbidDroppedReagent);


    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (!ptACDPlayer ||
            !ptACDPlayer->IsAlive() ||
            !ACDInSameWorld(tACDItem, *ptACDPlayer) ||
            (bIsDroppedReagent && ptACDPlayer->id != idACDUser))
        {
            continue;
        }


        if (bIsDroppedReagent)
        {
            FastAttribKey tCursedRealmReagentsDroppedKey = MakeAttribKey(ATTRIBUTE_HQ_CURSED_REALM_REAGENTS_DROPPED, nCursedRealmAct);
            int32 nDroppedCount = ptACDPlayer->AttributesGetInt(tCursedRealmReagentsDroppedKey);
            ptACDPlayer->AttributesSetInt(tCursedRealmReagentsDroppedKey, nDroppedCount - 1);
        }
        else
        {

            FastAttribKey tCursedRealmReagentsCollectedKey = MakeAttribKey(ATTRIBUTE_HQ_CURSED_REALM_REAGENTS_COLLECTED, nCursedRealmAct);
            int32 nCollectedCount = ptACDPlayer->AttributesGetInt(tCursedRealmReagentsCollectedKey);
            ptACDPlayer->AttributesSetInt(tCursedRealmReagentsCollectedKey, nCollectedCount + 1);
        }

        SQuestsOnHQCursedRealmReagentCollected(ptPlayer->idPlayerACD, nCursedRealmAct);

    }


    return ITEM_USE_RESULT_CONSUMED;
}
// NON_MATCHING: 15084 vs 15088 bytes. The Horadric reagent loop hoists a shared constant 1 at +0x38d4. The original loads it separately in each branch. Tested negative guards, act-switch return placement and explicit BOOL comparisons.
ItemUseResult ItemUse(
    const PowerID idPower,
    const ACDID idACDUser,
    const ACDID idACDItemToUse,
    const Target * ptTarget)
{
    if (idPower != POWERID_INVALID && !PowerHelper_DoFirstTimeCosts(idPower, TRUE))
    {
        return ITEM_USE_RESULT_CANT_USE;
    }

    ActorCommonData & tACDItemToUse = ACDGet(idACDItemToUse);
    if (ItemCanBeUsed(idACDUser, tACDItemToUse, ptTarget) != GAMEERROR_NONE)
    {
        return ITEM_USE_RESULT_CANT_USE;
    }

    ItemUseResult eResult = ITEM_USE_RESULT_CANT_USE;
    ItemUseType eType = ItemGetUseType(tACDItemToUse);
    switch (eType)
    {
    case ITEM_USE_HEALTHPOTION:
        eResult = sUseHealthPotion(idACDUser, tACDItemToUse.id, idPower);
        break;

    case ITEM_USE_HEALTHGLYPH:
        eResult = sUseHealthGlyph(idPower, idACDUser, tACDItemToUse.id);
        break;

    case ITEM_USE_POWERUPGLYPH:
        eResult = sUsePowerUpGlyph(idPower, idACDUser, tACDItemToUse);
        break;

    case ITEM_USE_TOWNPORTALSTONE:
        eResult = sUseTownPortalStone(idACDUser);
        break;

    case ITEM_USE_TOWNPORTAL_UNLOCK:
        ACDGet(idACDUser).SetStoneOfRecallUnlocked(TRUE);
        eResult = ITEM_USE_RESULT_CONSUMED;
        break;

    case ITEM_USE_HEARTHPORTALSCROLL:
        eResult = sUseHearthPortalScroll(idACDUser);
        break;


    case ITEM_USE_DYE:
        eResult = sUseDye(idACDUser, tACDItemToUse.id, ptTarget->idACDTarget, FALSE);
        break;


    case ITEM_USE_DYE_REUSABLE:
        eResult = sUseDye(idACDUser, tACDItemToUse.id, ptTarget->idACDTarget, TRUE);
        break;


    case ITEM_USE_ADD_SOCKETS:
        eResult = sUseConsumableAddSockets(idACDUser, ptTarget->idACDTarget, tACDItemToUse);
        break;





    case ITEM_USE_LEARN_RECIPE:
        eResult = sLearnRecipe(idACDUser, tACDItemToUse);
        break;

    case ITEM_USE_SCROLL:
        eResult = sUseScroll(idPower, idACDUser, tACDItemToUse.id);
        break;

    case ITEM_USE_SUMMON_MONSTER:
        eResult = sSummonMonster(ACDGet(idACDUser));
        break;
        // Inference: only the inlined ACDGet at 1914 survives (its TLS call), then CONSUMED; whatever consumed the ACD left no code and no DWARF.
    case ITEM_USE_SUMMON_COMPANION:
        ACDGet(idACDUser);
        eResult = ITEM_USE_RESULT_CONSUMED;
        break;
    case ITEM_USE_ARCANEGLYPH:
        eResult = sUseArcaneGlyph(idPower, idACDUser, tACDItemToUse.id);
        break;

    case ITEM_USE_COLLECTORS_EDITION_BUFF:
        eResult = sUseCollectorsEditionBuff(idPower, idACDUser, tACDItemToUse);
        break;


    case ITEM_USE_PAGE_OF_FATE:
        eResult = sReadPageOfFate(ACDGet(idACDUser), tACDItemToUse);
        break;

    case ITEM_USE_PORTAL_SCROLL:
        eResult = sUsePortalScroll(ACDGet(idACDUser), tACDItemToUse);
        break;

    case ITEM_USE_POWER_SCROLL:
        eResult = sUsePowerScroll(idACDUser, tACDItemToUse);
        break;

    case ITEM_USE_SHRINEGLYPH:
        eResult = sUseShrineGlyph(idACDUser, idPower, tACDItemToUse);
        break;

    case ITEM_USE_UNLOCK_TRANSMOGS:
        eResult = sUnlockTransmogs(idACDUser, tACDItemToUse);
        break;

    case ITEM_USE_TREASURE_BAG:
        eResult = sUseTreasureBag(idACDUser, tACDItemToUse);
        break;

    case ITEM_USE_LOOT_RUN_PROGRESS_GLYPH:
        eResult = sUseLootRunProgressGlyph(idPower, idACDUser, tACDItemToUse.id);
        break;

    case ITEM_USE_LEARN_WINGS:
        eResult = sLearnWings(idACDUser, tACDItemToUse);
        break;

    case ITEM_USE_LEARN_PENNANT:
        eResult = sLearnPennant(idACDUser, tACDItemToUse);
        break;

    case ITEM_USE_LEARN_PET:
        eResult = sLearnPet(idACDUser, tACDItemToUse);
        break;





    case ITEM_USE_LEARN_PORTRAIT_FRAME:
        eResult = sLearnPortraitFrame(idACDUser, tACDItemToUse);
        break;

    case ITEM_USE_HORADRIC_REAGENT:
        eResult = sUseHoradricReagent(idACDUser, tACDItemToUse);
        break;




    }
    // Inference: cmp eResult, #1; b.hi return at 1983, so CONSUMED (0) and OK (1) continue.
    if (eResult == ITEM_USE_RESULT_CONSUMED || eResult == ITEM_USE_RESULT_OK)
    {
        SGameStatsPlayerCounterIncrement(PlayerGetByACD(idACDUser), STAT_COUNTER_PLAYER_ITEM_USED, tACDItemToUse.GetGBHandle().gbid, 0, 1.0f);

        SQuestsOnItemUsed(idACDUser, tACDItemToUse.id);

























        if (eResult == ITEM_USE_RESULT_CONSUMED)
        {

            GBID gbidItem = tACDItemToUse.hGB.gbid;

            ActorID idActorUseItem = (ActorID)ACDGetOwner(tACDItemToUse.id);
            if (idActorUseItem != ACTORID_INVALID &&
                !ActorFlaggedForDeletion(idActorUseItem))
            {
                SACDInventoryDeleteItemAfterUse(tACDItemToUse);
            }

            if (eType == ITEM_USE_HEALTHPOTION)
            {


                Player * ptPlayer = PlayerGetByACD(idACDUser);
                if (ptPlayer != NULL)
                {

                    uint32 nCount = ActorInventoryCountItemsOfTypeOnPlayer(idACDUser, gbidItem, NULL);
                    if (nCount == 0)
                    {
                        PlayErrorSoundMessage tMessage;
                        tMessage.dwSoundTag = TAG_ACTOR_WARNING_OUT_OF_POTIONS;

                        PlayerList listDestPlayers(PlayerListGetElementPool());
                        listDestPlayers.AddTail_Tracked(ptPlayer);

                        MessageSendToClient(listDestPlayers, SMSG_PLAY_ERROR_SOUND, &tMessage, sizeof(tMessage));
                    }
                }
            }
        }
    }





    return eResult;
}




void PowerUseItem(
    Power * ptPower,
    PowerMessage eMessage,
    void * pParam)
{
    if (eMessage == POWER_START)
    {

        // Inference: eMessage test written as a plain if; line table has 2064, 2069, 2071 and the closing brace at 2081.
        ACDID idACDItemToUse = (ACDID)(intptr_t)pParam;
        if (ItemUse(ptPower->id, ptPower->idACD, idACDItemToUse, &ptPower->tTarget) != ITEM_USE_RESULT_CANT_USE)
        {

            AIStateActivateCooldown(*ptPower);
        }






    }
}




void PowerStartUseItem(
    ACDID idACDPlayer,
    ACDID idACDItemToUse,
    const Target * ptTarget)
{
    ActorCommonData & tACDItemToUse = ACDGet(idACDItemToUse);
    // Inference: PowerStartData members assigned after PowerStartDataInit, lines 2095-2099 from the line table.
    PowerStartData tPowerStartData;
    PowerStartDataInit(tPowerStartData);
    tPowerStartData.idACD = idACDPlayer;
    tPowerStartData.idACDAlternateSource = idACDPlayer;

    tPowerStartData.snoPower = ItemGetUsePower(tACDItemToUse.hGB.gbid);
    tPowerStartData.ptExtraPowerStartData = (void *)(intptr_t)idACDItemToUse;

    if (ptTarget != NULL)
    {
        tPowerStartData.tTarget = *ptTarget;
    }
    else
    {
        tPowerStartData.tTarget.Clear();
    }

    PowerStartEx(tPowerStartData, -1, FALSE, FALSE);






}
