// Original file: X/Server/SGame.cpp
// Partly written: functions sit on their original lines (see the reference .cpp).

#include "SGame.h"
#include "SGameConstants.h"
#include "SGameManager.h"
#include "SActTransitions.h"
#include "../Common/Game.h"
#include "../Common/GlobalSNO.h"
#include "../Common/Definitions/ActDefinition.h"
#include "../Common/Definitions/LevelAreaDefinition.h"
#include "../Sigma/RefString.h"
#include "../Common/Act.h"
#include "../Common/Player.h"
#include "../Common/GameCommonData.h"
#include "../Common/Networking/GameMessage.h"
#include "../Common/Networking/Network.h"
#include "../Common/OnlineService/OnlineServiceErrorCodes.h"
#include "../Common/Definitions/ActorDefinition.h"
#include "../Common/Definitions/MonsterDefinition.h"
#include "../Sigma/Math/MathGeneral.h"
#include "PlayerList.h"
#include "SPlayer.h"
#include "SWorld.h"
#include "../Common/Protobuf/Account.pb.h"
#include "../Common/Protobuf/Hero.pb.h"
#include "../Common/Protobuf/Items.pb.h"
#include "Map/Minimap.h"
#include "Shell/SConsoleCommand.h"
#include "../Common/Definitions/QuestRangeDefinition.h"
#include "LuaScript/ScriptUtil.h"
#include "../Common/GameSwig.h"
#include "../Common/ActorCommonData.h"
#include "../Sigma/DataStructures/XVar.h"
#include "../Sigma/DataStructures/DataStructuresExternal.h"
#include "../Common/OnlineService/Achievements.h"
// Declared here from their definitions elsewhere (headers not adopted yet).
struct CachedWaypoint {  // size 0x2c (X/Common/Act.h:13)
    Act eSourceAct;  // 0x00
    WaypointInfo tInfo;  // 0x04
};
typedef blz::vector<CachedWaypoint, blz::allocator<CachedWaypoint> > CachedWaypointArray;  // X/Common/Act.h:19
const CachedWaypointArray & ActGetCachedWaypointArray();  // X/Common/Act.cpp:146
int32 CountSetBits(const uint8 * lpb, const int32 nLen);  // X/Sigma/BitManip.cpp:87
struct GameSyncedDataMessage {  // size 0xd8 (X/Common/GameCommonData.h:120)
    RequiredMessageHeader tHeader;  // 0x00
    GameSyncedData tSyncedData;  // 0x08
};
#include "../Sigma/DataStructures/XGrowableList.h"
#include "../Common/Shell/CmdLine.h"
struct UpdateCounters {  // size 0x8
    int32 nCurCallCount;  // 0x00
    int32 nAveCallCount;  // 0x04
};

struct UpdateFunctionItem {  // size 0x8
    UpdateFunctionType nUpdateFunctionType;  // 0x00
    DataID idUpdate;  // 0x04
};

typedef XGrowableList<UpdateFunctionItem, const UpdateFunctionItem &> TUpdateFunctionList;

struct SGamePrivateGlobals {  // size 0x2ca0
    TUpdateFunctionList tUpdateFunctionQueue[TICK_UPDATE_BUCKETS];  // 0x00
    UpdateCounters tUpdateCounters[NUM_UPDATE_FUNCTION_TYPE];  // 0x2c00
    BOOL fUpdateTimeExhausted;  // 0x2c98
};
static_assert(sizeof(SGamePrivateGlobals) == 0x2ca0, "SGamePrivateGlobals");

enum { MF_TREASURE_GOBLIN = 11 };  // X/Common/ActorCommonDataSwig.h:41
class XVarInt32;
extern XVarInt32 s_varAnniversaryEventStatus;  // X/Common/OnlineService/OnlineService.cpp:183
Act ActGetWaypointActByLevelArea(const SNO snoLevelArea);  // X/Common/Act.cpp:214
SNO PlayerGetLastWaypointLevelArea(const Player * ptPlayer);  // X/Common/Player.cpp:1876
Act SPlayerGetMaxAct(const Player & tPlayer);  // X/Server/SPlayer.cpp:10834


BOOL ReplayIsPlayingMultiServerGame();  // X/Server/Replay.cpp:821
BOOL ReplayIsPlayingServerCPUTest();  // X/Server/Replay.cpp:838
void ReplayTestUpdate(uint32 dwGameUpdateCount);  // X/Server/Replay.cpp:1646
void XServerReadUpdate();  // X/Server/Network/XServer.cpp:1157
void SNetLoungeUpdate();  // X/Server/Network/SNetLounge.cpp:291
void ServerMonUpdate();  // X/Server/Network/ServerMon.cpp:444
void PowersForceStopForWorld(SWorldID idSWorld);  // X/Server/Powers/Powers.cpp:937
void LevelAreasDeleteFlagged();  // X/Server/LevelArea.cpp:465
void LevelAreaWorldFlagForDeletion(SWorldID idSWorld, uint32 dwDeleteTime);  // X/Server/LevelArea.cpp:654

void SConductor_ChangeActs();  // X/Server/SConductor.cpp:1178
SWorldID WorldCommonFindBySNO(SNO snoWorld, BOOL bIgnorePendingDelete);  // X/Common/World.cpp:605
void ServerUpdateClients(BOOL bUpdateKnownLists);  // X/Server/Server.cpp:1738

#include "Achievements/AchievementModifierEvaluationLogic.h"
namespace Achievement {
typedef int32 EventID;










}  // namespace Achievement
enum { ACH_EVENT_COMPLETE_ACT = 405 };  // X/Common/Achievements/AchievementDefines.h:85

namespace NephalemRiftManager {
int32 RiftPlayerCount();  // X/Server/System/SNephalemRift.cpp:1640
}
#include "../Client/CGame.h"










BOOL SPlayerIsLoggingOut(BOOL includeGracePeriod, Player & tPlayer);  // X/Server/SPlayer.cpp:7750
BOOL SPlayerInScriptedSequence(const Player & tPlayer);  // X/Server/SPlayer.cpp:10849
void PlayerBrainClearQueuedPower(ACDID idACDPlayer);  // X/Server/AI/PlayerBrain.cpp:462
namespace Console { namespace Online {
BOOL PartyIsInParty();  // X/Client/Console/Online.cpp:3967
void PartySetGameIsPaused(BOOL bIsPaused);  // X/Client/Console/Online.cpp:4010
void PartySetGameIsSoftPaused(BOOL bIsPaused);  // X/Client/Console/Online.cpp:4029
} }
namespace NephalemRift { BOOL IsTieredRiftActive(); }  // X/Common/NephalemRiftCommon.h
void SGamePause(BOOL bPaused);  // SGame.cpp:7972
void SGameSoftPause(BOOL bPaused);  // SGame.cpp:7932













void SPlayerConsoleSync(Player & tPlayer);  // X/Server/SPlayer.cpp
void SPlayerUnlockAdventureMode(Player & tPlayer);  // X/Server/SPlayer.cpp:2951
void QuestSetGameOver();  // X/Common/Quests/Quests.cpp:2681
void SNOFilesSetCurrentAct(Act eAct);  // X/Common/SNO/SNOFiles.cpp:8497
void ActCacheWaypoints(const Act eAct);  // X/Common/Act.cpp:83
void SGameFinishGameForConsole(QuitReason eReason);  // SGame.cpp:7627
void QuestsActivateForAct(Act eAct);  // X/Common/Quests/Quests.h:174
void ScriptEventOnChangedAct(SNO snoAct);  // X/Server/LuaScript/ScriptEvent.h:88
namespace Console { namespace Online { void PartyServerEndingGame(); } }  // X/Client/Console/Online.cpp:3937





void SWorldResetDungeonFinder();  // X/Server/SWorld.cpp:3153
BOOL SWorldIsFlaggedForDeletion(SWorldID idSWorld);  // X/Server/SWorld.cpp:346
void SWorldRemovePlayersAndFlagForDeletion(SWorldID idSWorld);  // X/Server/SWorld.cpp:4047
void ServerQueueClientUpdate();  // X/Server/Network/Server.cpp:438
void SPlayerClearAllReturnPoints(Player & tPlayer, SNO snoWorld);  // X/Server/SPlayer.cpp:10981
BOOL SGameSetOpenWorldMode(Player * ptPlayerInitiating, OpenWorldMode eOpenWorldMode, Act eOpenWorldModeAct, int32 nOpenWorldModeParam, SNO snoHubWorld, uint32 hubEntranceGUID);  // SGame.cpp:6514
void SGameAdvanceOpenWorldMode();  // SGame.cpp:6675
void SGameGetOpenWorldHubWorldForPlayer(const Player & tPlayer, ResolvedPortalDestination & tDest);  // SGame.cpp:6756
void SGameACDChangeWorld(const ACDID idACD, const SNO snoWorld, const uint32 uEntranceGUID, PlayerWarpReason eReason, const float flWarpFadeInSecs, const Player * ptReferencePlayer);  // SGame.cpp:7358
void ScriptEventOnUserEvent(LPCSTR szUserEvent, ACDID idACD, int32 nParam);  // X/Server/LuaScript/ScriptEvent.h
namespace BossEncounterManager { BOOL IsAnyEncounterActive(); }  // X/Server/System/BossEncounter.cpp




int32 PlayerGetNumPlayersForDifficulty();  // X/Common/Player.cpp:2159
int32 ServerGetPlayerCountForGameConnectionID(GameConnectionID idGameConnection);  // X/Server/Network/Server.cpp:2149
int32 SPlayerCreate(GameConnectionID idGameConnection, int32 nConsolePartyIndex);  // X/Server/SPlayer.cpp:10563
Player * PlayerGetByPlayerIndex(int32 nPlayerIndex);  // X/Common/Player.cpp:1956
uint32 SNOGetCoreDataHash();  // X/Common/SNO/SNOFiles.cpp:9830
void ActorsAdjustForPlayerChanged(const int32 nOldPlayerCount, const int32 nNewPlayerCount);  // X/Server/Actors/Actors.cpp:6724
void ActorsAdjustForDifficultyChanged(const int32 nOldDifficulty, const int32 nNewDifficulty);  // X/Server/Actors/Actors.cpp:6829
namespace SHero { void SetHandicapValues(ActorCommonData & tACD); }  // X/Server/SHero.cpp:2615
#include "../Common/Subsystem.h"  // per-game subsystem entry points follow (defined in their own files)
void ACDDisposeForGame(); void ACDInitializeForGame(); void AIStateDisposeForGame(); void AIStateInitializeForGame(); void ActorInventoryDisposeForGame(); void ActorInventoryInitializeForGame(); void ActorsDisposeForGame(); void ActorsInitializeForGame(); void ActsDisposeForGame(); void ActsInitializeForGame(); void AdventureDisposeForGame(); void AdventureInitializeForGame();
void ClientValidationDisposeForGame(); void ClientValidationInitializeForGame(); void DefinitionStatDisposeForGame(); void DefinitionStatInitializeForGame(); void FastAttribDisposeForGame(); void FastAttribInitializeForGame(); void FlippiesDisposeForGame(); void FlippiesInitializeForGame(); void GameBalanceDisposeForGame(); void GameBalanceInitializeForGame(); void GameDebugDisposeForGame(); void GameDebugInitializeForGame();
void InactiveMarkerDisposeForGame(); void InactiveMarkerInitializeForGame(); void ItemsDisposeForGame(); void ItemsInitializeForGame(); void KnownListDisposeForGame(); void KnownListInitializeForGame(); void LevelAreasDisposeForGame(); void LevelAreasInitializeForGame(); void LootDisposeForGame(); void LootInitializeForGame(); void LuaScriptDisposeForGame(); void LuaScriptInitializeForGame();
void MarkersDisposeForGame(); void MarkersInitializeForGame(); void NavCellPathDisposeForGame(); void NavCellPathInitializeForGame(); void NodePathDisposeForGame(); void NodePathInitializeForGame(); void PlayerListDisposeForGame(); void PlayerListInitializeForGame(); void PortalsDisposeForGame(); void PortalsInitializeForGame(); void PowersDisposeForGame(); void PowersInitializeForGame();
void QuestsDisposeForGame(); void QuestsInitializeForGame(); void SActDisposeForGame(); void SActInitializeForGame(); void SActTransitionsDisposeForGame(); void SActTransitionsInitializeForGame(); void SActorMovementDisposeForGame(); void SActorMovementInitializeForGame(); void SComplexEffectDisposeForGame(); void SComplexEffectInitializeForGame(); void SConductorDisposeForGame(); void SConductorInitializeForGame();
void SGameRefStringDisposeForGame(); void SGameRefStringInitializeForGame(); void SGameStatsDisposeForGame(); void SGameStatsInitializeForGame(); void SMemoryPoolsDisposeForGame(); void SMemoryPoolsInitializeForGame(); void SPlayersDisposeForGame(); void SPlayersInitializeForGame(); void SScenesDisposeForGame(); void SScenesInitializeForGame(); void STalkDisposeForGame(); void STalkInitializeForGame();
void STeamsInitializeForGame(); void STimedEventsDisposeForGame(); void STimedEventsInitializeForGame(); void STrickleDisposeForGame(); void STrickleInitializeForGame(); void SWorldsDisposeForGame(); void SWorldsInitializeForGame(); void ScriptEventDisposeForGame(); void ScriptEventInitializeForGame(); void ServerDisposeForGame(); void ServerInitializeForGame(); void TagMapDisposeForGame();
void TagMapInitializeForGame(); void TeamsDisposeForGame(); void TimersDisposeForGame(); void TimersInitializeForGame(); void VisualInventoryDisposeForGame(); void VisualInventoryInitializeForGame();
namespace Minimap { void CreateMarkerListForGame(); void DisposeMarkerListForGame(); } namespace UIGame { void InitializeFor_Server(); void DisposeFor_Server(); } namespace ConversationMgr { void InitializeForGame(); void DisposeForGame(); }
namespace BossEncounterManager { void EncounterStateCreateForGame(); void EncounterStateDestroyForGame(); } namespace NephalemRiftManager { void NephalemRiftStateCreateForGame(); void NephalemRiftStateDestroyForGame(); }
namespace SetEncounterManager { void EncounterStateCreateForGame(); void EncounterStateDestroyForGame(); } namespace VoteKick { namespace Server { void StateCreateForGame(); void StateDestroyForGame(); } }
void SHirelingSendStableInfoToClient(Player & tPlayer);  // X/Server/SHireling.cpp:431
void SPlayerGiveSharedMapVision(Player & tPlayerDest);  // X/Server/SPlayer.cpp:4328
void PVPOnPlayerEnteredGame(Player * ptPlayer);  // X/Server/PVP.cpp:354
void ActorBoostToLevel(ActorCommonData & tACDPlayer, const int32 nLevel, LPCSTR szTemplate, LPCSTR szTemplateParam, const uint32 dwPlayerBoostOptionBits);  // X/Server/ActorScript.cpp:148
enum {  // X/Server/ActorScript.h:12
    PLAYER_BOOST_PICKUP_ITEMS_BIT = 0,
    PLAYER_BOOST_NOTIFY_CLIENT_BIT = 1,
    PLAYER_BOOST_GIVE_WAYPOINTS_BIT = 2,
    PLAYER_BOOST_REMOVE_EQUIPMENT_FIRST_BIT = 3,
    PLAYER_BOOST_AUTO_ALLOC_ATTRIBUTES = 4,
    PLAYER_BOOST_SHOW_LEVEL_UP_UI_BIT = 5,
};

void ScriptEventOnPlayerEnteredGame(ACDID idACDPlayer);  // X/Server/LuaScript/ScriptEvent.h
struct PowerAPIDamage;
void BrainAPIDoDeath(ActorID idDeadActor, ActorID idKillerActor, const BOOL fNoExp, const BOOL fNoLoot, const BOOL fNoHealthDrop, PowerAPIDamage * ptDamage, float * pflDamageDone);  // X/Server/AI/BrainComponents.h:63












static SubsystemSetup sgtSGameMemorySubsystems[] =
{
    { "SMemoryPools", SMemoryPoolsInitializeForGame, SMemoryPoolsDisposeForGame },
    { "SGameRefString", SGameRefStringInitializeForGame, SGameRefStringDisposeForGame },
};
static int32 sgnNumSGameMemorySubsystems = sizeof(sgtSGameMemorySubsystems) / sizeof(sgtSGameMemorySubsystems[0]);


static SubsystemSetup sgtSGameSubsystems[] =
{
    { "GameDebug", GameDebugInitializeForGame, GameDebugDisposeForGame },
    { "TagMap", TagMapInitializeForGame, TagMapDisposeForGame },
    { "Timers", TimersInitializeForGame, TimersDisposeForGame },
    { "PlayerList", PlayerListInitializeForGame, PlayerListDisposeForGame },
    { "KnownList", KnownListInitializeForGame, KnownListDisposeForGame },
    { "DefinitionStat", DefinitionStatInitializeForGame, DefinitionStatDisposeForGame },
    { "Minimap", Minimap::CreateMarkerListForGame, Minimap::DisposeMarkerListForGame },
    { "ActorInventory", ActorInventoryInitializeForGame, ActorInventoryDisposeForGame },
    { "VisualInventory", VisualInventoryInitializeForGame, VisualInventoryDisposeForGame },
    { "NavCellPath", NavCellPathInitializeForGame, NavCellPathDisposeForGame },
    { "SActorMovement", SActorMovementInitializeForGame, SActorMovementDisposeForGame },
    { "Path", NodePathInitializeForGame, NodePathDisposeForGame },
    { "Acts", ActsInitializeForGame, ActsDisposeForGame },
    { "Markers", MarkersInitializeForGame, MarkersDisposeForGame },
    { "InactiveMarkers", InactiveMarkerInitializeForGame, InactiveMarkerDisposeForGame },
    { "SGameStats", SGameStatsInitializeForGame, SGameStatsDisposeForGame },
    { "UIGame", UIGame::InitializeFor_Server, UIGame::DisposeFor_Server },
    { "GameBalance", GameBalanceInitializeForGame, GameBalanceDisposeForGame },
    { "FastAttrib", FastAttribInitializeForGame, FastAttribDisposeForGame },
    { "SComplexEffect", SComplexEffectInitializeForGame, SComplexEffectDisposeForGame },
    { "LuaScript", LuaScriptInitializeForGame, LuaScriptDisposeForGame },
    { "ScriptEvents", ScriptEventInitializeForGame, ScriptEventDisposeForGame },
    { "ACD", ACDInitializeForGame, ACDDisposeForGame },
    { "STeams", STeamsInitializeForGame, TeamsDisposeForGame },
    { "Portals", PortalsInitializeForGame, PortalsDisposeForGame },
    { "Actors", ActorsInitializeForGame, ActorsDisposeForGame },
    { "Powers", PowersInitializeForGame, PowersDisposeForGame },
    { "AIStates", AIStateInitializeForGame, AIStateDisposeForGame },
    { "Quests", QuestsInitializeForGame, QuestsDisposeForGame },
    { "Conductor", SConductorInitializeForGame, SConductorDisposeForGame },
    { "Server", ServerInitializeForGame, ServerDisposeForGame },
    { "Items", ItemsInitializeForGame, ItemsDisposeForGame },
    { "Loot", LootInitializeForGame, LootDisposeForGame },
    { "SScenes", SScenesInitializeForGame, SScenesDisposeForGame },
    { "SWorlds", SWorldsInitializeForGame, SWorldsDisposeForGame },
    { "STrickle", STrickleInitializeForGame, STrickleDisposeForGame },
    { "SAct", SActInitializeForGame, SActDisposeForGame },
    { "STalk", STalkInitializeForGame, STalkDisposeForGame },
    { "LevelAreas", LevelAreasInitializeForGame, LevelAreasDisposeForGame },
    { "Flippies", FlippiesInitializeForGame, FlippiesDisposeForGame },
    { "BossEncounter", BossEncounterManager::EncounterStateCreateForGame, BossEncounterManager::EncounterStateDestroyForGame },
    { "NephalemRift", NephalemRiftManager::NephalemRiftStateCreateForGame, NephalemRiftManager::NephalemRiftStateDestroyForGame },
    { "SetDungeon", SetEncounterManager::EncounterStateCreateForGame, SetEncounterManager::EncounterStateDestroyForGame },
    { "VoteKick", VoteKick::Server::StateCreateForGame, VoteKick::Server::StateDestroyForGame },
    { "SPlayers", SPlayersInitializeForGame, SPlayersDisposeForGame },
    { "Adventures", AdventureInitializeForGame, AdventureDisposeForGame },
    { "STimedEvents", STimedEventsInitializeForGame, STimedEventsDisposeForGame },
    { "SActTransitions", SActTransitionsInitializeForGame, SActTransitionsDisposeForGame },
    { "ConversationMgr", ConversationMgr::InitializeForGame, ConversationMgr::DisposeForGame },
    { "ClientValidation", ClientValidationInitializeForGame, ClientValidationDisposeForGame },
};
#include "../Sigma/Networking/XNetAddress.h"
#include "../Common/ChallengeRift.h"
#include "../Common/Items.h"
#include "../Common/Definitions/SkillKitDefinition.h"
#include "../Common/Powers/PowersCommon.h"
#include "SGameStats.h"
#include "SHero.h"
#include "Crafting/SItemCrafting.h"
#include "SCosmeticItems.h"
#include "LuaScript/ScriptEvent.h"
#include "OnlineService/SStore.h"  // the CU defines OnlineService::StoreHelpers::STATUS_METADATA_TAG (SStore.h:116) and initialises it before the XVars
#include "../../NintendoSDK/Include/nn/ldn/ldn_Types.h"  // the CU defines the ldn_Ipv4Address.h:153-163 / ldn_MacAddress.h:101-106 statics (static initializer order)




static int32 sgnNumSGameSubsystems = sizeof(sgtSGameSubsystems) / sizeof(sgtSGameSubsystems[0]);

static float DPS_TRACKING_WINDOW_DEFAULT_SECONDS = 5.0f;  // non-const _ZL in the DWARF; 5.0f inferred from the folded SetWindowSeconds argument
const int32 ELAPSE_GAME_TIME_FOR_CLEANUP_MIN = 3600;




const int32 SGAME_CONNECT_TIMEOUT = 120;

const int32 GAME_SIM_THRESHOLD = 50;
const int32 KNOWN_LIST_UPDATE_INTERVAL = 20;
const int32 MATCHMAKING_FORCE_OPEN_DURATION = 10800;
const int32 MATCHMAKING_CLOSE_AFTER_PLAYER_EXIT_DURATION = 3600;


const int32 COMMUNITY_BUFFS_UPDATE_RATE = 60;
const int32 DEFAULT_TIME_BETWEEN_VERBOSE_SLOW_FRAME_LOGS = 3600;
const int32 DEFAULT_NUM_SLOW_POWERS_PER_LOG = 3;

static XVarScope s_scopeSGame("SGame", NULL);
XVarBool s_varAchievementsEnabled(s_scopeSGame, "AchievementsEnabled", true);
XVarUint32 s_varPlayerConnectTimeout(s_scopeSGame, "PlayerConnectTimeout", SGAME_CONNECT_TIMEOUT);
XVarUint32 s_varKnownListUpdateInterval(s_scopeSGame, "KnownListUpdateInterval", KNOWN_LIST_UPDATE_INTERVAL);
XVarInt32 s_varElapseGameTimeMin(s_scopeSGame, "ElapseGameTimeMin", ELAPSE_GAME_TIME_FOR_CLEANUP_MIN);
XVarUint32 s_varGameSimThreshold(s_scopeSGame, "GameSimThreshold", GAME_SIM_THRESHOLD);
XVarUint32 s_varMatchmakingPlayerExitFunctionalityEnabled(s_scopeSGame, "MatchmakingPlayerExitFunctionalityEnabled", 0);
XVarUint32 s_varMatchmakingForceOpenDuration(s_scopeSGame, "MatchmakingForceOpenDuration", MATCHMAKING_FORCE_OPEN_DURATION);
XVarUint32 s_varMatchmakingCloseAfterPlayerExitDuration(s_scopeSGame, "MatchmakingCloseAfterPlayerExitDuration", MATCHMAKING_CLOSE_AFTER_PLAYER_EXIT_DURATION);

XVarBool s_varRogerCheatDetectionEnabled(s_scopeSGame, "RogerCheatDetectionEnabled", false);

XVarBool s_varVerboseSlowFrameLogging(s_scopeSGame, "VerboseSlowFrameLogging", true);
XVarUint32 s_varVerboseSlowFrameTimeBetweenLogs(s_scopeSGame, "VerboseSlowFrameTimeBetweenLogs", DEFAULT_TIME_BETWEEN_VERBOSE_SLOW_FRAME_LOGS);
XVarBool s_varVerboseSlowFrameLoggingWantPlayerInfoTextLog(s_scopeSGame, "VerboseSlowFrameLoggingWantPlayerInfoTextLog", false);
XVarBool s_varVerboseSlowFrameLoggingWantPowerInfoTextLog(s_scopeSGame, "VerboseSlowFrameLoggingWantPowerInfoTextLog", false);
XVarBool s_varVerboseSlowFrameLoggingWantPlayerInfoBI(s_scopeSGame, "VerboseSlowFrameLoggingWantPlayerInfoBI", false);
XVarBool s_varVerboseSlowFrameLoggingWantPowerInfoBI(s_scopeSGame, "VerboseSlowFrameLoggingWantPowerInfoBI", false);

XVarUint32 s_varVerboseSlowFrameLoggingNumPowersPerLog(s_scopeSGame, "VerboseSlowFrameLoggingNumPowersPerLog", DEFAULT_NUM_SLOW_POWERS_PER_LOG);
XVarBool s_varVerboseSlowFrameLoggingWantTextLog(s_scopeSGame, "VerboseSlowFrameLoggingWantTextLog", false);

XVarBool s_varGameDisableAreaDamage(s_scopeSGame, "GameDisableAreaDamage", false);
XVarBool s_varGameDisableDOTAreaDamage(s_scopeSGame, "GameDisableDOTAreaDamage", false);
XVarBool s_varGameUseFixedAttributeTickUpdate(s_scopeSGame, "GameUseFixedAttributeTickUpdate", false);
// s_nMax/MinTotalUpdateDuration: linker-removed (DW_OP_addr 0), initial values unknown
int32 s_nMaxTotalUpdateDuration;
int32 s_nMinTotalUpdateDuration;

char s_szBroadcast[512];
BOOL PowerActorMeetsSkillRuneRequirement(const ActorCommonData & tACD, SNO snoPower, const RuneType eRuneType);  // DWARF records only X/Common/Powers/PowersCommon.cpp:3908; no header declaration found.
Blizzard::Time::Timestamp LeaderboardGetRiftScoreTimeToComplete(uint64 score);  // DWARF records only X/Common/Leaderboard.cpp:118; no header declaration found.
void PlayerSendHeroStateAllLoadoutsToClient(Player & tPlayer);  // DWARF records only X/Server/SPlayer.cpp:2663; no header declaration found.
void SGameSendDungeonFinderProgressToPlayer(Player & tPlayer);  // DWARF records only X/Server/SGame.cpp:8491; no header declaration found.
void PlayerAskClientToLoadLegendaryPowerIcons(const Player & tPlayer);  // DWARF records only X/Server/SPlayer.cpp:2875; no header declaration found.
void SGameSendDungeonFinderTimedEventToPlayer(Player & tPlayer);  // DWARF records only X/Server/SGame.cpp:8506; no header declaration found.
void PlayerListGetAllConnectedExceptConnection(PlayerList & listPlayers, const int32 idGameConnection, BOOL bIgnoreHardcoreDeadPlayers);  // DWARF records only X/Server/PlayerList.cpp:288; no header declaration found.
void PlayerSendTutorialStashIconsStateToClient(const Player * ptPlayer, const PlayerList & listDestPlayers);  // DWARF records only X/Server/SPlayer.cpp:2454; no header declaration found.
void PlayerSendHeroStateExtractedLegendariesToClient(const Player & tPlayer);  // DWARF records only X/Server/SPlayer.cpp:2861; no header declaration found.
void SGameSendDungeonFinderParticipatingPlayerCountToPlayer(Player & tPlayer);  // DWARF records only X/Server/SGame.cpp:8536; no header declaration found.
void TraceStat(LPCSTR sEventType, ACDID idACD, LPCSTR cpFmtStr, ...);  // DWARF records only X/Server/SGame.cpp:7255; no header declaration found.
void SGameSetCreatorInfo(OnlineService::HeroId uHeroId, const CHAR * uszHeroName, const CHAR * uszAccountName);  // DWARF CPP-only home: SGame.cpp:6385.

#include "../Common/Achievements/AchievementDefines.h"
namespace Console { namespace Online { int32 PartyGetHostIndex(); } }  // DWARF CPP-only home Online.cpp:4323; no header declaration found.
#include "../Common/Crafting/ItemCraftingSwig.h"
namespace Console { namespace Online { BOOL PartyIsHost(); } }  // DWARF CPP-only home Online.cpp:3857; no header declaration found; avoid unrelated static objects.
CRefString PlayerGetHeroName(const Player & tPlayer);  // DWARF CPP-only home Player.cpp:654; no header declaration found.
namespace SHero { void Load(Player & tPlayer, const D3::Hero::SavedDefinition & tHeroDefinition, const D3::Account::SavedDefinition & tAccountDefinition); }  // DWARF CPP-only home SHero.cpp:1635; no header declaration found.
namespace Minimap { void OnNewPlayer(const Player & tPlayer); }  // DWARF CPP-only home Minimap.cpp:1109; no header declaration found.
#include "../Common/TimeConstants.h"
Act ActEnumFromActIndex(const int32 nIndex);  // X/Common/GameCommonData.cpp:380
int32 ActIndexFromActEnum(const Act eAct);  // X/Common/GameCommonData.cpp:395
void SGameChangeActs(const Act eAct, SNO snoForceToWorld, BOOL bMovePlayers);  // SGame.cpp:6816
void SGameEvaluateActCompletedAchievement(Act eActCompleted);  // SGame.cpp:7122
void SGameFinishGameEnd();  // SGame.cpp:7670
BOOL SNOPackChunkLoadAct(Act eAct);  // X/Common/SNO/SNOFiles.cpp:11554
void ActGetCurrentStartLocation(ResolvedPortalDestination & tDest);  // X/Common/Act.cpp:260
void LevelAreaFlagAllForDeletion(uint32 dwDeleteTime);  // X/Server/System/LevelArea.cpp:92
void InactiveMarkerInitializeForAct();  // X/Common/InactiveMarker.cpp:389
SWorldID SActTravelTo(const Act eAct, Player & tPlayer, ResolvedPortalDestination & tDest, SWorldID idSWorldToUse);  // X/Server/SAct.cpp:62
BOOL BrainAPIResurrectToCheckpoint(ACDID idACD);  // X/Server/AI/BrainComponents.cpp:3134
void SGameSetCurrentAct(const Act eAct);  // SGame.cpp:6439
void SQuestsGetCurrent(SNO & snoQuest, int32 & nStepUID);  // DWARF records only X/Server/Quests/SQuests.cpp:3469; no header declaration found.
void SPlayerRestoreBuffs(Player & tPlayer);  // DWARF records only X/Server/SPlayer.cpp:12207; no header declaration found.
CRefString PlayerGetAccountName(const Player & tPlayer);  // DWARF records only X/Common/Player.cpp:686; no header declaration found.
void SPlayerSetHasFriends(const Player & tPlayer, BOOL bHasFriends);  // DWARF records only X/Server/SPlayer.cpp:10695; no header declaration found.
Team STeamGetTeamForPlayer(Player & tPlayer);  // DWARF records only X/Common/TeamCommon.cpp:479; no header declaration found.
void STrickleTriggerUpdate();  // DWARF records only X/Server/Network/STrickle.cpp:129; no header declaration found.
BOOL HeroSkillsAreOutOfDate(const GBID gbidHero, const D3::Hero::SavedData & tSavedData);  // DWARF records only X/Common/Definitions/HeroDefinition.cpp:692; no header declaration found.
void QuestsVersionForPlayer(HeroStateData & tStateData);  // DWARF records only X/Common/Quests/Quests.cpp:1528; no header declaration found.
BOOL ServerGetPlayerAddress(const Player & tPlayer, XNetAddress & tPlayerAddress);  // DWARF records only X/Server/Network/Server.cpp:2107; no header declaration found.
void SQuestsPlayerJoinedGame(Player &);  // DWARF records only X/Server/Quests/SQuests.cpp:131; no header declaration found.
void SPlayerRemoveTraitInSlot(Player & tPlayer, const int32 nSlot);  // DWARF records only X/Server/SPlayer.cpp:6291; no header declaration found.
void SPlayerSwitchActiveSkills(Player & tPlayer, const SNO snoPower, const RuneType eRuneType, const int32 nSlot, const BOOL bApplyCooldown);  // DWARF records only X/Server/SPlayer.cpp:6192; no header declaration found.
void SPlayerSendNewPlayerMessage(const PlayerList & listDestPlayers, const Player & tPlayer, BOOL bJustJoined);  // DWARF records only X/Server/SPlayer.cpp:2888; no header declaration found.
void ServerConnectionEnteredGame(GameConnectionID idGameConnection);  // DWARF records only X/Server/Network/Server.cpp:855; no header declaration found.
void SPlayerSetConsoleLicenseFlags(const Player & tPlayer, uint32 uConsoleLicenses);  // DWARF records only X/Server/SPlayer.cpp:10662; no header declaration found.
void ActorForceCleanupItemsOnGround(int32 nPlayerIndex);  // DWARF records only X/Server/Actors/Actors.cpp:796; no header declaration found.
void SPlayerEvaluateMultiplayerBuff();  // DWARF records only X/Server/SPlayer.cpp:12081; no header declaration found.
void SPlayerRegisterForExperienceId(const Player & tNewPlayer);  // DWARF records only X/Server/SPlayer.cpp:7458; no header declaration found.
void SGameStatsPlayerTimedEventStart(Player * ptPlayer, StatCounterType eType, int32 nParam0, int32 nParam1, BOOL bReportIfUnfinished);  // DWARF records only X/Server/SGameStats.cpp:1571; no header declaration found.
void SPlayerRegisterPowerForRecharge(Player * ptPlayer, SNO snoPower);  // DWARF records only X/Server/SPlayer.cpp:7045; no header declaration found.
BOOL ItemPlayerCanAssignLegendaryPower(const Player & tPlayer, GBID gbidItem, LegendaryPowerSlotType eSlotType);  // DWARF records only X/Common/Items.cpp:10837; no header declaration found.
void SPlayerRefreshWaypointDestination(ACDID idACDPlayer);  // DWARF records only X/Server/SPlayer.cpp:13243; no header declaration found.
void SPlayerRemoveLegendaryPowerInSlot(Player & tPlayer, const int32 nSlot);  // DWARF records only X/Server/SPlayer.cpp:6333; no header declaration found.
void SPlayerSetLicensesAndSessionFlags(const Player & tPlayer, const OnlineService::ContentLicenseArray & tContentLicenses, const OnlineServiceServer::LicenseArray & tRawLicenses, uint32 uSessionFlags);  // DWARF records only X/Server/SPlayer.cpp:10618; no header declaration found.
void SGameSendDungeonFinderSeedToPlayer(Player & tPlayer);  // DWARF records only X/Server/SGame.cpp:8521; no header declaration found.
void PlayerSendHeroStateCurrencyToClient(Player & tPlayer, BOOL bOnlySendDirtyCurrencies);  // DWARF records only X/Server/SPlayer.cpp:2603; no header declaration found.
























BOOL PendingJoinMap::SetPlayerConnected(
    OnlineService::HeroId uHeroId,
    const ServerSocketID idServerSocket,
    const XLocale eLocale
    )
{
    OnlineService::PlayerResolveData * ptResolveData;
    Blizzard::Lock::ScopedMutex scopedLock(m_tLock);
    ptResolveData = m_tMap.LookupPtr(uHeroId);
    if (ptResolveData == NULL)
    {
        return FALSE;
    }
    ptResolveData->idServerSocket = idServerSocket;
    ptResolveData->eLocale = eLocale;
    ptResolveData->uJoinFlags |= (1 << OnlineService::PLAYER_JOIN_CONNECTED_BIT);


    return TRUE;
}




















BOOL PendingJoinMap::SetPlayerAchievementRegistrationComplete(
    OnlineService::HeroId uHeroId,
    OnlineService::ErrorCode errorCode)
{
    OnlineService::PlayerResolveData * ptResolveData;
    Blizzard::Lock::ScopedMutex scopedLock(m_tLock);
    ptResolveData = m_tMap.LookupPtr(uHeroId);
    if (ptResolveData == NULL)
    {
        return FALSE;
    }

    ptResolveData->uJoinFlags &= ~(1 << OnlineService::PLAYER_JOIN_WAITING_FOR_ACHIEVEMENT_REGISTRATION);
    if (errorCode == ERROR_NONE)
    {
        ptResolveData->uJoinFlags |= (1 << OnlineService::PLAYER_JOIN_ACHIEVEMENT_REGISTRATION_COMPLETE);
    }
    else
    {
        ptResolveData->uJoinFlags |= (1 << OnlineService::PLAYER_JOIN_ACHIEVEMENT_REGISTRATION_FAILED);
    }
    return TRUE;
}


BOOL PendingJoinMap::SetPlayerWaitingOnWorldCreation(
    OnlineService::HeroId uHeroId,
    BOOL bWaiting)
{
    OnlineService::PlayerResolveData * ptResolveData;
    Blizzard::Lock::ScopedMutex scopedLock(m_tLock);
    ptResolveData = m_tMap.LookupPtr(uHeroId);
    if (ptResolveData == NULL)
    {
        return FALSE;
    }

    if (bWaiting) ptResolveData->uJoinFlags |= (1 << OnlineService::PLAYER_JOIN_WAITING_ON_WORLD_CREATION); else ptResolveData->uJoinFlags &= ~(1 << OnlineService::PLAYER_JOIN_WAITING_ON_WORLD_CREATION);


    return TRUE;
}


BOOL PendingJoinMap::SetPlayerReplayJoinRecorded(
    OnlineService::HeroId uHeroId,
    BOOL bRecorded)
{
    OnlineService::PlayerResolveData * ptResolveData;
    Blizzard::Lock::ScopedMutex scopedLock(m_tLock);
    ptResolveData = m_tMap.LookupPtr(uHeroId);
    if (ptResolveData == NULL)
    {
        return FALSE;
    }

    if (bRecorded) ptResolveData->uJoinFlags |= (1 << OnlineService::PLAYER_JOIN_REPLAY_JOIN_RECORDED); else ptResolveData->uJoinFlags &= ~(1 << OnlineService::PLAYER_JOIN_REPLAY_JOIN_RECORDED);


    return TRUE;
}


void SGameRefStringInitializeForGame()
{
    SGame & tSGame = SGameManager::GameGet();
    tSGame.ptRefStringMemPool = RefStringMemPoolCreate(tSGame.ptMemoryInterface, FALSE);
}

BOOL SWorldResolveLocationFromSavePointData(const SavePointData * ptSavePoint, WorldPlace & tPlace);  // X/Server/SWorld.cpp
void GizmoSendSavePointInfoToPlayer(const PlayerList & listPlayers, const SavePointData & tSavePoint);  // X/Server/SGizmo.cpp
void GizmoSetHearthPortal(Player & tPlayer, const WorldPlace & tPlace, BOOL bSave);  // X/Server/SGizmo.cpp

void SGameRefStringDisposeForGame()
{
    SGame & tSGame = SGameManager::GameGet();
    RefStringMemPoolDelete(tSGame.ptRefStringMemPool, tSGame.ptMemoryInterface);
}

ACDID SSceneFindFirstGizmo(SSceneID idSScene, int32 eGizmoType);  // X/Server/SScene.cpp
BOOL ScenesCommonNavMeshNodeFromPlace(const WorldPlace & tPlace, NavMeshHandle * ptNavMeshHandle, const BOOL fCanActivateScenes, const BOOL fProcessInactive);  // X/Common/SceneCommon.cpp

static void sSavepointWorldCreated(
    SWorldID idSWorld,
    ACDID idACDPortalOperator,
    ACDID,
    uint32,
    PlayerWarpReason)
{
    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);

    POS pos = listDestPlayers.GetHeadPosition();
    while (pos)
    {
        Player * ptPlayer = listDestPlayers.GetNext(pos);

        WorldPlace placeSavePoint;
        if (SWorldResolveLocationFromSavePointData(&ptPlayer->tHeroStateData.tSavedData.tLastSavePoint, placeSavePoint) &&

            placeSavePoint.idSWorld == idSWorld)
        {
            PlayerList listPlayer(PlayerListGetElementPool());
            PlayerListGetSingle(listPlayer, *ptPlayer);


            if (ptPlayer->bIsPrimaryPlayerServerOnly)
            {


                GizmoSendSavePointInfoToPlayer(listPlayer, ptPlayer->tHeroStateData.tSavedData.tLastSavePoint);

                if (ptPlayer->idPlayerACD == idACDPortalOperator && ptPlayer->idPlayerACD != ACDID_INVALID)
                {
                    ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
                    if (ptACDPlayer)
                    {
                        BOOL bHasSavePoint = ptPlayer->tHeroStateData.tSavedData.tLastSavePoint.IsValid();

                        WorldPlace placeCurr = ptACDPlayer->GetPlace();









                        if (bHasSavePoint)
                        {
                            ACDID idACDPortal = ACDID_INVALID;
                            if (placeCurr.idSWorld != SWORLDID_INVALID)
                            {
                                NavMeshHandle tNavMeshHandle;
                                if (ScenesCommonNavMeshNodeFromPlace(placeCurr, &tNavMeshHandle, TRUE, FALSE))
                                {
                                    idACDPortal = SSceneFindFirstGizmo(tNavMeshHandle.idSScene, 12);
                                }
                            }

                            ActorCommonData * ptACDPortal = ACDTryToGet(idACDPortal);
                            if (ptACDPortal)
                            {

                                WorldPlace placeSavePointLoc;
                                if (SWorldResolveLocationFromSavePointData(&ptPlayer->tHeroStateData.tSavedData.tLastSavePoint, placeSavePointLoc))
                                {

                                    const float MIN_DISTANCE_TO_SPAWN_PORTAL = 150.0f;
                                    const float MIN_DISTANCE_TO_SPAWN_PORTAL_SQR = MIN_DISTANCE_TO_SPAWN_PORTAL * MIN_DISTANCE_TO_SPAWN_PORTAL;
                                    if (placeSavePointLoc.idSWorld != placeCurr.idSWorld ||
                                        Vector2D::DistanceSquared(placeSavePointLoc.wp.As2D(), placeCurr.wp.As2D()) >= MIN_DISTANCE_TO_SPAWN_PORTAL_SQR)
                                    {


                                        if (ptPlayer->bIsPrimaryPlayerServerOnly)
                                        {
                                            for (Player * ptNextPlayer = PlayerGetFirstInGame(); ptNextPlayer; ptNextPlayer = PlayerGetNextInGame(ptNextPlayer))
                                            {
                                                if (PlayersAreLocalToEachOther(*ptNextPlayer, *ptPlayer))
                                                {
                                                    GizmoSetHearthPortal(*ptNextPlayer, placeSavePointLoc, TRUE);
                                                }
                                            }
                                        }



                                        const int32 SAVE_POINT_PORTAL_PING_TICKS = 1800;
                                        PingID idPing = Minimap::PingLocation(ptACDPortal->GetPlace(), SAVE_POINT_PORTAL_PING_TICKS, ptPlayer->uHeroId, SNO_NONE, FALSE, (uint32)-1, SNO_NONE, SNO_NONE, 0.0f, SNO_NONE);
                                        ptACDPortal->AttributesSetInt(ATTRIBUTE_PING, idPing);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

}

#include "../Common/GameTesting.h"
static void sSGameProcessPendingPlayers();  // :1670
static void sStartingWorldCreated(
    SWorldID idSWorld,
    ACDID,
    ACDID,
    uint32,
    PlayerWarpReason)

{
    BOOL bInitiateTesting = FALSE;












































    {
        const CmdLineParams * ptCmdLineParams = CmdLineGetParams();
        if (ptCmdLineParams->eTestType != TESTTYPE_NONE)
        {
            GameTesting::SetTestType(ptCmdLineParams->eTestType);
            if (ptCmdLineParams->nTestDumpFreq)
                GameTesting::SetDumpFPSFequency(ptCmdLineParams->nTestDumpFreq);

            if (ptCmdLineParams->nTestSampleTime)
                GameTesting::SetSampleTime(ptCmdLineParams->nTestSampleTime);

            if (ptCmdLineParams->nTestWaitTime)
                GameTesting::SetWorldAdvanceWaitTime(ptCmdLineParams->nTestWaitTime);

            if (ptCmdLineParams->szTestFilter[0])
                GameTesting::SetFilter(ptCmdLineParams->szTestFilter);

            if (ptCmdLineParams->szTestListPath[0])
                GameTesting::SetListPath(ptCmdLineParams->szTestListPath);

            if (ptCmdLineParams->szTestEntryPoint[0])
                GameTesting::SetEntryPoint(ptCmdLineParams->szTestEntryPoint);

            if (ptCmdLineParams->dwMinFPSScreenshot)
                GameTesting::SetMinFPSToTriggerScreenshot(ptCmdLineParams->dwMinFPSScreenshot);

            bInitiateTesting = TRUE;
        }
    }




    SGameID idSGame = SGameManager::GetCurrentGame();
    SGame & tSGame = SGameManager::GameGet(idSGame);

    XGrowableMapTS<unsigned long, OnlineService::PlayerResolveData>::MapContentsArray tPendingPlayers;
    tSGame.tPendingJoinMap.Copy(tPendingPlayers);
    for (int32 i = 0; i < tPendingPlayers.GetArraySize(); i++)
    {
        OnlineService::PlayerResolveData & tResolvedPlayer = tPendingPlayers[i];
        if (tResolvedPlayer.uJoinFlags & (1 << OnlineService::PLAYER_JOIN_WAITING_ON_WORLD_CREATION))
        {
            tSGame.tPendingJoinMap.SetPlayerWaitingOnWorldCreation(tResolvedPlayer.uHeroId, FALSE);
        }
    }
    sSGameProcessPendingPlayers();
}
#include "../Sigma/MemoryPools.h"
#include "../Common/GameConstants.h"
void SGameInitializeApp()
{
}
namespace Console {
namespace Online {
BOOL PartyGetPlayerAccountAndHero(int32, D3::Account::SavedDefinition *, D3::Hero::SavedDefinition *);  // X/Client/Console/Online.cpp (mangled name in the SGameInitializeGame call)
}
}
#include "../Sigma/SigmaTiming.h"
void SGameDisposeApp()
{
}

#include "../Common/OnlineService/OnlineServiceLogStream.h"
#include "../Common/Achievements/Console/ConsoleAchievements.h"

static SGameID sSGameCreate(const GameParams & tParams); static void sSGameGlobalsInit(SGameID idSGame, const GameParams & tParams);  // SGame.cpp:1397 (defined at 934: 1397-1418 hold relocated declarations), :959
void SGameResolveWorldState(const GameParams & tParams, const D3::Hero::SavedDefinition & tHeroDefinition);  // SGame.cpp:5606
SNO SGameGetStartWorld(); void DungeonFinderOverridesInit(DungeonFinderOverrides & tDungeonFinderOverrides); Act ActGetOpenWorldDefaultAct();  // SGame.cpp:7569, X/Server/Definitions/DungeonFinderDefinition.cpp:639, X/Common/Act.cpp
SGameID SGameInitializeGame(
	GameParams & tParams,
	const OnlineService::PlayerResolveData * ptResolvedPlayer)
{


	GameSetActiveGameCommonData(NULL, FALSE);


	D3::Hero::SavedDefinition tResumeHeroDefinition;

	if (tParams.bResumeFromSave)
	{





























		if (tParams.eGameType != GT_CHALLENGE_RIFTS)
		{
			Console::Online::PartyGetPlayerAccountAndHero(0xff, NULL, &tResumeHeroDefinition);  // 0xff: literal in the binary (orr w0, wzr, #0xff); constant name unknown
		}


		if (tResumeHeroDefinition.has_digest() &&
			tParams.nGameParts != GAME_PARTS_HIDDEN_SERVER)
		{
			const D3::Hero::Digest & tDigest = tResumeHeroDefinition.digest();
			tParams.eAct = (Act)tDigest.last_played_act();
			tParams.snoInitialQuest = tDigest.last_played_quest();
			tParams.nInitialQuestStepUID = tDigest.last_played_quest_step();
		}
	}


	sSGameCreate(tParams);

	if (tParams.bValid)
	{
		GameCommonData & tGCD = SGameGetGameCommonData();

		int32 nMonsterLevel = tParams.nMonsterLevel;
		if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
		{
			nMonsterLevel = 70;  // literal 0x46 in the binary
		}


		tGCD.tGameSyncedData.nMonsterLevel = nMonsterLevel;



		SGameInitAnniversaryEventFlags();


		SubsystemsInitialize(sgtSGameSubsystems, sgnNumSGameSubsystems);


		tGCD.GameModeSet(TRUE);


		SNOFilesUpdate();










		const D3::Hero::Digest & tDigest = tResumeHeroDefinition.digest();
		SGameSetCreatorInfo(tDigest.hero_id().id_low(), tDigest.hero_name().c_str(), "");




		SGameResolveWorldState(tParams, tResumeHeroDefinition);






		if (GameGetParts() != GAME_PARTS_HIDDEN_SERVER && !(tParams.dwCreationFlags & (1 << GAMECREATEPARAM_UTILITY)))
		{

			ResolvedPortalDestination tDest;
			ActGetCurrentStartLocation(tDest);
			SNO snoWorldStart = SGameGetStartWorld();
			SWorldCreate(snoWorldStart, FALSE, FALSE, sStartingWorldCreated, ACDID_INVALID, ACDID_INVALID, tDest.snoWorld == snoWorldStart ? tDest.uEntranceGUID : 0, WARP_UNKNOWN, FALSE, 0);
		}

		if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
		{
			GameFlagSet("AutoRiftBlizzcon", TRUE);
		}
		else if (tParams.dwCreationFlags & (1 << GAMECREATEPARAM_AUTO_RIFT))
		{
			GameFlagSet("AutoRift", TRUE);
		}

		TraceStat("gamestart", ACDID_INVALID, "%s", tParams.szGameName);
	}

	SGameID idSGame = SGameManager::GetCurrentGame();

	LPCSTR szGameTag = "";
	const SGame & tSGame = SGameManager::GameGet(idSGame);
	if (tSGame.ptSGameGlobals)
	{
		szGameTag = tSGame.ptSGameGlobals->tMatchmakingData.szGameTag;
	}
	D3::OnlineService::GameCreateParams protoParams;
	tParams.Export(protoParams, szGameTag);
	ONLINE_SERVICE_LOG(SLVL_INFO, 3, OUTPUTSTREAM_DEFAULT, OnlineService::GameIDLabel(idSGame) << " SGameInitializeGame idSGame=" << idSGame << " GameCreateParams=" << protoParams);



	if (tSGame.ptSGameGlobals)
	{
		UpdateFunctionInitialize(UPDATE_GAME_COMMUNITY_BUFFS, (DataID)-1, tSGame.ptSGameGlobals->tCommunityBuffsUpdateData);
		ServerQueueUpdateFunction(60, tSGame.ptSGameGlobals->tCommunityBuffsUpdateData);
	}
	GameServerCodeLeave_Tracked(TRUE);

	return idSGame;
}
static SGameID sSGameCreate(const GameParams & tParams)  // DWARF SGame.cpp:1397 (lines 1397-1418 hold relocated declarations)
{
	SGameID idSGame = SGameManager::GameAllocate();
	SGameManager::SetCurrentGame(idSGame);
	SubsystemsInitialize(sgtSGameMemorySubsystems, sgnNumSGameMemorySubsystems);
	sSGameGlobalsInit(idSGame, tParams); return idSGame;
}
void DPSTrackingInfo::SetWindowSeconds(float flSeconds)
{

	if (flSeconds <= 0.0f)  // inferred: linker-removed copy (0xd4 bytes) keeps only rows 944:19/944:9 and an early-return block at 946; condition and 946 body are guesses
	{
		m_uWindowTicks = 0; m_flOOWindowSecs = 0.0f;
		return;
	}


	m_uWindowTicks = Math::FloatRoundToInt(flSeconds * TICKS_PER_SECOND);
	m_flOOWindowSecs = 1.0f / flSeconds;

}




static void sSGameGlobalsInit(
	SGameID idSGame,
	const GameParams & tParams)
{
	SGame & tSGame = SGameManager::GameGet(idSGame);


	tSGame.tStartParams = tParams;



	tSGame.ptSGameGlobals = (SGameGlobals *)SigmaMemoryNew(sizeof(SGameGlobals), 4, tSGame.ptMemoryInterface, TRUE);




	SGameGlobals & tSGameGlobals = *tSGame.ptSGameGlobals;

	for (int32 i = 0; i < 10; i++)
	{
		tSGameGlobals.szLogHistory[i][0] = 0;
	}

	tSGameGlobals.nLogHistoryCurrentIndex = 9;

	tSGameGlobals.ptSGamePrivate = (SGamePrivateGlobals *)SigmaMemoryNew(sizeof(SGamePrivateGlobals), 4, tSGame.ptMemoryInterface, TRUE);
	SGamePrivateGlobals * ptSGamePrivate = tSGameGlobals.ptSGamePrivate;

	for (int32 i = 0; i < TICK_UPDATE_BUCKETS; i++)
	{
		ptSGamePrivate->tUpdateFunctionQueue[i].Initialize(tSGame.ptMemoryInterface);
		ptSGamePrivate->tUpdateFunctionQueue[i].SetBlockCount_Tracked(64);  // inferred: block count 0x40 and grow (1, 0x40) are in the merged stores; no inlined DIE survives
		ptSGamePrivate->tUpdateFunctionQueue[i].SetGrow(XALLOC_GROW_LINEAR, 64);  // inferred, see above

		ptSGamePrivate->tUpdateFunctionQueue[i].SetShrinkable(TRUE);
	}


	ptSGamePrivate->fUpdateTimeExhausted = FALSE;

	for (int32 i = 0; i < 2; i++)
	{
		tSGameGlobals.atExternalSignals[i].tGameSignalsList.Initialize(tSGame.ptMemoryInterface);
		tSGameGlobals.atExternalSignals[i].tDisconnectedSocketList.Initialize(tSGame.ptMemoryInterface);
	}


	tSGameGlobals.ptActiveSignals = &tSGameGlobals.atExternalSignals[0];
	tSGameGlobals.ptInactiveSignals = &tSGameGlobals.atExternalSignals[1];

	tSGameGlobals.dwFlags = 0;
	tSGameGlobals.dwFlags2 = 0;
	tSGameGlobals.snoInitialWorld = tParams.snoInitialWorld;
	tSGameGlobals.snoInitialQuest = tParams.snoInitialQuest;
	tSGameGlobals.nInitialQuestStepUID = tParams.nInitialQuestStepUID;
	tSGameGlobals.bResumeFromSave = tParams.bResumeFromSave;
	tSGameGlobals.uEntranceGUIDOverride = tParams.uEntranceGUIDOverride;

	tSGameGlobals.tCreatorSavePoint.Reset();
	tSGameGlobals.tReturnPoint.Reset();
	tSGameGlobals.idACDReturnPointPortal = ACDID_INVALID;
	tSGameGlobals.idGreedPortal = ACDID_INVALID;

	tSGameGlobals.uNextDynamicEntranceGUID = 100000;
	tSGameGlobals.snoOpenWorldHubWorld = SNO_NONE;
	tSGameGlobals.uOpenWorldHubEntranceGUID = 0;


	tSGameGlobals.tMatchmakingData.dwGameMadePublicTick = 0;
	tSGameGlobals.tMatchmakingData.dwLastPlayerLeaveTick = 0;
	tSGameGlobals.tMatchmakingData.bNephalemValorBuffGranted = FALSE;
	tSGameGlobals.tMatchmakingData.bMatchmakingServerState = FALSE;
	tSGameGlobals.tMatchmakingData.szGameTag[0] = 0;
	tSGameGlobals.tMatchmakingData.szServerPool[0] = 0;

	RandInit(tSGameGlobals.rng);

	if (tParams.dwSeed != (uint32)-1)
	{

		RandSetSeed_Internal(tSGameGlobals.rng, tParams.dwSeed);
	}


	RandSetSeed_Internal(tSGameGlobals.tDungeonFinderData.tDungeonFinderRNG, RandGetSeed(tSGameGlobals.rng));

	RandInit(tSGameGlobals.rngSyncedAnim);


	strcpy(tSGameGlobals.szGameName, tParams.szGameName);


	GameSetActiveGameCommonData(&tSGame.tGCD, FALSE);
	tSGame.tGCD.Initialize(tParams);

	tSGameGlobals.tDungeonFinderData.listAvailableLevelAreas.Initialize(MemoryPoolsGetDATAIDListPool());


	DungeonFinderOverridesInit(tSGameGlobals.tChallengeRift.tDungeonFinderOverrides);




	if (tSGame.tGCD.tGameSyncedData.eOpenWorldDefaultAct == ACT_INVALID)
	{

		if (tSGame.tStartParams.ePreferredAdventureAct != ACT_INVALID)
		{
			tSGame.tGCD.tGameSyncedData.eOpenWorldDefaultAct = tSGame.tStartParams.ePreferredAdventureAct;
		}
		else
		{

			tSGame.tGCD.tGameSyncedData.eOpenWorldDefaultAct = ActGetOpenWorldDefaultAct();
		}
	}






























































	tSGameGlobals.dwGameHash = 0;
	tSGameGlobals.nBoostLevel = -1;
	tSGameGlobals.nMaxPlayers = 0;
	tSGameGlobals.nMonstersKilled = 0;
	tSGameGlobals.dwGameStartAppTime = AppGetTicks();
	tSGameGlobals.dwUniqueGameKey = GetMachineTicks();
	tSGameGlobals.dwGameEventNumber = 1;
	tSGameGlobals.dwLastAppTimeWithPlayer = tSGameGlobals.dwGameStartAppTime;
	tSGameGlobals.dwPlayerCounter = 1;
	tSGameGlobals.dwLastChampRareUniqueKilled = 0;
	tSGameGlobals.nLargestPVPScoreDeficitTeam0 = 0;
	tSGameGlobals.nLargestPVPScoreDeficitTeam1 = 0;
	tSGameGlobals.tUTCGameStartTime = Blizzard::Time::GetTimestamp();
	tSGameGlobals.tSimTimeSum = 0;
	tSGameGlobals.tSimTimesSpikes = 0;
	tSGameGlobals.tSimTimePos = tSGameGlobals.tUTCGameStartTime % (sizeof(tSGameGlobals.tSimTimes) / sizeof(tSGameGlobals.tSimTimes[0]));
	ZeroMemory(tSGameGlobals.tSimTimes, sizeof(tSGameGlobals.tSimTimes));

	tSGameGlobals.tConversationVarMap.Initialize(16, FALSE, NULL);
	tSGameGlobals.tConversationRandomLineMap.Initialize(16, FALSE, NULL);


	tSGameGlobals.bGameCreatorInfoSet = FALSE;
	tSGameGlobals.uHeroIdCreator = OnlineService::INVALID_HERO_ID;
	tSGameGlobals.uszCreatorHeroName[0] = 0;
	tSGameGlobals.uszCreatorAccountName[0] = 0;

	tSGameGlobals.dwActivatedWaypoints = 0;

	tSGameGlobals.listCompletedBossEncounters.Initialize(MemoryPoolsGetDATAIDListPool());


	tSGameGlobals.dwNextBNetGameUpdate = AppGetTicks();
	tSGameGlobals.uPandemoniumFlags = 0;
	tSGameGlobals.uTicksSinceLastMessage = 0;
	tSGameGlobals.tDPSTrackingInfo.SetWindowSeconds(DPS_TRACKING_WINDOW_DEFAULT_SECONDS);  // folded to 5.0f (300 ticks / 0.2 in the binary)



	tSGameGlobals.tPerformanceTrackingInfo.Reset();

	tSGameGlobals.uNextServerVariableCheckTick = 0;
	tSGameGlobals.uNextServerPartyGuideCheckTick = 0;

	ZeroMemory(tSGameGlobals.pauseMessages, sizeof(tSGameGlobals.pauseMessages));
	tSGameGlobals.dwPauseMessageDropCount = 0;
	tSGameGlobals.dwLoggingOutMessageDropCount = 0;

	tSGameGlobals.dwKeyWardenKilledFlags = 0;

	tSGameGlobals.tMapPendingCCMessages.Initialize(16, FALSE, NULL);

	tSGameGlobals.flBlizzconRiftTier1RewardDropChance = BLIZZCON_RIFT_REWARD_DROP_CHANCE_TIER1;
	tSGameGlobals.flBlizzconRiftTier2RewardDropChance = BLIZZCON_RIFT_REWARD_DROP_CHANCE_TIER2;

	tSGameGlobals.dwLastTimeVerboseSlowFrameLog = GameGetServerTime();
	tSGameGlobals.dwTimeBetweenVerboseSlowFrameLogs = DEFAULT_TIME_BETWEEN_VERBOSE_SLOW_FRAME_LOGS;
	tSGameGlobals.dwVerboseSlowFrameLogMaxPowersPerLog = 3;






	if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) && !GameIsWeeklyChallenge())
	{




















		AppGlobals & tAppGlobals = sgtAppGlobals;


		tSGameGlobals.ptAchievementsInterface = static_cast<Console::Achievements *>(tAppGlobals.ptAchievements)->CreateServerAchievementsInterface();
	}
	else
		tSGameGlobals.ptAchievementsInterface = NULL;
}

#include "../Common/Achievements/Console/ConsoleAchievements.h"
#include "Actors/Actors.h"
static void sSGameDispose(SGameID idSGame); void SGameBootPendingPlayer(OnlineService::HeroId uHeroId, QuitReason eQuitReason, const char * pszBootLogReason, ServerSocketID idServerSocket);  // X/Server/SGame.cpp:1424, :7719



static void sSGameGlobalsDispose(
    SGameID idSGame)
{
    SGame & tSGame = SGameManager::GameGet(idSGame);

    tSGame.ptSGameGlobals->tConversationVarMap.Dispose();
    tSGame.ptSGameGlobals->tConversationRandomLineMap.Dispose();


    if (tSGame.ptSGameGlobals->ptAchievementsInterface)
    {


















        AppGlobals & tAppGlobals = sgtAppGlobals;


        tAppGlobals.ptAchievements->DestroyAchievementsInterface(tSGame.ptSGameGlobals->ptAchievementsInterface);
        tSGame.ptSGameGlobals->ptAchievementsInterface = NULL;
    }


    SGamePrivateGlobals * ptSGamePrivate = tSGame.ptSGameGlobals->ptSGamePrivate;

    for (int32 i = 0; i < TICK_UPDATE_BUCKETS; i++)
    {
        ptSGamePrivate->tUpdateFunctionQueue[i].Dispose();
    }

    for (int32 i = 0; i < 2; i++)
    {
        tSGame.ptSGameGlobals->atExternalSignals[i].tGameSignalsList.Dispose();
        tSGame.ptSGameGlobals->atExternalSignals[i].tDisconnectedSocketList.Dispose();
    }

    tSGame.ptSGameGlobals->listCompletedBossEncounters.Dispose();

    tSGame.ptSGameGlobals->tDungeonFinderData.listAvailableLevelAreas.Dispose();


    tSGame.ptSGameGlobals->tMapPendingCCMessages.Dispose();

    SigmaMemoryFree(tSGame.ptSGameGlobals->ptSGamePrivate, tSGame.ptMemoryInterface); tSGame.ptSGameGlobals->ptSGamePrivate = NULL;
    SigmaMemoryFree(tSGame.ptSGameGlobals, tSGame.ptMemoryInterface); tSGame.ptSGameGlobals = NULL;
    tSGame.tGCD.Dispose();
}




void SGameDisposeGame()
{



    SGame & tSGame = SGameManager::GameGet();


    SGameID idSGame = SGameManager::GameGet().id;
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();




    ActorsDumpGarbageCollectionInfo();

    tSGameGlobals.dwFlags |= (1 << SGAMEGLOBALS_SHUTTING_DOWN);












    {
        XGrowableMapTS<OnlineService::HeroId, OnlineService::PlayerResolveData>::MapContentsArray tPendingPlayers;
        tSGame.tPendingJoinMap.Copy(tPendingPlayers);
        for (int32 i = 0; i < tPendingPlayers.GetArraySize(); i++)
        {

            OnlineService::PlayerResolveData & tResolvedPlayer = tPendingPlayers[i];
            SGameBootPendingPlayer(
                tResolvedPlayer.uHeroId,
                QUIT_REASON_SHUTDOWN_GAME,
                NULL,
                tResolvedPlayer.idServerSocket);
        }
        tSGame.tPendingJoinMap.RemoveAll();




























    }


    SubsystemsDispose(sgtSGameSubsystems, sgnNumSGameSubsystems);


    sSGameDispose(idSGame);











}
#include "../Common/Nav/NavZone.h"
#include "../Sigma/SigmaProf.h"
#include "System/SNephalemRift.h"
#include "System/SetEncounter.h"
BOOL QuestIsEligibleForMatchmaking();  // X/Common/Quests/Quests.cpp:2769
namespace Console { namespace Online { BOOL PartyGetGameTag(int32 & pnTag); void PartySetMatchmakingEnabled(BOOL bMatchmakingEnabled); } }  // X/Client/Console/Online.cpp:4798, :4059

uint32 ServerGameConnectionGetTotalCount(); GameConnectionID ServerGetOnlyGameConnection();  // X/Server/Network/Server.cpp:2160, :2088
BOOL ReplayIsRecordingGame(); BOOL ReplayIsPlaying();  // X/Server/Replay.cpp:877, :804
Player * PlayerGetByGameAccount(const OnlineService::GameAccountHandle & tGameAccount);  // X/Common/Player.cpp:1571
bool SGameGetPendingPlayerByAccount(const OnlineService::GameAccountHandle & tAccountId, OnlineService::PlayerResolveData & tResolvedPlayer);  // X/Server/SGame.cpp:7784
void SPlayerAchievementEarned(const OnlineService::GameAccountHandle & tAccountId, const OnlineService::AchievementId & achievementId);  // X/Server/SPlayer.cpp:12917
void SPlayerHealAchievementPointLeaderboard(Player & tPlayer); void SPlayerReconcileGuaranteedRewards(Player & player);  // X/Server/SPlayer.cpp:14255, :12799
void SPlayerQueueUnregisterAchievements(Player * ptPlayer); void SPlayerQueueFutureAchievementRegistrationAttempt(Player * ptPlayer);  // X/Server/SPlayer.cpp:3519, :3503
void SPlayerMarkConquestDelivered(Player & player, OnlineService::AchievementId achievementId);  // X/Server/SPlayer.cpp:14725
extern XVarUint32 s_varKnownListUpdateInterval; extern XVarInt32 s_varElapseGameTimeMin; extern XVarUint32 s_varGameSimThreshold;  // X/Server/SGame.cpp:323-325
extern XVarUint32 s_varMatchmakingPlayerExitFunctionalityEnabled; extern XVarUint32 s_varMatchmakingForceOpenDuration; extern XVarUint32 s_varMatchmakingCloseAfterPlayerExitDuration;  // X/Server/SGame.cpp:326-328
extern char s_szBroadcast[512]; extern XVarUint32 s_varMaxGameUpdateCount;  // X/Server/SGame.cpp:349
void SGameSendShutdownWarning(int32 nTime, ServerShutdownType eType); static void SGameBroadcast(const char * szMessage); void SGameForcePlayersQuit(QuitReason eReason);  // X/Server/SGame.cpp:7507, :7522, :7555
static void sSGameUpdateMatchmakingStatus(); static void sServerPartyGuideUpdate(); static void sServerVariableUpdate();  // X/Server/SGame.cpp:8179, :8074, :8052

void FastAttribGroupApplyDirty(AttribGroupID idAttribGroup); void GameDebugDumpInfo();  // X/Common/Attributes/FastAttribute.cpp:3250, X/Common/GameDebug.cpp:815
void LuaScriptOnSGameUpdateStart(); void LuaScriptResetTimeoutTimer();  // X/Server/LuaScript/LuaScript.cpp:1173, :5407
void PowerUpdateDeletion(Power * ptPower); BOOL ReplayIsReplayGame(); void SPlayerIssuePendingSave(); void SPlayerSetGameLeaderAccount(const OnlineService::GameAccountHandle & tAccountId);  // X/Server/Powers/Powers.cpp:3667, X/Server/Replay.cpp:860, X/Server/SPlayer.cpp:1203, :12442
void ServerCleanUpGameConnections(ExternalGameSignals & tExternalSignals); void ServerCleanupOrphanedPackets(); void ServerProcessIncomingMessages();  // X/Server/Network/Server.cpp:679, :1537, :1680
void ActorsDumpList(); void ActorsPerformGarbageCollection(); void GameTimersUpdate(); void TimersDumpList();  // X/Server/Actors/Actors.cpp:5036, :1249, X/Server/Timers.cpp:343, :143
void LuaScriptUpdate(); void PVPUpdate(); void SWorldCreateUpdate(BOOL bFlushing); void SWorldUpdateMonstersRemaining();  // X/Server/LuaScript/LuaScript.cpp:1225, X/Server/PVP.cpp:58, X/Server/SWorld.cpp:2254, :664
int32 ServerGetUnsentPacketCountTotal(); void ServerSendHeartBeats(); void ServerSendOutgoingMessages();  // X/Server/Network/Server.cpp:2218, :1693, :1505



static void sSGameDispose(
    SGameID idSGame)
{



    sSGameGlobalsDispose(idSGame);


    SubsystemsDispose(sgtSGameMemorySubsystems, sgnNumSGameMemorySubsystems);


    SGameManager::GameDelete(idSGame);


    if (SGameManager::GetCurrentGame() == idSGame)
    {
        SGameManager::SetCurrentGame(-1);
    }
}






static uint32 sUpdateSGameClock(const uint32 dwGameClockTicks)
{
    AppGlobals & tAppGlobals = sgtAppGlobals;
    GameClock & tGameClock = tAppGlobals.tSGameClock;
    uint32 dwTicksPerSec = tAppGlobals.GetTicksPerSec();

    if (GameClockDebugLag(dwGameClockTicks, tGameClock))
        return 0;

    uint32 dwClockTicksToUse;
    uint32 dwGameTicksToUse;
    GameClockAdvanceStart(tGameClock, dwGameClockTicks, dwClockTicksToUse, dwGameTicksToUse, dwTicksPerSec);

    BOOL bResetMachineTime = FALSE;
    if (dwGameTicksToUse > dwTicksPerSec && !ReplayIsPlayingMultiServerGame() && !ReplayIsPlayingServerCPUTest())
    {
        dwGameTicksToUse = dwTicksPerSec ? 1 : 0;
        dwClockTicksToUse = dwTicksPerSec ? 3000 / dwTicksPerSec : 0;
        bResetMachineTime = TRUE;
    }

    GameClockAdvanceComplete(tGameClock, bResetMachineTime, dwClockTicksToUse, dwGameTicksToUse, dwTicksPerSec);

    return dwGameTicksToUse;
}














static BOOL sPlayerSlotsInUseOrPendingPlayers()
{


    int32 nNumPlayers = PlayerGetNumPlayers();
    if (nNumPlayers > 0)
    {
        return TRUE;
    }


    SGame & tSGame = SGameManager::GameGet();
    return tSGame.tPendingJoinMap.Count() != 0;
}













































































extern XVarUint32 s_varPlayerConnectTimeout;  // X/Server/SGame.cpp:322
BOOL SQuestStateIsExploitable(); ServerSocketID ServerGetSocketIDFromGameConnectionID(GameConnectionID idGameConnection);  // X/Server/Quests/SQuests.cpp:5090, X/Server/Network/Server.cpp:2171
BOOL SGameJoinPendingPlayer(OnlineService::HeroId uHeroId); void SGameBootPendingPlayer(OnlineService::HeroId uHeroId, QuitReason eQuitReason, const char * pszBootLogReason, ServerSocketID idServerSocket);  // X/Server/SGame.cpp, :7719

static void sPendingPlayerRegisterForAchievements(
    OnlineService::HeroId uHeroId,
    const OnlineService::GameAccountHandle & tAccountId)
{

































    SGameManager::GameGet().tPendingJoinMap.SetPlayerAchievementRegistrationComplete(uHeroId, ERROR_NONE);
}





void SGameAchievementRegister(const OnlineService::GameAccountHandle & tAccountId)
{
}



















void SGameAchievementUnregister(const OnlineService::GameAccountHandle & tAccountId)
{
}



















static void sSGameProcessPendingPlayers()
{
    uint32 connectTimeout = s_varPlayerConnectTimeout.Get();

    Blizzard::Time::Second now = Blizzard::Time::Seconds();
    XGrowableMapTS<OnlineService::HeroId, OnlineService::PlayerResolveData>::MapContentsArray tPendingPlayers;
    SGameManager::GameGet().tPendingJoinMap.Copy(tPendingPlayers);

    for (int32 i = 0; i < tPendingPlayers.GetArraySize(); i++)
    {
        OnlineService::PlayerResolveData & tResolvedPlayer = tPendingPlayers[i];

        if (tResolvedPlayer.uJoinFlags & (1 << OnlineService::PLAYER_JOIN_CONNECTED_BIT))
        {

            if (!(tResolvedPlayer.uJoinFlags & ((1 << OnlineService::PLAYER_JOIN_WAITING_FOR_ACHIEVEMENT_REGISTRATION) | (1 << OnlineService::PLAYER_JOIN_ACHIEVEMENT_REGISTRATION_COMPLETE))))
            {


                sPendingPlayerRegisterForAchievements(tResolvedPlayer.uHeroId, tResolvedPlayer.tAccountId);
            }


            if ((tResolvedPlayer.uJoinFlags & ((1 << OnlineService::PLAYER_JOIN_ACHIEVEMENT_REGISTRATION_COMPLETE) | (1 << OnlineService::PLAYER_JOIN_WAITING_ON_WORLD_CREATION))) == (1 << OnlineService::PLAYER_JOIN_ACHIEVEMENT_REGISTRATION_COMPLETE))
            {

                SGameGlobals & tSGameGlobals = SGameGlobalsGet();



                if (!(SQuestStateIsExploitable() &&
                      tSGameGlobals.uHeroIdCreator != OnlineService::INVALID_HERO_ID &&
                      tSGameGlobals.uHeroIdCreator != tResolvedPlayer.tHeroDefinition.digest().hero_id().id_low() &&


                      !(tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_GAME_CREATOR_JOINED_BIT))))
                {




                    BOOL bCanJoinNow = tResolvedPlayer.bIsPrimary;

                    if (!bCanJoinNow)
                    {

                        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
                        {
                            bCanJoinNow = ptPlayer->bIsPrimaryPlayerServerOnly &&
                                ptPlayer->idGameConnection != GAMECONNECTIONID_INVALID &&
                                ServerGetSocketIDFromGameConnectionID(ptPlayer->idGameConnection) == tResolvedPlayer.idServerSocket;
                            if (bCanJoinNow)
                                break;
                        }


                    }


                    if (bCanJoinNow)
                    {


                        SGameJoinPendingPlayer(tResolvedPlayer.uHeroId);
                        continue;
                    }
                }
            }
        }
        if (now - tResolvedPlayer.tAdded > connectTimeout)
        {


            SGameBootPendingPlayer(tResolvedPlayer.uHeroId, QUIT_REASON_JOIN_FAILED, NULL, tResolvedPlayer.idServerSocket);
        }
    }
}



static void sSGameCleanupGamesWithoutPlayers()
{

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    int32 dwCurAppTime = AppGetTicks();

    if (sPlayerSlotsInUseOrPendingPlayers())
    {
        tSGameGlobals.dwLastAppTimeWithPlayer = dwCurAppTime;
        return;
    }

    SGame & tSGame = SGameManager::GameGet();
    if (tSGame.tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_UTILITY))
    {
        tSGameGlobals.dwLastAppTimeWithPlayer = dwCurAppTime;
        return;
    }



    if (ServerGameConnectionGetTotalCount())
    {
        return;
    }


    int32 nElapseGameTime = dwCurAppTime - tSGameGlobals.dwGameStartAppTime;

    BOOL bCleanupNow = FALSE;

    int32 nMinGameExistanceTime = s_varElapseGameTimeMin.Get();


    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        nMinGameExistanceTime = 1800;
    }


    if (GameTypeIsPvP())
    {
        nMinGameExistanceTime = 1800;
    }







    if (nElapseGameTime > nMinGameExistanceTime)
    {
        SGameID idSGame = SGameManager::GetCurrentGame();

        tSGameGlobals.dwFlags |= (1 << SGAMEGLOBALS_REQUEST_SHUT_DOWN);
    }
}




void UpdateFunctionInitialize(UpdateFunctionType nUpdateFunctionType, DataID idUpdate, UpdateData & tUpdateData)
{
    tUpdateData.nUpdateFunctionType = nUpdateFunctionType;
    tUpdateData.idUpdate = idUpdate;
    tUpdateData.dwGameTime = 0;
    tUpdateData.posQueue = NULL;
    tUpdateData.nCallsSinceQueueWrap = 0;
}







void ServerRemoveUpdateFunction(UpdateData & tUpdateData)
{
    POS posQueue = tUpdateData.posQueue;

    if (posQueue != NULL)
    {
        GameCommonData & tGCD = SGameGetGameCommonData();
        SGameGlobals & tSGameGlobals = SGameGlobalsGet();
        SGamePrivateGlobals * ptSGamePrivate = tSGameGlobals.ptSGamePrivate;

        uint32 dwCurGameTime = tGCD.dwServerTime;
        uint32 dwUpdateGameTime = tUpdateData.dwGameTime;

        UpdateFunctionType nUpdateFunctionType = tUpdateData.nUpdateFunctionType;
        DataID idUpdate = tUpdateData.idUpdate;



        int32 nCurQueueIndex = dwUpdateGameTime % TICK_UPDATE_BUCKETS;
        TUpdateFunctionList & tCurList = ptSGamePrivate->tUpdateFunctionQueue[nCurQueueIndex];




        tCurList.RemoveAt(posQueue);

        tUpdateData.posQueue = NULL;
    }
}



















































void ServerQueueUpdateFunction(int32 nTicksDelay, UpdateData & tUpdateData)
{
    GameCommonData & tGCD = SGameGetGameCommonData();
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    SGamePrivateGlobals * ptSGamePrivate = tSGameGlobals.ptSGamePrivate;





    if ((tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_NO_MORE_UPDATES_THIS_TICK)) &&
        nTicksDelay == 0)
    {
        nTicksDelay = 1;
    }



    uint32 dwCurGameTime = tGCD.dwServerTime;
    uint32 dwUpdateGameTime = dwCurGameTime + nTicksDelay;

    POS posQueue = tUpdateData.posQueue;
    UpdateFunctionType nUpdateFunctionType = tUpdateData.nUpdateFunctionType; DataID idUpdate = tUpdateData.idUpdate;

    uint32 dwDelayGameTime = tUpdateData.dwGameTime;

    if (posQueue != NULL && dwUpdateGameTime >= dwDelayGameTime)
        return;







    int32 nNewQueueLap = dwUpdateGameTime / TICK_UPDATE_BUCKETS;
    int32 nCurQueueLap = dwDelayGameTime / TICK_UPDATE_BUCKETS;
    if (nNewQueueLap > nCurQueueLap)
    {
        tUpdateData.nCallsSinceQueueWrap = 0;
    }


    ServerRemoveUpdateFunction(tUpdateData);

    int32 nNewQueueIndex = dwUpdateGameTime % TICK_UPDATE_BUCKETS;
    TUpdateFunctionList & tNewList = ptSGamePrivate->tUpdateFunctionQueue[nNewQueueIndex];

    UpdateFunctionItem tUpdateItem;
    tUpdateItem.nUpdateFunctionType = nUpdateFunctionType;
    tUpdateItem.idUpdate = idUpdate;

    tUpdateData.posQueue = tNewList.AddTail_Tracked(tUpdateItem);
    tUpdateData.dwGameTime = dwUpdateGameTime;
}


#include "ConversationMgr.h"
void PowerUpdate(PowerID idPower);  // X/Server/Powers/Powers.cpp
void SQuestSystemUpdate();  // X/Server/Quests/SQuest.cpp
void ActorAIUpdate(ActorID idActor);  // X/Server/AI/ActorAI.cpp
void ActorUpdateAttributes(ActorID idActor);  // X/Server/Actors/Actors.cpp
void ActorUpdateGizmo(ActorID idActor);  // X/Server/Actors/Actors.cpp
void ActorUpdateAreaDamage(ActorID idActor);  // X/Server/Actors/Actors.cpp
void FlippyUpdate(int32 idFlippy);  // X/Server/Flippy.cpp
void SPlayersUpdate();  // X/Server/SPlayer.cpp
void SConductorUpdate();  // X/Server/SConductor.cpp
void SGameStatsUpdate();  // X/Server/SGameStats.cpp
void STimedEventsUpdate();  // X/Server/STimedEvents.cpp
void PowersUpdateScriptedPowers();  // X/Server/Powers/Powers.cpp
void SQuestsUpdateBountySceneActivation();  // X/Server/Quests/SQuest.cpp
static void sSGameUpdateCommunityBuffs();
void SActorMovementUpdate(ACDID idACD);  // X/Server/Movement/SActorMovement.cpp
void SGameCheckOpenWorldModeAdvance();











typedef int32 FlippyID;
#include "Powers/Powers.h"
#include "Powers/PowerAPI.h"
#include "Actors/Actors.h"
#include "../Common/Marker/Marker.h"
void ActorsDeleteAllFlagged(); void ActorsFireSpawnEvents(); void PowerAPI_BuffDeleteAllFlagged(); void ACDInventoryHandleEquipmentOrSkillChanges(); CRefString FlippyGetDebugString(FlippyID idFlippy);  // Actors.cpp:4512, 4531, PowerAPI.cpp:11020, ActorInventory.cpp:9222, Flippies.cpp:1296


static void sServerUpdateCounters()
{


    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    SGamePrivateGlobals * ptSGamePrivate = tSGameGlobals.ptSGamePrivate;

    for (int32 i = 0; i < NUM_UPDATE_FUNCTION_TYPE; i++)
    {
        UpdateCounters & tUpdateCounters = ptSGamePrivate->tUpdateCounters[i];
        tUpdateCounters.nAveCallCount = tUpdateCounters.nCurCallCount;
        tUpdateCounters.nCurCallCount = 0;
    }
}
























































void ServerUpdateWasCalled(UpdateData & tUpdateData)
{
    tUpdateData.nCallsSinceQueueWrap++;
    tUpdateData.posQueue = NULL;
}
















static CRefString sUpdateFunctionItemToString(
    UpdateFunctionItem & tUpdateItem)
{
    CRefString sText = CRefString().Format("Update function type:%d idUpdate:%d ", tUpdateItem.nUpdateFunctionType, tUpdateItem.idUpdate);

    switch (tUpdateItem.nUpdateFunctionType)
    {

    case UPDATE_POWER:
        {
            Power * ptPower = PowerTryToGet(tUpdateItem.idUpdate);
            sText += CRefString().Format("power:%s ", ptPower ? SNOToString(POWER_SNO, ptPower->snoPower, FALSE).str() : "invalid power");
        }
        break;

    case UPDATE_QUESTS:
        sText += "Quest system update ";
        break;

    case UPDATE_ACD_MOVEMENT:
        {
            ActorCommonData * ptACD = ACDTryToGet((ACDID)tUpdateItem.idUpdate);
            sText += CRefString().Format("Movement update for actor: %s rarity: %d ", ptACD ? MarkerNameGetString(ptACD->tName) : "invalid actor", ptACD ? ptACD->eMonsterRarity : -1);
        }
        break;

    case UPDATE_ACTOR_AI:
        {
            Actor * ptActor = ActorTryToGet((ActorID)tUpdateItem.idUpdate);
            sText += CRefString().Format("AI update for actor: %s ", ptActor ? MarkerNameGetString(ptActor->tName) : "invalid actor");
        }
        break;

    case UPDATE_ACTOR_ATTRIBUTES:
        {
            Actor * ptActor = ActorTryToGet((ActorID)tUpdateItem.idUpdate);
            sText += CRefString().Format("Attribute update for actor: %s ", ptActor ? MarkerNameGetString(ptActor->tName) : "invalid actor");
        }
        break;

    case UPDATE_GIZMOS:
        {
            Actor * ptActor = ActorTryToGet((ActorID)tUpdateItem.idUpdate);
            sText += CRefString().Format("Gizmo update for gizmo actor: %s ", ptActor ? MarkerNameGetString(ptActor->tName) : "invalid actor");
        }
        break;

    case UPDATE_FLIPPIES:
        {
            sText += CRefString().Format("Flippy update for flippy: %s ", FlippyGetDebugString(tUpdateItem.idUpdate).str());
        }
        break;

    case UPDATE_PLAYERS:
        sText += "Players update ";
        break;

    case UPDATE_CONDUCTORS:
        sText += "Conductor system update ";
        break;

    case UPDATE_STATS:
        sText += "Game stats update ";
        break;

    case UPDATE_TIMEDEVENTS:
        sText += "Timed events update ";
        break;

    case UPDATE_ACTTRANSITIONS:
        sText += "Act Transitions update ";
        break;

    case UPDATE_CONVERSATIONS:
        sText += "Conversation system update ";
        break;

    case UPDATE_SCRIPTED_POWERS:
        sText += "Scripted Power system update ";
        break;

    case UPDATE_OPEN_WORLD_TRANSITION:
        sText += "Open WorldTransition update";
        break;

    case UPDATE_BOUNTY_SCENE_ACTIVATION:
        sText += "Bounty Scene Activation update";
        break;

    case UPDATE_GAME_COMMUNITY_BUFFS:
        sText += "Game Community Buffs update";
        break;
    case UPDATE_ACTOR_AREA_DAMAGE:
        {
            Actor * ptActor = ActorTryToGet((ActorID)tUpdateItem.idUpdate);
            sText += CRefString().Format("Area Damage update for actor: %s ", ptActor ? MarkerNameGetString(ptActor->tName) : "invalid actor");
        }
        break;

    default:
        sText += "Unknown update function type ";
        break;
    }

    return sText;
}




static void sServerProcessUpdate(UpdateFunctionItem & tUpdateItem)
{
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    SGamePrivateGlobals * ptSGamePrivate = tSGameGlobals.ptSGamePrivate;





    UpdateFunctionType nUpdateFunctionType = tUpdateItem.nUpdateFunctionType;
    ptSGamePrivate->tUpdateCounters[nUpdateFunctionType].nCurCallCount++;

    switch (tUpdateItem.nUpdateFunctionType)
    {

    case UPDATE_POWER:
        PowerUpdate(tUpdateItem.idUpdate);
        break;

    case UPDATE_QUESTS:
        SQuestSystemUpdate();
        break;

    case UPDATE_ACD_MOVEMENT:
        SActorMovementUpdate((ACDID)tUpdateItem.idUpdate);
        break;

    case UPDATE_ACTOR_AI:
        ActorAIUpdate((ActorID)tUpdateItem.idUpdate);
        break;

    case UPDATE_ACTOR_ATTRIBUTES:
        ActorUpdateAttributes((ActorID)tUpdateItem.idUpdate);
        break;

    case UPDATE_GIZMOS:
        ActorUpdateGizmo((ActorID)tUpdateItem.idUpdate);
        break;

    case UPDATE_FLIPPIES:
        FlippyUpdate(tUpdateItem.idUpdate);
        break;

    case UPDATE_PLAYERS:
        SPlayersUpdate();
        break;

    case UPDATE_CONDUCTORS:
        SConductorUpdate();
        break;

    case UPDATE_STATS:
        SGameStatsUpdate();
        break;

    case UPDATE_TIMEDEVENTS:
        STimedEventsUpdate();
        break;

    case UPDATE_ACTTRANSITIONS:
        SActTransitionsUpdate();
        break;

    case UPDATE_CONVERSATIONS:
        ConversationMgr::Update();
        break;

    case UPDATE_SCRIPTED_POWERS:
        PowersUpdateScriptedPowers();
        break;

    case UPDATE_OPEN_WORLD_TRANSITION:
        SGameCheckOpenWorldModeAdvance();
        break;

    case UPDATE_BOUNTY_SCENE_ACTIVATION:
        SQuestsUpdateBountySceneActivation();
        break;

    case UPDATE_GAME_COMMUNITY_BUFFS:
        sSGameUpdateCommunityBuffs();
        break;

    case UPDATE_ACTOR_AREA_DAMAGE:
        ActorUpdateAreaDamage((ActorID)tUpdateItem.idUpdate);
        break;

    default:
        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }
}




void sCheckForPendingCCMessages()

{
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();

    if (!tSGameGlobals.tMapPendingCCMessages.IsEmpty())
    {
        POS pos = tSGameGlobals.tMapPendingCCMessages.GetStartAssoc();
        while (pos != NULL)
        {
            ACDID idCurr;
            CCInfo tCCinfo;


            tSGameGlobals.tMapPendingCCMessages.GetNextAssoc(pos, idCurr, tCCinfo);

            if (ACDIsPlayer(tCCinfo.idACDAttacker))
            {
                Player * ptPlayer = PlayerGetByACD(tCCinfo.idACDAttacker);
                if (ptPlayer)
                {

                    FloatingNumberMessage tMessage;
                    tMessage.annTarget = ANNFromACD(idCurr);
                    tMessage.flDamage = 0.0f;
                    tMessage.eType = (FloatingNumberType)tCCinfo.eType;

                    PlayerList listDestPlayers(PlayerListGetElementPool());
                    listDestPlayers.AddTail_Tracked(ptPlayer);
                    Player * ptPlayerTarget = ACDGetControllingPlayer(idCurr);
                    if (ptPlayerTarget)
                    {
                        listDestPlayers.AddTail_Tracked(ptPlayerTarget);
                    }
                    MessageSendToClient(listDestPlayers, SMSG_FLOATING_NUMBER, &tMessage, sizeof(tMessage));
                }
            }
        }

        tSGameGlobals.tMapPendingCCMessages.RemoveAll();
    }
}






static void sServerProcessUpdateFunctions(
    BOOL bLogThisUpdate,
    int32 nTotalUpdateCount)
{


    GameCommonData & tGCD = SGameGetGameCommonData();
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    SGamePrivateGlobals * ptSGamePrivate = tSGameGlobals.ptSGamePrivate;

    Stopwatch & tStopwatch = tSGameGlobals.tStopwatch;
    float flTime;

    uint32 dwCurGameTime = tGCD.dwServerTime;

    if (bLogThisUpdate)
    {
        tStopwatch.Reset();
    }

    if (dwCurGameTime % 60 == 0)
    {
        sServerUpdateCounters();
    }

    if (bLogThisUpdate)
    {
        flTime = tStopwatch.Check();





        tStopwatch.Reset();
    }

    int32 nCurQueueIndex = dwCurGameTime % TICK_UPDATE_BUCKETS;

    ActorsDeleteAllFlagged();

    if (bLogThisUpdate)
    {
        flTime = tStopwatch.Check();




        tStopwatch.Reset();
    }

    PowerAPI_BuffDeleteAllFlagged();

    if (bLogThisUpdate)
    {
        flTime = tStopwatch.Check();




        tStopwatch.Reset();
    }

    ActorsFireSpawnEvents();

    if (bLogThisUpdate)
    {
        flTime = tStopwatch.Check();




        tStopwatch.Reset();
    }

    ACDInventoryHandleEquipmentOrSkillChanges();

    if (bLogThisUpdate)
    {
        flTime = tStopwatch.Check();




        tStopwatch.Reset();
    }

    TUpdateFunctionList & tCurList = ptSGamePrivate->tUpdateFunctionQueue[nCurQueueIndex];

    CRefString sLogString;

    while (!tCurList.IsEmpty())
    {
        POS posHead = tCurList.GetHeadPosition();


        UpdateFunctionItem tUpdateItem = tCurList.GetHead();
        tCurList.RemoveHead();

        if (bLogThisUpdate)
        {
            sLogString = sUpdateFunctionItemToString(tUpdateItem);
            tStopwatch.Reset();
        }

        sServerProcessUpdate(tUpdateItem);

        if (bLogThisUpdate)
        {
            flTime = tStopwatch.Check();






            tStopwatch.Reset();
        }


        ActorsDeleteAllFlagged();

        if (bLogThisUpdate)
        {
            flTime = tStopwatch.Check();




            tStopwatch.Reset();
        }

        ActorsFireSpawnEvents();

        if (bLogThisUpdate)
        {
            flTime = tStopwatch.Check();




            tStopwatch.Reset();
        }

        sCheckForPendingCCMessages();
    }
}





static void sRemovePlayerByAccount(
    OnlineService::GameAccountHandle & tAccountId)
{
    BOOL bFound = FALSE;

    Player * ptPlayer = PlayerGetByGameAccount(tAccountId);
    if (ptPlayer && ptPlayer->idGameConnection != GAMECONNECTIONID_INVALID)
    {

        SPlayerDisconnect(*ptPlayer, QUIT_REASON_REMOVED, TRUE);
    }
    else
    {

        OnlineService::PlayerResolveData tResolvedPlayer;
        if (SGameGetPendingPlayerByAccount(tAccountId, tResolvedPlayer))
        {
            SGameBootPendingPlayer(
                tResolvedPlayer.uHeroId,
                QUIT_REASON_REMOVED,
                NULL,
                tResolvedPlayer.idServerSocket);
        }

    }
}









static void sHandlePlayerAchievementEarned(
    const OnlineService::GameAccountHandle & tAccountId,
    const XVariant & tContext)
{




    OnlineService::AchievementId achievementId = tContext.AsUint64();
    SPlayerAchievementEarned(tAccountId, achievementId);
}



static void sHandleAchievementsInterfaceAvailability(OnlineService::GameAccountHandle & tAccountId,
    const XVariant & tContext)
{




    BOOL bAchievementsAvailable = tContext.AsUint64() == ERROR_NONE;
    Player * ptPlayer = PlayerGetByGameAccount(tAccountId);
    if (!ptPlayer)
        return;

    if (bAchievementsAvailable)
    {


        BoolDataMessage tMessageToClient;
        tMessageToClient.bVal = TRUE;

        PlayerList listDestPlayers(PlayerListGetElementPool());
        listDestPlayers.AddTail_Tracked(ptPlayer);
        MessageSendToClient(listDestPlayers, SMSG_ACHIEVEMENTS_AVAILABILITY, &tMessageToClient, sizeof(tMessageToClient));


        SPlayerHealAchievementPointLeaderboard(*ptPlayer);
        SPlayerReconcileGuaranteedRewards(*ptPlayer);
    }
    else
    {
        if (tContext.AsUint64() == OnlineService::ERROR_ACHIEVEMENTS_ALREADY_REGISTERED)
        {


            SPlayerQueueUnregisterAchievements(ptPlayer);
        }
        else
        {



            SPlayerQueueFutureAchievementRegistrationAttempt(ptPlayer);
        }
    }
}


static void sHandlePlayerClanIdUpdate(
    OnlineService::GameAccountHandle & tAccountId,
    const XVariant & tContext)
{




    uint64 uClanId = tContext.AsUint64();
    Player * ptPlayer = PlayerGetByGameAccount(tAccountId);
    if (ptPlayer)
    {


        ptPlayer->uGuildId = uClanId;
    }
}
















































































































































































































































































































































































































































































































































































































































































































static void sHandleConquestPosted(
    const OnlineService::GameAccountHandle & tAccountId,
    const XVariant & tContext)
{
    OnlineService::AchievementId achievementId;
    if (!tContext.GetUint64(achievementId))
    {
        return;
    }



    Player * ptPlayer = PlayerGetByGameAccount(tAccountId);
    if (!ptPlayer)
    {
        return;
    }




    SPlayerMarkConquestDelivered(*ptPlayer, achievementId);
}























































































static void sProcessAccountSignals(
    ExternalGameSignals & tExternalSignals)
{
    while (tExternalSignals.tGameSignalsList.Count())
    {
        GameSignal & tAccountSignal = tExternalSignals.tGameSignalsList.GetHead();

        switch (tAccountSignal.eSignal)
        {
        case GAST_REMOVE_PLAYER:
            sRemovePlayerByAccount(tAccountSignal.tAccountId);
            break;

        case GAST_GAME_LEADER_STATE_CHANGE:
            SPlayerSetGameLeaderAccount(tAccountSignal.tAccountId);
            break;

        case GAST_PLAYER_ACHIEVEMENT:
            sHandlePlayerAchievementEarned(tAccountSignal.tAccountId, tAccountSignal.tContext);
            break;

        case GAST_ACHIEVEMENTS_INTERFACE_AVAILABILITY:
            sHandleAchievementsInterfaceAvailability(tAccountSignal.tAccountId, tAccountSignal.tContext);
            break;

        case GAST_PLAYER_CLAN_ID_UPDATE:
            sHandlePlayerClanIdUpdate(tAccountSignal.tAccountId, tAccountSignal.tContext);
            break;

        case GAST_PERFORM_ESCROW:
            break;

        case GAST_ENTITLEMENTS_GRANTED:
            break;

        case GAST_CLEAR_PENDING_CONSUMABLE_SAVE:
            break;

        case GAST_EXTERNAL_REWARD_GRANTED:
            break;

        case GAST_GET_PLATINUM_BALANCE:
            break;

        case GAST_PARTY_CHANNEL_ID:
            break;

        case GAST_CONQUEST_POSTED:






            sHandleConquestPosted(tAccountSignal.tAccountId, tAccountSignal.tContext);
            break;

        default:




            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }

        tExternalSignals.tGameSignalsList.RemoveHead();
    }
}



static void sCleanupHero()
{

    ActorsDeleteAllFlagged();

    PowerAPI_BuffDeleteAllFlagged();


    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer)
        {
            FastAttribGroupApplyDirty(ptACDPlayer->AttributesGetAttributeGroup());
        }
    }


    PowerGlobals * ptPowerGlobals = SGameManager::GameGet().ptPowerGlobals;
    TDataArrayIterator<Power> tIt;
    while (Power * ptPower = ptPowerGlobals->pArrayPowers->Iterate(tIt))
    {

        if (ACDIsPlayer(ptPower->idACD))
        {
            PowerUpdateDeletion(ptPower);
        }
    }
}




























static void sServerIdle(
    uint32 dwGameUpdateCount)
{








    if (!ReplayIsReplayGame() && !ReplayIsRecordingGame())
    {
        uint32 dwMaxGameUpdateCount = s_varMaxGameUpdateCount.Get();
        dwGameUpdateCount = Math::Min(dwMaxGameUpdateCount, dwGameUpdateCount);
    }






























    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    ExternalGameSignals & tExternalSignals = *tSGameGlobals.ptActiveSignals;
    uint32 uSignalFlags = tExternalSignals.uGameManagerSignalFlags;
    tExternalSignals.uGameManagerSignalFlags = 0;


    if (uSignalFlags & (1 << SGAMEMANAGER_SIGNAL_DISABLE_PLAYER_SAVE_BIT))
    {
        tSGameGlobals.dwFlags |= (1 << SGAMEGLOBALS_DISABLED_HERO_SAVE);
    }

    if (uSignalFlags & (1 << SGAMEMANAGER_SIGNAL_CLEAR_SAVE_IN_PROGESS))
    {
        tSGameGlobals.dwFlags2 &= ~(1 << SGAMEGLOBALS2_SAVE_IN_PROGRESS);
    }


    SPlayerIssuePendingSave();

    ServerCleanupOrphanedPackets();


    if (tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_REQUEST_SHUT_DOWN))
    {
        return;
    }















    ThreadGlobals::Get().m_bSimulatingGame = TRUE;

    uint32 simTimeThreshold = s_varGameSimThreshold.Get();
    Blizzard::Time::Millisecond tSimStartTime = 0;

    if (simTimeThreshold)
    {
        tSimStartTime = Blizzard::Time::Milliseconds();
    }


    LuaScriptOnSGameUpdateStart();

    BOOL bPrevTraceAllocs;

    bPrevTraceAllocs = SigmaMemorySetTraceAllocations(TRUE);

    GameCommonData & tGCD = SGameGetGameCommonData();
    ServerCleanUpGameConnections(tExternalSignals);







    sProcessAccountSignals(tExternalSignals);

    if (GameIsBattleNetServer())
    {
        sSGameCleanupGamesWithoutPlayers();
    }




    if (!ReplayIsReplayGame())
    {
        sSGameProcessPendingPlayers();
    }




    if (uSignalFlags & (1 << SGAMEMANAGER_SIGNAL_BROADCAST_BIT))
    {
        const char * szBroadcast = s_szBroadcast;
        if (strlen(szBroadcast))
        {
            SGameBroadcast(szBroadcast);
        }
    }


    bool challengeRiftsDisabled = GameIsWeeklyChallenge() && !OnlineService::IsChallengeRiftsEnabled();

    if ((uSignalFlags & (1 << SGAMEMANAGER_SIGNAL_KILL_GAME_BIT)) || challengeRiftsDisabled)
    {
        if (challengeRiftsDisabled)
        {
            SGameSendShutdownWarning(0, SST_CHALLENGE_RIFTS_DISABLED);
        }


        SGameFinishGameForConsole(QUIT_REASON_SHUTDOWN_GAME);


        SGameGlobalsGet().dwFlags |= (1 << SGAMEGLOBALS_REQUEST_SHUT_DOWN);
    }

    if (uSignalFlags & (1 << SGAMEMANAGER_SIGNAL_REMOVE_PLAYERS_BIT))
    {
        SGameForcePlayersQuit(QUIT_REASON_SHUTDOWN_GAME);
    }

    if (uSignalFlags & (1 << SGAMEMANAGER_SIGNAL_DUMP_GAME_INFO_BIT))
    {






        GameDebugDumpInfo();
    }

    BOOL bLogThisUpdate = FALSE;
    Stopwatch & tStopwatch = tSGameGlobals.tStopwatch;
    float flTime;


    if (uSignalFlags & (1 << SGAMEMANAGER_SIGNAL_LOG_NEXT_UPDATE_BIT))
    {
        bLogThisUpdate = TRUE;

        tStopwatch.Reset();
    }


    if (tExternalSignals.nShutdownWarningTime)
    {
        SGameSendShutdownWarning(tExternalSignals.nShutdownWarningTime, tExternalSignals.eShutdownType);
        tExternalSignals.nShutdownWarningTime = tExternalSignals.eShutdownType = 0;
    }





















    if (bLogThisUpdate)
    {
        tStopwatch.Reset();
    }




    if (tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_GAME_PAUSED_BIT))
    {

        ServerProcessIncomingMessages();
        LuaScriptResetTimeoutTimer(); dwGameUpdateCount = 0;
    }

    if (tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_GAME_SOFT_PAUSED_BIT))
    {
        sCleanupHero();



        ServerProcessIncomingMessages();

        ServerQueueClientUpdate();

        ACDInventoryHandleEquipmentOrSkillChanges();

        ServerUpdateClients(TRUE);

        LuaScriptResetTimeoutTimer(); dwGameUpdateCount = 0;
    }

    if (tSGameGlobals.dwFlags2 & (1 << SGAMEGLOBALS2_GAME_FLAGS_CHANGED))
    {
        GameFlagSendSyncedFlags();
    }

    if (bLogThisUpdate)
    {
        flTime = tStopwatch.Check();




        tStopwatch.Reset();
    }



    if (tGCD.GameModeGet() != GAMEMODE_PLAYING)
    {

        DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
    }

    if (ReplayIsPlaying())
    {
        if (ServerGetOnlyGameConnection() == GAMECONNECTIONID_INVALID)
        {
            dwGameUpdateCount = 1;
        }
    }


    if (dwGameUpdateCount > 0)
    {
        tSGameGlobals.uTicksSinceLastMessage = dwGameUpdateCount - 1;
    }

    if (bLogThisUpdate)
    {
        flTime = tStopwatch.Check();




        tStopwatch.Reset();
    }

    for (uint32 i = 0; i < dwGameUpdateCount; i++)
    {


















        if (i > 0)
        {
            if (bLogThisUpdate)
            {
                flTime = tStopwatch.Check();

                tStopwatch.Reset();
            }

            ServerSendOutgoingMessages();
        }



        if (bLogThisUpdate)
        {
            tStopwatch.Reset();
        }



        if (ReplayIsReplayGame())
        {
            sSGameProcessPendingPlayers();
        }

        ServerProcessIncomingMessages();
        if (bLogThisUpdate)
        {
            flTime = tStopwatch.Check();





            tStopwatch.Reset();
        }

        tSGameGlobals.uTicksSinceLastMessage = 0;



        if (GameGetServerTime() % 1800 == 0)
        {
            ServerQueueClientUpdate();
        }

        ActorsPerformGarbageCollection();
        if (bLogThisUpdate)
        {

            flTime = tStopwatch.Check();




            tStopwatch.Reset();
        }














        SWorldCreateUpdate(FALSE);
        if (bLogThisUpdate)
        {

            flTime = tStopwatch.Check();




            tStopwatch.Reset();
        }


        NavZoneInstance::KeepLocks();


        GameTimersUpdate();

        if (bLogThisUpdate)
        {
            flTime = tStopwatch.Check();




            tStopwatch.Reset();
        }

        sServerProcessUpdateFunctions(bLogThisUpdate, dwGameUpdateCount);


        NavZoneInstance::ReleaseLocks();

        if (bLogThisUpdate)
        {

            flTime = tStopwatch.Check();




            tStopwatch.Reset();
        }


        tSGameGlobals.dwFlags |= (1 << SGAMEGLOBALS_NO_MORE_UPDATES_THIS_TICK);

        if (GameTypeIsPvP())
        {
            PVPUpdate();
        }

        LuaScriptUpdate();

        if (bLogThisUpdate)
        {
            flTime = tStopwatch.Check();




            tStopwatch.Reset();
        }

        uint32 uKnownListUpdateInterval = s_varKnownListUpdateInterval.Get();
        BOOL bUpdateKnownList = GameGetServerTime() % uKnownListUpdateInterval == 0;
        ServerUpdateClients(bUpdateKnownList);

        if (bLogThisUpdate)
        {
            flTime = tStopwatch.Check();




            tStopwatch.Reset();
        }

        ServerSendHeartBeats();

        if (bLogThisUpdate)
        {
            flTime = tStopwatch.Check();




            tStopwatch.Reset();
        }

        tGCD.dwServerTime++;


        tSGameGlobals.dwFlags &= ~(1 << SGAMEGLOBALS_NO_MORE_UPDATES_THIS_TICK);
    }











    SWorldsDeleteFlagged();
    SWorldUpdateMonstersRemaining();

    sSGameUpdateMatchmakingStatus();

    if (bLogThisUpdate)
    {
        flTime = tStopwatch.Check();




        tStopwatch.Reset();
    }

    LevelAreasDeleteFlagged();

    if (bLogThisUpdate)
    {
        flTime = tStopwatch.Check();

        tStopwatch.Reset();
    }

    if (GameGetParts() != GAME_PARTS_HIDDEN_SERVER)
    {
        ServerSendOutgoingMessages();

        if (bLogThisUpdate)
        {
            flTime = tStopwatch.Check();

            tStopwatch.Reset();
        }
    }

    SigmaMemorySetTraceAllocations(bPrevTraceAllocs);

    if (bLogThisUpdate)
    {
        flTime = tStopwatch.Check();




        tStopwatch.Reset();
    }

    sServerVariableUpdate();

    sServerPartyGuideUpdate();
    if (bLogThisUpdate)
    {

        flTime = tStopwatch.Check();




        tStopwatch.Reset();
    }


    if (tSGameGlobals.ptAchievementsInterface)
    {
        tSGameGlobals.ptAchievementsInterface->Process();
    }

    if (bLogThisUpdate)
    {
        flTime = tStopwatch.Check();




        tStopwatch.Reset();




        ActorsDumpList();
        TimersDumpList();




        BOOL bHalt = TRUE;
        if (bHalt)
        {





            DisplayInternalError(NULL, "", __LINE__, ERROR_INTERNAL_FATAL); ExitProgram(ERROR_INTERNAL_FATAL);
        }
    }


    ThreadGlobals::Get().m_bSimulatingGame = FALSE;

    SGame & tSGame = SGameManager::GameGet();
    if (!(tSGame.tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_UTILITY)) && simTimeThreshold)
    {




        Blizzard::Time::Millisecond tElapsed = Blizzard::Time::Milliseconds() - tSimStartTime;
        Blizzard::Time::Millisecond tLastTime = tSGameGlobals.tSimTimes[tSGameGlobals.tSimTimePos];

        tSGameGlobals.tSimTimes[tSGameGlobals.tSimTimePos++] = tElapsed;
        if (tSGameGlobals.tSimTimePos >= 512)
            tSGameGlobals.tSimTimePos = 0;



        tSGameGlobals.tSimTimeSum -= tLastTime;
        tSGameGlobals.tSimTimeSum += tElapsed;

        if (tLastTime >= simTimeThreshold)
            tSGameGlobals.tSimTimesSpikes--;

        if (tElapsed >= simTimeThreshold)
        {
            tSGameGlobals.tSimTimesSpikes++;


            int32 nUnsentPacketCount = ServerGetUnsentPacketCountTotal();
        }
    }
}





















BOOL SGameIsJoinAllowed(SGameID idSGame)
{
    SGame * ptSGame = SGameManager::GameTryToGet(idSGame);
    if (ptSGame == NULL)
        return FALSE;
    return (ptSGame->ptSGameGlobals->dwFlags & (1 << SGAMEGLOBALS_JOIN_DISALLOWED)) == 0;
}









GameCommonData & SGameGetGameCommonData()
{
    SGame & tSGame = SGameManager::GameGet();
    return tSGame.tGCD;
}







BOOL SGameIsValid()
{
    return SGameManager::TryToGetCurrentGame() != SGAMEID_INVALID;
}





SGameGlobals & SGameGlobalsGet()
{
    SGame & tSGame = SGameManager::GameGet();
    return *tSGame.ptSGameGlobals;
}









BOOL SGameTraceLootEnabled()
{
    SGameGlobals & tGlobals = SGameGlobalsGet();
    return (tGlobals.dwFlags & (1 << SGAMEGLOBALS_TRACE_LOOT)) != 0;
}















void SGameFinishPlayerSetup(Player & tNewPlayer)
{

    tNewPlayer.bDidFirstTimeSetup = TRUE;

    ACDID idACDNewPlayer = tNewPlayer.idPlayerACD;
    ActorCommonData & tACDNewPlayer = ACDGet(idACDNewPlayer);

    SHirelingSendStableInfoToClient(tNewPlayer);




    SPlayerGiveSharedMapVision(tNewPlayer);

    ScriptEventOnPlayerEnteredGame(tACDNewPlayer.id);

    if (GameIsPvP())
    {
        PVPOnPlayerEnteredGame(&tNewPlayer);
    }

















    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        uint32 dwPlayerBoostOptionBits = (1 << PLAYER_BOOST_PICKUP_ITEMS_BIT) |
                                         (1 << PLAYER_BOOST_NOTIFY_CLIENT_BIT) |
                                         (1 << PLAYER_BOOST_REMOVE_EQUIPMENT_FIRST_BIT) |
                                         (1 << PLAYER_BOOST_SHOW_LEVEL_UP_UI_BIT);


        ActorBoostToLevel(tACDNewPlayer, 70, "Blizzcon", "", dwPlayerBoostOptionBits);
    }






    if ((tNewPlayer.tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_HARDCORE)) && (tNewPlayer.tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_DECEASED)))
    {


        ActorID idActor = (ActorID)ACDGetOwner(idACDNewPlayer);
        BrainAPIDoDeath(idActor, ACTORID_INVALID, TRUE, TRUE, TRUE, NULL, NULL);
    }


    if (tACDNewPlayer.GetLocationType() == AL_WORLD)
    {
        SActTransitionOptInsUpdated();
    }



    PlayerList listDestPlayer(PlayerListGetElementPool());
    PlayerListGetSingle(listDestPlayer, tNewPlayer);

    SimpleMessage tMessage;
    MessageSendToClient(listDestPlayer, SMSG_FINISHED_GAME_SETUP_DONE, &tMessage, sizeof(tMessage));

}










































































































































































































































































































































void SGamePlayerJoin(
    Player & tNewPlayer,
    const OnlineService::PlayerResolveData & tResolvedPlayer,
    XLocale eLocale,
    SWorldID idSWorld)
{
    const D3::Hero::SavedDefinition & tHeroDefinition = tResolvedPlayer.tHeroDefinition;
    const D3::Account::SavedDefinition & tAccountDefinition = tResolvedPlayer.tAccountDefinition;
    const D3::Items::Mails & tMails = tResolvedPlayer.tMails;




    Blizzard::String::Copy(tNewPlayer.usHeroName, "Default", sizeof(tNewPlayer.usHeroName));
    Blizzard::String::Copy(tNewPlayer.szHeroFilename, "Default", sizeof(tNewPlayer.szHeroFilename));
    if (tHeroDefinition.has_digest())
    {
        Blizzard::String::Copy(tNewPlayer.usHeroName, tHeroDefinition.digest().hero_name().c_str(), sizeof(tNewPlayer.usHeroName));
    }

    tNewPlayer.eJoinTeam = tResolvedPlayer.eTeam;




    tNewPlayer.uHeroId = tHeroDefinition.digest().hero_id().id_low();

    tNewPlayer.tAccount.tHandle = tResolvedPlayer.tAccountId;
    tNewPlayer.tAccount.uBnetAccountId = tResolvedPlayer.uBnetAccountId;
    Blizzard::String::Copy(tNewPlayer.tAccount.usName, tResolvedPlayer.szAccountName, sizeof(tNewPlayer.tAccount.usName));
    tNewPlayer.eLocale = eLocale;


    tNewPlayer.bIsPrimaryPlayerServerOnly = tResolvedPlayer.bIsPrimary;
    tNewPlayer.bIsGuestPlayerServerOnly = tResolvedPlayer.bIsGuest;
    tNewPlayer.bIsLocalToServerServerOnly = tResolvedPlayer.bIsLocal;
    SPlayerSetConsoleLicenseFlags(tNewPlayer, tResolvedPlayer.uConsoleLicenses);
    SPlayerSetHasFriends(tNewPlayer, FALSE);


    tNewPlayer.nTeamId = tResolvedPlayer.nTeamId;
    tNewPlayer.teamRating = tResolvedPlayer.teamRating;
    tNewPlayer.teamVariance = tResolvedPlayer.teamVariance;
    tNewPlayer.individualRating = tResolvedPlayer.individualRating;
    tNewPlayer.individualVariance = tResolvedPlayer.individualVariance;
    tNewPlayer.bCompletedSoloRift = tAccountDefinition.digest().completed_solo_rift();

    if (GameIsWeeklyChallenge())
    {

        for (size_t i = 0; i < ChallengeRift::CRPB_COUNT; ++i)
        {
            tNewPlayer.nChallengeBestTimesMilliseconds[i] = 0;
        }

        tNewPlayer.nLastRewardChallengeNumber = tResolvedPlayer.uLastChallengeRewardEarned;
        for (const auto & entry : tResolvedPlayer.tChallengeRiftPersonalBests)
        {
            ChallengeRift::ChallengeRiftPersonalBest best = ChallengeRift::LeaderboardIdToChallengeRiftPersonalBest(entry.first);
            if (best == ChallengeRift::CRPB_COUNT)
            {
                continue;
            }


            Blizzard::Time::Timestamp ts = LeaderboardGetRiftScoreTimeToComplete(entry.second);
            tNewPlayer.nChallengeBestTimesMilliseconds[best] = uint32(ts / TimeConstants::NANOSECONDS_IN_A_MILLISECOND);
        }
    }

    if (!GameTypeIsPvP() || !GameIsBattleNetServer())
    {
        tNewPlayer.eTeam = STeamGetTeamForPlayer(tNewPlayer);
    }
    else
    {
        tNewPlayer.eTeam = tResolvedPlayer.eTeam;
    }



    SGameSetCreatorInfo(tNewPlayer.uHeroId, PlayerGetHeroName(tNewPlayer), PlayerGetAccountName(tNewPlayer));


    SGameUpdateMonsterLevel(tHeroDefinition.digest().level(), FALSE);

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();



    if (!GameIsBattleNetServer() && tSGameGlobals.ptAchievementsInterface)
    {
        OnlineService::ErrorCode errorCode = static_cast<Console::ServerAchievementsInterface *>(tSGameGlobals.ptAchievementsInterface)->RegisterPlayer(tNewPlayer.tAccount.tHandle, tAccountDefinition);
    }




    PlayerList listNewPlayer(PlayerListGetElementPool());
    PlayerListGetSingle(listNewPlayer, tNewPlayer);


    uint32 dwNow = GameGetServerTime();
    tNewPlayer.dwJoinTime = dwNow;

    BOOL bFirstPlayerForConnection = ServerGetPlayerCountForGameConnectionID(tNewPlayer.idGameConnection) == 1;
    if (bFirstPlayerForConnection)
    {

        GameSetupMessage tGameSetupMsg;
        tGameSetupMsg.dwFirstHeartbeat = dwNow;
        MessageSendToClient(listNewPlayer, SMSG_GAME_SETUP, &tGameSetupMsg, sizeof(tGameSetupMsg));

        ServerConnectionEnteredGame(tNewPlayer.idGameConnection);
    }

    SHero::Load(tNewPlayer, tHeroDefinition, tAccountDefinition);










































    QuestsVersionForPlayer(tNewPlayer.tHeroStateData);


    Act eAct = GameGetCurrentAct();

    SPlayerRegisterForExperienceId(tNewPlayer);
    SPlayerSetLicensesAndSessionFlags(tNewPlayer, tResolvedPlayer.tContentLicenses, tResolvedPlayer.tRawLicenses, tResolvedPlayer.uSessionFlags);

    SQuestsPlayerJoinedGame(tNewPlayer);

    if (GameIsOpenWorld())
    {

        if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        {
            tNewPlayer.bPlayOpenWorldTutorial = TRUE;
        }
    }

    SNO snoQuest = SNO_NONE;
    int32 nStepUID;
    SQuestsGetCurrent(snoQuest, nStepUID);

    SGameStatsPlayerTimedEventStart(&tNewPlayer, STAT_COUNTER_PLAYER_TIME_SPENT_FROM_START_QUEST, snoQuest, SGameGetStatDataKey(), TRUE);

    uint32 uEntranceGUID = (uint32)-1;
    GameType eGameType = GameGetGameType();
    BOOL fAllowStartLocationOverride = TRUE;


    if (eGameType == GT_DIABLO3)
    {
        SNO snoWorld;











        ResolvedPortalDestination tDest;
        ActGetCurrentStartLocation(tDest);

        snoWorld = tDest.snoWorld;
        uEntranceGUID = tDest.uEntranceGUID;

        if (tSGameGlobals.uEntranceGUIDOverride != (uint32)-1)
        {
            uEntranceGUID = tSGameGlobals.uEntranceGUIDOverride;
        }

        if (idSWorld == SWORLDID_INVALID)
        {
            idSWorld = WorldCommonFindBySNO(snoWorld, TRUE);
            if (idSWorld == SWORLDID_INVALID)
            {
                idSWorld = SWorldCreate(snoWorld, TRUE, SPlayerIsLocal(tNewPlayer), NULL, ACDID_INVALID, ACDID_INVALID, uEntranceGUID, WARP_UNKNOWN, FALSE, 0);
            }
        }
    }

    else if (GameTypeIsPvP())
    {
        fAllowStartLocationOverride = FALSE;
        if (idSWorld == SWORLDID_INVALID)
        {
            idSWorld = SWorldFindBySNO(tSGameGlobals.snoInitialWorld);


            if (idSWorld == SWORLDID_INVALID)
            {
                idSWorld = SWorldCreate(tSGameGlobals.snoInitialWorld, FALSE, FALSE, NULL, ACDID_INVALID, ACDID_INVALID, 0, WARP_UNKNOWN, FALSE, 0);
                ScriptEventOnUserEvent("PVPGameStart", ACDID_INVALID, 0);
            }
        }
    }
    else
    {
        if (idSWorld == SWORLDID_INVALID)
        {
            idSWorld = SWorldCreate(tSGameGlobals.snoInitialWorld, FALSE, FALSE, NULL, ACDID_INVALID, ACDID_INVALID, 0, WARP_UNKNOWN, FALSE, 0);
        }
    }

    if (!GameIsStartUp())
    {
        SPlayerEnterAct(tNewPlayer, eAct, TRUE);
    }

    ACDID idACDNewPlayer = SWorldPlayerEnter(idSWorld, ACDID_INVALID, &tNewPlayer, uEntranceGUID, fAllowStartLocationOverride, tHeroDefinition, tAccountDefinition, tMails, NULL);









    ActorCommonData & tACDNewPlayer = ACDGet(idACDNewPlayer);
    int32 nNewPlayerLevel = tACDNewPlayer.AttributesGetInt(ATTRIBUTE_LEVEL);


    SPlayerRecalculateUpscalingLevels();





    if (HeroSkillsAreOutOfDate(tACDNewPlayer.hGB.gbid, tHeroDefinition.saved_data()))
    {
        PowerResetSkills(tACDNewPlayer, TRUE);
    }



    {
        for (int i = 0; i < NUM_SKILL_SLOTS; ++i)
        {
            SNO snoSkill = tNewPlayer.tHeroStateData.tSavedData.tActiveSkills[i].snoSkill;
            if (snoSkill != SNO_NONE)
            {
                if (!PowerActorMeetsActiveSkillMasteryRequirement(tACDNewPlayer, AutoPowerDefinitionR(snoSkill)))
                {

                    SPlayerSwitchActiveSkills(tNewPlayer, SNO_NONE, RUNE_TYPE_INVALID, i, FALSE);
                }
                else
                {
                    RuneType eRuneType = tNewPlayer.tHeroStateData.tSavedData.tActiveSkills[i].eRuneType;
                    if (!PowerActorMeetsSkillRuneRequirement(tACDNewPlayer, snoSkill, eRuneType))
                    {

                        SPlayerSwitchActiveSkills(tNewPlayer, snoSkill, RUNE_TYPE_INVALID, i, FALSE);
                    }
                }

                SPlayerRegisterPowerForRecharge(&tNewPlayer, snoSkill);
            }
        }


        for (int i = 0; i < NUM_TRAIT_SLOTS; ++i)
        {
            SNO snoSkill = tNewPlayer.tHeroStateData.tSavedData.snoTraits[i];
            if (snoSkill != SNO_NONE && !PowerActorMeetsTraitMasteryRequirement(tACDNewPlayer, snoSkill))
            {

                SPlayerRemoveTraitInSlot(tNewPlayer, i);
            }
        }


        for (int32 i = 0; i < LPST_NUM_TYPES; ++i)
        {
            GBID gbidLegendary = tNewPlayer.tHeroStateData.tSavedData.gbidLegendaryPowers[i];
            if (gbidLegendary != GBID_NONE && !ItemPlayerCanAssignLegendaryPower(tNewPlayer, gbidLegendary, i))
            {

                SPlayerRemoveLegendaryPowerInSlot(tNewPlayer, i);
            }
        }
    }

    if (GameIsWeeklyChallenge())
    {
        ChallengeRift::SnapshotPlayerSkills(tNewPlayer);
    }


    if (tNewPlayer.tHeroStateData.tSavedData.uSkillKitVersion != gptGroupDefSkillKit->GetCurrentVersion(SIGMAPLATFORM_UNKNOWN) && nNewPlayerLevel != 1)
    {


        SimpleMessage tMessage;
        MessageSendToClient(listNewPlayer, SMSG_NEW_SKILLKIT, &tMessage, sizeof(tMessage));
    }




    if (bFirstPlayerForConnection)
    {

        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {

            if (ptPlayer->idGameConnection != GAMECONNECTIONID_INVALID)
            {
                SPlayerSendNewPlayerMessage(listNewPlayer, *ptPlayer, FALSE);
            }
        }
    }
    else
    {

        SPlayerSendNewPlayerMessage(listNewPlayer, tNewPlayer, FALSE);
    }



    ActorForceCleanupItemsOnGround(tNewPlayer.nPlayerIndex);




    PlayerList listExistingPlayers(PlayerListGetElementPool());

    PlayerListGetAllConnectedExceptConnection(listExistingPlayers, tNewPlayer.idGameConnection, FALSE);




    SPlayerSendNewPlayerMessage(listExistingPlayers, tNewPlayer, TRUE);


    if (NephalemRiftManager::IsNonTieredRiftActive())
    {
        NephalemRiftManager::PlayerSetPaidForRift(&tNewPlayer, TRUE, TRUE, FALSE);
    }


    SPlayerSendKickTimers(tNewPlayer);


    {
        HandicapMessage tMessage;
        tMessage.nHandicapLevel = GameGetHandicapLevel();
        MessageSendToClient(listNewPlayer, SMSG_CURRENT_HANDICAP_LEVEL, &tMessage, sizeof(tMessage));
    }


    SGame & tSGame = SGameManager::GameGet();
    if (tSGame.ptSGameGlobals && (tSGame.tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_PUBLIC_BIT)))
    {


        OnlineService::OpenGameToPublicMessage tMessage;
        Blizzard::String::Copy(tMessage.szGameTag, tSGame.ptSGameGlobals->tMatchmakingData.szGameTag, sizeof(tMessage.szGameTag));
        tMessage.bUserInitiated = FALSE;
        MessageSendToClient(listNewPlayer, SMSG_GAME_PROMOTED_TO_PUBLIC, &tMessage, sizeof(tMessage));
    }


    SItemCrafting_InitSavedData(tNewPlayer);
    for (int i = 0; i < CRAFTER_TYPE_COUNT; ++i)
    {
        SItemCrafting_SendPlayerInfo(tNewPlayer, i, TRUE);
    }
    SItemCrafting_SendPlayerTransmogInfo(tNewPlayer, TRUE);

    SCosmeticItems_SendPlayerCosmeticItemInfo(tNewPlayer);

    PlayerSendTutorialStashIconsStateToClient(&tNewPlayer, listNewPlayer);
    PlayerSendHeroStateCurrencyToClient(tNewPlayer, FALSE);
    PlayerSendHeroStateExtractedLegendariesToClient(tNewPlayer);
    PlayerSendHeroStateAllLoadoutsToClient(tNewPlayer);
    PlayerAskClientToLoadLegendaryPowerIcons(tNewPlayer);



    if (tNewPlayer.uHeroId == tSGameGlobals.uHeroIdCreator)
    {
        tSGameGlobals.dwFlags |= (1 << SGAMEGLOBALS_GAME_CREATOR_JOINED_BIT);
    }














    tNewPlayer.tHeroStateData.tSavedData.tLastSavePoint.Reset();

    if (tSGameGlobals.bResumeFromSave && tHeroDefinition.has_digest())
    {
        const D3::Hero::Digest & tDigest = tHeroDefinition.digest();
        Act eLastPlayedAct = tDigest.last_played_act();
        SNO snoLastPlayedQuest = tDigest.last_played_quest();
        int32 nLastPlayedStepUID = tDigest.last_played_quest_step();



        if (eAct == eLastPlayedAct &&
            tSGameGlobals.snoInitialQuest == snoLastPlayedQuest &&
            tSGameGlobals.nInitialQuestStepUID == nLastPlayedStepUID)
        {
            SavePointData tHeroSavePoint;
            tHeroSavePoint.Import(tHeroDefinition.saved_data().save_point());

            if (tHeroSavePoint.IsValid() && SNOFileExists(WORLD_SNO, tHeroSavePoint.snoWorld))
            {
                tNewPlayer.tHeroStateData.tSavedData.tLastSavePoint = tHeroSavePoint;


                SWorldID idSWorldSavePoint = SWorldFindBySNO(tHeroSavePoint.snoWorld);
                if (idSWorldSavePoint == SWORLDID_INVALID)
                {

                    SWorldCreate(tHeroSavePoint.snoWorld, FALSE, FALSE, sSavepointWorldCreated, tACDNewPlayer.id, ACDID_INVALID, 0, WARP_UNKNOWN, FALSE, 0);
                }
                else
                {
                    SWorld * ptSWorldStart = SWorldGet(idSWorldSavePoint);
                    if (SWorldIsCreated(*ptSWorldStart))
                    {


                        if (tNewPlayer.bIsPrimaryPlayerServerOnly)
                        {


                            GizmoSendSavePointInfoToPlayer(listNewPlayer, tHeroSavePoint);
                        }
                    }
                }
            }
        }
    }



    BOOL bNewGame = FALSE;
    uint32 uNow = GameGetServerTime();
    if (uNow < 3601)
    {
        bNewGame = TRUE;
    }

    SPlayerApplyLoadingBuff(tACDNewPlayer, tACDNewPlayer, bNewGame, TRUE);


    SPlayerEvaluateMultiplayerBuff();


    SPlayerRestoreBuffs(tNewPlayer);

    SGameSendDungeonFinderSeedToPlayer(tNewPlayer);
    SGameSendDungeonFinderParticipatingPlayerCountToPlayer(tNewPlayer);
    SGameSendDungeonFinderProgressToPlayer(tNewPlayer);
    SGameSendDungeonFinderTimedEventToPlayer(tNewPlayer);





    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (ptPlayer->nPlayerIndex != tNewPlayer.nPlayerIndex)
        {
            SPlayerTrickleAdd(tNewPlayer, ptPlayer->idPlayerACD);
        }
    }


    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        if (ptPlayer->nPlayerIndex != tNewPlayer.nPlayerIndex)
        {
            SPlayerTrickleAdd(*ptPlayer, idACDNewPlayer);
        }
    }



    STrickleTriggerUpdate();


    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer)
        {
            ptACDPlayer->MarkHeroStateLastWaypointLevelAreaChanged();
        }
    }


    GameCommonData & tGCD = GameGetActiveGameCommonData();
    SimpleMessage tMessage;
    if (tGCD.dwDifficultyChangedFlags & (1 << DIFFICULTY_RAISED_BIT))
    {
        MessageSendToClient(listNewPlayer, SMSG_DIFFICULTY_RAISED_FLAG, &tMessage, sizeof(tMessage));
    }
    if (tGCD.dwDifficultyChangedFlags & (1 << DIFFICULTY_LOWERED_BIT))
    {
        MessageSendToClient(listNewPlayer, SMSG_DIFFICULTY_LOWERED_FLAG, &tMessage, sizeof(tMessage));
    }

    SGameSyncData(&tNewPlayer);

    SGameStatsPlayerCounterIncrement(&tNewPlayer, STAT_COUNTER_PLAYER_JOINED_GAME, 0, 0, 1.0f);
    SGameStatsPlayerTimedEventStart(&tNewPlayer, STAT_COUNTER_PLAYER_TIME_SPENT_AT_LEVEL, tACDNewPlayer.AttributesGetInt(ATTRIBUTE_LEVEL), 0, TRUE);

    uint32 nPlayers = PlayerGetNumPlayers();
    if (nPlayers > SGameManager::GameGet().ptSGameStatGlobals->uMaxNumberOfPlayers)
    {
        SGameManager::GameGet().ptSGameStatGlobals->uMaxNumberOfPlayers = nPlayers;
    }

    PlayerClass ePlayerClass = PlayerGetPlayerClass(&tNewPlayer);

    ++SGameManager::GameGet().ptSGameStatGlobals->uTotalNumberOfPlayersByClass[ePlayerClass];





    {
        Achievement::EvaluationData tEvaluationData;

        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            tEvaluationData.SetPlayer(&tNewPlayer);



            tEvaluationData.EvaluateEvent(
                ptPlayer,
                Achievement::ACH_EVENT_BROADCAST_PLAYER_JOINED_GAME,
                PlayerGetPlayerClass(tNewPlayer.idPlayerACD),
                1, (uint32)sub_71007b5e70());


            if (tNewPlayer.nPlayerIndex != ptPlayer->nPlayerIndex)
            {
                tEvaluationData.SetPlayer(ptPlayer);

                tEvaluationData.EvaluateEvent(
                    &tNewPlayer,
                    Achievement::ACH_EVENT_JOINED_GAME_WITH_PLAYER,
                    PlayerGetPlayerClass(ptPlayer->idPlayerACD),
                    1, (uint32)sub_71007b5e70());
            }
        }

        tEvaluationData.SetPlayer(&tNewPlayer);







        tEvaluationData.EvaluateEvent(&tNewPlayer, Achievement::ACH_EVENT_PLAYER_LEAVE_GAME, 1, (uint32)sub_71007b5e70());





        tEvaluationData.EvaluateEvent(&tNewPlayer, Achievement::ACH_EVENT_PLAYER_JOINED_GAME, 1, (uint32)sub_71007b5e70());






        tEvaluationData.EvaluateEvent(&tNewPlayer, Achievement::ACH_EVENT_HAS_HERO_AT_LEVEL, nNewPlayerLevel, 1, (uint32)sub_71007b5e70());






        for (blz::vector<OnlineServiceServer::License>::const_iterator iter = tResolvedPlayer.tRawLicenses.begin(); iter != tResolvedPlayer.tRawLicenses.end(); ++iter)
        {


            OnlineServiceServer::LicenseId uLicenseID = iter->id;
            tEvaluationData.EvaluateEvent(&tNewPlayer, Achievement::ACH_EVENT_PLAYER_HAS_LICENSE, uLicenseID, 1, (uint32)sub_71007b5e70());
        }













    }

    Minimap::OnNewPlayer(tNewPlayer);

















    SPlayerRefreshWaypointDestination(tNewPlayer.idPlayerACD);

    if (!GameIsStartUp())
    {
        SPlayerPlayActInterstitial(tNewPlayer, GameGetCurrentAct(), FALSE);
    }

    tNewPlayer.dwTimeStampJoined = dwNow;














    XNetAddress tPlayerAddress;
    ServerGetPlayerAddress(tNewPlayer, tPlayerAddress);

    Blizzard::Time::TimeRec tr;
    Blizzard::Time::BreakTime(Blizzard::Time::FromUnixTime(tHeroDefinition.digest().last_played_time()), tr);


    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
    {
        if (Console::Online::PartyIsHost())
        {
            int32 hostIndex = Console::Online::PartyGetHostIndex();
            Player * ptPlayerDest = PlayerGetByPlayerIndex(hostIndex);


            if (ptPlayerDest->idPlayerACD != tNewPlayer.idPlayerACD)
            {
                ActorCommonData * acd = ACDTryToGet(tNewPlayer.idPlayerACD);
                if (!acd || !ACDIsInTieredLootRun(*acd))
                {







                    ActorCommonData * hostACD = ACDTryToGet(ptPlayerDest->idPlayerACD);
                    if (hostACD && ACDIsInTieredLootRun(*hostACD))
                    {
                        SPlayerWarpToOtherPlayer(&tNewPlayer, ptPlayerDest);
                    }
                    else
                    {


                        bool bAnyRiftActive = NephalemRift::IsTieredRiftActive();
                        if (bAnyRiftActive)
                        {
                            NephalemRiftManager::PlayerManuallyPortIntoRift(&tNewPlayer);
                        }
                    }
                }
            }
        }
    }

































    TraceStat("gamejoin", idACDNewPlayer, "");
}




int32 SGamePlayerConnected(GameConnectionID idGameConnection, int32 nControllerIndex, int32 nConsolePartyIndex)
{





    int32 nOldNumPlayers = PlayerGetNumPlayersForDifficulty();




    BOOL bFirstPlayerForConnection = ServerGetPlayerCountForGameConnectionID(idGameConnection) == 0;



    int32 nNewPlayerIndex = SPlayerCreate(idGameConnection, nConsolePartyIndex);


    Player & tNewPlayer = *PlayerGetByPlayerIndex(nNewPlayerIndex);



    tNewPlayer.uHeroId = idGameConnection;








    tNewPlayer.nControllerIndex = nControllerIndex;
    tNewPlayer.uConsoleSyncFlags = 0;
    tNewPlayer.tConsoleCounters.uMonstersKilled = 0;
    tNewPlayer.tConsoleCounters.uElitesKilled = 0;
    tNewPlayer.tConsoleCounters.uGoldPickedUp = 0;



    PlayerList listNewPlayer(PlayerListGetElementPool());
    PlayerListGetSingle(listNewPlayer, tNewPlayer);


    ConnectionEstablishedMessage tConnectionMsg;
    tConnectionMsg.nPlayerIndex = nNewPlayerIndex;
    tConnectionMsg.dwAnimSyncedSeed = RandGetSeed(SGameGlobalsGet().rngSyncedAnim);
    tConnectionMsg.dwSnoCoreDataHash = SNOGetCoreDataHash();

    MessageSendToClient(listNewPlayer, bFirstPlayerForConnection ? SMSG_CONNECTION_ESTABLISHED : SMSG_CONNECTION_ADD_PLAYER, &tConnectionMsg, sizeof(tConnectionMsg));

    int32 nNewNumPlayers = PlayerGetNumPlayersForDifficulty();
    ActorsAdjustForPlayerChanged(nOldNumPlayers, nNewNumPlayers);

    return nNewPlayerIndex;
}




BOOL SGameIsShuttingDown()
{
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    return (tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_SHUTTING_DOWN)) != 0;
}
























































































































void SQuestsLoadFromStartData(); void SQuestsLoadSavedQuestObjectives(const D3::Hero::SavedDefinition & tHeroDefinition); void SQuestsGetDifficultySnapshot(const D3::Hero::SavedDefinition & tHeroDefinition); void SQuestsSetGameFlagsForCompletedQuests();  // X/Server/Quests/SQuests.cpp:5026, :4974, :5002, :502
void SPlayerInitGameWaypointsFromPlayer(const D3::Hero::SavedDefinition & tHeroDefinition); void SPlayerInitGameSavePointFromPlayer(const D3::Hero::SavedDefinition & tHeroDefinition);  // X/Server/SPlayer.cpp:7215, :7236

void SGameResolveWorldState(
	const GameParams & tParams,
	const D3::Hero::SavedDefinition & tHeroDefinition)
{








	SQuestsLoadFromStartData();

	if (tParams.bResumeFromSave)
	{

		SQuestsLoadSavedQuestObjectives(tHeroDefinition);


		SPlayerInitGameWaypointsFromPlayer(tHeroDefinition);


		SPlayerInitGameSavePointFromPlayer(tHeroDefinition);


		SQuestsGetDifficultySnapshot(tHeroDefinition);
	}


	SGameInitWaypointsFromStartData(FALSE, FALSE);


	SQuestsSetGameFlagsForCompletedQuests();
}


// NON_MATCHING: in the bUpdateClients copy of the loop (the loop is unswitched) ours keeps one 64-bit counter where the original keeps i + 1 in its own register, as both do in the other copy; lines and columns follow the original line table
void SGameInitWaypointsFromStartData(
    BOOL bReset,
    BOOL bUpdateClients)
{
    if (GameIsOpenWorld())
    {
        return;
    }

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();

    if (bReset)
    {
        tSGameGlobals.dwActivatedWaypoints = 0;
    }

    const CachedWaypointArray& tWaypoints = ActGetCachedWaypointArray();
    size_t nCount = tWaypoints.size();























    for (uint32 i = 0; i < nCount; i++)
    {


        if (!(tSGameGlobals.dwActivatedWaypoints & (1 << i)))
        {
            const WaypointInfo & tInfo = tWaypoints[i].tInfo;
            if (tInfo.snoQuestRange != SNO_NONE &&
                QuestRangeEvaluate(tInfo.snoQuestRange, NULL))
            {
                if (bUpdateClients)
                {
                    SPlayerActivateWaypoint(i, TRUE);
                    Minimap::ActivateWaypointMarker(tInfo.snoLevelArea);
                }
                else
                {
                    tSGameGlobals.WaypointEnable(i, TRUE);
                }
            }
        }
    }
}



void SGameInitAnniversaryEventFlags()
{
    if (!GameIsOpenWorld())
        return;

    {
        AnniversaryEventStatus eStatus = (AnniversaryEventStatus)s_varAnniversaryEventStatus.Get();
        if (eStatus == ANNIVERSARY_INVALID)
            eStatus = GameGetAnniversaryEventStatus();

        if (eStatus == ANNIVERSARY_PRECURSOR_START)
            GameFlagSet("RetroAnniversaryEvent_Precursor", TRUE);
        else
            GameFlagSet("RetroAnniversaryEvent_Precursor", FALSE);

        if (eStatus == ANNIVERSARY_EVENT_START)
        {
            GameFlagSet("RetroAnniversaryEvent_Dungeon", TRUE);
            GameGetActiveGameCommonData().tGameSyncedData.dwGameSyncedFlags |= (1 << GAME_SYNCED_FLAGS_ANNIVERSARY_DUNGEON);
        }
        else
        {
            GameFlagSet("RetroAnniversaryEvent_Dungeon", FALSE);
            GameGetActiveGameCommonData().tGameSyncedData.dwGameSyncedFlags &= ~(1 << GAME_SYNCED_FLAGS_ANNIVERSARY_DUNGEON);
        }
    }
}






#include "Network/XServerSocket.h"
void ServerSendOutOfGameMessage(ServerSocketID idServerSocket, void * pMessage, GameMessageType eType, uint32 dwSize); BOOL XServerSocketIDValid(ServerSocketID idServerSocket);  // X/Server/Network/Server.cpp:917, XServerSocket.cpp:198
#include "../Common/OnlineService/OnlineServiceLogStream.h"
BOOL SGameValidateJoinRequest(
    ServerSocketID idServerSocket,
    SGameID idSGame,
    OnlineService::HeroId uHeroId,
    uint64 uAuthToken,
    XLocale eLocale)
{
    SGame * ptSGame = SGameManager::GameTryToGet(idSGame);
    if (ptSGame == NULL ||
        !SGameIsJoinAllowed(idSGame))
    {
        ONLINE_SERVICE_LOG(SLVL_INFO, 3, OUTPUTSTREAM_DEFAULT, OnlineService::GameIDLabel(idSGame) << " Invalid game or can't join a game idSGame=" << idSGame << " IsJoinAllowed=" << SGameIsJoinAllowed(idSGame));

        QuitGameMessage tQuitMessage;
        tQuitMessage.eReason = QUIT_REASON_JOIN_FAILED;
        ServerSendOutOfGameMessage(idServerSocket, &tQuitMessage, SMSG_QUIT, sizeof(tQuitMessage));

        XServerSocketDisconnect(idServerSocket, SDR_INVALID_GAME);
        return FALSE;
    }


    OnlineService::PlayerResolveData tResolvedPlayer;
    BOOL bFound = ptSGame->tPendingJoinMap.Lookup(uHeroId, tResolvedPlayer);
    if (!bFound || tResolvedPlayer.uAuthToken != uAuthToken)
    {
        ONLINE_SERVICE_LOG(SLVL_INFO, 3, OUTPUTSTREAM_DEFAULT, OnlineService::GameIDLabel(idSGame) << " Unknown player or bad authToken idSGame=" << idSGame << " heroId=" << uHeroId << " authToken=" << uAuthToken << " bFound=" << bFound << " foundAuthToken=" << (bFound ? tResolvedPlayer.uAuthToken : 0));

        BOOL bReturnToClient = GameServerCodeEnter_Track() || _GameServerCodeEnter(idSGame);
        SGameBootPendingPlayer(uHeroId, QUIT_REASON_JOIN_FAILED, NULL, idServerSocket);
        GameServerCodeLeave_Tracked(bReturnToClient);
        return FALSE;
    }





    XServerSocketSetSGameID(idServerSocket, ptSGame->id);
    XServerSocketSetAccountId(idServerSocket, tResolvedPlayer.tAccountId);
    XServerSocketClearMsgPermissions(idServerSocket, 1 << MSG_PERMISSIONS_PREGAME_BIT);

    ptSGame->tPendingJoinMap.SetPlayerConnected(uHeroId, idServerSocket, eLocale);






















    return TRUE;
}
BOOL ReplayPlayerJoin(const OnlineService::PlayerResolveData & tResolvedPlayer); void SQuestsGetCurrent(SNO & snoQuest, int32 & nStepUID); GameConnectionID ServerClientConnect(ServerSocketID idServerSocket, int32 nControllerIndex, int32 nConsolePartyIndex); int32 ServerGetPlayerIndexFromGameConnectionID(GameConnectionID idGameConnection, int32 nControllerIndex);  // X/Server/Replay.cpp:1925, Quests/SQuests.cpp:3469, Network/Server.cpp:562, :2134
void SGamePlayerJoin(Player & tNewPlayer, const OnlineService::PlayerResolveData & tResolvedPlayer, XLocale eLocale, SWorldID idSWorld); uint32 XServerSocketSetMsgPermissions(ServerSocketID idServerSocket, uint32 dwNewPermissionsMask); BOOL ACDWarpToACD(ActorCommonData & tACDToWarp, const ActorCommonData & tACDTarget, const PlayerWarpReason eReason, float flMinWarpRadius); PlayerAvengerData * GetPlayerAvengerData(Player * ptPlayer, AvengerSource eAvengerSource);  // X/Server/SGame.cpp:4637, Network/XServerSocket.cpp:1367, AI/ActorAI.cpp:1802, X/Common/Player.cpp:1634
void SItemCrafting_InitSavedData(Player &);  // X/Server/Crafting/SItemCrafting.cpp:3010
SNO SGameGetStartWorld();  // :7569
BOOL SGameJoinPendingPlayer(const OnlineService::HeroId uHeroId)

{

    OnlineService::PlayerResolveData tResolvedPlayer;
    SGame & tSGame = SGameManager::GameGet();
    BOOL bFound = tSGame.tPendingJoinMap.Lookup(uHeroId, tResolvedPlayer);
    if (!bFound)
    {
        ONLINE_SERVICE_LOG(SLVL_INFO, 3, OUTPUTSTREAM_DEFAULT, OnlineService::GameIDLabel(tSGame.id) << " Unknown player for idSGame=" << tSGame.id << " heroId=" << uHeroId);
        SGameBootPendingPlayer(uHeroId, QUIT_REASON_JOIN_FAILED, NULL, SERVERSOCKETID_INVALID);
        return FALSE;
    }


    if (GameIsNetworked() &&
        !ReplayIsReplayGame())
    {
        if (!XServerSocketIDValid(tResolvedPlayer.idServerSocket))
        {
            SGameBootPendingPlayer(tResolvedPlayer.uHeroId, QUIT_REASON_COUNT, NULL, tResolvedPlayer.idServerSocket);
            return FALSE;
        }
    }


    GameType eGameType = GameGetGameType();
    if (eGameType == GT_DIABLO3)
    {
        uint32 uNumPlayers = PlayersGetCount();
        if (uNumPlayers >= 5)
        {
            SGameBootPendingPlayer(uHeroId, QUIT_REASON_JOIN_FAILED, NULL, tResolvedPlayer.idServerSocket);
            return FALSE;
        }
    }


    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    if (tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_GAME_PAUSED_BIT))
    {
        SGamePause(FALSE);
    }

    if (tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_GAME_SOFT_PAUSED_BIT))
    {
        SGameSoftPause(FALSE);
    }

    if (ReplayIsRecordingGame() && ReplayPlayerJoin(tResolvedPlayer))
    {
        tSGame.tPendingJoinMap.SetPlayerReplayJoinRecorded(uHeroId, TRUE);
    }

    SNO snoWorldStart = SGameGetStartWorld();
    if (snoWorldStart == SNO_NONE || !SNOFileExists(WORLD_SNO, snoWorldStart))
    {
        SGameBootPendingPlayer(uHeroId, QUIT_REASON_JOIN_FAILED, NULL, tResolvedPlayer.idServerSocket);
        return FALSE;
    }







    SNO snoQuest;
    int32 nStepUID;
    SQuestsGetCurrent(snoQuest, nStepUID);























    SWorldID idSWorldStart = WorldCommonFindBySNO(snoWorldStart, TRUE);

    if (idSWorldStart == SWORLDID_INVALID)
    {
        idSWorldStart = SWorldCreate(snoWorldStart, FALSE, FALSE, sStartingWorldCreated, ACDID_INVALID, ACDID_INVALID, 0, 0, FALSE, 0);
    }

    SWorld * ptSWorldStart = SWorldGet(idSWorldStart);
    if (!SWorldIsCreated(*ptSWorldStart))
    {
        tSGame.tPendingJoinMap.SetPlayerWaitingOnWorldCreation(uHeroId, TRUE);
        return TRUE;
    }






    int32 nControllerIndex = tResolvedPlayer.nControllerIndex;
    int32 nConsolePartyIndex = tResolvedPlayer.nPartyIndex;




    GameConnectionID idGameConnection = ServerClientConnect(tResolvedPlayer.idServerSocket, nControllerIndex, nConsolePartyIndex);

    int32 nPlayerIndex = ServerGetPlayerIndexFromGameConnectionID(idGameConnection, nControllerIndex);

    Player & tPlayer = *PlayerGetByPlayerIndex(nPlayerIndex);



    SGamePlayerJoin(tPlayer, tResolvedPlayer, tResolvedPlayer.eLocale, idSWorldStart);

    tSGameGlobals.nMaxPlayers = Math::Max(tSGameGlobals.nMaxPlayers, PlayerGetNumPlayers());


    SItemCrafting_InitSavedData(tPlayer);

    XServerSocketSetMsgPermissions(tResolvedPlayer.idServerSocket, 1 << MSG_PERMISSIONS_INGAME_BIT);


    for (Player * ptExisting = PlayerGetFirstInGame(); ptExisting != NULL; ptExisting = PlayerGetNextInGame(ptExisting))
    {
        if (ptExisting->nPlayerIndex != tPlayer.nPlayerIndex &&
            PlayersAreLocalToEachOther(*ptExisting, tPlayer) &&
            ptExisting->bIsPrimaryPlayerServerOnly)
        {
            ActorCommonData & tACDPlayer = ACDGet(tPlayer.idPlayerACD);
            ActorCommonData & tACDExistingPlayer = ACDGet(ptExisting->idPlayerACD);

            ACDWarpToACD(tACDPlayer, tACDExistingPlayer, WARP_CONSOLE_PORTAL, 80.0f);

            tPlayer.dwBossEncounterFlags = ptExisting->dwBossEncounterFlags;
            tACDPlayer.AttributesSetInt(ATTRIBUTE_IN_BOSS_ENCOUNTER, tACDExistingPlayer.AttributesGetInt(ATTRIBUTE_IN_BOSS_ENCOUNTER));

            SNO snoBossLevelArea = ACDGetLevelArea(tACDPlayer);
            PlayerAvengerData * ptPlayerAvengerData = GetPlayerAvengerData(&tPlayer, tPlayer.eAvengerSource);
            ptPlayerAvengerData->snoBossLevelArea = snoBossLevelArea;
            break;
        }
    }



    if (tResolvedPlayer.uJoinFlags & (1 << OnlineService::PLAYER_JOIN_ACHIEVEMENT_REGISTRATION_FAILED))
    {
        BoolDataMessage tMessage;
        tMessage.bVal = FALSE;
        SPlayerQueueFutureAchievementRegistrationAttempt(&tPlayer);

        PlayerList listDestPlayers(PlayerListGetElementPool());
        listDestPlayers.AddTail_Tracked(&tPlayer);
        MessageSendToClient(listDestPlayers, SMSG_ACHIEVEMENTS_AVAILABILITY, &tMessage, sizeof(tMessage));
    }
    else
    {
        SPlayerHealAchievementPointLeaderboard(tPlayer);
    }







    tSGame.tPendingJoinMap.RemoveAt(uHeroId);

















    return TRUE;
}

bool sub_71007b7080()  // 0x71007b7080 (unnamed): tStartParams.dwCreationFlags bit 20 (2.7.7 GAMECREATEPARAM bit, name unknown)
{
    return (SGameManager::GameGet().tStartParams.dwCreationFlags >> 20) & 1;
}



























































































































int32 SGameGetMaxPlayers()
{
    return 4;
}







RNG & SGameGetRNG()
{
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    return tSGameGlobals.rng;
}






































static void sRunServerIdle(
    SGame * ptSGame,
    uint32 dwGameUpdateCount)
{


    GameIsBattleNetServer();

































    sServerIdle(dwGameUpdateCount);
}






// NON_MATCHING: register allocation only (71/952 insns): the original keeps tSGameGlobals in x19 and bReturnToClient in w20, ours has them swapped
void SGameUpdateGame(
    SGame * ptSGame,
    uint32 dwGameUpdateCount)
{




    {
        BOOL bReturnToClient = GameServerCodeEnter_Track() || _GameServerCodeEnter(ptSGame->id);

        sRunServerIdle(ptSGame, dwGameUpdateCount);








        GameServerCodeLeave_Tracked(bReturnToClient);
    }


    SigmaProfNewFrame(NULL);

}




void AppServerIdle(uint32 dwGameClockTicks)
{
    uint32 dwGameFrameUpdateCount = sUpdateSGameClock(dwGameClockTicks);
    bool bGameManagerShouldUpdate = dwGameFrameUpdateCount > 0;
    if (!bGameManagerShouldUpdate)
    {
        if (GameIsServerOnly())
            Blizzard::Process::Sleep(2);
        return;
    }

    SGameManager::QueueGameFrameUpdate(dwGameFrameUpdateCount);
    ReplayTestUpdate(dwGameFrameUpdateCount);
    XServerReadUpdate();
    SNetLoungeUpdate();
    TelnetConsoleUpdate();
    ServerMonUpdate();
    SGameManager::UpdateGames(dwGameClockTicks);
}



















































SGameID AppServerGetFirstGame()
{
    SGameIterator tIt;
    if (SGameManager::GameIterate(tIt))
        return tIt->id;
    return SGAMEID_INVALID;
}



SGameID AppServerGetOnlyGame()
{
    return AppServerGetFirstGame();
}







uint32 SGameGetEventNumber()
{
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    uint32 dwGameEventNumber = tSGameGlobals.dwGameEventNumber++;
    return dwGameEventNumber;
}








void SGameSetCreatorInfo(
    OnlineService::HeroId uHeroId,
    const CHAR * uszHeroName,
    const CHAR * uszAccountName)
{
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();


    if (!tSGameGlobals.bGameCreatorInfoSet)
    {
        tSGameGlobals.uHeroIdCreator = uHeroId;
        Blizzard::String::Copy(tSGameGlobals.uszCreatorHeroName, uszHeroName, sizeof(tSGameGlobals.uszCreatorHeroName));
        Blizzard::String::Copy(tSGameGlobals.uszCreatorAccountName, uszAccountName, sizeof(tSGameGlobals.uszCreatorAccountName));
        tSGameGlobals.bGameCreatorInfoSet = TRUE;
    }
}






































void SGameSetCurrentAct(const Act eAct)
{

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    tGCD.tGameSyncedData.eAct = eAct;

    SNOFilesSetCurrentAct(eAct);


    ActCacheWaypoints(eAct);

    QuestsActivateForAct(eAct);

    SGameSyncData(NULL);
}




static void sSGameResetOpenWorld()
{

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();


    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        SNO snoHubWorld = SNO_NONE;
        uint32 uEntranceGUID;

        if (tSGameGlobals.snoOpenWorldHubWorld != SNO_NONE)
        {
            snoHubWorld = tSGameGlobals.snoOpenWorldHubWorld;
            uEntranceGUID = tSGameGlobals.uOpenWorldHubEntranceGUID;
        }
        else
        {
            ResolvedPortalDestination tDest;
            SGameGetOpenWorldHubWorldForPlayer(*ptPlayer, tDest);
            snoHubWorld = tDest.snoWorld;
            uEntranceGUID = tDest.uEntranceGUID;
        }

        ActorCommonData & tACDPlayer = ACDGet(ptPlayer->idPlayerACD);
        SWorldID idSWorldIDHubWorld = SWorldFindBySNO(snoHubWorld);

        if (tACDPlayer.GetSWorldID() != idSWorldIDHubWorld)
        {
            SGameACDChangeWorld(tACDPlayer.id, snoHubWorld, uEntranceGUID, 3, 0.0f, NULL);
        }
    }


    SWorldResetDungeonFinder();

    SGame & tSGame = SGameManager::GameGet();
    TDataArrayIterator<SWorld> tIt;
    while (tSGame.ptSWorlds->Iterate(tIt))
    {
        if (!(tIt->dwFlags & (1 << FLAG_SWORLD_IS_HUB)) && !SWorldIsFlaggedForDeletion(tIt->id))
        {
            SWorldRemovePlayersAndFlagForDeletion(tIt->id);
        }
    }


    ServerQueueClientUpdate();
    ServerUpdateClients(TRUE);

    SWorldsDeleteFlagged();
    LevelAreasDeleteFlagged();
}



BOOL SGameSetOpenWorldMode(Player * ptPlayerInitiating, OpenWorldMode eOpenWorldMode, Act eOpenWorldModeAct, int32 nOpenWorldModeParam, SNO snoHubWorld, uint32 hubEntranceGUID)
{







    GameCommonData & tGCD = GameGetActiveGameCommonData();
    OpenWorldMode eCurrentMode = tGCD.tGameSyncedData.eOpenWorldMode;

    if (eOpenWorldMode == 0)
    {
        if (eCurrentMode == 0)
        {
            return FALSE;
        }



        tGCD.tGameSyncedData.eOpenWorldMode = 0;
        tGCD.tGameSyncedData.eOpenWorldModeAct = ACT_INVALID;
        tGCD.tGameSyncedData.nOpenWorldModeParam = 0;

        SGameGlobals & tSGameGlobals = SGameGlobalsGet();
        tSGameGlobals.snoOpenWorldHubWorld = SNO_NONE;
        tSGameGlobals.uOpenWorldHubEntranceGUID = 0;

        SGameSyncData(NULL);
        return TRUE;
    }


    if (eCurrentMode != 0)
    {
        return FALSE;
    }



    if (BossEncounterManager::IsAnyEncounterActive())
    {
        return FALSE;
    }


    if (eOpenWorldMode == 2)
    {
        if (!ptPlayerInitiating)
        {
            return FALSE;
        }

        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayerInitiating->idPlayerACD);
        if (!ptACDPlayer)
        {
            return FALSE;
        }

        ACDID idACDKey = ACDID_INVALID;
        int32 nCount = 0;
        GBHandle hGBItem;
        hGBItem.eType = GB_ITEMS;
        hGBItem.gbid = GlobalGBIDGet(GLOBALGBID_LOOT_RUN_KEY);
        ACDInventoryGetItem(*ptACDPlayer, hGBItem, cgtDefaultPlayerInventorySlots, idACDKey, nCount, FALSE);

        if (idACDKey == ACDID_INVALID)
        {
            return FALSE;
        }

        ActorCommonData & tACDKey = ACDGet(idACDKey);
        SACDInventoryDeleteItem(tACDKey, 1, TRUE, FALSE);
    }


    tGCD.tGameSyncedData.eOpenWorldMode = eOpenWorldMode;
    tGCD.tGameSyncedData.eOpenWorldModeAct = eOpenWorldModeAct;
    tGCD.tGameSyncedData.nOpenWorldModeParam = nOpenWorldModeParam;

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    tSGameGlobals.snoOpenWorldHubWorld = snoHubWorld;
    tSGameGlobals.uOpenWorldHubEntranceGUID = hubEntranceGUID;

    const uint32 OPT_IN_DURATION = 1800;
    tGCD.tGameSyncedData.dwOpenWorldTransitionTime = GameGetServerTime() + OPT_IN_DURATION;


    SGameSyncData(NULL);


    if (PlayersGetCount() <= 1)
    {
        SGameAdvanceOpenWorldMode();
    }
    else
    {
        int32 nPlayerIndexToIgnore = -1;
        UpdateFunctionInitialize(UPDATE_OPEN_WORLD_TRANSITION, (DataID)-1, tSGameGlobals.tOpenWorldUpdateData);
        ServerQueueUpdateFunction(1, tSGameGlobals.tOpenWorldUpdateData);



        if (ptPlayerInitiating)
        {
            nPlayerIndexToIgnore = ptPlayerInitiating->nPlayerIndex;
            ptPlayerInitiating->bOptedInToOpenWorldModeChange = TRUE;
        }

        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            if (ptPlayer != ptPlayerInitiating)
            {
                ptPlayer->bOptedInToOpenWorldModeChange = FALSE;
                PlayerList listDestPlayers(PlayerListGetElementPool());
                PlayerIndexMessage tMessage;
                tMessage.nPlayerIndex = nPlayerIndexToIgnore;
                PlayerListGetSingle(listDestPlayers, ptPlayer->idPlayerACD);
                MessageSendToClient(listDestPlayers, SMSG_OPEN_WORLD_MODE_CHANGE, &tMessage, sizeof(tMessage));
            }
        }
    }

    return TRUE;
}



void SGameCheckOpenWorldModeAdvance()
{

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    ServerUpdateWasCalled(tSGameGlobals.tOpenWorldUpdateData);

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    if (tGCD.tGameSyncedData.dwOpenWorldTransitionTime == (uint32)-1)
    {
        return;
    }

    if (tGCD.tGameSyncedData.dwOpenWorldTransitionTime >= GameGetServerTime())
    {




        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            if (!ptPlayer->bOptedInToOpenWorldModeChange)
            {
                ServerQueueUpdateFunction(1, tSGameGlobals.tOpenWorldUpdateData);
                return;
            }
        }
    }
    SGameAdvanceOpenWorldMode();
}



void SGameAdvanceOpenWorldMode()
{


    GameCommonData & tGCD = GameGetActiveGameCommonData();
    OpenWorldMode eOpenWorldMode = tGCD.tGameSyncedData.eOpenWorldMode;

    if (tGCD.tGameSyncedData.dwOpenWorldTransitionTime == (uint32)-1)
    {
        return;
    }

    tGCD.tGameSyncedData.dwOpenWorldTransitionTime = (uint32)-1;
    SGameSyncData(NULL);

    if (eOpenWorldMode != 0)
    {
        sSGameResetOpenWorld();
    }

    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
    {
        SPlayerClearAllReturnPoints(*ptPlayer, SNO_NONE);
    }

    switch (eOpenWorldMode)
    {







    case 1:
        {
            ScriptEventOnUserEvent("OpenWorldGrantObjectivesForAct", ACDID_INVALID, 0);
        }
        break;


    case 2:
        {
            ScriptEventOnUserEvent("OpenWorldOpenDungeonFinderPortal", ACDID_INVALID, 0);
        }
        break;


    case 3:
        {
            SGameSetOpenWorldMode(NULL, 0, ACT_INVALID, -1, SNO_NONE, 0);
        }
        break;
    }



}

// Removed by the linker in the original (DWARF: SGame.cpp:6736, 0x9c bytes); only its DWARF and line-table rows survive: '{' at 6738, a 2-instruction
// statement at 6740, another at 6747, a 1-instruction statement at 6749 (all column 9), SGameSyncData inlined at 6750, '}' at 6752. Partly reconstructed.
void SGameOpenWorldReset()

{

    // 6740: not reconstructed (2 instructions; callee unknown)






    // 6747: not reconstructed (2 instructions; callee unknown)

    sSGameResetOpenWorld();  // inferred, no DWARF evidence of the callee: a 1-instruction statement here fits a no-argument call, and that static needs a second caller
    SGameSyncData(NULL);
}


















































































































































































































































void SGameGetOpenWorldHubWorldForPlayer(const Player & tPlayer, ResolvedPortalDestination & tDest)
{
    Act eAct = ACT_INVALID;
    SNO snoLastLevelArea = PlayerGetLastWaypointLevelArea(&tPlayer);
    if (snoLastLevelArea != SNO_NONE)
    {
        eAct = ActGetWaypointActByLevelArea(snoLastLevelArea);
    }
    if (eAct == ACT_INVALID)
    {
        // fall back to the game's default open world act

        SGame & tSGame = SGameManager::GameGet();


        eAct = tSGame.tGCD.tGameSyncedData.eOpenWorldDefaultAct;
    }

    ScopedActDefinitionLockR pdefAct(GlobalSNOGet(GLOBALSNO_ACT_OPEN_WORLD));
    switch (eAct)
    {
    case ACT_1:
        tDest = pdefAct->aStartLocOverride[0].tStartLocation;
        break;
    case ACT_2:
        tDest = pdefAct->aStartLocOverride[1].tStartLocation;
        break;
    case ACT_3:
    case ACT_4:
        tDest = pdefAct->aStartLocOverride[2].tStartLocation;
        break;
    case ACT_5:
        tDest = pdefAct->aStartLocOverride[3].tStartLocation;
        break;
    default:
        XASSERT(FALSE);
        break;
    }
}

SNO SGameGetChallengeRiftHubWorld()
{
    ScopedActDefinitionLockR pdefAct(GlobalSNOGet(GLOBALSNO_ACT_WEEKLY_CHALLENGE));
    return pdefAct->tStartLocDefault.snoWorld;
}















void SGameChangeActs(const Act eAct, SNO snoForceToWorld, BOOL bMovePlayers)
{




    if (eAct == ACT_INVALID)
    {
        return;
    }




    if (!SNOPackChunkLoadAct(eAct))
    {
        return;
    }





    SNO snoAct = ActSNOFromActEnum(eAct);
    ScriptEventOnChangedAct(snoAct);

    SConductor_ChangeActs();

    Minimap::ChangeActs();

    if (bMovePlayers)
    {
        GameCommonData & tGCD = GameGetActiveGameCommonData();
        if (tGCD.tGameSyncedData.eAct == eAct)
        {

            if (snoForceToWorld != SNO_NONE)
            {
                for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
                {
                    SGameACDChangeWorld(ptPlayer->idPlayerACD, snoForceToWorld, (uint32)-1, 0, 0.0f, NULL);
                }
            }
            return;
        }
    }





    CGameServerLoadingPartRequired(LSC_SNO_MANAGER);



    SGameSetCurrentAct(eAct);


    SGameInitWaypointsFromStartData(TRUE, TRUE);

    ResolvedPortalDestination tDest;
    Player * ptPlayer;
    if (bMovePlayers)
    {
        if (snoForceToWorld != SNO_NONE)
        {
            tDest.snoWorld = snoForceToWorld;
            tDest.uEntranceGUID = (uint32)-1;
        }
        else
        {
            ActGetCurrentStartLocation(tDest);
        }
        if (!SNOFileExists(WORLD_SNO, tDest.snoWorld))
        {
            CGameServerLoadingPartCompleted(LSC_SNO_MANAGER);
            return;
        }





        Player * ptFirstPlayer = PlayerGetFirstInGame();
        if (!ptFirstPlayer)
        {
            CGameServerLoadingPartCompleted(LSC_SNO_MANAGER);
            return;
        }




        ActorCommonData & tACDPlayer = ACDGet(ptFirstPlayer->idPlayerACD);

        if (tDest.snoWorld == WorldCommonGetWorldSNO(tACDPlayer.GetSWorldID()))
        {
            CGameServerLoadingPartCompleted(LSC_SNO_MANAGER);
            return;
        }







        if (tDest.snoWorld == SNO_NONE)
        {
            CGameServerLoadingPartCompleted(LSC_SNO_MANAGER);
            return;
        }










        uint32 dwDeleteTime = GameGetServerTime();
        SGame & tSGame = SGameManager::GameGet();
        SWorldID idSWorldNew = SWORLDID_INVALID;
        TDataArrayIterator<SWorld> tIt;
        while (tSGame.ptSWorlds->Iterate(tIt))
        {
            PowersForceStopForWorld(tIt->id);
            if (!SWorldIsFlaggedForDeletion(tIt->id))
            {
                SWorldFlagForDeletion(tIt->id, 0, FALSE);
            }
        }
        LevelAreaFlagAllForDeletion(dwDeleteTime);

        InactiveMarkerInitializeForAct();

        ServerQueueClientUpdate();


        for (ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {

            SPlayerKillAllPets(*ptPlayer, FALSE, FALSE, FALSE, FALSE);

            idSWorldNew = SActTravelTo(eAct, *ptPlayer, tDest, idSWorldNew);
        }



        ServerUpdateClients(TRUE);

        SWorldsDeleteFlagged();
        LevelAreasDeleteFlagged();
    }

    for (ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {

        ptPlayer->fOptedIntoActTransition = FALSE;

        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer)
        {

            if (!ptACDPlayer->IsAlive())
            {
                BrainAPIResurrectToCheckpoint(ptACDPlayer->id);
            }
            else
            {
                ptACDPlayer->ResetLifeStats(FALSE, TRUE, TRUE, TRUE, FALSE);
            }

            ptACDPlayer->ResetRestoredPlayerStats();
        }
    }



    CGameServerLoadingPartCompleted(LSC_SNO_MANAGER);



}



Act SGameGetCurrentMaxGameAct()
{
    Act eMaxAct = ACT_4;
    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACDPlayer)
            eMaxAct = Math::Max(SPlayerGetMaxAct(*ptPlayer), eMaxAct);
    }
    return eMaxAct;
}











void SGameAdvanceActs()
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    Act eCurrentAct = tGCD.tGameSyncedData.eAct;


    SGameEvaluateActCompletedAchievement(eCurrentAct);


    if (eCurrentAct == ACT_4)
    {
        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            BOOL bHardcore = ptPlayer->tHeroStateData.uPlayerFlags & (1 << PLAYERFLAG_HARDCORE);
            if (!bHardcore && !(ptPlayer->tAccount.uDigestFlags & (1 << D3::Account::Digest_Flags_MASTER_DIFFICULTY_UNLOCKED_DEPRECATED)))
            {
                ptPlayer->tAccount.uDigestFlags |= (1 << D3::Account::Digest_Flags_MASTER_DIFFICULTY_UNLOCKED_DEPRECATED);
                SPlayerAddToSaveList(*ptPlayer, 0, PlayerSaveReason_AcctFlags, PLAYER_SAVE_MODE_END_OF_FRAME);
                DWordDataMessage tMessage;
                tMessage.dwData = ptPlayer->tAccount.uDigestFlags;
                PlayerList listDestPlayers(PlayerListGetElementPool());
                PlayerListGetSingle(listDestPlayers, *ptPlayer);
                MessageSendToClient(listDestPlayers, SMSG_ACCOUNT_FLAGS_SYNC, &tMessage, sizeof(tMessage));
            }



            else if (bHardcore && !(ptPlayer->tAccount.uDigestFlags & (1 << D3::Account::Digest_Flags_HARDCORE_MASTER_DIFFICULTY_UNLOCKED_DEPRECATED)))
            {
                ptPlayer->tAccount.uDigestFlags |= (1 << D3::Account::Digest_Flags_HARDCORE_MASTER_DIFFICULTY_UNLOCKED_DEPRECATED);
                SPlayerAddToSaveList(*ptPlayer, 0, PlayerSaveReason_AcctFlags, PLAYER_SAVE_MODE_END_OF_FRAME);
                DWordDataMessage tMessage;
                tMessage.dwData = ptPlayer->tAccount.uDigestFlags;
                PlayerList listDestPlayers(PlayerListGetElementPool());
                PlayerListGetSingle(listDestPlayers, *ptPlayer);
                MessageSendToClient(listDestPlayers, SMSG_ACCOUNT_FLAGS_SYNC, &tMessage, sizeof(tMessage));
            }
        }
    }




    Act eMaxAct = SGameGetCurrentMaxGameAct();
    if (eCurrentAct >= eMaxAct)
    {
        SGameFinishGameEnd();
    }
    else
    {
        PlayerList listKickedPlayers(PlayerListGetElementPool());
        PlayerList listContinuingPlayers(PlayerListGetElementPool());
        Player * ptPlayer = PlayerGetFirstInGame();
        while (ptPlayer)
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            if (ptACDPlayer)
            {
                if (SPlayerGetMaxAct(*ptPlayer) == eCurrentAct)
                {
                    listKickedPlayers.AddTail_Tracked(ptPlayer);
                    SPlayerDisconnect(*ptPlayer, QUIT_REASON_GAME_END, FALSE);
                }
                else
                {
                    listContinuingPlayers.AddTail_Tracked(ptPlayer);
                }
            }

            ptPlayer = PlayerGetNextInGame(ptPlayer);
        }

        QuitGameMessage tQuitMessage;
        tQuitMessage.eReason = QUIT_REASON_GAME_END;
        MessageSendToClient(listKickedPlayers, SMSG_QUIT, &tQuitMessage, sizeof(tQuitMessage));

        if (eCurrentAct == ACT_4)
        {

            SimpleMessage tMessage;
            MessageSendToClient(listContinuingPlayers, SMSG_PLAY_FINAL_CUTSCENE, &tMessage, sizeof(tMessage));
        }

        int32 nIndex = ActIndexFromActEnum(eCurrentAct);
        Act eNextAct = ActEnumFromActIndex(nIndex + 1);
        if (eNextAct != ACT_INVALID)
        {
            SGameChangeActs(eNextAct, SNO_NONE, TRUE);
        }
        SActTransitionEnd();
    }
}



void SGameEvaluateActCompletedAchievement(Act eActCompleted)
{
    Achievement::EvaluationData tEvaluationData;
    for (tEvaluationData.SetPlayer(PlayerGetFirstInGame()); tEvaluationData.GetPlayer(); tEvaluationData.SetPlayer(PlayerGetNextInGame(tEvaluationData.GetPlayer())))
        tEvaluationData.EvaluateEvent(tEvaluationData.GetPlayer(), ACH_EVENT_COMPLETE_ACT, eActCompleted, 1, ((SGameManager::GameGet().tStartParams.dwCreationFlags >> GAMECREATEPARAM_SEASON) & 1) | ((SGameManager::GameGet().tStartParams.dwCreationFlags >> 19) & 2));
}



















BOOL DeleteAllWorldsAndTransferPlayersToLimbo()
{
    SNO snoWorldLimbo = GlobalSNOGet(GLOBALSNO_WORLD_LIMBO);
    if (snoWorldLimbo == SNO_NONE)
        return FALSE;

    SConductor_ChangeActs();
    Minimap::ChangeActs();

    SWorldID idSWorldLimbo = WorldCommonFindBySNO(snoWorldLimbo, TRUE);
    if (idSWorldLimbo == SWORLDID_INVALID)
        idSWorldLimbo = SWorldCreate(snoWorldLimbo, TRUE, TRUE, NULL, ACDID_INVALID, ACDID_INVALID, 0, 0, FALSE, 0);

    for (Player * ptPlayerCurr = PlayerGetFirstInGame(); ptPlayerCurr; ptPlayerCurr = PlayerGetNextInGame(ptPlayerCurr))
    {
        ActorCommonData * ptACDPlayerCurr = ACDTryToGet(ptPlayerCurr->idPlayerACD);
        if (ptACDPlayerCurr)
        {
            SPlayerKillAllPets(*ptPlayerCurr, TRUE, TRUE, TRUE, FALSE);
            SWorldPlayerEnter(idSWorldLimbo, ptPlayerCurr->idPlayerACD, NULL, (uint32)-1, FALSE,
                              D3::Hero::SavedDefinition::default_instance(),
                              D3::Account::SavedDefinition::default_instance(),
                              D3::Items::Mails::default_instance(), NULL);
            SPlayerWarped(*ptACDPlayerCurr, 0, NULL, ACDID_INVALID, 0.0f, TRUE);
        }
    }

    ServerUpdateClients(TRUE);
    DeleteAllWorldsExceptLimbo();
    return TRUE;
}



















void DeleteAllWorldsExceptLimbo()
{
    SNO snoWorldLimbo = GlobalSNOGet(GLOBALSNO_WORLD_LIMBO);
    uint32 uDeleteTime = GameGetServerTime();
    SGame & tSGame = SGameManager::GameGet();
    TDataArrayIterator<SWorld> tIt;
    while (tSGame.ptSWorlds->Iterate(tIt))
    {
        SWorldID idSWorldCurr = tIt->id;
        SWorld * ptSWorldCurr = SWorldTryToGet(idSWorldCurr);
        if (ptSWorldCurr && ptSWorldCurr->tWCD.snoWorld != snoWorldLimbo)
        {
            PowersForceStopForWorld(idSWorldCurr);
            SWorldFlagForDeletion(idSWorldCurr, 0, TRUE);
            LevelAreaWorldFlagForDeletion(idSWorldCurr, uDeleteTime);
            Minimap::RemoveAllForWorld(idSWorldCurr);
        }
    }
    SWorldsDeleteFlagged();
    LevelAreasDeleteFlagged();
    Minimap::ChangeActs();
}




















BOOL TraceStatEnabled()
{
    return (sgtAppGlobals.dwDebugFlags & (1 << APP_GLOBAL_TRACE_STAT_BIT)) != 0;
}


FILE * fopen_ex(const char * filename, const char * mode);  // X/Sigma/Files/XFilesCore.cpp
SNO SceneCommonGetSceneSNO(SSceneID idSScene);  // X/Common/SceneCommon.cpp








void TraceStat(LPCSTR sEventType, ACDID idACD, LPCSTR cpFmtStr, ...)
{
    char szACDBuf[1024];


    if (!TraceStatEnabled())
        return;



    if (!SGameIsValid())
        return;



    SGameGlobals & tSGameGlobals = SGameGlobalsGet();

    memset(szACDBuf, 0, sizeof(szACDBuf));

    if (idACD != ACDID_INVALID)
    {
        char szAccountName[128];
        char szHeroName[32];
        char szScene[255];
        char szLevelArea[255];
        ActorCommonData & tACD = ACDGet(idACD);

        Player * ptPlayer = PlayerGetByACD(tACD.id);

        if (ptPlayer)
        {
            Blizzard::String::Copy(szAccountName, ptPlayer->tAccount.usName, sizeof(szAccountName));
            Blizzard::String::Copy(szHeroName, ptPlayer->usHeroName, sizeof(szHeroName));
        }
        else
        {
            Blizzard::String::Copy(szAccountName, "NOACCOUNT", sizeof(szAccountName));
            Blizzard::String::Copy(szHeroName, "NOHERO", sizeof(szHeroName));
        }

        SSceneID idSScene = ACDGetCurrentSSceneID(tACD);
        SNO snoScene;
        if (idSScene != -1)
            snoScene = SceneCommonGetSceneSNO(idSScene);
        else
            snoScene = SNO_NONE;

        SNO snoLevelArea = ACDGetLevelArea(tACD);

        strcpy(szScene, SNOToString(SCENE_SNO, snoScene, FALSE));
        strcpy(szLevelArea, SNOToString(LEVELAREA_SNO, snoLevelArea, FALSE));

        sprintf(szACDBuf, "\t%s\t%s\t%s\t%s", szAccountName, szHeroName, szLevelArea, szScene);
    }










    char szBuf[1024];
    va_list tArgList;
    va_start(tArgList, cpFmtStr);

    Blizzard::String::VFormat(szBuf, sizeof(szBuf), cpFmtStr, tArgList);
    szBuf[sizeof(szBuf) - 1] = 0;
    va_end(tArgList);


    FILE * fileStatFile = fopen_ex("debug\\DiabloStat.txt", "a");
    if (fileStatFile)
    {




        Blizzard::Time::TimeRec tTimeRecord;
        Blizzard::Time::BreakTime(Blizzard::Time::LocalBias(0), tTimeRecord);

        fprintf(fileStatFile, "%s\t%04d-%02d-%02d %02d:%02d:%02d\t%u%s\t%s\n",
            sEventType,



            tTimeRecord.year,
            tTimeRecord.month,
            tTimeRecord.day,
            tTimeRecord.hour,
            tTimeRecord.minute,
            tTimeRecord.second,
            tSGameGlobals.dwUniqueGameKey,
            szACDBuf, szBuf);



        fclose(fileStatFile);
    }
}

void SGameACDChangeWorld(const ACDID idACD, const SNO snoWorld, const uint32 uEntranceGUID, PlayerWarpReason eReason, const float flWarpFadeInSecs, const Player * ptReferencePlayer)
{
    if (!SPlayerCanSwitchToNewWorld(idACD))
        return;

    SWorldID idSWorld = WorldCommonFindBySNO(snoWorld, TRUE);
    if (idSWorld == SWORLDID_INVALID)
    {
        idSWorld = SWorldCreate(snoWorld, TRUE, ptReferencePlayer ? SPlayerIsLocal(*ptReferencePlayer) : FALSE, NULL, ACDID_INVALID, ACDID_INVALID, uEntranceGUID, 0, FALSE, 0);
        if (idSWorld == SWORLDID_INVALID)
            return;
    }

    {
        SWorld * ptSWorld = SWorldGet(idSWorld);
        if (ptSWorld && ptSWorld->dwDeleteTime == 0 && !SWorldIsCreated(*ptSWorld))
            SWorldFlushWorldCreationList();

        ActorCommonData & tACD = ACDGet(idACD);
        SWorldPlayerEnter(idSWorld, tACD.id, NULL, uEntranceGUID, FALSE,
                          D3::Hero::SavedDefinition::default_instance(),
                          D3::Account::SavedDefinition::default_instance(),
                          D3::Items::Mails::default_instance(), ptReferencePlayer);
        if (ACDIsPlayer(tACD.id))
            SPlayerWarped(tACD, eReason, NULL, ACDID_INVALID, flWarpFadeInSecs, TRUE);
    }
}


































































void SGameStopServer(SGameID idSGame)
{
    BOOL bReturnToClient = GameServerCodeEnter_Track() || _GameServerCodeEnter(idSGame);
    SGameDisposeGame();
    GameServerCodeLeave_Tracked(bReturnToClient);
}





void SGameSyncData(const Player * ptPlayer)
{
    const GameCommonData & tGCD = GameGetActiveGameCommonData();
    GameSyncedDataMessage tMessage;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    tMessage.tSyncedData = tGCD.tGameSyncedData;
    if (ptPlayer == NULL)
        PlayerListGetAllConnected(listDestPlayers, -1, FALSE);
    else
        listDestPlayers.AddTail_Tracked((Player *)ptPlayer);
    MessageSendToClient(listDestPlayers, SMSG_GAME_SYNCED_DATA, &tMessage, sizeof(tMessage));
}

uint64 sub_71007b5e70()  // 0x71007b5e70 (unnamed): GAMECREATEPARAM_SEASON bit | (creation bit 20 << 1); every 2.7.7 EvaluateEvent caller passes it
{
    return ((SGameManager::GameGet().tStartParams.dwCreationFlags >> GAMECREATEPARAM_SEASON) & 1) | ((SGameManager::GameGet().tStartParams.dwCreationFlags >> 19) & 2);
}




















BOOL SGameIsPvPFlagSet()
{
    GameCommonData & tGCD = GameGetActiveGameCommonData();
    return (tGCD.tGameSyncedData.dwGameSyncedFlags & (1 << GAME_SYNCED_FLAGS_PVP)) != 0;
}



void SGameSendShutdownWarning(
    int32 nTime,
    ServerShutdownType eType)
{
    ServerNotificationDataMessage tMsg;
    tMsg.uSec = nTime;
    tMsg.eType = eType;

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, 0, -1);
    MessageSendToClient(listDestPlayers, SMSG_SYSTEM_NOTIFICATION, &tMsg, sizeof(tMsg));
}



static void SGameBroadcast(
    const char * szMessage)
{
    BroadcastTextMessage tMsg;
    Blizzard::String::Copy(tMsg.szMsg, szMessage, sizeof(tMsg.szMsg));

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, 0, -1);
    MessageSendToClient(listDestPlayers, SMSG_SYSTEM_BROADCAST, &tMsg, sizeof(tMsg));
}



void SGame::OnSavedHeros(SGameID id, OnlineService::ErrorCode errCode)
{
    if (errCode == OnlineService::ERROR_GLOBAL_SUCCESS)
        return;
    {
        SGame * ptSGame = SGameManager::GameTryToGet(id);
        if (ptSGame && (ptSGame->tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_UTILITY)))
            return;
    }
    SGameManager::SignalKillGame(id);
}









void SGameForcePlayersQuit(QuitReason eReason)
{
    for (Player * ptPlayer = PlayerGetFirstAll(); ptPlayer; ptPlayer = PlayerGetNextAll(ptPlayer))
    {
        if (ptPlayer->idGameConnection != GAMECONNECTIONID_INVALID)
            SPlayerDisconnect(*ptPlayer, eReason, TRUE);
    }
}






SNO SGameGetStartWorld()
{

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    SNO snoWorld = tSGameGlobals.snoInitialWorld;









    GameType eGameType = GameGetGameType();
    if (eGameType == GT_DIABLO3)
    {
        ResolvedPortalDestination tDest;
        ActGetCurrentStartLocation(tDest);
        if (tDest.IsValid())  // if/else form: original standalone copy (0x74) selects with csinv and returns at 7599
        {
            snoWorld = tDest.snoWorld;
        }
        else
        {
            snoWorld = SNO_NONE;
        }
    }


    return snoWorld;
}

















void SGameStartGameEnd(Player & tPlayer)
{
    SActTransitionStart(&tPlayer, FALSE);
}





void SGameFinishGameForConsole(QuitReason eReason)
{


    SGamePause(FALSE);
    SGameSoftPause(FALSE);

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);



    POS pos = listDestPlayers.GetHeadPosition();
    while (pos)
    {
        Player * ptPlayer = listDestPlayers.GetNext(pos);
        if (ptPlayer)
        {
            SPlayerConsoleSync(*ptPlayer);
        }
    }

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();


    if (tSGameGlobals.ptAchievementsInterface)
    {
        tSGameGlobals.ptAchievementsInterface->Process();
    }

    QuitGameMessage tQuitMessage;
    tQuitMessage.eReason = eReason;
    MessageSendToClient(listDestPlayers, SMSG_QUIT, &tQuitMessage, sizeof(tQuitMessage));

    SimpleMessage tGameOverMessage;
    MessageSendToClient(listDestPlayers, SMSG_CONSOLE_GAME_ENDED, &tGameOverMessage, sizeof(tGameOverMessage));

    Console::Online::PartyServerEndingGame();
}




void SGameFinishGameEnd()
{



    SGamePause(FALSE);
    SGameSoftPause(FALSE);




    SNO snoAct = ActSNOFromActEnum(GameGetCurrentAct());
    ScriptEventOnChangedAct(snoAct);


    QuestSetGameOver();

    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);



    POS pos = listDestPlayers.GetHeadPosition();
    while (pos)
    {
        Player * ptPlayer = listDestPlayers.GetNext(pos);
        if (ptPlayer)
        {
            SPlayerUnlockAdventureMode(*ptPlayer);
        }
    }






    SGameFinishGameForConsole(QUIT_REASON_GAME_END);


    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    SGameID idSGame = SGameManager::GetCurrentGame();


    tSGameGlobals.dwFlags |= (1 << SGAMEGLOBALS_REQUEST_SHUT_DOWN);
}



void SGameBootPendingPlayer(
    OnlineService::HeroId uHeroId,
    QuitReason eQuitReason,
    const char * pszBootLogReason,
    ServerSocketID idServerSocket)
{
    SGame & tSGame = SGameManager::GameGet();
    OnlineService::PlayerResolveData tResolvedPlayer;

    if (tSGame.tPendingJoinMap.RemoveAt(uHeroId, tResolvedPlayer))
    {




        if (SGameGlobalsGet().ptAchievementsInterface)
        {
            SGameGlobalsGet().ptAchievementsInterface->CancelSubscription(tResolvedPlayer.tAccountId,
                OnlineService::AchievementsInterface::RegistrationCallback());
        }
        SGameManager::ClearAccountSGameId(tResolvedPlayer.tAccountId);
    }






















    if (eQuitReason != QUIT_REASON_COUNT && XServerSocketIDValid(idServerSocket))
    {

        QuitGameMessage tQuitMessage;
        tQuitMessage.eReason = eQuitReason;
        ServerSendOutOfGameMessage(idServerSocket, &tQuitMessage, SMSG_QUIT, sizeof(tQuitMessage));

        SGameManager::SocketMarkForDelete(idServerSocket, SDR_BOOT);
    }
}











bool SGameGetPendingPlayerByAccount(const OnlineService::GameAccountHandle & tAccountId, OnlineService::PlayerResolveData & tResolvedPlayer)
{
    SGame & tSGame = SGameManager::GameGet();

    XGrowableArray<OnlineService::PlayerResolveData, const OnlineService::PlayerResolveData &> tPendingPlayers;
    tSGame.tPendingJoinMap.Copy(tPendingPlayers);
    for (int32 i = 0; i < tPendingPlayers.GetArraySize(); i++)
    {
        OnlineService::PlayerResolveData & tPlayerInArray = tPendingPlayers[i];
        if (tPlayerInArray.tAccountId == tAccountId)
        {
            tResolvedPlayer = tPlayerInArray;
            return true;
        }
    }
    return false;
}



void SGameHandlePauseRequest(Player & tPlayer, BOOL bPause)
{





    if (!bPause)
    {
        SGamePause(FALSE);
        return;
    }

    if (tPlayer.bIsBeingDeleted)
    {
        return;
    }



    SGame & tSGame = SGameManager::GameGet();
    if (tSGame.tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_PUBLIC_BIT))
    {
        return;
    }


    if (SPlayerInScriptedSequence(tPlayer))
    {
        return;
    }


    if (GameIsWeeklyChallenge() && NephalemRift::IsTieredRiftActive())
    {
        return;
    }


    PlayerList listDestPlayers(PlayerListGetElementPool());



    if (SPlayersAreAllLocal()
















        && !SWorldAnyLoading())






    {
        SGamePause(bPause);
    }
}


void SGameHandleSoftPauseRequest(Player & tPlayer, BOOL bPause)
{





    if (!bPause)
    {
        SGameSoftPause(FALSE);
        return;
    }


    SGame & tSGame = SGameManager::GameGet();
    if (tSGame.tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_PUBLIC_BIT))
    {
        return;
    }


    PlayerList listDestPlayers(PlayerListGetElementPool());



    if (SPlayersAreAllLocal()
















        && !SWorldAnyLoading())






    {
        SGameSoftPause(bPause);
    }
}


void SGameSoftPause(BOOL bPaused)
{





    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    if (((tSGameGlobals.dwFlags >> SGAMEGLOBALS_GAME_SOFT_PAUSED_BIT) & 1) == bPaused)
    {
        return;
    }


    if (bPaused && (tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_REQUEST_SHUT_DOWN)))
    {
        return;
    }



    if (bPaused) tSGameGlobals.dwFlags |= (1 << SGAMEGLOBALS_GAME_SOFT_PAUSED_BIT); else tSGameGlobals.dwFlags &= ~(1 << SGAMEGLOBALS_GAME_SOFT_PAUSED_BIT);



    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);
    BoolDataMessage tMessage;
    tMessage.bVal = bPaused;
    MessageSendToClient(listDestPlayers, SMSG_SOFT_PAUSE_STATE_CHANGE, &tMessage, sizeof(tMessage));

    if (Console::Online::PartyIsInParty())
    {
        Console::Online::PartySetGameIsSoftPaused(bPaused);
    }

}



void SGamePause(BOOL bPaused)
{

    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    if (((tSGameGlobals.dwFlags >> SGAMEGLOBALS_GAME_PAUSED_BIT) & 1) == bPaused)
    {
        return;
    }



    if (bPaused && (tSGameGlobals.dwFlags & (1 << SGAMEGLOBALS_REQUEST_SHUT_DOWN)))
    {
        return;
    }



    if (bPaused)
    {

        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            if (SPlayerIsLoggingOut(FALSE, *ptPlayer))
            {
                return;
            }
        }


        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
        {
            PlayerBrainClearQueuedPower(ptPlayer->idPlayerACD);
        }
    }


    if (bPaused) tSGameGlobals.dwFlags |= (1 << SGAMEGLOBALS_GAME_PAUSED_BIT); else tSGameGlobals.dwFlags &= ~(1 << SGAMEGLOBALS_GAME_PAUSED_BIT);
    tSGameGlobals.dwPauseMessageDropCount = 0;



    PlayerList listDestPlayers(PlayerListGetElementPool());
    PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);
    BoolDataMessage tMessage;
    tMessage.bVal = bPaused;
    MessageSendToClient(listDestPlayers, SMSG_PAUSE_STATE_CHANGE, &tMessage, sizeof(tMessage));


    if (Console::Online::PartyIsInParty())
    {
        Console::Online::PartySetGameIsPaused(bPaused);
    }

}













#include "Movement/SActorMovement.h"
void SGameAutoSelectPartyGuideAccount();
void ActorAIUpdateServerVariables();  // X/Server/AI/ActorAI.cpp
void ActorUpdateServerVariables();  // X/Server/Actors/Actors.cpp
void BrainComponentUpdateServerVariables();  // X/Server/Brains/BrainComponent.cpp
void PowerUpdateServerVariables();  // X/Server/Powers/Powers.cpp
void SPlayerUpdateLevelCaps();  // X/Server/SPlayer.cpp
void ServerUpdateServerVars();  // X/Server/Server.cpp
void SGameUpdateServerVariables();


static const uint32 sgTicksBetweenServerVariableChecks = 180;
static void sServerVariableUpdate()
{
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    uint32 uNow = GameGetServerTime();
    if (uNow >= tSGameGlobals.uNextServerVariableCheckTick)
    {
        SActorMovement::UpdateServerVariables();
        ActorAIUpdateServerVariables();
        ActorUpdateServerVariables();
        BrainComponentUpdateServerVariables();
        PowerUpdateServerVariables();
        SPlayerUpdateLevelCaps();
        SGameUpdateServerVariables();
        ServerUpdateServerVars();

        tSGameGlobals.uNextServerVariableCheckTick = uNow + sgTicksBetweenServerVariableChecks;
    }
}



static const uint32 sgTicksBetweenServerPartyGuideChecks = 120;
static void sServerPartyGuideUpdate()
{





    if (!(SGameManager::GameGet().tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_PUBLIC_BIT)))
        return;



    uint32 uNow = GameGetServerTime();
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    if (uNow < tSGameGlobals.uNextServerPartyGuideCheckTick)
        return;
    tSGameGlobals.uNextServerPartyGuideCheckTick = uNow + sgTicksBetweenServerPartyGuideChecks;



    GameCommonData & tGCD = GameGetActiveGameCommonData();
    if (tGCD.tGameSyncedData.uPartyGuideHeroId != OnlineService::INVALID_HERO_ID)
    {
        Player * ptPlayer = PlayerGetByHeroId(tGCD.tGameSyncedData.uPartyGuideHeroId);
        if (ptPlayer && ptPlayer->idGameConnection != GAMECONNECTIONID_INVALID)
        {

            return;
        }
    }



    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACD && ptACD->AttributesGetInt(ATTRIBUTE_LOADING_NEW_GAME))
        {
            return;
        }
    }



    SGameAutoSelectPartyGuideAccount();
}
void SGameSetPartyGuideAccount(const Player & tPlayer)
{


    if (SGameManager::GameGet().tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_PUBLIC_BIT))
    {




        GameCommonData & tGCD = GameGetActiveGameCommonData();
        tGCD.tGameSyncedData.uPartyGuideHeroId = tPlayer.uHeroId;

        SGameSyncData(NULL);
    }
}


void SGameAutoSelectPartyGuideAccount()
{


    SGame & tSGame = SGameManager::GameGet();
    if (!(tSGame.tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_PUBLIC_BIT)))
        return;

    int32 nMaxLevel = -1;
    int32 nMaxAltLevel = -1;
    GameCommonData & tGCD = GameGetActiveGameCommonData();



    for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
    {
        const ActorCommonData * ptACD = ACDTryToGet(ptPlayer->idPlayerACD);
        if (ptACD)
        {



            int32 nLevelCap = ptACD->AttributesGetInt(ATTRIBUTE_LEVEL_CAP);
            if (ptPlayer->nCurrentLevel > nMaxLevel ||
                (ptPlayer->nCurrentLevel >= nLevelCap && ptPlayer->nCurrentLevel >= nMaxLevel &&
                 ptPlayer->nCurrentAltLevel > nMaxAltLevel))
            {
                nMaxAltLevel = ptPlayer->nCurrentAltLevel;
                nMaxLevel = ptPlayer->nCurrentLevel;
                tGCD.tGameSyncedData.uPartyGuideHeroId = ptPlayer->uHeroId;
            }
        }
    }
    if (nMaxLevel >= 0)
    {
        SGameSyncData(NULL);
    }
}



static void sSGameUpdateMatchmakingStatus()
{

    SGame & tSGame = SGameManager::GameGet();
    MatchmakingData & tMatchmakingData = tSGame.ptSGameGlobals->tMatchmakingData;









    int32 nGameTag = -1;



    if (Console::Online::PartyGetGameTag(nGameTag))
    {






        OnlineService::XGameTagArray arGameTags;
        OnlineService::GetDefaultGameTags(arGameTags);
        for (uint32 i = 0; i < arGameTags.size(); i++)
        {
            if (arGameTags[i].m_tagId == nGameTag)
            {
                Blizzard::String::Copy(tSGame.ptSGameGlobals->tMatchmakingData.szGameTag, arGameTags[i].m_tag.str(), sizeof(tSGame.ptSGameGlobals->tMatchmakingData.szGameTag));
                break;
            }
        }
    }


    BOOL bMatchmakingEnabled = TRUE;










    if (GameIsOpenWorld() && (tSGame.ptSGameGlobals->dwFlags2 & (1 << SGAMEGLOBALS2_ADVENTURE_MODE_BOUNTY_ACT_COMPLETE)))
    {
        bMatchmakingEnabled = FALSE;
    }


    else if (tSGame.ptSGameGlobals &&
        strcasecmp(tSGame.ptSGameGlobals->tMatchmakingData.szGameTag, OnlineService::GAME_TAG_KEYWARDEN) == 0)
    {
        int32 nKeyWardensKilledThreshold =
            GameIsOpenWorld() ? 4 : 1;




        int32 nKeywardensKilled = 0;
        for (int32 i = 0; i < 4; i++)
        {
            if (tSGame.ptSGameGlobals->dwKeyWardenKilledFlags & (1 << i))
                nKeywardensKilled++;
        }
        if (nKeywardensKilled >= nKeyWardensKilledThreshold)
        {
            bMatchmakingEnabled = FALSE;
        }
    }




    else if (tSGame.ptSGameGlobals &&
        strcasecmp(tSGame.ptSGameGlobals->tMatchmakingData.szGameTag, OnlineService::GAME_TAG_DEFAULT) == 0)
    {

        bMatchmakingEnabled = QuestIsEligibleForMatchmaking();



        if (bMatchmakingEnabled && s_varMatchmakingPlayerExitFunctionalityEnabled.Get())
        {
            uint32 dwMatchmakingForceOpenDuration = s_varMatchmakingForceOpenDuration.Get();
            uint32 dwMatchmakingCloseAfterPlayerExitDuration = s_varMatchmakingCloseAfterPlayerExitDuration.Get();

            uint32 dwGameForcedOpenUntil = tMatchmakingData.dwGameMadePublicTick + dwMatchmakingForceOpenDuration;
            uint32 dwGameReopenTime = tMatchmakingData.dwLastPlayerLeaveTick + dwMatchmakingCloseAfterPlayerExitDuration;

            uint32 dwNow = GameGetServerTime();

            if (tMatchmakingData.dwLastPlayerLeaveTick > dwGameForcedOpenUntil && dwNow < dwGameReopenTime)
            {
                bMatchmakingEnabled = FALSE;
            }
        }
    }


    else if (tSGame.ptSGameGlobals &&
        strcasecmp(tSGame.ptSGameGlobals->tMatchmakingData.szGameTag, OnlineService::GAME_TAG_RIFT) == 0)
    {
        if (tSGame.ptSGameGlobals->dwFlags2 & (1 << SGAMEGLOBALS2_LOOT_RUN_BOSS_KILLED))
        {
            bMatchmakingEnabled = FALSE;
        }
    }
    else if (tSGame.ptSGameGlobals &&
        strcasecmp(tSGame.ptSGameGlobals->tMatchmakingData.szGameTag, OnlineService::GAME_TAG_GREATER_RIFT) == 0)
    {
        if (NephalemRiftManager::IsAnyRiftActive())
        {
            bMatchmakingEnabled = FALSE;
        }
    }


    if (bMatchmakingEnabled && NephalemRiftManager::IsTieredRiftActive())
    {

        if (!CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT))
        {
            bMatchmakingEnabled = FALSE;
        }
    }

    if (bMatchmakingEnabled && SetEncounterManager::IsInProgress())
    {
        bMatchmakingEnabled = FALSE;
    }

    if (CmdLineGetParams()->tBitArray.Get(CMDLINE_BLIZZCON_BIT) &&
        GameGetActiveGameCommonData().tGameSyncedData.bLootRunBossDead)
    {
        bMatchmakingEnabled = FALSE;
    }




    if (bMatchmakingEnabled != tMatchmakingData.bMatchmakingServerState)
    {







        Console::Online::PartySetMatchmakingEnabled(bMatchmakingEnabled);
        tMatchmakingData.bMatchmakingServerState = bMatchmakingEnabled;
    }
}

extern XVarBool s_varEasterEggWorldPerkEnabled;

static void sSGameUpdateCommunityBuffs()
{
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();

    ServerUpdateWasCalled(tSGameGlobals.tCommunityBuffsUpdateData);
    ServerQueueUpdateFunction(60, tSGameGlobals.tCommunityBuffsUpdateData);


    BOOL bEasterEggWorldEnabled = s_varEasterEggWorldPerkEnabled.Get();
    if (bEasterEggWorldEnabled && !GameFlagIsSet("TotallyNotACowLevel"))
    {
        GameFlagSet("TotallyNotACowLevel_NotSpawned", TRUE);
    }
    GameFlagSet("TotallyNotACowLevel", bEasterEggWorldEnabled);
}




int32 SGameGetStatDataKey()
{
    SGame & tSGame = SGameManager::GameGet();
    int32 nDataKey = GameGetHandicapLevel() * 10000 + Math::Max(tSGame.tStartParams.nMonsterLevel, 0) * 100 + Math::Max(GameGetMonsterLevel(), 0);
    if (tSGame.tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_HARDCORE))
        nDataKey += 1000000;
    if (tSGame.tStartParams.dwCreationFlags & (1 << GAMECREATEPARAM_PUBLIC_BIT))
        nDataKey += 10000000;
    return nDataKey;
}




















void SGameInitializeDungeonFinderPortalLevelAreaList()
{
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    tSGameGlobals.tDungeonFinderData.listAvailableLevelAreas.RemoveAll();
    const CachedWaypointArray & tWaypoints = ActGetCachedWaypointArray();
    size_t nCount = tWaypoints.size();
    for (uint32 i = 0; i < nCount; i++)
    {
        const CachedWaypoint & tWaypoint = tWaypoints[i];
        if (tWaypoint.tInfo.snoLevelArea != SNO_NONE)
        {
            ScopedLevelAreaDefinitionLockR pdefLevelArea(tWaypoint.tInfo.snoLevelArea);
            if ((pdefLevelArea->dwFlags >> LEVEL_AREA_FLAG_DUNGEON_FINDER_AREA) & 1)
            {
                if (!tSGameGlobals.tDungeonFinderData.listAvailableLevelAreas.Find(tWaypoint.tInfo.snoLevelArea))
                    tSGameGlobals.tDungeonFinderData.listAvailableLevelAreas.AddTail_Tracked(tWaypoint.tInfo.snoLevelArea);
            }
        }
    }
}





#include "../Sigma/MemoryPools.h"

SNO SGamePickDungeonFinderLevelArea()
{


    GameCommonData & tGCD = GameGetActiveGameCommonData();
    Act eCurrentAct = tGCD.tGameSyncedData.eOpenWorldDefaultAct;
    if (tGCD.tGameSyncedData.snoDungeonFinderLevelArea != SNO_NONE)
    {
        eCurrentAct = ActGetWaypointActByLevelArea(tGCD.tGameSyncedData.snoDungeonFinderLevelArea);
    }

    SNOPooledList listLevelAreas(MemoryPoolsGetDATAIDListPool());
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    POS pos = tSGameGlobals.tDungeonFinderData.listAvailableLevelAreas.GetHeadPosition();
    while (pos)
    {
        SNO snoLevelArea = tSGameGlobals.tDungeonFinderData.listAvailableLevelAreas.GetNext(pos);
        if (ActGetWaypointActByLevelArea(snoLevelArea) == eCurrentAct)
        {
            listLevelAreas.AddTail_Tracked(snoLevelArea);
        }
    }


    if (listLevelAreas.Count() == 0)
    {
        pos = tSGameGlobals.tDungeonFinderData.listAvailableLevelAreas.GetHeadPosition();
        while (pos)
        {
            SNO snoLevelArea = tSGameGlobals.tDungeonFinderData.listAvailableLevelAreas.GetNext(pos);
            listLevelAreas.AddTail_Tracked(snoLevelArea);
        }
    }

    if (listLevelAreas.Count() == 0)
    {
        return SNO_NONE;
    }


    SNO snoLevelArea = SNO_NONE;


    int32 nRandomIndex = Rand_Internal(tSGameGlobals.rng, listLevelAreas.Count() - 1);
    pos = listLevelAreas.GetHeadPosition();
    for (int32 i = nRandomIndex; i >= 0; i--)
    {
        snoLevelArea = listLevelAreas.GetNext(pos);
    }



    tSGameGlobals.tDungeonFinderData.listAvailableLevelAreas.RemoveAt(tSGameGlobals.tDungeonFinderData.listAvailableLevelAreas.Find(snoLevelArea));

    tGCD.tGameSyncedData.snoDungeonFinderLevelArea = snoLevelArea;
    SGameSyncData(NULL);

    {
        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetAllInGame(listDestPlayers, -1, 0, -1);


        LootRunAppearedMessage tMessage;
        SigmaMemoryClear(&tMessage, sizeof(tMessage));
        tMessage.snoLevelArea = snoLevelArea;
        MessageSendToClient(listDestPlayers, SMSG_LOOTRUN_APPEARED, &tMessage, sizeof(tMessage));
    }

    return snoLevelArea;
}



void SGameSendDungeonFinderProgressToPlayer(Player & tPlayer)
{
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    FloatDataMessage tProgressMessage;
    tProgressMessage.flData = tSGameGlobals.tDungeonFinderData.flDungeonFinderProgress;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    listDestPlayers.AddTail_Tracked(&tPlayer);
    MessageSendToClient(listDestPlayers, SMSG_DUNGEON_FINDER_PROGRESS, &tProgressMessage, sizeof(tProgressMessage));
}






void SGameSendDungeonFinderTimedEventToPlayer(Player & tPlayer)
{
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    SNODataMessage tMessage;
    tMessage.snoData = tSGameGlobals.tDungeonFinderData.snoDungeonFinderTimedEvent;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    listDestPlayers.AddTail_Tracked(&tPlayer);
    MessageSendToClient(listDestPlayers, SMSG_DUNGEON_FINDER_SET_TIMEDEVENT, &tMessage, sizeof(tMessage));
}






void SGameSendDungeonFinderSeedToPlayer(Player & tPlayer)
{
    SGameGlobals & tSGameGlobals = SGameGlobalsGet();
    IntDataMessage tSeedMessage;
    tSeedMessage.nData = tSGameGlobals.tDungeonFinderData.tDungeonFinderRNG.dwSeed;
    PlayerList listDestPlayers(PlayerListGetElementPool());
    listDestPlayers.AddTail_Tracked(&tPlayer);
    MessageSendToClient(listDestPlayers, SMSG_DUNGEON_FINDER_SEED, &tSeedMessage, sizeof(tSeedMessage));
}






void SGameSendDungeonFinderParticipatingPlayerCountToPlayer(Player & tPlayer)
{
    IntDataMessage tPlayerCountMessage;
    tPlayerCountMessage.nData = NephalemRiftManager::RiftPlayerCount();
    PlayerList listDestPlayers(PlayerListGetElementPool());
    listDestPlayers.AddTail_Tracked(&tPlayer);
    MessageSendToClient(listDestPlayers, SMSG_DUNGEON_FINDER_PARTICIPATING_PLAYER_COUNT, &tPlayerCountMessage, sizeof(tPlayerCountMessage));
}





void SGameSendDungeonFinderCompletionTimeToPlayer(Player & tPlayer)
{
    if (GameIsWeeklyChallenge())
    {
        SGameGlobals & tSGameGlobals = SGameGlobalsGet();
        DungeonFinderCompletionTimeMessage tCompletionTimeMessage;
        tCompletionTimeMessage.nCompletionTimeMessage = tSGameGlobals.tChallengeRift.tDungeonFinderOverrides.nWeeklyChallengeTargetMilliseconds;
        tCompletionTimeMessage.nPlayerIndex = tSGameGlobals.tChallengeRift.nInstigatorIndex;
        PlayerList listDestPlayers(PlayerListGetElementPool());
        listDestPlayers.AddTail_Tracked(&tPlayer);
        MessageSendToClient(listDestPlayers, SMSG_DUNGEON_FINDER_COMPLETION_TIME, &tCompletionTimeMessage, sizeof(tCompletionTimeMessage));
    }
}






void SGameUpdateMonsterLevel(int32 nPossibleNewLevel, BOOL bForceDownscale)
{
    const int32 MAX_LEVEL_DIFFERENCE = 70;

    GameCommonData & tGCD = GameGetActiveGameCommonData();
    int32 nCurrentLevel = tGCD.tGameSyncedData.nMonsterLevel;
    if (nCurrentLevel == -1 || (nPossibleNewLevel > nCurrentLevel && nPossibleNewLevel <= nCurrentLevel + MAX_LEVEL_DIFFERENCE) || (bForceDownscale && nPossibleNewLevel < nCurrentLevel))
    {
        tGCD.tGameSyncedData.nMonsterLevel = nPossibleNewLevel;


        for (Player * ptPlayer = PlayerGetFirstInGame(); ptPlayer; ptPlayer = PlayerGetNextInGame(ptPlayer))
            SGameSyncData(ptPlayer);
    }
}












































void SGameUpdateDifficultyLevel(Player & tRequestingPlayer, int32 nNewDifficultyLevel)
{


    if (nNewDifficultyLevel >= 0 && nNewDifficultyLevel <= 19)
    {
        GameCommonData & tGCD = GameGetActiveGameCommonData();
        int32 nOldDifficultyLevel = tGCD.nHandicapLevel;
        tGCD.nHandicapLevel = nNewDifficultyLevel;






        Player * ptPlayer = PlayerGetFirstInGame();
        while (ptPlayer)
        {
            ActorCommonData * ptACDPlayer = ACDTryToGet(ptPlayer->idPlayerACD);
            if (ptACDPlayer)
            {
                SHero::SetHandicapValues(*ptACDPlayer);
            }






            ptPlayer = PlayerGetNextInGame(ptPlayer);
        }

        ActorsAdjustForDifficultyChanged(nOldDifficultyLevel, nNewDifficultyLevel);


        PlayerList listDestPlayers(PlayerListGetElementPool());
        PlayerListGetAllInGame(listDestPlayers, -1, FALSE, -1);
        IntDataMessage tMessage;
        tMessage.nData = nNewDifficultyLevel;

        MessageSendToClient(listDestPlayers, SMSG_DIFFICULTY_CHANGED, &tMessage, sizeof(tMessage));
    }














}




BOOL SGameGlobals::WaypointIsActive(int32 nWaypoint)
{
    if (GameIsOpenWorld())
        return TRUE;
    const CachedWaypointArray & tWaypoints = ActGetCachedWaypointArray();
    int32 nCount = (int32)tWaypoints.size();
    if (nWaypoint < 0 || nWaypoint >= nCount)
        return FALSE;
    int32 nActivatedWaypoints = CountSetBits((const uint8 *)&dwActivatedWaypoints, sizeof(dwActivatedWaypoints));
    if (nActivatedWaypoints < 2)
        return FALSE;
    return (dwActivatedWaypoints >> nWaypoint) & 1;
}

















void SGameGlobals::WaypointEnable(int32 nWaypoint, BOOL bState)
{
    if (GameIsOpenWorld())
        return;
    const CachedWaypointArray & tWaypoints = ActGetCachedWaypointArray();
    int32 nCount = (int32)tWaypoints.size();
    if (nWaypoint < 0 || nWaypoint >= nCount)
        return;
    if (bState)
        dwActivatedWaypoints |= (1 << nWaypoint);
    else
        dwActivatedWaypoints &= ~(1 << nWaypoint);
}




















void SGameSetCommunityBuffDoubleGoblins(BOOL bActive)
{
    SGame & tSGame = SGameManager::GameGet();
    if (bActive)
        tSGame.ptSGameGlobals->dwFlags2 |= (1 << SGAMEGLOBALS2_COMMUNITY_BUFF_DOUBLE_GOBLINS);
    else
        tSGame.ptSGameGlobals->dwFlags2 &= ~(1 << SGAMEGLOBALS2_COMMUNITY_BUFF_DOUBLE_GOBLINS);
}

BOOL SGameCommunityBuffDoubleGoblinsActive()
{
    SGame & tSGame = SGameManager::GameGet();
    return (tSGame.ptSGameGlobals->dwFlags2 & (1 << SGAMEGLOBALS2_COMMUNITY_BUFF_DOUBLE_GOBLINS)) != 0;
}




BOOL SGameIsBonusDoubledMonster(SNO snoActor)
{
    SGame & tSGame = SGameManager::GameGet();
    if (tSGame.ptSGameGlobals->dwFlags2 & (1 << SGAMEGLOBALS2_COMMUNITY_BUFF_DOUBLE_GOBLINS))
    {
        SNO snoMonster = ActorGetMonsterSNO(snoActor);
        if (snoMonster != SNO_NONE)
        {
            ScopedMonsterDefinitionLockR pdefMonster(snoMonster);
            return pdefMonster && pdefMonster->eMonsterFamily == MF_TREASURE_GOBLIN;
        }
    }
    return FALSE;
}


extern XVarFloat s_varBlizzconRiftTier1RewardDropChance;
extern XVarFloat s_varBlizzconRiftTier2RewardDropChance;

void SGameUpdateServerVariables()
{
    SGame & tSGame = SGameManager::GameGet();

    tSGame.ptSGameGlobals->flBlizzconRiftTier1RewardDropChance = s_varBlizzconRiftTier1RewardDropChance.Get();
    tSGame.ptSGameGlobals->flBlizzconRiftTier2RewardDropChance = s_varBlizzconRiftTier2RewardDropChance.Get();
}
