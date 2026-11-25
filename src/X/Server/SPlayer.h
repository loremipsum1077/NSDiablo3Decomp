// Adopted from tools/gen_headers.py output; hand-maintained from here on.
// Original file: X/Server/SPlayer.h
// Subset: only the declarations PlayerList.cpp uses; the full reference header pulls in most
// of the server.

#pragma once

#include "../../BlizzardCore/Include/Blizzard/Types.h"
#include "../Common/Act.h"
#include "../Common/CommonTypes.h"
#include "../Common/World.h"
#include "../Sigma/DataStructures/XPooledList.h"

struct Player;
class ActorCommonData;

typedef int32 LogoutContext;  // X/Common/Player.h

// Declared here from their definitions in SPlayer.cpp.
uint32 SPlayerGetContentFlags(const Player & tPlayer);  // SPlayer.cpp:10778
void SPlayerStartLogoutTimer(int32 nPlayerIndex, BOOL bIssueKickWarning, LogoutContext eContext);  // SPlayer.cpp:7639
void SPlayerSendKickTimers(Player & tPlayer);  // SPlayer.cpp:12138
void SPlayerWarped(ActorCommonData & tACDPlayer, const PlayerWarpReason eReason,
                   const WorldPlace * ptPlaceBeforeWarp, const ACDID idACDWarpGizmo,
                   const float flWarpFadeInSecs, const BOOL bSendClientWarped);  // SPlayer.cpp:8793
void SPlayerEnterAct(Player & tPlayer, const Act eAct, const BOOL bOnJoin);  // SPlayer.cpp:10313
void SPlayerPlayActInterstitial(Player & tPlayer, const Act eAct, BOOL bForceInterstitial);  // SPlayer.cpp:10343

// line 38
enum SPlayerSaveMode {
    PLAYER_SAVE_MODE_LAZY = 0,
    PLAYER_SAVE_MODE_WITHIN_5_SECONDS = 1,
    PLAYER_SAVE_MODE_END_OF_FRAME = 2,
    PLAYER_SAVE_MODE_IMMEDIATE = 3,
};

typedef int32 PlayerSaveReason;  // X/Common/Player.h:391
typedef int32 InventorySlotType;  // X/Common/ActorCommonData.h
namespace D3 { namespace Items { class Generator; } }

BOOL SPlayerInSaveList(const Player & tPlayer);  // SPlayer.cpp:4839
void SPlayerAddToSaveList(Player & tPlayer, uint32 dwHeroSaveOptions, PlayerSaveReason reason,
                          SPlayerSaveMode eSaveMode);  // SPlayer.cpp:4851
void SPlayerRetrieveSeasonalStashItemOrChallengeRiftReward(const Player & tPlayerFrom,
                                                           D3::Items::Generator & tGenerator,
                                                           InventorySlotType eInvSlot);  // SPlayer.cpp:13542
void SPlayerRemoveSkill(Player & tPlayer, SNO snoPower, int32 nSlot);
typedef int32 GBID;  // X/Common/GameBalance.h
void SPlayerActivatePet(Player & tPlayer, GBID gbidPet);  // SPlayer.cpp:14401
void SPlayerSetCosmeticPortrait(Player & tPlayer, GBID gbidPortrait);  // SPlayer.cpp:14502

typedef XPooledList<int, int> ServerTrickleList;  // SPlayer.h:38
void SPlayerTrickleClearAll(const Player & tPlayer);  // SPlayer.cpp
ServerTrickleList * SPlayerGetPossibleTrickleList(const Player & tPlayer);  // SPlayer.cpp:5214
ServerTrickleList * SPlayerGetPrevTrickleList(const Player & tPlayer);  // SPlayer.cpp
void SPlayerTrickleAdd(Player & tPlayer, ACDID idACD);  // SPlayer.cpp
void SPlayerTrickleRemove(Player & tPlayer, ACDID idACD);  // SPlayer.cpp
void SPlayerTrickleUpdate(Player & tPlayer);  // SPlayer.cpp

struct Player;
uint32 SPlayerGetExperienceId(const Player & tPlayer);  // SPlayer.cpp:7523

void SPlayerWarpToOtherPlayer(Player * ptPlayerWarping, Player * ptPlayerDest);  // SPlayer.cpp:10131
void SPlayerWarpToWaypoint(Player * ptPlayer, uint32 uWaypointIndex);  // SPlayer.cpp:10268

// More of SPlayer.cpp, for X/Server/SGame.cpp.
typedef int32 QuitReason;  // X/Server/SPlayer.h:29
void SPlayerDisconnect(Player & tPlayer, QuitReason eQuitReason, BOOL bHonorLogoutTimerRules);  // SPlayer.cpp:7539
BOOL SPlayersAreAllLocal();  // SPlayer.cpp:12957
void SPlayerKillAllPets(Player & tPlayer, BOOL bDismissFollowers, BOOL bKillHirelings, BOOL bKillCosmeticPets,
                        BOOL bEnteringRift);  // SPlayer.cpp:1709
void SPlayerActivateWaypoint(const int32 nWaypointNumber, const BOOL bNotifyClient);  // SPlayer.cpp:10504
BOOL SPlayerCanSwitchToNewWorld(const ACDID idACDPlayer);  // SPlayer.cpp:7444
BOOL SPlayerIsLocal(const Player & tPlayer);  // SPlayer.cpp:12943
BOOL PlayerSetPlayerGameFlag(Player * ptPlayer, LPCSTR szLabel, BOOL bValue);  // SPlayer.cpp:8840
void PlayerRecordHeal(ActorCommonData & tACDPlayer, float flHealAmount);  // SPlayer.cpp:4603

// More of SPlayer.h, for X/Server/SPlayer.cpp.
#include "../../BlizzardCore/Include/Blizzard/Time.h"
#include "../../Contrib/Contrib/blz/include/blz/vector.h"
#include "../Sigma/DataStructures/XGrowableList.h"

namespace D3 { namespace Account { class Consumable; class Consumables; class DeliveredRewards; } }

struct TickBankBucket {  // size 0x8
    uint32 dwTick;  // 0x00
    int32 nStoredTicks;  // 0x04
};

typedef XGrowableList<TickBankBucket, const TickBankBucket &> TickBankBucketList;  // line 46

struct TickRateBucket {  // size 0x8
    int32 nClientTickDelta;  // 0x00
    int32 nServerTickDelta;  // 0x04
    void Initialize() { nClientTickDelta = 0; nServerTickDelta = 0; }  // line 69
};

static const uint32 TICKRATE_TRACKER_MAX_BUCKETS = 30;  // line 72
class TickRateTracker {  // size 0xfc
public:
    void Initialize();
    float GetAverage();
    void AddBucket(int32 nClientTickDelta, int32 nServerTickDelta);
protected:
    TickRateBucket m_arBuckets[TICKRATE_TRACKER_MAX_BUCKETS];  // 0x00
    int32 m_nClientTickTotal;  // 0xf0
    int32 m_nServerTickTotal;  // 0xf4
    int32 m_nCurIndex;  // 0xf8
};

struct SPlayerConsumable {  // size 0x38
    SPlayerConsumable() : uLicenseInstanceId(0), uLicenseId(0), uTransactionId(0), nQuantity(0), tConsumeTime(0), tRevokeTime(0), uType(0), uDataPersistFlags(0) {}  // SPlayer.h:114
    void Import(const D3::Account::Consumable & tConsumable);
    void Export(D3::Account::Consumable & tConsumable) const;
    uint64 uLicenseInstanceId;  // 0x00
    uint64 uLicenseId;  // 0x08
    uint64 uTransactionId;  // 0x10
    int64 nQuantity;  // 0x18
    Blizzard::Time::Timestamp tConsumeTime;  // 0x20
    Blizzard::Time::Timestamp tRevokeTime;  // 0x28
    int32 uType;  // 0x30
    uint32 uDataPersistFlags;  // 0x34
};

struct SPlayerConsumables {  // size 0x18
    void Import(const D3::Account::Consumables & tConsumables, BOOL bClearOldContents);
    void Export(D3::Account::Consumables & tConsumables, uint32 uDataPersistFlagsFilter,
                uint32 uDataPersistFlagsToClear, uint32 uDataPersistFlagsToSet);
    blz::vector<SPlayerConsumable, blz::allocator<SPlayerConsumable> > arConsumables;  // 0x00
};

struct DeliveredAchievementRewardEntry { uint64 achievement_id; /* 0x00 */ int32 set_id; /* 0x08 */ };  // size 0x10 (invented type name: 2.7.7 has no DWARF; field names from D3::Account::DeliveredAchievementReward)
struct DeliveredRewardsSavedData {  // size 0x30
    void Export(D3::Account::DeliveredRewards & tDeliveredRewardsSavedData);
    void Import(const D3::Account::DeliveredRewards & tDeliveredRewardsSavedData);
    void AddAchievementReward(uint64 achievement, int32 set_id, Player * ptPlayer);  // 2.7.7: set_id (DeliveredAchievementReward field 2)
    void UndeliverAchievementReward(uint64 achievement);
    bool ContainsAchievement(uint64 achievement, int32 set_id);
    void Clear();
    enum {
        MAX_RECENT_ACHIEVEMENTS = 25,
    };
    blz::vector<DeliveredAchievementRewardEntry, blz::allocator<DeliveredAchievementRewardEntry> > tAchievementRewards;  // 0x00
    blz::vector<unsigned long, blz::allocator<unsigned long> > tLegacyAchievementsToDeliver;  // 0x18
};

typedef float (*PetComparisonValueFunc)(ACDID);
typedef int32 PetType;  // X/Common/Player.h
int32 SPlayerAddPet(Player & tPlayer, PetType eType, ACDID idACDPet, BOOL fSendNow, int32 nMaxCount,
                    PetComparisonValueFunc pCompareFunc, BOOL bIgnoreFade);  // SPlayer.cpp:3007
void SPlayerClearRechargeTimer(Player & tPlayer, SNO snoPower);  // SPlayer.cpp
void SPlayerAssignToLegendaryPowerSlot(Player & tPlayer, GBID gbidPower, int32 nSlot);  // SPlayer.cpp
void SPlayerUpdateSidekickStatsBoost(ActorCommonData & tACDPlayer);  // SPlayer.cpp:13762
void SPlayerConsoleSync(Player & tPlayer);  // SPlayer.cpp:13042
void PlayerSendHeroStateLoadoutToClient(Player & tPlayer, int32 nLoadoutIndex, BOOL bOnSave);  // SPlayer.cpp:2624

// Layout checks generated from the DWARF by tools/layout_asserts.py.
#include <stddef.h>
static_assert(sizeof(TickBankBucket) == 0x8, "TickBankBucket");
static_assert(offsetof(TickBankBucket, dwTick) == 0x0, "TickBankBucket");
static_assert(offsetof(TickBankBucket, nStoredTicks) == 0x4, "TickBankBucket");
static_assert(sizeof(TickRateBucket) == 0x8, "TickRateBucket");
static_assert(offsetof(TickRateBucket, nClientTickDelta) == 0x0, "TickRateBucket");
static_assert(offsetof(TickRateBucket, nServerTickDelta) == 0x4, "TickRateBucket");
static_assert(sizeof(TickRateTracker) == 0xfc, "TickRateTracker");
static_assert(sizeof(SPlayerConsumable) == 0x38, "SPlayerConsumable");
static_assert(offsetof(SPlayerConsumable, uLicenseInstanceId) == 0x0, "SPlayerConsumable");
static_assert(offsetof(SPlayerConsumable, uLicenseId) == 0x8, "SPlayerConsumable");
static_assert(offsetof(SPlayerConsumable, uTransactionId) == 0x10, "SPlayerConsumable");
static_assert(offsetof(SPlayerConsumable, nQuantity) == 0x18, "SPlayerConsumable");
static_assert(offsetof(SPlayerConsumable, tConsumeTime) == 0x20, "SPlayerConsumable");
static_assert(offsetof(SPlayerConsumable, tRevokeTime) == 0x28, "SPlayerConsumable");
static_assert(offsetof(SPlayerConsumable, uType) == 0x30, "SPlayerConsumable");
static_assert(offsetof(SPlayerConsumable, uDataPersistFlags) == 0x34, "SPlayerConsumable");
static_assert(sizeof(SPlayerConsumables) == 0x18, "SPlayerConsumables");
static_assert(offsetof(SPlayerConsumables, arConsumables) == 0x0, "SPlayerConsumables");
static_assert(sizeof(DeliveredRewardsSavedData) == 0x30, "DeliveredRewardsSavedData");
static_assert(offsetof(DeliveredRewardsSavedData, tAchievementRewards) == 0x0, "DeliveredRewardsSavedData");
static_assert(offsetof(DeliveredRewardsSavedData, tLegacyAchievementsToDeliver) == 0x18, "DeliveredRewardsSavedData");

// More of SPlayer.h: the save records and the GameSaveData hierarchy (X/Server/SPlayer.cpp).
#include "../../Contrib/Contrib/blz/include/blz/set.h"
#include "../../Contrib/Contrib/blz/include/blz/string.h"
#include "../Common/ActorInventory.h"
#include "../Common/ItemMailCommon.h"
#include "../Common/OnlineService/OnlineService.h"
#include "../Common/Partition.h"
#include "OnlineService/OnlineServiceServer.h"
namespace OnlineService { enum class StoreEscrowState : uint32; }  // X/Server/OnlineService/SStore.h:7 (that header also defines a static string, so it is not included here)
#include "SHero.h"

struct SGame;

struct EscrowSaveData {  // size 0x78; line 87
    EscrowSaveData();
    uint64 account_id;  // 0x00
    uint64 bnet_account_id;  // 0x08
    blz::string transaction_id;  // 0x10
    int64 amount;  // 0x38
    blz::string order_id;  // 0x40
    OnlineService::StoreEscrowState state;  // 0x68
    OnlineServiceServer::ProtoMessageCallback cb;  // 0x70
};
static_assert(sizeof(EscrowSaveData) == 0x78, "EscrowSaveData");
static_assert(offsetof(EscrowSaveData, transaction_id) == 0x10, "EscrowSaveData");
static_assert(offsetof(EscrowSaveData, amount) == 0x38, "EscrowSaveData");
static_assert(offsetof(EscrowSaveData, order_id) == 0x40, "EscrowSaveData");
static_assert(offsetof(EscrowSaveData, state) == 0x68, "EscrowSaveData");
static_assert(offsetof(EscrowSaveData, cb) == 0x70, "EscrowSaveData");

struct ConsumableSaveRecord {  // size 0x20; line 100
    ConsumableSaveRecord();
    OnlineService::GameAccountHandle account_id;  // 0x00
    uint64 transaction_id;  // 0x10
    OnlineServiceServer::ProtoMessageCallback cb;  // 0x18
};
static_assert(sizeof(ConsumableSaveRecord) == 0x20, "ConsumableSaveRecord");
static_assert(offsetof(ConsumableSaveRecord, transaction_id) == 0x10, "ConsumableSaveRecord");
static_assert(offsetof(ConsumableSaveRecord, cb) == 0x18, "ConsumableSaveRecord");

struct GameSaveData {  // size 0x38
    GameSaveData() : idSGame(SGAMEID_INVALID) {}  // line 170
    virtual ~GameSaveData() {}  // line 174; vtable slot 0
    static GameSaveData * PopulateData(SGame & tSGame);
    virtual void IssueSave(const OnlineServiceServer::GenericCallback &) = 0;  // vtable slot 2
    virtual void PerformPostSaveOperations(DbErrorCode) = 0;  // vtable slot 3
    SGameID idSGame;  // 0x08
    blz::set<unsigned int, blz::less<unsigned int>, blz::allocator<unsigned int> > accountsToUnlock;  // 0x10
protected:
    virtual bool PopulateDataFromGame(SGame &) = 0;  // vtable slot 4
};
static_assert(sizeof(GameSaveData) == 0x38, "GameSaveData");
static_assert(offsetof(GameSaveData, idSGame) == 0x8, "GameSaveData");
static_assert(offsetof(GameSaveData, accountsToUnlock) == 0x10, "GameSaveData");

struct GameSaveDataCampaignOrAdventureMode : public GameSaveData {  // size 0xc8
    GameSaveDataCampaignOrAdventureMode()  // line 195
        : eGamePartitionType(ACCOUNT_PARTITION_NORMAL)
        , tPlayerSaveDataVec()
        , tDirtyInventoryRecords()
    {
    }
    virtual ~GameSaveDataCampaignOrAdventureMode() {}  // line 201; vtable slot 0
    virtual void IssueSave(const OnlineServiceServer::GenericCallback & pfnCallback);  // vtable slot 2
    virtual void PerformPostSaveOperations(DbErrorCode dbErrCode);  // vtable slot 3
    D3AccountPartition eGamePartitionType;  // 0x38
    blz::vector<SHero::PlayerSaveData, blz::allocator<SHero::PlayerSaveData> > tPlayerSaveDataVec;  // 0x40
    DirtyInventoryRecordMap tDirtyInventoryRecords;  // 0x58
    MailRecordMap tDirtyMailRecords;  // 0x78
    blz::vector<EscrowSaveData, blz::allocator<EscrowSaveData> > tEscrowData;  // 0x98
    blz::vector<ConsumableSaveRecord, blz::allocator<ConsumableSaveRecord> > tConsumableSaveRecords;  // 0xb0
protected:
    virtual bool PopulateDataFromGame(SGame & tSGame);  // vtable slot 4
};
static_assert(sizeof(GameSaveDataCampaignOrAdventureMode) == 0xc8, "GameSaveDataCampaignOrAdventureMode");
static_assert(offsetof(GameSaveDataCampaignOrAdventureMode, eGamePartitionType) == 0x38, "GameSaveDataCampaignOrAdventureMode");
static_assert(offsetof(GameSaveDataCampaignOrAdventureMode, tPlayerSaveDataVec) == 0x40, "GameSaveDataCampaignOrAdventureMode");
static_assert(offsetof(GameSaveDataCampaignOrAdventureMode, tDirtyInventoryRecords) == 0x58, "GameSaveDataCampaignOrAdventureMode");
static_assert(offsetof(GameSaveDataCampaignOrAdventureMode, tDirtyMailRecords) == 0x78, "GameSaveDataCampaignOrAdventureMode");
static_assert(offsetof(GameSaveDataCampaignOrAdventureMode, tEscrowData) == 0x98, "GameSaveDataCampaignOrAdventureMode");
static_assert(offsetof(GameSaveDataCampaignOrAdventureMode, tConsumableSaveRecords) == 0xb0, "GameSaveDataCampaignOrAdventureMode");

struct GameSaveDataChallengeRifts : public GameSaveData {  // size 0x50
    GameSaveDataChallengeRifts() {}  // line 221
    virtual ~GameSaveDataChallengeRifts() {}  // line 224; vtable slot 0
    virtual void IssueSave(const OnlineServiceServer::GenericCallback & pfnCallback);  // vtable slot 2
    virtual void PerformPostSaveOperations(DbErrorCode);  // vtable slot 3
    blz::vector<SHero::ChallengeRiftPlayerSaveData, blz::allocator<SHero::ChallengeRiftPlayerSaveData> > tPlayerSaveDataVec;  // 0x38
protected:
    virtual bool PopulateDataFromGame(SGame & tSGame);  // vtable slot 4
};
static_assert(sizeof(GameSaveDataChallengeRifts) == 0x50, "GameSaveDataChallengeRifts");
static_assert(offsetof(GameSaveDataChallengeRifts, tPlayerSaveDataVec) == 0x38, "GameSaveDataChallengeRifts");

class CheckpointData;
void SPlayerSetCheckpoint(Player & tPlayer, const CheckpointData & tCheckpoint);  // SPlayer.cpp:10893

void SPlayerAddFollower(ActorCommonData & tACDPet, Player & tPlayer);
void SPlayerAddFollowerProxy(ActorCommonData & tACDPet, Player & tPlayer);

void SPlayerSpendParagonPoints(Player & tPlayer, GBID gbidParagonBonus, int32 nPoints);  // SPlayer.cpp:6685

void SPlayerQueueFloatingNumberHealing(ActorCommonData & tACDTarget, float flHealAmount);  // SPlayer.cpp:4690

// Declaration placement inferred from the owning API; DWARF definition: SPlayer.cpp:11097.
void SPlayerRecordSentSpeedValue(Player & tPlayer, ActorCommonData & tACD, float flValue);

// No header declarations recovered; owning-header placement inferred from SPlayer.cpp:8240 and :13633.
void SPlayerApplyLoadingBuff(ActorCommonData & tACD, ActorCommonData & tACDPlayer, BOOL bNewGame, BOOL bApplyInvulBuff);
void SPlayerRecalculateUpscalingLevels();
