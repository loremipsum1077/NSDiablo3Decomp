// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/Actors/Actors.h
// Subset: the Actor members the minimap uses. The other members are padding (invented names)
// until someone needs them; the full reference header pulls in most of the server.

#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../Common/CommonTypes.h"
#include "../../Common/Definitions/ActDefinition.h"
#include "../../Common/InactiveMarker.h"
#include "../../Common/SNO/SNOFiles.h"
#include "../../Common/GameBalance.h"
#include "../../Common/OnlineService/OnlineService.h"
#include "../../Common/SNO/SNOName.h"
#include "../../Common/WorldPlace.h"
#include "../../Sigma/DataStructures/DataArraysTemplated.h"
#include "../../Sigma/Math/Quaternion.h"
#include "../../Sigma/Random.h"
#include "../SGame.h"

class SActorMovement;
struct ActorPowers;
struct ActorAI;
struct ActorProjectile;
struct TimedEffects;
struct StaggerDamageInfo;
namespace D3 { namespace OnlineService { class EntityId; } }

class InventoryId {  // size 0x18
public:
    InventoryId();
    InventoryId & operator=(const OnlineService::GameAccountHandle & tHandle);
    InventoryId & operator=(OnlineService::HeroId uHeroId);
    bool IsInvalid() const;
    void SetInvalid();
    void Import(const D3::OnlineService::EntityId & tEntityId);
    void Export(D3::OnlineService::EntityId & tEntityId) const;
private:
    OnlineService::GameAccountHandle m_tGameAccount;  // 0x00
    OnlineService::HeroId m_uHeroId;  // 0x10
};
static_assert(sizeof(InventoryId) == 0x18, "InventoryId");


struct Actor {  // size 0x290
    ActorID id;  // 0x00
    RNG rndOriginal;  // 0x04
    uint32 tName;  // 0x0c
    ACDID idACD;  // 0x10
    SNO snoActor;  // 0x14
    SNO snoMonster;  // 0x18
    SNOPooledList listSNOScripts;  // 0x20
    SNO snoConvList;  // 0x48
    Angle aYaw;  // 0x4c
    Vector3D wvVelocity;  // 0x50
    SSceneID idCurrentScene;  // 0x5c
    int32 idCurrentLevelArea;  // 0x60 (LevelAreaID)
    uint32 uCurrNavMeshFlags;  // 0x64
    ActorAI * ptAI;  // 0x68
    int32 nAttackerCount;  // 0x70
    int32 nAttackerSlotsUsed;  // 0x74
    int32 nAttackerLargeSlotsUsed;  // 0x78
    int32 eBrain;  // 0x7c (ActorBrain)
    ActorPowers * ptActorPowers;  // 0x80
    CHAR szPowerState[256];  // 0x88
    TimedEffects * ptTimedEffects;  // 0x188
    ActorProjectile * ptProjectile;  // 0x190
    uint32 dwActorFlags;  // 0x198
    SNO snoCurrentAnim;  // 0x19c
    int32 nCurrentAnimPermutationIndex;  // 0x1a0
    float flCurrentAnimSpeedScale;  // 0x1a4
    Vector3D wvCurrentAnimVelocity;  // 0x1a8
    UpdateData tAttrUpdateData;  // 0x1b8
    UpdateData tGizmoUpdateData;  // 0x1d8
    UpdateData tAreaDamageUpdateData;  // 0x1f8
    uint32 dwTimeCreated;  // 0x218
    uint32 dwLastAttributeUpdateTime;  // 0x21c
    int32 nAttributeUpdateBucketIndex;  // 0x220
    uint32 dwLastAreaDamageUpdateTime;  // 0x224
    int32 nAreaDamageBucketIndex;  // 0x228
    uint32 dwTrickleBits;  // 0x22c
    uint32 dwTimeSinceLastRecoilUpdate;  // 0x230
    int32 idDeathTimer;  // 0x234 (TimerID)
    int32 nSourceSceneUID;  // 0x238
    InactiveMarkerOwner tMarkerOwner;  // 0x23c
    SActorMovement * ptSActorMovement;  // 0x248
    StaggerDamageInfo * ptStaggerDamageInfo;  // 0x250
    InventoryId tItemInventoryId;  // 0x258
    OnlineService::ItemId tItemId;  // 0x270
    uint32 dwItemInitSeed;  // 0x280
    uint32 dwItemIdentifySeed;  // 0x284
    uint32 dwSeasonCreated;  // 0x288
};
static_assert(sizeof(Actor) == 0x290, "Actor");

typedef int32 PortalResolveMethod;

Actor * ActorGet(ActorID idActor);  // Actors.cpp:4880
Actor * ActorTryToGet(ActorID idActor);  // Actors.cpp:4890
int32 ActorGetInheritedTagMapValueInt(const Actor * ptActor, const uint32 dwActorTag,
                                      const uint32 dwMarkerTag);  // Actors.cpp:7107
PortalResolveMethod ActorResolvePortalDestination(const Actor * ptActor,
                                                  ResolvedPortalDestination & tResolvedDest);  // Actors.cpp:1626

// Actors.h:525. Body recovered from MirrorImageBrainActionUpdate's two inline sites.
inline ACDID ACDFindByActor(ActorID idActor) {
    return ActorGet(idActor)->idACD;
}

// Actors.h:527.
inline ACDID ACDTryFindByActor(ActorID idActor) {
    if (idActor != ACTORID_INVALID) {
        Actor * ptActor = ActorTryToGet(idActor);
        if (ptActor)
            return ptActor->idACD;
    }
    return ACDID_INVALID;
}

static_assert(__builtin_offsetof(Actor, ptActorPowers) == 0x80, "Actor::ptActorPowers");

struct WorldPlace;
void ActorFlagForDeletion(ActorID idActor);  // Actors.cpp:4407
void ActorSnapToPosition(const ActorID idActor, const WorldPlace & tPlace, BOOL bSnapToGround, BOOL bSendToClient);  // Actors.cpp:4721

enum {
    ACTORADD_BRAINLESS_BIT = 0,
    ACTORADD_DISABLE_BRAIN_BIT = 1,
    ACTORADD_DOESNT_WALK_BIT = 2,
    ACTORADD_FORCE_INVENTORY_CREATE = 3,
    ACTORADD_EVEN_WHEN_STUCK = 4,
    ACTORADD_DONT_REPORT_SPAWN_ERROR = 5,
    ACTORADD_DONT_PERTURB = 6,
    ACTORADD_DONT_SEARCH_FOR_LOCATION = 7,
    ACTORADD_IS_FROM_MARKER = 8,
    ACTORADD_PROJECTILE = 9,
    ACTORADD_DOESNT_DEACTIVATE = 10,
    ACTORADD_DONT_ADD_IF_LOCATION_SEARCH_FAILS = 11,
    ACTORADD_ADD_AT_SPAWNER_LOCATION_IF_SEARCH_FAILS = 12,
    ACTORADD_IGNORE_SPAWNER = 13,
    ACTORADD_IS_FROM_INACTIVE = 14,
    ACTORADD_USE_NEW_ENCOUNTER_PASSABILITY = 15,
    ACTORADD_NO_DAMAGE = 16,
    ACTORADD_DOES_FAKE_DAMAGE = 17,
    ACTORADD_KEEP_MARKER_NAME = 18,
    ACTORADD_INVULNERABLE = 19,
    ACTORADD_GIZMO_DOOR_OPEN = 20,
    ACTORADD_IS_FROM_DISTRIBUTION = 21,
    ACTORADD_OVERRIDE_RARE_NAME = 22,
    ACTORADD_FAST_MOVER = 23,
    ACTORADD_NO_AFFIXES = 24,
    ACTORADD_USE_ACTOR_STATIC_FLAGS_FOR_SPAWN = 25,
};

enum ActorFlag {
    ACTOR_GOD_MODE_BIT = 0,
    ACTOR_EASYKILL_BIT = 1,
    ACTOR_DELETE_AT_END_OF_UPDATE_BIT = 2,
    ACTOR_DOESNT_DEACTIVATE_BIT = 3,
    ACTOR_NO_COLLISION_BIT = 4,
    ACTOR_DYING = 5,
    ACTOR_DEACTIVATING = 6,
    ACTOR_NO_DAMAGE_RANGE_BIT = 7,
    ACTOR_DELETING_BIT = 8,
    ACTOR_DB_PERSISTED = 9,
    ACTOR_CHEATED = 10,
    ACTOR_IS_PLAYER = 11,
    ACTOR_SUMMON_PRELOADED = 12,
    ACTOR_DELETES_INSTEAD_OF_INACTIVE_BIT = 13,
};
void ActorFlagSet(const ActorID idActor, const ActorFlag eFlag, const BOOL bValue);  // Actors.cpp:6612
typedef int32 AttribGroupID;  // X/Common/Attributes/FastAttribute.h
typedef int32 AttributeTimerType;  // X/Common/ActorCommonData.h
AttribGroupID ActorTimerEffectsGetGroup(const ActorID idActor, AttributeTimerType eTimerType);  // Actors.cpp:6244

// ---- Added for X/Server/Actors/Actors.cpp ----

struct ActorSavedAI;
struct SavedDataKey;
struct Player;
struct PassabilitySettings;
class ActorCommonData;


typedef TDataArrayIterator<Actor> ActorIterator;
typedef XPooledDataIDList ActorIDListPooled;
typedef int32 Team;
typedef int32 FastAttribType;
typedef int32 MonsterRarity;
typedef int32 LevelAreaID;
typedef int32 ACDPackID;
typedef int32 PassabilityType;

struct ActorAddStruct {  // size 0x148
    SSceneID idMarkersSScene;  // 0x00
    SNO snoActor;  // 0x04
    uint32 dwActorAddFlags;  // 0x08
    uint32 dwMarkersetCreateOptions;  // 0x0c
    WorldPlace tPlace;  // 0x10
    Quaternion qOrientation;  // 0x20
    float flScale;  // 0x30
    uint32 tMarkersName;  // 0x34
    NavMeshHandle tNavMeshHandle;  // 0x38
    NavMeshHandle tNavMeshHandlePathingSpace;  // 0x40
    GBHandle hGB;  // 0x48
    SNOName tSNOName;  // 0x50
    Team eTeam;  // 0x58
    ActorSavedAI * ptSavedAI;  // 0x60
    SavedDataKey * ptLuaSavedDataKey;  // 0x68
    FastAttribType eFastAttribType;  // 0x70
    RNG * prngActorSeed;  // 0x78
    MonsterRarity eMonsterRarity;  // 0x80
    LevelAreaID idLevelArea;  // 0x84
    SNO snoEncounterSource;  // 0x88
    int32 nLevel;  // 0x8c
    GBID pRareNameGBIDs[2];  // 0x90
    GBID pMonsterAffixGBIDs[8];  // 0x98
    GBID pMonsterSpawnerAffixGBIDs[8];  // 0xb8
    int32 nSpawnerAdditionalAffixes;  // 0xd8
    ActorIDListPooled * plistActors;  // 0xe0
    BOOL bAvenger;  // 0xe8
    SNOPooledList listSNOScripts;  // 0xf0
    SNO snoConvList;  // 0x118
    InactiveMarkerOwner tMarkerOwner;  // 0x11c
    Player * ptPlayer;  // 0x128
    uint32 dwSpawnAnimTag;  // 0x130
    uint32 uPrimaryGroup;  // 0x134
    uint32 uSecondaryGroup;  // 0x138
    uint32 dwInventorySeed;  // 0x13c
    BOOL bIsSummon;  // 0x140
    ActorAddStruct();
    ActorAddStruct(const ActorAddStruct & other);
    void Clear();
};
static_assert(sizeof(ActorAddStruct) == 0x148, "ActorAddStruct");
ActorID ActorAdd(const ActorAddStruct & tActorAdd);  // Actors.cpp:2948

struct EncounterAddStruct {  // size 0x18
    GBHandleList * plistGroupAffixes;  // 0x00
    MonsterRarity eGroupAffixType;  // 0x08
    int32 nRecursionLevel;  // 0x0c
    ACDPackID idACDPack;  // 0x10
};
static_assert(sizeof(EncounterAddStruct) == 0x18, "EncounterAddStruct");

void PassabilitySettingsInit(PassabilitySettings & tPassability);  // Actors.cpp:6702
void EncounterAddInitialize(EncounterAddStruct & tEncounterAdd, GBHandleList & listGroupAffixes);  // Actors.cpp:6027
void ActorCheckChangeLevelArea(ActorID idActor);  // Actors.cpp:2804
void ActorCheckNavMeshTypeChange(ActorID idActor, uint32 uPrevNavMeshFlags, SWorldID idSWorldPrev);  // Actors.cpp:2833
void HeroCheckAreaAfterCreate(ActorID idActor);  // Actors.cpp:5890
void ActorPushSpawnSkipDistance(int32 nSkipDistMin, int32 nSkipDistDelta);  // Actors.cpp:6646
void ActorPopSpawnSkipDistance();  // Actors.cpp:6667
int32 ActorsGetCount();  // Actors.cpp:4900
uint32 ActorGetAttributeUpdateTicks();  // Actors.cpp:7182
BOOL ActorShouldThrottleScriptActors();  // Actors.cpp:7191
void ActorsDumpGarbageCollectionInfo();  // Actors.cpp:5020
float ActorGetInheritedTagMapValueFloat(const Actor * ptActor, const uint32 dwActorTag, const uint32 dwMarkerTag);  // Actors.cpp:7098
uint32 ActorGetInheritedTagMapValueDWORD(const Actor * ptActor, const uint32 dwActorTag, const uint32 dwMarkerTag);  // Actors.cpp:7116
SNO ActorGetInheritedTagMapValueSNO(const Actor * ptActor, const uint32 dwActorTag, const uint32 dwMarkerTag);  // Actors.cpp:7125
GBID ActorGetInheritedTagMapValueGBID(const Actor * ptActor, const uint32 dwActorTag, const uint32 dwMarkerTag);  // Actors.cpp:7134
BOOL ActorIsQuestLifespanValid(const Actor & tActor);  // Actors.cpp:6917
SNO ActorGetQuestLifespan(const Actor & tActor);  // Actors.cpp:6931
BOOL ActorFlagIsSet(const ActorID idActor, const ActorFlag eFlag);  // Actors.cpp:6623
void ActorDelete(ActorID idActor);  // Actors.cpp:4397
BOOL ActorFlaggedForDeletion(ActorID idActor);  // Actors.cpp:4620
Actor * ActorDataArrayIterate(ActorIterator & tIterator);  // Actors.cpp:4817
struct NavMeshHandle;
BOOL FindSpawnLocation(SNO snoActor, const NavMeshHandle & tNavMeshHandle, const PassabilityType ePassabilityType, const uint32 dwFindLocationFlags, WorldPlace & placeResult, NavMeshHandle & tNavMeshHandleResult);  // Actors.cpp:3698
BOOL FindPerturbedSpawnLocation(SNO snoActor, const NavMeshHandle & tNavMeshHandle, const PassabilitySettings & tPassability, const float flMinSearchRadius, const uint32 dwFindLocationFlags, WorldPlace & placeResult, NavMeshHandle & tNavMeshHandleResult, Team eTeam);  // Actors.cpp:3660
void ActorFindAddLocation(const ActorCommonData & tACDSearchFrom, const PassabilityType ePassabilityType, ActorAddStruct & tActorAdd);  // Actors.cpp:3632
void ActorDeactivateFromQuestRange(ActorID idActor);  // Actors.cpp:4429
int32 ActorGetCorrectLevel(const SNOName & tActorSNOName, int32 nOverrideLevel);  // Actors.cpp:2063
typedef int32 TimerID;
struct StaggerDamageInfo {  // size 0x28
    blz::vector<float, blz::allocator<float> > arStaggeredDamageBuckets;  // 0x00
    float flStaggeredDamageDuration;  // 0x18
    uint32 nStaggeredDamagePosition;  // 0x1c
    uint32 uStaggeredDamageLastTickCheck;  // 0x20
    int32 nStaggeredDamagePreviousBucket;  // 0x24
    // Actors.h:133 (the DWARF records the constructor; body from the copy inlined into PowerAPI_DoDamage).
    StaggerDamageInfo() : flStaggeredDamageDuration(0.0f), nStaggeredDamagePosition(0), uStaggeredDamageLastTickCheck(0), nStaggeredDamagePreviousBucket(0) {}
};
static_assert(sizeof(StaggerDamageInfo) == 0x28, "StaggerDamageInfo");
struct Quaternion;
void ActorSetOrientation(const ActorID idActor, const Quaternion & qOrientation, const BOOL bSendToClient);  // Actors.cpp:3736
void ActorUpdateOrientation(const ActorID idActor, BOOL bSendToClient);  // Actors.cpp:4635
void ActorClearStaggeredDamage(const ActorID idActor);  // Actors.cpp:3812
TimerID ActorDeleteSoon(ActorID idActor, int32 nDuration);  // Actors.cpp:4470
void ActorTimerEffectsExpireGroup(const ActorID idActor, AttributeTimerType eTimerType);  // Actors.cpp:6264

void ActorFindMonsterForPlayersLevelArea(ActorCommonData & tACDPlayer, SNOArray & listActors, blz::vector<int> & arAllowedMonsterRarities);  // Actors.cpp:7462

ActorID GBAdd(ActorAddStruct & tActorAdd);  // Actors.cpp:6041

// Declaration placement inferred from the owning API; DWARF definition: Actors.cpp:3770.
void ActorTriggerAttributeUpdate(const ActorID idActor);

// No header declaration recovered; owning-header placement inferred from Actors.cpp:3755.
void ActorInternalSetCurrentFacing(ActorID idActor, Angle aYaw);
