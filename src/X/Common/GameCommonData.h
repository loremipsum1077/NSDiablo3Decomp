// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/GameCommonData.h
// Subset: GameCommonData's synced data (up to bPVPGameOver), clock, flags and per-system globals
// pointers. The members before them (0x00-0x2f: difficulty, GameTemplate), the rest of
// GameSyncedData and the globals pointers after ptGBGlobals are not adopted yet; they are stood in
// for by invented placeholder arrays.

#pragma once

#include "GameSwig.h"

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "CommonTypes.h"
#include "Game.h"
#include "../Sigma/SigmaTypes.h"

struct ACDCollGlobals;
struct ActGlobals;
namespace Tutorial { struct TutorialGlobals; }
struct ACDGlobals;
struct ActorInventoryGlobals;
struct ActorCollisionGlobals;
struct AnimPlayerGlobals;
struct AttributeGlobals;
struct AttributeGroupGlobals;
struct AttributeListGlobals;
struct ChallengeRiftGlobals;
namespace CombatLog { struct CombatLogGlobals; }
struct FastAttribGameGlobals;
struct GameBalanceGlobals;
struct AnimPlayerGlobals;
struct PathfindingGlobals;
struct NavCellPathGlobals;
struct VisualInventoryGlobals;
struct PortalGlobals;
struct InactiveMarkerGlobals;
struct ItemGlobals;
struct MarkerGlobals;
struct NavMeshGlobals;
struct NavMeshStitchGlobals;
struct PathGameGlobals;
struct PlayerGlobals;
struct QuestGlobals;
struct SceneCommonGlobals;
struct TeamGlobals;

enum {
    GAMEMODE_NONE = 0,
    GAMEMODE_PLAYING = 1,
    GAMEMODE_EDITUNITLAYOUT = 2,
    GAMEMODE_EDITSCENE = 3,
    GAMEMODE_HOLODECK = 4,
    GAMEMODE_MENU = 5,
    GAMEMODE_COUNT = 6,
};

// GameCommonData.h:63
enum GameSyncedFlags {
    GAME_SYNCED_FLAGS_PVP = 0,
    GAME_SYNCED_FLAGS_SEASONAL = 1,
    GAME_SYNCED_FLAGS_BATTLE_PAY = 2,
    GAME_SYNCED_FLAGS_ANNIVERSARY_DUNGEON = 3,
    GAME_SYNCED_FLAGS_COUNT = 4,
};

typedef int32 DifficultyLevel;  // line 129

BOOL GameIsWeeklyChallenge();  // GameCommonData.cpp

struct GameSyncedData {  // size 0xd0
    uint32 dwGameSyncedFlags;  // 0x00
    Act eAct;  // 0x04
    int32 nMonsterLevel;  // 0x08 (2.7.7: the 2.6.2 field nInitialMonsterLevel is gone; GameGetInitialMonsterLevel is not in the binary)
    int32 nRandomWeatherSeed;  // 0x0c
    int32 eOpenWorldMode;  // 0x10 (OpenWorldMode)
    Act eOpenWorldModeAct;  // 0x14
    int32 nOpenWorldModeParam;  // 0x18
    uint32 dwOpenWorldTransitionTime;  // 0x1c
    Act eOpenWorldDefaultAct;  // 0x20
    Act eOpenWorldBonusAct;  // 0x24
    int32 snoDungeonFinderLevelArea;  // 0x28 (SNO)
    BOOL bLootRunOpen;  // 0x2c
    int32 nOpenLootRunLevel;  // 0x30
    BOOL bLootRunBossDead;  // 0x34
    int32 nHunterPlayerIdx;  // 0x38
    BOOL bLootRunBossActive;  // 0x3c
    BOOL bTieredLootRunFailed;  // 0x40
    BOOL bLootRunChallengeCompleted;  // 0x44
    BOOL bSetDungeonActive;  /* 0x48 */  int32 unk_4c;  // 0x4c (2.7.7: new; zeroed by GameCommonData::Initialize, set from sub_71007d5160)
    int32 arPlayerAllyTeam[4];  // 0x50 (Team)
    int32 arMonsterTeam[4];  // 0x60 (Team)
    int32 eSceneryTeam;  // 0x70 (Team)
    int32 eNeutralTeam;  // 0x74 (Team)
    BOOL bPregame;  // 0x78
    uint32 dwPregameEnd;  // 0x7c
    uint32 dwRoundStart;  // 0x80
    uint32 dwRoundEnd;  // 0x84
    BOOL bPVPGameOver;  // 0x88
    BOOL bPVPAwardsAssigned;  // 0x8c
    uint32 nTeamWins[2];  // 0x90
    uint32 nTeamScore[2];  // 0x98
    int32 ePVPGameResult[2];  // 0xa0 (PVPGameResult)

    uint64 uPartyGuideHeroId;  // 0xa8 (OnlineService::HeroId)
    uint64 arTieredRiftParticipatingHeroID[4];  // 0xb0
};
static_assert(sizeof(GameSyncedData) == 0xd0, "GameSyncedData");

struct ItemWeightInfo {  // size 0x8 (line 126)
    int32 nItemWeight;  // 0x00
    int32 nItemIndex;  // 0x04
};

class GameCommonData {  // size 0x238
public:
    DifficultyLevel eDifficultyLevel;  // 0x00
    int32 nHandicapLevel;  // 0x04
    uint32 dwDifficultyChangedFlags;  // 0x08
    uint32 nNumPlayers;  // 0x0c
    GameType eGameType;  // 0x10
    GameTemplate tTemplate;  // 0x18
    GameSyncedData tGameSyncedData;  // 0x30
    GameClock tGameClock;  // 0x100
    uint32 dwServerTime;  // 0x120
    int nGameFlagsSize;  // 0x124
    uint8 * arGameFlags;  // 0x128
    MarkerGlobals * ptMarkerGlobals;  // 0x130
    InactiveMarkerGlobals * ptInactiveMarkerGlobals;  // 0x138
    PlayerGlobals * ptPlayerGlobals;  // 0x140
    SceneCommonGlobals * ptSceneCommonGlobals;  // 0x148
    ItemGlobals * ptItemGlobals;  // 0x150
    NavMeshGlobals * ptNavMeshGlobals;  // 0x158
    NavMeshStitchGlobals * ptNavMeshStitchGlobals;  // 0x160
    AttributeGroupGlobals * sgptAttributeGroupGlobals;  // 0x168
    AttributeListGlobals * sgptAttributeListGlobals;  // 0x170
    AttributeGlobals * sgptAttributeGlobals;  // 0x178
    FastAttribGameGlobals * ptFastAttribGameGlobals;  // 0x180
    TeamGlobals * ptTeamGlobals;  // 0x188
    PathGameGlobals * ptPathGlobals;  // 0x190
    ACDGlobals * ptACDGlobals;  // 0x198
    ActorCollisionGlobals * ptActorCollisionGlobals;  // 0x1a0
    ACDCollGlobals * ptACDCollGlobals;  // 0x1a8
    GameBalanceGlobals * ptGBGlobals;  // 0x1b0
    QuestGlobals * ptQuestGlobals;  // 0x1b8
    AnimPlayerGlobals * ptAnimPlayerGlobals;  // 0x1c0
    PathfindingGlobals * ptPathFindingGlobals;  // 0x1c8
    NavCellPathGlobals * ptNavCellPathGlobals;  // 0x1d0
    ActorInventoryGlobals * ptActorInventoryGlobals;  // 0x1d8
    VisualInventoryGlobals * ptVisualInventoryGlobals;  // 0x1e0
    PortalGlobals * ptPortalGlobals;  // 0x1e8
    ItemWeightInfo * arItemWeights;  // 0x1f0
    struct GameTestingGlobals * ptGameTestingGlobals;  // 0x1f8
    CombatLog::CombatLogGlobals * ptCombatLogGlobals;  // 0x200
    Tutorial::TutorialGlobals * ptTutorialGlobals;  // 0x208
    ActGlobals * ptActGlobals;  // 0x210
    ChallengeRiftGlobals * ptChallengeRiftGlobals;  // 0x218
    CHAR * szLootTraceBuffer;  // 0x220
    struct ReplayGlobals * ptReplayGlobals;  // 0x228
    void Initialize(const GameParams & tParams);
    void Dispose();
    GameMode GameModeGet() const;
    void GameModeSet(const GameMode _eMode);
private:
    GameMode eMode;  // 0x230
};
static_assert(__builtin_offsetof(GameCommonData, ptACDGlobals) == 0x198, "GameCommonData");
static_assert(sizeof(GameCommonData) == 0x238, "GameCommonData");

// Declared here from its definition in GameCommonData.cpp.
uint32 GameGetServerTime();  // GameCommonData.cpp:292

SNO ActSNOFromActEnum(const Act eAct); // GameCommonData.cpp:415

// GameCommonData.h:160, the game-type values used by GameTemplate.cpp.
enum {
    GT_NOTHING = -1,
    GT_STARTUP = 0,
    GT_DIABLO3 = 1,
    GT_PVP_GAME_FIRST = 2,
    GT_PVP_DOTA = 2,
    GT_PVP_ARENA = 3,
    GT_PVP_TEAMDEATHMATCH = 4,
    GT_PVP_FREEFORALL = 5,
    GT_PVP_GAME_LAST = 5,
    GT_CHALLENGE_RIFTS = 6,
    GT_COUNT = 7,
};

// Defined in GameCommonData.cpp:302 and GameCommonData.cpp:311.
Act GameGetCurrentAct();
BOOL GameIsOpenWorld();
OpenWorldMode GameGetOpenWorldMode();  // GameCommonData.cpp:334
Act GameGetOpenWorldModeAct();  // GameCommonData.cpp:343

// GameCommonData.h:183, recovered from the raw DWARF enum.
enum { DIFFICULTY_LOWERED_BIT = 0, DIFFICULTY_RAISED_BIT = 1 };
