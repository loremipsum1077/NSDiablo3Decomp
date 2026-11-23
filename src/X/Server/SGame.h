// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/SGame.h
// Subset: SGame's world array. The other members are padding (invented names) until someone
// needs them; the full reference header pulls in most of the server.

#pragma once
#include "../Sigma/Stopwatch.h"
#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/Game.h"
#include "../Common/GameCommonData.h"
#include "../Common/OnlineService/OnlineService.h"
#include "../Common/OnlineService/OnlineServiceErrorCodes.h"
#include "../Common/SNO/SNOFiles.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../Sigma/DataStructures/XGrowableMapTS.h"
#include "../Sigma/DataStructures/XGrowableMap.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../Sigma/DataStructures/XGrowableList.h"
#include "../Sigma/Random.h"
#include "../Sigma/DataStructures/XVariant.h"
#include "OnlineService/OnlineServiceServer.h"
#include "OnlineService/PlayerResolver.h"
#include "../../Contrib/Contrib/blz/include/blz/func_poly.h"
#include "Definitions/DungeonFinderDefinition.h"
#include "../Common/Definitions/HeroDefinition.h"

namespace OnlineService { class AchievementsInterface; }
struct SWorld;
struct SScene;
struct SSceneGlobals;
struct AIStateGameGlobals;
struct PlayerListGlobals;
struct PowerGlobals;
struct BrainStanceGlobals;
struct BrainComponentGlobals;
struct ActorPowerGlobals;
struct SActGlobals;
struct STalkGlobals;
struct STimedEventGlobals;
struct TrickleGlobals;
struct ConversationMgrGlobals;
struct KnownEntryGlobals;
struct ServerPlayerGlobals;
struct SActTransitionGlobals;
struct SpawnGroupExactInstance;
struct BrainTranslateGlobals;
struct GameDebugGlobals;
struct TagMapPools;
class XMemoryAllocatorInterface;
class XSphereNode;
template <typename T> class XListMemoryPool;
namespace Minimap { struct MiniMapMarkerGlobals; }
namespace NephalemRiftManager { class NephalemRiftState; }
namespace BossEncounterManager { class BossEncounterState; }
namespace SetEncounterManager { class SetEncounterState; }
namespace VoteKick { struct VoteKickState; }

struct ChallengeRiftData {  // size 0xe0
    uint32 nChallengeNumber;  // 0x00
    bool bChallengeOverriden;  // 0x04
    DungeonFinderOverrides tDungeonFinderOverrides;  // 0x08
    int32 nCompletionMilliseconds;  // 0xd4
    int32 nEndTime;  // 0xd8
    int32 nInstigatorIndex;  // 0xdc
};
static_assert(sizeof(ChallengeRiftData) == 0xe0, "ChallengeRiftData");
// Added for LevelAreaGetLocalizedName; the other members of SGameGlobals stay padding.
struct DungeonFinderData {  // size 0x50
    SNO snoDungeonFinderEngine;  // 0x00
    uint32 dwInitialSeed;  // 0x04
    RNG tDungeonFinderRNG;  // 0x08
    float flDungeonFinderProgress;  // 0x10
    SNO snoDungeonFinderTimedEvent;  // 0x14
    SNOPooledList listAvailableLevelAreas;  // 0x18
    SNO snoBossSpawned;  // 0x40
    int32 nDeaths;  // 0x44
    int32 nBossSpawnFloor;  // 0x48
    void Initialize() {
        snoDungeonFinderEngine = SNO_NONE;
        dwInitialSeed = 0;
        tDungeonFinderRNG.dwSeed = 0;
        tDungeonFinderRNG.dwCarry = 0;
        flDungeonFinderProgress = 0.0f;
        snoDungeonFinderTimedEvent = SNO_NONE;
        snoBossSpawned = SNO_NONE;
        nDeaths = 0;
        nBossSpawnFloor = -1;
    }
};
static_assert(sizeof(DungeonFinderData) == 0x50, "DungeonFinderData");

struct ReturnPointData {  // size 0xc
    SNO snoWorld;  // 0x00
    int32 nNum;  // 0x04
    ACDID idACDReturnPortal;  // 0x08
    void Reset();
    BOOL IsValid() const;  // line 200
};
static_assert(sizeof(ReturnPointData) == 0xc, "ReturnPointData");

// line 200
inline BOOL ReturnPointData::IsValid() const {
    return idACDReturnPortal != ACDID_INVALID;
}

class DPSTrackingInfo {  // size 0x8
public:
    DPSTrackingInfo();
    void SetWindowSeconds(float flSeconds);
    uint32 m_uWindowTicks;  // 0x00
    float m_flOOWindowSecs;  // 0x04
};
static_assert(sizeof(DPSTrackingInfo) == 0x8, "DPSTrackingInfo");

class GamePerformanceTrackingInfo {  // size 0xc
public:
    void Reset() { m_uScenesActivatedThisIteration = 0; m_uScenesDeactivatedThisIteration = 0; m_uACDsCreatedThisIteration = 0; }  // SGame.h:125 (in-class body, line table col 55)
    uint32 m_uScenesActivatedThisIteration;  // 0x00
    uint32 m_uScenesDeactivatedThisIteration;  // 0x04
    uint32 m_uACDsCreatedThisIteration;  // 0x08
};
static_assert(sizeof(GamePerformanceTrackingInfo) == 0xc, "GamePerformanceTrackingInfo");

enum GameSignalType {
    GAST_REMOVE_PLAYER = 0,
    GAST_GAME_LEADER_STATE_CHANGE = 1,
    GAST_PLAYER_ACHIEVEMENT = 2,
    GAST_ACHIEVEMENTS_INTERFACE_AVAILABILITY = 3,
    GAST_PLAYER_CLAN_ID_UPDATE = 4,
    GAST_PERFORM_ESCROW = 5,
    GAST_ENTITLEMENTS_GRANTED = 6,
    GAST_CLEAR_PENDING_CONSUMABLE_SAVE = 7,
    GAST_EXTERNAL_REWARD_GRANTED = 8,
    GAST_GET_PLATINUM_BALANCE = 9,
    GAST_CONQUEST_POSTED = 10,
    GAST_PARTY_CHANNEL_ID = 11,
    GAST_COUNT = 12,
};

struct GameSignal {  // size 0x48
    OnlineService::GameAccountHandle tAccountId;  // 0x00
    GameSignalType eSignal;  // 0x0c
    XVariant tContext;  // 0x10
    OnlineServiceServer::ProtoMessageCallback tCb;  // 0x40
};
static_assert(sizeof(GameSignal) == 0x48, "GameSignal");

struct ExternalGameSignals {  // size 0xc0
    uint32 uGameManagerSignalFlags;  // 0x00
    int32 nShutdownWarningTime;  // 0x04
    int32 eShutdownType;  // 0x08 (ServerShutdownType)
    uint8 _unadopted0[0x10 - 0xc];
    XGrowableList<int, int> tDisconnectedSocketList;  // 0x10
    XGrowableList<GameSignal, const GameSignal &> tGameSignalsList;  // 0x68
};
static_assert(sizeof(ExternalGameSignals) == 0xc0, "ExternalGameSignals");

// Original update queue layout, required by ActorAI::tAIUpdateData.
enum UpdateFunctionType {
    UPDATE_TIMERS = 0,
    UPDATE_POWER = 1,
    UPDATE_QUESTS = 2,
    UPDATE_ACD_MOVEMENT = 3,
    UPDATE_ACTOR_AI = 4,
    UPDATE_ACTOR_ATTRIBUTES = 5,
    UPDATE_GIZMOS = 6,
    UPDATE_FLIPPIES = 7,
    UPDATE_PLAYERS = 8,
    UPDATE_CONDUCTORS = 9,
    UPDATE_TIMEDEVENTS = 10,
    UPDATE_ACTTRANSITIONS = 11,
    UPDATE_STATS = 12,
    UPDATE_CONVERSATIONS = 13,
    UPDATE_SCRIPTED_POWERS = 14,
    UPDATE_OPEN_WORLD_TRANSITION = 15,
    UPDATE_BOUNTY_SCENE_ACTIVATION = 16,
    UPDATE_GAME_COMMUNITY_BUFFS = 17,
    UPDATE_ACTOR_AREA_DAMAGE = 18,
    NUM_UPDATE_FUNCTION_TYPE = 19,
};

struct UpdateData {  // size 0x20
    UpdateFunctionType nUpdateFunctionType;  // 0x00
    DataID idUpdate;  // 0x04
    uint32 dwGameTime;  // 0x08
    POS posQueue;  // 0x10
    int32 nCallsSinceQueueWrap;  // 0x18
};
static_assert(sizeof(UpdateData) == 0x20, "UpdateData");

typedef XGrowableMap<int, int, int, int> ConversationVarMap;  // SGame.h:256

struct ConversationRandomLineKey {  // size 0x8 (SGame.h:265)
    SNO snoConversation;  // 0x00
    uint32 dwLineUID;  // 0x04
    void Set(const SNO _snoConv, const uint32 _dwLineUID) { snoConversation = _snoConv; dwLineUID = _dwLineUID; }  // SGame.h:278
    BOOL operator==(const ConversationRandomLineKey & tKey) const { return tKey.snoConversation == snoConversation && tKey.dwLineUID == dwLineUID; }  // SGame.h:286
    ConversationRandomLineKey & operator=(const ConversationRandomLineKey & tKey) { Set(tKey.snoConversation, tKey.dwLineUID); return *this; }  // SGame.h:308
};
static_assert(sizeof(ConversationRandomLineKey) == 0x8, "ConversationRandomLineKey");

template <>
inline uint32 XHashKey<const ConversationRandomLineKey &>(const ConversationRandomLineKey & tKey) {  // SGame.h:316
    return (tKey.snoConversation & 0xFFFFFF) | (tKey.dwLineUID << 24);
}

typedef XGrowableMap<ConversationRandomLineKey, const ConversationRandomLineKey &, int, int> ConversationRandomLineMap;  // SGame.h:236

struct PauseMessage {  // size 0x8 (SGame.h, DWARF)
    GameMessageType eType;  // 0x00
    uint32 nPlayerIndex;  // 0x04
};
static_assert(sizeof(PauseMessage) == 0x8, "PauseMessage");

struct MatchmakingData {  // size 0x30 (SGame.h)
    uint32 dwGameMadePublicTick;  // 0x00
    uint32 dwLastPlayerLeaveTick;  // 0x04
    BOOL bNephalemValorBuffGranted;  // 0x08
    BOOL bMatchmakingServerState;  // 0x0c
    CHAR szGameTag[16];  // 0x10
    CHAR szServerPool[16];  // 0x20
};
static_assert(sizeof(MatchmakingData) == 0x30, "MatchmakingData");

struct CCInfo;  // X/Server/Powers/PowerAPI.h
typedef XGrowableMap<ACDID, ACDID, CCInfo, CCInfo &> mapPendingCCMessages;  // SGame.h:252

struct SGameGlobals {  // size 0x2268
    BOOL WaypointIsActive(int32 nWaypoint);
    void WaypointEnable(int32 nWaypoint, BOOL bState);
    OnlineService::AchievementsInterface * ptAchievementsInterface;  // 0x00
    ExternalGameSignals atExternalSignals[2];  // 0x08
    ExternalGameSignals * ptActiveSignals;  // 0x188
    ExternalGameSignals * ptInactiveSignals;  // 0x190
    uint32 dwFlags;  // 0x198
    uint32 dwFlags2;  // 0x19c
    uint32 dwGameHash;  // 0x1a0
    uint32 dwGameStartAppTime;  // 0x1a4
    uint32 dwLastAppTimeWithPlayer;  // 0x1a8
    uint32 dwLastAppTimeInput;  // 0x1ac
    uint32 dwUniqueGameKey;  // 0x1b0
    uint32 dwGameEventNumber;  // 0x1b4
    int64 tUTCGameStartTime;  // 0x1b8 (Blizzard::Time::Timestamp)
    Stopwatch tStopwatch;  // 0x1c0
    RNG rng;  // 0x1c8
    RNG rngSyncedAnim;  // 0x1d0
    int32 nBoostLevel;  // 0x1d8
    int32 nUpdatesLeftOver;  // 0x1dc
    int32 nMaxPlayers;  // 0x1e0
    int32 nMonstersKilled;  // 0x1e4
    SNO snoInitialWorld;  // 0x1e8
    SNO snoInitialQuest;  // 0x1ec
    int32 nInitialQuestStepUID;  // 0x1f0
    uint32 uEntranceGUIDOverride;  // 0x1f4
    BOOL bResumeFromSave;  // 0x1f8
    SavePointData tCreatorSavePoint;  // 0x1fc
    ReturnPointData tReturnPoint;  // 0x20c
    ACDID idACDReturnPointPortal;  // 0x218
    MatchmakingData tMatchmakingData;  /* 0x21c */  uint8 _unadopted5b[0x250 - 0x24c];  // (invented name: 0x24c .. 0x250)
    DungeonFinderData tDungeonFinderData;  // 0x250
    uint32 uNextDynamicEntranceGUID;  // 0x2a0
    SNO snoOpenWorldHubWorld;  // 0x2a4
    uint32 uOpenWorldHubEntranceGUID;  // 0x2a8
    UpdateData tOpenWorldUpdateData;  // 0x2b0
    struct SGamePrivateGlobals * ptSGamePrivate;  // 0x2d0
    CHAR szGameName[4096];  // 0x2d8
    BOOL bGameCreatorInfoSet;  // 0x12d8
    OnlineService::HeroId uHeroIdCreator;  // 0x12e0
    CHAR uszCreatorHeroName[49];  // 0x12e8
    CHAR uszCreatorAccountName[128];  // 0x1319
    uint32 dwActivatedWaypoints;  // 0x139c
    SNOPooledList listCompletedBossEncounters;  // 0x13a0
    uintptr uDatabaseSerializeToken;  // 0x13c8
    uint32 dwNextBNetGameUpdate;  // 0x13d0
    uint32 dwPlayerCounter;  // 0x13d4
    uint32 dwLastChampRareUniqueKilled;  // 0x13d8
    uint32 uTicksSinceLastMessage;  // 0x13dc
    uint32 uPandemoniumFlags;  // 0x13e0
    int32 nLargestPVPScoreDeficitTeam0;  // 0x13e4
    int32 nLargestPVPScoreDeficitTeam1;  // 0x13e8
    DPSTrackingInfo tDPSTrackingInfo;  // 0x13ec
    GamePerformanceTrackingInfo tPerformanceTrackingInfo;  // 0x13f4
    uint32 uNextServerVariableCheckTick;  // 0x1400
    uint32 uNextServerPartyGuideCheckTick;  // 0x1404
    PauseMessage pauseMessages[10];  // 0x1408
    uint32 dwPauseMessageDropCount;  // 0x1458
    uint32 dwLoggingOutMessageDropCount;  // 0x145c
    char szLogHistory[10][80];  // 0x1460
    uint32 nLogHistoryCurrentIndex;  // 0x1780
    Blizzard::Time::Millisecond tSimTimes[512];  // 0x1784
    Blizzard::Time::Millisecond tSimTimesSpikes;  // 0x1f84
    Blizzard::Time::Millisecond tSimTimeSum;  // 0x1f88
    uint32 tSimTimePos;  // 0x1f8c
    ConversationVarMap tConversationVarMap;  // 0x1f90
    ConversationRandomLineMap tConversationRandomLineMap;  // 0x2010
    uint8 dwKeyWardenKilledFlags;  // 0x2090
    mapPendingCCMessages tMapPendingCCMessages;  // 0x2098
    uint64 tAchievementTime;  // 0x2118 (OnlineService::Timestamp)
    ACDID idGreedPortal;  // 0x2120
    float flBlizzconRiftTier1RewardDropChance;  // 0x2124
    float flBlizzconRiftTier2RewardDropChance;  // 0x2128
    uint8 _unadopted6g[0x2130 - 0x212c];  // (invented name: 0x212c .. 0x2130)
    UpdateData tCommunityBuffsUpdateData;  // 0x2130
    uint32 dwLastTimeVerboseSlowFrameLog;  // 0x2150
    uint32 dwTimeBetweenVerboseSlowFrameLogs;  // 0x2154
    uint32 dwVerboseSlowFrameLogMaxPowersPerLog;  // 0x2158
    uint8 _unadopted6h[0x2160 - 0x215c];  // (invented name: 0x215c .. 0x2160)
    XPooledList<struct PacketBuffer *, struct PacketBuffer *> tOrphanedPacketBufferList;  // 0x2160 (TPacketBufferList)
    ChallengeRiftData tChallengeRift;  // 0x2188
};
static_assert(sizeof(SGameGlobals) == 0x2268, "SGameGlobals");
static_assert(__builtin_offsetof(SGameGlobals, dwActivatedWaypoints) == 0x139c, "SGameGlobals::dwActivatedWaypoints");
static_assert(__builtin_offsetof(SGameGlobals, uDatabaseSerializeToken) == 0x13c8, "SGameGlobals::uDatabaseSerializeToken");
static_assert(__builtin_offsetof(SGameGlobals, nLogHistoryCurrentIndex) == 0x1780, "SGameGlobals::nLogHistoryCurrentIndex");

BOOL SGameIsValid();  // SGame.cpp:4187
SGameGlobals & SGameGlobalsGet();  // SGame.cpp:4196

class PendingJoinMap : public XGrowableMapTS<unsigned long, OnlineService::PlayerResolveData> {  // size 0xa0
public:
    BOOL SetPlayerConnected(OnlineService::HeroId uHeroId, const ServerSocketID idServerSocket, const XLocale eLocale);
    BOOL SetPlayerWaitingOnAchievementRegistration(OnlineService::HeroId);
    BOOL SetPlayerAchievementRegistrationComplete(OnlineService::HeroId uHeroId, OnlineService::ErrorCode errorCode);
    BOOL SetPlayerWaitingOnWorldCreation(OnlineService::HeroId uHeroId, BOOL bWaiting);
    BOOL SetPlayerReplayJoinRecorded(OnlineService::HeroId uHeroId, BOOL bRecorded);
};
static_assert(sizeof(PendingJoinMap) == 0xa0, "PendingJoinMap");

typedef XGrowableMapTS<OnlineService::GameAccountHandle, int> GameAccountKickMap;

struct RefStringMemPool;

enum SGameSimState {  // line 334
    SGAME_SLEEPING = 0,
    SGAME_SIMULATING = 1,
};

struct ScriptEventGameGlobals;
struct LuaScriptGameGlobals;
struct ConductorGlobals;
struct ActorGlobals;
struct TimedEffectsGlobals;
struct ActorAIGlobals;
struct SWorldGlobals;
struct SActorMovementGameGlobals;
struct FlippyGlobals;
struct SGame {  // size 0x908
    SGameID id;  // 0x00
    SGameSimState eSimState;  // 0x04
    uint32 uPendingGameUpdateCount;  // 0x08
    uint32 uScheduledGameUpdateCount;  // 0x0c
    GameParams tStartParams;  // 0x10
    SGameGlobals * ptSGameGlobals;  // 0x3d0
    GameCommonData tGCD;  // 0x3d8
    uint8 _unadopted8[0x618 - 0x610];  // (invented name: 0x610 .. 0x618)
    ActorGlobals * ptActorGlobals;  // 0x618
    struct SpawnerGlobals * ptSpawnerGlobals;  // 0x620
    PowerGlobals * ptPowerGlobals;  // 0x628
    ActorPowerGlobals * ptActorPowerGlobals;  // 0x630
    TimedEffectsGlobals * ptTimedEffectsGlobals;  // 0x638
    ActorAIGlobals * ptActorAIGlobals;  // 0x640
    BrainComponentGlobals * ptBrainComponentGlobals;  // 0x648
    BrainStanceGlobals * ptBrainStanceGlobals;  // 0x650
    BrainTranslateGlobals * ptBrainTranslateGlobals;  // 0x658
    uint8 _unadopted6b[0x670 - 0x660];  // (invented name: 0x660 .. 0x670)
    struct ActorProjectileGlobals * ptActorProjectileGlobals;  // 0x670
    FlippyGlobals * ptFlippyGlobals;  struct HeroGlobals * ptHeroGlobals;  // 0x678, 0x680 (one line: later lines keep their numbers)
    struct LevelAreaGlobals * ptLevelAreaGlobals;  // 0x688
    PlayerListGlobals * ptPlayerListGlobals;  // 0x690
    STalkGlobals * ptSTalkGlobals;  // 0x698
    ServerPlayerGlobals * ptServerPlayerGlobals;  // 0x6a0
    KnownEntryGlobals * ptKnownEntryGlobals;  // 0x6a8
    TrickleGlobals * ptTrickleGlobals;  // 0x6b0
    uint8 _unadopted3c[0x6c0 - 0x6b8];  // (invented name: 0x6b8 .. 0x6c0)
    struct ServerGlobals * ptServerGlobals;  // 0x6c0
    struct TimersGlobals * ptTimersGlobals;  // 0x6c8
    TDataArray<SScene, int> * ptSScenes;  // 0x6d0
    SSceneGlobals * ptSSceneGlobals;  // 0x6d8
    TDataArray<SWorld, int> * ptSWorlds;  // 0x6e0
    SWorldGlobals * ptSWorldGlobals;  // 0x6e8
    uint8 _unadopted1[0x6f8 - 0x6f0];  // (invented name: 0x6f0 .. 0x6f8)
    SActGlobals * ptSActGlobals;  // 0x6f8
    struct SGameStatGlobals * ptSGameStatGlobals;  // 0x700
    struct AdventureGlobals * ptAdventureGlobals;  // 0x708
    struct SComplexEffectGlobals * ptSComplexEffectGlobals;  // 0x710
    STimedEventGlobals * ptSTimedEventGlobals;  // 0x718
    SActTransitionGlobals * ptSActTransitionGlobals;  // 0x720
    ConversationMgrGlobals * ptConversationMgrGlobals;  // 0x728
    BossEncounterManager::BossEncounterState * ptBossEncounterState;  // 0x730
    NephalemRiftManager::NephalemRiftState * ptNephalemRiftState;  // 0x738
    SetEncounterManager::SetEncounterState * ptSetEncounterState;  // 0x740
    VoteKick::VoteKickState * ptVoteKickState;  // 0x748
    AIStateGameGlobals * ptAIStateGameGlobals;  // 0x750
    SActorMovementGameGlobals * ptSActorMovementGlobals;  // 0x758
    Minimap::MiniMapMarkerGlobals * ptSMiniMapGlobals;  // 0x760
    LuaScriptGameGlobals * ptLuaScriptGlobals;  // 0x768
    ConductorGlobals * ptConductorGlobals;  // 0x770
    ScriptEventGameGlobals * ptScriptEventGameGlobals;  // 0x778
    struct ClientValidationGlobals * ptClientValidationGlobals;  // 0x780
    RefStringMemPool * ptRefStringMemPool;  // 0x788
    TagMapPools * ptTagMapPools;  // 0x790
    XListMemoryPool<int> * ptDATAIDMemPool;  // 0x798
    XListMemoryPool<const XSphereNode *> * ptSphereNodeMemPool;  // 0x7a0
    XListMemoryPool<int> * ptGBIDMemPool;  // 0x7a8
    XListMemoryPool<SpawnGroupExactInstance> * ptSpawnGroupExactPool;  // 0x7b0
    GameDebugGlobals * ptGameDebugGlobals;  // 0x7b8
    XMemoryAllocatorInterface * ptMemoryInterface;  // 0x7c0
    PendingJoinMap tPendingJoinMap;  // 0x7c8
    GameAccountKickMap tKickMap;  // 0x868
    static void OnSavedHeros(SGameID id, OnlineService::ErrorCode errCode);
};
static_assert(sizeof(SGame) == 0x908, "SGame");
static_assert(__builtin_offsetof(SGame, tGCD) == 0x3d8, "SGame::tGCD");
static_assert(__builtin_offsetof(SGame, ptRefStringMemPool) == 0x788, "SGame::ptRefStringMemPool");

typedef TDataArrayIterator<SGame> SGameIterator;  // line 422

// SGame.h:940 and :962. The bodies are not in the DWARF; they are written from the copies inlined
// into X/Common/Game.cpp (GameRandRangeInt, GameRandRangeFloat).
inline int32 ServerRandRangeInt(const int32 nMin, const int32 nMax) {  // line 940
    return RandRange_Internal(SGameGlobalsGet().rng, nMin, nMax - nMin);  // lines 944-945
}
inline float ServerRandRangeFloat(const float flMin, const float flMax) {  // line 962
    return RandRangeFloat_Internal(SGameGlobalsGet().rng, flMin, flMax - flMin);  // lines 966-967
}

// SGame.h:427-436 (struct) and :751 / :764 (constructor / destructor). The two bodies are not in the
// DWARF; they are written from the copy inlined into SACDInventoryBuy (ActorInventory.cpp:10014-10018):
// line 751 copy-constructs the callback, 754-756 save and replace the game RNG, 767/769 test and
// invoke the callback, 772 restores the RNG.
typedef blz::function<void ()> RNGOutOfScopeCallback;

struct ConsoleScopedOverrideRNG {  // size 0x30
    ConsoleScopedOverrideRNG(int32 dwSeed, int32 dwCarry, RNGOutOfScopeCallback Callback) : m_Callback(Callback) {  // line 751
        m_tSavedRNG = SGameGlobalsGet().rng;  // line 754
        SGameGlobalsGet().rng.dwSeed = dwSeed;  // line 755
        SGameGlobalsGet().rng.dwCarry = dwCarry;  // line 756
    }
    ~ConsoleScopedOverrideRNG() {  // line 764
        if (m_Callback) {  // line 767
            m_Callback();  // line 769
        }
        SGameGlobalsGet().rng = m_tSavedRNG;  // line 772
    }
private:
    ConsoleScopedOverrideRNG();
    RNG m_tSavedRNG;  // 0x00
    RNGOutOfScopeCallback m_Callback;  // 0x08
};
static_assert(sizeof(ConsoleScopedOverrideRNG) == 0x30, "ConsoleScopedOverrideRNG");

// Defined in X/Server/SGame.cpp (line numbers from the reference .cpp).
namespace OnlineService { struct PlayerResolveData; }
void SGameInitializeApp();  // the sgtAppSubsystems entry
void SGameDisposeApp();
SGameID SGameInitializeGame(GameParams & tParams, const OnlineService::PlayerResolveData * ptResolvedPlayer);  // SGame.cpp:781
void AppServerIdle(uint32 dwGameClockTicks);  // SGame.cpp:6280
SGameID AppServerGetOnlyGame();  // SGame.cpp:6360
BOOL SGameIsShuttingDown();  // SGame.cpp:5478
Act SGameGetCurrentMaxGameAct();  // SGame.cpp:7005
void SGameAdvanceActs();  // SGame.cpp:7027

static_assert(__builtin_offsetof(SGame, ptActorPowerGlobals) == 0x630, "SGame::ptActorPowerGlobals");
static_assert(__builtin_offsetof(SGame, ptSTimedEventGlobals) == 0x718, "SGame::ptSTimedEventGlobals");
static_assert(__builtin_offsetof(SGame, ptSActTransitionGlobals) == 0x720, "SGame::ptSActTransitionGlobals");


void ServerQueueUpdateFunction(int32 nTicksDelay, UpdateData & tUpdateData);  // SGame.cpp:1909
void UpdateFunctionInitialize(UpdateFunctionType nUpdateFunctionType, DataID idUpdate,
                              UpdateData & tUpdateData);  // SGame.cpp:1814
void ServerUpdateWasCalled(UpdateData & tUpdateData);  // SGame.cpp:2072

// More of X/Server/SGame.cpp (line numbers from the reference .cpp).
struct Player;
void SGameRefStringInitializeForGame();  // SGame.cpp:536
void SGameRefStringDisposeForGame();  // SGame.cpp:546
void SGameDisposeGame();  // SGame.cpp:1303
GameCommonData & SGameGetGameCommonData();  // SGame.cpp:4175
BOOL SGameTraceLootEnabled();  // SGame.cpp:4210
int32 SGameGetMaxPlayers();  // SGame.cpp:6146
RNG & SGameGetRNG();  // SGame.cpp:6157
SGameID AppServerGetFirstGame();  // SGame.cpp:6350
uint32 SGameGetEventNumber();  // SGame.cpp:6371
SNO SGameGetChallengeRiftHubWorld();  // SGame.cpp:6796
BOOL TraceStatEnabled();  // SGame.cpp:7239
void SGameStopServer(SGameID idSGame);  // SGame.cpp:7451
BOOL SGameIsPvPFlagSet();  // SGame.cpp:7499
void SGameStartGameEnd(Player & tPlayer);  // SGame.cpp:7618
void SGameSetCommunityBuffDoubleGoblins(BOOL bActive);  // SGame.cpp:8751
BOOL SGameCommunityBuffDoubleGoblinsActive();  // SGame.cpp:8760

enum {  // SGame.h:115
    SERVER_TICKS_PER_UPDATE = 6,
    MAX_TICKS_UPDATE_DELAY = 127,
    TICK_UPDATE_BUCKETS = 128,
    LOG_HISTORY_ENTRY_COUNT = 10,
    LOG_HISTORY_ENTRY_SIZE = 80,
};
void ServerRemoveUpdateFunction(UpdateData & tUpdateData);  // SGame.cpp:1829
BOOL SGameIsJoinAllowed(SGameID idSGame);  // SGame.cpp:4159
void SGameInitWaypointsFromStartData(BOOL bReset, BOOL bUpdateClients);  // SGame.cpp:5644
void SGameInitAnniversaryEventFlags();  // SGame.cpp:5711
BOOL DeleteAllWorldsAndTransferPlayersToLimbo();  // SGame.cpp:7147
void DeleteAllWorldsExceptLimbo();  // SGame.cpp:7197
void SGameSyncData(const Player * ptPlayer);  // SGame.cpp:7462
int32 SGameGetStatDataKey();  // SGame.cpp:8361
void SGameUpdateMonsterLevel(int32 nPossibleNewLevel, BOOL bForceDownscale);  // SGame.cpp:8568
BOOL SGameIsBonusDoubledMonster(SNO snoActor);  // SGame.cpp:8769

// line 192
inline void ReturnPointData::Reset() {
    snoWorld = SNO_NONE;
    nNum = -1;
    idACDReturnPortal = ACDID_INVALID;
}
static_assert(__builtin_offsetof(SGame, ptFlippyGlobals) == 0x678, "SGame::ptFlippyGlobals");
static_assert(__builtin_offsetof(SGame, ptHeroGlobals) == 0x680, "SGame::ptHeroGlobals");
