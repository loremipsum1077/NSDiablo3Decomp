// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/SWorld.h

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/CommonTypes.h"
#include "../Common/World.h"
#include "SGame.h"
#include "NoSpawnTree.h"
#include "SGameManager.h"
#include "Map/Minimap.h"
#include "../Sigma/DataStructures/XPooledMap.h"
#include "../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../Contrib/Contrib/Domino/Include/dmId.h"
#include "../Common/Definitions/ActDefinition.h"
#include "../Common/WorldPlace.h"
#include "../Sigma/DataStructures/XGrowableList.h"
#include "../Sigma/DataStructures/XGrowableMap.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "../Sigma/Math/Quaternion.h"
#include "../Sigma/Math/Vector.h"

struct Player;

namespace D3 { namespace Hero { class SavedDefinition; } }
namespace D3 { namespace Account { class SavedDefinition; } }
namespace D3 { namespace Items { class Mails; } }

typedef uint32 StringLabelHandle;  // X/Common/Labels.h

struct SpawnGroupLocation {  // size 0x8
    int32 nDRLGIndex;  // 0x00
    int32 nNodeCount;  // 0x04
};
static_assert(sizeof(SpawnGroupLocation) == 0x8, "SpawnGroupLocation");

struct SpawnGroupExactInstance {  // size 0x18
    SpawnGroupLocation tLocation;  // 0x00
    SNO snoLevelAreaOrig;  // 0x08
    SNO snoLevelAreaSpawn;  // 0x0c
    int32 nPopIndex;  // 0x10
    int32 nGroupIndex;  // 0x14
};
static_assert(sizeof(SpawnGroupExactInstance) == 0x18, "SpawnGroupExactInstance");

enum {
    SWORLDMODIFIED_WORLD_STATUS = 0,
    SWORLDMODIFIED_NUM_TYPES = 1,
};

typedef int32 SWorldModifiedType;

enum {
    SWORLD_CREATE_PRELOADING = 0,
    SWORLD_CREATE_START = 1,
    SWORLD_CREATE_WORLD_INSTANCES = 2,
    SWORLD_CREATE_DRLG = 3,
    SWORLD_CREATE_SPECIAL_MARKERS = 4,
    SWORLD_CREATE_FINALIZE = 5,
    SWORLD_CREATE_WAIT_PREFETCH = 6,
    SWORLD_CREATE_COMPLETED = 7,
    SWORLD_CREATE_COUNT = 8,
};

typedef XListMemoryPool<SpawnGroupExactInstance> TSpawnGroupExactInstancePool;
typedef XPooledList<SpawnGroupExactInstance, const SpawnGroupExactInstance &> TPooledSpawnGroupExactInstanceList;
typedef XGrowableMap<int, int, int, int> TGizmoLocSpawnMap;

struct EntranceLocation {  // size 0xc
    SSceneID idSScene;  // 0x00
    SNO snoMarker;  // 0x04
    uint32 tName;  // 0x08
};
static_assert(sizeof(EntranceLocation) == 0xc, "EntranceLocation");

struct SavePointLocation {  // size 0x18
    WorldPlace placeSavePoint;  // 0x00
    SNO snoMarker;  // 0x10
    uint32 tName;  // 0x14
};
static_assert(sizeof(SavePointLocation) == 0x18, "SavePointLocation");

struct TownPortalLocation {  // size 0x14
    WorldPlace wpPortal;  // 0x00
    SNO snoScene;  // 0x10
};
static_assert(sizeof(TownPortalLocation) == 0x14, "TownPortalLocation");

struct ReturnPortalLocation {  // size 0x14
    WorldPlace placeReturn;  // 0x00
    SSceneID idSScene;  // 0x10
};
static_assert(sizeof(ReturnPortalLocation) == 0x14, "ReturnPortalLocation");

typedef XGrowableMap<unsigned int, unsigned int, EntranceLocation, EntranceLocation &> EntranceLocationMap;
typedef XGrowableMap<unsigned int, unsigned int, SavePointLocation, SavePointLocation &> SavePointLocationMap;

enum SWorldFlags {
    FLAG_SWORLD_HAS_BEEN_ENTERED = 0,
    FLAG_SWORLD_IS_HUB = 1,
    FLAG_SWORLD_FROM_DEATH_PORTAL = 2,
};

struct UniqueMonsterActorEntry {  // size 0xc
    SNO snoMonsterActor;  // 0x00
    SNO snoLevelArea;  // 0x04
    int32 nDRLGIndex;  // 0x08
};
static_assert(sizeof(UniqueMonsterActorEntry) == 0xc, "UniqueMonsterActorEntry");

class PlayerList;

struct SWorld {  // size 0x450
    BOOL IsModifiedAny() const;
    BOOL IsModifiedSpecific(const SWorldModifiedType eModifiedType) const;
    void ModifiedClear();
    void ModifiedMark(const SWorldModifiedType);
    void SendSegmentIfModified(const PlayerList & listDestPlayers, const SWorldModifiedType eModifiedType);
    void SendServerUpdateMessage(const PlayerList & listDestPlayers);
    void SendStatus(const PlayerList & listDestPlayers);
    void SetEntranceLocation(const uint32 uGUID, EntranceLocation & tLocation);
    SWorld();
    ~SWorld();
    SWorldID id;  // 0x00
    WorldCommonData tWCD;  // 0x08
    SNO snoWorldScript;  // 0x78
    uint32 dwFlags;  // 0x7c
    EntranceLocationMap tEntranceMap;  // 0x80
    SavePointLocationMap tSavePortalMap;  // 0x100
    TPooledSpawnGroupExactInstanceList listSpawnGroupExact;  // 0x180
    TPooledSpawnGroupExactInstanceList listSpawnGroupFailed;  // 0x1a8
    ActorID idActorEntrancePortal;  // 0x1d0
    uint32 dwModifiedBits;  // 0x1d4
    int32 nSeed;  // 0x1d8
    NoSpawnTree tNoSpawnTree;  // 0x1e0
    int32 eCreateStage;  // 0x228
    WorldCreatedCallback pfCreatedCallback;  // 0x230
    Vector2D vExtMin;  // 0x238
    Vector2D vExtMax;  // 0x240
    SNO snoBossEncounter;  // 0x248
    SNO snoChallenge;  // 0x24c
    BOOL bChallengeMonstersCleared;  // 0x250
    WorldPlace tChallengeCheckpointPlace;  // 0x254
    Quaternion qChallengeCheckpointOrientation;  // 0x264
    int32 nMonstersRemaining;  // 0x274
    BOOL bCheckMonstersRemaining;  // 0x278
    SNO snoWorldMonsterPower;  // 0x27c
    SNO snoWorldPlayerPower;  // 0x280
    StringLabelHandle hWorldQuestDescriptionLabel;  // 0x284
    StringLabelHandle hWorldQuestTitleLabel;  // 0x288
    ResolvedPortalDestination tDynamicReturnLocation;  // 0x28c
    dmWorldId worldId;  // 0x298
    TownPortalLocation tTownPortalLocation;  // 0x29c
    ReturnPortalLocation tReturnPortalLocation;  // 0x2b0
    uint32 dwDeleteTime;  // 0x2c4
    uint32 dwTimeWithoutBeingVisited;  // 0x2c8
    uint64 arAllowedPlayers[4];  // 0x2d0
    Blizzard::Time::Millisecond dwPreloadingStart;  // 0x2f0
    BOOL fPreloadingStarted;  // 0x2f4
    BOOL fPreloadingDungeonFinderStarted;  // 0x2f8
    BOOL fWaitForLoadingScreen;  // 0x2fc
    struct WorldCreatedCallbackWrapper {  // size 0x18
        WorldCreatedCallback pfCreatedCallback;  // 0x00
        ACDID idACDPortalOperator;  // 0x08
        ACDID idACDPortal;  // 0x0c
        uint32 uEntranceGUID;  // 0x10
        PlayerWarpReason eWarpReason;  // 0x14
    };
    typedef XGrowableList<SWorld::WorldCreatedCallbackWrapper, SWorld::WorldCreatedCallbackWrapper> WorldCreatedCallbackList;
    SWorld::WorldCreatedCallbackList tExtraWorldCreatedCallbacks;  // 0x300
    void AddCreateCallback(WorldCreatedCallback _pfCreatedCallback, ACDID idACDPortalOperator, ACDID idACDPortal, uint32 uEntranceGUID, PlayerWarpReason eWarpReason);
    XPooledMap<unsigned int, unsigned int, Minimap::DiscoveryInfo, const Minimap::DiscoveryInfo &, 16> mapUndiscoveredMinimapMarkers;  // 0x358
    XPooledList<int, int> listEventSNOs;  // 0x400
    XPooledList<UniqueMonsterActorEntry, const UniqueMonsterActorEntry &> listUniqueMonsterActorEntries;  // 0x428
};
static_assert(sizeof(SWorld) == 0x450, "SWorld");

inline SWorld * SWorldGet(SWorldID idSWorld) {  // line 280
    return SGameManager::GameGet().ptSWorlds->ElementGet(idSWorld);
}

inline SWorld * SWorldTryToGet(SWorldID idSWorld) {  // line 286
    return SGameManager::GameGet().ptSWorlds->ElementTryToGet(idSWorld);
}

// Declared here from their definitions in SWorld.cpp.
SWorldID SWorldFindBySNO(const SNO & snoWorld);  // SWorld.cpp:1406
BOOL SWorldPlayerAllowed(SWorldID idSWorld, const Player & tPlayer);  // SWorld.cpp:3984
SWorldID SWorldCreate(const SNO snoWorld, BOOL bCreateImmediately, BOOL bCausedByLocalPlayer,
                      WorldCreatedCallback pfCreatedCallback, ACDID idACDPortalOperator,
                      ACDID idACDPortal, uint32 uEntranceGUID, PlayerWarpReason eWarpReason,
                      BOOL fCreateWhenLoadingScreenIsDone);  // SWorld.cpp:1134
ACDID SWorldPlayerEnter(SWorldID idSWorld, const ACDID idACDPlayer, Player * ptPlayer,
                        const uint32 uEntranceGUID, const BOOL fAllowStartLocationOverride,
                        const D3::Hero::SavedDefinition & tHeroDefinition,
                        const D3::Account::SavedDefinition & tAccountDefinition,
                        const D3::Items::Mails & tMails,
                        const Player * ptReferencePlayer);  // SWorld.cpp:1497

// SWorld.cpp:2237, :2245 and :3565 (X/Common/Game.cpp GameStop / GameStartGame).
namespace D3 { namespace Leaderboard { class RiftSnapshot; } }
void SWorldsSuspendPreload();
void SWorldsResumePreload();
void SWorldLoadRiftShapshot(const SGameID idSGame, uint32 nChallengeNum, const D3::Leaderboard::RiftSnapshot & tSnapshot, int32 nEndTime, bool bChallengeOverriden, float flOrbValueMultiplier);

// SWorld.cpp:284,293; used by the original NoSpawnTree allocator path.
NoSpawnCirclePool * SWorldGetNoSpawnCirclePool();
NoSpawnNodePool * SWorldGetNoSpawnNodePool();
static_assert(__builtin_offsetof(SWorld, tNoSpawnTree) == 0x1e0, "SWorld::tNoSpawnTree");

BOOL SWorldAnyLoading();  // SWorld.cpp:2701
void SWorldFlagForDeletion(SWorldID idSWorld, uint32 nTicks, BOOL bCheckForPlayers);  // SWorld.cpp:358
void SWorldsDeleteFlagged();  // SWorld.cpp:588
BOOL SWorldIsCreated(const SWorld & tSWorld);  // SWorld.cpp:2228
void SWorldFlushWorldCreationList();  // SWorld.cpp:1035
BOOL SWorldIsTieredDungeonFinder(SWorldID idSWorld);  // SWorld.cpp:3321

SNO SWorldGetDungeonFinderTileset(SNO snoWorld);
ACDID SWorldDungeonFinderSpawnBoss(WorldPlace tPlaceOrigin);

void SWorldRemovePlayersAndFlagForDeletion(SWorldID idSWorld);  // SWorld.cpp:4047

// SWorld.cpp:3304; no recovered header declaration. Owning API header placement is inferred.
BOOL SWorldIsDungeonFinder(SWorldID idSWorld);
