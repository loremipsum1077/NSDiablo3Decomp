#pragma once

#include "../../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../../BlizzardCore/Include/Blizzard/Types.h"
#include "../../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../CommonTypes.h"
#include "../../Sigma/RefString.h"
#include "../../Sigma/SigmaTypes.h"

typedef int32 PlayerClass;  // X/Common/GameSwig.h
typedef int32 SNO;  // X/Common/SNO/SNOFiles.h

enum AvengerSource {
    AVENGER_SOURCE_INVALID = 0,
    AVENGER_SOURCE_SOLO = 1,
    AVENGER_SOURCE_HARDCORE = 2,
    AVENGER_SOURCE_FRIEND = 3,
};

struct AvengerVictim {  // size 0x8
    PlayerClass eClass;  // 0x00
    BOOL bIsFemale;  // 0x04
    AvengerVictim();
};
static_assert(sizeof(AvengerVictim) == 0x8, "AvengerVictim");

typedef blz::vector<AvengerVictim, blz::allocator<AvengerVictim> > AvengerVictimArray;

enum AvengerSpawnCeremonyState {
    SPAWN_STATE_NONE = 0,
    SPAWN_STATE_PHASE_1 = 1,
    SPAWN_STATE_PHASE_2 = 2,
    SPAWN_STATE_PHASE_3 = 3,
    SPAWN_STATE_SPAWN = 4,
};

struct PlayerAvengerData {  // size 0x68
    CRefString sAvengerName;  // 0x00
    int32 nAvengerAffixBucket;  // 0x18
    uint64 nSentFrom;  // 0x20
    AvengerSource eAvengerSource;  // 0x28
    AvengerVictimArray arVictims;  // 0x30
    ACDNetworkName annTarget;  // 0x48
    BOOL bHasAvenger;  // 0x4c
    BOOL bHasSpawned;  // 0x50
    AvengerSpawnCeremonyState eSpawnState;  // 0x54
    uint32 uPhaseAdvanceTime;  // 0x58
    Blizzard::Time::Millisecond uLastCreationTime;  // 0x5c
    Blizzard::Time::Millisecond uLastSpawnTime;  // 0x60
    SNO snoBossLevelArea;  // 0x64
    PlayerAvengerData();
    void ClearCurrentAvenger();
};
static_assert(sizeof(PlayerAvengerData) == 0x68, "PlayerAvengerData");

struct AvengerData {  // size 0x58
    int32 nPlayerIndex;  // 0x00
    int32 nAffixBucket;  // 0x04
    CRefString sName;  // 0x08
    AvengerSource eAvengerSource;  // 0x20
    AvengerVictimArray arVictims;  // 0x28
    blz::vector<ActorID, blz::allocator<ActorID> > arVictimIDs;  // 0x40
    AvengerData();
};
static_assert(sizeof(AvengerData) == 0x58, "AvengerData");

// Header inline (DWARF: _ZN17PlayerAvengerData19ClearCurrentAvengerEv, Avenger.h:127, inlined into
// ClientMessageReceiveGameMessage at ClientMessageReceive.cpp:3116, :3139 and the avenger-killed case); body
// from those inlined copies.
inline void PlayerAvengerData::ClearCurrentAvenger()
{
    bHasAvenger = FALSE;
    bHasSpawned = FALSE;
    eAvengerSource = AVENGER_SOURCE_INVALID;
    nSentFrom = 0;
    sAvengerName.Format("");
    annTarget = ANN_INVALID;
    nAvengerAffixBucket = 0;
    eSpawnState = SPAWN_STATE_NONE;
    uPhaseAdvanceTime = 0;
}

// Header inline (DWARF: _ZN11AvengerDataC2Ev, inlined into ClientMessageReceiveGameMessage at
// ClientMessageReceive.cpp:3102 and :3190); body from those inlined copies.
inline AvengerData::AvengerData()
    : nPlayerIndex(0), nAffixBucket(0), eAvengerSource(AVENGER_SOURCE_INVALID)
{
    sName.Format("Avenger");
}

// Header inline (DWARF: _ZN17PlayerAvengerDataC2Ev, Avenger.h:119, abstract DIE only, inlined three times into
// Player::Player). Body from those copies and rows 121:27 (one 8-byte zero store over uLastCreationTime and
// uLastSpawnTime; uLastSpawnTime's own row is merged, inferred), 123:26 (snoBossLevelArea = -1, i.e. SNO_NONE,
// SNOFiles.h:101, not visible here) and ClearCurrentAvenger inlined at 124.
inline PlayerAvengerData::PlayerAvengerData()
{
    uLastCreationTime = 0;
    uLastSpawnTime = 0;
    snoBossLevelArea = -1;
    ClearCurrentAvenger();
}

inline AvengerVictim::AvengerVictim()  // line 38
    : eClass(-1), bIsFemale(FALSE)
{
}

// Avenger.cpp
class Player;
class ActorCommonData;
struct AvengerInfoMessage;

struct AvengerUpdateSourceMessage {  // size 0xc
    RequiredMessageHeader tHeader;  // 0x00
    AvengerSource eAvengerSource;  // 0x08
};
static_assert(sizeof(AvengerUpdateSourceMessage) == 0xc, "AvengerUpdateSourceMessage");
struct UIID;
typedef int32 GlobalSNO;  // X/Common/GlobalSNO.h
namespace D3 { namespace Account { class Avenger; class AvengerData; class AvengerVictim; class SavedDefinition; } }
namespace google { namespace protobuf { template <typename Element> class RepeatedPtrField; } }

namespace Avenger { namespace Console {
void SaveAvengerInfoToAccount(Player * ptPlayer, D3::Account::AvengerData & tAccountAvengerData);
void SaveAvengerToPlayer(const D3::Account::Avenger & tAvenger, PlayerAvengerData * ptPlayerAvengerData, AvengerSource eSource);
void SaveAccountInfoToAvengerData(Player * ptPlayer, const D3::Account::AvengerData & tAvengerAccountData);
void InitializePlayerAvengerData(Player * ptPlayer, D3::Account::SavedDefinition & tDefinition);
void StartAvengerSpawn(PlayerAvengerData * ptPlayerAvengerData, Player * ptPlayer);
void ResetAvengerSpawnTimer(ActorCommonData & tACD);
BOOL HasPlayerWaitedEnoughForAvengerSpawn(const ActorCommonData & tACD);
void TryToGetFriendAvenger(BOOL bSendToServer);
void OnEventNewMail(int32 nUserIndex);
void TryToRemoveAvenger(ACDID idACDPlayer);
int32 AvengerGetNumberOfAffixBuckets();
BOOL CanAvengerSpawnForPlayer(Player & tPlayer);
void CheckSpawnStartForPlayer(Player & tPlayer);
void PlayExitEffect(ActorCommonData & tACDAvenger);
int32 GetMaxPlayerKills();
void RemoveShadowClones(blz::vector<ActorID, blz::allocator<ActorID> > & tShadowClones);
void UpdateForPlayer(Player & tPlayer);
void VictimsCopyArrayToMessage(const AvengerVictimArray & arVictims, AvengerInfoMessage & tMessage);
void VictimsCopyMessageToArray(const AvengerInfoMessage & tMessage, AvengerVictimArray & arVictims);
void VictimsCopyArrayToArray(const AvengerVictimArray & arVictimsSource, AvengerVictimArray & arVictimsDestination);
void VictimsCopyProtobufToArray(const google::protobuf::RepeatedPtrField<D3::Account::AvengerVictim> & arProtobuf, AvengerVictimArray & arVictims);
void VictimsCopyArrayToProtobuf(const AvengerVictimArray & arVictims, google::protobuf::RepeatedPtrField<D3::Account::AvengerVictim> * arProtobuf);
void VictimsCopyProtobufToProtobuf(const google::protobuf::RepeatedPtrField<D3::Account::AvengerVictim> & arProtobufSource, google::protobuf::RepeatedPtrField<D3::Account::AvengerVictim> * arProtobufDestination);
GlobalSNO GetGlobalSNOActorForShadowCloneVictim(AvengerVictim & tVictim);
void ShowOnly(const UIID & idParent, const int32 nIndexToShow);
void InitializeForGame();
void DisposeForGame();
} }  // namespace Avenger::Console
