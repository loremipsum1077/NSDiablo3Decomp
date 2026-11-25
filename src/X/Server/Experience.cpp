// Original file: X/Server/Experience.cpp
#include "./Achievements/AchievementModifierEvaluationLogic.h"
#include "./AI/ActorAI.h"
#include "./PlayerList.h"
#include "./Actors/ActorEffect.h"
#include "./LuaScript/ScriptEvent.h"
#include "./Powers/Powers.h"
#include "./Powers/PowerAPI.h"
#include "./SGame.h"
#include "./SGameStats.h"
#include "./SGameStatsSwig.h"
#include "./SPlayer.h"
#include "./SWorld.h"
#include "./System/SNephalemRift.h"
#include "../Common/Actors/ActorEffectCommon.h"
#include "../Common/Attributes/FastAttribute.h"
#include "../Common/Networking/GameMessage.h"
#include "../Common/PlayerAccount.h"
#include "../Common/Protobuf/Account.pb.h"
#include "../Common/Target.h"
#include "../Sigma/ErrorManager/ErrorManager.h"
#include "../Common/Achievements/AchievementDefines.h"
#include "../Common/ActorCommonData.h"
#include "../Common/Attributes/FastAttribConstantsSwig.h"
#include "../Common/Attributes/FastAttributeStructs.h"
#include "../Common/Definitions/GameBalanceDefinition.h"
#include "../Common/Definitions/GlobalDefinition.h"
#include "../Common/Game.h"
#include "../Common/GameCommonData.h"
#include "../Common/GameTemplate.h"
#include "../Common/GameSwig.h"
#include "../Common/GlobalSNO.h"
#include "../Common/Items.h"
#include "../Common/Monsters.h"
#include "../Common/Player.h"
#include "../Common/Shell/CmdLine.h"
#include "../Sigma/Math/MathGeneral.h"

uint64 ExperienceGetCurrentLevelXP(const ActorCommonData & tACD, const BOOL bUseAlt);  // X/Common/ExperienceCommon.cpp:20
BOOL ExperiencePlayersCloseEnough(const ActorCommonData & tACDVictim, ACDID idACDOtherPlayer);  // X/Common/ExperienceCommon.cpp:78
BOOL LootDropLootForEligiblePlayers(ActorCommonData & tACDLootee, PlayerList & listEligiblePlayers, const ACDID idACDPrimaryLooter, uint32 dwGameEventNumber, BOOL fAutoIdentify, float flCameraBiasDist, BOOL bSuppressUnluckyBonus);  // X/Server/Loot/Loot.cpp:698
BOOL sExperienceGrantToACD(ActorCommonData & tACD, const uint64 nExp, BOOL bNotifyClient, BOOL bApplyRestXPBonus);
// 0x71004b8ae0 (unnamed, 100 bytes, in the ActorCommonData accessor range): takes the player ACD in x0
// and returns a BOOL; when it holds, rest XP is refilled to the XP still needed for the level.
extern "C" BOOL sub_71004b8ae0(const ActorCommonData * ptACD);
void SHirelingLevelUp(ActorCommonData & tACDHireling, int32 nLevel);  // X/Server/SHireling.cpp:525
uint32 PlayerGetPlayedSeconds(const Player & tPlayer);  // X/Common/Player.cpp:3593
PlayerClass PlayerGetPlayerClass(const ACDID idACD);  // X/Common/Player.cpp:2292
void PowerAPI_RecomputeHitpointHealTarget(ActorCommonData * ptACD);  // X/Server/Powers/PowerAPI.cpp:13170
void SPlayerInitializeParagonPoints(Player & tPlayer);  // X/Server/SPlayer.cpp:6726
void SPlayerRecalculateUpscalingLevels();  // X/Server/SPlayer.cpp:13633
void SPlayerAutoPurchaseSkillForNewLevel(ActorCommonData & tACD, Player & tPlayer, int32 nLevel);  // X/Server/SPlayer.cpp:6550
void SGameStatsPlayerTimedEventStart(Player * ptPlayer, StatCounterType eType, int32 nParam0, int32 nParam1, BOOL bReportIfUnfinished);  // X/Server/SGameStats.cpp:1571
void SGameStatsPlayerTimedEventFinish(Player * ptPlayer, StatCounterType eType, int32 nParam0, int32 nParam1);  // X/Server/SGameStats.cpp:1599
BOOL SGameStatsUsedCheats();  // X/Server/SGameStats.cpp:1808
void SendPlayerLevelMessage(const ACDID idPlayerACD, Attrib eAttribLevel);  // X/Server/Network/ServerMessageSend.cpp:1331
void ExperienceUpdatePerLevelHitpointFactorVitality(ActorCommonData & tACD);

BOOL SWorldIsDungeonFinder(SWorldID idSWorld);  // X/Server/SWorld.cpp:3304
float ExperienceGetBonusFromGear(const ActorCommonData & tACDPlayer);  // X/Common/ExperienceCommon.cpp:137
float ExperienceGetHirelingBonus(const ActorCommonData & tACDPlayer);  // X/Common/ExperienceCommon.cpp:165
uint64 ActorCalculateXPForMonster(const ActorCommonData & tACDVictim, int32 nPlayerLevel);  // X/Server/Actors/Actors.cpp:2322
void TraceStat(LPCSTR sEventType, ACDID idACD, LPCSTR cpFmtStr, ...);  // X/Server/SGame.cpp:7255
void LootDropCatchupLoot(ActorCommonData & tACDVictim, PlayerList & listPlayersToEarnXP, uint32 dwGameEventNumber);  // X/Server/Loot/Loot.cpp:922
void GoldGainForKill(ACDID idACDVictim, ACDID idACDPlayer);

BOOL ExperienceGrantDirectly(ActorCommonData & tACDPlayer, const uint64 nExpGranted, const BOOL bNotifyClient, const BOOL bGrantToAllPlayersIfLocal);

//@ 62
static const float sgflLowerExpRatios[] =
{
    1.0f,
    0.9f,
    0.8f,
    0.7f,
    0.6f,
    0.45f,
    0.3f,
    0.15f,
    0.05f,
    0.02f,
    0.0f,
};

static const int32 sgnLowerExpLimit = 10;

//@ 79
static const float sgflUpperExpRatios[] =
{
    1.0f,
    1.15f,
    1.2f,
    1.25f,
};

static const int32 sgnUpperExpLimit = 3;

//@ 94
static uint64 sExpAdjustForRelativeLevel(int32 nMonsterRelativeLevel, uint64 nExperience)
{


    if (nMonsterRelativeLevel < 0)
    {
        nMonsterRelativeLevel = Math::Min(-nMonsterRelativeLevel, sgnLowerExpLimit);


        return (uint64)((float)nExperience * sgflLowerExpRatios[nMonsterRelativeLevel]);
    }
    else
    {

        nMonsterRelativeLevel = Math::Min(nMonsterRelativeLevel, sgnUpperExpLimit);

        return (uint64)((float)nExperience * sgflUpperExpRatios[nMonsterRelativeLevel]);
    }
}

//@ 117
static void sHeroAttributeInc(ACDID idACD, Attrib eAttrib, int32 nAdditionalAmount)



{
    ActorCommonData & tACD = ACDGet(idACD);
    float flOldValue = tACD.AttributesGetBaseFloat(FastAttribKey(eAttrib));
    tACD.AttributesSetFloat(FastAttribKey(eAttrib), flOldValue + (float)nAdditionalAmount);
}

// Only the normal-level half of this function is observable: its one caller passes
// fUseAltLevels == FALSE, so whatever the alt-level half did (lines 153-168) was folded away.
//@ 129
static void sHeroAttributeAward(ACDID idACD, int32 nLevel, const BOOL fUseAltLevels)













{
    if (nLevel < 1 || nLevel > 70)
    {
        return;
    }


    PlayerClass ePlayerClass = PlayerGetPlayerClass(idACD);

    if (ePlayerClass == PLAYER_CLASS_NONE)
    {
        return;
    }

















    if (!fUseAltLevels)
    {
        SNO snoXPTable = GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE);
        ScopedGameBalanceDefinitionLockR ptGameBalDef(snoXPTable);


        const ExperienceTable & tExpTable = ptGameBalDef->tExperienceTable;
        const AttributeAwardDefinition & tAward = tExpTable.ptExperienceLevels[nLevel - 1].tAttributeAward[ePlayerClass];
        sHeroAttributeInc(idACD, ATTRIBUTE_STRENGTH, tAward.nStrength);
        sHeroAttributeInc(idACD, ATTRIBUTE_DEXTERITY, tAward.nDexterity);
        sHeroAttributeInc(idACD, ATTRIBUTE_INTELLIGENCE, tAward.nIntelligence);
        sHeroAttributeInc(idACD, ATTRIBUTE_VITALITY, tAward.nVitality);
    }
}

//@ 186
static void sExperienceLevelUpPlayer(ActorCommonData & tACD, int32 nLevel, BOOL fUseAltLevels)













{
    if (!fUseAltLevels)
    {
        sHeroAttributeAward(tACD.id, nLevel, fUseAltLevels);
    }
















    Player * ptPlayer = PlayerGetByACD(tACD.id);
    ptPlayer->tHeroStateData.tSavedData.uTimeLastLevel = PlayerGetPlayedSeconds(*ptPlayer);


    tACD.MarkHeroStateSavedDataChanged();


    PowerAPI_RecomputeHitpointHealTarget(&tACD);

    ExperienceUpdatePerLevelHitpointFactorVitality(tACD);

    SPlayerInitializeParagonPoints(*ptPlayer);


    SPlayerRecalculateUpscalingLevels();
    PowerAPI_ResetCooldowns(tACD);
}

//@ 242
static void sPlayerLevelUpPower(const ACDID idACD, const SNO snoPower)










{
    PowerStart(idACD, snoPower, cgTargetNone);
}

// 2.7.7: sUpdateAccountFlagsForNewLevel (2.6.2 //@ 263) is gone; sExperienceLevelUp no longer syncs the deprecated digest difficulty flags.











































//@ 309
static void sUpdateGameStatsForLevel(Player * ptPlayer, int32 nLevel, ActorCommonData & tACD)



{
    if (SGameStatsUsedCheats())
    {
        return;
    }


    SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_VITALITY_AT_LEVEL, nLevel, 0, tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_VITALITY_TOTAL)));
    SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_ARMOR_AT_LEVEL, nLevel, 0, tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_ARMOR_TOTAL)));

    for (int32 i = 0; i < 7; i++)
    {
        SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_RESISTANCE_AT_LEVEL, nLevel, i, tACD.AttributesGetFloat(MakeAttribKey(ATTRIBUTE_RESISTANCE_TOTAL, i)));
    }
}

// bWasAlive and bResetParam are not in the DWARF local list (invented names); the binary keeps
// both values across the function.
//@ 332
static void sExperienceLevelUp(ActorCommonData & tACD, BOOL fUpdateClient, BOOL fUseAltLevels, BOOL fResetLifeStats)




{
    BOOL bWasAlive = tACD.IsAlive();




    Attrib eAttribLevel = fUseAltLevels ? ATTRIBUTE_ALT_LEVEL : ATTRIBUTE_LEVEL;


    int32 nLevel = tACD.AttributesGetInt(FastAttribKey(eAttribLevel)) + 1;



    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) &&
        tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_LOADING)))
    {
        fUpdateClient = FALSE;
    }















    if (fUseAltLevels)
    {
        tACD.AttributesSetInt(FastAttribKey(eAttribLevel), nLevel);
        SNO snoXPTable = GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE_ALT);
        ScopedGameBalanceDefinitionLockR ptGameBalDef(snoXPTable);
        const ExperienceTableAlt & tExpTableAlt = ptGameBalDef->tExperienceTableAlt;


        uint64 nNewXPDiff = tExpTableAlt.ptExperienceLevelsAlt[Math::Min(nLevel, 19999)].nDeltaXP;
        tACD.SetAltExperienceNext(nNewXPDiff);
    }
    else
    {
        tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_LEVEL), nLevel);
        SNO snoXPTable = GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE);
        ScopedGameBalanceDefinitionLockR ptGameBalDef(snoXPTable);
        const ExperienceTable & tExpTable = ptGameBalDef->tExperienceTable;


        uint64 nNewXPDiff = tExpTable.ptExperienceLevels[nLevel - 1].nDeltaXP;
        tACD.SetExperienceNext(nNewXPDiff);
    }

    if (ACDIsPlayer(tACD.id))
    {
        Player * ptPlayer = PlayerGetByACD(tACD.id);

        sExperienceLevelUpPlayer(tACD, nLevel, fUseAltLevels);


        SPlayerAddToSaveList(*ptPlayer, 0, PlayerSaveReason_LevelUp, PLAYER_SAVE_MODE_WITHIN_5_SECONDS);









        if (!fUseAltLevels)
        {
            SGameStatsPlayerTimedEventFinish(ptPlayer, STAT_COUNTER_PLAYER_TIME_SPENT_AT_LEVEL, nLevel - 1, 0);
            SGameStatsPlayerTimedEventStart(ptPlayer, STAT_COUNTER_PLAYER_TIME_SPENT_AT_LEVEL, nLevel, 0, TRUE);




            ACDID idACDHireling = PlayerGetHireling(tACD.id);
            ActorCommonData * ptACDHireling = ACDTryToGet(idACDHireling);
            if (ptACDHireling)
            {
                SHirelingLevelUp(*ptACDHireling, nLevel);
                if (fUpdateClient)
                {
                    sPlayerLevelUpPower(ptACDHireling->id, GlobalSNOGet(GLOBALSNO_POWERS_LEVELUP));
                }
            }
        }




        if (fUpdateClient)
        {

            EffectType eEffect = fUseAltLevels ? EFFECT_ALT_LEVELUP : EFFECT_LEVELUP;
            SNO snoPower = GlobalSNOGet(fUseAltLevels ? GLOBALSNO_POWERS_ALT_LEVELUP : GLOBALSNO_POWERS_LEVELUP);

            ACDPlayEffectForPlayer(tACD.id, tACD.id, eEffect, 0);

            if (tACD.IsAlive())
            {
                sPlayerLevelUpPower(tACD.id, snoPower);
            }
        }


        if (!fUseAltLevels)
        {
            SGameUpdateMonsterLevel(nLevel, FALSE);



            SPlayerAutoPurchaseSkillForNewLevel(tACD, *ptPlayer, nLevel);
        }


        SendPlayerLevelMessage(tACD.id, eAttribLevel);





















        {
            Achievement::EvaluationData tEvaluationData;
            tEvaluationData.SetPlayer(ptPlayer);

            tEvaluationData.EvaluateEvent(ptPlayer,

                fUseAltLevels ? Achievement::ACH_EVENT_LEVELUP_PARAGON : Achievement::ACH_EVENT_LEVELUP_HERO,
                nLevel, 1, (uint32)sub_71007b5e70());






            if (!fUseAltLevels)
            {
                tEvaluationData.EvaluateEvent(ptPlayer, Achievement::ACH_EVENT_HAS_HERO_AT_LEVEL, nLevel, 1, (uint32)sub_71007b5e70());
            }




        }

        sUpdateGameStatsForLevel(ptPlayer, nLevel, tACD);
    }
    else if (tACD.IsPet())
    {

        SHirelingLevelUp(tACD, nLevel);

        if (fUpdateClient)
        {
            sPlayerLevelUpPower(tACD.id, GlobalSNOGet(GLOBALSNO_POWERS_LEVELUP));
        }
    }
    else
    {
        DisplayInternalError(NULL, "", 467, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    BOOL bResetParam = GameIsParamFlagSet(GPF_RESET_LIFE_STATS_UPON_LEVEL_UP);
    if (fResetLifeStats && bWasAlive && bResetParam)
    {
        tACD.ResetLifeStats(TRUE, TRUE, TRUE, TRUE, FALSE);
    }
}

//@ 524
void sPVPExperienceRankUp(ActorCommonData & tACD)
{

    int32 nRank = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_PVP_RANK)) + 1;
    if (nRank > 100)
    {
        return;
    }


    tACD.AttributesSetInt(FastAttribKey(ATTRIBUTE_PVP_RANK), nRank);

    Player * ptPlayer = PlayerGetByACD(tACD.id);






    {
        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(ptPlayer);

        tEvaluationData.EvaluateEvent(ptPlayer, Achievement::ACH_EVENT_PVP_LEVELUP_HERO, nRank, 1, (uint32)sub_71007b5e70());
    }




    ScopedGameBalanceDefinitionLockR ptGameBalDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));
    const ExperienceTable & tExpTable = ptGameBalDef->tExperienceTable;

    uint64 uXPToLevelPrev = tExpTable.ptExperienceLevels[nRank - 1].nPVPXPToLevel;
    uint64 uXPToLevel = tExpTable.ptExperienceLevels[nRank].nPVPXPToLevel;

    uint64 uNewPVPXPDiff = uXPToLevel - uXPToLevelPrev;
    tACD.SetPvPExperienceNext(uNewPVPXPDiff);
}



//@ 570
#line 570
BOOL sExperienceGrantToACD(
    ActorCommonData & tACD,
    const uint64 nExp,
    BOOL bNotifyClient,
    BOOL bApplyRestXPBonus)
{
    if (GameIsParamFlagSet(GPF_NO_EXPERIENCE))
    {
        return FALSE;
    }

    BOOL fAwarded = FALSE;
    BOOL fLeveled = FALSE;

    uint64 nExpLeftToGrant = nExp;
    while (nExpLeftToGrant > 0)
    {
        int32 nLevelCap = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL_CAP));
        BOOL fUseAltLevels = tACD.ShouldUseAltLevels();
        if (fUseAltLevels)
        {
            nLevelCap = 20000;
        }

        int32 nLevel = tACD.AttributesGetInt(FastAttribKey(fUseAltLevels ? ATTRIBUTE_ALT_LEVEL : ATTRIBUTE_LEVEL));
        if (nLevel >= nLevelCap)
        {
            break;
        }


        uint64 nExpNext = (fUseAltLevels ? tACD.GetAltExperienceNext() : tACD.GetExperienceNext());
        if (nExpLeftToGrant >= nExpNext)
        {
            sExperienceLevelUp(tACD, bNotifyClient, fUseAltLevels, FALSE);
            nExpLeftToGrant -= nExpNext;
            // Inference: one line for both flags (rows put the else-branch `-=` at 611).
            fLeveled = fAwarded = TRUE;
            bNotifyClient = FALSE;
        }
        else
        {
            nExpNext -= nExpLeftToGrant;
            if (fUseAltLevels)
            {
                tACD.SetAltExperienceNext(nExpNext);
            }
            else
            {
                tACD.SetExperienceNext(nExpNext);
            }
            fAwarded = TRUE;
            break;
        }
    }

    if (fLeveled)
    {
        if (tACD.IsAlive())
        {
            tACD.ResetLifeStats(TRUE, TRUE, TRUE, TRUE, FALSE);
        }
        ScriptEventOnActorGainedLevel(tACD.id);
        PowerAPI_BuffApplyOnLevelEffect(tACD);
    }

    if (fAwarded && bApplyRestXPBonus)
    {
        float flRestXPBonusPct = tACD.AttributesGetFloat(FastAttribKey(ATTRIBUTE_REST_EXPERIENCE_BONUS_PCT));
        uint64 nRestXPToGrant = (uint64)(nExp * flRestXPBonusPct);
        uint64 nCurrentRestXP = tACD.GetRestExperience();
        nRestXPToGrant = Math::Min(nRestXPToGrant, nCurrentRestXP);
        if (nRestXPToGrant > 0)
        {
            sExperienceGrantToACD(tACD, nRestXPToGrant, bNotifyClient, FALSE);

            // 2.7.7: see GrantRestExperience; sub_71004b8ae0 refills rest XP to the full level.
            if (sub_71004b8ae0(&tACD))
            {
                tACD.SetRestExperience(tACD.ShouldUseAltLevels() ? tACD.GetAltExperienceNext() : tACD.GetExperienceNext());
            }
            else
            {
                nCurrentRestXP -= nRestXPToGrant;
                tACD.SetRestExperience(nCurrentRestXP);
            }
        }
        else if (nCurrentRestXP > 0 && sub_71004b8ae0(&tACD))
        {
            tACD.SetRestExperience(tACD.ShouldUseAltLevels() ? tACD.GetAltExperienceNext() : tACD.GetExperienceNext());
        }
    }

    return fAwarded;
}
#line 651

//@ 656
void sPVPExperienceGrantToACD(ActorCommonData & tACD, const uint64 nExp, BOOL bNotifyClient)



{
    BOOL bRankedUp = FALSE;
    uint64 nExpLeftToGrant = nExp;

    while (nExpLeftToGrant > 0)
    {
        int32 nRankCap = 100;
        int32 nRank = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_PVP_RANK));

        if (nRank >= nRankCap)
        {
            return;
        }

        uint64 nExpNext = tACD.GetPvPExperienceNext();
        if (nExpLeftToGrant >= nExpNext)
        {

            if (bRankedUp)
            {
                return;
            }




            sPVPExperienceRankUp(tACD);
            nExpLeftToGrant -= nExpNext;
            bRankedUp = TRUE;
        }
        else
        {
            nExpNext -= nExpLeftToGrant;

            tACD.SetPvPExperienceNext(nExpNext);
            return;
        }
    }
}

//@ 699
BOOL ExperienceGrantDirectly(ActorCommonData & tACDPlayer, const uint64 nExpGranted, const BOOL bNotifyClient, const BOOL bGrantToAllPlayersIfLocal)



{
    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (ptPlayer &&
        (ptPlayer->tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_HARDCORE)) &&
        !tACDPlayer.IsAlive() &&
        !tACDPlayer.IsDueling() &&
        !GameTypeIsPvP())
    {
        return FALSE;
    }











    if (GameIsWeeklyChallenge() || CmdLineGetParams()->tBitArray.Test(CMDLINE_BLIZZCON_BIT))
    {
        return FALSE;
    }


    if (ptPlayer && ptPlayer->bIsBeingDeleted)
    {
        return FALSE;
    }





    BOOL fAwardedToPlayer = sExperienceGrantToACD(tACDPlayer, nExpGranted, bNotifyClient, TRUE);




    if (ptPlayer && bGrantToAllPlayersIfLocal)
    {
        for (Player * ptCurrPlayer = PlayerGetFirstInGame(); ptCurrPlayer; ptCurrPlayer = PlayerGetNextInGame(ptCurrPlayer))
        {
            if (ptCurrPlayer != ptPlayer &&
                PlayersAreLocalToEachOther(*ptCurrPlayer, *ptPlayer))
            {
                ActorCommonData * ptACDCurrPlayer = ACDTryToGet(ptCurrPlayer->idPlayerACD);
                if (ptACDCurrPlayer)
                {
                    ExperienceGrantDirectly(*ptACDCurrPlayer, nExpGranted, bNotifyClient, FALSE);
                }
            }
        }
    }

    return fAwardedToPlayer;
}

//@ 764
void PVPExperienceGrantDirectly(ActorCommonData & tACDPlayer, const uint64 nPVPExpGranted, const BOOL bNotifyClient)



{
    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (ptPlayer &&
        (ptPlayer->tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_HARDCORE)) &&
        !tACDPlayer.IsAlive() &&
        !tACDPlayer.IsDueling() &&
        !GameTypeIsPvP())
    {
        return;
    }


    if (ptPlayer && ptPlayer->bIsBeingDeleted)
    {
        return;
    }





    sPVPExperienceGrantToACD(tACDPlayer, nPVPExpGranted, bNotifyClient);
}

//@ 794
void GrantRestExperience(ActorCommonData & tACDPlayer)
{

    if (!tACDPlayer.CanGainMoreExperience())
    {
        return;
    }

    uint64 nTotalXPForLevel;

    if (tACDPlayer.ShouldUseAltLevels())
    {
        uint64 nCurrentLevelXP = ExperienceGetCurrentLevelXP(tACDPlayer, TRUE);
        uint64 nXPLeftForLevel = tACDPlayer.GetAltExperienceNext();
        nTotalXPForLevel = nCurrentLevelXP + nXPLeftForLevel;
    }
    else
    {
        uint64 nCurrentLevelXP = ExperienceGetCurrentLevelXP(tACDPlayer, FALSE);
        uint64 nXPLeftForLevel = tACDPlayer.GetExperienceNext();
        nTotalXPForLevel = nCurrentLevelXP + nXPLeftForLevel;
    }


    ScopedGlobalDefinitionLockR pdefGlobal;
    uint64 nRestXPToGrant = (uint64)(pdefGlobal->flRestXPGrantLevelPercent * (float)nTotalXPForLevel);


    uint64 nCurrentRestXP = tACDPlayer.GetRestExperience();
    uint64 nNewRestXP = nCurrentRestXP + nRestXPToGrant;
    if (nNewRestXP > nCurrentRestXP)
    {




        uint64 nRestXPCap = (uint64)(pdefGlobal->flRestXPGrantLevelCapModifier * (float)nTotalXPForLevel);
        uint64 nRestXP = Math::Min(nNewRestXP, nRestXPCap);

        // 2.7.7: when sub_71004b8ae0 holds, rest XP is set to the full XP still needed for the level.
        if (sub_71004b8ae0(&tACDPlayer))
            nRestXP = tACDPlayer.ShouldUseAltLevels() ? tACDPlayer.GetAltExperienceNext() : tACDPlayer.GetExperienceNext();

        tACDPlayer.SetRestExperience(nRestXP);
    }
}

//@ 840
ACDID ExperienceGetUltimateOwner(const ActorCommonData & tACD)
{
    ACDID idACDUltimateOwner = ACDID_INVALID;

    if (tACD.GetPetOwnerPlayer())
    {
        idACDUltimateOwner = ACDGetPetOwner(tACD.id);
    }
    else if (ACDIsPlayer(tACD.id))
    {
        idACDUltimateOwner = tACD.id;
    }
    else if (ACDIsSummoned(tACD.id))
    {
        idACDUltimateOwner = ACDGetSummoner(tACD.id);
    }

    return idACDUltimateOwner;
}

//@ 882
int32 ExperienceGetPlayersEligibleForKill(ACDID idACDVictim, PlayerList & listPlayersToEarnXP)
{
    ActorCommonData & tACDVictim = ACDGet(idACDVictim);
    Team eVictimTeam = tACDVictim.GetTeam();
    BOOL bLRBoss = FALSE;

    if (tACDVictim.eMonsterRarity == MR_BOSS)
    {
        WorldPlace placeVictim = tACDVictim.GetPlace();
        SWorld * ptSWorld = SWorldTryToGet(placeVictim.idSWorld);
        bLRBoss = ptSWorld && SWorldIsDungeonFinder(ptSWorld->id);
    }

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (TeamsAreFriendly(ptPlayer->eTeam, eVictimTeam))
        {
            continue;
        }

        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);

        if (ptACDPlayer && ACDIsInTieredLootRun(*ptACDPlayer) &&
            !ACDIsInTieredLootRun(tACDVictim))
        {
            continue;
        }

        BOOL bPlayerTagged = tACDVictim.GetExpTagPlayer(*ptPlayer);

        if (bPlayerTagged ||
            (bLRBoss && NephalemRiftManager::PlayerPaidForRift(ptPlayer)) ||
            ExperiencePlayersCloseEnough(tACDVictim, ptPlayer->idPlayerACD))
        {
            listPlayersToEarnXP.AddTail_Tracked(ptPlayer);
        }
    }

    return listPlayersToEarnXP.Count();
}

//@ 941
static const int32 LEVEL_DIFFERENCE_TO_GET_ADDITIVE_BONUSES = -5;

//@ 943
uint64 ExperienceApplyModifiers(uint64 nBaseExp, SNO snoActorVictim, int32 nVictimLevel, int32 nPlayerLevel, float flHirelingExpBonus, float flExpMultiplierForPlayer, float flExperienceBonus, float flExperienceBonusNoPenalty)








{
    uint64 nExpForPlayer = nBaseExp;


    if (snoActorVictim != SNO_NONE && nVictimLevel > nPlayerLevel)
    {

        float flVictimLevelXP = GetMonsterLevelAttribute(nVictimLevel, SNO_NONE, 58);
        if (flVictimLevelXP > EPSILON)
        {
            float flPlayerLevelXP = GetMonsterLevelAttribute(nPlayerLevel, SNO_NONE, 58);
            float flXPRatio = flPlayerLevelXP / flVictimLevelXP;
            nExpForPlayer = (uint64)(flXPRatio * (float)nExpForPlayer);
        }
    }

    int32 nLevelDifference = nVictimLevel - nPlayerLevel;
    if (GameIsParamFlagSet(GPF_ADJUST_EXP_WITH_RELATIVE_LEVEL))
    {
        nExpForPlayer = sExpAdjustForRelativeLevel(nLevelDifference, nExpForPlayer);
    }


    if (nLevelDifference >= LEVEL_DIFFERENCE_TO_GET_ADDITIVE_BONUSES)
    {
        nExpForPlayer += (int64)flExperienceBonus + (int64)flHirelingExpBonus;

    }


    nExpForPlayer += (int64)flExperienceBonusNoPenalty;


    nExpForPlayer = (uint64)((float)nExpForPlayer * flExpMultiplierForPlayer);


    return Math::Max(nExpForPlayer, (uint64)1);
}

//@ 993
void ExperienceGainForKill(ACDID idACDVictim, ACDID idACDKiller, BOOL bGainXP, BOOL bDropLoot, uint32 dwGameEventNumber)
{
    ActorCommonData & tACDVictim = ACDGet(idACDVictim);

    if (GameTypeIsPvP() || GameIsWeeklyChallenge() || CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        return;
    }

    if (tACDVictim.IsDueling())
    {
        return;
    }

    if (tACDVictim.IsPet())
    {
        return;
    }

    PlayerList listPlayersToEarnXP(PlayerListGetElementPool());
    int32 nNumPlayers = ExperienceGetPlayersEligibleForKill(idACDVictim, listPlayersToEarnXP);

    if (nNumPlayers == 0)
    {
        return;
    }

    ACDID idACDPrimaryPlayer = listPlayersToEarnXP.GetHead()->idPlayerACD;
    uint64 nBaseExp = tACDVictim.GetExperienceGranted();
    int32 nVictimLevel = tACDVictim.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));

    if (!tACDVictim.AttributesGetInt(FastAttribKey(ATTRIBUTE_GRANTS_NO_XP)))
    {
        tACDVictim.TagForExp(idACDKiller);

        float flAvgExpMultiplier = 0.0f;
        float flAvgExperienceBonus = 0.0f;
        float flAvgExperienceBonusNoPenalty = 0.0f;
        float flAvgHirelingExpBonus = 0.0f;
        int32 nEligiblePlayerCount = 0;
        ScopedGlobalDefinitionLockR pdefGlobal;

        POS pos = listPlayersToEarnXP.GetHeadPosition();
        while (pos)
        {
            Player * ptPlayer = listPlayersToEarnXP.GetNext(pos);

            ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
            if (bGainXP &&
                tACDPlayer.IsAlive())
            {
                nEligiblePlayerCount++;

                float flExpMultiplierForPlayer = ExperienceGetBonusFromGear(tACDPlayer);

                if (!ACDIsInTieredLootRun(tACDVictim))
                {
                    flExpMultiplierForPlayer *= 1.0f + Math::Max(0.0f, tACDPlayer.AttributesGetFloat(FastAttribKey(ATTRIBUTE_EXPERIENCE_BONUS_PCT_HANDICAP)));
                }

                float flHirelingExpBonus = ExperienceGetHirelingBonus(tACDPlayer);

                flExpMultiplierForPlayer *= tACDPlayer.AttributesGetFloat(FastAttribKey(ATTRIBUTE_EXPERIENCE_BONUS_PCT_SUPER_SCALAR_TOTAL));

                float flExperienceBonus = tACDPlayer.AttributesGetFloat(FastAttribKey(ATTRIBUTE_EXPERIENCE_BONUS));
                float flExperienceBonusNoPenalty = tACDPlayer.AttributesGetFloat(FastAttribKey(ATTRIBUTE_EXPERIENCE_BONUS_NO_PENALTY));

                flAvgExpMultiplier += flExpMultiplierForPlayer;
                flAvgExperienceBonus += flExperienceBonus;
                flAvgExperienceBonusNoPenalty += flExperienceBonusNoPenalty;
                flAvgHirelingExpBonus += flHirelingExpBonus;
            }
        }

        if (nEligiblePlayerCount > 1)
        {
            flAvgExpMultiplier /= nEligiblePlayerCount;
            flAvgExperienceBonus /= nEligiblePlayerCount;
            flAvgExperienceBonusNoPenalty /= nEligiblePlayerCount;
            flAvgHirelingExpBonus /= nEligiblePlayerCount;
        }

        pos = listPlayersToEarnXP.GetHeadPosition();
        while (pos)
        {
            Player * ptPlayer = listPlayersToEarnXP.GetNext(pos);

            ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
            if (bGainXP)
            {
                if (tACDPlayer.IsAlive())
                {
                    uint64 nExpForPlayer;
                    int32 nPlayerLevel = tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));

                    if (tACDPlayer.AttributesGetInt(FastAttribKey(ATTRIBUTE_UPSCALED_LEVEL)) > 1)
                    {
                        nExpForPlayer = ActorCalculateXPForMonster(tACDVictim, nPlayerLevel);

                        if (nVictimLevel - nPlayerLevel >= LEVEL_DIFFERENCE_TO_GET_ADDITIVE_BONUSES)
                        {
                            nExpForPlayer += Math::FloatRoundToInt(flAvgExperienceBonus);
                            nExpForPlayer += (uint64)flAvgHirelingExpBonus;
                        }

                        nExpForPlayer += Math::FloatRoundToInt(flAvgExperienceBonusNoPenalty);

                        nExpForPlayer = (int64)((float)nExpForPlayer * flAvgExpMultiplier);
                    }
                    else
                    {
                        nExpForPlayer = ExperienceApplyModifiers(nBaseExp, tACDVictim.snoActor, nVictimLevel, nPlayerLevel, flAvgHirelingExpBonus, flAvgExpMultiplier, flAvgExperienceBonus, flAvgExperienceBonusNoPenalty);
                    }

                    ExperienceGrantDirectly(tACDPlayer, nExpForPlayer, TRUE, FALSE);

                    TraceStat("exp", tACDPlayer.id, "%u\t%u", dwGameEventNumber, nExpForPlayer);

                    tACDPlayer.AttributesSetInt(FastAttribKey(ATTRIBUTE_ITEM_UNLUCKY_BONUS_ALLOW), 1);
                }

                GoldGainForKill(tACDVictim.id, tACDPlayer.id);
            }
        }
    }

    if (bDropLoot)
    {
        if (LootDropLootForEligiblePlayers(tACDVictim, listPlayersToEarnXP, idACDPrimaryPlayer, dwGameEventNumber, FALSE, 0.0f, FALSE))
        {
            LootDropCatchupLoot(tACDVictim, listPlayersToEarnXP, dwGameEventNumber);
        }
    }
}

//@ 1149
void ExperienceDropLootWithinExpRange(ActorCommonData & tACDLootee, const ACDID idACDPrimaryLooter, uint32 dwGameEventNumber, BOOL fAutoIdentify, float flCameraBiasDist)




{
    PlayerList listEligiblePlayers(PlayerListGetElementPool());

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (ptPlayer->idPlayerACD == idACDPrimaryLooter ||
            ExperiencePlayersCloseEnough(tACDLootee, ptPlayer->idPlayerACD))
        {
            listEligiblePlayers.AddTail_Tracked(ptPlayer);
        }
    }

    LootDropLootForEligiblePlayers(tACDLootee, listEligiblePlayers, idACDPrimaryLooter, dwGameEventNumber, fAutoIdentify, flCameraBiasDist, FALSE);
}

//@ 1172
void ExperienceDropLootForAll(ActorCommonData & tACDLootee, const ACDID idACDPrimaryLooter, uint32 dwGameEventNumber, BOOL fAutoIdentify, float flCameraBiasDist)




{
    PlayerList listEligiblePlayers(PlayerListGetElementPool());

    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);


        if (ptACDPlayer && ACDIsInTieredLootRun(*ptACDPlayer) && !ACDIsInTieredLootRun(tACDLootee))
        {
            continue;
        }

        listEligiblePlayers.AddTail_Tracked(ptPlayer);
    }



    LootDropLootForEligiblePlayers(tACDLootee, listEligiblePlayers, idACDPrimaryLooter, dwGameEventNumber, fAutoIdentify, flCameraBiasDist, TRUE);
}

//@ 1200
void GoldGainForKill(ACDID idACDVictim, ACDID idACDPlayer)
{

    if (!GameIsParamFlagSet(GPF_GRANT_GOLD_UPON_KILL))
    {
        return;
    }

    ActorCommonData & tACDVictim = ACDGet(idACDVictim);
    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);

    int32 nAmountToGrant = tACDVictim.AttributesGetInt(FastAttribKey(ATTRIBUTE_GOLD_GRANTED));
    tACDPlayer.ModifyCurrencyAmount(nAmountToGrant, CURRENCY_GOLD, GMR_KILL, FALSE);
}

//@ 1218
void ExperienceSetLevel(const ACDID idACD, int32 nLevels, const BOOL fUpdateClient)



{
    BOOL fUpdate = fUpdateClient;
    ActorCommonData & tACD = ACDGet(idACD);
    int32 nLevelCap = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL_CAP));
    int32 nNormalLevels = Math::Pin(nLevels, 1, nLevelCap);
    BOOL fAwarded = FALSE;
    int32 nKillerLevel = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));



    for (int32 i = nKillerLevel; i < nNormalLevels; i++)
    {
        BOOL fResetLifeStats = (i == nNormalLevels - 1) && (nLevels <= nLevelCap);
        sExperienceLevelUp(tACD, fUpdate, FALSE, fResetLifeStats);
        fUpdate = FALSE;
        fAwarded = TRUE;
    }

    int32 nStartAltLevel = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_ALT_LEVEL));
    int32 nAltLevels = Math::Pin(nLevels - nLevelCap, 0, 20000);
    for (int32 i = nStartAltLevel; i < nAltLevels; i++)
    {
        BOOL fResetLifeStats = (i == nAltLevels - 1);
        sExperienceLevelUp(tACD, fUpdate, TRUE, fResetLifeStats);
        fUpdate = FALSE;
        fAwarded = TRUE;
    }



    if (fAwarded)
    {
        ScriptEventOnActorGainedLevel(tACD.id);
        PowerAPI_BuffApplyOnLevelEffect(tACD);
    }
}

//@ 1293
void ExperienceUpdatePerLevelHitpointFactorVitality(ActorCommonData & tACD)
{

    int32 nLevel = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_LEVEL));



    int32 nUpscaledLevel = tACD.AttributesGetInt(FastAttribKey(ATTRIBUTE_UPSCALED_LEVEL));

    nLevel = Math::Max(nUpscaledLevel, nLevel);




    ScopedGameBalanceDefinitionLockR ptGameBalDef(GlobalSNOGet(GLOBALSNO_GB_EXPERIENCE));
    const ExperienceTable & tExpTable = ptGameBalDef->tExperienceTable;
    float flLifePerVitality = tExpTable.ptExperienceLevels[nLevel - 1].flLifePerVitality;

    tACD.AttributesSetFloat(FastAttribKey(ATTRIBUTE_HITPOINTS_FACTOR_VITALITY), flLifePerVitality);
}
