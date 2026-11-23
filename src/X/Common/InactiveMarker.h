// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Common/InactiveMarker.h
// Subset: InactiveMarkerOwner. The full reference header pulls in a lot of the game.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "CommonTypes.h"
#include "SNO/SNOFiles.h"
#include "../Sigma/DataStructures/XPooledList.h"
#include "OnlineService/OnlineService.h"
#include "Protobuf/Items.pb.h"
#include "SavedData.h"
#include "../Sigma/Math/Sphere.h"
#include "../Sigma/Math/Transform.h"
#include "../Sigma/Math/Vector.h"
#include "../Sigma/Random.h"

typedef int32 ACDPackID;
typedef int32 GBID;
typedef int32 GameConnectionID;
typedef int32 PingID;

class Frustum;
struct Vector3D;

struct InactiveMarkerOwner {  // size 0xc
    SNO snoMarkerSet;  // 0x00
    int16 nMarkerIndex;  // 0x04
    uint16 nAdventureParentIndex;  // 0x06
    uint32 nMarkerSetInstanceIndex;  // 0x08
    void Initialize();
};
static_assert(sizeof(InactiveMarkerOwner) == 0xc, "InactiveMarkerOwner");

typedef int32 AIInactiveSaveBlockID;

struct ActorSavedAI {  // size 0x8
    uint32 dwIndexUsedFlags;  // 0x00
    AIInactiveSaveBlockID idFirstBlock;  // 0x04
};
static_assert(sizeof(ActorSavedAI) == 0x8, "ActorSavedAI");

enum InactiveMarkerBaseType {
    INACTIVE_MARKERSET = 0,
    INACTIVE_MONSTER = 1,
    INACTIVE_GIZMO = 2,
};

struct InactiveBaseMarker {  // size 0x48
    uint8 eInactiveMarkerType;  // 0x00
    uint16 nFlags;  // 0x02
    uint8 nScriptState;  // 0x04
    InactiveMarkerOwner tMarkerOwner;  // 0x08
    int16 nPrimaryGroupIndex;  // 0x14
    int16 nSecondaryGroupIndex;  // 0x16
    PRTransform transform;  // 0x18
    Sphere wsBounds;  // 0x34
    SavedDataKey tLuaSavedDataKey;  // 0x44
};
static_assert(sizeof(InactiveBaseMarker) == 0x48, "InactiveBaseMarker");

typedef XListMemoryPool<InactiveBaseMarker *> TInactiveMarkerListPool;

typedef XPooledList<InactiveBaseMarker *, InactiveBaseMarker *> TInactiveMarkerList;

enum {
    GSA_FLAG_NO_DAMAGE = 0,
    GSA_FLAG_DOES_FAKE_DAMAGE = 1,
    GSA_FLAG_INVULNERABLE = 2,
    GSA_FLAG_CANNOT_BE_ADDED_TO_AI_TARGET_LIST = 3,
    GSA_FLAG_GRANTS_NO_XP = 4,
    GSA_FLAG_DROPS_NO_LOOT = 5,
    GSA_FLAG_DISABLE_BRAIN = 6,
};

struct InactiveGizmoMarker : public InactiveBaseMarker {  // size 0x70
    SNO snoActor;  // 0x48
    float flScale;  // 0x4c
    CHAR nGizmoState;  // 0x50
    CHAR nGizmoCharges;  // 0x51
    CHAR nGizmoChestOpen;  // 0x52
    CHAR nGizmoDisabledByScript;  // 0x53
    CHAR nLevel;  // 0x54
    CHAR nTeam;  // 0x55
    uint8 dwFlags;  // 0x56
    int32 nSourceSceneUID;  // 0x58
    int32 dwCooldown;  // 0x5c
    uint32 uSpawnerCountID;  // 0x60
    uint32 uDynamicEntranceGUID;  // 0x64
    PingID idPing;  // 0x68
    uint32 dwAnimIdleTag;  // 0x6c
};
static_assert(sizeof(InactiveGizmoMarker) == 0x70, "InactiveGizmoMarker");

enum {
    MSA_FLAG_NO_DAMAGE = 0,
    MSA_FLAG_DOES_FAKE_DAMAGE = 1,
    MSA_FLAG_INVULNERABLE = 2,
    MSA_FLAG_CANNOT_BE_ADDED_TO_AI_TARGET_LIST = 3,
    MSA_FLAG_GRANTS_NO_XP = 4,
    MSA_FLAG_DROPS_NO_LOOT = 5,
    MSA_FLAG_HAS_DROPPED_SPECIAL_LOOT = 6,
    MSA_FLAG_IN_TIERED_RIFT = 7,
    MSA_FLAG_DISABLE_BRAIN = 8,
    MSA_FLAG_IS_FROM_DISTRIBUTION = 9,
    MSA_FLAG_RESURRECTED = 10,
};

struct MonsterSavedAttributes {  // size 0x28
    uint32 uDeactivationTick;  // 0x00
    float flHitPoints;  // 0x04
    int32 nPlayersWhenDeactivated;  // 0x08
    int32 nDifficultyWhenDeactivated;  // 0x0c
    ACDID idACDSummonedBy;  // 0x10
    uint32 dwSummonExpirationTick;  // 0x14
    uint32 dwAnimIdleTag;  // 0x18
    uint32 dwInventorySeed;  // 0x1c
    CHAR nLevel;  // 0x20
    CHAR nDamageState;  // 0x21
    CHAR nTeam;  // 0x22
    uint16 dwSavedFlagAttributes;  // 0x24
};
static_assert(sizeof(MonsterSavedAttributes) == 0x28, "MonsterSavedAttributes");

struct InactiveMonsterMarker : public InactiveBaseMarker {  // size 0xd8
    MonsterSavedAttributes tSavedAttributes;  // 0x48
    ActorSavedAI tSavedAI;  // 0x70
    SNO snoMonster;  // 0x78
    float flScale;  // 0x7c
    NavMeshHandle tNavMeshHandleCenter;  // 0x80
    ACDPackID idACDPack;  // 0x88
    SSceneID arOccupiedScenes[4];  // 0x8c
    uint8 nOccupiedSceneCount;  // 0x9c
    CHAR eMonsterRarity;  // 0x9d
    uint8 nLookLinkIndex;  // 0x9e
    uint8 nPad2;  // 0x9f
    RNG tRNGOriginal;  // 0xa0
    GBID arRareNameGBIDs[2];  // 0xa8
    GBID arMonsterAffixGBIDs[8];  // 0xb0
    int32 nSourceSceneUID;  // 0xd0
    SNO snoConvList;  // 0xd4
};
static_assert(sizeof(InactiveMonsterMarker) == 0xd8, "InactiveMonsterMarker");

struct InactiveItemMarker {  // size 0x1a0
    OnlineService::ItemId tItemId;  // 0x00
    DataID tUpdateId;  // 0x10
    Vector3D wp;  // 0x14
    Sphere wsBounds;  // 0x20
    uint32 dwCreateTime;  // 0x30
    ACDID idACDBoundTo;  // 0x34
    uint32 uUnlockTime;  // 0x38
    PingID idPing;  // 0x3c
    D3::Items::Generator tItemGenerator;  // 0x40
    int32 nGoldAmount;  // 0x118
    uint8 nSocketedItemCount;  // 0x11c
    uint8 bSoulbound : 1;  // 0x11d
    uint8 bPersisted : 1;  // 0x11d
    uint8 bCheated : 1;  // 0x11d
    int8 nPlayerOwner;  // 0x11e
    uint8 nPad;  // 0x11f
    InactiveItemMarker * arSocketedItems[3];  // 0x120
    OnlineService::HeroId idAssignedHero;  // 0x138
    uint32 dwSeasonCreated;  // 0x140
    uint32 uTradeEndTime;  // 0x144
    uint64 idTradablePlayers[4];  // 0x148
    uint64 idSourcePlayer;  // 0x168
    GameConnectionID idAssignedGameConnectionID;  // 0x170
    uint64 idPlayerGiftRecipient;  // 0x178
    char szPlayerName[32];  // 0x180
};
static_assert(sizeof(InactiveItemMarker) == 0x1a0, "InactiveItemMarker");

typedef XListMemoryPool<InactiveItemMarker *> TInactiveItemMarkerListPool;

typedef XPooledList<InactiveItemMarker *, InactiveItemMarker *> TInactiveItemMarkerList;

struct InactiveMarkerList {  // size 0x10
    TInactiveItemMarkerList * ptInactiveItemMarkerList;  // 0x00
    TInactiveMarkerList * ptInactiveMarkerList;  // 0x08
};
static_assert(sizeof(InactiveMarkerList) == 0x10, "InactiveMarkerList");

struct SceneCommonData;
struct PRTransform;
struct Sphere;
void InactiveMarkerCreateFromMarkerSet(InactiveMarkerList * ptInactiveMarkerList, const InactiveMarkerOwner & tOwner, const PRTransform & transform, const Sphere & wsBounds);  // InactiveMarker.cpp:554
InactiveBaseMarker * InactiveMarkerCreateForMarkerSet(const InactiveMarkerOwner & tMarkerOwner, const Sphere & wsBounds, const PRTransform & transform);  // InactiveMarker.cpp:588
ACDPackID InactiveMarkerGetPack(InactiveBaseMarker * ptInactiveMarker);  // InactiveMarker.cpp:892
void InactiveMarkerCreateFromMarkerSet(SceneCommonData & tSCD, const InactiveMarkerOwner & tOwner, const PRTransform & transform, const Sphere & wsBounds);  // InactiveMarker.cpp:566

void InactiveMarkerActivateInFrustrumForWorld(SWorldID idSWorld, const Frustum & tFrustum, const Vector3D & vCenterPos);  // InactiveMarker.cpp:506

// line 98. Body written from the copy inlined into ActorAddStruct::Clear (Actors.cpp:429).
inline void InactiveMarkerOwner::Initialize() {
    snoMarkerSet = SNO_NONE;
    nMarkerIndex = -1;
    nAdventureParentIndex = 0xffff;
    nMarkerSetInstanceIndex = 0;
}

ACDID ActivateActorMarkerInWorld(SNO snoActor, SNO snoWorld, SNO snoLevelArea);  // InactiveMarker.cpp:2126

ACDID ActivateMarkerName(LPCSTR szMarkerName);

// Original InactiveMarker.h:77.
enum {
    INACTIVE_MARKER_FLAG_CHECK_ACTIVATE_STATUS = 0,
    INACTIVE_MARKER_FLAG_IS_SCENE_MARKER = 1,
    INACTIVE_MARKER_FLAG_WAITING_ON_QUEST = 2,
    INACTIVE_MARKER_FLAG_DISABLED = 3,
    INACTIVE_MARKER_FLAG_QUEST_MONSTER = 4,
    INACTIVE_MARKER_FLAG_DISABLE_OPERATION = 5,
    INACTIVE_MARKER_FLAG_MINIMAP_ENABLED = 6,
    INACTIVE_MARKER_FLAG_BOUNTY_OBJECTIVE = 7,
};
