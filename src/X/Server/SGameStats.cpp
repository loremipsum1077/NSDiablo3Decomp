// Original file: X/Server/SGameStats.cpp
// Partly written: functions sit on their original lines (see the reference .cpp).

#include "X/Server/SGameStats.h"

#include "SGameManager.h"
#include "../Common/GameCommonData.h"
#include "../Common/GameConstants.h"
#include "../Sigma/Math/MathGeneral.h"
#include "../Common/Player.h"
#include "../Sigma/RefString.h"
#include "../Common/ActorCommonData.h"
#include "../Common/Powers/PowersCommon.h"
#include "../Common/TeamCommon.h"
#include "../Common/Items.h"
#include "Powers/PowerAPI.h"
#include "Powers/Powers.h"
#include "System/SNephalemRift.h"
#include "../Common/GlobalSNO.h"
#include "../Common/Quests/QuestsSwig.h"
#include "../Common/Achievements/AchievementDefines.h"
// Declared locally until their headers are adopted; the comment names the defining file.
typedef int32 HirelingClass;  // X/Common/Hireling.h
HirelingClass HirelingGetClass(const ActorCommonData & tACDHireling);  // X/Common/Hireling.cpp:222
BOOL ExperiencePlayersCloseEnough(const ActorCommonData & tACDVictim, ACDID idACDOtherPlayer);  // X/Common/ExperienceCommon.cpp:50

#include "Achievements/AchievementModifierEvaluationLogic.h"
extern "C" uint32 sub_71007b75d0();  // 2.7.7 only, unnamed (SGame.cpp); returns two game-flag bits (quantity of ACH_EVENT_PLAYER_DIED)
namespace Achievement {
typedef int32 EventID;











}  // namespace Achievement

int32 ItemGetStatsDataKey(const ActorCommonData & tACDItem);  // X/Common/Items.cpp:2250
enum {
    LC_TREASUREBAG = 1,  // X/Server/Loot/Loot.h
    LC_RANDOMITEMVENDOR = 2,  // X/Server/Loot/Loot.h
};
struct LooteeInfo;

uint32 PlayerGetPlayedSeconds(const Player & tPlayer);  // X/Common/Player.cpp

namespace CombatLog {
typedef int32 LogEventType;  // X/Common/CombatLog.h
enum {
    COMBATLOG_EVENT_POWER_USED = 0,  // invented name; the value is from the binary
    COMBATLOG_EVENT_KILL = 9,  // invented name; the value is from the binary
};
void LogEvent(LogEventType eType, ACDID idACD1, ACDID idACD2, SNO snoData, float flParam1, float flParam2, uint32 dwParam1);  // X/Common/CombatLog.cpp:610
}

// Invented: the original's hash for StatCounterKey is not in the DWARF; the binary hashes the
// key by its eType alone.
template <> inline uint32 XHashKey<StatCounterKey>(StatCounterKey key) { return key.eType; }

// The bodies of these SGameStats.h inlines are not in the DWARF; their spelling is inferred.
inline StatCounterKey::StatCounterKey()
{
}
inline StatCounterKey::StatCounterKey(const StatCounterType eTypeIn, const int32 nParam0In, const int32 nParam1In)
{
    eType = eTypeIn;
    nParam0 = nParam0In;
    nParam1 = nParam1In;
}
inline BOOL StatCounterKey::operator==(const StatCounterKey & tKey) const
{
    return eType == tKey.eType && nParam0 == tKey.nParam0 && nParam1 == tKey.nParam1;
}

void SGameStatsGenerateLogForPlayer(ACDID idACDPlayer, CRefString & strOut);
void SGameStatsCounterIncrement(StatCounterType eType, int32 nParam0, int32 nParam1, float flValue);
// X/Common/Marker/Marker.h:72, definition Marker.cpp:176.
const CHAR * MarkerNameGetString(uint32 dwHash);
// SGameStats.cpp:1548; genuine game-stat caller below needs this discarded standalone helper.
float SGameStatsCounterFetch(StatCounterType eType, int32 nParam0, int32 nParam1);
// DWARF World.h:97; original definition World.cpp:544.
SNO WorldCommonGetWorldSNO(SWorldID idSWorld);
#include "../Common/CombatLog.h"





































































































































static uint32 sStatCounterTypeGetAlertLevel(StatCounterType eType, int32)
{
    switch (eType)
    {
    case STAT_COUNTER_PLAYER_RECIPE_CRAFTED:
        return 256;
    case STAT_COUNTER_PLAYER_ITEM_SALVAGED:
    case STAT_COUNTER_PLAYER_ITEM_BOUGHT:
    case STAT_COUNTER_PLAYER_ITEM_VENDORED:
    case STAT_COUNTER_PLAYER_ITEM_RECEIVED_IN_TRADE:
        return 32;
    default:
        return 0;
    }
}





















void SGameStatsInitializeForApp()
{
}




void SGameStatsDisposeForApp()
{
}

// Defined below at its DWARF line; no earlier declaration is in the DWARF.
void SGameStatsRecordGameStats();
// NON_MATCHING: 1496 vs 1496 bytes; 11/374 insns differ, register swaps only in the inlined XPooledMap::Initialize stores of the MAX_PLAYERS loop (lines 292-296); tried ++i.
void SGameStatsInitializeForGame()

{
    SGame & tSGame = SGameManager::GameGet();
    SGameStatGlobals *& ptSGameStatGlobals = tSGame.ptSGameStatGlobals;
    ptSGameStatGlobals = (SGameStatGlobals *)SigmaMemoryNew(sizeof(SGameStatGlobals), 4, tSGame.ptMemoryInterface, TRUE);

    ptSGameStatGlobals->poolStatTimedEventMap.Initialize();
    ptSGameStatGlobals->poolStatTimedEventMap.SetMemoryInterface(AppGetCurrentMemoryInterface());
    ptSGameStatGlobals->poolStatTimedEventMap.SetBlockCount_Tracked(1024);

    ptSGameStatGlobals->poolStatCounterMap.Initialize();
    ptSGameStatGlobals->poolStatCounterMap.SetMemoryInterface(AppGetCurrentMemoryInterface());
    ptSGameStatGlobals->poolStatCounterMap.SetBlockCount_Tracked(1024);

    ptSGameStatGlobals->mapStatTimedEvents.Initialize(&ptSGameStatGlobals->poolStatTimedEventMap);
    ptSGameStatGlobals->mapStatCounters.Initialize(&ptSGameStatGlobals->poolStatCounterMap);

    for (int32 i = 0; i < MAX_PLAYERS; i++)
    {
        ptSGameStatGlobals->mapPlayerStatTimedEvents[i].Initialize(&ptSGameStatGlobals->poolStatTimedEventMap);
        ptSGameStatGlobals->mapPlayerStatCounters[i].Initialize(&ptSGameStatGlobals->poolStatCounterMap);

        ptSGameStatGlobals->mapPlayerCurrentLifeStatTimedEvents[i].Initialize(&ptSGameStatGlobals->poolStatTimedEventMap);
        ptSGameStatGlobals->mapPlayerCurrentLifeStatCounters[i].Initialize(&ptSGameStatGlobals->poolStatCounterMap);
    }

    ptSGameStatGlobals->bCheatUsed = FALSE;

    ptSGameStatGlobals->uMaxNumberOfPlayers = 0;

    for (int32 i = 0; i < 7; i++)
    {
        ptSGameStatGlobals->uTotalNumberOfPlayersByClass[i] = 0;
        ptSGameStatGlobals->flTotalPlayTimeByClass[i] = 0.0f;
    }

    ptSGameStatGlobals->uTotalGoldDropped = 0;
    ptSGameStatGlobals->uMaxGoldDroppedAtOnce = 0;

    ptSGameStatGlobals->bInitialized = TRUE;

    ptSGameStatGlobals->tStartTime = Blizzard::Time::LocalBias(Blizzard::Time::GetTimestamp());
    ptSGameStatGlobals->tEndTime = Blizzard::Time::LocalBias(Blizzard::Time::GetTimestamp());

    UpdateFunctionInitialize(UPDATE_STATS, DATAID_INVALID, ptSGameStatGlobals->tStatsUpdateData);
    ServerQueueUpdateFunction(1, ptSGameStatGlobals->tStatsUpdateData);
}



void SGameStatsDisposeForGame()

{

    SGameStatsRecordGameStats();


    SGame & tSGame = SGameManager::GameGet();
    SGameStatGlobals * ptSGameStatGlobals = tSGame.ptSGameStatGlobals;

    if (!GameActiveGameCommonDataIsClient())
    {
        ServerRemoveUpdateFunction(ptSGameStatGlobals->tStatsUpdateData);
    }

    ptSGameStatGlobals->mapStatTimedEvents.Dispose();
    ptSGameStatGlobals->mapStatCounters.Dispose();

    for (int32 i = 0; i < MAX_PLAYERS; i++)
    {
        ptSGameStatGlobals->mapPlayerStatTimedEvents[i].Dispose();
        ptSGameStatGlobals->mapPlayerStatCounters[i].Dispose();

        ptSGameStatGlobals->mapPlayerCurrentLifeStatTimedEvents[i].Dispose();
        ptSGameStatGlobals->mapPlayerCurrentLifeStatCounters[i].Dispose();
    }

    ptSGameStatGlobals->poolStatCounterMap.Dispose();
    ptSGameStatGlobals->poolStatTimedEventMap.Dispose();

    SigmaMemoryFree(tSGame.ptSGameStatGlobals, tSGame.ptMemoryInterface); tSGame.ptSGameStatGlobals = NULL;
}



void SGameStatsUpdate()
{
    SGameStatGlobals * ptSGameStatGlobals = SGameManager::GameGet().ptSGameStatGlobals;
    ServerUpdateWasCalled(ptSGameStatGlobals->tStatsUpdateData);
    ServerQueueUpdateFunction(1, ptSGameStatGlobals->tStatsUpdateData);
}



















































void sGameStatsRecordPlayerKilledMonster(ActorCommonData & tACDMonsterVictim, Player * ptPlayerKiller, const PowerAPIDamage * ptPowerDamage)
{
    if (tACDMonsterVictim.GetPetOwnerPlayer() != ptPlayerKiller)
    {
        SGameStatsPlayerCounterIncrement(ptPlayerKiller, STAT_COUNTER_PLAYER_MONSTER_KILLED, tACDMonsterVictim.snoActor, 0, 1.0f);
        SGameStatsPlayerCounterIncrement(ptPlayerKiller, STAT_COUNTER_PLAYER_MONSTERS_KILLED, 0, 0, 1.0f);

        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetACD(&tACDMonsterVictim);
        tEvaluationData.SetPlayer(ptPlayerKiller);
        tEvaluationData.SetPower(ptPowerDamage);
        tEvaluationData.EvaluateEvent(ptPlayerKiller, 109, tACDMonsterVictim.snoActor, 1, (uint32)sub_71007b5e70());
    }
}


















void sGameStatsRecordMonsterKilledMonster(ActorCommonData & tACDMonsterVictim, ActorCommonData & tACDMonsterKiller, const PowerAPIDamage * ptPowerDamage)
{
    {
        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetACD(&tACDMonsterKiller);
        tEvaluationData.SetPower(ptPowerDamage);
        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            if (!TeamsAreFriendly(ptPlayer->eTeam, tACDMonsterVictim.GetTeam()) && ExperiencePlayersCloseEnough(tACDMonsterVictim, ptPlayer->idPlayerACD))
            {
                tEvaluationData.SetPlayer(ptPlayer);
                tEvaluationData.EvaluateEvent(ptPlayer, 104, tACDMonsterVictim.snoActor, 1, (uint32)sub_71007b5e70());
            }
        }
    }
}























void sGameStatsRecordFollowerKilledMonster(ActorCommonData & tACDMonsterVictim, ActorCommonData & tACDMonsterKiller, Player * ptControllingPlayer, const PowerAPIDamage * ptPowerDamage)
{
    {
        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetACD(&tACDMonsterKiller);
        tEvaluationData.SetPower(ptPowerDamage);
        tEvaluationData.SetPlayer(ptControllingPlayer);
        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer != NULL; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            tEvaluationData.EvaluateEvent(ptPlayer, 111, tACDMonsterVictim.snoActor, 1, (uint32)sub_71007b5e70());
        }
    }
}

















void sGameStatsRecordPlayerKilled(
    ActorCommonData & tACDPlayerVictim,
    ACDID idACDKiller,
    const PowerAPIDamage * ptPowerDamage)
{
	Player *ptPlayerVictim = PlayerGetByACD(tACDPlayerVictim.id);


	SNO snoLevelArea = ACDGetLevelArea(tACDPlayerVictim);
	SGameStatsPlayerCounterIncrement(ptPlayerVictim, STAT_COUNTER_PLAYER_KILLED_IN_LEVEL_AREA, snoLevelArea, 0, 1.0f);

	WorldPlace tPlace = tACDPlayerVictim.GetPlace();
	SNO snoWorld = WorldCommonGetWorldSNO(tPlace.idSWorld);
	SGameStatsPlayerCounterIncrement(ptPlayerVictim, STAT_COUNTER_PLAYER_KILLED_IN_WORLD, snoWorld, 0, 1.0f);







	ActorCommonData * ptACDKiller = ACDTryToGet(idACDKiller);
	if (ptACDKiller && ptACDKiller->IsMonster())
	{
		// Inference: the original unsigned range check accepts champion, rare and rare-minion rarities.
		if (ptACDKiller->eMonsterRarity == MR_CHAMPION || ptACDKiller->eMonsterRarity == MR_RARE || ptACDKiller->eMonsterRarity == MR_RARE_MINION)
		{


			// Inference: the original unrolls all eight affixes, skipping GBID_NONE entries.
			for (int i = 0; i < NUM_MONSTER_AFFIXES; ++i)
			{
				GBID gbidAffix = ptACDKiller->pMonsterAffixGBIDs[i];
				if (gbidAffix != GBID_NONE)
				{

					SGameStatsPlayerCounterIncrement(ptPlayerVictim, STAT_COUNTER_PLAYER_KILLED_BY_MONSTER_AFFIX, gbidAffix, ptACDKiller->GetActorSNO(), 1.0f);
				}
			}
		}

		SGameStatsPlayerCounterIncrement(ptPlayerVictim, STAT_COUNTER_PLAYER_KILLED_BY_MONSTER, ptACDKiller->snoActor, 0, 1.0f);
	}




    // Inference: the evaluation scope ends at the original destructor row 574.
    {
        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(ptPlayerVictim);
        tEvaluationData.SetACD(ptACDKiller);
        tEvaluationData.SetPower(ptPowerDamage);

        tEvaluationData.EvaluateEvent(ptPlayerVictim, Achievement::ACH_EVENT_PLAYER_DIED, 1, sub_71007b75d0(), (uint32)sub_71007b5e70());



    }
}



void sGameStatsRecordHirelingKilled(ActorCommonData & tACDHirelingVictim, ACDID idACDKiller)
{
    ActorCommonData * ptACDKiller = ACDTryToGet(idACDKiller);
    if (ptACDKiller != NULL)
    {
        SGameStatsPlayerCounterIncrement(tACDHirelingVictim.GetPetOwnerPlayer(), STAT_COUNTER_HIRELING_KILLED_BY_MONSTER, ptACDKiller->snoActor, HirelingGetClass(tACDHirelingVictim), 1.0f);
    }
}


#include "PlayerList.h"
// X/Server/Experience.cpp:882; no header declaration in DWARF.
int32 ExperienceGetPlayersEligibleForKill(ACDID idACDVictim, PlayerList & listPlayersToEarnXP);
// X/Common/ActorCommonData.cpp:8206; no header declaration in DWARF.
BOOL ACDIsFollower(const ActorCommonData & tACD);


void sGameStatsRecordMonsterKilled(
    ActorCommonData & tACDMonsterVictim,
    ACDID idACDKiller,
    const PowerAPIDamage * ptDamage)
{
	ActorCommonData * ptACDKiller = ACDTryToGet(idACDKiller);
	Player * ptPlayerKiller = ACDGetControllingPlayer(idACDKiller);
	if (ptPlayerKiller != NULL)
	{
		sGameStatsRecordPlayerKilledMonster(tACDMonsterVictim, ptPlayerKiller, ptDamage);

		if (ptACDKiller != NULL && (ACDIsFollower(*ptACDKiller) || ACDIsHireling(*ptACDKiller)))
		{
			sGameStatsRecordFollowerKilledMonster(tACDMonsterVictim, *ptACDKiller, ptPlayerKiller, ptDamage);
		}


		ptPlayerKiller->tConsoleCounters.uMonstersKilled++;
	}

	else
	{
		if (ptACDKiller != NULL && ptACDKiller->IsMonster())
		{
			sGameStatsRecordMonsterKilledMonster(tACDMonsterVictim, *ptACDKiller, ptDamage);
		}
	}





	{
		Achievement::EvaluationData tEvaluationData;
		tEvaluationData.SetACD(&tACDMonsterVictim);
		tEvaluationData.SetPlayer(ptPlayerKiller);
		tEvaluationData.SetPower(ptDamage);




		PlayerList listPlayersToEvaluate(PlayerListGetElementPool());
		ExperienceGetPlayersEligibleForKill(tACDMonsterVictim.id, listPlayersToEvaluate);

		if (ptPlayerKiller != NULL)
		{

			if (listPlayersToEvaluate.Find(ptPlayerKiller) == NULL)
			{
				listPlayersToEvaluate.AddHead_Tracked(ptPlayerKiller);
			}
		}

		POS pos = listPlayersToEvaluate.GetHeadPosition();
		while (pos != NULL)
		{
			Player * ptPlayer = listPlayersToEvaluate.GetNext(pos);

			tEvaluationData.SetPlayer(ptPlayer);
			tEvaluationData.SetEquipmentInfo(ptPlayer->idPlayerACD);

			tEvaluationData.EvaluateEvent(
				ptPlayer,
				Achievement::ACH_EVENT_MONSTER_KILLED_BY_XP_ELIGIBLE_PLAYER,
				tACDMonsterVictim.snoActor,
				1, (uint32)sub_71007b5e70());

			// Inference: the original bit test accepts champion, rare, unique and boss rarities.

			if (tACDMonsterVictim.eMonsterRarity == MR_CHAMPION || tACDMonsterVictim.eMonsterRarity == MR_RARE || tACDMonsterVictim.eMonsterRarity == MR_UNIQUE || tACDMonsterVictim.eMonsterRarity == MR_BOSS)
			{






				if (!tACDMonsterVictim.AttributesGetInt(ATTRIBUTE_DROPS_NO_LOOT))
				{
					tEvaluationData.EvaluateEvent(
						ptPlayer,
						Achievement::ACH_EVENT_ELITE_MONSTER_KILLED,
						tACDMonsterVictim.snoActor,
						1, (uint32)sub_71007b5e70());

					SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_ELITES_KILLED, 0, 0, 1.0f);


					ptPlayer->tConsoleCounters.uElitesKilled++;
				}
			}
		}

	}
}




void SGameStatsRecordKill(ActorCommonData & tACDVictim, ACDID idACDKiller, const PowerAPIDamage * ptDamage)
{
    SNO snoPower = SNO_NONE;
    SGame & tSGame = SGameManager::GameGet();
    SGameStatGlobals * ptSGameStatGlobals = tSGame.ptSGameStatGlobals;

    if (tACDVictim.IsMonster() && !tACDVictim.AttributesGetInt(ATTRIBUTE_IS_PLAYER_DECOY) && !tACDVictim.AttributesGetInt(ATTRIBUTE_ILLUSION))
    {
        sGameStatsRecordMonsterKilled(tACDVictim, idACDKiller, ptDamage);
        SGameGlobals & tSGameGlobals = SGameGlobalsGet();
        tSGameGlobals.nMonstersKilled++;
    }
    else if (ACDIsPlayer(tACDVictim.id))
    {
        sGameStatsRecordPlayerKilled(tACDVictim, idACDKiller, ptDamage);
    }
    else if (ACDIsHireling(tACDVictim))
    {
        sGameStatsRecordHirelingKilled(tACDVictim, idACDKiller);
    }

    if (ptDamage != NULL)
    {
        Power * ptPower = PowerTryToGet(ptDamage->idPower);
        if (ptPower != NULL)
        {
            snoPower = ptPower->snoPower;
        }
    }

    CombatLog::LogEvent(CombatLog::COMBATLOG_EVENT_KILL, idACDKiller, tACDVictim.id, snoPower, 0.0f, 0.0f, 0);
}

















void SGameStatsRecordPowerUse(ActorCommonData & tACD, SNO snoPower, PowerID, ACDID idACDTarget)
{

    if (snoPower == SNO_NONE)
    {
        return;
    }





    SGame & tSGame = SGameManager::GameGet();
    SGameStatGlobals * ptSGameStatGlobals = tSGame.ptSGameStatGlobals;

    if (ACDIsPlayer(tACD.id))
    {
        Player * ptPlayerMain = PlayerGetByACD(tACD.id);
        if (ptPlayerMain != NULL)
        {
            SGameStatsPlayerCounterIncrement(ptPlayerMain, STAT_COUNTER_PLAYER_POWER_USED, snoPower, PowerGetRuneTypeForACD(tACD, snoPower), 1.0f);
            SGameStatsPlayerCounterIncrement(ptPlayerMain, STAT_COUNTER_PLAYER_POWER_USED_ALL_RUNES, snoPower, 0, 1.0f);
        }
    }

    if (ACDIsHireling(tACD))
    {
        HirelingClass eClass = HirelingGetClass(tACD);
        SGameStatsPlayerCounterIncrement(tACD.GetPetOwnerPlayer(), STAT_COUNTER_HIRELING_POWER_USED, snoPower, eClass, 1.0f);
    }

    CombatLog::LogEvent(CombatLog::COMBATLOG_EVENT_POWER_USED, tACD.id, idACDTarget, snoPower, 0.0f, 0.0f, 0);
}









void SGameStatsRecordLoot(ActorCommonData & tACDItem, LootContext eLootContext, const LooteeInfo * ptLootee)
{
    ItemQualityLevel eIQL = ItemGetQualityLevel(tACDItem);
    SGame & tSGame = SGameManager::GameGet();
    SGameStatGlobals * ptSGameStatGlobals = tSGame.ptSGameStatGlobals;
    GBID gbidItem = tACDItem.hGB.gbid;

    if (gbidItem == GBID_NONE)
    {
        return;
    }

    if (ItemIsGold(tACDItem))
    {
        uint32 uGold = tACDItem.AttributesGetInt(ATTRIBUTE_GOLD);
        ptSGameStatGlobals->uMaxGoldDroppedAtOnce = Math::Max(uGold, ptSGameStatGlobals->uMaxGoldDroppedAtOnce);
        ptSGameStatGlobals->uTotalGoldDropped += uGold;
        return;
    }

    SGameStatsCounterIncrement(STAT_COUNTER_GAME_ITEM_DROPPED, tACDItem.hGB.gbid, ItemGetStatsDataKey(tACDItem), 1.0f);

    if (eLootContext == LC_TREASUREBAG)
    {
        SGameStatsCounterIncrement(STAT_COUNTER_GAME_ITEM_DROPPED_FROM_TREASURE_BAG, tACDItem.hGB.gbid, ItemGetStatsDataKey(tACDItem), 1.0f);
    }
    else if (eLootContext == LC_RANDOMITEMVENDOR)
    {
        SGameStatsCounterIncrement(STAT_COUNTER_GAME_ITEM_DROPPED_FROM_RANDOM_ITEM_VENDOR, tACDItem.hGB.gbid, ItemGetStatsDataKey(tACDItem), 1.0f);
    }
}
















void SGameStatsRecordGoldSpent(ActorCommonData & tACDPlayer, uint64 uGoldSpent)
{
    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (uGoldSpent != 0 && ptPlayer != NULL)
    {
        ptPlayer->tStats.m_uGoldSpent += uGoldSpent;
    }
}






void SGameStatsRecordGoldObtainedThroughSelling(ActorCommonData & tACDPlayer, uint64 uGoldObtained)
{
    Player * ptPlayer = PlayerGetByACD(tACDPlayer.id);
    if (uGoldObtained != 0 && ptPlayer != NULL)
    {
        ptPlayer->tStats.m_uGoldObtainedThroughSelling += uGoldObtained;
    }
}





// No standalone body in the linked binary; this helper is checkable in GenerateLogForPlayer.
static CRefString sGetTimeString(
    uint32 uTicks)
{
    CRefString strTime;
    if (uTicks < TICKS_PER_MINUTE)
    {
        strTime.Format("%.2f Seconds", (float)uTicks * OO_TICKS_PER_SECOND);
    }
    else {
        // Inference: original minute conversion multiplies by the folded reciprocal of 3600.
        float flMinutes = (float)uTicks * (1.0f / TICKS_PER_MINUTE);
        strTime.Format("%.2f Minutes", flMinutes);
    }
    // Inference: the named return object survives both formatting branches.
    return strTime;
}































































void SGameStatsGenerateLogForPlayer(
    ACDID idACDPlayer,
    CRefString & strOut)
{
    const Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer == NULL)
    {
        return;
    }

    ActorCommonData & tACDPlayer = ACDGet(idACDPlayer);
    uint32 uNow = GameGetServerTime();


    strOut += CRefString().Format("Stats for Player HeroID %llu (%s): ",
        ptPlayer->uHeroId, MarkerNameGetString(tACDPlayer.tName));



    uint32 uCurrentSessionTicks = uNow - ptPlayer->tStats.m_uStartTickForPlaySession;
    strOut += CRefString().Format("Total Session Time=%s, ",
        sGetTimeString(uCurrentSessionTicks).str());











    // Original format strings use %d even though the currency totals are uint64.
    strOut += CRefString().Format("Total Gold Picked Up=%d, ", ptPlayer->tStats.m_uTotalCurrencyPickedUp[CURRENCY_GOLD]);
    strOut += CRefString().Format("Total Blood Shards Picked Up=%d, ", ptPlayer->tStats.m_uTotalCurrencyPickedUp[CURRENCY_BLOODSHARDS]);
    strOut += CRefString().Format("Total Platinum Picked Up=%d, ", ptPlayer->tStats.m_uTotalCurrencyPickedUp[CURRENCY_PLATINUM]);











    // Inference: the original converts the fetched float count to an unsigned 64-bit total.
    uint64 nMonstersKilled = SGameStatsPlayerCounterFetch(ptPlayer, STAT_COUNTER_PLAYER_MONSTERS_KILLED, 0, 0);
    strOut += CRefString().Format("Monsters Killed=%llu", nMonstersKilled);

    strOut += CRefString("\n");
}













































// Inference: array initializer order is the seven original calls at line 1072; DWARF DIE05aba999.
static const StatCounterType sctStatCounterPlayTimeByClass[7] =
{
    STAT_COUNTER_GAME_TIME_PLAYED_DEMON_HUNTER,
    STAT_COUNTER_GAME_TIME_PLAYED_BARBARIAN,
    STAT_COUNTER_GAME_TIME_PLAYED_WIZARD,
    STAT_COUNTER_GAME_TIME_PLAYED_WITCH_DOCTOR,
    STAT_COUNTER_GAME_TIME_PLAYED_MONK,
    STAT_COUNTER_GAME_TIME_PLAYED_CRUSADER,
    STAT_COUNTER_GAME_TIME_PLAYED_NECROMANCER
};


void SGameStatsRecordGameStats()

{


    SGame & tSGame = SGameManager::GameGet();

    SGameStatGlobals * ptSGameStatGlobals = tSGame.ptSGameStatGlobals;
    if (ptSGameStatGlobals == NULL)
    {
        return;
    }

    float flGameDurationInSeconds = (float)GameGetServerTime() / TICKS_PER_SECOND;

    SGameStatsCounterIncrement(STAT_COUNTER_GAME_TIME_PLAYED, 0, 0, flGameDurationInSeconds);
    SGameStatsCounterIncrement(STAT_COUNTER_GAME_MAX_PLAYERS, ptSGameStatGlobals->uMaxNumberOfPlayers, 0, 1.0f);
    // Inference: the original unrolled loop visits each of the seven class counters.
    for (int32 i = 0; i < 7; ++i)
    {
        SGameStatsCounterIncrement(sctStatCounterPlayTimeByClass[i], ptSGameStatGlobals->uTotalNumberOfPlayersByClass[i], 0, ptSGameStatGlobals->flTotalPlayTimeByClass[i]);
    }

    int32 nStatDataKey = SGameGetStatDataKey();

    SGameStatsCounterIncrement(STAT_COUNTER_GAME_TIME_PLAYED_IN_DIFFICULTY, nStatDataKey, 0, flGameDurationInSeconds);
    SGameStatsCounterIncrement(STAT_COUNTER_GAME_CREATED_IN_DIFFICULTY, nStatDataKey, 0, 1.0f);
    SGameStatsCounterIncrement(STAT_COUNTER_GAME_JOINS_IN_DIFFICULTY, nStatDataKey, 0, SGameStatsCounterFetch(STAT_COUNTER_PLAYER_JOINED_GAME, 0, 0));

    if (tSGame.tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_PUBLIC_BIT))
    {
        SGameStatsCounterIncrement(STAT_COUNTER_GAME_PUBLIC, 0, 0, 1.0f);
    }

    if (tSGame.ptSGameGlobals != NULL && strlen(tSGame.ptSGameGlobals->tMatchmakingData.szGameTag))
    {
        SGameStatsCounterIncrement(STAT_COUNTER_GAME_TAG, StringToGBID(tSGame.ptSGameGlobals->tMatchmakingData.szGameTag), 0, 1.0f);
    }

    ptSGameStatGlobals->tEndTime = Blizzard::Time::LocalBias(Blizzard::Time::GetTimestamp());













}




void SGameStatsRecordPlayerStats(const Player & tPlayer)
{

    SGame & tSGame = SGameManager::GameGet();

    SGameStatGlobals *ptSGameStatGlobals = tSGame.ptSGameStatGlobals;
    if (!ptSGameStatGlobals)
    {
        return;
    }

    uint32 dwNow = GameGetServerTime();

    POS pos = ptSGameStatGlobals->mapPlayerStatTimedEvents[tPlayer.nPlayerIndex].GetStartAssoc();
    while (pos != NULL)
    {
        StatCounterKey tKey;
        StatTimedEventData tData;
        ptSGameStatGlobals->mapPlayerStatTimedEvents[tPlayer.nPlayerIndex].GetNextAssoc(pos, tKey, tData);

        uint32 dwDuration = tData.dwRunningTotal;
        // Inferred from columns: && at 1132:39, != at 1132:56, - at 1135:34 (`+= (`), cast operand at 1138:44 and / at 1138:55
        if (tData.bReportIfUnfinished && tData.dwStart != 0)
        {

            dwDuration += (dwNow - tData.dwStart);
        }

        float flDurationInSeconds = (float)dwDuration / (float)TICKS_PER_SECOND;


        float flExistingGameValue = 0.0f;
        ptSGameStatGlobals->mapStatCounters.Lookup(tKey, flExistingGameValue);
        float flNewGameValue = flDurationInSeconds + flExistingGameValue;
        ptSGameStatGlobals->mapStatCounters.SetAt_Tracked(tKey, flNewGameValue);
    }







}
















void SGameStatsDumpPlayerToLog(ACDID idACDPlayer)
{
    CRefString strLog;
    SGameStatsGenerateLogForPlayer(idACDPlayer, strLog);
}





void SGameStatsPlayerStartedQuest(ACDID idACDPlayer, SNO snoQuest, QuestType eQuestType)
{
    const Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer != NULL && GlobalSNOGet(GLOBALSNO_QUEST_DUNGEONFINDER) == snoQuest)
    {
        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(ptPlayer);
        tEvaluationData.EvaluateEvent(ptPlayer, 415, snoQuest, 1, (uint32)sub_71007b5e70());
    }
}
























void SGameStatsPlayerCompletedQuest(ACDID idACDPlayer, SNO snoQuest, QuestType eQuestType, BOOL bGetAchievementCredit)
{
    Player * ptPlayer = PlayerGetByACD(idACDPlayer);
    if (ptPlayer == NULL)
    {
        return;
    }

    BOOL bIsDungeonFinder;
    BOOL bIsOrphanedGriftQuest = GlobalSNOGet(GLOBALSNO_QUEST_DUNGEONFINDER) == snoQuest && NephalemRiftManager::IsTieredRiftOrphaned(NULL);

    if (!bIsOrphanedGriftQuest)
    {
        SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_QUEST_COMPLETED, snoQuest, 0, 1.0f);
    }
    if (eQuestType == QUEST_TYPE_BOUNTY)
    {
        SGameStatsPlayerCounterIncrement(ptPlayer, STAT_COUNTER_PLAYER_BOUNTY_COMPLETED_ALL, 0, 0, 1.0f);
    }
    if (!bIsOrphanedGriftQuest)
    {
        ptPlayer->tConsoleCounters.uLootRunsCompleted++;
    }

    uint32 uPlayedSeconds = PlayerGetPlayedSeconds(*ptPlayer);

    if (eQuestType == QUEST_TYPE_BOUNTY || bGetAchievementCredit)
    {
        Achievement::EvaluationData tEvaluationData;
        tEvaluationData.SetPlayer(ptPlayer);
        Achievement::EventID eEventID;
        if (eQuestType == QUEST_TYPE_BOUNTY)
        {
            eEventID = 412;
        }
        else if (eQuestType == QUEST_TYPE_CHALLENGE)
        {
            eEventID = 414;
        }
        else
        {
            bIsDungeonFinder = GlobalSNOGet(GLOBALSNO_QUEST_DUNGEONFINDER) == snoQuest;
            eEventID = bIsDungeonFinder ? 413 : 406;
        }
        tEvaluationData.EvaluateEvent(ptPlayer, eEventID, snoQuest, 1, (uint32)sub_71007b5e70());
    }
}
























void SGameStatsRecordDamage(
    ACDID idACDMainActor,
    ACDID idACDTarget,
    float flDamageDone,
    SNO snoPower,
    PowerID idPower,
    BOOL bCrit,
    DamageType ePrimaryDamageType,
    PayloadDeliveryMechanism eDeliveryMechanism)
{
    if (flDamageDone < EPSILON)
    {
        return;
    }
    // Inference: unused game/stat snapshots follow DWARF locals and the neighboring recording functions.
    SGame & tSGame = SGameManager::GameGet();
    SGameStatGlobals * ptSGameStatGlobals = tSGame.ptSGameStatGlobals;


    ActorCommonData * ptACDMainActor = ACDTryToGet(idACDMainActor);

    Player *ptPlayerMain = ACDGetControllingPlayer(idACDMainActor);
    if (ptPlayerMain != NULL)
    {
        SNO snoMonsterActor = SNO_NONE;
        // Inference: unused monster SNO assignment explains the retained IsMonster call; DWARF records initial SNO_NONE.
        ActorCommonData * ptACDTarget = ACDTryToGet(idACDTarget);
        if (ptACDTarget &&
            ptACDTarget->IsMonster())
        {
            snoMonsterActor = ptACDTarget->snoActor;
        }

        Power *ptPower = PowerTryToGet(idPower);
        if (ptPower != NULL)
        {
            SGameStatsPlayerCounterIncrement(ptPlayerMain, STAT_COUNTER_PLAYER_POWER_HIT, snoPower, PowerGetRuneType(ptPower), 1.0f);
            SGameStatsPlayerCounterIncrement(ptPlayerMain, STAT_COUNTER_PLAYER_POWER_DAMAGE_DONE, snoPower, PowerGetRuneType(ptPower), flDamageDone);
            SGameStatsPlayerCounterIncrement(ptPlayerMain, STAT_COUNTER_PLAYER_POWER_DAMAGE_DONE_ALL_RUNES, snoPower, 0, flDamageDone);
            SGameStatsPlayerCounterIncrement(ptPlayerMain, STAT_COUNTER_PLAYER_DAMAGE_DONE_TOTAL, 0, 0, flDamageDone);

            if (bCrit)
            {
                SGameStatsPlayerCounterIncrement(ptPlayerMain, STAT_COUNTER_PLAYER_POWER_CRIT, snoPower, PowerGetRuneType(ptPower), 1.0f);
                SGameStatsPlayerCounterIncrement(ptPlayerMain, STAT_COUNTER_PLAYER_POWER_DAMAGE_CRIT, snoPower, PowerGetRuneType(ptPower), flDamageDone);
                SGameStatsPlayerCounterIncrement(ptPlayerMain, STAT_COUNTER_PLAYER_DAMAGE_CRIT_TOTAL, 0, 0, flDamageDone);
            }
            else
            {
                SGameStatsPlayerCounterIncrement(ptPlayerMain, STAT_COUNTER_PLAYER_POWER_DAMAGE_NON_CRIT, snoPower, PowerGetRuneType(ptPower), flDamageDone);
            }

            ActorCommonData *ptACDPlayerMain = ACDTryToGet(ptPlayerMain->idPlayerACD);

            if (ptACDPlayerMain && ptACDPlayerMain->IsStealthed())
            {
                SGameStatsPlayerCounterIncrement(ptPlayerMain, STAT_COUNTER_PLAYER_DAMAGE_DONE_WHILE_STEALTHED, 0, 0, flDamageDone);
            }
        }
    }

    if (ACDIsPlayer(idACDTarget))
    {
        SNO snoDamageSource = SNO_NONE;
        if (ptACDMainActor != NULL)
        {
            snoDamageSource = ptACDMainActor->snoActor;
        }

        Player *ptPlayerTarget = PlayerGetByACD(idACDTarget);
        SGameStatsPlayerCounterIncrement(ptPlayerTarget, STAT_COUNTER_PLAYER_DAMAGE_TAKEN, snoPower, snoDamageSource, flDamageDone);
        SGameStatsPlayerCounterIncrement(ptPlayerTarget, STAT_COUNTER_PLAYER_DAMAGE_TAKEN_TOTAL, 0, 0, flDamageDone);

        if (GameTypeIsPvP() && ptPlayerMain && ptPlayerTarget)
        {
            ptPlayerTarget->flPVPPlayerDamageTaken[ptPlayerMain->nPlayerIndex] += flDamageDone;
        }
    }
    // Inference: packed event parameter uses critical bit and damage type in the high half, as the original ORR.
    uint32 dwParam = bCrit;
    dwParam |= ePrimaryDamageType << 16;
    CombatLog::LogEvent(
        CombatLog::LOG_EVENT_DAMAGE_DONE,
        idACDMainActor,
        idACDTarget,
        snoPower,
        flDamageDone,
        (float) eDeliveryMechanism,
        dwParam);
}












static inline void sTimedEventStart(StatTimedEventMap * pStatTimedEventMap, StatCounterType eType, int32 nParam0, int32 nParam1, BOOL bReportIfUnfinished)
{
    StatCounterKey tKey(eType, nParam0, nParam1);
    StatTimedEventData tData;
    tData.dwRunningTotal = 0;
    pStatTimedEventMap->Lookup(tKey, tData);
    tData.dwStart = GameGetServerTime();
    tData.bReportIfUnfinished = bReportIfUnfinished;
    pStatTimedEventMap->SetAt_Tracked(tKey, tData);
}















static void sTimedEventFinish(StatTimedEventMap * pStatTimedEventMap, StatCounterType eType, int32 nParam0, int32 nParam1)
{
    StatCounterKey tKey(eType, nParam0, nParam1);
    StatTimedEventData tData;
    if (pStatTimedEventMap->Lookup(tKey, tData))
    {
        tData.dwRunningTotal += GameGetServerTime() - tData.dwStart;
        tData.dwStart = 0;
        pStatTimedEventMap->SetAt_Tracked(tKey, tData);
    }
}












static float sCounterFetch(StatCounterMapFloat * pStatCounterMap, StatCounterKey tKey);
static void sCounterIncrement(StatCounterMapFloat * pStatCounterMap, StatCounterType eType, int32 nParam0, int32 nParam1, float flValue)
{
    StatCounterKey tKey(eType, nParam0, nParam1);
    float flNewValue = sCounterFetch(pStatCounterMap, tKey) + flValue;
    pStatCounterMap->SetAt_Tracked(tKey, flNewValue);
}










static float sCounterFetch(
    StatCounterMapFloat * pStatCounterMap,
    StatCounterKey tKey)
{
    float flExistingValue = 0.0f;
    pStatCounterMap->Lookup(tKey, flExistingValue);
    // Inference: Lookup leaves the initial zero for missing keys, as the original does.
    return flExistingValue;
}


void SGameStatsTimedEventStart(StatCounterType eType, int32 nParam0, int32 nParam1, BOOL bReportIfUnfinished)
{
    SGame & tSGame = SGameManager::GameGet();
    SGameStatGlobals * ptSGameStatGlobals = tSGame.ptSGameStatGlobals;
    if (ptSGameStatGlobals == NULL)
    {
        return;
    }
    sTimedEventStart(&ptSGameStatGlobals->mapStatTimedEvents, eType, nParam0, nParam1, bReportIfUnfinished);
}











void SGameStatsTimedEventFinish(StatCounterType eType, int32 nParam0, int32 nParam1)
{
    SGame & tSGame = SGameManager::GameGet();
    SGameStatGlobals * ptSGameStatGlobals = tSGame.ptSGameStatGlobals;
    if (ptSGameStatGlobals == NULL)
    {
        return;
    }
    sTimedEventFinish(&ptSGameStatGlobals->mapStatTimedEvents, eType, nParam0, nParam1);
}










void SGameStatsCounterIncrement(StatCounterType eType, int32 nParam0, int32 nParam1, float flValue)
{
    SGame & tSGame = SGameManager::GameGet();
    SGameStatGlobals * ptSGameStatGlobals = tSGame.ptSGameStatGlobals;
    if (ptSGameStatGlobals == NULL)
    {
        return;
    }
    sCounterIncrement(&ptSGameStatGlobals->mapStatCounters, eType, nParam0, nParam1, flValue);
}











void SGameStatsCounterSet(StatCounterType eType, int32 nParam0, int32 nParam1, float flValue)
{
    SGame & tSGame = SGameManager::GameGet();
    SGameStatGlobals * ptSGameStatGlobals = tSGame.ptSGameStatGlobals;
    if (ptSGameStatGlobals == NULL)
    {
        return;
    }
    StatCounterKey tKey(eType, nParam0, nParam1);
    ptSGameStatGlobals->mapStatCounters.SetAt_Tracked(tKey, flValue);
}











// Original standalone body was linker-removed (DIE05af2399, low_pc0); not independently checkable.
float SGameStatsCounterFetch(
    StatCounterType eType,
    int32 nParam0,
    int32 nParam1)
{


    SGame & tSGame = SGameManager::GameGet();

    SGameStatGlobals * ptSGameStatGlobals = tSGame.ptSGameStatGlobals;
    if (ptSGameStatGlobals == NULL)
    {
        return 0.0f;
    }

    StatCounterKey tKey(eType, nParam0, nParam1);

    return sCounterFetch(&ptSGameStatGlobals->mapStatCounters, tKey);
}




void SGameStatsPlayerTimedEventStart(Player * ptPlayer, StatCounterType eType, int32 nParam0, int32 nParam1, BOOL bReportIfUnfinished)
{
    if (ptPlayer == NULL)
    {
        return;
    }
    SGame & tSGame = SGameManager::GameGet();
    SGameStatGlobals * ptSGameStatGlobals = tSGame.ptSGameStatGlobals;
    if (ptSGameStatGlobals == NULL)
    {
        return;
    }
    sTimedEventStart(&ptSGameStatGlobals->mapPlayerStatTimedEvents[ptPlayer->nPlayerIndex], eType, nParam0, nParam1, bReportIfUnfinished);
}














void SGameStatsPlayerTimedEventFinish(Player * ptPlayer, StatCounterType eType, int32 nParam0, int32 nParam1)
{
    if (ptPlayer == NULL)
    {
        return;
    }
    SGame & tSGame = SGameManager::GameGet();
    SGameStatGlobals * ptSGameStatGlobals = tSGame.ptSGameStatGlobals;
    if (ptSGameStatGlobals == NULL)
    {
        return;
    }
    sTimedEventFinish(&ptSGameStatGlobals->mapPlayerStatTimedEvents[ptPlayer->nPlayerIndex], eType, nParam0, nParam1);
}













void SGameStatsPlayerCounterIncrement(Player * ptPlayer, StatCounterType eType, int32 nParam0, int32 nParam1, float flValue)
{
    if (ptPlayer == NULL)
    {
        return;
    }
    SGame & tSGame = SGameManager::GameGet();
    SGameStatGlobals * ptSGameStatGlobals = tSGame.ptSGameStatGlobals;
    if (ptSGameStatGlobals == NULL)
    {
        return;
    }








    float flCurrentValue;
    StatCounterKey tKey(eType, nParam0, nParam1);
    if (ptSGameStatGlobals->mapPlayerStatCounters[ptPlayer->nPlayerIndex].Lookup(tKey, flCurrentValue))
    {
        uint32 nAlertLevel = sStatCounterTypeGetAlertLevel(eType, nParam0);
        if (nAlertLevel > 0)
        {
            float flNextValue = flCurrentValue + flValue;
            uint32 dwPlayerSessionDuration = GameGetServerTime() - ptPlayer->dwJoinTime;
            uint32 nHoursPlayed = dwPlayerSessionDuration / (TICKS_PER_MINUTE * 60) + 1;
            uint32 nNextValue = Math::FloatToInt(flNextValue / nHoursPlayed);
            uint32 nCurrentValue = Math::FloatToInt(flCurrentValue / nHoursPlayed);
            if (nNextValue > nAlertLevel)
            {
                uint32 nNextLarger2PowerFromCurrent = Math::NextLarger2Power(nCurrentValue);
                uint32 nNextLarger2PowerFromNext = Math::NextLarger2Power(nNextValue);
                if (nNextLarger2PowerFromCurrent != nNextLarger2PowerFromNext)
                {
                }
            }
        }
    }


    sCounterIncrement(&ptSGameStatGlobals->mapPlayerStatCounters[ptPlayer->nPlayerIndex], eType, nParam0, nParam1, flValue);


    sCounterIncrement(&ptSGameStatGlobals->mapStatCounters, eType, nParam0, nParam1, flValue);


    if (GameTypeIsPvP())
    {
        sCounterIncrement(&ptSGameStatGlobals->mapPlayerCurrentLifeStatCounters[ptPlayer->nPlayerIndex], eType, nParam0, nParam1, flValue);
    }
}









float SGameStatsPlayerCounterFetch(const Player * ptPlayer, StatCounterType eType, int32 nParam0, int32 nParam1)
{
    SGame & tSGame = SGameManager::GameGet();
    SGameStatGlobals * ptSGameStatGlobals = tSGame.ptSGameStatGlobals;
    if (ptSGameStatGlobals == NULL)
    {
        return 0.0f;
    }
    return sCounterFetch(&ptSGameStatGlobals->mapPlayerStatCounters[ptPlayer->nPlayerIndex], StatCounterKey(eType, nParam0, nParam1));
}













float SGameStatsPlayerCurrentLifeCounterFetch(const Player * ptPlayer, StatCounterType eType, int32 nParam0, int32 nParam1)
{
    SGame & tSGame = SGameManager::GameGet();
    SGameStatGlobals * ptSGameStatGlobals = tSGame.ptSGameStatGlobals;
    if (ptSGameStatGlobals == NULL)
    {
        return 0.0f;
    }
    return sCounterFetch(&ptSGameStatGlobals->mapPlayerCurrentLifeStatCounters[ptPlayer->nPlayerIndex], StatCounterKey(eType, nParam0, nParam1));
}













void SGameStatsClearForPlayer(Player & tPlayer)
{
    SGame & tSGame = SGameManager::GameGet();
    SGameStatGlobals * ptSGameStatGlobals = tSGame.ptSGameStatGlobals;
    if (ptSGameStatGlobals == NULL)
    {
        return;
    }



    ptSGameStatGlobals->mapPlayerStatCounters[tPlayer.nPlayerIndex].RemoveAll();
    ptSGameStatGlobals->mapPlayerStatTimedEvents[tPlayer.nPlayerIndex].RemoveAll();

    ptSGameStatGlobals->mapPlayerCurrentLifeStatCounters[tPlayer.nPlayerIndex].RemoveAll();
    ptSGameStatGlobals->mapPlayerCurrentLifeStatTimedEvents[tPlayer.nPlayerIndex].RemoveAll();
}



void SGameStatsClearForPlayerDeath(Player & tPlayer)
{
    SGame & tSGame = SGameManager::GameGet();
    SGameStatGlobals * ptSGameStatGlobals = tSGame.ptSGameStatGlobals;
    if (ptSGameStatGlobals == NULL)
    {
        return;
    }




    ptSGameStatGlobals->mapPlayerCurrentLifeStatCounters[tPlayer.nPlayerIndex].RemoveAll();
    ptSGameStatGlobals->mapPlayerCurrentLifeStatTimedEvents[tPlayer.nPlayerIndex].RemoveAll();
}



int32 SGameStatsEncodeSNODataToID(SNO sno, int32 nData) {
    return (sno << 8) | (nData & 0xFF);
}































BOOL SGameStatsUsedCheats()
{
    SGameStatGlobals * ptSGameStatGlobals = SGameManager::GameGet().ptSGameStatGlobals;
    if (ptSGameStatGlobals != NULL)
    {
        return ptSGameStatGlobals->bCheatUsed != FALSE;
    }
    return FALSE;
}
